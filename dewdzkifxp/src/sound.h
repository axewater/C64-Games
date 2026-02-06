/* sound.h - Simple SID sound effects */

#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

/* Initialize sound system */
void sound_init(void);

/* Play menu selection click (short, high-pitched) */
void sound_play_click(void);

/* Play back/cancel sound (lower, thumpy) */
void sound_play_back(void);

/* Silence all voices */
void sound_silence(void);

#endif /* SOUND_H */
