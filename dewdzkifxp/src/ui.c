#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "rank.h"
#include "forum.h"
#include "release.h"
#include "ftp.h"
#include "input.h"
#include "random.h"
#include <conio.h>
#include <stdio.h>
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

    /* Show all discovered FTPs */
    for (i = 0; i < MAX_FTP_SERVERS; i++) {
        if (ftps[i].active) {
            has_space = ftp_has_space(i);
            color = has_space ? COLOR_WHITE : COLOR_GRAY2;

            /* Store mapping for later (NEW) */
            ftp_index_map[menu_idx] = i;

            screen_set_char(2, row, '[', color);
            screen_print_number(3, row, menu_idx + 1, 1, color);
            screen_set_char(4, row, ']', color);

            ui_print_string(6, row, ftps[i].name, color);

            /* Show trait tag (NEW) */
            if (ftps[i].trait != TRAIT_NONE) {
                const char* trait_tag = ftp_get_trait_tag(ftps[i].trait);
                uint8_t trait_color = ftp_get_trait_color(ftps[i].trait);
                ui_print_string(27, row, trait_tag, trait_color);
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

                /* Show risk percentage (NEW) */
                screen_set_char(29, row, '[', color);
                screen_print_number(30, row, risk, 2, risk_color);
                screen_set_char(32, row, '%', color);
                screen_set_char(33, row, ']', color);
            }

            /* Show slots */
            ui_print_string(24, row, "(", color);
            screen_print_number(25, row, ftps[i].release_count, 1, color);
            screen_set_char(26, row, '/', color);
            screen_print_number(27, row, MAX_RELEASES_PER_FTP, 1, color);
            ui_print_string(28, row, ")", color);

            row++;
            menu_idx++;
        }
    }

    ui_print_centered(18, "[Q] CANCEL", COLOR_WHITE);

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

uint8_t ui_show_hardware_shop(void) {
    uint8_t i;
    uint8_t row;
    uint8_t color;
    uint8_t owned;
    uint8_t can_afford;
    extern const HardwareTier hardware_tiers[8];

    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "HARDWARE SHOP - CUMULATIVE", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    /* Show current totals */
    ui_print_string(2, 5, "CURRENT BW:", COLOR_WHITE);
    screen_print_number(14, 5, game_state.bandwidth, 3, COLOR_GREEN);
    ui_print_string(18, 5, "KB/S", COLOR_GRAY2);

    ui_print_string(25, 5, "ACT:", COLOR_WHITE);
    screen_print_number(30, 5, gamestate_calculate_actions(), 1, COLOR_GREEN);

    row = 7;

    /* List all 8 hardware items */
    for (i = 0; i < 8 && row < 20; i++) {
        const HardwareTier* tier = &hardware_tiers[i];
        owned = gamestate_has_hardware(1 << i);
        can_afford = (game_state.reputation >= tier->rep_cost);

        /* Color: gray=owned, white=can afford, red=too expensive */
        if (owned) {
            color = COLOR_GRAY2;
        } else if (can_afford) {
            color = COLOR_WHITE;
        } else {
            color = COLOR_GRAY3;
        }

        /* Menu number */
        screen_set_char(2, row, '[', color);
        screen_print_number(3, row, i + 1, 1, color);
        screen_set_char(4, row, ']', color);

        /* Hardware name */
        ui_print_string(6, row, tier->name, color);

        /* Owned indicator */
        if (owned) {
            ui_print_string(24, row, "[OWNED]", COLOR_GRAY2);
        } else {
            /* REP cost for unowned */
            ui_print_string(24, row, "REP:", COLOR_GRAY2);
            screen_print_number(29, row, tier->rep_cost, 3, color);
        }

        row++;

        /* Show effect */
        if (i == 6) {
            /* Second Phone Line */
            ui_print_string(8, row, "+1 ACTION", color);
        } else if (tier->bandwidth > 0) {
            /* Bandwidth items */
            ui_print_string(8, row, "+", color);
            screen_print_number(9, row, tier->bandwidth, 3, color);
            ui_print_string(13, row, "KB/S", color);
        }

        row++;
    }

    ui_print_centered(21, "[1-8] PURCHASE [Q] BACK", COLOR_WHITE);

    return input_read_menu(8);
}

void ui_show_raid_alert(void) {
    uint8_t i;
    uint8_t posts_nuked;
    uint8_t flash;

    screen_clear();
    ui_render_hud();

    /* Flashing red border effect */
    for (flash = 0; flash < 3; flash++) {
        /* Draw red border */
        ui_draw_hline(0, 2, 40, 160, COLOR_RED);
        ui_draw_hline(0, 18, 40, 160, COLOR_RED);

        ui_print_centered(8, "!!! POLICE RAID !!!", COLOR_RED);
        ui_print_centered(10, "FTP SERVER SEIZED", COLOR_WHITE);

        /* Show downloads that triggered raid */
        ui_print_string(8, 12, "DOWNLOADS:", COLOR_GRAY2);
        screen_print_number(19, 12, game_state.last_raid_downloads, 5, COLOR_YELLOW);

        /* Count nuked posts */
        posts_nuked = 0;
        for (i = 0; i < MAX_POSTS; i++) {
            if (posts[i].active && posts[i].nuked &&
                posts[i].ftp_id == game_state.last_raid_ftp_idx) {
                posts_nuked++;
            }
        }

        ui_print_string(8, 13, "POSTS NUKED:", COLOR_GRAY2);
        screen_print_number(21, 13, posts_nuked, 2, COLOR_RED);

        ui_print_centered(16, "YOUR WAREZ ARE GONE!", COLOR_RED);

        /* Flash delay */
        for (i = 0; i < 10; i++) {
            waitvsync();
        }

        /* Clear for next flash */
        if (flash < 2) {
            screen_clear();
            ui_render_hud();
            for (i = 0; i < 5; i++) {
                waitvsync();
            }
        }
    }

    ui_print_centered(20, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();

    /* Reset raid tracking */
    game_state.last_raid_ftp_idx = 255;
}

void ui_show_event(void) {
    const char* event_titles[13] = {
        "ELITE NOTICED YOUR WORK!",
        "RELEASE WENT VIRAL!",
        "FORUM FEATURED POST!",
        "ACCUSED OF DUPING!",
        "RELEASE WAS FAKE!",
        "FORUM DRAMA!",
        "BANDWIDTH UPGRADE!",
        "ISP THROTTLING!",
        "HOT TIP: NEW FTP!",
        "COURIER HINT!",
        "FRIEND LENDS T1 LINE!",
        "CABLE MODEM ACCESS!",
        "CORPORATE T3 HOOKUP!"
    };

    const char* event_descriptions[13] = {
        "AN ELITE MEMBER SAW YOUR POST",
        "YOUR RELEASE IS TRENDING",
        "MODERATORS LIKED YOUR CONTENT",
        "SOMEONE CLAIMS YOU'RE A FAKER",
        "YOUR UPLOAD WAS CORRUPTED",
        "YOU GOT INTO AN ARGUMENT",
        "YOUR ISP UPGRADED SERVICE",
        "CONNECTION ISSUES DETECTED",
        "A COURIER SHARED FTP INFO",
        "INSIDER TIP ON TOPSITE",
        "TEMPORARY BANDWIDTH BOOST",
        "NEIGHBOR SHARING INTERNET",
        "OFFICE LETS YOU USE THEIR NET"
    };

    uint8_t event_type;

    screen_clear();
    ui_render_hud();

    event_type = game_state.current_event - 1;  /* 1-13 -> 0-12 */

    if (event_type >= 13) {
        return;  /* Invalid event */
    }

    ui_draw_hline(0, 6, 40, 160, COLOR_CYAN);
    ui_print_centered(8, "*** RANDOM EVENT ***", COLOR_YELLOW);
    ui_draw_hline(0, 10, 40, 160, COLOR_CYAN);

    ui_print_centered(12, event_titles[event_type], COLOR_WHITE);
    ui_print_centered(14, event_descriptions[event_type], COLOR_GRAY2);

    /* Show effect based on event type */
    if (event_type == 0) {
        /* Elite noticed: +REP */
        ui_print_string(12, 16, "REP:", COLOR_WHITE);
        ui_print_string(17, 16, "+", COLOR_GREEN);
        screen_print_number(18, 16, game_state.event_value, 3, COLOR_GREEN);
    } else if (event_type == 1) {
        /* Viral release: double downloads message */
        ui_print_centered(16, "DOWNLOADS DOUBLED!", COLOR_GREEN);
    } else if (event_type == 2) {
        /* Featured post: +REP */
        ui_print_string(14, 16, "REP: +25", COLOR_GREEN);
    } else if (event_type == 3 || event_type == 5) {
        /* Duping/Drama: -REP */
        ui_print_string(12, 16, "REP:", COLOR_WHITE);
        ui_print_string(17, 16, "-", COLOR_RED);
        screen_print_number(18, 16, game_state.event_value, 2, COLOR_RED);
    } else if (event_type == 4) {
        /* Fake release: nuked */
        ui_print_centered(16, "POST NUKED!", COLOR_RED);
    } else if (event_type == 6) {
        /* BW upgrade */
        ui_print_string(10, 16, "BANDWIDTH +20 FOR 5 TURNS", COLOR_GREEN);
    } else if (event_type == 7) {
        /* ISP throttle */
        ui_print_string(10, 16, "BANDWIDTH -15 FOR 3 TURNS", COLOR_RED);
    } else if (event_type == 8) {
        /* New FTP discovered */
        ui_print_centered(16, "FTP AUTO-DISCOVERED!", COLOR_GREEN);
    } else if (event_type == 9) {
        /* Topsite preview */
        ui_print_centered(16, "TOPSITE UNLOCKED!", COLOR_GREEN);
    } else if (event_type == 10) {
        /* T1 line rental (NEW) */
        ui_print_string(8, 16, "+193 KB/S FOR 10 TURNS", COLOR_GREEN);
    } else if (event_type == 11) {
        /* Cable modem rental (NEW) */
        ui_print_string(8, 16, "+256 KB/S FOR 5 TURNS", COLOR_GREEN);
    } else if (event_type == 12) {
        /* T3 line rental (NEW) */
        ui_print_string(8, 16, "+512 KB/S FOR 3 TURNS", COLOR_GREEN);
    }

    ui_print_centered(20, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();

    /* Reset event */
    game_state.current_event = 0;
    game_state.event_value = 0;
}

uint8_t ui_show_move_releases(void) {
    uint8_t i, j;
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

uint8_t ui_show_rep_shop(void) {
    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "REPUTATION SHOP", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    ui_print_string(2, 5, "CURRENT REP:", COLOR_WHITE);
    screen_print_number(15, 5, game_state.reputation, 4, COLOR_YELLOW);

    /* Option 1: Bribe Sysop */
    ui_print_string(2, 7, "[1] BRIBE SYSOP", COLOR_WHITE);
    ui_print_string(28, 7, "50 REP", COLOR_GRAY2);
    ui_print_string(4, 8, "REMOVE NUKE FROM 1 POST", COLOR_GRAY2);

    /* Option 2: Inside Info (simplified - just show message) */
    ui_print_string(2, 10, "[2] INSIDE INFO", COLOR_WHITE);
    ui_print_string(28, 10, "30 REP", COLOR_GRAY2);
    ui_print_string(4, 11, "PREVIEW NEXT 3 RELEASES", COLOR_GRAY2);

    /* Option 3: VIP Access */
    ui_print_string(2, 13, "[3] VIP ACCESS", COLOR_WHITE);
    ui_print_string(27, 13, "100 REP", COLOR_GRAY2);
    ui_print_string(4, 14, "PREMIUM FORUM (20 TURNS, 5X REP)", COLOR_GRAY2);

    /* Option 4: Security Audit */
    ui_print_string(2, 16, "[4] SECURITY AUDIT", COLOR_WHITE);
    ui_print_string(28, 16, "75 REP", COLOR_GRAY2);
    ui_print_string(4, 17, "-25% FTP RISK (10 TURNS)", COLOR_GRAY2);

    ui_print_centered(22, "[1-4] PURCHASE [Q] BACK", COLOR_WHITE);

    return input_read_menu(4);
}

void ui_show_inside_info(void) {
    screen_clear();
    ui_render_hud();

    ui_draw_hline(0, 2, 40, 160, COLOR_BLUE);
    ui_print_centered(3, "INSIDE INFO - COMING SOON", COLOR_CYAN);
    ui_draw_hline(0, 4, 40, 160, COLOR_BLUE);

    ui_print_centered(10, "PREVIEW NEXT 3 RELEASES", COLOR_WHITE);
    ui_print_centered(12, "FEATURE NOT IMPLEMENTED", COLOR_GRAY2);

    ui_print_centered(20, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();
}
