#include "collision.h"
#include "packet.h"
#include "player.h"
#include <peekpoke.h>

/* Server center position (must match game.c) */
#define SERVER_X 172
#define SERVER_Y 140

/* Collision radius (in pixels) for proximity check.
 * Shield is 24px wide and 6px tall; packet diamond is ~8px */
#define PLAYER_COLL_X 14
#define PLAYER_COLL_Y 8
#define SERVER_COLL_DIST 12

/* Simple absolute value */
static uint16_t abs16(int16_t v) {
    return (v < 0) ? (uint16_t)(-v) : (uint16_t)v;
}

uint8_t collision_check(void) {
    uint8_t damage = 0;
    uint8_t i;
    int16_t dx, dy;

    for (i = 0; i < MAX_PACKETS; i++) {
        if (!packets[i].active) continue;

        /* Check collision with player shield (sprite 0) */
        dx = (int16_t)packets[i].x - (int16_t)player_x;
        dy = (int16_t)packets[i].y - (int16_t)player_y;

        if (abs16(dx) < PLAYER_COLL_X && abs16(dy) < PLAYER_COLL_Y) {
            packet_deflect(i);
            continue;
        }

        /* Check collision with server (sprite 1) */
        dx = (int16_t)packets[i].x - SERVER_X;
        dy = (int16_t)packets[i].y - SERVER_Y;

        if (abs16(dx) < SERVER_COLL_DIST && abs16(dy) < SERVER_COLL_DIST) {
            packet_deactivate(i);
            damage++;
        }
    }

    return damage;
}
