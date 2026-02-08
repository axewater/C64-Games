#ifndef BOSS_H
#define BOSS_H

#include <stdint.h>

/* Boss position (top-left corner of 2x2 formation) */
extern uint16_t boss_x;
extern uint8_t boss_y;

/* Initialize boss sprites (loads 4 sprites in 2x2 grid) */
void boss_init(void);

/* Update boss movement (clockwise around screen edge) */
void boss_update(void);

/* Boss attack: spawn a packet aimed at server
 * Returns 1 if packet was spawned, 0 if not (no free slot or cooldown) */
uint8_t boss_attack(void);

/* Disable all boss sprites */
void boss_cleanup(void);

#endif /* BOSS_H */
