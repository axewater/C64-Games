/* ui_hub.c - Main menu and hub screens */

#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "forum.h"
#include "release.h"
#include "input.h"
#include "sprite.h"
#include "sprite_data.h"
#include "sound.h"
#include <conio.h>
#include <c64.h>

/* Wait N frames using waitvsync */
static void splash_wait_frames(uint8_t n) {
    uint8_t i;
    for (i = 0; i < n; ++i) {
        waitvsync();
    }
}

/* Draw a full-width solid block bar */
static void splash_draw_bar(uint8_t y, uint8_t color) {
    ui_draw_hline(0, y, 40, 160, color);
}

/* Draw a centered text line with click sound and delay */
static void splash_draw_line(uint8_t y, const char* str, uint8_t color) {
    ui_print_centered(y, str, color);
    sound_play_click();
    splash_wait_frames(4);
}

void ui_show_splash(void) {
    uint8_t i;
    uint8_t blink_on;
    uint8_t blink_count;
    uint16_t spr_x;
    uint8_t spr_y;
    int8_t dx, dy;

    screen_clear();

    /* === Phase 1: Entrance — lines appear top-to-bottom === */

    /* Row 0: blue bar */
    splash_draw_bar(0, COLOR_BLUE);
    sound_play_click();
    splash_wait_frames(4);

    /* Row 1: light blue bar */
    splash_draw_bar(1, COLOR_LIGHTBLUE);
    sound_play_click();
    splash_wait_frames(4);

    /* Row 3: DEWDZKI title */
    splash_draw_line(3, "D E W D Z K I", COLOR_CYAN);

    /* Row 4: FXP subtitle */
    splash_draw_line(4, "F X P", COLOR_WHITE);

    /* Row 6: separator */
    ui_draw_hline(4, 6, 32, '-', COLOR_BLUE);
    sound_play_click();
    splash_wait_frames(4);

    /* Row 8: tagline */
    splash_draw_line(8, "WAREZ SCENE SIMULATOR", COLOR_LIGHTGREEN);

    /* Row 13: chapter */
    splash_draw_line(13, "CHAPTER ONE", COLOR_YELLOW);

    /* Row 15: separator */
    ui_draw_hline(4, 15, 32, '-', COLOR_BLUE);
    sound_play_click();
    splash_wait_frames(4);

    /* Row 17: production credit */
    splash_draw_line(17, "A DEWDZKI PRODUCTION", COLOR_LIGHTBLUE);

    /* Row 18: platform */
    splash_draw_line(18, "COMMODORE 64", COLOR_GRAY2);

    /* Row 20: separator */
    ui_draw_hline(4, 20, 32, '-', COLOR_BLUE);
    sound_play_click();
    splash_wait_frames(4);

    /* Row 22: press any key */
    ui_print_centered(22, "PRESS ANY KEY", COLOR_GREEN);
    sound_play_click();
    splash_wait_frames(4);

    /* Row 23-24: bottom bars */
    splash_draw_bar(23, COLOR_LIGHTBLUE);
    sound_play_click();
    splash_wait_frames(4);

    splash_draw_bar(24, COLOR_BLUE);
    sound_play_click();
    splash_wait_frames(4);

    /* === Phase 2: FTP sprite fade-in === */

    /* Load FTP sprite into cassette buffer at $0340 */
    sprite_load(SPRITE_FTP, sprite_ftp, SPRITE_DATA_BASE);

    /* Configure multicolor: MC0=GREEN (lights), sprite=LIGHT_BLUE, MC1=DARK_GRAY */
    sprite_set_multicolor_shared(COLOR_GREEN, COLOR_GRAY1);
    sprite_set_multicolor(SPRITE_FTP, 1);
    sprite_set_color(SPRITE_FTP, COLOR_LIGHTBLUE);
    sprite_set_position(SPRITE_FTP, 172, 138);

    /* Rapid blink: 3 on / 3 off x 8 */
    for (i = 0; i < 8; ++i) {
        sprite_enable(SPRITE_FTP, 1);
        splash_wait_frames(3);
        sprite_enable(SPRITE_FTP, 0);
        splash_wait_frames(3);
    }

    /* Medium blink: 6 on / 6 off x 4 */
    for (i = 0; i < 4; ++i) {
        sprite_enable(SPRITE_FTP, 1);
        splash_wait_frames(6);
        sprite_enable(SPRITE_FTP, 0);
        splash_wait_frames(6);
    }

    /* Slow blink: 12 on / 12 off x 2 */
    for (i = 0; i < 2; ++i) {
        sprite_enable(SPRITE_FTP, 1);
        splash_wait_frames(12);
        sprite_enable(SPRITE_FTP, 0);
        splash_wait_frames(12);
    }

    /* Solid on */
    sprite_enable(SPRITE_FTP, 1);

    /* === Phase 3: Idle loop — DVD bounce + blink until keypress === */
    blink_on = 1;
    blink_count = 0;
    spr_x = 172;
    spr_y = 138;
    dx = 1;
    dy = 1;

    while (!kbhit()) {
        waitvsync();

        /* DVD-logo bounce: move sprite and reverse on edges */
        spr_x += dx;
        spr_y += dy;

        /* Bounce off left/right edges (visible sprite area ~24-320) */
        if (spr_x <= 24 || spr_x >= 320) {
            dx = -dx;
            spr_x += dx;
        }
        /* Bounce off top/bottom edges (visible sprite area ~50-229) */
        if (spr_y <= 50 || spr_y >= 229) {
            dy = -dy;
            spr_y += dy;
        }

        sprite_set_position(SPRITE_FTP, spr_x, spr_y);

        /* "PRESS ANY KEY" blink: 25 frames on, 25 frames off */
        ++blink_count;
        if (blink_count >= 25) {
            blink_count = 0;
            blink_on ^= 1;
            if (blink_on) {
                ui_print_centered(22, "PRESS ANY KEY", COLOR_GREEN);
            } else {
                ui_print_centered(22, "             ", COLOR_BLACK);
            }
        }
    }

    /* Consume the keypress */
    cgetc();

    /* === Cleanup === */
    sprite_enable(SPRITE_FTP, 0);
    sound_silence();
}

void ui_show_menu(void) {
    screen_clear();

    ui_print_centered(8, "MAIN MENU", COLOR_CYAN);
    ui_print_centered(11, "[1] NEW GAME", COLOR_WHITE);
    ui_print_centered(12, "[2] HELP", COLOR_WHITE);
    ui_print_centered(13, "[3] PLAY CHAPTER 2", COLOR_YELLOW);
    ui_print_centered(15, "[Q] QUIT", COLOR_WHITE);
}

void ui_show_help(void) {
    screen_clear();

    /* Title */
    ui_print_centered(3, "GAME HELP", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, '-', COLOR_BLUE);

    /* Gameplay explanation */
    ui_print_string(2, 6, "SCAN FTPS:", COLOR_YELLOW);
    ui_print_string(14, 6, "DISCOVER WAREZ SITES", COLOR_WHITE);

    ui_print_string(2, 7, "BROWSE TOPSITES:", COLOR_YELLOW);
    ui_print_string(20, 7, "FIND RELEASES", COLOR_WHITE);

    ui_print_string(2, 8, "FXP:", COLOR_YELLOW);
    ui_print_string(7, 8, "TRANSFER FILES TO YOUR FTPS", COLOR_WHITE);

    ui_print_string(2, 9, "POST TO FORUMS:", COLOR_YELLOW);
    ui_print_string(19, 9, "EARN REPUTATION", COLOR_WHITE);

    ui_print_string(2, 10, "GOAL:", COLOR_YELLOW);
    ui_print_string(8, 10, "REACH ELITE RANK (9999 REP)", COLOR_WHITE);

    /* HUD explanation */
    ui_draw_hline(0, 12, 40, '-', COLOR_BLUE);
    ui_print_centered(13, "TOP STATS BAR", COLOR_CYAN);
    ui_draw_hline(0, 14, 40, '-', COLOR_BLUE);

    ui_print_string(2, 15, "RANK:", COLOR_YELLOW);
    ui_print_string(8, 15, "SCENE STATUS (LEECH-ELITE)", COLOR_WHITE);

    ui_print_string(2, 16, "REP:", COLOR_YELLOW);
    ui_print_string(7, 16, "REPUTATION POINTS (0-9999)", COLOR_WHITE);

    ui_print_string(2, 17, "BW:", COLOR_YELLOW);
    ui_print_string(6, 17, "BANDWIDTH IN KB/S", COLOR_WHITE);

    ui_print_string(2, 18, "ACT:", COLOR_YELLOW);
    ui_print_string(7, 18, "ACTIONS LEFT THIS SESSION", COLOR_WHITE);

    ui_print_string(2, 19, "SITES:", COLOR_YELLOW);
    ui_print_string(9, 19, "FTP SERVERS DISCOVERED", COLOR_WHITE);

    ui_print_string(2, 20, "POSTS:", COLOR_YELLOW);
    ui_print_string(9, 20, "ACTIVE FORUM RELEASES", COLOR_WHITE);

    /* Footer */
    ui_print_centered(22, "[SPACE] BACK TO MENU", COLOR_WHITE);

    input_wait_key();
}

void ui_show_hub(void) {
    uint8_t i;
    uint8_t count;
    ForumPost* post;
    Release* rel;
    uint8_t row;

    screen_clear();
    ui_render_hud();

    /* Title */
    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "ACTIVE RELEASES", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, '-', COLOR_BLUE);

    /* Show top 3 posts */
    count = 0;
    row = 5;
    for (i = 0; i < MAX_POSTS && count < 3; i++) {
        post = forum_get_post(i);
        if (post && post->active) {
            rel = release_get(post->release_id);
            if (rel && rel->active) {
                screen_print_number(0, row, count + 1, 1, COLOR_WHITE);
                ui_print_string(2, row, rel->name, COLOR_WHITE);
                ui_print_string(23, row, "DL:", COLOR_GRAY2);
                screen_print_number(26, row, post->downloads, 3, COLOR_YELLOW);
                ui_print_string(31, row, "REP:+", COLOR_GRAY2);
                screen_print_number(36, row, post->rep_earned / 10, 2, COLOR_GREEN);

                count++;
                row++;
            }
        }
    }

    /* Menu */
    ui_draw_hline(0, 13, 40, '-', COLOR_BLUE);
    ui_print_centered(14, "[1] SCAN FOR FTPS", COLOR_WHITE);
    ui_print_centered(15, "[2] BROWSE TOPSITE", COLOR_WHITE);
    ui_print_centered(16, "[3] POST TO FORUM", COLOR_WHITE);
    ui_print_centered(17, "[4] VIEW STATS", COLOR_WHITE);
    ui_print_centered(18, "[5] VIEW FTPS", COLOR_WHITE);
    ui_print_centered(19, "[6] HARDWARE SHOP", COLOR_WHITE);
    ui_print_centered(20, "[7] REPUTATION SHOP", COLOR_YELLOW);
    ui_print_centered(22, "[Q] QUIT", COLOR_WHITE);
}
