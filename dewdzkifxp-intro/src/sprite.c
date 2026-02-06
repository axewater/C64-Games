#include "sprite.h"

void sprite_init(void) {
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

void sprite_load(uint8_t sprite_num, const uint8_t* data, uint16_t location) {
    uint8_t i;
    uint8_t* sprite_mem = (uint8_t*)location;

    /* Copy sprite data to memory */
    for (i = 0; i < 63; i++) {
        sprite_mem[i] = data[i];
    }

    /* Set sprite pointer for this sprite */
    SPRITE_PTRS[sprite_num] = location / 64;
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

void sprite_set_expand_y(uint8_t sprite_num, uint8_t enabled) {
    if (sprite_num > 7) return;

    if (enabled) {
        *VIC_SPRITE_EXPAND_Y |= (1 << sprite_num);
    } else {
        *VIC_SPRITE_EXPAND_Y &= ~(1 << sprite_num);
    }
}

void sprite_set_multicolor(uint8_t sprite_num, uint8_t enabled) {
    if (sprite_num > 7) return;

    if (enabled) {
        *VIC_SPRITE_MCOLOR |= (1 << sprite_num);
    } else {
        *VIC_SPRITE_MCOLOR &= ~(1 << sprite_num);
    }
}

void sprite_set_multicolor_shared(uint8_t mc1, uint8_t mc2) {
    *VIC_SPRITE_MC1 = mc1;
    *VIC_SPRITE_MC2 = mc2;
}
