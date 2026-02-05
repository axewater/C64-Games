#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdint.h>

/* Game states */
#define STATE_SPLASH    0
#define STATE_MENU      1
#define STATE_PLAYING   2
#define STATE_SCAN      3
#define STATE_TOPSITE   4
#define STATE_FXP       5
#define STATE_FORUM     6
#define STATE_RANK_UP   7
#define STATE_GAME_OVER 8

/* Game state structure */
typedef struct {
    uint8_t state;              /* Current game state (0-8) */
    uint8_t rank;               /* 0-5: LEECH to ELITE */
    uint16_t reputation;        /* 0-9999 */
    uint8_t bandwidth;          /* 10-100 MB/s */
    uint8_t actions_remaining;  /* 3 per session */
    uint16_t turn;              /* Turn counter */
    uint8_t ftps_known_count;   /* Discovered public FTPs */
    uint8_t posts_active_count; /* Active forum posts */
    uint8_t current_topsite;    /* Unlocked topsite index */
    uint8_t current_forum;      /* Unlocked forum index */
} GameState;

/* Global game state */
extern GameState game_state;

/* Initialize game state */
void gamestate_init(void);

/* Start new game */
void gamestate_start_game(void);

/* Advance turn by N turns */
void gamestate_advance_turn(uint8_t turns);

/* Check and handle rank progression */
void gamestate_check_rank_up(void);

/* Add reputation points */
void gamestate_add_reputation(uint16_t rep);

#endif /* GAMESTATE_H */
