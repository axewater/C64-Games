#ifndef BULLET_H
#define BULLET_H

#include <stdint.h>

#define MAX_BULLETS 1

// Direction constants
#define DIR_NONE 0
#define DIR_UP 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define DIR_RIGHT 4
#define DIR_UP_LEFT 5
#define DIR_UP_RIGHT 6
#define DIR_DOWN_LEFT 7
#define DIR_DOWN_RIGHT 8

// Bullet sprite configuration
#define BULLET_SPRITE_NUM 7
#define BULLET_SPRITE_LOC 0x3100
#define BULLET_SPRITE_PTR (BULLET_SPRITE_LOC / 64)

typedef struct {
    uint16_t x;  // Pixel coordinates
    uint16_t y;
    int8_t dx;   // Velocity
    int8_t dy;
    uint8_t active;
} Bullet;

extern Bullet bullets[MAX_BULLETS];

void bullet_init(void);
void bullet_spawn(uint16_t x, uint16_t y, uint8_t direction);
void bullet_update_all(void);
void bullet_render_all(void);

#endif
