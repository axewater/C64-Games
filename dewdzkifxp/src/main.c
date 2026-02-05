#include <stdint.h>
#include "screen.h"
#include "input.h"
#include "random.h"
#include "gamestate.h"
#include "rank.h"
#include "ftp.h"
#include "topsite.h"
#include "release.h"
#include "forum.h"
#include "ui.h"

/* Temporary release buffer for topsite browsing */
Release temp_releases[3];
uint8_t selected_release_id;

void action_scan(void);
void action_browse_topsite(void);
void action_post_to_forum(void);
void action_hardware_shop(void);
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

            case STATE_HARDWARE_SHOP:
                action_hardware_shop();
                break;

            case STATE_RAID_ALERT:
                ui_show_raid_alert();
                game_state.state = STATE_PLAYING;
                break;

            case STATE_EVENT:
                ui_show_event();
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

            /* Check if raid occurred this turn (NEW) */
            if (game_state.last_raid_ftp_idx != 255) {
                game_state.state = STATE_RAID_ALERT;
            }

            /* Roll for random event (5% chance per turn) (NEW) */
            if (game_state.current_event == 0 && random_range(0, 100) < 5) {
                uint8_t event_type;
                uint8_t i;
                ForumPost* post;

                event_type = random_range(1, 11);  /* 1-10 */
                game_state.current_event = event_type;

                /* Apply event effects */
                if (event_type == 1) {
                    /* Elite noticed: +50-100 rep */
                    game_state.event_value = random_range(50, 101);
                    gamestate_add_reputation(game_state.event_value);
                } else if (event_type == 2) {
                    /* Viral release: double downloads on random post */
                    for (i = 0; i < MAX_POSTS; i++) {
                        post = forum_get_post(i);
                        if (post && post->active && !post->nuked) {
                            post->downloads *= 2;
                            game_state.event_value = i;  /* Store post index */
                            break;
                        }
                    }
                } else if (event_type == 3) {
                    /* Featured post: +25 rep */
                    game_state.event_value = 25;
                    gamestate_add_reputation(25);
                } else if (event_type == 4) {
                    /* Duping accusation: -20-40 rep */
                    game_state.event_value = random_range(20, 41);
                    if (game_state.reputation > game_state.event_value) {
                        game_state.reputation -= game_state.event_value;
                    } else {
                        game_state.reputation = 0;
                    }
                } else if (event_type == 5) {
                    /* Fake release: nuke random post */
                    for (i = 0; i < MAX_POSTS; i++) {
                        post = forum_get_post(i);
                        if (post && post->active && !post->nuked) {
                            post->nuked = 1;
                            game_state.event_value = i;  /* Store post index */
                            break;
                        }
                    }
                } else if (event_type == 6) {
                    /* Forum drama: -15 rep */
                    game_state.event_value = 15;
                    if (game_state.reputation > 15) {
                        game_state.reputation -= 15;
                    } else {
                        game_state.reputation = 0;
                    }
                } else if (event_type == 7) {
                    /* BW upgrade: +20 BW for 5 turns */
                    game_state.event_value = 5;  /* Duration in turns */
                    game_state.bandwidth += 20;
                } else if (event_type == 8) {
                    /* ISP throttle: -15 BW for 3 turns */
                    game_state.event_value = 3;  /* Duration in turns */
                    if (game_state.bandwidth > 15) {
                        game_state.bandwidth -= 15;
                    }
                } else if (event_type == 9) {
                    /* New FTP discovered */
                    ftp_scan();  /* Auto-discover FTP */
                } else if (event_type == 10) {
                    /* Topsite unlocked */
                    if (game_state.current_topsite < 2) {
                        game_state.current_topsite++;
                    }
                }

                /* Trigger event display */
                game_state.state = STATE_EVENT;
            }

            release_cleanup_unused();  /* Clean up unused releases */
            gamestate_check_rank_up();
        }
    }

    return 0;
}

void handle_playing_state(void) {
    uint8_t choice;
    uint8_t has_hardware;

    has_hardware = (game_state.hardware_tier < 255);

    ui_show_hub();
    choice = input_read_menu(6);

    if (choice == 0) {
        /* Scan for FTPs - requires hardware */
        if (!has_hardware) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "NO INTERNET CONNECTION!", COLOR_RED);
            ui_print_centered(12, "VISIT HARDWARE SHOP FIRST", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            return;
        }
        game_state.state = STATE_SCAN;
    } else if (choice == 1) {
        /* Browse topsites - requires hardware */
        if (!has_hardware) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "NO INTERNET CONNECTION!", COLOR_RED);
            ui_print_centered(12, "VISIT HARDWARE SHOP FIRST", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            return;
        }
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
    } else if (choice == 5) {
        /* Hardware shop */
        game_state.state = STATE_HARDWARE_SHOP;
    } else if (choice == 255) {
        /* Quit */
        game_state.state = STATE_GAME_OVER;
    }
}

void action_scan(void) {
    FTPServer* ftp;
    uint8_t num_found;
    uint8_t i;
    uint8_t start_idx;

    /* Check if player has actions remaining */
    if (!gamestate_can_use_action()) {
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "OUT OF ACTIONS!", COLOR_RED);
        ui_print_centered(12, "WAIT FOR NEW SESSION", COLOR_WHITE);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        game_state.state = STATE_PLAYING;
        return;
    }

    ui_show_scan_anim();

    /* Remember starting FTP count */
    start_idx = game_state.ftps_known_count;

    /* Scan for FTPs */
    num_found = ftp_scan();

    screen_clear();
    ui_render_hud();

    if (num_found > 0) {
        /* Success - show found FTP(s) */
        if (num_found == 1) {
            ui_print_centered(10, "FTP FOUND!", COLOR_GREEN);
        } else {
            ui_print_centered(10, "MULTIPLE FTPS FOUND!", COLOR_GREEN);
        }

        /* Show up to 3 FTP names */
        for (i = 0; i < num_found && i < 3; i++) {
            if (start_idx + i < game_state.ftps_known_count) {
                ftp = ftp_get(start_idx + i);
                if (ftp) {
                    ui_print_centered(12 + i, ftp->name, COLOR_CYAN);
                }
            }
        }

        /* Cost: 1 action, 1 turn */
        gamestate_use_action();
        gamestate_advance_turn(1);
    } else {
        /* Failure - lose 1 REP */
        ui_print_centered(10, "SCAN FAILED!", COLOR_RED);
        ui_print_centered(12, "NO FTP FOUND", COLOR_WHITE);

        if (game_state.reputation > 0) {
            game_state.reputation--;
            ui_print_centered(14, "-1 REP", COLOR_RED);
        }

        /* Cost: 1 action, 1 turn (same as success) */
        gamestate_use_action();
        gamestate_advance_turn(1);
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
    uint8_t ftp_index_map[MAX_FTP_SERVERS];
    uint8_t ftp_count;

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

        /* Check if player has actions remaining */
        if (!gamestate_can_use_action()) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "OUT OF ACTIONS!", COLOR_RED);
            ui_print_centered(12, "WAIT FOR NEW SESSION", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Build FTP index mapping BEFORE showing menu to avoid race condition */
        ftp_count = 0;
        for (i = 0; i < MAX_FTP_SERVERS; i++) {
            if (ftps[i].active) {
                ftp_index_map[ftp_count++] = i;
            }
        }

        choice = ui_show_ftp_select(&temp_releases[game_state.selected_release_idx]);

        if (choice == 255) {
            /* Cancelled - back to release browse */
            game_state.state = STATE_TOPSITE;
            return;
        }

        /* Use pre-built mapping (safe from race conditions) */
        if (choice >= ftp_count) {
            /* Invalid choice somehow */
            game_state.state = STATE_PLAYING;
            return;
        }
        ftp_idx = ftp_index_map[choice];

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

        /* Calculate transfer time - FXP uses MIN(topsite_bw, ftp_bw) */
        {
            uint16_t topsite_bw;
            uint16_t ftp_bw;
            uint16_t transfer_bw;

            topsite_bw = topsites[game_state.selected_topsite_idx].bandwidth;
            ftp_bw = ftps[ftp_idx].bandwidth;

            /* FXP speed limited by slower of the two servers */
            transfer_bw = (topsite_bw < ftp_bw) ? topsite_bw : ftp_bw;

            /* Calculate turns based on transfer speed */
            turns = (rel->size_mb / transfer_bw) + 1;
            if (turns > 10) turns = 10;  /* Cap at 10 turns */
        }

        /* Show FXP animation */
        ui_show_fxp_anim(rel, turns);

        /* Add release to FTP */
        if (!ftp_add_release(ftp_idx, selected_release_id)) {
            /* Failed to add (duplicate or FTP full) */
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "ERROR: RELEASE NOT ADDED!", COLOR_RED);
            ui_print_centered(12, "DUPLICATE OR FTP ISSUE", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Cost: 1 action, N turns */
        gamestate_use_action();
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

void action_hardware_shop(void) {
    uint8_t choice;
    const HardwareTier* tier;
    extern const HardwareTier hardware_tiers[6];

    /* Show hardware shop */
    choice = ui_show_hardware_shop();

    /* Check for cancel */
    if (choice == 255) {
        game_state.state = STATE_PLAYING;
        return;
    }

    /* Validate choice (should be 0-5) */
    if (choice >= 6) {
        game_state.state = STATE_PLAYING;
        return;
    }

    tier = &hardware_tiers[choice];

    /* Check if trying to buy same or lower tier (skip check if no hardware yet) */
    if (game_state.hardware_tier < 255 && choice <= game_state.hardware_tier) {
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "INVALID PURCHASE!", COLOR_RED);
        if (choice == game_state.hardware_tier) {
            ui_print_centered(12, "ALREADY OWNED", COLOR_WHITE);
        } else {
            ui_print_centered(12, "CANNOT DOWNGRADE", COLOR_WHITE);
        }
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        game_state.state = STATE_HARDWARE_SHOP;
        return;
    }

    /* Check if player has enough REP */
    if (game_state.reputation < tier->rep_cost) {
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "INSUFFICIENT REP!", COLOR_RED);
        ui_print_string(10, 12, "NEED:", COLOR_WHITE);
        screen_print_number(16, 12, tier->rep_cost, 3, COLOR_YELLOW);
        ui_print_string(10, 13, "HAVE:", COLOR_WHITE);
        screen_print_number(16, 13, game_state.reputation, 3, COLOR_YELLOW);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        game_state.state = STATE_HARDWARE_SHOP;
        return;
    }

    /* Purchase successful */
    game_state.reputation -= tier->rep_cost;
    game_state.hardware_tier = choice;
    game_state.bandwidth = tier->bandwidth;  /* Update bandwidth */

    screen_clear();
    ui_render_hud();
    ui_print_centered(10, "PURCHASE SUCCESSFUL!", COLOR_GREEN);
    ui_print_string(8, 12, "NEW HARDWARE:", COLOR_WHITE);
    ui_print_string(22, 12, tier->name, COLOR_CYAN);
    ui_print_string(8, 13, "BANDWIDTH:", COLOR_WHITE);
    screen_print_number(19, 13, tier->bandwidth, 3, COLOR_CYAN);
    ui_print_string(22, 13, "KB/S", COLOR_WHITE);
    ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();

    game_state.state = STATE_PLAYING;
}
