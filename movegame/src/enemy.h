#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>

#define MAX_ENEMIES 6

// Enemy types
#define ENEMY_TYPE_BASIC 0
#define ENEMY_TYPE_FAST 1
#define ENEMY_TYPE_TANK 2

typedef struct {
    uint16_t x;
    uint16_t y;
    int8_t dx;
    int8_t dy;
    uint8_t sprite_num;  // Sprite 1-7
    uint8_t active;
    uint8_t type;        // BASIC, FAST, or TANK
    uint8_t hp;
} Enemy;

extern Enemy enemies[MAX_ENEMIES];

void enemy_init(void);
void enemy_spawn(uint8_t type);
void enemy_update_all(uint16_t player_x, uint16_t player_y);
void enemy_hit(uint8_t index);
uint8_t enemy_count_active(void);
void enemy_clear_all(void);
void enemy_spawn_wave(void);

#endif
