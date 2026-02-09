#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>

/* VIC-II Registers */
#define VIC_SPRITE0_X ((uint8_t*)0xD000)
#define VIC_SPRITE0_Y ((uint8_t*)0xD001)
#define VIC_SPRITE_MSB_X ((uint8_t*)0xD010)
#define VIC_SPRITE_ENABLE ((uint8_t*)0xD015)
#define VIC_SPRITE_EXPAND_X ((uint8_t*)0xD01D)
#define VIC_SPRITE_EXPAND_Y ((uint8_t*)0xD017)
#define VIC_SPRITE_PRI ((uint8_t*)0xD01B)
#define VIC_SPRITE_MCOLOR ((uint8_t*)0xD01C)
#define VIC_SPRITE_MC1 ((uint8_t*)0xD025)
#define VIC_SPRITE_MC2 ((uint8_t*)0xD026)
#define VIC_SPRITE_COLOR ((uint8_t*)0xD027)

/* Sprite pointers in screen memory */
#define SPRITE_PTRS ((uint8_t*)0x07F8)

/* Sprite data memory locations (must be above program end, 64-byte aligned)
 * BSS ends at ~$3E86, so base must be $3EC0+. 5 slots fit: $3EC0-$3FFF.
 * Layout: player(0), server(1), shared_packet(2), boss_tl(3), boss_tr(4).
 * Boss BL/BR use low memory ($0380/$0800) to stay in VIC bank 0.
 * Explosion data goes to $0340 (cassette buffer). */
#define SPRITE_DATA_BASE 0x3EC0
#define SPRITE_DATA_SIZE 64

/* Sprite IDs */
#define SPRITE_CURSOR 0
#define SPRITE_LEWIS 1
#define SPRITE_FTP 2

/* Initialize sprite system */
void sprite_init(void);

/* Load sprite data to memory */
void sprite_load(uint8_t sprite_num, const uint8_t* data, uint16_t location);

/* Enable/disable sprite */
void sprite_enable(uint8_t sprite_num, uint8_t enabled);

/* Set sprite position (x can be 0-511, y can be 0-255) */
void sprite_set_position(uint8_t sprite_num, uint16_t x, uint8_t y);

/* Set sprite color (0-15) */
void sprite_set_color(uint8_t sprite_num, uint8_t color);

/* Set sprite expand Y */
void sprite_set_expand_y(uint8_t sprite_num, uint8_t enabled);

/* Set sprite multicolor mode */
void sprite_set_multicolor(uint8_t sprite_num, uint8_t enabled);

/* Set shared multicolor registers */
void sprite_set_multicolor_shared(uint8_t mc1, uint8_t mc2);

#endif /* SPRITE_H */
