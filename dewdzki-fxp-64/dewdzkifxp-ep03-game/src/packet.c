#include "packet.h"
#include "sprite.h"
#include "sprite_data.h"
#include <peekpoke.h>
#include <conio.h>
#include <stdlib.h>

/* Server center position (sprite coords) */
#define SERVER_X 172
#define SERVER_Y 140

/* Screen bounds */
#define SCREEN_LEFT   20
#define SCREEN_RIGHT  340
#define SCREEN_TOP    40
#define SCREEN_BOTTOM 250

/* Sprite data locations for packets (after player=0, server=1) */
#define PACKET_SPRITE_BASE_LOC (SPRITE_DATA_BASE + (2 * SPRITE_DATA_SIZE))
#define PACKET_SPRITE_START 2

Packet packets[MAX_PACKETS];

static uint8_t packet_sprites_loaded = 0;

void packet_init(void) {
    uint8_t i;

    for (i = 0; i < MAX_PACKETS; i++) {
        packets[i].active = 0;
        packets[i].sprite_num = PACKET_SPRITE_START + i;
    }

    /* Load packet sprite data to all 6 sprite slots */
    if (!packet_sprites_loaded) {
        for (i = 0; i < MAX_PACKETS; i++) {
            sprite_load(PACKET_SPRITE_START + i, sprite_packet,
                        PACKET_SPRITE_BASE_LOC + (i * SPRITE_DATA_SIZE));
            sprite_set_color(PACKET_SPRITE_START + i, COLOR_RED);
            sprite_set_multicolor(PACKET_SPRITE_START + i, 0);
        }
        packet_sprites_loaded = 1;
    }
}

/* Calculate dx/dy to aim at server center from spawn position.
 * Uses integer approximation: find the larger axis distance,
 * set that to speed, scale the other proportionally. */
static void aim_at_server(uint16_t sx, uint8_t sy, uint8_t speed,
                          int8_t *out_dx, int8_t *out_dy) {
    int16_t diff_x = (int16_t)SERVER_X - (int16_t)sx;
    int16_t diff_y = (int16_t)SERVER_Y - (int16_t)sy;
    uint16_t abs_x = (diff_x < 0) ? (uint16_t)(-diff_x) : (uint16_t)diff_x;
    uint16_t abs_y = (diff_y < 0) ? (uint16_t)(-diff_y) : (uint16_t)diff_y;

    if (abs_x == 0 && abs_y == 0) {
        *out_dx = 0;
        *out_dy = speed;
        return;
    }

    if (abs_x >= abs_y) {
        /* X is the dominant axis */
        *out_dx = (diff_x > 0) ? speed : -speed;
        /* Scale Y proportionally: dy = speed * abs_y / abs_x */
        *out_dy = (int8_t)((uint16_t)speed * abs_y / abs_x);
        if (diff_y < 0) *out_dy = -(*out_dy);
        /* Ensure at least 1 pixel movement on minor axis if distance > 0 */
        if (*out_dy == 0 && abs_y > 0) {
            *out_dy = (diff_y > 0) ? 1 : -1;
        }
    } else {
        /* Y is the dominant axis */
        *out_dy = (diff_y > 0) ? speed : -speed;
        *out_dx = (int8_t)((uint16_t)speed * abs_x / abs_y);
        if (diff_x < 0) *out_dx = -(*out_dx);
        if (*out_dx == 0 && abs_x > 0) {
            *out_dx = (diff_x > 0) ? 1 : -1;
        }
    }
}

uint8_t packet_spawn(uint8_t edge, uint8_t speed) {
    uint8_t i;
    uint16_t start_x;
    uint8_t start_y;
    int8_t dx, dy;
    uint8_t rnd;

    /* Find free slot */
    for (i = 0; i < MAX_PACKETS; i++) {
        if (!packets[i].active) break;
    }
    if (i >= MAX_PACKETS) return 0xFF;

    rnd = rand() & 0xFF;

    /* Determine spawn position based on edge */
    switch (edge) {
        case EDGE_TOP:
            start_x = 50 + (rnd % 220);
            start_y = SCREEN_TOP;
            break;
        case EDGE_RIGHT:
            start_x = SCREEN_RIGHT;
            start_y = 60 + (rnd % 160);
            break;
        case EDGE_BOTTOM:
            start_x = 50 + (rnd % 220);
            start_y = SCREEN_BOTTOM;
            break;
        case EDGE_LEFT:
        default:
            start_x = SCREEN_LEFT;
            start_y = 60 + (rnd % 160);
            break;
    }

    /* Aim directly at server center */
    aim_at_server(start_x, start_y, speed, &dx, &dy);

    packets[i].x = start_x;
    packets[i].y = start_y;
    packets[i].dx = dx;
    packets[i].dy = dy;
    packets[i].active = 1;

    sprite_set_position(packets[i].sprite_num, start_x, start_y);
    sprite_enable(packets[i].sprite_num, 1);

    return i;
}

void packet_update_all(void) {
    uint8_t i;
    int16_t nx;
    int16_t ny;

    for (i = 0; i < MAX_PACKETS; i++) {
        if (!packets[i].active) continue;

        nx = (int16_t)packets[i].x + packets[i].dx;
        ny = (int16_t)packets[i].y + packets[i].dy;

        /* Deactivate if off-screen */
        if (nx < SCREEN_LEFT || nx > SCREEN_RIGHT ||
            ny < SCREEN_TOP || ny > SCREEN_BOTTOM) {
            packet_deactivate(i);
            continue;
        }

        packets[i].x = (uint16_t)nx;
        packets[i].y = (uint8_t)ny;
        sprite_set_position(packets[i].sprite_num, packets[i].x, packets[i].y);
    }
}

void packet_deactivate(uint8_t index) {
    if (index >= MAX_PACKETS) return;
    packets[index].active = 0;
    sprite_enable(packets[index].sprite_num, 0);
}

void packet_deflect(uint8_t index) {
    if (index >= MAX_PACKETS) return;

    /* Reverse direction */
    packets[index].dx = -packets[index].dx;
    packets[index].dy = -packets[index].dy;

    /* Push away a bit to prevent re-collision */
    packets[index].x = (uint16_t)((int16_t)packets[index].x + packets[index].dx * 3);
    packets[index].y = (uint8_t)((int16_t)packets[index].y + packets[index].dy * 3);
}

void packet_disable_all(void) {
    uint8_t i;
    for (i = 0; i < MAX_PACKETS; i++) {
        packets[i].active = 0;
        sprite_enable(packets[i].sprite_num, 0);
    }
}
