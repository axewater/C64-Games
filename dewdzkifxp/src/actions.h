#ifndef ACTIONS_H
#define ACTIONS_H

#include <stdint.h>
#include "release.h"

/* Temporary release buffer for topsite browsing */
extern Release temp_releases[3];
extern uint8_t selected_release_id;

/* Main hub menu handler */
void handle_playing_state(void);

/* Game action handlers */
void action_scan(void);
void action_browse_topsite(void);
void action_post_to_forum(void);
void action_hardware_shop(void);
void action_move_releases(void);
void action_rep_shop(void);

#endif
