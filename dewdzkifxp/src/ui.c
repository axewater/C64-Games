#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "rank.h"
#include "forum.h"
#include "release.h"
#include "input.h"
#include "random.h"
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
    uint8_t colors[3];
    uint8_t y;
    uint8_t x;
    uint8_t ch;
    uint8_t color;

    colors[0] = COLOR_CYAN;
    colors[1] = COLOR_BLUE;
    colors[2] = COLOR_WHITE;

    for (y = 6; y < 15; y++) {
        for (x = 5; x < 35; x++) {
            if ((x + y + frame) % 3 == 0) {
                ch = matrix_chars[(x * y + frame) % 7];
                color = colors[(x + frame) % 3];
                screen_set_char(x, y, ch, color);
            }
        }
    }
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
    screen_print_number(29, 0, game_state.bandwidth, 2, COLOR_CYAN);
    ui_print_string(31, 0, "MB/S", COLOR_CYAN);

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

void ui_show_splash(void) {
    screen_clear();

    ui_print_centered(10, "DEWDZKI FXP", COLOR_CYAN);
    ui_print_centered(12, "WAREZ SCENE SIMULATOR", COLOR_WHITE);
    ui_print_centered(14, "PRESS ANY KEY", COLOR_GREEN);

    input_wait_key();
}

void ui_show_menu(void) {
    screen_clear();

    ui_print_centered(8, "MAIN MENU", COLOR_CYAN);
    ui_print_centered(11, "[1] NEW GAME", COLOR_WHITE);
    ui_print_centered(13, "[Q] QUIT", COLOR_WHITE);
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
    ui_draw_hline(0, 15, 40, '-', COLOR_BLUE);
    ui_print_centered(17, "[1] SCAN FOR FTPS", COLOR_WHITE);
    ui_print_centered(18, "[2] BROWSE TOPSITE", COLOR_WHITE);
    ui_print_centered(19, "[3] POST TO FORUM", COLOR_WHITE);
    ui_print_centered(20, "[Q] QUIT", COLOR_WHITE);
}

void ui_show_scan_anim(void) {
    uint8_t frame;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "SCANNING FOR PUBLIC FTPS", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    /* Animate for 60 frames */
    for (frame = 0; frame < 60; frame++) {
        ui_animate_matrix(frame);

        /* Simple delay */
        waitvsync();

        /* Allow skip with space */
        if (input_check_quit()) {
            break;
        }
    }
}

uint8_t ui_show_topsite(Release* releases, uint8_t count) {
    uint8_t i;
    uint8_t row;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "TOPSITE: ELITE.WAREZ.NL", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    ui_print_string(2, 5, "AVAILABLE RELEASES:", COLOR_WHITE);

    /* Show releases */
    row = 7;
    for (i = 0; i < count; i++) {
        screen_set_char(2, row, '[', COLOR_WHITE);
        screen_print_number(3, row, i + 1, 1, COLOR_YELLOW);
        screen_set_char(4, row, ']', COLOR_WHITE);
        screen_set_char(6, row, 160, COLOR_GREEN);  /* Block */
        ui_print_string(8, row, releases[i].name, COLOR_WHITE);

        row++;
        ui_print_string(8, row, "SIZE:", COLOR_GRAY2);
        screen_print_number(14, row, releases[i].size_mb, 4, COLOR_CYAN);
        ui_print_string(18, row, "MB", COLOR_CYAN);

        row += 2;
    }

    ui_draw_hline(0, row, 40, '-', COLOR_BLUE);
    ui_print_centered(row + 2, "SELECT RELEASE (1-3)", COLOR_WHITE);
    ui_print_centered(row + 3, "[Q] BACK TO HUB", COLOR_WHITE);

    return input_read_menu(count);
}

void ui_show_fxp_anim(Release* rel, uint8_t turns) {
    uint8_t frames;
    uint8_t i;
    uint8_t progress;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "FXP TRANSFER IN PROGRESS", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    ui_print_string(2, 6, "FILE:", COLOR_WHITE);
    ui_print_string(8, 6, rel->name, COLOR_YELLOW);

    ui_print_string(2, 7, "SIZE:", COLOR_WHITE);
    screen_print_number(8, 7, rel->size_mb, 4, COLOR_CYAN);
    ui_print_string(12, 7, "MB", COLOR_CYAN);

    frames = turns * 30;  /* 30 frames per turn */

    for (i = 0; i < frames; i++) {
        progress = (i * 100) / frames;

        ui_draw_progress_bar(5, 12, 30, progress);

        ui_print_string(5, 14, "SPEED:", COLOR_WHITE);
        screen_print_number(12, 14, game_state.bandwidth, 2, COLOR_GREEN);
        ui_print_string(14, 14, "MB/S", COLOR_WHITE);

        waitvsync();

        /* Allow skip */
        if (kbhit()) {
            cgetc();
            break;
        }
    }

    /* Show complete */
    ui_draw_progress_bar(5, 12, 30, 100);
    ui_print_centered(16, "TRANSFER COMPLETE", COLOR_GREEN);
    ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();
}

void ui_show_forum_post(Release* rel) {
    const char* forum_name;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    forum_name = forum_get_name(game_state.current_forum);
    ui_print_string(5, 3, "FORUM: ", COLOR_WHITE);
    ui_print_string(12, 3, forum_name, COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    ui_print_string(5, 6, "POSTING:", COLOR_WHITE);
    ui_print_string(5, 7, rel->name, COLOR_YELLOW);

    ui_print_centered(12, "[NEW RELEASE]", COLOR_GREEN);
    ui_print_centered(14, "[POST CREATED]", COLOR_GREEN);
    ui_print_centered(16, "+5 REP FROM POST", COLOR_GREEN);

    ui_print_centered(20, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();
}

void ui_show_rank_up(void) {
    const char* rank_name;

    screen_clear();

    ui_draw_hline(0, 8, 40, 160, COLOR_CYAN);
    ui_print_centered(10, "RANK UP!", COLOR_YELLOW);
    ui_draw_hline(0, 12, 40, 160, COLOR_CYAN);

    rank_name = rank_get_name(game_state.rank);
    ui_print_string(12, 14, "NEW RANK:", COLOR_WHITE);
    ui_print_string(22, 14, rank_name, COLOR_YELLOW);

    ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();
}

void ui_show_game_over(void) {
    screen_clear();

    ui_print_centered(8, "CONGRATULATIONS!", COLOR_GREEN);
    ui_print_centered(10, "YOU REACHED ELITE RANK!", COLOR_YELLOW);

    ui_print_string(10, 13, "FINAL REP:", COLOR_WHITE);
    screen_print_number(21, 13, game_state.reputation, 4, COLOR_YELLOW);

    ui_print_string(10, 14, "TURNS:", COLOR_WHITE);
    screen_print_number(21, 14, game_state.turn, 3, COLOR_CYAN);

    ui_print_centered(18, "YOU ARE NOW SCENE LEGEND!", COLOR_CYAN);

    ui_print_centered(22, "[Q] QUIT", COLOR_WHITE);
}
