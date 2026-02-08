#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>

/* Scene 6: RIZZ Codes the Bot */
uint8_t scene6_rizz_codes(void) {
    uint8_t result;

    clrscr();

    /* Header */
    textcolor(COLOR_CYAN);
    gotoxy(7, 1);
    cputs("BOT DEVELOPMENT WORKSPACE");
    screen_draw_hline(0, 2, 40, 0xC0, COLOR_GRAY1);

    wait_frames(20);

    /* RIZZ sprite */
    sprite_load(0, sprite_rizz, SPRITE_DATA_BASE);
    sprite_set_color(0, COLOR_YELLOW);
    sprite_set_multicolor(0, 1);
    sprite_fade_in(0, 160, 160);

    wait_frames(20);

    /* Code typing effect */
    textcolor(COLOR_GREEN);
    gotoxy(2, 5);
    cputs("// DEWDZKI BOT v1.0");
    wait_frames(15);
    gotoxy(2, 6);
    cputs("// C&C VIA IRC");
    wait_frames(20);

    gotoxy(2, 8);
    cputs("connect(\"irc.efnet.org\")");
    wait_frames(15);
    gotoxy(2, 9);
    cputs("join(\"#dewdzki-bots\")");
    wait_frames(20);

    gotoxy(2, 11);
    cputs("on_connect:");
    wait_frames(15);
    gotoxy(4, 12);
    cputs("send_info(ip, os, uptime)");
    wait_frames(15);
    gotoxy(4, 13);
    cputs("await_commands()");
    wait_frames(25);

    gotoxy(2, 15);
    cputs("commands:");
    wait_frames(15);
    gotoxy(4, 16);
    cputs("!exec <cmd>  - run shell cmd");
    wait_frames(15);
    gotoxy(4, 17);
    cputs("!count      - show bot count");
    wait_frames(15);
    gotoxy(4, 18);
    cputs("!dl <url>   - download file");
    wait_frames(30);

    /* Bottom text */
    screen_draw_hline(0, 20, 40, 0xC0, COLOR_GRAY1);

    textcolor(COLOR_YELLOW);
    gotoxy(2, 22);
    cputs("RIZZ BUILDS THE C&C SYSTEM");
    wait_frames(20);

    textcolor(COLOR_WHITE);
    gotoxy(2, 23);
    cputs("IRC-BASED COMMAND & CONTROL");
    wait_frames(15);

    gotoxy(2, 24);
    cputs("CENTRALIZED BOTNET MANAGEMENT");
    wait_frames(40);

    /* Prompt */
    textcolor(COLOR_YELLOW);
    gotoxy(8, 26);
    cputs("PRESS ANY KEY");

    result = wait_for_key_or_quit();

    /* Disable sprite */
    sprite_enable(0, 0);

    return result;
}
