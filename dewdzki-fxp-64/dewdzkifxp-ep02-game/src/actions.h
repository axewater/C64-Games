#ifndef ACTIONS_H
#define ACTIONS_H

#include <stdint.h>

/* Handle the hub menu (STATE_PLAYING) */
void handle_playing_state(void);

/* Individual action handlers */
void action_scan(void);
void action_manage_boxes(void);
void action_cred_dump(void);
void action_proxy_post(void);
void action_view_stats(void);
void action_shop(void);

#endif /* ACTIONS_H */
