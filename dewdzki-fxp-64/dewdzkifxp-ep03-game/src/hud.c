#include "hud.h"
#include "screen.h"
#include <conio.h>

/* Draw the full HUD at rows 0-1 */
void hud_draw(uint8_t health, const char* level_name, uint8_t timer_secs) {
    /* Row 0: Level name centered */
    screen_draw_hline(0, 0, 40, ' ', COLOR_BLACK);
    screen_print_centered(0, level_name, COLOR_YELLOW);

    /* Row 1: Health left, Timer right */
    screen_draw_hline(0, 1, 40, ' ', COLOR_BLACK);
    hud_update_health(health);
    hud_update_timer(timer_secs);

    /* Divider line */
    screen_draw_hline(0, 2, 40, 0xC0, COLOR_GRAY1);
}

/* Update just the health display (row 1, left side) */
void hud_update_health(uint8_t health) {
    uint8_t i;

    screen_print_string(0, 1, "SRV:", COLOR_GREEN);
    for (i = 0; i < 3; i++) {
        if (i < health) {
            screen_set_char(4 + i, 1, 0x51, COLOR_GREEN);  /* Filled circle */
        } else {
            screen_set_char(4 + i, 1, 0x57, COLOR_RED);    /* Empty circle */
        }
    }
}

/* Update just the timer display (row 1, right side) */
void hud_update_timer(uint8_t timer_secs) {
    uint8_t tens, ones;

    tens = timer_secs / 10;
    ones = timer_secs % 10;

    screen_print_string(34, 1, "T:", COLOR_GRAY2);
    screen_set_char(36, 1, '0' + tens, COLOR_WHITE);
    screen_set_char(37, 1, '0' + ones, COLOR_WHITE);
    screen_set_char(38, 1, 'S', COLOR_GRAY2);
}
