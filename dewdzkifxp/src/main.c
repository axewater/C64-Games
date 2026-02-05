#include <stdint.h>
#include "screen.h"
#include "input.h"
#include "random.h"
#include "gamestate.h"
#include "rank.h"
#include "ftp.h"
#include "release.h"
#include "forum.h"
#include "ui.h"

/* Temporary release buffer for topsite browsing */
Release temp_releases[3];
uint8_t selected_release_id;

void action_scan(void);
void action_browse_topsite(void);
void action_post_to_forum(void);
void handle_playing_state(void);

int main(void) {
    uint8_t running;

    /* Initialize all systems */
    ui_init();
    input_init();
    random_init();
    gamestate_init();
    ftp_init();
    release_init();
    forum_init();

    running = 1;

    /* Main game loop */
    while (running) {
        switch (game_state.state) {
            case STATE_SPLASH:
                ui_show_splash();
                game_state.state = STATE_MENU;
                break;

            case STATE_MENU:
                ui_show_menu();
                if (input_read_menu(1) == 0) {
                    gamestate_start_game();
                } else {
                    running = 0;
                }
                break;

            case STATE_PLAYING:
                handle_playing_state();
                break;

            case STATE_SCAN:
                action_scan();
                break;

            case STATE_TOPSITE:
                action_browse_topsite();
                break;

            case STATE_FXP:
                /* FXP animation already shown in action_browse_topsite */
                game_state.state = STATE_PLAYING;
                break;

            case STATE_FORUM:
                action_post_to_forum();
                break;

            case STATE_RANK_UP:
                ui_show_rank_up();
                game_state.state = STATE_PLAYING;
                break;

            case STATE_GAME_OVER:
                ui_show_game_over();
                if (input_check_quit()) {
                    running = 0;
                }
                break;
        }

        /* Update stats every turn */
        if (game_state.state == STATE_PLAYING) {
            forum_update_all_posts();
            gamestate_check_rank_up();
        }
    }

    return 0;
}

void handle_playing_state(void) {
    uint8_t choice;

    ui_show_hub();
    choice = input_read_menu(3);

    if (choice == 0) {
        /* Scan for FTPs */
        game_state.state = STATE_SCAN;
    } else if (choice == 1) {
        /* Browse topsite */
        game_state.state = STATE_TOPSITE;
    } else if (choice == 2) {
        /* Post to forum */
        game_state.state = STATE_FORUM;
    } else if (choice == 255) {
        /* Quit */
        game_state.state = STATE_GAME_OVER;
    }
}

void action_scan(void) {
    FTPServer* ftp;
    uint8_t success;

    ui_show_scan_anim();

    success = ftp_scan();

    screen_clear();
    ui_render_hud();

    if (success) {
        /* Show found FTP */
        ftp = ftp_get(game_state.ftps_known_count - 1);
        ui_print_centered(10, "FTP FOUND!", COLOR_GREEN);
        if (ftp) {
            ui_print_centered(12, ftp->name, COLOR_CYAN);
        }

        /* Cost: 1 action, 1 turn */
        if (game_state.actions_remaining > 0) {
            game_state.actions_remaining--;
        }
        gamestate_advance_turn(1);
    } else {
        ui_print_centered(10, "SCAN FAILED", COLOR_RED);
        ui_print_centered(12, "NO FTP FOUND", COLOR_WHITE);
    }

    ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();

    game_state.state = STATE_PLAYING;
}

void action_browse_topsite(void) {
    uint8_t i;
    uint8_t choice;
    uint8_t turns;

    /* Generate 3 random releases */
    for (i = 0; i < 3; i++) {
        release_generate(&temp_releases[i], game_state.current_topsite);
    }

    /* Show topsite and get selection */
    choice = ui_show_topsite(temp_releases, 3);

    if (choice < 3) {
        /* Valid selection - start FXP transfer */
        /* Calculate transfer time (1-3 turns) */
        turns = (temp_releases[choice].size_mb / game_state.bandwidth) / 20;
        if (turns < 1) {
            turns = 1;
        }
        if (turns > 3) {
            turns = 3;
        }

        /* Show FXP animation */
        ui_show_fxp_anim(&temp_releases[choice], turns);

        /* Add to inventory */
        selected_release_id = release_add(&temp_releases[choice]);

        /* Cost: 1 action, N turns */
        if (game_state.actions_remaining > 0) {
            game_state.actions_remaining--;
        }
        gamestate_advance_turn(turns);
    }

    game_state.state = STATE_PLAYING;
}

void action_post_to_forum(void) {
    uint8_t count;
    Release* rel;

    count = release_count_active();

    if (count == 0) {
        /* No releases to post */
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "NO RELEASES TO POST!", COLOR_RED);
        ui_print_centered(12, "GET WAREZ FROM TOPSITE", COLOR_WHITE);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        game_state.state = STATE_PLAYING;
        return;
    }

    /* Use last added release */
    rel = release_get(selected_release_id);

    if (rel && rel->active) {
        /* Create forum post */
        forum_create_post(selected_release_id, game_state.current_forum);

        /* Show confirmation */
        ui_show_forum_post(rel);

        /* Cost: 0 actions, 1 turn */
        gamestate_advance_turn(1);
    }

    game_state.state = STATE_PLAYING;
}
