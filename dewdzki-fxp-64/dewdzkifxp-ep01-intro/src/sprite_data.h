#ifndef SPRITE_DATA_H
#define SPRITE_DATA_H

#include <stdint.h>

/* Sprite data for person (walking character) - 3 frame animation */
extern const uint8_t sprite_person_frame1[63];  /* Left leg forward */
extern const uint8_t sprite_person_frame2[63];  /* Standing */
extern const uint8_t sprite_person_frame3[63];  /* Right leg forward */

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

/* Sprite data for radar hand (4-frame rotation animation) */
extern const uint8_t sprite_radar_frame1[63];  /* Vertical | */
extern const uint8_t sprite_radar_frame2[63];  /* Diagonal \ */
extern const uint8_t sprite_radar_frame3[63];  /* Horizontal - */
extern const uint8_t sprite_radar_frame4[63];  /* Diagonal / */

/* Sprite data for radar blip (diamond dot) */
extern const uint8_t sprite_dot[63];

#endif /* SPRITE_DATA_H */
