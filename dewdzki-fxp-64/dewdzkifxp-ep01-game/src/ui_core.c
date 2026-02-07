/* ui_core.c - Core UI functions and utilities */

#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "rank.h"
#include "ftp.h"
#include "sound.h"
#include <conio.h>
#include <string.h>

/* Matrix animation characters */
const uint8_t matrix_chars[] = {'/', '\\', '|', '-', '+', '=', '#'};

void ui_init(void) {
    screen_init();
}

void ui_print_string(uint8_t x, uint8_t y, const char* str, uint8_t color) {
    uint16_t offset;
    uint8_t i;

    offset = y * 40 + x;
    i = 0;

    while (str[i] != '\0' && x + i < 40) {
        SCREEN_MEM[offset + i] = str[i];
        COLOR_MEM[offset + i] = color;
        i++;
    }
}

void ui_print_centered(uint8_t y, const char* str, uint8_t color) {
    uint8_t len;
    uint8_t x;

    len = strlen(str);
    if (len > 40) {
        len = 40;
    }

    x = (40 - len) / 2;
    ui_print_string(x, y, str, color);
}

void ui_draw_hline(uint8_t x, uint8_t y, uint8_t width, uint8_t ch, uint8_t color) {
    uint8_t i;

    for (i = 0; i < width && x + i < 40; i++) {
        screen_set_char(x + i, y, ch, color);
    }
}

void ui_draw_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t percent) {
    uint8_t filled;
    uint8_t i;
    uint8_t ch;
    uint8_t color;

    filled = (width * percent) / 100;

    screen_set_char(x, y, '[', COLOR_WHITE);

    for (i = 0; i < width; i++) {
        ch = (i < filled) ? 160 : 32;  /* Block or space */
        color = (i < filled) ? COLOR_GREEN : COLOR_GRAY2;
        screen_set_char(x + 1 + i, y, ch, color);
    }

    screen_set_char(x + width + 1, y, ']', COLOR_WHITE);
}

void ui_animate_matrix(uint8_t frame) {
    /* Radar sweep animation - sweeps in a circle like a radar scanner */
    uint8_t angle;
    uint8_t prev_angle;
    uint8_t cx, cy;  /* Center point */
    uint8_t radius;
    int8_t dx, dy;
    uint8_t x, y;

    /* Center of scan area */
    cx = 20;
    cy = 10;

    /* Current angle (0-15 for 16 directions, wrapping for multiple sweeps) */
    angle = frame % 16;
    prev_angle = (frame > 0) ? ((frame - 1) % 16) : 15;

    /* Play radar sweep sound that matches the animation */
    sound_play_radar_sweep(angle);

    /* Clear previous sweep line */
    for (radius = 1; radius < 12; radius++) {
        /* Calculate direction based on previous angle (16 directions) */
        if (prev_angle == 0) { dx = 0; dy = -1; }        /* N */
        else if (prev_angle == 1) { dx = 1; dy = -1; }   /* NE */
        else if (prev_angle == 2) { dx = 1; dy = -1; }   /* NE */
        else if (prev_angle == 3) { dx = 1; dy = 0; }    /* E */
        else if (prev_angle == 4) { dx = 1; dy = 0; }    /* E */
        else if (prev_angle == 5) { dx = 1; dy = 1; }    /* SE */
        else if (prev_angle == 6) { dx = 1; dy = 1; }    /* SE */
        else if (prev_angle == 7) { dx = 0; dy = 1; }    /* S */
        else if (prev_angle == 8) { dx = 0; dy = 1; }    /* S */
        else if (prev_angle == 9) { dx = -1; dy = 1; }   /* SW */
        else if (prev_angle == 10) { dx = -1; dy = 1; }  /* SW */
        else if (prev_angle == 11) { dx = -1; dy = 0; }  /* W */
        else if (prev_angle == 12) { dx = -1; dy = 0; }  /* W */
        else if (prev_angle == 13) { dx = -1; dy = -1; } /* NW */
        else if (prev_angle == 14) { dx = -1; dy = -1; } /* NW */
        else { dx = 0; dy = -1; }                        /* N */

        x = cx + (dx * radius) / 2;
        y = cy + (dy * radius) / 3;

        if (x >= 5 && x < 35 && y >= 6 && y < 15) {
            screen_set_char(x, y, ' ', COLOR_BLACK);
        }
    }

    /* Draw new sweep line in current direction */
    for (radius = 1; radius < 12; radius++) {
        /* Calculate direction based on current angle */
        if (angle == 0) { dx = 0; dy = -1; }        /* N */
        else if (angle == 1) { dx = 1; dy = -1; }   /* NE */
        else if (angle == 2) { dx = 1; dy = -1; }   /* NE */
        else if (angle == 3) { dx = 1; dy = 0; }    /* E */
        else if (angle == 4) { dx = 1; dy = 0; }    /* E */
        else if (angle == 5) { dx = 1; dy = 1; }    /* SE */
        else if (angle == 6) { dx = 1; dy = 1; }    /* SE */
        else if (angle == 7) { dx = 0; dy = 1; }    /* S */
        else if (angle == 8) { dx = 0; dy = 1; }    /* S */
        else if (angle == 9) { dx = -1; dy = 1; }   /* SW */
        else if (angle == 10) { dx = -1; dy = 1; }  /* SW */
        else if (angle == 11) { dx = -1; dy = 0; }  /* W */
        else if (angle == 12) { dx = -1; dy = 0; }  /* W */
        else if (angle == 13) { dx = -1; dy = -1; } /* NW */
        else if (angle == 14) { dx = -1; dy = -1; } /* NW */
        else { dx = 0; dy = -1; }                   /* N */

        x = cx + (dx * radius) / 2;
        y = cy + (dy * radius) / 3;

        if (x >= 5 && x < 35 && y >= 6 && y < 15) {
            /* Bright at the tip, dimmer further back */
            if (radius > 8) {
                screen_set_char(x, y, 160, COLOR_CYAN);  /* Solid block */
            } else if (radius > 4) {
                screen_set_char(x, y, '+', COLOR_CYAN);
            } else {
                screen_set_char(x, y, '.', COLOR_BLUE);
            }
        }
    }

    /* Draw center dot */
    screen_set_char(cx, cy, 'O', COLOR_GREEN);
}

void ui_render_hud(void) {
    const char* rank_name;

    /* Row 0: RANK:LEECH    REP:0023    BW:45MB/s */
    ui_print_string(0, 0, "RANK:", COLOR_WHITE);
    rank_name = rank_get_name(game_state.rank);
    ui_print_string(5, 0, rank_name, COLOR_YELLOW);

    ui_print_string(15, 0, "REP:", COLOR_WHITE);
    screen_print_number(19, 0, game_state.reputation, 4, COLOR_YELLOW);

    ui_print_string(26, 0, "BW:", COLOR_WHITE);
    screen_print_number(29, 0, game_state.bandwidth, 3, COLOR_CYAN);
    ui_print_string(32, 0, "KB/S", COLOR_CYAN);

    /* Show temp equipment indicator (NEW) */
    if (game_state.temp_equipment_type != TEMP_EQUIP_NONE) {
        ui_print_string(36, 0, "[T]", COLOR_YELLOW);
    }

    /* Row 1: ACT:3  SITES:2  POSTS:5   TURN:012 */
    ui_print_string(0, 1, "ACT:", COLOR_WHITE);
    screen_print_number(4, 1, game_state.actions_remaining, 1, COLOR_GREEN);

    ui_print_string(7, 1, "SITES:", COLOR_WHITE);
    screen_print_number(13, 1, game_state.ftps_known_count, 1, COLOR_CYAN);

    ui_print_string(16, 1, "POSTS:", COLOR_WHITE);
    screen_print_number(22, 1, game_state.posts_active_count, 1, COLOR_CYAN);

    ui_print_string(26, 1, "TURN:", COLOR_WHITE);
    screen_print_number(31, 1, game_state.turn, 3, COLOR_WHITE);
}
