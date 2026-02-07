#include "screen.h"
#include <string.h>

void screen_init(void) {
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLUE);
    clrscr();
    textcolor(COLOR_WHITE);
}

void screen_clear(void) {
    uint16_t i;

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

    offset = y * SCREEN_WIDTH + x;
    SCREEN_MEM[offset] = ch;
    COLOR_MEM[offset] = color;
}

void screen_print_string(uint8_t x, uint8_t y, const char* str, uint8_t color) {
    uint16_t offset;
    uint8_t i;

    offset = y * SCREEN_WIDTH + x;
    i = 0;

    while (str[i] != '\0' && x + i < SCREEN_WIDTH) {
        SCREEN_MEM[offset + i] = str[i];
        COLOR_MEM[offset + i] = color;
        i++;
    }
}

void screen_print_centered(uint8_t y, const char* str, uint8_t color) {
    uint8_t len;
    uint8_t x;

    len = strlen(str);
    if (len > SCREEN_WIDTH) {
        len = SCREEN_WIDTH;
    }

    x = (SCREEN_WIDTH - len) / 2;
    screen_print_string(x, y, str, color);
}

void screen_draw_hline(uint8_t x, uint8_t y, uint8_t width, uint8_t ch, uint8_t color) {
    uint8_t i;

    for (i = 0; i < width && x + i < SCREEN_WIDTH; i++) {
        screen_set_char(x + i, y, ch, color);
    }
}

void screen_draw_vline(uint8_t x, uint8_t y, uint8_t height, uint8_t ch, uint8_t color) {
    uint8_t i;

    for (i = 0; i < height && y + i < SCREEN_HEIGHT; i++) {
        screen_set_char(x, y + i, ch, color);
    }
}

void screen_draw_box(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
    /* Top and bottom */
    screen_draw_hline(x, y, width, '-', color);
    screen_draw_hline(x, y + height - 1, width, '-', color);

    /* Left and right */
    screen_draw_vline(x, y, height, '|', color);
    screen_draw_vline(x + width - 1, y, height, '|', color);

    /* Corners */
    screen_set_char(x, y, '+', color);
    screen_set_char(x + width - 1, y, '+', color);
    screen_set_char(x, y + height - 1, '+', color);
    screen_set_char(x + width - 1, y + height - 1, '+', color);
}
