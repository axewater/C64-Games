/* ui_releases.c - Topsite browsing, FXP, and release selection screens */

#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "release.h"
#include "ftp.h"
#include "forum.h"
#include "input.h"
#include "random.h"
#include "sound.h"
#include <conio.h>

void ui_show_scan_anim(void) {
    uint8_t frame;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "SCANNING FOR PUBLIC FTPS", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    /* Radar sweep: 48 frames = 3 full rotations (16 directions x 3) */
    for (frame = 0; frame < 48; frame++) {
        ui_animate_matrix(frame);

        /* Simple delay */
        waitvsync();

        /* Allow skip with space */
        if (input_check_quit()) {
            break;
        }
    }

    /* Stop radar sweep sound */
    sound_silence();
}

uint8_t ui_show_topsite_list(void) {
    const char* topsite_names[] = {
        "NEWBIE.DUMP.US",
        "ELITE.WAREZ.NL",
        "0DAY.TOPSITE.SE"
    };
    const char* topsite_quality[] = {
        "LOW QUALITY",
        "GOOD QUALITY",
        "SCENE QUALITY"
    };
    uint8_t max_topsites;
    uint8_t i;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "SELECT TOPSITE", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    /* Show unlocked topsites based on rank */
    max_topsites = game_state.current_topsite + 1;

    for (i = 0; i < max_topsites && i < 3; i++) {
        screen_set_char(5, 7 + i * 3, '[', COLOR_WHITE);
        screen_print_number(6, 7 + i * 3, i + 1, 1, COLOR_YELLOW);
        screen_set_char(7, 7 + i * 3, ']', COLOR_WHITE);

        ui_print_string(9, 7 + i * 3, topsite_names[i], COLOR_CYAN);
        ui_print_string(11, 8 + i * 3, topsite_quality[i], COLOR_GRAY2);
    }

    ui_print_centered(18, "[Q] BACK TO HUB", COLOR_WHITE);

    return input_read_menu(max_topsites);
}

uint8_t ui_show_topsite(uint8_t topsite_idx, Release* releases, uint8_t count) {
    const char* topsite_names[] = {
        "NEWBIE.DUMP.US",
        "ELITE.WAREZ.NL",
        "0DAY.TOPSITE.SE"
    };
    uint8_t i;
    uint8_t row;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);

    /* Show topsite name dynamically */
    if (topsite_idx < 3) {
        ui_print_string(10, 3, "TOPSITE: ", COLOR_WHITE);
        ui_print_string(19, 3, topsite_names[topsite_idx], COLOR_CYAN);
    }

    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    ui_print_string(2, 5, "AVAILABLE RELEASES:", COLOR_WHITE);

    /* Show releases */
    row = 7;
    for (i = 0; i < count; i++) {
        uint8_t name_color;

        /* Color code by group tier */
        if (releases[i].group == GROUP_IND) {
            name_color = COLOR_WHITE;
        } else if (releases[i].group <= GROUP_RZR) {
            name_color = COLOR_CYAN;  /* Elite groups */
        } else {
            name_color = COLOR_YELLOW;  /* Top groups */
        }

        screen_set_char(2, row, '[', COLOR_WHITE);
        screen_print_number(3, row, i + 1, 1, COLOR_YELLOW);
        screen_set_char(4, row, ']', COLOR_WHITE);
        screen_set_char(6, row, 160, COLOR_GREEN);  /* Block */
        ui_print_string(8, row, releases[i].name, name_color);

        row++;
        ui_print_string(8, row, "SIZE:", COLOR_GRAY2);
        screen_print_number(14, row, releases[i].size_mb, 4, COLOR_CYAN);
        ui_print_string(18, row, "MB", COLOR_CYAN);

        row += 2;
    }

    ui_draw_hline(0, row, 40, '-', COLOR_BLUE);
    ui_print_centered(row + 2, "SELECT RELEASE (1-3)", COLOR_WHITE);
    ui_print_centered(row + 3, "[Q] BACK", COLOR_WHITE);

    return input_read_menu(count);
}

uint8_t ui_show_ftp_select(Release* rel) {
    uint8_t i;
    uint8_t row;
    uint8_t menu_idx;
    uint8_t has_space;
    uint8_t color;
    uint8_t choice;
    uint8_t ftp_index_map[MAX_FTP_SERVERS];
    uint8_t selected_ftp_idx;
    uint8_t risk;
    char key;

show_menu:
    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "SELECT DESTINATION FTP", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    ui_print_string(2, 6, "RELEASE:", COLOR_WHITE);
    ui_print_string(11, 6, rel->name, COLOR_YELLOW);

    row = 9;
    menu_idx = 0;

    /* Show all discovered FTPs (2-line format) */
    for (i = 0; i < MAX_FTP_SERVERS; i++) {
        if (ftps[i].active) {
            uint8_t col;
            uint8_t trait_color;
            const char* trait_tag;

            has_space = ftp_has_space(i);
            color = has_space ? COLOR_WHITE : COLOR_GRAY2;

            /* Store mapping for later (NEW) */
            ftp_index_map[menu_idx] = i;

            /* Line 1: [N] FTP-NAME */
            screen_set_char(2, row, '[', color);
            screen_print_number(3, row, menu_idx + 1, 1, COLOR_YELLOW);
            screen_set_char(4, row, ']', color);
            ui_print_string(6, row, ftps[i].name, color);

            /* Line 2: <indent> (X/5) [TRAIT] [XX%] */
            col = 6;

            /* Show slots first */
            screen_set_char(col, row + 1, '(', color);
            screen_print_number(col + 1, row + 1, ftps[i].release_count, 1, color);
            screen_set_char(col + 2, row + 1, '/', color);
            screen_print_number(col + 3, row + 1, MAX_RELEASES_PER_FTP, 1, color);
            screen_set_char(col + 4, row + 1, ')', color);
            col = 12;  /* Fixed position after slots */

            /* Show trait tag if present */
            if (ftps[i].trait != TRAIT_NONE) {
                trait_tag = ftp_get_trait_tag(ftps[i].trait);
                trait_color = ftp_get_trait_color(ftps[i].trait);
                ui_print_string(col, row + 1, trait_tag, trait_color);
                col = 20;  /* Advance past trait tag */
            }

            /* Show raid risk if FTP has been used for posts */
            if (ftps[i].used_for_posts) {
                uint8_t risk = ftps[i].raid_risk;
                uint8_t risk_color;

                if (risk < 34) {
                    risk_color = COLOR_GREEN;
                } else if (risk < 67) {
                    risk_color = COLOR_YELLOW;
                } else {
                    risk_color = COLOR_RED;
                }

                /* Show risk percentage */
                screen_set_char(col, row + 1, '[', color);
                screen_print_number(col + 1, row + 1, risk, 2, risk_color);
                screen_set_char(col + 3, row + 1, '%', color);
                screen_set_char(col + 4, row + 1, ']', color);
            }

            row += 2;  /* 2-line increment */
            menu_idx++;

            if (row >= 19) break;  /* Adjusted limit for 2-line entries */
        }
    }

    ui_print_centered(21, "[Q] CANCEL", COLOR_WHITE);

    choice = input_read_menu(menu_idx);

    /* Check for cancel */
    if (choice == 255) {
        return 255;
    }

    /* Get selected FTP and check risk (NEW) */
    if (choice < menu_idx) {
        selected_ftp_idx = ftp_index_map[choice];

        /* Check if FTP has been used and has high risk */
        if (ftps[selected_ftp_idx].used_for_posts) {
            risk = ftps[selected_ftp_idx].raid_risk;

            if (risk >= 67) {
                /* High risk - show warning and ask for confirmation */
                screen_clear();
                ui_render_hud();

                ui_draw_hline(0, 6, 40, 160, COLOR_RED);
                ui_print_centered(8, "!!! WARNING !!!", COLOR_RED);
                ui_print_centered(10, "HIGH RAID RISK!", COLOR_YELLOW);
                ui_draw_hline(0, 12, 40, 160, COLOR_RED);

                ui_print_string(8, 14, "RAID RISK:", COLOR_WHITE);
                screen_print_number(19, 14, risk, 2, COLOR_RED);
                ui_print_string(21, 14, "%", COLOR_WHITE);

                ui_print_centered(16, "POST ANYWAY?", COLOR_WHITE);
                ui_print_centered(18, "[Y] YES  [N] NO", COLOR_WHITE);

                /* Wait for Y/N */
                while (1) {
                    if (kbhit()) {
                        key = cgetc();
                        if (key == 'y' || key == 'Y') {
                            /* Confirmed - proceed with post */
                            return choice;
                        } else if (key == 'n' || key == 'N' || key == 'q' || key == 'Q') {
                            /* Cancelled - show menu again */
                            goto show_menu;
                        }
                    }
                }
            }
        }
    }

    return choice;
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
        screen_print_number(12, 14, game_state.bandwidth, 3, COLOR_GREEN);
        ui_print_string(15, 14, "KB/S", COLOR_WHITE);

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

uint8_t ui_show_release_select(uint8_t* out_ftp_idx, uint8_t* out_release_idx) {
    uint8_t ftp_idx;
    uint8_t i;
    uint8_t row;
    uint8_t menu_idx;
    uint8_t rel_id;
    Release* rel;
    uint8_t selection_map[32][2];
    uint8_t choice;
    uint8_t active_ftps;
    uint8_t total_releases;

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "SELECT RELEASE TO POST", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    row = 6;
    menu_idx = 0;

    /* Loop through FTPs and show releases */
    for (ftp_idx = 0; ftp_idx < MAX_FTP_SERVERS; ftp_idx++) {
        if (!ftps[ftp_idx].active || ftps[ftp_idx].release_count == 0) {
            continue;
        }

        /* Show FTP name */
        ui_print_string(2, row, ftps[ftp_idx].name, COLOR_CYAN);
        row++;

        /* Show releases on this FTP */
        for (i = 0; i < ftps[ftp_idx].release_count; i++) {
            rel_id = ftps[ftp_idx].releases[i];
            rel = release_get(rel_id);

            if (rel && rel->active) {
                uint8_t posted;
                uint8_t name_color;

                /* Check if this release has been posted */
                posted = forum_has_post(rel_id, ftp_idx);

                if (posted) {
                    name_color = COLOR_GRAY2;  /* Posted releases are grayed out */
                } else {
                    /* Color code by group tier */
                    if (rel->group == GROUP_IND) {
                        name_color = COLOR_WHITE;
                    } else if (rel->group <= GROUP_RZR) {
                        name_color = COLOR_CYAN;  /* Elite groups */
                    } else {
                        name_color = COLOR_YELLOW;  /* Top groups */
                    }
                }

                screen_set_char(4, row, '[', COLOR_WHITE);
                screen_print_number(5, row, menu_idx + 1, 1, COLOR_YELLOW);
                screen_set_char(6, row, ']', COLOR_WHITE);

                ui_print_string(8, row, rel->name, name_color);

                /* Show [POSTED] indicator if already posted */
                if (posted) {
                    ui_print_string(29, row, "[POSTED]", COLOR_GRAY2);
                }

                /* Store mapping */
                selection_map[menu_idx][0] = ftp_idx;
                selection_map[menu_idx][1] = rel_id;

                menu_idx++;
                row++;
            }
        }

        row++;  /* Spacing between FTPs */
    }

    if (menu_idx == 0) {
        /* Debug: Show why no releases found */
        screen_clear();
        ui_render_hud();
        ui_print_centered(8, "DEBUG: NO RELEASES FOUND", COLOR_RED);

        /* Count active FTPs */
        active_ftps = 0;
        total_releases = 0;
        for (i = 0; i < MAX_FTP_SERVERS; i++) {
            if (ftps[i].active) {
                active_ftps++;
                total_releases += ftps[i].release_count;
            }
        }

        ui_print_string(5, 10, "ACTIVE FTPS: ", COLOR_WHITE);
        screen_print_number(18, 10, active_ftps, 1, COLOR_YELLOW);

        ui_print_string(5, 11, "TOTAL RELS: ", COLOR_WHITE);
        screen_print_number(18, 11, total_releases, 2, COLOR_YELLOW);

        /* Show first release details */
        for (i = 0; i < MAX_FTP_SERVERS; i++) {
            if (ftps[i].active && ftps[i].release_count > 0) {
                rel_id = ftps[i].releases[0];
                rel = release_get(rel_id);

                ui_print_string(5, 13, "REL ID: ", COLOR_WHITE);
                screen_print_number(13, 13, rel_id, 3, COLOR_YELLOW);

                if (rel) {
                    ui_print_string(5, 14, "REL PTR: OK", COLOR_GREEN);
                    ui_print_string(5, 15, "ACTIVE: ", COLOR_WHITE);
                    screen_print_number(13, 15, rel->active, 1, COLOR_YELLOW);
                } else {
                    ui_print_string(5, 14, "REL PTR: NULL", COLOR_RED);
                }
                break;
            }
        }

        ui_print_centered(20, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        return 255;  /* No releases available */
    }

    ui_print_centered(20, "[Q] CANCEL", COLOR_WHITE);

    choice = input_read_menu(menu_idx);

    if (choice < menu_idx) {
        *out_ftp_idx = selection_map[choice][0];
        *out_release_idx = selection_map[choice][1];
        return choice;
    }

    return 255;  /* Cancelled */
}
