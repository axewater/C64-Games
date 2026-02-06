#include "input.h"
#include "sound.h"
#include <conio.h>

void input_init(void) {
    /* Keyboard only - no initialization needed */
}

uint8_t input_read_menu(uint8_t num_options) {
    uint8_t key;
    uint8_t max_key;

    /* Handle edge case: no options available */
    if (num_options == 0) {
        /* Only allow quit */
        while (1) {
            if (kbhit()) {
                key = cgetc();
                if (key == 'q' || key == 'Q') {
                    sound_play_back();  /* Play back/cancel sound */
                    return 255;
                }
            }
        }
    }

    /* Cap at 9 options maximum (keys '1' to '9') */
    if (num_options > 9) {
        num_options = 9;
    }

    max_key = '0' + num_options;  /* '1' for 1 option, '9' for 9 options */

    while (1) {
        if (kbhit()) {
            key = cgetc();

            /* Check for number keys 1-N */
            if (key >= '1' && key <= max_key) {
                sound_play_click();  /* Play menu selection sound */
                return key - '1';  /* Return 0-based index */
            }

            /* Check for quit */
            if (key == 'q' || key == 'Q') {
                sound_play_back();  /* Play back/cancel sound */
                return 255;  /* Special value for quit */
            }
        }
    }
}

void input_wait_key(void) {
    /* Clear any pending input */
    while (kbhit()) {
        cgetc();
    }

    /* Wait for new key press */
    while (!kbhit()) {
        /* Busy wait */
    }

    /* Consume the key */
    cgetc();
}

uint8_t input_check_quit(void) {
    uint8_t key;

    if (kbhit()) {
        key = cgetc();
        if (key == 'q' || key == 'Q') {
            return 1;
        }
    }

    return 0;
}
