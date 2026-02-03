#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>

/* VIC-II Sprite Registers */
#define VIC_SPRITE_ENABLE  ((uint8_t*)0xD015)
#define VIC_SPRITE_COLOR   ((uint8_t*)0xD027)
#define VIC_SPRITE_MCOLOR  ((uint8_t*)0xD01C)
#define VIC_SPRITE_EXPAND_X ((uint8_t*)0xD01D)
#define VIC_SPRITE_EXPAND_Y ((uint8_t*)0xD017)
#define VIC_SPRITE_PRI     ((uint8_t*)0xD01B)
#define VIC_SPRITE_MSB_X   ((uint8_t*)0xD010)

/* Sprite X/Y position registers (sprites 0-7) */
#define VIC_SPRITE0_X  ((uint8_t*)0xD000)
#define VIC_SPRITE0_Y  ((uint8_t*)0xD001)

/* Screen sprite pointer location */
#define SPRITE_PTRS ((uint8_t*)0x07F8)

/* Sprite data location in memory */
#define SPRITE_DATA_LOC 0x0340

void sprite_init(void);
void sprite_enable(uint8_t sprite_num, uint8_t enabled);
void sprite_set_position(uint8_t sprite_num, uint16_t x, uint8_t y);
void sprite_set_color(uint8_t sprite_num, uint8_t color);
void sprite_set_multicolor(uint8_t sprite_num, uint8_t enabled);

#endif
