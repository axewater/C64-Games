#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include <conio.h>
#include <peekpoke.h>

/* Scene 10: "FIN" - Title card and THE END */
uint8_t scene10_fin(void) {
    uint8_t blink_state = 0;
    uint16_t frames = 0;

    clrscr();
    bordercolor(COLOR_BLUE);

    /* Dramatic reveal with pauses */
    wait_frames(30);

    screen_print_centered(5, "- - - - - - - - - - -", COLOR_GRAY2);
    wait_frames(20);

    screen_print_centered(7, "DEWDZKI FXP-64", COLOR_WHITE);
    wait_frames(25);

    screen_print_centered(9, "A THREE-CHAPTER STORY", COLOR_CYAN);
    wait_frames(20);

    screen_print_centered(11, "ABOUT GROWING UP", COLOR_CYAN);
    wait_frames(20);

    screen_print_centered(13, "ON THE INTERNET", COLOR_CYAN);
    wait_frames(25);

    screen_print_centered(15, "- - - - - - - - - - -", COLOR_GRAY2);
    wait_frames(30);

    screen_print_centered(17, "CREATED FOR THE COMMODORE 64", COLOR_GRAY1);
    wait_frames(20);

    screen_print_centered(18, "2026", COLOR_GRAY1);
    wait_frames(20);

    screen_print_centered(19, "ALL RIGHTS FUCKED UP", COLOR_GRAY2);
    wait_frames(30);

    /* Type "THE END" with typewriter effect */
    type_text(15, 20, "- THE END -", COLOR_WHITE);

    wait_frames(30);

    /* Border fade: BLUE -> GRAY2 -> GRAY1 -> BLACK */
    POKE(0xD020, COLOR_GRAY2);
    wait_frames(20);
    POKE(0xD020, COLOR_GRAY1);
    wait_frames(20);
    POKE(0xD020, COLOR_BLACK);

    wait_frames(20);

    /* Blinking "PRESS ANY KEY TO EXIT" prompt */
    while (1) {
        if (blink_state) {
            screen_print_centered(23, "PRESS ANY KEY TO EXIT", COLOR_YELLOW);
        } else {
            screen_print_centered(23, "                     ", COLOR_BLACK);
        }

        if (kbhit()) {
            cgetc();
            return 0;
        }

        frames++;
        if (frames >= 30) {
            frames = 0;
            blink_state = !blink_state;
        }

        wait_frames(1);
    }
}
