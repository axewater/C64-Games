#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include <conio.h>

/* Scene 1: Failed Exploit Attempts */
uint8_t scene1_failed_exploits(void) {
    clrscr();

    /* Header */
    textcolor(COLOR_CYAN);
    gotoxy(10, 2);
    cputs("X-SCAN EXPLOIT v3.0");

    screen_draw_hline(0, 3, 40, 0xC0, COLOR_GRAY1);

    textcolor(COLOR_WHITE);
    gotoxy(2, 5);
    cputs("SCANNING FOR VULNERABILITIES...");
    wait_frames(30);

    /* Show failed exploit attempts */
    textcolor(COLOR_GRAY2);
    gotoxy(2, 7);
    cputs("Apache 2.0 DoS (192.168.1.45)");
    wait_frames(20);
    textcolor(COLOR_RED);
    gotoxy(2, 8);
    cputs(">>> CONNECTION REFUSED");
    wait_frames(30);

    textcolor(COLOR_GRAY2);
    gotoxy(2, 10);
    cputs("DCOM RPC (10.0.0.123)");
    wait_frames(20);
    textcolor(COLOR_RED);
    gotoxy(2, 11);
    cputs(">>> CONNECTION TIMEOUT");
    wait_frames(30);

    textcolor(COLOR_GRAY2);
    gotoxy(2, 13);
    cputs("SMB Share (172.16.0.88)");
    wait_frames(20);
    textcolor(COLOR_RED);
    gotoxy(2, 14);
    cputs(">>> PORT FILTERED");
    wait_frames(30);

    textcolor(COLOR_GRAY2);
    gotoxy(2, 16);
    cputs("IIS WebDAV (203.45.12.67)");
    wait_frames(20);
    textcolor(COLOR_RED);
    gotoxy(2, 17);
    cputs(">>> FIREWALL BLOCKED");
    wait_frames(30);

    /* Stats */
    screen_draw_hline(0, 19, 40, 0xC0, COLOR_GRAY1);
    textcolor(COLOR_RED);
    gotoxy(2, 21);
    cputs("SUCCESS RATE: 2%");
    textcolor(COLOR_GRAY2);
    gotoxy(2, 22);
    cputs("DOWN FROM 18% LAST MONTH");

    wait_frames(40);

    /* Prompt */
    textcolor(COLOR_YELLOW);
    gotoxy(8, 24);
    cputs("PRESS ANY KEY");

    return wait_for_key_or_quit();
}
