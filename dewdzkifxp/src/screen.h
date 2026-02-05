#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <conio.h>

#define SCREEN_MEM ((uint8_t*)0x0400)
#define COLOR_MEM  ((uint8_t*)0xD800)
#define SCREEN_WIDTH  40
#define SCREEN_HEIGHT 25

void screen_init(void);
void screen_clear(void);
void screen_set_char(uint8_t x, uint8_t y, uint8_t ch, uint8_t color);
uint16_t screen_offset(uint8_t x, uint8_t y);
void screen_print_number(uint8_t x, uint8_t y, uint16_t num, uint8_t digits, uint8_t color);

#endif
