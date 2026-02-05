#include "enemy.h"
#include "sprite.h"
#include "gamestate.h"
#include "random.h"
#include "room.h"
#include <string.h>

Enemy enemies[MAX_ENEMIES];

/* Enemy sprite data - 3 types */

/* Basic enemy - diamond shape */
const uint8_t enemy_basic_sprite[63] = {
    0b00000000, 0b01100000, 0b00000000,
    0b00000000, 0b11110000, 0b00000000,
    0b00000001, 0b11111000, 0b00000000,
    0b00000011, 0b11111100, 0b00000000,
    0b00000111, 0b11111110, 0b00000000,
    0b00001111, 0b11111111, 0b00000000,
    0b00011111, 0b11111111, 0b10000000,
    0b00111111, 0b11111111, 0b11000000,
    0b01111111, 0b11111111, 0b11100000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b01111111, 0b11111111, 0b11100000,
    0b00111111, 0b11111111, 0b11000000,
    0b00011111, 0b11111111, 0b10000000,
    0b00001111, 0b11111111, 0b00000000,
    0b00000111, 0b11111110, 0b00000000,
    0b00000011, 0b11111100, 0b00000000,
    0b00000001, 0b11111000, 0b00000000,
    0b00000000, 0b11110000, 0b00000000,
    0b00000000, 0b01100000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000
};

/* Fast enemy - elongated aggressive shape */
const uint8_t enemy_fast_sprite[63] = {
    0b00000000, 0b11110000, 0b00000000,
    0b00000001, 0b11111000, 0b00000000,
    0b00000011, 0b11111100, 0b00000000,
    0b00000111, 0b11111110, 0b00000000,
    0b00001111, 0b11111111, 0b00000000,
    0b00011111, 0b11111111, 0b10000000,
    0b00111111, 0b11111111, 0b11000000,
    0b01111111, 0b11111111, 0b11100000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b01111111, 0b11111111, 0b11100000,
    0b00111111, 0b11111111, 0b11000000,
    0b00011111, 0b11111111, 0b10000000,
    0b00001111, 0b11111111, 0b00000000,
    0b00000111, 0b11111110, 0b00000000,
    0b00000011, 0b11111100, 0b00000000,
    0b00000001, 0b11111000, 0b00000000,
    0b00000000, 0b11110000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000
};

/* Tank enemy - wide blocky shape */
const uint8_t enemy_tank_sprite[63] = {
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b01111111, 0b11111111, 0b11100000,
    0b01111111, 0b11111111, 0b11100000,
    0b00111111, 0b11111111, 0b11000000,
    0b00111111, 0b11111111, 0b11000000,
    0b00011111, 0b11111111, 0b10000000,
    0b00011111, 0b11111111, 0b10000000,
    0b00011111, 0b11111111, 0b10000000,
    0b00111111, 0b11111111, 0b11000000,
    0b00111111, 0b11111111, 0b11000000,
    0b01111111, 0b11111111, 0b11100000,
    0b01111111, 0b11111111, 0b11100000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000,
    0b11111111, 0b11111111, 0b11110000
};

void sprite_load_enemy_data(void) {
    uint8_t i;
    uint8_t* sprite_mem;

    /* Load basic enemy sprite at 0x3000 */
    sprite_mem = (uint8_t*)0x3000;
    for (i = 0; i < 63; i++) {
        sprite_mem[i] = enemy_basic_sprite[i];
    }

    /* Load fast enemy sprite at 0x3040 */
    sprite_mem = (uint8_t*)0x3040;
    for (i = 0; i < 63; i++) {
        sprite_mem[i] = enemy_fast_sprite[i];
    }

    /* Load tank enemy sprite at 0x3080 */
    sprite_mem = (uint8_t*)0x3080;
    for (i = 0; i < 63; i++) {
        sprite_mem[i] = enemy_tank_sprite[i];
    }
}

void enemy_init(void) {
    uint8_t i;

    /* Clear all enemies */
    for (i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
    }

    /* Load enemy sprite data */
    sprite_load_enemy_data();
}

void enemy_spawn(uint8_t type, uint16_t player_x, uint16_t player_y) {
    uint8_t i;
    uint8_t edge;
    uint8_t sprite_ptr;
    uint8_t attempts;
    int16_t dx, dy;
    uint16_t dist_sq;

    /* Find free slot */
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = 1;
            enemies[i].type = type;
            enemies[i].sprite_num = i + 1; // Sprites 1-7

            /* Set HP and sprite based on type */
            switch (type) {
                case ENEMY_TYPE_BASIC:
                    enemies[i].hp = 1;
                    sprite_ptr = 0x3000 / 64;
                    sprite_set_color(enemies[i].sprite_num, 2); /* Red */
                    break;
                case ENEMY_TYPE_FAST:
                    enemies[i].hp = 1;
                    sprite_ptr = 0x3040 / 64;
                    sprite_set_color(enemies[i].sprite_num, 8); /* Orange */
                    break;
                case ENEMY_TYPE_TANK:
                    enemies[i].hp = 3;
                    sprite_ptr = 0x3080 / 64;
                    sprite_set_color(enemies[i].sprite_num, 4); /* Purple */
                    break;
                default:
                    enemies[i].hp = 1;
                    sprite_ptr = 0x3000 / 64;
                    sprite_set_color(enemies[i].sprite_num, 2); /* Red */
                    break;
            }

            /* Set sprite pointer */
            SPRITE_PTRS[enemies[i].sprite_num] = sprite_ptr;

            /* Spawn from random edge, ensure minimum distance from player */
            /* Minimum distance: 80 pixels (approximately 1/3 of screen) */
            attempts = 0;
            do {
                edge = random_get() & 3; // 0-3
                switch (edge) {
                    case 0: // Top
                        enemies[i].x = 24 + (random_get() % 296);
                        enemies[i].y = 50;
                        break;
                    case 1: // Bottom
                        enemies[i].x = 24 + (random_get() % 296);
                        enemies[i].y = 229;
                        break;
                    case 2: // Left
                        enemies[i].x = 24;
                        enemies[i].y = 50 + (random_get() % 179);
                        break;
                    case 3: // Right
                        enemies[i].x = 320;
                        enemies[i].y = 50 + (random_get() % 179);
                        break;
                }

                /* Check distance from player */
                dx = (int16_t)enemies[i].x - (int16_t)player_x;
                dy = (int16_t)enemies[i].y - (int16_t)player_y;

                /* Use squared distance to avoid expensive sqrt */
                /* 80*80 = 6400 minimum distance squared */
                if (dx < 0) dx = -dx;
                if (dy < 0) dy = -dy;
                dist_sq = (uint16_t)dx * (uint16_t)dx + (uint16_t)dy * (uint16_t)dy;

                attempts++;
            } while (dist_sq < 6400 && attempts < 10);

            /* Set initial velocity to zero */
            enemies[i].dx = 0;
            enemies[i].dy = 0;

            /* Enable sprite */
            sprite_set_position(enemies[i].sprite_num, enemies[i].x, enemies[i].y);
            sprite_enable(enemies[i].sprite_num, 1);

            return;
        }
    }
}

void enemy_update_all(uint16_t player_x, uint16_t player_y) {
    uint8_t i;
    int16_t dx, dy;
    uint8_t speed;
    uint16_t new_x, new_y;
    uint8_t moved;

    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            /* Calculate direction to player */
            dx = (int16_t)player_x - (int16_t)enemies[i].x;
            dy = (int16_t)player_y - (int16_t)enemies[i].y;

            /* Set speed based on type */
            speed = (enemies[i].type == ENEMY_TYPE_FAST) ? 2 : 1;

            moved = 0;

            /* Try to move toward player, checking for walls */
            /* First, try diagonal movement (both x and y) */
            if (dx != 0 && dy != 0) {
                new_x = enemies[i].x;
                new_y = enemies[i].y;

                if (dx > 0) new_x += speed;
                else if (dx < 0) new_x -= speed;

                if (dy > 0) new_y += speed;
                else if (dy < 0) new_y -= speed;

                /* Check if diagonal move is valid */
                if (!room_check_collision(new_x, new_y)) {
                    enemies[i].x = new_x;
                    enemies[i].y = new_y;
                    moved = 1;
                }
            }

            /* If diagonal failed or not applicable, try moving in one direction */
            if (!moved) {
                /* Try X movement first */
                if (dx != 0) {
                    new_x = enemies[i].x;
                    if (dx > 0) new_x += speed;
                    else new_x -= speed;

                    if (!room_check_collision(new_x, enemies[i].y)) {
                        enemies[i].x = new_x;
                        moved = 1;
                    }
                }

                /* If X movement failed or not needed, try Y movement */
                if (!moved && dy != 0) {
                    new_y = enemies[i].y;
                    if (dy > 0) new_y += speed;
                    else new_y -= speed;

                    if (!room_check_collision(enemies[i].x, new_y)) {
                        enemies[i].y = new_y;
                        moved = 1;
                    }
                }
            }

            /* Update sprite position */
            sprite_set_position(enemies[i].sprite_num, enemies[i].x, enemies[i].y);
        }
    }
}

void enemy_hit(uint8_t index) {
    if (index >= MAX_ENEMIES || !enemies[index].active) {
        return;
    }

    enemies[index].hp--;

    if (enemies[index].hp == 0) {
        /* Enemy destroyed */
        enemies[index].active = 0;
        sprite_enable(enemies[index].sprite_num, 0);

        /* Add score */
        switch (enemies[index].type) {
            case ENEMY_TYPE_BASIC:
                gamestate_add_score(10);
                break;
            case ENEMY_TYPE_FAST:
                gamestate_add_score(20);
                break;
            case ENEMY_TYPE_TANK:
                gamestate_add_score(30);
                break;
        }

        /* Track kills for wave progression */
        game_state.enemies_killed_this_wave++;
    }
}

uint8_t enemy_count_active(void) {
    uint8_t i;
    uint8_t count = 0;

    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            count++;
        }
    }

    return count;
}

void enemy_clear_all(void) {
    uint8_t i;

    /* Clear all enemies */
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].active = 0;
            sprite_enable(enemies[i].sprite_num, 0);
        }
    }
}

void enemy_spawn_wave(uint16_t player_x, uint16_t player_y) {
    uint8_t i;
    uint8_t type;
    uint8_t wave_difficulty;

    /* Determine enemy composition based on wave */
    wave_difficulty = game_state.wave;

    for (i = 0; i < game_state.enemies_to_spawn; i++) {
        /* Choose enemy type based on wave difficulty */
        if (wave_difficulty == 1) {
            /* Wave 1: Only basic enemies */
            type = ENEMY_TYPE_BASIC;
        } else if (wave_difficulty == 2) {
            /* Wave 2: Mix of basic and fast */
            type = (i % 2 == 0) ? ENEMY_TYPE_BASIC : ENEMY_TYPE_FAST;
        } else {
            /* Wave 3+: All types */
            type = i % 3;
        }

        enemy_spawn(type, player_x, player_y);
    }
}
