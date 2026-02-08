#include <stdint.h>
#include "screen.h"
#include "input.h"
#include "sound.h"
#include "random.h"
#include "gamestate.h"
#include "box.h"
#include "range.h"
#include "tools.h"
#include "credential.h"
#include "actions.h"
#include "ui.h"
#include "chainload.h"

/* Forward declarations */
void process_turn(void);

int main(void) {
    uint8_t running;

    /* Initialize all systems */
    ui_init();
    input_init();
    sound_init();
    random_init();
    gamestate_init();

    running = 1;

    while (running) {
        switch (game.state) {
            case STATE_SPLASH:
                ui_show_splash();
                game.state = STATE_MENU;
                break;

            case STATE_MENU:
                ui_show_menu();
                {
                    uint8_t choice = input_read_menu(3);
                    if (choice == 0) {
                        /* New Game */
                        gamestate_start_game();
                        box_init();
                        range_init();
                        tools_init();
                        credential_init();
                    } else if (choice == 1) {
                        game.state = STATE_HELP;
                    } else if (choice == 2) {
                        /* Chapter 3 - chainload to intro */
                        chainload("ch03-intro");
                        running = 0;  /* Exit if chainload fails */
                    } else {
                        running = 0;
                    }
                }
                break;

            case STATE_HELP:
                ui_show_help();
                game.state = STATE_MENU;
                break;

            case STATE_PLAYING:
                handle_playing_state();
                break;

            case STATE_SCAN_RANGE:
                ui_show_scan_range();
                break;

            case STATE_SCAN_ANIM:
                ui_show_scan_anim();
                break;

            case STATE_SCAN_RESULT:
                ui_show_scan_result();
                /* Check for tool unlocks after scan */
                {
                    uint8_t new_tool = tools_check_milestones();
                    if (new_tool) {
                        game.pending_tool = new_tool;
                        game.state = STATE_TOOL_UNLOCK;
                    } else {
                        game.state = STATE_PLAYING;
                    }
                }
                break;

            case STATE_BOX_LIST:
                ui_show_box_list();
                break;

            case STATE_BOX_DETAIL:
                ui_show_box_detail();
                break;

            case STATE_INSTALL_SVC:
                ui_show_install_service();
                break;

            case STATE_CRED_DUMP:
                ui_show_cred_dump();
                break;

            case STATE_CRED_STATUS:
                ui_show_cred_status();
                game.state = STATE_PLAYING;
                break;

            case STATE_PROXY_POST:
                ui_show_proxy_post();
                game.state = STATE_PLAYING;
                break;

            case STATE_TOOL_UNLOCK:
                ui_show_tool_unlock();
                game.state = STATE_PLAYING;
                break;

            case STATE_DISCOVERY:
                ui_show_discovery();
                game.state = STATE_PLAYING;
                break;

            case STATE_EVENT:
                ui_show_event();
                game.state = STATE_PLAYING;
                break;

            case STATE_STATS:
                ui_show_stats();
                game.state = STATE_PLAYING;
                break;

            case STATE_SHOP:
                ui_show_shop();
                break;

            case STATE_GAME_WON:
                ui_show_game_won();
                game.state = STATE_MENU;
                break;

            case STATE_CONFIRM_QUIT:
                ui_show_confirm_quit();
                break;

            case STATE_QUIT:
                running = 0;
                break;
        }

        /* Check for end-of-turn processing */
        if (game.state == STATE_PLAYING && game.actions_remaining == 0) {
            process_turn();
        }
    }

    return 0;
}

void process_turn(void) {
    uint8_t i;
    uint8_t new_tool;
    uint8_t risk_add;
    uint8_t hit_rate;

    /* Reset per-turn tracking */
    game.last_discovered_box = 255;
    game.discovery_count = 0;

    /* 1. Risk accumulation on all active boxes */
    for (i = 0; i < MAX_BOXES; ++i) {
        if (boxes[i].active != BOX_ACTIVE) continue;

        risk_add = ranges[boxes[i].range_id].risk_per_turn;

        /* Services increase risk */
        if (boxes[i].services & SVC_FTP) risk_add += 1;
        if (boxes[i].services & SVC_SCANNER) risk_add += 1;
        if (boxes[i].services & SVC_PROXY) risk_add += 2;

        /* Stealth reduces risk growth */
        if (risk_add > boxes[i].stealth * 2) {
            risk_add -= boxes[i].stealth * 2;
        } else {
            risk_add = 0;
        }

        /* HackDB halves risk growth */
        if (tools_has(TOOL_HACKDB)) {
            risk_add = (risk_add + 1) / 2;
        }

        boxes[i].risk += risk_add;
        if (boxes[i].risk > 100) boxes[i].risk = 100;

        ++boxes[i].days_alive;
    }

    /* 2. Discovery checks */
    for (i = 0; i < MAX_BOXES; ++i) {
        if (boxes[i].active != BOX_ACTIVE) continue;

        if (random_range(0, 100) < boxes[i].risk) {
            /* Sysadmin found this box */
            if (game.last_discovered_box == 255) {
                game.last_discovered_box = i;
            }
            box_destroy(i);
            ++game.discovery_count;
        }
    }

    /* Show discovery screen if any were found */
    if (game.discovery_count > 0) {
        game.state = STATE_DISCOVERY;
        /* Don't advance turn yet - will come back */
        gamestate_end_turn();
        return;
    }

    /* 3. Scanner auto-scan: each scanner box tries to find a new box */
    for (i = 0; i < MAX_BOXES; ++i) {
        if (boxes[i].active != BOX_ACTIVE) continue;
        if (!(boxes[i].services & SVC_SCANNER)) continue;

        hit_rate = range_get_hit_rate(boxes[i].range_id);
        if (random_range(0, 100) < hit_rate) {
            box_create(boxes[i].range_id);
        }
    }

    /* 4. Credential jobs tick */
    credential_tick();

    /* 5. FTP boxes generate reputation */
    for (i = 0; i < MAX_BOXES; ++i) {
        if (boxes[i].active != BOX_ACTIVE) continue;
        if (!(boxes[i].services & SVC_FTP)) continue;

        gamestate_add_reputation(boxes[i].bandwidth / 10);
    }

    /* 6. Tool milestone checks */
    new_tool = tools_check_milestones();
    if (new_tool) {
        game.pending_tool = new_tool;
        game.state = STATE_TOOL_UNLOCK;
        gamestate_end_turn();
        return;
    }

    /* 7. Win check */
    if (game.tools_unlocked == TOOL_ALL) {
        game.state = STATE_GAME_WON;
        return;
    }

    /* 8. Random IRC event (5% chance) */
    if (game.current_event == 0 && random_range(0, 100) < 5) {
        game.current_event = random_range(1, 9); /* 1-8 */
        game.state = STATE_EVENT;
        gamestate_end_turn();
        return;
    }

    /* 9. Tick down active event */
    if (game.event_turns_left > 0) {
        --game.event_turns_left;
        if (game.event_turns_left == 0) {
            game.current_event = 0;
        }
    }

    /* 10. Range unlocks */
    range_check_unlocks();

    /* Advance turn */
    gamestate_end_turn();
}
