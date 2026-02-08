#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "input.h"
#include "sound.h"
#include "tools.h"
#include <conio.h>

void ui_show_splash(void) {
    screen_clear();

    ui_print_centered(3, "================================", COLOR_LIGHTBLUE);
    ui_print_centered(5, "R I S E   O F   D E W D Z K I", COLOR_CYAN);
    ui_print_centered(7, "DEWDZKI FXP-64 - CHAPTER 2", COLOR_GRAY2);
    ui_print_centered(9, "================================", COLOR_LIGHTBLUE);

    ui_print_centered(12, "BUILD YOUR NETWORK OF HACKED", COLOR_WHITE);
    ui_print_centered(13, "SERVERS. INSTALL SERVICES.", COLOR_WHITE);
    ui_print_centered(14, "CRACK CREDENTIALS. GROW YOUR", COLOR_WHITE);
    ui_print_centered(15, "NETWORK WHILE AVOIDING DETECTION.", COLOR_WHITE);

    ui_print_centered(18, "AXE IS COUNTING ON YOU.", COLOR_YELLOW);

    ui_print_centered(22, "PRESS ANY KEY", COLOR_GRAY2);
    input_wait_key();
}

void ui_show_menu(void) {
    screen_clear();

    ui_print_centered(3, "B O X   E M P I R E", COLOR_CYAN);
    ui_draw_hline(5, 5, 30, COLOR_LIGHTBLUE);

    ui_print_string(10, 8,  "1. NEW GAME", COLOR_WHITE);
    ui_print_string(10, 10, "2. HOW TO PLAY", COLOR_WHITE);
    ui_print_string(10, 12, "3. CHAPTER 3", COLOR_GRAY2);

    ui_print_centered(16, "Q. QUIT", COLOR_GRAY2);
}

void ui_show_help(void) {
    screen_clear();

    ui_print_centered(0, "HOW TO PLAY", COLOR_CYAN);
    ui_draw_hline(0, 1, 40, COLOR_GRAY2);

    ui_print_string(1, 3,  "EACH TURN YOU HAVE 3 ACTIONS.", COLOR_WHITE);
    ui_print_string(1, 4,  "SCAN IP RANGES TO FIND BOXES.", COLOR_LIGHTBLUE);
    ui_print_string(1, 5,  "INSTALL SERVICES ON YOUR BOXES.", COLOR_LIGHTBLUE);

    ui_print_string(1, 7,  "SERVICES:", COLOR_YELLOW);
    ui_print_string(1, 8,  "  FTP  - EARN REPUTATION", COLOR_GREEN);
    ui_print_string(1, 9,  "  SCAN - AUTO-SCAN EACH TURN", COLOR_GREEN);
    ui_print_string(1, 10, "  PROXY- UNLOCK HIDDEN RANGES", COLOR_GREEN);

    ui_print_string(1, 12, "BOXES ACCUMULATE RISK OVER TIME.", COLOR_WHITE);
    ui_print_string(1, 13, "HIGH RISK = SYSADMIN DISCOVERY!", COLOR_RED);
    ui_print_string(1, 14, "USE STEALTH UPGRADES TO SURVIVE.", COLOR_WHITE);

    ui_print_string(1, 16, "UNLOCK ALL 6 OF AXE'S TOOLS", COLOR_YELLOW);
    ui_print_string(1, 17, "TO RISE TO THE TOP.", COLOR_YELLOW);

    ui_print_string(1, 19, "SPEND REP IN THE SHOP FOR", COLOR_WHITE);
    ui_print_string(1, 20, "EXTRA ACTIONS AND UPGRADES.", COLOR_WHITE);

    ui_print_centered(23, "PRESS ANY KEY", COLOR_GRAY2);
    input_wait_key();
}

void ui_show_hub(void) {
    uint8_t y = 4;
    uint8_t has_lopht;
    uint8_t has_proxy;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== NETWORK HUB ==", COLOR_CYAN);

    has_lopht = tools_has(TOOL_LOPHTCRACK);
    has_proxy = tools_has(TOOL_PROXY);

    /* Always available (costs 1 action) */
    ui_print_string(3, y, "1. SCAN IP RANGE", COLOR_WHITE);
    ui_print_string(28, y, "(1 ACT)", COLOR_GRAY2);
    ++y;
    ++y;

    /* Free actions */
    ui_print_string(3, y, "2. MANAGE BOXES", COLOR_WHITE);
    ui_print_string(28, y, "(FREE)", COLOR_GRAY2);
    ++y;
    ++y;

    /* Credential dump (needs LophtCrack) */
    if (has_lopht) {
        ui_print_string(3, y, "3. DUMP CREDENTIALS", COLOR_WHITE);
        ui_print_string(28, y, "(1 ACT)", COLOR_GRAY2);
    } else {
        ui_print_string(3, y, "3. DUMP CREDENTIALS", COLOR_GRAY2);
        ui_print_string(28, y, "[LOCKED]", COLOR_GRAY1);
    }
    ++y;
    ++y;

    /* Proxy post (needs Proxy) */
    if (has_proxy) {
        ui_print_string(3, y, "4. POST PROXY", COLOR_WHITE);
        ui_print_string(28, y, "(FREE)", COLOR_GRAY2);
    } else {
        ui_print_string(3, y, "4. POST PROXY", COLOR_GRAY2);
        ui_print_string(28, y, "[LOCKED]", COLOR_GRAY1);
    }
    ++y;
    ++y;

    /* Always free */
    ui_print_string(3, y, "5. VIEW NETWORK STATS", COLOR_WHITE);
    ui_print_string(28, y, "(FREE)", COLOR_GRAY2);
    ++y;
    ++y;

    ui_print_string(3, y, "6. UPGRADE SHOP", COLOR_WHITE);
    ui_print_string(28, y, "(FREE)", COLOR_GRAY2);
    ++y;
    ++y;

    ui_print_string(3, y, "Q. QUIT TO MENU", COLOR_GRAY2);

    /* Actions remaining notice */
    if (game.actions_remaining == 0) {
        ui_print_centered(22, "NO ACTIONS LEFT - ENDING TURN...", COLOR_YELLOW);
    }
}

void ui_show_confirm_quit(void) {
    ui_clear_body();
    ui_render_hud();

    ui_print_centered(10, "QUIT TO MAIN MENU?", COLOR_YELLOW);
    ui_print_centered(12, "YOUR PROGRESS WILL BE LOST!", COLOR_RED);

    ui_print_centered(16, "Y. YES, QUIT", COLOR_WHITE);
    ui_print_centered(18, "N. NO, KEEP PLAYING", COLOR_GREEN);

    /* Wait for Y or N */
    while (1) {
        if (kbhit()) {
            uint8_t key = cgetc();
            if (key == 'y' || key == 'Y') {
                sound_play_back();
                game.state = STATE_MENU;
                return;
            }
            if (key == 'n' || key == 'N') {
                sound_play_click();
                game.state = STATE_PLAYING;
                return;
            }
        }
    }
}
