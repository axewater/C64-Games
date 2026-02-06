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
#include "actions.h"

/* Forward declarations */
void process_events(void);

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
                {
                    uint8_t choice = input_read_menu(2);
                    if (choice == 0) {
                        game_state.state = STATE_HELP;
                    } else if (choice == 1) {
                        gamestate_start_game();
                    } else {
                        running = 0;
                    }
                }
                break;

            case STATE_HELP:
                ui_show_help();
                game_state.state = STATE_MENU;
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

            case STATE_MOVE_RELEASES:
                action_move_releases();
                break;

            case STATE_REP_SHOP:
                action_rep_shop();
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
            process_events();
        }
    }

    return 0;
}

void process_events(void) {
    uint8_t event_type;
    uint8_t i;
    ForumPost* post;

    /* Update all systems */
    forum_update_all_posts();
    ftp_update_all_risks();
    ftp_process_raids();

    /* Check if raid occurred this turn */
    if (game_state.last_raid_ftp_idx != 255) {
        game_state.state = STATE_RAID_ALERT;
        return;
    }

    /* Roll for random event (5% chance per turn) */
    if (game_state.current_event == 0 && random_range(0, 100) < 5) {
        event_type = random_range(1, 14);  /* 1-13 */

        /* Check event requirements before triggering */
        if (event_type == 1 || event_type == 3) {
            /* Events 1,3: Elite noticed/Featured post - need posts */
            if (game_state.posts_active_count == 0) {
                return;  /* Skip event this turn */
            }
        } else if (event_type == 4 || event_type == 6) {
            /* Events 4,6: Duping/Drama - need reputation */
            if (game_state.reputation < 100) {
                return;  /* Skip event this turn */
            }
        } else if (event_type == 9) {
            /* Event 9: FTP tip - need hardware */
            if (game_state.hardware_owned == 0) {
                return;  /* Skip event this turn */
            }
        } else if (event_type == 10) {
            /* Event 10: Topsite hint - need FTPs + rep */
            if (game_state.ftps_known_count < 3 || game_state.reputation < 200) {
                return;  /* Skip event this turn */
            }
        }

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
        } else if (event_type == 11) {
            /* Friend lends T1 line for 10 turns */
            game_state.event_value = 10;
            gamestate_apply_temp_equipment(TEMP_EQUIP_T1_LOAN, 10);
        } else if (event_type == 12) {
            /* Cable modem access for 5 turns */
            game_state.event_value = 5;
            gamestate_apply_temp_equipment(TEMP_EQUIP_CABLE, 5);
        } else if (event_type == 13) {
            /* Corporate T3 line for 3 turns */
            game_state.event_value = 3;
            gamestate_apply_temp_equipment(TEMP_EQUIP_T3, 3);
        }

        /* Trigger event display */
        game_state.state = STATE_EVENT;
        return;
    }

    release_cleanup_unused();  /* Clean up unused releases */
    gamestate_check_rank_up();
}
