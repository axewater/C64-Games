#include "screen.h"
#include <string.h>

uint16_t screen_offset(uint8_t x, uint8_t y) {
    return (uint16_t)y * SCREEN_WIDTH + x;
}

void screen_init(void) {
    /* Use conio functions for proper initialization */
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLUE);
    clrscr();
    textcolor(COLOR_WHITE);
}

void screen_clear(void) {
    uint16_t i;

    /* Fill screen with spaces */
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        SCREEN_MEM[i] = ' ';
        COLOR_MEM[i] = COLOR_WHITE;
    }
}

void screen_set_char(uint8_t x, uint8_t y, uint8_t ch, uint8_t color) {
    uint16_t offset;

    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
        return;
    }

    offset = screen_offset(x, y);
    SCREEN_MEM[offset] = ch;
    COLOR_MEM[offset] = color;
}

void screen_print_number(uint8_t x, uint8_t y, uint16_t num, uint8_t digits, uint8_t color) {
    uint8_t i;
    uint16_t divisor = 1;
    uint8_t digit;

    /* Calculate divisor for most significant digit */
    for (i = 1; i < digits; i++) {
        divisor *= 10;
    }

    /* Print each digit */
    for (i = 0; i < digits; i++) {
        digit = (num / divisor) % 10;
        screen_set_char(x + i, y, '0' + digit, color);
        divisor /= 10;
    }
}
