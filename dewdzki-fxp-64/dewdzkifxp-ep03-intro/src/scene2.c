#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include <conio.h>

/* Scene 2: MS Blaster Worm News */
uint8_t scene2_blaster_news(void) {
    uint8_t blink;

    clrscr();

    /* IRC header */
    textcolor(COLOR_YELLOW);
    gotoxy(12, 1);
    cputs("#DEWDZKI-FXP");
    screen_draw_hline(0, 2, 40, 0xC0, COLOR_GRAY1);

    wait_frames(20);

    /* IRC conversation */
    type_irc_line(4, "SICK0", "URGENT - MS BLASTER WORM", COLOR_RED, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(6, "AXE", "WHAT HAPPENED?", COLOR_CYAN, COLOR_WHITE);
    wait_frames(25);

    type_irc_line(8, "SICK0", "WORM SPREADING VIA DCOM", COLOR_RED, COLOR_WHITE);
    wait_frames(30);  /* Pause for impact */
    type_irc_line(9, "SICK0", "MILLIONS OF BOXES INFECTED", COLOR_RED, COLOR_WHITE);
    wait_frames(50);  /* Dramatic pause - massive scale */

    type_irc_line(11, "ZZZ", "MICROSOFT EMERGENCY PATCHES", COLOR_LIGHTRED, COLOR_WHITE);
    wait_frames(35);

    type_irc_line(13, "AXE", "THIS CHANGES EVERYTHING", COLOR_CYAN, COLOR_WHITE);
    wait_frames(50);  /* Dramatic pause - key realization */

    type_irc_line(15, "RIZZ", "SYSADMINS ARE PANICKING", COLOR_YELLOW, COLOR_WHITE);
    wait_frames(40);

    /* Flash warning */
    for (blink = 0; blink < 4; blink++) {
        textcolor(COLOR_RED);
        gotoxy(4, 20);
        cputs("*** 2002: THE INTERNET LOCKDOWN ***");
        wait_frames(15);

        gotoxy(4, 20);
        cputs("                                    ");
        wait_frames(15);
    }

    textcolor(COLOR_RED);
    gotoxy(4, 20);
    cputs("*** 2002: THE INTERNET LOCKDOWN ***");
    wait_frames(30);

    /* Prompt */
    textcolor(COLOR_YELLOW);
    gotoxy(8, 24);
    cputs("PRESS ANY KEY");

    return wait_for_key_or_quit();
}
