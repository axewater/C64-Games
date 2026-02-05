#include "input.h"
#include <conio.h>

void input_init(void) {
    /* Keyboard only - no initialization needed */
}

uint8_t input_read_menu(uint8_t num_options) {
    uint8_t key;

    while (1) {
        if (kbhit()) {
            key = cgetc();

            /* Check for number keys 1-9 */
            if (key >= '1' && key <= '0' + num_options) {
                return key - '1';  /* Return 0-based index */
            }

            /* Check for quit */
            if (key == 'q' || key == 'Q') {
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
