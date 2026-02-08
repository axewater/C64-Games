#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdint.h>

/* Game states */
#define STATE_SPLASH         0
#define STATE_MENU           1
#define STATE_HELP           2
#define STATE_PLAYING        3
#define STATE_SCAN_RANGE     4
#define STATE_SCAN_ANIM      5
#define STATE_SCAN_RESULT    6
#define STATE_BOX_LIST       7
#define STATE_BOX_DETAIL     8
#define STATE_INSTALL_SVC    9
#define STATE_CRED_DUMP     10
#define STATE_CRED_STATUS   11
#define STATE_PROXY_POST    12
#define STATE_TOOL_UNLOCK   13
#define STATE_DISCOVERY     14
#define STATE_EVENT         15
#define STATE_STATS         16
#define STATE_SHOP          17
#define STATE_GAME_WON      18
#define STATE_CONFIRM_QUIT  19
#define STATE_QUIT          20

/* Pool sizes */
#define MAX_BOXES     24
#define MAX_RANGES     6
#define MAX_CRED_JOBS  4

/* Service bitmasks */
#define SVC_NONE    0x00
#define SVC_FTP     0x01
#define SVC_SCANNER 0x02
#define SVC_PROXY   0x04

/* Tool bitmasks */
#define TOOL_XSCAN      0x01
#define TOOL_SERVU      0x02
#define TOOL_LOPHTCRACK 0x04
#define TOOL_SCANNER    0x08
#define TOOL_PROXY      0x10
#define TOOL_HACKDB     0x20
#define TOOL_ALL        0x3F

/* Base actions per turn */
#define BASE_ACTIONS 3

/* Game state structure */
typedef struct {
    uint8_t state;
    uint16_t turn;
    uint8_t actions_remaining;
    uint8_t actions_per_turn;
    uint16_t reputation;
    uint8_t tools_unlocked;       /* bitmask */
    uint8_t total_boxes_ever;
    uint8_t active_box_count;
    uint8_t ftp_count;
    uint8_t scanner_count;
    uint8_t proxy_count;
    uint8_t creds_cracked_total;
    uint8_t selected_range;
    uint8_t selected_box;
    uint8_t current_event;        /* 0=none, 1-8=event type */
    uint8_t event_value;
    uint8_t event_turns_left;
    uint8_t event_range;
    uint8_t last_discovered_box;  /* 255=none */
    uint8_t discovery_count;      /* how many discovered this turn */
    uint8_t pending_tool;         /* 0=none, else tool bit */
    uint8_t last_scan_found;      /* boxes found in last scan */
} GameState;

/* Global game state */
extern GameState game;

/* Initialize game state to defaults */
void gamestate_init(void);

/* Start a new game */
void gamestate_start_game(void);

/* Use one action point */
void gamestate_use_action(void);

/* End turn - reset actions, advance turn counter */
void gamestate_end_turn(void);

/* Add reputation (capped at 9999) */
void gamestate_add_reputation(uint16_t rep);

/* Spend reputation (returns 1 on success, 0 if can't afford) */
uint8_t gamestate_spend_rep(uint16_t cost);

/* Recount service totals from box pool */
void gamestate_recount(void);

#endif /* GAMESTATE_H */
