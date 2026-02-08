#include "irc.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>
#include <peekpoke.h>
#include <string.h>

/* Sprite locations for IRC character avatars */
#define IRC_SPRITE_LOC (SPRITE_DATA_BASE)
#define IRC_SPRITE_NUM 0

/* Colors */
#define CLR_NICK_AXE   COLOR_CYAN
#define CLR_NICK_SICK0  COLOR_BROWN
#define CLR_NICK_RIZZ  COLOR_YELLOW
#define CLR_MSG         COLOR_GRAY2
#define CLR_CHAN        COLOR_GREEN

/* Wait for a number of frames */
static void irc_wait(uint8_t frames) {
    uint8_t i;
    for (i = 0; i < frames; i++) {
        while (PEEK(0xD012) != 255);
        while (PEEK(0xD012) == 255);
    }
}

/* Type IRC line with nick and message */
static void irc_type_line(uint8_t y, const char *nick, const char *msg,
                          uint8_t nick_color) {
    uint8_t i, len;

    gotoxy(0, y);
    textcolor(nick_color);
    cprintf("<%s> ", nick);

    textcolor(CLR_MSG);
    len = strlen(msg);
    for (i = 0; i < len; i++) {
        cputc(msg[i]);
        irc_wait(2);
    }
    irc_wait(15);
}

/* Show character sprite for a nick */
static void irc_show_avatar(const char* nick) {
    const uint8_t* data;
    uint8_t color;

    /* Disable previous sprite */
    sprite_enable(IRC_SPRITE_NUM, 0);

    if (nick[0] == 'A') {      /* AXE */
        data = sprite_axe;
        color = COLOR_CYAN;
    } else if (nick[0] == 'S') { /* SICK0 */
        data = sprite_sick0;
        color = COLOR_BROWN;
    } else {                    /* RIZZ */
        data = sprite_rizz;
        color = COLOR_YELLOW;
    }

    sprite_load(IRC_SPRITE_NUM, data, IRC_SPRITE_LOC);
    sprite_set_color(IRC_SPRITE_NUM, color);
    sprite_set_multicolor(IRC_SPRITE_NUM, 1);
    sprite_set_multicolor_shared(COLOR_GRAY2, COLOR_BLACK);
    sprite_set_position(IRC_SPRITE_NUM, 300, 80);
    sprite_enable(IRC_SPRITE_NUM, 1);
}

/* Draw IRC channel header */
static void irc_draw_header(void) {
    screen_init();
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLUE);

    screen_print_centered(0, "#DEWDZKI-BOTS", CLR_CHAN);
    screen_draw_hline(0, 1, 40, 0xC0, COLOR_BLUE);
}

/* Wait for key or check for quit */
static uint8_t irc_wait_key(void) {
    uint8_t key;
    screen_print_centered(23, "PRESS ANY KEY", COLOR_GRAY1);
    while (!kbhit()) {
        irc_wait(1);
    }
    key = cgetc();
    sprite_enable(IRC_SPRITE_NUM, 0);
    return (key == 'q' || key == 'Q') ? 1 : 0;
}

uint8_t irc_show_briefing(uint8_t level_num) {
    irc_draw_header();

    switch (level_num) {
        case 0:  /* Before Level 1: Script Kiddies */
            irc_show_avatar("AXE");
            irc_type_line(3, "AXE", "SCRIPT KIDDIES ARE PORT SCANNING US", CLR_NICK_AXE);
            irc_type_line(5, "AXE", "PROTECT THE C&C SERVER", CLR_NICK_AXE);
            irc_show_avatar("RIZZ");
            irc_type_line(7, "RIZZ", "842 BOTS CONNECTED RIGHT NOW", CLR_NICK_RIZZ);
            irc_type_line(9, "RIZZ", "WE CANT LOSE THIS", CLR_NICK_RIZZ);
            irc_show_avatar("AXE");
            irc_type_line(11, "AXE", "DEFLECT THEIR SCANS. GO.", CLR_NICK_AXE);
            break;

        case 1:  /* Before Level 2: Security Researchers */
            irc_show_avatar("SICK0");
            irc_type_line(3, "SICK0", "SECURITY RESEARCHERS FOUND US", CLR_NICK_SICK0);
            irc_type_line(5, "SICK0", "THEY'RE FASTER AND SMARTER", CLR_NICK_SICK0);
            irc_show_avatar("AXE");
            irc_type_line(7, "AXE", "MULTIPLE ANGLES NOW", CLR_NICK_AXE);
            irc_type_line(9, "AXE", "STAY FOCUSED", CLR_NICK_AXE);
            break;

        case 2:  /* Before Level 3: FBI Cyber Unit */
            irc_show_avatar("RIZZ");
            irc_type_line(3, "RIZZ", "FBI CYBER UNIT IS HERE", CLR_NICK_RIZZ);
            irc_show_avatar("SICK0");
            irc_type_line(5, "SICK0", "WE'RE DONE", CLR_NICK_SICK0);
            irc_show_avatar("AXE");
            irc_type_line(7, "AXE", "NOT YET. HOLD THE LINE.", CLR_NICK_AXE);
            irc_type_line(9, "AXE", "THEY'RE COMING FROM ALL SIDES", CLR_NICK_AXE);
            irc_type_line(11, "AXE", "THIS IS IT.", CLR_NICK_AXE);
            break;

        case 3:  /* Before Boss: THE HONEYPOT */
            irc_show_avatar("RIZZ");
            irc_type_line(3, "RIZZ", "DETECTING MASSIVE SCANNER", CLR_NICK_RIZZ);
            irc_show_avatar("SICK0");
            irc_type_line(5, "SICK0", "WHAT IS THAT THING?", CLR_NICK_SICK0);
            irc_show_avatar("AXE");
            irc_type_line(7, "AXE", "HONEYPOT. THEY BUILT A TRAP.", CLR_NICK_AXE);
            irc_type_line(9, "AXE", "ITS HUGE. FOUR-NODE CLUSTER.", CLR_NICK_AXE);
            irc_type_line(11, "AXE", "LAST STAND. DEFLECT EVERYTHING.", CLR_NICK_AXE);
            break;
    }

    return irc_wait_key();
}

void irc_show_victory(void) {
    irc_draw_header();

    irc_show_avatar("AXE");
    irc_type_line(3, "AXE", "THE HONEYPOT IS DOWN!", CLR_NICK_AXE);
    irc_type_line(5, "AXE", "WE SURVIVED.", CLR_NICK_AXE);
    irc_show_avatar("RIZZ");
    irc_type_line(7, "RIZZ", "842 BOTS STILL ONLINE", CLR_NICK_RIZZ);
    irc_type_line(9, "RIZZ", "THEY NEVER FOUND US", CLR_NICK_RIZZ);
    irc_show_avatar("SICK0");
    irc_type_line(11, "SICK0", "FOR NOW...", CLR_NICK_SICK0);
    irc_show_avatar("AXE");
    irc_type_line(13, "AXE", "TIME TO DISAPPEAR.", CLR_NICK_AXE);

    screen_print_centered(17, "*** PACKET STORM COMPLETE ***", COLOR_YELLOW);
    screen_print_centered(19, "THE BOTNET LIVES ON.", COLOR_GREEN);
}
