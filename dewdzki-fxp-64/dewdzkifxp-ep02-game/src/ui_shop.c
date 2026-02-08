#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "box.h"
#include "credential.h"
#include "input.h"
#include "sound.h"
#include <conio.h>

/* Shop items */
#define SHOP_EXTRA_ACTION  0
#define SHOP_STEALTH_UP    1
#define SHOP_MASS_STEALTH  2
#define SHOP_RUSH_CREDS    3
#define SHOP_BACKUP_BOX    4
#define SHOP_ITEMS         5

static const char* shop_names[SHOP_ITEMS] = {
    "EXTRA ACTION",
    "STEALTH UP",
    "MASS STEALTH",
    "RUSH CREDS",
    "BACKUP BOX"
};

static const uint16_t shop_costs[SHOP_ITEMS] = {
    50, 100, 200, 75, 150
};

static const char* shop_desc[SHOP_ITEMS] = {
    "+1 ACTION THIS TURN",
    "+1 STEALTH ON 1 BOX",
    "+1 STEALTH ALL BOXES",
    "COMPLETE 1 CRED JOB",
    "REVIVE 1 LOST BOX"
};

void ui_show_shop(void) {
    uint8_t i;
    uint8_t y;
    uint8_t choice;
    uint8_t can_afford;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== UPGRADE SHOP ==", COLOR_CYAN);

    ui_print_string(2, 4, "REP: ", COLOR_GRAY2);
    screen_print_number(7, 4, game.reputation, 4, COLOR_YELLOW);

    y = 6;
    for (i = 0; i < SHOP_ITEMS; ++i) {
        can_afford = (game.reputation >= shop_costs[i]) ? 1 : 0;

        screen_set_char(2, y, '1' + i, can_afford ? COLOR_WHITE : COLOR_GRAY1);
        screen_set_char(3, y, '.', can_afford ? COLOR_WHITE : COLOR_GRAY1);
        ui_print_string(5, y, shop_names[i], can_afford ? COLOR_WHITE : COLOR_GRAY1);

        /* Cost */
        screen_print_number(24, y, shop_costs[i], 3, can_afford ? COLOR_YELLOW : COLOR_GRAY1);
        ui_print_string(28, y, "REP", can_afford ? COLOR_YELLOW : COLOR_GRAY1);

        /* Description */
        ui_print_string(5, y + 1, shop_desc[i], COLOR_GRAY2);

        y += 2;
    }

    ++y;
    ui_print_string(2, y, "Q. BACK", COLOR_GRAY2);

    choice = input_read_menu(SHOP_ITEMS);

    if (choice == 255) {
        game.state = STATE_PLAYING;
        return;
    }

    if (choice >= SHOP_ITEMS) {
        return;
    }

    /* Check if can afford */
    if (!gamestate_spend_rep(shop_costs[choice])) {
        ui_print_centered(22, "NOT ENOUGH REPUTATION!", COLOR_RED);
        sound_play_back();
        input_wait_key();
        return; /* Stay in shop */
    }

    /* Apply purchase */
    switch (choice) {
        case SHOP_EXTRA_ACTION:
            ++game.actions_remaining;
            ui_print_centered(22, "+1 ACTION!", COLOR_GREEN);
            break;

        case SHOP_STEALTH_UP:
            /* Find first active box with stealth < 3 */
            {
                uint8_t found = 0;
                uint8_t j;
                for (j = 0; j < MAX_BOXES; ++j) {
                    if (boxes[j].active == BOX_ACTIVE && boxes[j].stealth < 3) {
                        ++boxes[j].stealth;
                        ui_print_centered(21, "STEALTH UP ON:", COLOR_GREEN);
                        ui_print_ip(12, 22, boxes[j].range_id,
                                   boxes[j].octet3, boxes[j].octet4, COLOR_WHITE);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    /* Refund */
                    gamestate_add_reputation(shop_costs[choice]);
                    ui_print_centered(22, "NO BOXES NEED STEALTH!", COLOR_YELLOW);
                }
            }
            break;

        case SHOP_MASS_STEALTH:
            {
                uint8_t j;
                uint8_t upgraded = 0;
                for (j = 0; j < MAX_BOXES; ++j) {
                    if (boxes[j].active == BOX_ACTIVE && boxes[j].stealth < 3) {
                        ++boxes[j].stealth;
                        ++upgraded;
                    }
                }
                ui_print_string(10, 22, "+1 STEALTH ON ", COLOR_GREEN);
                screen_print_number(24, 22, upgraded, 2, COLOR_WHITE);
                ui_print_string(27, 22, "BOXES", COLOR_GREEN);
            }
            break;

        case SHOP_RUSH_CREDS:
            if (credential_rush_one()) {
                ui_print_centered(22, "CRED JOB COMPLETED!", COLOR_GREEN);
            } else {
                /* Refund */
                gamestate_add_reputation(shop_costs[choice]);
                ui_print_centered(22, "NO ACTIVE CRED JOBS!", COLOR_YELLOW);
            }
            break;

        case SHOP_BACKUP_BOX:
            /* Find first discovered box and revive it */
            {
                uint8_t found = 0;
                uint8_t j;
                for (j = 0; j < MAX_BOXES; ++j) {
                    if (boxes[j].active == BOX_DISCOVERED) {
                        boxes[j].active = BOX_ACTIVE;
                        boxes[j].risk = 0;
                        boxes[j].services = SVC_NONE;
                        ++game.active_box_count;
                        ui_print_centered(21, "BOX REVIVED:", COLOR_GREEN);
                        ui_print_ip(12, 22, boxes[j].range_id,
                                   boxes[j].octet3, boxes[j].octet4, COLOR_WHITE);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    gamestate_add_reputation(shop_costs[choice]);
                    ui_print_centered(22, "NO BOXES TO REVIVE!", COLOR_YELLOW);
                }
            }
            break;
    }

    sound_play_click();
    input_wait_key();
    /* Stay in shop for more purchases */
}
