#ifndef INTRO_H
#define INTRO_H

#include <stdint.h>

/* Sprite data for cursor (8x8 block) */
extern const uint8_t sprite_cursor[63];

/* Sprite data for spinner (4-frame animation) */
extern const uint8_t sprite_spinner_frame1[63];
extern const uint8_t sprite_spinner_frame2[63];
extern const uint8_t sprite_spinner_frame3[63];
extern const uint8_t sprite_spinner_frame4[63];

/* Sprite data for arrow */
extern const uint8_t sprite_arrow[63];

/* Sprite data for Lewis avatar (24x21 face with sunglasses) */
extern const uint8_t sprite_lewis[63];

/* Sprite data for FTP server icon */
extern const uint8_t sprite_ftp[63];

/* Initialize intro sequence */
void intro_init(void);

/* Run intro sequence (returns 0 to continue, 1 to quit) */
uint8_t intro_run(void);

#endif /* INTRO_H */
