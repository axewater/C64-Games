#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <conio.h>

/* Screen dimensions */
#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25

/* Screen memory pointers */
#define SCREEN_MEM ((uint8_t*)0x0400)
#define COLOR_MEM ((uint8_t*)0xD800)

/* Initialize screen */
void screen_init(void);

/* Clear screen */
void screen_clear(void);

/* Set a single character at position */
void screen_set_char(uint8_t x, uint8_t y, uint8_t ch, uint8_t color);

/* Print a string at position */
void screen_print_string(uint8_t x, uint8_t y, const char* str, uint8_t color);

/* Print a string centered on a row */
void screen_print_centered(uint8_t y, const char* str, uint8_t color);

/* Draw horizontal line */
void screen_draw_hline(uint8_t x, uint8_t y, uint8_t width, uint8_t ch, uint8_t color);

/* Draw vertical line */
void screen_draw_vline(uint8_t x, uint8_t y, uint8_t height, uint8_t ch, uint8_t color);

/* Draw box */
void screen_draw_box(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);

#endif /* SCREEN_H */
