#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

/* Player shield position */
extern uint16_t player_x;
extern uint8_t player_y;

/* Initialize player sprite */
void player_init(void);

/* Read joystick input and update position */
void player_update(void);

/* Disable player sprite */
void player_disable(void);

#endif /* PLAYER_H */
