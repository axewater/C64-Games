#ifndef GAME_H
#define GAME_H

#include <stdint.h>

/* Game states */
#define STATE_TITLE     0
#define STATE_IRC       1
#define STATE_PLAYING   2
#define STATE_BOSS      3
#define STATE_GAMEOVER  4
#define STATE_VICTORY   5
#define STATE_QUIT      6

/* Current game state */
extern uint8_t game_state;
extern uint8_t current_level;  /* 0-2 = regular levels, 3 = boss */
extern uint8_t server_health;

/* Dynamic server position (updated by orbit each frame) */
extern uint16_t server_x;
extern uint8_t server_y;

/* Initialize game systems */
void game_init(void);

/* Run one frame of gameplay (regular levels) */
void game_run_frame(void);

/* Run one frame of boss fight */
void game_run_boss_frame(void);

/* Set up a level (sprites, background, HUD) */
void game_start_level(void);

/* Set up boss fight */
void game_start_boss(void);

/* Frame sync: wait for next jiffy tick (PAL 50fps) */
void sync_frame(void);

#endif /* GAME_H */
