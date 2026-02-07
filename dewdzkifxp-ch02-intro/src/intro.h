#ifndef INTRO_H
#define INTRO_H

#include <stdint.h>

/* ===== TIMING CONFIGURATION ===== */
#define TYPING_DELAY 3        /* VSync frames per character typed */
#define WALK_SPEED 2          /* VSync frames per walk animation step */
#define BLINK_CYCLE 30        /* Frames for cursor blink cycle */
#define SPINNER_SPEED 8       /* Frames per spinner rotation frame */
#define SLIDE_SPEED 1         /* VSync frames per slide animation step */

/* ===== SINE WAVE SCAN CONSTANTS ===== */
#define SINE_TABLE_SIZE 32    /* Number of entries in sine lookup table */
#define SCAN_AREA_X 2         /* Left column of scan area */
#define SCAN_AREA_Y 7         /* Top row of scan area */
#define SCAN_AREA_W 28        /* Width of scan area in columns */
#define SCAN_AREA_H 8         /* Height of scan area in rows */

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
