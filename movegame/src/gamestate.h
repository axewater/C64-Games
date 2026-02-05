#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdint.h>

// Game states
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_GAME_OVER 2

// Game state struct
typedef struct {
    uint8_t state;
    uint16_t score;
    uint8_t lives;
    uint8_t wave;
    uint8_t enemies_killed_this_wave;
    uint8_t enemies_to_spawn;
    uint8_t current_room;
} GameState;

// Global game state
extern GameState game_state;

// Functions
void gamestate_init(void);
void gamestate_start_game(void);
void gamestate_game_over(void);
void gamestate_next_wave(void);
void gamestate_add_score(uint16_t points);
void gamestate_lose_life(void);
void gamestate_render_ui(void);
void gamestate_render_game_over(void);

#endif
