#include "helpers.h"
#include "sprite.h"
#include <conio.h>
#include <string.h>
#include <peekpoke.h>

/* Wait for specified number of frames (50 fps PAL) */
void wait_frames(uint8_t count) {
    uint8_t i;
    for (i = 0; i < count; i++) {
        /* Wait for raster line 255 (bottom of screen) */
        while (PEEK(0xD012) != 255);
        /* Wait for next frame */
        while (PEEK(0xD012) == 255);
    }
}

/* Fade in sprite at position */
void sprite_fade_in(uint8_t sprite_num, uint16_t x, uint8_t y) {
    sprite_set_position(sprite_num, x, y);
    sprite_enable(sprite_num, 1);
    wait_frames(10);
}

/* Wait for key or check for quit */
uint8_t wait_for_key_or_quit(void) {
    uint8_t key;

    /* Wait for any key */
    while (!kbhit()) {
        wait_frames(1);
    }

    key = cgetc();

    /* Check if Q or q pressed */
    if (key == 'q' || key == 'Q') {
        return 1;  /* User wants to quit */
    }

    return 0;  /* Continue */
}

/* Typewriter text effect */
void type_text(uint8_t x, uint8_t y, const char *text, uint8_t color) {
    uint8_t i;
    uint8_t len = strlen(text);

    gotoxy(x, y);
    textcolor(color);

    for (i = 0; i < len; i++) {
        cputc(text[i]);
        wait_frames(2);
    }
}

/* Type IRC line with nick and message */
void type_irc_line(uint8_t y, const char *nick, const char *msg, uint8_t nick_color, uint8_t msg_color) {
    /* Format: <nick> message */
    gotoxy(0, y);
    textcolor(nick_color);
    cprintf("<%s> ", nick);

    textcolor(msg_color);
    cputs(msg);
}
