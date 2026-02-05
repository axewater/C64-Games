#ifndef INTRO_H
#define INTRO_H

#include <stdint.h>

/* ===== TIMING CONFIGURATION ===== */
/* Adjust these constants to tune animation speeds */
#define TYPING_DELAY 3        /* VSync frames per character typed */
#define WALK_SPEED 2          /* VSync frames per walk animation step */
#define BLINK_CYCLE 30        /* Frames for cursor blink cycle */
#define SPINNER_SPEED 8       /* Frames per spinner rotation frame */
#define SLIDE_SPEED 1         /* VSync frames per slide animation step */

/* Sprite data for person (walking character) */
extern const uint8_t sprite_person[63];

/* Sprite data for card (rectangular card for insertion) */
extern const uint8_t sprite_card[63];

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

/* ===== ANIMATION HELPERS ===== */
/* Wait specified number of VSync frames */
void wait_frames(uint8_t count);

/* Slide sprite smoothly from (start_x, start_y) to (end_x, end_y) over specified frames */
void sprite_slide_to(uint8_t sprite_num, uint16_t start_x, uint8_t start_y,
                     uint16_t end_x, uint8_t end_y, uint8_t frames);

#endif /* INTRO_H */
