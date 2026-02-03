#include "collision.h"
#include "bullet.h"
#include "enemy.h"
#include "gamestate.h"
#include "sprite.h"
#include "player.h"
#include <stdlib.h>

#define COLLISION_TOLERANCE 12

uint8_t collision_check_bullet_enemy(Bullet *b, Enemy *e) {
    int16_t dx, dy;

    /* Calculate distance */
    dx = abs((int16_t)b->x - (int16_t)e->x);
    dy = abs((int16_t)b->y - (int16_t)e->y);

    /* Check if within tolerance */
    return (dx < COLLISION_TOLERANCE && dy < COLLISION_TOLERANCE);
}

void collision_check_player_enemies(void) {
    uint8_t collision_reg;
    uint8_t i;
    Player* player;
    int16_t dx, dy;

    /* Read sprite-sprite collision register */
    collision_reg = *VIC_SPRITE_SPRITE_COLL;

    /* Check if player sprite (0) collided with any sprite */
    if (collision_reg & 0x01) {
        /* Player collided with something - verify it's an enemy, not bullet */
        player = player_get();

        /* Check if any active enemy is near the player */
        for (i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].active) {
                /* Calculate distance to this enemy */
                dx = abs((int16_t)player->x - (int16_t)enemies[i].x);
                dy = abs((int16_t)player->y - (int16_t)enemies[i].y);

                /* If enemy is close enough, it's a real collision */
                if (dx < COLLISION_TOLERANCE && dy < COLLISION_TOLERANCE) {
                    gamestate_lose_life();
                    break; /* Only lose one life per collision */
                }
            }
        }

        /* The register will auto-clear on read */
    }
}

void collision_update(void) {
    uint8_t i, j;

    /* Check player-enemy collisions (hardware) */
    collision_check_player_enemies();

    /* Check bullet-enemy collisions (software) */
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].active) {
                    if (collision_check_bullet_enemy(&bullets[i], &enemies[j])) {
                        /* Collision detected */
                        bullets[i].active = 0;
                        sprite_enable(BULLET_SPRITE_NUM, 0);
                        enemy_hit(j);
                        break; // Bullet can only hit one enemy
                    }
                }
            }
        }
    }
}
