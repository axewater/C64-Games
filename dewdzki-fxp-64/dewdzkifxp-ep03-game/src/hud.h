#ifndef HUD_H
#define HUD_H

#include <stdint.h>

/* Draw the full HUD (health, level name, timer) */
void hud_draw(uint8_t health, const char* level_name, uint8_t timer_secs);

/* Update just the health display */
void hud_update_health(uint8_t health);

/* Update just the timer display */
void hud_update_timer(uint8_t timer_secs);

#endif /* HUD_H */
