#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>

/* Wait for specified number of frames (50 fps PAL) */
void wait_frames(uint8_t count);

/* Fade in sprite at position */
void sprite_fade_in(uint8_t sprite_num, uint16_t x, uint8_t y);

/* Slide sprite from start to end position over given frames */
void sprite_slide_to(uint8_t sprite_num, uint16_t start_x, uint8_t start_y,
                     uint16_t end_x, uint8_t end_y, uint8_t frames);

/* Wait for key or check for quit
 * Returns: 0 = continue, 1 = quit (Q pressed) */
uint8_t wait_for_key_or_quit(void);

/* Typewriter text effect */
void type_text(uint8_t x, uint8_t y, const char *text, uint8_t color);

/* Type IRC line with nick and message */
void type_irc_line(uint8_t y, const char *nick, const char *msg, uint8_t nick_color, uint8_t msg_color);

/* Draw a progress bar emptying right-to-left */
void reverse_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t delay);

/* Flash border between two colors */
void screen_flash_border(uint8_t color1, uint8_t color2, uint8_t cycles);

/* Print dossier line: label instantly, value with typewriter */
void type_dossier_line(uint8_t y, const char *label, const char *value,
                       uint8_t label_color, uint8_t value_color);

#endif /* HELPERS_H */
