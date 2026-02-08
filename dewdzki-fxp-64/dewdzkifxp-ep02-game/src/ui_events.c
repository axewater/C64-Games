#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "box.h"
#include "range.h"
#include "tools.h"
#include "credential.h"
#include "random.h"
#include "input.h"
#include "sound.h"
#include <conio.h>

/* Tool unlock descriptions */
static const char* tool_desc[TOOL_COUNT] = {
    "SCAN IP RANGES FOR BOXES",
    "INSTALL FTP ON YOUR BOXES",
    "DUMP AND CRACK CREDENTIALS",
    "AUTO-SCAN FROM YOUR BOXES",
    "INSTALL PROXIES, NEW RANGES",
    "AUTO-STEALTH, HALVED RISK!"
};

/* Event titles */
static const char* event_titles[8] = {
    "AXE IMPROVES TOOL",
    "GOLDEN RANGE FOUND",
    "SYSADMIN CONFERENCE",
    "CREDENTIAL LEAK!",
    "ISP CRACKDOWN!",
    "SCENE KUDOS!",
    "DEFAULT PASSWORD FOUND",
    "MASS DISCOVERY!"
};

/* Wait helper */
static void wait_frames(uint8_t count) {
    uint8_t i;
    for (i = 0; i < count; ++i) {
        waitvsync();
    }
}

void ui_show_tool_unlock(void) {
    uint8_t tool_bit = game.pending_tool;
    uint8_t tool_idx;
    uint8_t i;

    /* Find tool index from bit */
    tool_idx = 0;
    for (i = 0; i < TOOL_COUNT; ++i) {
        if (tool_bit == (1 << i)) {
            tool_idx = i;
            break;
        }
    }

    screen_clear();

    /* Flash effect */
    for (i = 0; i < 3; ++i) {
        bordercolor(COLOR_WHITE);
        wait_frames(4);
        bordercolor(COLOR_BLUE);
        wait_frames(4);
    }

    ui_print_centered(4, "================================", COLOR_YELLOW);
    ui_print_centered(6, "AXE HAS A NEW TOOL FOR YOU!", COLOR_CYAN);
    ui_print_centered(8, "================================", COLOR_YELLOW);

    ui_print_centered(11, tool_names[tool_idx], COLOR_GREEN);

    ui_print_centered(14, tool_desc[tool_idx], COLOR_WHITE);

    /* Show tool progress bar */
    ui_print_string(5, 17, "TOOLS: [", COLOR_LIGHTBLUE);
    for (i = 0; i < TOOL_COUNT; ++i) {
        if (game.tools_unlocked & (1 << i)) {
            screen_set_char(13 + i, 17, '#', COLOR_GREEN);
        } else {
            screen_set_char(13 + i, 17, '-', COLOR_GRAY2);
        }
    }
    screen_set_char(19, 17, ']', COLOR_LIGHTBLUE);

    screen_print_number(21, 17, tools_count_unlocked(), 1, COLOR_WHITE);
    ui_print_string(22, 17, "/6", COLOR_GRAY2);

    ui_print_centered(22, "PRESS ANY KEY", COLOR_GRAY2);

    game.pending_tool = 0;
    input_wait_key();
}

void ui_show_discovery(void) {
    uint8_t y;
    uint8_t first = game.last_discovered_box;
    uint8_t count = game.discovery_count;

    screen_clear();

    /* Red border flash */
    bordercolor(COLOR_RED);

    ui_print_centered(3, "!! SYSADMIN ALERT !!", COLOR_RED);
    ui_draw_hline(5, 5, 30, COLOR_RED);

    y = 7;
    if (first != 255 && boxes[first].active == BOX_DISCOVERED) {
        ui_print_string(2, y, "BOX DISCOVERED:", COLOR_YELLOW);
        ++y;
        ui_print_ip(4, y, boxes[first].range_id,
                    boxes[first].octet3, boxes[first].octet4, COLOR_RED);
        ++y;
        ++y;
    }

    if (count == 1) {
        ui_print_string(2, y, "A SYSADMIN FOUND YOUR BOX!", COLOR_WHITE);
    } else {
        screen_print_number(2, y, count, 1, COLOR_RED);
        ui_print_string(4, y, "BOXES WERE DISCOVERED!", COLOR_WHITE);
    }
    ++y;
    ++y;

    ui_print_string(2, y, "ACTIVE BOXES: ", COLOR_GRAY2);
    screen_print_number(16, y, game.active_box_count, 2, COLOR_WHITE);
    ++y;

    ui_print_string(2, y, "USE STEALTH UPGRADES TO", COLOR_GRAY2);
    ++y;
    ui_print_string(2, y, "REDUCE DISCOVERY RISK.", COLOR_GRAY2);

    ui_print_centered(22, "PRESS ANY KEY", COLOR_GRAY2);

    sound_play_back();
    input_wait_key();
    bordercolor(COLOR_BLUE);
}

void ui_show_event(void) {
    uint8_t evt = game.current_event;
    uint8_t y;
    uint8_t i;

    if (evt == 0 || evt > 8) {
        game.current_event = 0;
        return;
    }

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== IRC EVENT ==", COLOR_CYAN);

    ui_print_string(2, 5, "<AXE> ", COLOR_GREEN);
    ui_print_string(8, 5, event_titles[evt - 1], COLOR_YELLOW);

    y = 8;
    switch (evt) {
        case 1: /* AXE improves tool */
            ui_print_string(2, y, "AXE TWEAKED THE SCAN TOOL.", COLOR_WHITE);
            ++y;
            ui_print_string(2, y, "+5% HIT RATE FOR 5 TURNS.", COLOR_GREEN);
            game.event_value = 5;
            game.event_turns_left = 5;
            break;

        case 2: /* Golden range */
            /* Pick a random unlocked range */
            game.event_range = 0;
            for (i = 0; i < MAX_RANGES; ++i) {
                if (range_unlocked[i]) game.event_range = i;
            }
            ui_print_string(2, y, "FOUND GOLDEN RANGE:", COLOR_WHITE);
            ++y;
            ui_print_string(4, y, ranges[game.event_range].name, COLOR_YELLOW);
            ++y;
            ui_print_string(2, y, "2X HIT RATE FOR 5 TURNS!", COLOR_GREEN);
            game.event_value = 2;
            game.event_turns_left = 5;
            break;

        case 3: /* Sysadmin conference */
            ui_print_string(2, y, "SYSADMIN CONFERENCE THIS WEEK!", COLOR_RED);
            ++y;
            ui_print_string(2, y, "2X DISCOVERY RISK, 3 TURNS.", COLOR_YELLOW);
            game.event_value = 2;
            game.event_turns_left = 3;
            break;

        case 4: /* Credential leak */
            /* Give creds for random unlocked range */
            game.event_range = 0;
            for (i = 0; i < MAX_RANGES; ++i) {
                if (range_unlocked[i] && !range_has_creds[i]) {
                    game.event_range = i;
                    range_has_creds[i] = 1;
                    ++game.creds_cracked_total;
                    break;
                }
            }
            ui_print_string(2, y, "PASSWORD LIST LEAKED!", COLOR_WHITE);
            ++y;
            ui_print_string(2, y, "FREE CREDS FOR ", COLOR_GREEN);
            ui_print_string(17, y, ranges[game.event_range].name, COLOR_YELLOW);
            game.event_turns_left = 0;
            break;

        case 5: /* ISP crackdown */
            /* Lose 1 random box */
            for (i = 0; i < MAX_BOXES; ++i) {
                if (boxes[i].active == BOX_ACTIVE) {
                    box_destroy(i);
                    ui_print_string(2, y, "ISP SHUT DOWN A BOX!", COLOR_RED);
                    ++y;
                    ui_print_ip(4, y, boxes[i].range_id,
                               boxes[i].octet3, boxes[i].octet4, COLOR_GRAY2);
                    break;
                }
            }
            game.event_turns_left = 0;
            break;

        case 6: /* Scene kudos */
            ui_print_string(2, y, "YOUR NETWORK GOT NOTICED!", COLOR_WHITE);
            ++y;
            ui_print_string(2, y, "+50 REPUTATION!", COLOR_GREEN);
            gamestate_add_reputation(50);
            game.event_turns_left = 0;
            break;

        case 7: /* Default password */
            ui_print_string(2, y, "NEW DEFAULT PASSWORD FOUND!", COLOR_WHITE);
            ++y;
            ui_print_string(2, y, "NEXT SCAN: 2+ HITS GUARANTEED.", COLOR_GREEN);
            game.event_value = 1; /* Flag for scan bonus */
            game.event_turns_left = 1;
            break;

        case 8: /* Mass discovery */
            if (game.active_box_count >= 10) {
                /* Lose 2 random boxes */
                {
                    uint8_t lost = 0;
                    for (i = 0; i < MAX_BOXES && lost < 2; ++i) {
                        if (boxes[i].active == BOX_ACTIVE) {
                            box_destroy(i);
                            ++lost;
                        }
                    }
                }
                ui_print_string(2, y, "COORDINATED TAKEDOWN!", COLOR_RED);
                ++y;
                ui_print_string(2, y, "2 BOXES LOST!", COLOR_YELLOW);
            } else {
                ui_print_string(2, y, "RUMOR OF CRACKDOWN...", COLOR_WHITE);
                ++y;
                ui_print_string(2, y, "BUT YOU'RE TOO SMALL TO NOTICE.", COLOR_GRAY2);
            }
            game.event_turns_left = 0;
            break;
    }

    ui_print_centered(20, "PRESS ANY KEY", COLOR_GRAY2);
    sound_play_click();
    input_wait_key();

    /* Clear event if no duration */
    if (game.event_turns_left == 0) {
        game.current_event = 0;
    }
}

void ui_show_cred_dump(void) {
    uint8_t i;
    uint8_t y;
    uint8_t num_options = 0;
    uint8_t option_range[MAX_RANGES];
    uint8_t choice;
    uint8_t active_jobs;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== LOPHTCRACK ==", COLOR_CYAN);

    /* Show active jobs first */
    active_jobs = credential_count_active();
    if (active_jobs > 0) {
        ui_print_string(2, 5, "ACTIVE JOBS:", COLOR_YELLOW);
        y = 6;
        for (i = 0; i < MAX_CRED_JOBS; ++i) {
            if (cred_jobs[i].active) {
                ui_print_string(4, y, ranges[cred_jobs[i].range_id].name, COLOR_LIGHTBLUE);
                ui_print_string(15, y, "- ", COLOR_GRAY2);
                screen_print_number(17, y, cred_jobs[i].turns_left, 1, COLOR_WHITE);
                ui_print_string(19, y, "TURNS LEFT", COLOR_GRAY2);
                ++y;
            }
        }
        ++y;
    } else {
        y = 5;
    }

    ui_print_string(2, y, "DUMP CREDENTIALS FROM:", COLOR_WHITE);
    ++y;
    ++y;

    /* Show unlocked ranges without creds */
    for (i = 0; i < MAX_RANGES; ++i) {
        if (!range_unlocked[i]) continue;
        if (range_has_creds[i]) {
            ui_print_string(4, y, ranges[i].name, COLOR_GRAY2);
            ui_print_string(14, y, "[CRACKED]", COLOR_GREEN);
            ++y;
            continue;
        }

        /* Check if already cracking */
        {
            uint8_t already = 0;
            uint8_t j;
            for (j = 0; j < MAX_CRED_JOBS; ++j) {
                if (cred_jobs[j].active && cred_jobs[j].range_id == i) {
                    already = 1;
                    break;
                }
            }
            if (already) {
                ui_print_string(4, y, ranges[i].name, COLOR_GRAY2);
                ui_print_string(14, y, "[CRACKING...]", COLOR_YELLOW);
                ++y;
                continue;
            }
        }

        ++num_options;
        option_range[num_options - 1] = i;
        screen_set_char(2, y, '0' + num_options, COLOR_WHITE);
        screen_set_char(3, y, '.', COLOR_WHITE);
        ui_print_string(5, y, ranges[i].name, COLOR_LIGHTBLUE);
        ui_print_string(14, y, "HIT: ", COLOR_GRAY2);
        screen_print_number(19, y, ranges[i].hit_rate, 2, COLOR_WHITE);
        ui_print_string(21, y, "% -> ", COLOR_GRAY2);
        {
            uint8_t boosted = ranges[i].hit_rate * 3;
            if (boosted > 99) boosted = 99;
            screen_print_number(26, y, boosted, 2, COLOR_GREEN);
        }
        screen_set_char(28, y, '%', COLOR_GREEN);
        ++y;
    }

    ++y;
    ui_print_string(2, y, "Q. BACK", COLOR_GRAY2);

    if (num_options == 0 && active_jobs == 0) {
        ui_print_centered(y + 2, "ALL RANGES CRACKED!", COLOR_GREEN);
        input_wait_key();
        game.state = STATE_PLAYING;
        return;
    }

    if (num_options == 0) {
        input_wait_key();
        game.state = STATE_PLAYING;
        return;
    }

    choice = input_read_menu(num_options);
    if (choice == 255) {
        game.state = STATE_PLAYING;
        return;
    }

    if (choice < num_options) {
        if (credential_start(option_range[choice])) {
            gamestate_use_action();
            ui_print_centered(22, "CRACKING STARTED!", COLOR_GREEN);
            sound_play_click();
        } else {
            ui_print_centered(22, "FAILED TO START JOB", COLOR_RED);
            sound_play_back();
        }
        input_wait_key();
    }

    game.state = STATE_PLAYING;
}

void ui_show_cred_status(void) {
    uint8_t i;
    uint8_t y = 5;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== CREDENTIAL STATUS ==", COLOR_CYAN);

    /* Active jobs */
    for (i = 0; i < MAX_CRED_JOBS; ++i) {
        if (cred_jobs[i].active) {
            ui_print_string(2, y, ranges[cred_jobs[i].range_id].name, COLOR_LIGHTBLUE);
            ui_print_string(14, y, "CRACKING... ", COLOR_YELLOW);
            screen_print_number(26, y, cred_jobs[i].turns_left, 1, COLOR_WHITE);
            ui_print_string(28, y, " TURNS", COLOR_GRAY2);
            ++y;
        }
    }

    /* Cracked ranges */
    ++y;
    ui_print_string(2, y, "CRACKED:", COLOR_GREEN);
    ++y;
    for (i = 0; i < MAX_RANGES; ++i) {
        if (range_has_creds[i]) {
            ui_print_string(4, y, ranges[i].name, COLOR_GREEN);
            ui_print_string(14, y, "3X HIT RATE", COLOR_WHITE);
            ++y;
        }
    }

    ui_print_centered(22, "PRESS ANY KEY", COLOR_GRAY2);
    input_wait_key();
}

void ui_show_proxy_post(void) {
    uint8_t i;
    uint8_t y = 5;
    uint8_t proxy_count = 0;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== POST PROXY ==", COLOR_CYAN);

    ui_print_string(2, y, "YOUR PROXY BOXES:", COLOR_YELLOW);
    ++y;

    for (i = 0; i < MAX_BOXES; ++i) {
        if (boxes[i].active != BOX_ACTIVE) continue;
        if (!(boxes[i].services & SVC_PROXY)) continue;

        ui_print_ip(4, y, boxes[i].range_id, boxes[i].octet3, boxes[i].octet4, COLOR_LIGHTBLUE);
        ui_print_string(20, y, "BW:", COLOR_GRAY2);
        screen_print_number(23, y, boxes[i].bandwidth, 3, COLOR_WHITE);
        ++proxy_count;
        ++y;
    }

    if (proxy_count == 0) {
        ui_print_centered(10, "NO PROXY BOXES YET.", COLOR_GRAY2);
    } else {
        ++y;
        /* Posting proxies to boards earns rep */
        {
            uint16_t rep_gain = (uint16_t)proxy_count * 10;
            ui_print_string(2, y, "POSTED TO BOARDS: +", COLOR_WHITE);
            screen_print_number(21, y, rep_gain, 3, COLOR_GREEN);
            ui_print_string(24, y, " REP", COLOR_GREEN);
            gamestate_add_reputation(rep_gain);
        }
    }

    ui_print_centered(22, "PRESS ANY KEY", COLOR_GRAY2);
    input_wait_key();
}

void ui_show_game_won(void) {
    uint8_t i;

    screen_clear();

    /* Victory flash */
    for (i = 0; i < 5; ++i) {
        bordercolor(COLOR_GREEN);
        waitvsync();
        waitvsync();
        bordercolor(COLOR_CYAN);
        waitvsync();
        waitvsync();
    }

    ui_print_centered(3, "================================", COLOR_GREEN);
    ui_print_centered(5, "Y O U   W I N !", COLOR_CYAN);
    ui_print_centered(7, "================================", COLOR_GREEN);

    ui_print_centered(10, "ALL 6 OF AXE'S TOOLS COMPLETE!", COLOR_YELLOW);
    ui_print_centered(11, "HACKDB IS NOW ACTIVE.", COLOR_WHITE);

    ui_print_centered(14, "DEWDZKI'S NETWORK IS UNSTOPPABLE.", COLOR_GREEN);

    ui_print_string(4, 17, "FINAL STATS:", COLOR_LIGHTBLUE);
    ui_print_string(4, 18, "TURNS: ", COLOR_GRAY2);
    screen_print_number(11, 18, game.turn, 4, COLOR_WHITE);
    ui_print_string(4, 19, "BOXES EVER: ", COLOR_GRAY2);
    screen_print_number(16, 19, game.total_boxes_ever, 3, COLOR_WHITE);
    ui_print_string(4, 20, "REP: ", COLOR_GRAY2);
    screen_print_number(9, 20, game.reputation, 4, COLOR_YELLOW);

    ui_print_centered(23, "PRESS ANY KEY", COLOR_GRAY2);

    bordercolor(COLOR_BLUE);
    input_wait_key();
}
