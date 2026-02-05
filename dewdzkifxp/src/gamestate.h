#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdint.h>

/* Game states */
#define STATE_SPLASH        0
#define STATE_MENU          1
#define STATE_PLAYING       2
#define STATE_SCAN          3
#define STATE_TOPSITE_LIST  4  /* NEW: Select topsite to browse */
#define STATE_TOPSITE       5  /* Browse releases on topsite */
#define STATE_FTP_SELECT    6  /* NEW: Select FTP for FXP */
#define STATE_FXP           7  /* FXP transfer animation */
#define STATE_RELEASE_SELECT 8 /* NEW: Select release to post */
#define STATE_FORUM         9  /* Post to forum */
#define STATE_RANK_UP       10 /* Rank up celebration */
#define STATE_GAME_OVER     11 /* Win/loss screen */
#define STATE_VIEW_STATS    12 /* View detailed post stats */
#define STATE_VIEW_FTPS     13 /* View FTP servers and releases */
#define STATE_HARDWARE_SHOP 14 /* Hardware shop */
#define STATE_RAID_ALERT    15 /* Raid notification screen */
#define STATE_EVENT         16 /* Random event screen */

/* Game state structure */
typedef struct {
    uint8_t state;               /* Current game state (0-11) */
    uint8_t rank;                /* 0-5: LEECH to ELITE */
    uint16_t reputation;         /* 0-9999 */
    uint16_t bandwidth;          /* Bandwidth in KB/s */
    uint8_t actions_remaining;   /* 3 per session */
    uint16_t turn;               /* Turn counter */
    uint8_t ftps_known_count;    /* Discovered public FTPs */
    uint8_t posts_active_count;  /* Active forum posts */
    uint8_t current_topsite;     /* Unlocked topsite index */
    uint8_t current_forum;       /* Unlocked forum index */
    uint8_t hardware_tier;       /* 0-5: hardware upgrade level */

    /* Multi-step selection state (NEW) */
    uint8_t selected_topsite_idx;  /* For topsite browsing */
    uint8_t selected_release_idx;  /* For FXP/forum posting */
    uint8_t selected_ftp_idx;      /* For FXP target selection */

    /* Raid tracking (NEW) */
    uint8_t last_raid_ftp_idx;     /* 255 = no raid this turn */
    uint16_t last_raid_downloads;  /* Downloads on raided FTP */
    uint8_t last_raid_turn;        /* Turn when raid occurred */
    uint8_t total_raids;           /* Total raids throughout game */

    /* Event tracking (NEW) */
    uint8_t current_event;         /* 0 = none, 1-10 = event type */
    uint8_t event_value;           /* Event-specific parameter */
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

/* Check if player can use an action */
uint8_t gamestate_can_use_action(void);

/* Use one action (decrements and resets at 0) */
void gamestate_use_action(void);

#endif /* GAMESTATE_H */
