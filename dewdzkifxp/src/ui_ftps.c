/* ui_ftps.c - FTP management screens */

#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "ftp.h"
#include "release.h"
#include "forum.h"
#include "input.h"
#include <conio.h>

void ui_show_ftps(void) {
    uint8_t i;
    uint8_t j;
    uint8_t row;
    uint8_t count;
    FTPServer* ftp;
    Release* rel;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "DISCOVERED FTP SERVERS", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, '-', COLOR_BLUE);

    row = 6;
    count = 0;

    /* Show all discovered FTPs */
    for (i = 0; i < MAX_FTP_SERVERS; i++) {
        if (ftps[i].active && row < 22) {
            ftp = &ftps[i];

            /* FTP name and info */
            ui_print_string(1, row, ftp->name, COLOR_CYAN);

            /* Trait tag (NEW) - show if FTP has a trait */
            if (ftp->trait != TRAIT_NONE) {
                const char* trait_tag = ftp_get_trait_tag(ftp->trait);
                uint8_t trait_color = ftp_get_trait_color(ftp->trait);
                ui_print_string(21, row, trait_tag, trait_color);
            }

            row++;

            /* Heat meter (NEW) - show if FTP has been used for posts */
            if (ftp->used_for_posts) {
                const char* heat_text = ftp_get_heat_text(i);
                uint8_t heat_color = ftp_get_heat_color(i);
                ui_print_string(3, row, "HEAT:[", COLOR_GRAY2);
                ui_print_string(9, row, heat_text, heat_color);
                ui_print_string(13, row, "]", COLOR_GRAY2);
            }

            row++;

            /* Bandwidth and slots on second line */
            ui_print_string(3, row, "BW:", COLOR_GRAY2);
            screen_print_number(6, row, ftp->bandwidth, 3, COLOR_YELLOW);
            ui_print_string(9, row, "KB/S", COLOR_GRAY2);

            /* Slots used */
            ui_print_string(17, row, "SLOTS:", COLOR_GRAY2);
            screen_print_number(24, row, ftp->release_count, 1, COLOR_WHITE);
            ui_print_string(25, row, "/", COLOR_GRAY2);
            screen_print_number(26, row, MAX_RELEASES_PER_FTP, 1, COLOR_WHITE);

            /* Show detailed risk percentage if FTP has been used */
            if (ftp->used_for_posts) {
                ui_print_string(29, row, "RISK:", COLOR_GRAY2);
                screen_print_number(34, row, ftp->raid_risk, 2, ftp_get_heat_color(i));
                ui_print_string(36, row, "%", COLOR_GRAY2);
            }

            row++;

            /* Show releases on this FTP */
            if (ftp->release_count > 0) {
                for (j = 0; j < ftp->release_count && j < MAX_RELEASES_PER_FTP; j++) {
                    rel = release_get(ftp->releases[j]);
                    if (rel && rel->active && row < 22) {
                        uint8_t name_color;

                        /* Color code by group tier */
                        if (rel->group == GROUP_IND) {
                            name_color = COLOR_WHITE;
                        } else if (rel->group <= GROUP_RZR) {
                            name_color = COLOR_CYAN;  /* Elite groups */
                        } else {
                            name_color = COLOR_YELLOW;  /* Top groups */
                        }

                        ui_print_string(3, row, "- ", COLOR_GRAY2);
                        ui_print_string(5, row, rel->name, name_color);
                        row++;
                    }
                }
            } else {
                ui_print_string(3, row, "- EMPTY", COLOR_GRAY2);
                row++;
            }

            row++;  /* Spacing between FTPs */
            count++;
        }
    }

    if (count == 0) {
        ui_print_centered(10, "NO FTP SERVERS FOUND", COLOR_GRAY2);
        ui_print_centered(12, "SCAN FOR FTPS FIRST!", COLOR_WHITE);
    }

    ui_print_centered(22, "[M] MOVE RELEASES", COLOR_YELLOW);
    ui_print_centered(23, "[SPACE] CONTINUE", COLOR_WHITE);

    /* Wait for key and check for M */
    {
        char key = cgetc();
        if (key == 'm' || key == 'M') {
            game_state.state = STATE_MOVE_RELEASES;
        }
    }
}

uint8_t ui_show_move_releases(void) {
    uint8_t i;
    uint8_t row;
    uint8_t src_ftp_idx, dst_ftp_idx;
    uint8_t release_idx;
    uint8_t choice;
    FTPServer* src_ftp;
    FTPServer* dst_ftp;
    Release* rel;
    uint8_t ftps_with_releases[MAX_FTP_SERVERS];
    uint8_t ftp_count;
    uint8_t valid_dsts[MAX_FTP_SERVERS];
    uint8_t valid_dst_count;
    char input_buf[2];

    /* Step 1: Find FTPs with releases */
    ftp_count = 0;
    for (i = 0; i < MAX_FTP_SERVERS; i++) {
        if (ftps[i].active && ftps[i].release_count > 0) {
            ftps_with_releases[ftp_count++] = i;
        }
    }

    if (ftp_count == 0) {
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "NO RELEASES TO MOVE!", COLOR_RED);
        ui_print_centered(12, "FXP SOME RELEASES FIRST", COLOR_WHITE);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        return 0;
    }

    /* Step 2: Select source FTP */
    screen_clear();
    ui_render_hud();
    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "MOVE RELEASES - SELECT SOURCE", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, '-', COLOR_BLUE);

    row = 6;
    for (i = 0; i < ftp_count && row < 20; i++) {
        src_ftp = &ftps[ftps_with_releases[i]];
        screen_set_char(2, row, '[', COLOR_WHITE);
        screen_print_number(3, row, i + 1, 1, COLOR_YELLOW);
        screen_set_char(4, row, ']', COLOR_WHITE);
        ui_print_string(6, row, src_ftp->name, COLOR_CYAN);

        /* Show trait tag (NEW) */
        if (src_ftp->trait != TRAIT_NONE) {
            const char* trait_tag = ftp_get_trait_tag(src_ftp->trait);
            uint8_t trait_color = ftp_get_trait_color(src_ftp->trait);
            ui_print_string(27, row, trait_tag, trait_color);
        }

        ui_print_string(37, row, "(", COLOR_GRAY2);
        screen_print_number(38, row, src_ftp->release_count, 1, COLOR_WHITE);
        ui_print_string(39, row, ")", COLOR_GRAY2);
        row++;
    }

    ui_print_centered(22, "[1-8] SELECT  [Q] CANCEL", COLOR_WHITE);

    /* Read source FTP choice */
    input_buf[0] = cgetc();
    if (input_buf[0] == 'q' || input_buf[0] == 'Q') {
        return 0;
    }

    choice = input_buf[0] - '1';
    if (choice >= ftp_count) {
        return 0;
    }

    src_ftp_idx = ftps_with_releases[choice];
    src_ftp = &ftps[src_ftp_idx];

    /* Step 3: Select release to move */
    screen_clear();
    ui_render_hud();
    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "SELECT RELEASE TO MOVE", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, '-', COLOR_BLUE);

    ui_print_string(2, 6, "FROM: ", COLOR_WHITE);
    ui_print_string(8, 6, src_ftp->name, COLOR_CYAN);

    row = 8;
    for (i = 0; i < src_ftp->release_count; i++) {
        rel = release_get(src_ftp->releases[i]);
        if (rel && rel->active) {
            screen_set_char(2, row, '[', COLOR_WHITE);
            screen_print_number(3, row, i + 1, 1, COLOR_YELLOW);
            screen_set_char(4, row, ']', COLOR_WHITE);
            ui_print_string(6, row, rel->name, COLOR_WHITE);
            row++;
        }
    }

    ui_print_centered(22, "[1-5] SELECT  [Q] CANCEL", COLOR_WHITE);

    /* Read release choice */
    input_buf[0] = cgetc();
    if (input_buf[0] == 'q' || input_buf[0] == 'Q') {
        return 0;
    }

    release_idx = input_buf[0] - '1';
    if (release_idx >= src_ftp->release_count) {
        return 0;
    }

    rel = release_get(src_ftp->releases[release_idx]);
    if (!rel || !rel->active) {
        return 0;
    }

    /* Step 4: Find valid destination FTPs (not source, has space) */
    valid_dst_count = 0;
    for (i = 0; i < MAX_FTP_SERVERS; i++) {
        if (ftps[i].active && i != src_ftp_idx && ftp_has_space(i)) {
            valid_dsts[valid_dst_count++] = i;
        }
    }

    if (valid_dst_count == 0) {
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "NO VALID DESTINATIONS!", COLOR_RED);
        ui_print_centered(12, "ALL OTHER FTPS ARE FULL", COLOR_WHITE);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        return 0;
    }

    /* Step 5: Select destination FTP */
    screen_clear();
    ui_render_hud();
    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "SELECT DESTINATION FTP", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, '-', COLOR_BLUE);

    ui_print_string(2, 6, "MOVING: ", COLOR_WHITE);
    ui_print_string(10, 6, rel->name, COLOR_YELLOW);

    row = 8;
    for (i = 0; i < valid_dst_count && row < 20; i++) {
        dst_ftp = &ftps[valid_dsts[i]];
        screen_set_char(2, row, '[', COLOR_WHITE);
        screen_print_number(3, row, i + 1, 1, COLOR_YELLOW);
        screen_set_char(4, row, ']', COLOR_WHITE);
        ui_print_string(6, row, dst_ftp->name, COLOR_CYAN);

        /* Show trait tag (NEW) */
        if (dst_ftp->trait != TRAIT_NONE) {
            const char* trait_tag = ftp_get_trait_tag(dst_ftp->trait);
            uint8_t trait_color = ftp_get_trait_color(dst_ftp->trait);
            ui_print_string(27, row, trait_tag, trait_color);
        }

        ui_print_string(36, row, "(", COLOR_GRAY2);
        screen_print_number(37, row, dst_ftp->release_count, 1, COLOR_WHITE);
        ui_print_string(38, row, "/", COLOR_GRAY2);
        screen_print_number(39, row, MAX_RELEASES_PER_FTP, 1, COLOR_WHITE);
        ui_print_string(40, row, ")", COLOR_GRAY2);
        row++;
    }

    ui_print_centered(22, "[1-8] SELECT  [Q] CANCEL", COLOR_WHITE);

    /* Read destination FTP choice */
    input_buf[0] = cgetc();
    if (input_buf[0] == 'q' || input_buf[0] == 'Q') {
        return 0;
    }

    choice = input_buf[0] - '1';
    if (choice >= valid_dst_count) {
        return 0;
    }

    dst_ftp_idx = valid_dsts[choice];
    dst_ftp = &ftps[dst_ftp_idx];

    /* Execute move */
    if (ftp_move_release(src_ftp_idx, dst_ftp_idx, src_ftp->releases[release_idx])) {
        uint8_t moved_release_id = src_ftp->releases[release_idx];

        /* Update any forum posts to point to new FTP */
        for (i = 0; i < MAX_POSTS; i++) {
            if (posts[i].active && posts[i].release_id == moved_release_id && posts[i].ftp_id == src_ftp_idx) {
                posts[i].ftp_id = dst_ftp_idx;
            }
        }

        /* Success message */
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "RELEASE MOVED!", COLOR_GREEN);
        ui_print_string(5, 12, rel->name, COLOR_YELLOW);
        ui_print_centered(14, "FROM", COLOR_WHITE);
        ui_print_string(5, 15, src_ftp->name, COLOR_CYAN);
        ui_print_centered(17, "TO", COLOR_WHITE);
        ui_print_string(5, 18, dst_ftp->name, COLOR_CYAN);
        ui_print_centered(22, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();

        return 1;  /* Success */
    }

    return 0;  /* Failed */
}
