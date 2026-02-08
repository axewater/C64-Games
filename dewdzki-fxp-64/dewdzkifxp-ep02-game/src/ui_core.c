#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "tools.h"
#include "range.h"
#include <conio.h>

void ui_init(void) {
    screen_init();
}

void ui_render_hud(void) {
    uint8_t i, bits, x;

    /* Row 0: BOX:XX/24  FTP:X  SCN:X  PRX:X  TURN:XXX */
    ui_print_string(0, 0, "BOX:", COLOR_LIGHTBLUE);
    screen_print_number(4, 0, game.active_box_count, 2, COLOR_WHITE);
    ui_print_string(6, 0, "/24", COLOR_GRAY2);

    ui_print_string(10, 0, "FTP:", COLOR_LIGHTBLUE);
    screen_print_number(14, 0, game.ftp_count, 1, COLOR_WHITE);

    ui_print_string(16, 0, "SCN:", COLOR_LIGHTBLUE);
    screen_print_number(20, 0, game.scanner_count, 1, COLOR_WHITE);

    ui_print_string(22, 0, "PRX:", COLOR_LIGHTBLUE);
    screen_print_number(26, 0, game.proxy_count, 1, COLOR_WHITE);

    ui_print_string(28, 0, "TURN:", COLOR_LIGHTBLUE);
    screen_print_number(33, 0, game.turn, 3, COLOR_WHITE);

    /* Row 1: REP:XXXX  CREDS:X  TOOLS:[######]  ACT:X */
    ui_print_string(0, 1, "REP:", COLOR_LIGHTBLUE);
    screen_print_number(4, 1, game.reputation, 4, COLOR_YELLOW);

    ui_print_string(10, 1, "CREDS:", COLOR_LIGHTBLUE);
    screen_print_number(16, 1, game.creds_cracked_total, 1, COLOR_WHITE);

    ui_print_string(19, 1, "TOOLS:[", COLOR_LIGHTBLUE);
    bits = game.tools_unlocked;
    x = 26;
    for (i = 0; i < TOOL_COUNT; ++i) {
        if (bits & (1 << i)) {
            screen_set_char(x, 1, '#', COLOR_GREEN);
        } else {
            screen_set_char(x, 1, '-', COLOR_GRAY2);
        }
        ++x;
    }
    screen_set_char(x, 1, ']', COLOR_LIGHTBLUE);

    ui_print_string(34, 1, "ACT:", COLOR_LIGHTBLUE);
    screen_print_number(38, 1, game.actions_remaining, 1, COLOR_WHITE);

    /* Separator line */
    ui_draw_hline(0, 2, 40, COLOR_GRAY2);
}

void ui_print_string(uint8_t x, uint8_t y, const char* str, uint8_t color) {
    while (*str) {
        screen_set_char(x, y, *str, color);
        ++str;
        ++x;
        if (x >= SCREEN_WIDTH) break;
    }
}

void ui_print_centered(uint8_t y, const char* str, uint8_t color) {
    uint8_t len = 0;
    const char* p = str;
    uint8_t x;

    while (*p) { ++len; ++p; }
    x = (SCREEN_WIDTH - len) / 2;
    ui_print_string(x, y, str, color);
}

void ui_draw_hline(uint8_t x, uint8_t y, uint8_t len, uint8_t color) {
    uint8_t i;
    for (i = 0; i < len; ++i) {
        screen_set_char(x + i, y, 0x40, color); /* horizontal line char */
    }
}

void ui_draw_progress_bar(uint8_t x, uint8_t y, uint8_t current, uint8_t max, uint8_t color) {
    uint8_t i;
    uint8_t filled;

    if (max == 0) return;
    filled = (current * 10) / max;
    if (filled > 10) filled = 10;

    screen_set_char(x, y, '[', color);
    for (i = 0; i < 10; ++i) {
        if (i < filled) {
            screen_set_char(x + 1 + i, y, '#', color);
        } else {
            screen_set_char(x + 1 + i, y, '.', color);
        }
    }
    screen_set_char(x + 11, y, ']', color);
}

void ui_print_ip(uint8_t x, uint8_t y, uint8_t range_id, uint8_t o3, uint8_t o4, uint8_t color) {
    /* Print IP: prefix.o3.o4 without sprintf */
    uint8_t cx = x;

    /* First octet */
    screen_print_number(cx, y, ranges[range_id].octet1, 3, color);
    cx += 3;
    screen_set_char(cx, y, '.', color);
    ++cx;

    /* Second octet */
    screen_print_number(cx, y, ranges[range_id].octet2, 3, color);
    cx += 3;
    screen_set_char(cx, y, '.', color);
    ++cx;

    /* Third octet */
    screen_print_number(cx, y, o3, 3, color);
    cx += 3;
    screen_set_char(cx, y, '.', color);
    ++cx;

    /* Fourth octet */
    screen_print_number(cx, y, o4, 3, color);
}

void ui_clear_body(void) {
    uint16_t offset;
    uint16_t end;
    /* Clear rows 3-24 (body area below HUD) */
    offset = screen_offset(0, 3);
    end = SCREEN_WIDTH * SCREEN_HEIGHT;
    while (offset < end) {
        SCREEN_MEM[offset] = ' ';
        COLOR_MEM[offset] = COLOR_WHITE;
        ++offset;
    }
}
