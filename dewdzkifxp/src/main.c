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

            case STATE_TOPSITE_LIST:
            case STATE_TOPSITE:
            case STATE_FTP_SELECT:
                /* All topsite/FXP steps handled by same function */
                action_browse_topsite();
                break;

            case STATE_FXP:
                /* Step 4: FXP animation - handled in action_browse_topsite */
                game_state.state = STATE_PLAYING;
                break;

            case STATE_RELEASE_SELECT:
            case STATE_FORUM:
                /* All forum posting steps handled by same function */
                action_post_to_forum();
                break;

            case STATE_RANK_UP:
                ui_show_rank_up();
                game_state.state = STATE_PLAYING;
                break;

            case STATE_VIEW_STATS:
                ui_show_stats();
                game_state.state = STATE_PLAYING;
                break;

            case STATE_VIEW_FTPS:
                ui_show_ftps();
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
            ftp_update_all_risks();
            ftp_process_raids();
            gamestate_check_rank_up();
        }
    }

    return 0;
}

void handle_playing_state(void) {
    uint8_t choice;

    ui_show_hub();
    choice = input_read_menu(5);

    if (choice == 0) {
        /* Scan for FTPs */
        game_state.state = STATE_SCAN;
    } else if (choice == 1) {
        /* Browse topsites (multi-step) */
        game_state.state = STATE_TOPSITE_LIST;
    } else if (choice == 2) {
        /* Post to forum (multi-step) */
        game_state.state = STATE_RELEASE_SELECT;
    } else if (choice == 3) {
        /* View stats */
        game_state.state = STATE_VIEW_STATS;
    } else if (choice == 4) {
        /* View FTPs */
        game_state.state = STATE_VIEW_FTPS;
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
    uint8_t ftp_idx;
    Release* rel;

    /* Multi-step flow based on current state */
    if (game_state.state == STATE_TOPSITE_LIST) {
        /* Step 1: Select topsite */
        choice = ui_show_topsite_list();

        if (choice == 255) {
            /* Cancelled */
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Store selected topsite and generate releases */
        game_state.selected_topsite_idx = choice;

        for (i = 0; i < 3; i++) {
            release_generate(&temp_releases[i], choice);
        }

        /* Move to release browse */
        game_state.state = STATE_TOPSITE;

    } else if (game_state.state == STATE_TOPSITE) {
        /* Step 2: Browse releases */
        choice = ui_show_topsite(game_state.selected_topsite_idx, temp_releases, 3);

        if (choice == 255) {
            /* Cancelled - back to topsite list */
            game_state.state = STATE_TOPSITE_LIST;
            return;
        }

        if (choice >= 3) {
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Check if any FTPs discovered */
        if (game_state.ftps_known_count == 0) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "NO FTPS DISCOVERED!", COLOR_RED);
            ui_print_centered(12, "SCAN FOR FTPS FIRST", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Store selected release */
        game_state.selected_release_idx = choice;

        /* Move to FTP selection */
        game_state.state = STATE_FTP_SELECT;

    } else if (game_state.state == STATE_FTP_SELECT) {
        /* Step 3: Select FTP for transfer */
        choice = ui_show_ftp_select(&temp_releases[game_state.selected_release_idx]);

        if (choice == 255) {
            /* Cancelled - back to release browse */
            game_state.state = STATE_TOPSITE;
            return;
        }

        /* Find actual FTP index from menu choice */
        ftp_idx = 0;
        for (i = 0; i < MAX_FTP_SERVERS; i++) {
            if (ftps[i].active) {
                if (choice == 0) {
                    ftp_idx = i;
                    break;
                }
                choice--;
            }
        }

        /* Check if FTP is still active (not raided) */
        if (!ftps[ftp_idx].active) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "FTP WAS RAIDED!", COLOR_RED);
            ui_print_centered(12, "CHOOSE ANOTHER FTP", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_FTP_SELECT;
            return;
        }

        /* Check if FTP has space */
        if (!ftp_has_space(ftp_idx)) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "FTP IS FULL!", COLOR_RED);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_FTP_SELECT;
            return;
        }

        /* Add release to global pool if needed */
        selected_release_id = release_add(&temp_releases[game_state.selected_release_idx]);

        /* Check if release pool is full */
        if (selected_release_id == 255) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "RELEASE POOL FULL!", COLOR_RED);
            ui_print_centered(12, "MAX CAPACITY REACHED", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_FTP_SELECT;
            return;
        }

        /* Get the release from global pool */
        rel = release_get(selected_release_id);

        /* Safety check: verify release was retrieved */
        if (!rel || !rel->active) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "ERROR: RELEASE NOT FOUND!", COLOR_RED);
            ui_print_centered(12, "TRY AGAIN", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_FTP_SELECT;
            return;
        }

        /* Calculate transfer time */
        turns = (rel->size_mb / game_state.bandwidth) / 20;
        if (turns < 1) turns = 1;
        if (turns > 3) turns = 3;

        /* Show FXP animation */
        ui_show_fxp_anim(rel, turns);

        /* Add release to FTP */
        ftp_add_release(ftp_idx, selected_release_id);

        /* Cost: 1 action, N turns */
        if (game_state.actions_remaining > 0) {
            game_state.actions_remaining--;
        }
        gamestate_advance_turn(turns);

        /* Move to FXP state (will return to hub) */
        game_state.state = STATE_FXP;
    }
}

void action_post_to_forum(void) {
    uint8_t choice;
    uint8_t ftp_idx;
    uint8_t release_idx;
    Release* rel;
    uint8_t i;
    uint8_t has_releases;

    /* Multi-step flow based on current state */
    if (game_state.state == STATE_RELEASE_SELECT) {
        /* Step 1: Check if any FTPs have releases */
        has_releases = 0;
        for (i = 0; i < MAX_FTP_SERVERS; i++) {
            if (ftps[i].active && ftps[i].release_count > 0) {
                has_releases = 1;
                break;
            }
        }

        if (!has_releases) {
            /* No releases on any FTP */
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "NO RELEASES ON FTPS!", COLOR_RED);
            ui_print_centered(12, "FXP WAREZ FIRST", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Show release selection */
        choice = ui_show_release_select(&ftp_idx, &release_idx);

        if (choice == 255) {
            /* Cancelled */
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Store selections */
        game_state.selected_ftp_idx = ftp_idx;
        game_state.selected_release_idx = release_idx;

        /* Get release */
        rel = release_get(release_idx);

        if (rel && rel->active) {
            /* Check if already posted */
            if (forum_has_post(release_idx, ftp_idx)) {
                screen_clear();
                ui_render_hud();
                ui_print_centered(10, "ALREADY POSTED!", COLOR_RED);
                ui_print_centered(12, "CHOOSE DIFFERENT RELEASE", COLOR_WHITE);
                ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
                input_wait_key();
                game_state.state = STATE_RELEASE_SELECT;
                return;
            }

            /* Create forum post with FTP tracking */
            forum_create_post(release_idx, ftp_idx, game_state.current_forum);

            /* Show confirmation */
            ui_show_forum_post(rel, ftp_idx);

            /* Cost: 0 actions, 1 turn */
            gamestate_advance_turn(1);
        }

        /* Move to forum state (will return to hub) */
        game_state.state = STATE_FORUM;
    } else if (game_state.state == STATE_FORUM) {
        /* Step 2: Return to hub after posting */
        game_state.state = STATE_PLAYING;
    }
}
