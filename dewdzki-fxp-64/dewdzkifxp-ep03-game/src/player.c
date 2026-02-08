#include "player.h"
#include "sprite.h"
#include "sprite_data.h"
#include <peekpoke.h>
#include <conio.h>

/* Sprite data location for player shield */
#define PLAYER_SPRITE_LOC (SPRITE_DATA_BASE)
#define PLAYER_SPRITE_NUM 0

/* Movement speed (pixels per frame) */
#define PLAYER_SPEED 3

/* Screen bounds for sprite coordinates */
#define PLAYER_MIN_X 24
#define PLAYER_MAX_X 320
#define PLAYER_MIN_Y 58    /* Below HUD */
#define PLAYER_MAX_Y 240

uint16_t player_x;
uint8_t player_y;

void player_init(void) {
    /* Load shield sprite */
    sprite_load(PLAYER_SPRITE_NUM, sprite_shield, PLAYER_SPRITE_LOC);
    sprite_set_color(PLAYER_SPRITE_NUM, COLOR_YELLOW);
    sprite_set_multicolor(PLAYER_SPRITE_NUM, 0);

    /* Start position: center, above server to intercept packets */
    player_x = 172;
    player_y = 100;

    sprite_set_position(PLAYER_SPRITE_NUM, player_x, player_y);
    sprite_enable(PLAYER_SPRITE_NUM, 1);
}

void player_update(void) {
    uint8_t joy;

    /* Read both joystick ports, OR together
     * $DC00 = joy port 2, $DC01 = joy port 1
     * Active low: invert and mask 5 bits */
    joy = (~PEEK(0xDC00) & 0x1F) | (~PEEK(0xDC01) & 0x1F);

    /* Bit 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT */
    if (joy & 0x01) {  /* UP */
        if (player_y > PLAYER_MIN_Y + PLAYER_SPEED) {
            player_y -= PLAYER_SPEED;
        } else {
            player_y = PLAYER_MIN_Y;
        }
    }
    if (joy & 0x02) {  /* DOWN */
        if (player_y < PLAYER_MAX_Y - PLAYER_SPEED) {
            player_y += PLAYER_SPEED;
        } else {
            player_y = PLAYER_MAX_Y;
        }
    }
    if (joy & 0x04) {  /* LEFT */
        if (player_x > PLAYER_MIN_X + PLAYER_SPEED) {
            player_x -= PLAYER_SPEED;
        } else {
            player_x = PLAYER_MIN_X;
        }
    }
    if (joy & 0x08) {  /* RIGHT */
        if (player_x < PLAYER_MAX_X - PLAYER_SPEED) {
            player_x += PLAYER_SPEED;
        } else {
            player_x = PLAYER_MAX_X;
        }
    }

    sprite_set_position(PLAYER_SPRITE_NUM, player_x, player_y);
}

void player_disable(void) {
    sprite_enable(PLAYER_SPRITE_NUM, 0);
}
