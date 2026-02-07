/* ui_forums.c - Forum posting and stats screens */

#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "forum.h"
#include "release.h"
#include "ftp.h"
#include "input.h"
#include <conio.h>

void ui_show_forum_post(Release* rel, uint8_t ftp_idx) {
    const char* forum_name;
    FTPServer* ftp;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    forum_name = forum_get_name(game_state.current_forum);
    ui_print_string(5, 3, "FORUM: ", COLOR_WHITE);
    ui_print_string(12, 3, forum_name, COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    ui_print_string(5, 6, "POSTING:", COLOR_WHITE);
    ui_print_string(5, 7, rel->name, COLOR_YELLOW);

    /* Show FTP source (NEW) */
    ftp = ftp_get(ftp_idx);
    if (ftp) {
        ui_print_string(5, 9, "FROM FTP:", COLOR_GRAY2);
        ui_print_string(15, 9, ftp->name, COLOR_CYAN);
    }

    ui_print_centered(12, "[NEW RELEASE]", COLOR_GREEN);
    ui_print_centered(14, "[POST CREATED]", COLOR_GREEN);
    ui_print_centered(16, "+5 REP FROM POST", COLOR_GREEN);

    ui_print_centered(20, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();
}

void ui_show_stats(void) {
    uint8_t i;
    uint8_t row;
    uint8_t count;
    ForumPost* post;
    Release* rel;
    FTPServer* ftp;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "ACTIVE FORUM POSTS", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, '-', COLOR_BLUE);

    row = 6;
    count = 0;

    /* Show all active posts with details */
    for (i = 0; i < MAX_POSTS; i++) {
        post = forum_get_post(i);
        if (post && post->active) {
            rel = release_get(post->release_id);
            ftp = ftp_get(post->ftp_id);

            if (rel && rel->active && ftp && row < 22) {
                /* Release name (truncated if needed) */
                ui_print_string(1, row, rel->name, COLOR_WHITE);

                /* FTP name (right side) */
                if (post->nuked) {
                    ui_print_string(22, row, "[NUKED]", COLOR_RED);
                } else {
                    ui_print_string(22, row, ftp->name, COLOR_CYAN);
                }
                row++;

                /* Stats line */
                ui_print_string(2, row, "DL:", COLOR_GRAY2);
                screen_print_number(5, row, post->downloads, 3, COLOR_YELLOW);

                ui_print_string(10, row, "REP:", COLOR_GRAY2);
                screen_print_number(14, row, post->rep_earned, 3, COLOR_GREEN);

                ui_print_string(19, row, "AGE:", COLOR_GRAY2);
                screen_print_number(23, row, post->age, 2, COLOR_WHITE);

                ui_print_string(27, row, "RPL:", COLOR_GRAY2);
                screen_print_number(31, row, post->replies, 2, COLOR_WHITE);

                row++;
                count++;
            }
        }
    }

    if (count == 0) {
        ui_print_centered(10, "NO ACTIVE POSTS", COLOR_GRAY2);
        ui_print_centered(12, "POST RELEASES TO FORUMS!", COLOR_WHITE);
    }

    ui_print_centered(23, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();
}
