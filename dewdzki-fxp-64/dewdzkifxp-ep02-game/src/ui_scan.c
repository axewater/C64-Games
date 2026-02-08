#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "range.h"
#include "box.h"
#include "random.h"
#include "input.h"
#include "sound.h"
#include <conio.h>

/* Wait helper using VSync for timing */
static void wait_frames(uint8_t count) {
    uint8_t i;
    for (i = 0; i < count; ++i) {
        waitvsync();
    }
}

void ui_show_scan_range(void) {
    uint8_t i;
    uint8_t y;
    uint8_t num_unlocked = 0;
    uint8_t choice;
    uint8_t rate;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== SELECT IP RANGE ==", COLOR_CYAN);

    y = 5;
    for (i = 0; i < MAX_RANGES; ++i) {
        if (range_unlocked[i]) {
            ++num_unlocked;
            /* Number label */
            screen_set_char(2, y, '0' + num_unlocked, COLOR_WHITE);
            screen_set_char(3, y, '.', COLOR_WHITE);

            /* Range name */
            ui_print_string(5, y, ranges[i].name, COLOR_LIGHTBLUE);

            /* Hit rate */
            rate = range_get_hit_rate(i);
            ui_print_string(14, y, "HIT:", COLOR_GRAY2);
            screen_print_number(18, y, rate, 2, COLOR_WHITE);
            screen_set_char(20, y, '%', COLOR_WHITE);

            /* Risk */
            ui_print_string(22, y, "RSK:", COLOR_GRAY2);
            screen_print_number(26, y, ranges[i].risk_per_turn, 1, COLOR_RED);

            /* Creds tag */
            if (range_has_creds[i]) {
                ui_print_string(29, y, "[CREDS]", COLOR_GREEN);
            }

            /* Bandwidth */
            ui_print_string(29, y + 1, "BW:", COLOR_GRAY2);
            screen_print_number(32, y + 1, ranges[i].bandwidth, 3, COLOR_YELLOW);

            y += 2;
        }
    }

    ui_print_string(2, y + 1, "Q. BACK", COLOR_GRAY2);

    if (num_unlocked == 0) {
        ui_print_centered(12, "NO RANGES AVAILABLE", COLOR_RED);
        input_wait_key();
        game.state = STATE_PLAYING;
        return;
    }

    choice = input_read_menu(num_unlocked);

    if (choice == 255) {
        game.state = STATE_PLAYING;
        return;
    }

    /* Map choice back to range index */
    {
        uint8_t count = 0;
        for (i = 0; i < MAX_RANGES; ++i) {
            if (range_unlocked[i]) {
                if (count == choice) {
                    game.selected_range = i;
                    break;
                }
                ++count;
            }
        }
    }

    gamestate_use_action();
    game.state = STATE_SCAN_ANIM;
}

void ui_show_scan_anim(void) {
    uint8_t i;
    uint8_t y;
    uint8_t range_id = game.selected_range;
    uint8_t hit_rate;
    uint8_t roll;
    uint8_t found = 0;
    uint8_t o3, o4;
    uint8_t angle = 0;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== SCANNING ==", COLOR_CYAN);
    ui_print_string(2, 4, "RANGE: ", COLOR_GRAY2);
    ui_print_string(9, 4, ranges[range_id].name, COLOR_LIGHTBLUE);

    hit_rate = range_get_hit_rate(range_id);

    /* Show 10 scan attempts with animation */
    y = 6;
    for (i = 0; i < 10; ++i) {
        o3 = random_range(1, 255);
        o4 = random_range(1, 255);

        /* Print IP being scanned */
        ui_print_ip(2, y, range_id, o3, o4, COLOR_GRAY2);

        /* Sweep sound */
        sound_play_radar_sweep(angle);
        angle = (angle + 2) & 15;

        /* Delay for animation */
        wait_frames(8);

        /* Roll for success */
        roll = random_range(0, 100);

        if (roll < hit_rate && game.active_box_count < MAX_BOXES) {
            /* SUCCESS - create box */
            uint8_t idx = box_create(range_id);
            if (idx != 255) {
                /* Override generated octets with displayed ones */
                boxes[idx].octet3 = o3;
                boxes[idx].octet4 = o4;
                ui_print_string(20, y, "  [OPEN!]", COLOR_GREEN);
                ++found;
                sound_play_click();
            } else {
                ui_print_string(20, y, "  [FULL]", COLOR_YELLOW);
            }
        } else if (roll < hit_rate + 15) {
            ui_print_string(20, y, "  TIMEOUT", COLOR_GRAY2);
        } else if (roll < hit_rate + 40) {
            ui_print_string(20, y, "  CLOSED", COLOR_RED);
        } else {
            ui_print_string(20, y, "  FAIL", COLOR_GRAY1);
        }

        ++y;
        if (y >= 17) break; /* Stay within screen */
    }

    sound_silence();

    game.last_scan_found = found;

    /* Brief pause before result */
    wait_frames(30);

    game.state = STATE_SCAN_RESULT;
}

void ui_show_scan_result(void) {
    uint8_t found = game.last_scan_found;

    ui_clear_body();
    ui_render_hud();

    ui_print_centered(3, "== SCAN COMPLETE ==", COLOR_CYAN);

    if (found > 0) {
        ui_print_string(5, 8, "FOUND ", COLOR_WHITE);
        screen_print_number(11, 8, found, 1, COLOR_GREEN);
        ui_print_string(13, 8, " NEW BOX", COLOR_WHITE);
        if (found > 1) {
            ui_print_string(21, 8, "ES", COLOR_WHITE);
        }
        ui_print_string(23, 8, "!", COLOR_WHITE);

        ui_print_string(5, 10, "TOTAL ACTIVE: ", COLOR_GRAY2);
        screen_print_number(19, 10, game.active_box_count, 2, COLOR_WHITE);
        ui_print_string(21, 10, "/", COLOR_GRAY2);
        screen_print_number(22, 10, MAX_BOXES, 2, COLOR_GRAY2);
    } else {
        ui_print_centered(8, "NO VULNERABLE HOSTS FOUND.", COLOR_YELLOW);
        ui_print_centered(10, "TRY ANOTHER RANGE OR", COLOR_GRAY2);
        ui_print_centered(11, "CRACK CREDENTIALS FOR 3X HIT.", COLOR_GRAY2);
    }

    ui_print_centered(20, "PRESS ANY KEY", COLOR_GRAY2);
    input_wait_key();
}
