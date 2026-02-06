#include "intro.h"
#include "screen.h"
#include "chainload.h"
#include <conio.h>

int main(void) {
    /* Run the intro sequence */
    intro_run();

    /* Clear screen and show loading message */
    screen_clear();
    screen_print_centered(12, "LOADING GAME...", COLOR_GREEN);

    /* Small delay for visual feedback */
    {
        unsigned int i;
        for (i = 0; i < 30000; i++) {
            /* Delay loop */
        }
    }

    /* Chain load the main game */
    chainload("game");

    /* If chainload fails, show error and exit */
    screen_clear();
    screen_print_centered(12, "LOAD FAILED", COLOR_RED);
    screen_print_centered(14, "PRESS ANY KEY TO EXIT", COLOR_WHITE);
    while (!kbhit()) {
        /* Wait */
    }
    cgetc();

    return 0;
}
