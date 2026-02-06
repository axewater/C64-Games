/* ui_hub.c - Main menu and hub screens */

#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "forum.h"
#include "release.h"
#include "input.h"
#include <conio.h>

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
    ui_print_centered(11, "[1] HELP", COLOR_WHITE);
    ui_print_centered(12, "[2] NEW GAME", COLOR_WHITE);
    ui_print_centered(14, "[Q] QUIT", COLOR_WHITE);
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
