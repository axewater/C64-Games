#include "intro.h"
#include "screen.h"
#include <conio.h>

int main(void) {
    /* Run the intro sequence */
    intro_run();

    /* Clear screen and show ending message */
    screen_clear();
    screen_print_centered(12, "INTRO COMPLETE", COLOR_GREEN);
    screen_print_centered(14, "PRESS ANY KEY TO EXIT", COLOR_WHITE);

    /* Wait for key */
    while (!kbhit()) {
        /* Wait */
    }
    cgetc();

    return 0;
}
