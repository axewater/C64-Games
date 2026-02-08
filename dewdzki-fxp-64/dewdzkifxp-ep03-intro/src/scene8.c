#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include <conio.h>

/* Scene 8: Success Counter */
uint8_t scene8_success_counter(void) {
    uint8_t result;
    const uint16_t counts[] = {1, 5, 15, 47, 128, 342, 847};
    uint8_t i;

    clrscr();

    /* IRC header */
    textcolor(COLOR_YELLOW);
    gotoxy(11, 1);
    cputs("#DEWDZKI-BOTS");
    screen_draw_hline(0, 2, 40, 0xC0, COLOR_GRAY1);

    wait_frames(20);

    /* Bot connection messages scrolling */
    textcolor(COLOR_GREEN);
    gotoxy(2, 4);
    cputs("[BOT-001] 203.12.45.78 ONLINE");
    wait_frames(12);

    gotoxy(2, 5);
    cputs("[BOT-002] 195.88.23.145 ONLINE");
    wait_frames(12);

    gotoxy(2, 6);
    cputs("[BOT-003] 84.122.67.201 ONLINE");
    wait_frames(12);

    gotoxy(2, 7);
    cputs("[BOT-004] 217.45.99.12 ONLINE");
    wait_frames(12);

    gotoxy(2, 8);
    cputs("[BOT-005] 195.201.34.89 ONLINE");
    wait_frames(12);

    gotoxy(2, 9);
    cputs("[BOT-006] 88.156.23.67 ONLINE");
    wait_frames(12);

    gotoxy(2, 10);
    cputs("[BOT-007] 203.78.45.123 ONLINE");
    wait_frames(12);

    gotoxy(2, 11);
    cputs("[BOT-008] 142.88.67.234 ONLINE");
    wait_frames(12);

    gotoxy(2, 12);
    cputs("[BOT-009] 78.134.56.78 ONLINE");
    wait_frames(12);

    gotoxy(2, 13);
    cputs("[BOT-010] 195.34.89.145 ONLINE");
    wait_frames(20);

    textcolor(COLOR_GRAY2);
    gotoxy(2, 14);
    cputs("...");
    wait_frames(30);

    /* Big counter animation */
    for (i = 0; i < 7; i++) {
        textcolor(COLOR_LIGHTGREEN);
        gotoxy(11, 16);
        cprintf("TOTAL BOTS: %u", counts[i]);
        wait_frames(i < 4 ? 30 : 20);  /* Slightly slower for impact */
    }

    wait_frames(50);  /* Dramatic pause before reactions */

    /* IRC reactions */
    type_irc_line(19, "AXE", "ITS WORKING", COLOR_CYAN, COLOR_WHITE);
    wait_frames(40);  /* Dramatic pause - victory moment */

    type_irc_line(20, "SICK0", "WE'RE BACK IN BUSINESS", COLOR_RED, COLOR_WHITE);
    wait_frames(40);  /* Dramatic pause - triumph */

    type_irc_line(21, "RIZZ", "800 BOTS AND COUNTING", COLOR_YELLOW, COLOR_WHITE);
    wait_frames(60);  /* Dramatic pause - final celebration */

    /* Prompt */
    textcolor(COLOR_YELLOW);
    gotoxy(8, 24);
    cputs("PRESS ANY KEY");

    result = wait_for_key_or_quit();

    return result;
}
