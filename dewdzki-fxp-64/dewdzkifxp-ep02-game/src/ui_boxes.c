#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "box.h"
#include "range.h"
#include "tools.h"
#include "input.h"
#include "sound.h"
#include <conio.h>

/* Pagination state */
static uint8_t list_page = 0;
#define BOXES_PER_PAGE 8

void ui_show_box_list(void) {
    uint8_t i;
    uint8_t y;
    uint8_t shown = 0;
    uint8_t page_start;
    uint8_t total_active;
    uint8_t display_idx = 0;
    uint8_t choice;
    uint8_t box_indices[BOXES_PER_PAGE];

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== YOUR BOXES ==", COLOR_CYAN);

    total_active = game.active_box_count;

    if (total_active == 0) {
        ui_print_centered(10, "NO BOXES YET.", COLOR_GRAY2);
        ui_print_centered(11, "SCAN AN IP RANGE FIRST!", COLOR_YELLOW);
        ui_print_centered(20, "PRESS ANY KEY", COLOR_GRAY2);
        input_wait_key();
        game.state = STATE_PLAYING;
        return;
    }

    /* Count to find page start */
    page_start = list_page * BOXES_PER_PAGE;

    /* Header row */
    y = 5;
    ui_print_string(1, y, "#", COLOR_GRAY2);
    ui_print_string(3, y, "IP ADDRESS", COLOR_GRAY2);
    ui_print_string(19, y, "SVC", COLOR_GRAY2);
    ui_print_string(24, y, "RISK", COLOR_GRAY2);
    ui_print_string(30, y, "BW", COLOR_GRAY2);
    ui_print_string(34, y, "DAYS", COLOR_GRAY2);
    ++y;

    /* List active boxes for this page */
    for (i = 0; i < MAX_BOXES && shown < BOXES_PER_PAGE; ++i) {
        if (boxes[i].active != BOX_ACTIVE) continue;

        if (display_idx < page_start) {
            ++display_idx;
            continue;
        }

        box_indices[shown] = i;

        /* Row number */
        screen_set_char(1, y, '1' + shown, COLOR_WHITE);
        screen_set_char(2, y, '.', COLOR_WHITE);

        /* IP */
        ui_print_ip(3, y, boxes[i].range_id, boxes[i].octet3, boxes[i].octet4, COLOR_LIGHTBLUE);

        /* Services */
        {
            uint8_t sx = 19;
            if (boxes[i].services & SVC_FTP) {
                screen_set_char(sx, y, 'F', COLOR_GREEN);
                ++sx;
            }
            if (boxes[i].services & SVC_SCANNER) {
                screen_set_char(sx, y, 'S', COLOR_CYAN);
                ++sx;
            }
            if (boxes[i].services & SVC_PROXY) {
                screen_set_char(sx, y, 'P', COLOR_YELLOW);
                ++sx;
            }
            if (boxes[i].services == SVC_NONE) {
                screen_set_char(sx, y, '-', COLOR_GRAY2);
            }
        }

        /* Risk with color coding */
        {
            uint8_t risk_color;
            if (boxes[i].risk < 25) risk_color = COLOR_GREEN;
            else if (boxes[i].risk < 50) risk_color = COLOR_YELLOW;
            else if (boxes[i].risk < 75) risk_color = COLOR_ORANGE;
            else risk_color = COLOR_RED;

            screen_print_number(24, y, boxes[i].risk, 3, risk_color);
        }

        /* Bandwidth */
        screen_print_number(30, y, boxes[i].bandwidth, 3, COLOR_WHITE);

        /* Days alive */
        screen_print_number(34, y, boxes[i].days_alive, 3, COLOR_GRAY2);

        ++shown;
        ++display_idx;
        ++y;
    }

    /* Page info */
    {
        uint8_t total_pages;
        total_pages = (total_active + BOXES_PER_PAGE - 1) / BOXES_PER_PAGE;
        ui_print_string(2, 15, "PAGE ", COLOR_GRAY2);
        screen_print_number(7, 15, list_page + 1, 1, COLOR_WHITE);
        screen_set_char(8, 15, '/', COLOR_GRAY2);
        screen_print_number(9, 15, total_pages, 1, COLOR_WHITE);
    }

    /* Navigation hints */
    y = 17;
    ui_print_string(2, y, "1-", COLOR_GRAY2);
    screen_set_char(4, y, '0' + shown, COLOR_GRAY2);
    ui_print_string(5, y, " SELECT BOX", COLOR_GRAY2);
    ++y;

    if (total_active > BOXES_PER_PAGE) {
        ui_print_string(2, y, "N/P NEXT/PREV PAGE", COLOR_GRAY2);
        ++y;
    }

    ui_print_string(2, y, "Q. BACK", COLOR_GRAY2);

    /* Input loop */
    while (1) {
        if (kbhit()) {
            uint8_t key = cgetc();

            if (key >= '1' && key <= ('0' + shown)) {
                game.selected_box = box_indices[key - '1'];
                game.state = STATE_BOX_DETAIL;
                sound_play_click();
                return;
            }
            if ((key == 'n' || key == 'N') && total_active > (list_page + 1) * BOXES_PER_PAGE) {
                ++list_page;
                sound_play_click();
                return; /* Will re-enter ui_show_box_list */
            }
            if ((key == 'p' || key == 'P') && list_page > 0) {
                --list_page;
                sound_play_click();
                return;
            }
            if (key == 'q' || key == 'Q') {
                list_page = 0;
                game.state = STATE_PLAYING;
                sound_play_back();
                return;
            }
        }
    }
}

void ui_show_box_detail(void) {
    uint8_t idx = game.selected_box;
    Box* b = &boxes[idx];
    uint8_t y;
    uint8_t choice;
    uint8_t risk_color;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== BOX DETAIL ==", COLOR_CYAN);

    y = 5;
    ui_print_string(2, y, "IP: ", COLOR_GRAY2);
    ui_print_ip(6, y, b->range_id, b->octet3, b->octet4, COLOR_WHITE);
    ++y;

    ui_print_string(2, y, "RANGE: ", COLOR_GRAY2);
    ui_print_string(9, y, ranges[b->range_id].name, COLOR_LIGHTBLUE);
    ++y;

    ui_print_string(2, y, "BANDWIDTH: ", COLOR_GRAY2);
    screen_print_number(13, y, b->bandwidth, 3, COLOR_WHITE);
    ui_print_string(17, y, " (X100 KB/S)", COLOR_GRAY2);
    ++y;

    ui_print_string(2, y, "STEALTH: ", COLOR_GRAY2);
    screen_print_number(11, y, b->stealth, 1, COLOR_WHITE);
    ui_print_string(12, y, "/3", COLOR_GRAY2);
    ++y;

    ui_print_string(2, y, "RISK: ", COLOR_GRAY2);
    if (b->risk < 25) risk_color = COLOR_GREEN;
    else if (b->risk < 50) risk_color = COLOR_YELLOW;
    else if (b->risk < 75) risk_color = COLOR_ORANGE;
    else risk_color = COLOR_RED;
    ui_draw_progress_bar(8, y, b->risk, 100, risk_color);
    screen_print_number(21, y, b->risk, 3, risk_color);
    screen_set_char(24, y, '%', risk_color);
    ++y;

    ui_print_string(2, y, "DAYS ALIVE: ", COLOR_GRAY2);
    screen_print_number(14, y, b->days_alive, 3, COLOR_WHITE);
    ++y;
    ++y;

    /* Services */
    ui_print_string(2, y, "SERVICES:", COLOR_YELLOW);
    ++y;
    if (b->services & SVC_FTP) {
        ui_print_string(4, y, "- FTP (SERV-U)", COLOR_GREEN);
        ++y;
    }
    if (b->services & SVC_SCANNER) {
        ui_print_string(4, y, "- SCANNER (AUTO-SCAN)", COLOR_CYAN);
        ++y;
    }
    if (b->services & SVC_PROXY) {
        ui_print_string(4, y, "- PROXY", COLOR_YELLOW);
        ++y;
    }
    if (b->services == SVC_NONE) {
        ui_print_string(4, y, "- NONE", COLOR_GRAY2);
        ++y;
    }

    ++y;
    ui_print_string(2, y, "1. INSTALL SERVICE", COLOR_WHITE);
    ++y;
    ui_print_string(2, y, "Q. BACK", COLOR_GRAY2);

    choice = input_read_menu(1);
    if (choice == 0) {
        game.state = STATE_INSTALL_SVC;
    } else {
        game.state = STATE_BOX_LIST;
    }
}

void ui_show_install_service(void) {
    uint8_t idx = game.selected_box;
    Box* b = &boxes[idx];
    uint8_t y;
    uint8_t num_options = 0;
    uint8_t option_svc[3]; /* map option to service bit */
    uint8_t choice;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== INSTALL SERVICE ==", COLOR_CYAN);

    ui_print_string(2, 5, "BOX: ", COLOR_GRAY2);
    ui_print_ip(7, 5, b->range_id, b->octet3, b->octet4, COLOR_WHITE);

    y = 8;

    /* FTP - needs Serv-U tool */
    if (tools_has(TOOL_SERVU)) {
        if (b->services & SVC_FTP) {
            ui_print_string(3, y, "FTP - ALREADY INSTALLED", COLOR_GRAY2);
        } else {
            ++num_options;
            option_svc[num_options - 1] = SVC_FTP;
            screen_set_char(2, y, '0' + num_options, COLOR_WHITE);
            screen_set_char(3, y, '.', COLOR_WHITE);
            ui_print_string(5, y, "INSTALL FTP (SERV-U)", COLOR_GREEN);
            ui_print_string(5, y + 1, "+REP/TURN, +1 RISK", COLOR_GRAY2);
            ++y;
        }
    } else {
        ui_print_string(3, y, "FTP - [NEED SERV-U TOOL]", COLOR_GRAY1);
    }
    ++y;
    ++y;

    /* Scanner - needs Remote Scanner tool */
    if (tools_has(TOOL_SCANNER)) {
        if (b->services & SVC_SCANNER) {
            ui_print_string(3, y, "SCANNER - ALREADY INSTALLED", COLOR_GRAY2);
        } else {
            ++num_options;
            option_svc[num_options - 1] = SVC_SCANNER;
            screen_set_char(2, y, '0' + num_options, COLOR_WHITE);
            screen_set_char(3, y, '.', COLOR_WHITE);
            ui_print_string(5, y, "INSTALL SCANNER", COLOR_CYAN);
            ui_print_string(5, y + 1, "AUTO-SCAN EACH TURN, +1 RISK", COLOR_GRAY2);
            ++y;
        }
    } else {
        ui_print_string(3, y, "SCANNER - [NEED REM.SCANNER]", COLOR_GRAY1);
    }
    ++y;
    ++y;

    /* Proxy - needs Proxy Module tool */
    if (tools_has(TOOL_PROXY)) {
        if (b->services & SVC_PROXY) {
            ui_print_string(3, y, "PROXY - ALREADY INSTALLED", COLOR_GRAY2);
        } else {
            ++num_options;
            option_svc[num_options - 1] = SVC_PROXY;
            screen_set_char(2, y, '0' + num_options, COLOR_WHITE);
            screen_set_char(3, y, '.', COLOR_WHITE);
            ui_print_string(5, y, "INSTALL PROXY", COLOR_YELLOW);
            ui_print_string(5, y + 1, "UNLOCK RANGES, +2 RISK", COLOR_GRAY2);
            ++y;
        }
    } else {
        ui_print_string(3, y, "PROXY - [NEED PROXY MODULE]", COLOR_GRAY1);
    }
    ++y;
    ++y;

    ui_print_string(2, y, "Q. BACK", COLOR_GRAY2);

    if (num_options == 0) {
        ui_print_centered(y + 2, "NOTHING TO INSTALL", COLOR_GRAY2);
        input_wait_key();
        game.state = STATE_BOX_LIST;
        return;
    }

    choice = input_read_menu(num_options);
    if (choice == 255) {
        game.state = STATE_BOX_LIST;
        return;
    }

    if (choice < num_options) {
        box_install_service(idx, option_svc[choice]);

        /* Show confirmation */
        ui_print_centered(22, "SERVICE INSTALLED!", COLOR_GREEN);
        input_wait_key();
    }

    game.state = STATE_BOX_LIST;
}
