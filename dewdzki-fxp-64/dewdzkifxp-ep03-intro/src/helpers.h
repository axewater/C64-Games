#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>

/* Wait for specified number of frames (50 fps PAL) */
void wait_frames(uint8_t count);

/* Fade in sprite at position */
void sprite_fade_in(uint8_t sprite_num, uint16_t x, uint8_t y);

/* Wait for key or check for quit
 * Returns: 0 = continue, 1 = quit (Q pressed) */
uint8_t wait_for_key_or_quit(void);

/* Typewriter text effect */
void type_text(uint8_t x, uint8_t y, const char *text, uint8_t color);

/* Type IRC line with nick and message */
void type_irc_line(uint8_t y, const char *nick, const char *msg, uint8_t nick_color, uint8_t msg_color);

#endif /* HELPERS_H */
