#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

typedef struct {
    uint16_t x;
    uint8_t y;
    uint8_t color;
    uint8_t sprite_num;
    uint8_t last_dir;
} Player;

void player_init(void);
void player_move(int16_t dx, int16_t dy);
void player_update(void);
Player* player_get(void);

#endif
