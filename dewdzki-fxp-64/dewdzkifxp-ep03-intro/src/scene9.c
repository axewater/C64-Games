#include "scenes.h"
#include "helpers.h"
#include <conio.h>

/* Scene 9: Title Card */
uint8_t scene9_title(void) {
    uint8_t blink_state = 0;
    uint16_t frames = 0;

    clrscr();

    /* Dramatic reveal with pauses */
    wait_frames(30);

    textcolor(COLOR_GRAY2);
    gotoxy(14, 8);
    cputs("- - - - - - - - - - -");
    wait_frames(15);

    textcolor(COLOR_CYAN);
    gotoxy(15, 10);
    cputs("CHAPTER 3");
    wait_frames(20);

    textcolor(COLOR_WHITE);
    gotoxy(14, 12);
    cputs("THE BOTNET");
    wait_frames(15);

    textcolor(COLOR_GRAY2);
    gotoxy(14, 14);
    cputs("- - - - - - - - - - -");
    wait_frames(30);

    /* Blinking prompt loop */
    while (1) {
        if (blink_state) {
            textcolor(COLOR_YELLOW);
            gotoxy(9, 20);
            cputs("PRESS ANY KEY TO BEGIN");
        } else {
            gotoxy(9, 20);
            cputs("                      ");
        }

        /* Check for keypress */
        if (kbhit()) {
            uint8_t key = cgetc();
            if (key == 'q' || key == 'Q') {
                return 1;  /* Quit */
            }
            return 0;  /* Continue */
        }

        frames++;
        if (frames >= 30) {
            frames = 0;
            blink_state = !blink_state;
        }

        wait_frames(1);
    }
}
