#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include <conio.h>

/* Scene 3: The Lockdown */
uint8_t scene3_lockdown(void) {
    clrscr();

    /* Scanner section */
    textcolor(COLOR_CYAN);
    gotoxy(12, 1);
    cputs("PORT SCANNER v4.0");

    wait_frames(20);

    textcolor(COLOR_WHITE);
    gotoxy(2, 3);
    cputs("SCANNING 139.178.45.0/24...");
    wait_frames(30);

    /* Port scan results - all blocked */
    textcolor(COLOR_GRAY2);
    gotoxy(2, 5);
    cputs("139.178.45.12:139");
    textcolor(COLOR_RED);
    cputs(" FILTERED");
    wait_frames(20);

    textcolor(COLOR_GRAY2);
    gotoxy(2, 6);
    cputs("139.178.45.34:445");
    textcolor(COLOR_RED);
    cputs(" BLOCKED");
    wait_frames(20);

    textcolor(COLOR_GRAY2);
    gotoxy(2, 7);
    cputs("139.178.45.67:139");
    textcolor(COLOR_RED);
    cputs(" FILTERED");
    wait_frames(20);

    textcolor(COLOR_GRAY2);
    gotoxy(2, 8);
    cputs("139.178.45.89:445");
    textcolor(COLOR_RED);
    cputs(" BLOCKED");
    wait_frames(30);

    /* ISP notice box */
    textcolor(COLOR_RED);
    gotoxy(2, 10);
    cputs("+----------------------------------+");
    gotoxy(2, 11);
    cputs("| ISP SECURITY NOTICE:            |");
    gotoxy(2, 12);
    cputs("| PORTS 139/445 NOW BLOCKED       |");
    gotoxy(2, 13);
    cputs("| EFFECTIVE IMMEDIATELY           |");
    gotoxy(2, 14);
    cputs("+----------------------------------+");

    wait_frames(60);  /* Dramatic pause - let ISP notice sink in */

    /* IRC section at bottom */
    screen_draw_hline(0, 16, 40, 0xC0, COLOR_GRAY1);

    type_irc_line(18, "SICK0", "EVERYTHING BEHIND FIREWALLS", COLOR_RED, COLOR_WHITE);
    wait_frames(35);  /* Build tension */

    type_irc_line(19, "ZZZ", "NAT EVERYWHERE NOW", COLOR_LIGHTRED, COLOR_WHITE);
    wait_frames(35);  /* Build tension */

    type_irc_line(20, "RIZZ", "OUR SCANS FIND NOTHING", COLOR_YELLOW, COLOR_WHITE);
    wait_frames(60);  /* Dramatic pause - desperation moment */

    /* Prompt */
    textcolor(COLOR_YELLOW);
    gotoxy(8, 24);
    cputs("PRESS ANY KEY");

    return wait_for_key_or_quit();
}
