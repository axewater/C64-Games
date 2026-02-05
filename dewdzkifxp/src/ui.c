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
    ui_draw_hline(0, 14, 40, '-', COLOR_BLUE);
    ui_print_centered(15, "[1] SCAN FOR FTPS", COLOR_WHITE);
    ui_print_centered(16, "[2] BROWSE TOPSITE", COLOR_WHITE);
    ui_print_centered(17, "[3] POST TO FORUM", COLOR_WHITE);
    ui_print_centered(18, "[4] VIEW STATS", COLOR_WHITE);
    ui_print_centered(19, "[5] VIEW FTPS", COLOR_WHITE);
    ui_print_centered(21, "[Q] QUIT", COLOR_WHITE);
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

            screen_set_char(2, row, '[', color);
            screen_print_number(3, row, menu_idx + 1, 1, color);
            screen_set_char(4, row, ']', color);

            ui_print_string(6, row, ftps[i].name, color);

            /* Show raid risk if FTP has been used for posts */
            if (ftps[i].used_for_posts) {
                uint8_t risk = ftps[i].raid_risk;
                const char* risk_label;
                uint8_t risk_color;

                if (risk < 34) {
                    risk_label = "LOW";
                    risk_color = COLOR_GREEN;
                } else if (risk < 67) {
                    risk_label = "MED";
                    risk_color = COLOR_YELLOW;
                } else {
                    risk_label = "HGH";
                    risk_color = COLOR_RED;
                }

                ui_print_string(30, row, risk_label, risk_color);
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

    return input_read_menu(menu_idx);
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

            /* Bandwidth */
            ui_print_string(22, row, "BW:", COLOR_GRAY2);
            screen_print_number(25, row, ftp->bandwidth, 3, COLOR_YELLOW);

            /* Slots used */
            ui_print_string(29, row, "(", COLOR_GRAY2);
            screen_print_number(30, row, ftp->release_count, 1, COLOR_WHITE);
            ui_print_string(31, row, "/", COLOR_GRAY2);
            screen_print_number(32, row, MAX_RELEASES_PER_FTP, 1, COLOR_WHITE);
            ui_print_string(33, row, ")", COLOR_GRAY2);

            row++;

            /* Show raid risk if FTP has been used for posts */
            if (ftp->used_for_posts) {
                uint8_t risk = ftp->raid_risk;
                const char* risk_label;
                uint8_t risk_color;

                if (risk < 34) {
                    risk_label = "LOW";
                    risk_color = COLOR_GREEN;
                } else if (risk < 67) {
                    risk_label = "MED";
                    risk_color = COLOR_YELLOW;
                } else {
                    risk_label = "HGH";
                    risk_color = COLOR_RED;
                }

                ui_print_string(22, row, "RISK:", COLOR_GRAY2);
                ui_print_string(28, row, risk_label, risk_color);
                row++;
            }

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

    ui_print_centered(23, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();
}
