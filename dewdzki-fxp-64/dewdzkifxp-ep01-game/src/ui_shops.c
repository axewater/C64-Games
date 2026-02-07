/* ui_shops.c - Hardware shop and reputation shop screens */

#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "ftp.h"
#include "input.h"
#include <conio.h>

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
