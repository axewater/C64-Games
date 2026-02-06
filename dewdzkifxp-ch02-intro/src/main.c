#include "screen.h"
#include <conio.h>

int main(void) {
    /* Initialize screen */
    screen_init();
    screen_clear();

    /* Display Chapter 2 title */
    screen_print_centered(10, "CHAPTER 2", COLOR_CYAN);
    screen_print_centered(11, "=========", COLOR_CYAN);

    screen_print_centered(13, "THE RISE", COLOR_WHITE);

    /* Instruction */
    screen_print_centered(16, "PRESS ANY KEY TO CONTINUE", COLOR_YELLOW);

    /* Wait for key press */
    while (!kbhit()) {
        /* Wait */
    }
    cgetc();

    return 0;
}
