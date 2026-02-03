#include "bullet.h"
#include "screen.h"
#include "sprite.h"

#define BULLET_SPEED 4

/* Bullet sprite: small diamond/circle shape - 63 bytes */
const uint8_t bullet_sprite_data[63] = {
    0x00, 0x00, 0x00,  /* Row 1-2: empty */
    0x00, 0x00, 0x00,
    0x00, 0x60, 0x00,  /* Row 3: start shape */
    0x00, 0xF0, 0x00,
    0x01, 0xF8, 0x00,  /* Row 5: widening */
    0x03, 0xFC, 0x00,
    0x07, 0xFE, 0x00,  /* Row 7: widest */
    0x07, 0xFE, 0x00,
    0x03, 0xFC, 0x00,  /* Row 9: narrowing */
    0x01, 0xF8, 0x00,
    0x00, 0xF0, 0x00,  /* Row 11: end shape */
    0x00, 0x60, 0x00,
    0x00, 0x00, 0x00,  /* Rows 13-21: empty */
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00
};

/* Load bullet sprite data to memory */
static void sprite_load_bullet_data(void) {
    uint8_t i;
    uint8_t* sprite_mem = (uint8_t*)BULLET_SPRITE_LOC;

    for (i = 0; i < 63; i++) {
        sprite_mem[i] = bullet_sprite_data[i];
    }
}

Bullet bullets[MAX_BULLETS];

void bullet_init(void) {
    uint8_t i;

    /* Clear all bullets */
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }

    /* Load bullet sprite data to memory */
    sprite_load_bullet_data();

    /* Set sprite pointer for bullet sprite */
    SPRITE_PTRS[BULLET_SPRITE_NUM] = BULLET_SPRITE_PTR;

    /* Set bullet sprite color to yellow */
    sprite_set_color(BULLET_SPRITE_NUM, COLOR_YELLOW);

    /* Initially disabled (no bullet active) */
    sprite_enable(BULLET_SPRITE_NUM, 0);
}

void bullet_spawn(uint16_t x, uint16_t y, uint8_t direction) {
    uint8_t i;

    // Find free slot
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].active = 1;

            // Set velocity based on direction
            bullets[i].dx = 0;
            bullets[i].dy = 0;

            switch (direction) {
                case DIR_UP:
                    bullets[i].dy = -BULLET_SPEED;
                    break;
                case DIR_DOWN:
                    bullets[i].dy = BULLET_SPEED;
                    break;
                case DIR_LEFT:
                    bullets[i].dx = -BULLET_SPEED;
                    break;
                case DIR_RIGHT:
                    bullets[i].dx = BULLET_SPEED;
                    break;
                case DIR_UP_LEFT:
                    bullets[i].dx = -3;
                    bullets[i].dy = -3;
                    break;
                case DIR_UP_RIGHT:
                    bullets[i].dx = 3;
                    bullets[i].dy = -3;
                    break;
                case DIR_DOWN_LEFT:
                    bullets[i].dx = -3;
                    bullets[i].dy = 3;
                    break;
                case DIR_DOWN_RIGHT:
                    bullets[i].dx = 3;
                    bullets[i].dy = 3;
                    break;
            }

            /* Enable bullet sprite */
            sprite_enable(BULLET_SPRITE_NUM, 1);

            return;
        }
    }
}

void bullet_update_all(void) {
    uint8_t i;

    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // Update position
            bullets[i].x += bullets[i].dx;
            bullets[i].y += bullets[i].dy;

            // Check bounds (screen area: 24-320 x, 50-229 y)
            if (bullets[i].x < 24 || bullets[i].x > 320 ||
                bullets[i].y < 50 || bullets[i].y > 229) {
                bullets[i].active = 0;
                sprite_enable(BULLET_SPRITE_NUM, 0);
            }
        }
    }
}

void bullet_render_all(void) {
    uint8_t i;

    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            /* Set sprite position directly (no grid conversion needed) */
            sprite_set_position(BULLET_SPRITE_NUM, bullets[i].x, bullets[i].y);
        }
    }
}
