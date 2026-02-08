/* sound.c - Simple SID sound effects using direct SID chip programming */

#include "sound.h"
#include <stdint.h>

/* SID chip registers */
#define SID_BASE        0xD400

/* Voice 1 registers */
#define SID_V1_FREQ_LO  (*(uint8_t*)(SID_BASE + 0x00))
#define SID_V1_FREQ_HI  (*(uint8_t*)(SID_BASE + 0x01))
#define SID_V1_PW_LO    (*(uint8_t*)(SID_BASE + 0x02))
#define SID_V1_PW_HI    (*(uint8_t*)(SID_BASE + 0x03))
#define SID_V1_CONTROL  (*(uint8_t*)(SID_BASE + 0x04))
#define SID_V1_AD       (*(uint8_t*)(SID_BASE + 0x05))
#define SID_V1_SR       (*(uint8_t*)(SID_BASE + 0x06))

/* Global registers */
#define SID_VOLUME      (*(uint8_t*)(SID_BASE + 0x18))

/* Control register bits */
#define SID_GATE        0x01
#define SID_SYNC        0x02
#define SID_RING        0x04
#define SID_TEST        0x08
#define SID_TRIANGLE    0x10
#define SID_SAWTOOTH    0x20
#define SID_PULSE       0x40
#define SID_NOISE       0x80

/* Frequency values (approximations for different notes) */
#define FREQ_CLICK_HI   0x28  /* ~1800 Hz - high click */
#define FREQ_CLICK_LO   0x1C  /* ~1000 Hz - high byte */
#define FREQ_BACK_HI    0x0A  /* ~200 Hz - low thump */
#define FREQ_BACK_LO    0x00

/* Radar sweep frequency range (sweeps from low to high and back) */
#define FREQ_RADAR_MIN  0x10  /* Base frequency */
#define FREQ_RADAR_STEP 0x03  /* Step per angle */

void sound_init(void) {
    uint8_t i;

    /* Clear all SID registers */
    for (i = 0; i < 25; i++) {
        (*(uint8_t*)(SID_BASE + i)) = 0;
    }

    /* Set master volume to maximum */
    SID_VOLUME = 15;

    /* Configure Voice 1 ADSR for short clicks */
    SID_V1_AD = 0x00;  /* Fast attack (0), fast decay (0) */
    SID_V1_SR = 0xF0;  /* No sustain, fast release */

    /* Set pulse width to 50% */
    SID_V1_PW_LO = 0x00;
    SID_V1_PW_HI = 0x08;
}

void sound_play_click(void) {
    uint16_t i;

    /* Set frequency for high click */
    SID_V1_FREQ_LO = FREQ_CLICK_LO;
    SID_V1_FREQ_HI = FREQ_CLICK_HI;

    /* Use pulse wave for sharper click */
    SID_V1_CONTROL = SID_PULSE | SID_GATE;

    /* Very short delay */
    for (i = 0; i < 500; i++) {
        /* Wait */
    }

    /* Release note */
    SID_V1_CONTROL = SID_PULSE;
}

void sound_play_back(void) {
    uint16_t i;

    /* Set frequency for low thump */
    SID_V1_FREQ_LO = FREQ_BACK_LO;
    SID_V1_FREQ_HI = FREQ_BACK_HI;

    /* Use triangle wave for deeper sound */
    SID_V1_CONTROL = SID_TRIANGLE | SID_GATE;

    /* Slightly longer delay for thump */
    for (i = 0; i < 800; i++) {
        /* Wait */
    }

    /* Release note */
    SID_V1_CONTROL = SID_TRIANGLE;
}

void sound_play_radar_sweep(uint8_t angle) {
    uint8_t freq_hi;

    /* Calculate frequency based on angle (0-15)
     * Creates a rising sweep for first half (0-7), falling for second half (8-15)
     * This gives the classic radar "ping" sweep sound */
    if (angle < 8) {
        /* Rising sweep */
        freq_hi = FREQ_RADAR_MIN + (angle * FREQ_RADAR_STEP);
    } else {
        /* Falling sweep */
        freq_hi = FREQ_RADAR_MIN + ((15 - angle) * FREQ_RADAR_STEP);
    }

    /* Set frequency */
    SID_V1_FREQ_LO = 0x00;
    SID_V1_FREQ_HI = freq_hi;

    /* Use sawtooth wave for classic radar tone */
    /* Gate bit stays on to maintain continuous tone */
    SID_V1_CONTROL = SID_SAWTOOTH | SID_GATE;
}

void sound_silence(void) {
    /* Clear control register to stop all sound */
    SID_V1_CONTROL = 0;

    /* Clear frequency */
    SID_V1_FREQ_LO = 0;
    SID_V1_FREQ_HI = 0;
}
