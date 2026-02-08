#include "background.h"
#include "screen.h"
#include <conio.h>

/* Draw network grid PETSCII background
 * Dark blue dots/lines on black, representing a network topology */
void background_draw(void) {
    uint8_t x, y;

    /* Black background */
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    screen_clear();

    /* Draw grid dots every 4 chars, starting at row 2 (below HUD) */
    for (y = 3; y < 24; y += 4) {
        for (x = 0; x < 40; x += 4) {
            screen_set_char(x, y, 0x51, COLOR_BLUE);  /* Circle/dot */
        }
    }

    /* Draw horizontal connection lines between dots */
    for (y = 3; y < 24; y += 4) {
        for (x = 1; x < 39; x++) {
            if (x % 4 != 0) {
                screen_set_char(x, y, 0xC0, COLOR_BLUE);  /* Horizontal line */
            }
        }
    }

    /* Draw vertical connection lines between dot rows */
    for (x = 0; x < 40; x += 4) {
        for (y = 4; y < 23; y++) {
            if (y % 4 != 3) {
                screen_set_char(x, y, 0x42, COLOR_BLUE);  /* Vertical line */
            }
        }
    }
}
