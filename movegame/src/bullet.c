#include "bullet.h"
#include "screen.h"

#define BULLET_SPEED 4

Bullet bullets[MAX_BULLETS];

void bullet_init(void) {
    uint8_t i;
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
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
            }

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
            }
        }
    }
}

void bullet_render_all(void) {
    uint8_t i;
    uint8_t char_x, char_y;

    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // Convert pixel coordinates to character grid
            char_x = (bullets[i].x - 24) / 8;
            char_y = (bullets[i].y - 50) / 8;

            // Render bullet character
            if (char_x < 40 && char_y < 25) {
                screen_set_char(char_x, char_y, '*', COLOR_YELLOW);
            }
        }
    }
}
