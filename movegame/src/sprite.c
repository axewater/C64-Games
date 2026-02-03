#include "sprite.h"
#include <string.h>

/* Player sprite data - 24x21 pixels = 63 bytes */
/* This is a simple spaceship/player character design */
const uint8_t player_sprite_data[63] = {
    0b00000000, 0b01100000, 0b00000000,  /* Row 1 */
    0b00000000, 0b11110000, 0b00000000,  /* Row 2 */
    0b00000001, 0b11111000, 0b00000000,  /* Row 3 */
    0b00000011, 0b11111100, 0b00000000,  /* Row 4 */
    0b00000011, 0b11111100, 0b00000000,  /* Row 5 */
    0b00000111, 0b11111110, 0b00000000,  /* Row 6 */
    0b00001111, 0b11111111, 0b00000000,  /* Row 7 */
    0b00011111, 0b11111111, 0b10000000,  /* Row 8 */
    0b00111111, 0b11111111, 0b11000000,  /* Row 9 */
    0b01111111, 0b11111111, 0b11100000,  /* Row 10 */
    0b11111111, 0b11111111, 0b11110000,  /* Row 11 */
    0b01111111, 0b11111111, 0b11100000,  /* Row 12 */
    0b00111111, 0b11111111, 0b11000000,  /* Row 13 */
    0b00011111, 0b11111111, 0b10000000,  /* Row 14 */
    0b00001111, 0b11111111, 0b00000000,  /* Row 15 */
    0b00000111, 0b11111110, 0b00000000,  /* Row 16 */
    0b00000011, 0b11111100, 0b00000000,  /* Row 17 */
    0b00000011, 0b11111100, 0b00000000,  /* Row 18 */
    0b00000001, 0b11111000, 0b00000000,  /* Row 19 */
    0b00000000, 0b11110000, 0b00000000,  /* Row 20 */
    0b00000000, 0b01100000, 0b00000000   /* Row 21 */
};

void sprite_init(void) {
    uint8_t i;
    uint8_t* sprite_mem = (uint8_t*)SPRITE_DATA_LOC;

    /* Copy sprite data to memory */
    for (i = 0; i < 63; i++) {
        sprite_mem[i] = player_sprite_data[i];
    }

    /* Set sprite pointer for sprite 0 */
    /* Sprite pointer = sprite_data_address / 64 */
    SPRITE_PTRS[0] = SPRITE_DATA_LOC / 64;

    /* Disable all sprites initially */
    *VIC_SPRITE_ENABLE = 0x00;

    /* Disable multicolor mode for all sprites */
    *VIC_SPRITE_MCOLOR = 0x00;

    /* Disable sprite expansion */
    *VIC_SPRITE_EXPAND_X = 0x00;
    *VIC_SPRITE_EXPAND_Y = 0x00;

    /* Set sprites in front of background */
    *VIC_SPRITE_PRI = 0x00;

    /* Clear X MSB register */
    *VIC_SPRITE_MSB_X = 0x00;
}

void sprite_enable(uint8_t sprite_num, uint8_t enabled) {
    if (sprite_num > 7) return;

    if (enabled) {
        *VIC_SPRITE_ENABLE |= (1 << sprite_num);
    } else {
        *VIC_SPRITE_ENABLE &= ~(1 << sprite_num);
    }
}

void sprite_set_position(uint8_t sprite_num, uint16_t x, uint8_t y) {
    uint8_t* sprite_x;
    uint8_t* sprite_y;

    if (sprite_num > 7) return;

    /* Calculate register addresses */
    sprite_x = VIC_SPRITE0_X + (sprite_num * 2);
    sprite_y = VIC_SPRITE0_Y + (sprite_num * 2);

    /* Set low 8 bits of X coordinate */
    *sprite_x = (uint8_t)(x & 0xFF);

    /* Set MSB (9th bit) of X coordinate if needed */
    if (x > 255) {
        *VIC_SPRITE_MSB_X |= (1 << sprite_num);
    } else {
        *VIC_SPRITE_MSB_X &= ~(1 << sprite_num);
    }

    /* Set Y coordinate */
    *sprite_y = y;
}

void sprite_set_color(uint8_t sprite_num, uint8_t color) {
    if (sprite_num > 7) return;
    VIC_SPRITE_COLOR[sprite_num] = color;
}

void sprite_set_multicolor(uint8_t sprite_num, uint8_t enabled) {
    if (sprite_num > 7) return;

    if (enabled) {
        *VIC_SPRITE_MCOLOR |= (1 << sprite_num);
    } else {
        *VIC_SPRITE_MCOLOR &= ~(1 << sprite_num);
    }
}
