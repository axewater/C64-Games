#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "box.h"
#include "range.h"
#include "tools.h"
#include "credential.h"
#include "input.h"
#include <conio.h>

void ui_show_stats(void) {
    uint8_t y;
    uint8_t i;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== NETWORK STATS ==", COLOR_CYAN);

    y = 5;

    /* Box counts */
    ui_print_string(2, y, "ACTIVE BOXES:", COLOR_LIGHTBLUE);
    screen_print_number(18, y, game.active_box_count, 2, COLOR_WHITE);
    ui_print_string(20, y, "/", COLOR_GRAY2);
    screen_print_number(21, y, MAX_BOXES, 2, COLOR_GRAY2);
    ++y;

    ui_print_string(2, y, "TOTAL EVER:", COLOR_LIGHTBLUE);
    screen_print_number(18, y, game.total_boxes_ever, 3, COLOR_WHITE);
    ++y;
    ++y;

    /* Services */
    ui_print_string(2, y, "FTP SERVERS:", COLOR_GREEN);
    screen_print_number(18, y, game.ftp_count, 2, COLOR_WHITE);
    ++y;

    ui_print_string(2, y, "SCANNERS:", COLOR_CYAN);
    screen_print_number(18, y, game.scanner_count, 2, COLOR_WHITE);
    ++y;

    ui_print_string(2, y, "PROXIES:", COLOR_YELLOW);
    screen_print_number(18, y, game.proxy_count, 2, COLOR_WHITE);
    ++y;
    ++y;

    /* Range status */
    ui_print_string(2, y, "RANGES:", COLOR_LIGHTBLUE);
    ++y;
    for (i = 0; i < MAX_RANGES; ++i) {
        if (range_unlocked[i]) {
            ui_print_string(4, y, ranges[i].name, COLOR_WHITE);
            if (range_has_creds[i]) {
                ui_print_string(14, y, "[CREDS]", COLOR_GREEN);
            }
        } else {
            ui_print_string(4, y, "???", COLOR_GRAY1);
        }
        ++y;
    }
    ++y;

    /* Tool checklist */
    ui_print_string(2, y, "TOOLS:", COLOR_LIGHTBLUE);
    ++y;
    for (i = 0; i < TOOL_COUNT; ++i) {
        if (game.tools_unlocked & (1 << i)) {
            screen_set_char(3, y, '*', COLOR_GREEN);
            ui_print_string(5, y, tool_names[i], COLOR_GREEN);
        } else {
            screen_set_char(3, y, '-', COLOR_GRAY1);
            ui_print_string(5, y, tool_names[i], COLOR_GRAY1);
        }
        ++y;
        if (y >= 24) break;
    }

    ui_print_centered(24, "PRESS ANY KEY", COLOR_GRAY2);
    input_wait_key();
}
