#include "input.h"
#include <joystick.h>
#include <conio.h>

static uint8_t joy_available = 0;

uint8_t input_init(void) {
    /* Install the static standard joystick driver for C64 */
    if (joy_install(joy_static_stddrv) == JOY_ERR_OK) {
        joy_available = 1;
        return 1;
    }
    /* Fallback to keyboard-only if joystick fails */
    joy_available = 0;
    return 0;
}

void input_read(InputState *state) {
    uint8_t joy;
    uint8_t key;

    /* Reset state */
    state->dx = 0;
    state->dy = 0;
    state->quit = 0;
    state->fire = 0;

    /* Read joystick if available */
    if (joy_available) {
        joy = joy_read(JOY_1);

        /* Check directional inputs */
        if (JOY_UP(joy)) {
            state->dy = -2;
        }
        if (JOY_DOWN(joy)) {
            state->dy = 2;
        }
        if (JOY_LEFT(joy)) {
            state->dx = -2;
        }
        if (JOY_RIGHT(joy)) {
            state->dx = 2;
        }

        /* Check fire button */
        if (JOY_BTN_1(joy)) {
            state->fire = 1;
        }
    }

    /* Read keyboard (overrides joystick if both pressed) */
    if (kbhit()) {
        key = cgetc();

        switch (key) {
            case 'w':
            case 'W':
                state->dy = -2;
                break;

            case 's':
            case 'S':
                state->dy = 2;
                break;

            case 'a':
            case 'A':
                state->dx = -2;
                break;

            case 'd':
            case 'D':
                state->dx = 2;
                break;

            case 'q':
            case 'Q':
                state->quit = 1;
                break;

            case ' ':
                state->fire = 1;
                break;
        }
    }
}

void input_cleanup(void) {
    if (joy_available) {
        joy_uninstall();
    }
}
