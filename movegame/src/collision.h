#ifndef COLLISION_H
#define COLLISION_H

#include <stdint.h>

/* VIC-II Collision Registers */
#define VIC_SPRITE_SPRITE_COLL ((uint8_t*)0xD01E)
#define VIC_SPRITE_BG_COLL     ((uint8_t*)0xD01F)

void collision_update(void);

#endif
