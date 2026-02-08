#include "helpers.h"
#include "sprite.h"
#include "screen.h"
#include <conio.h>
#include <string.h>
#include <peekpoke.h>

/* Wait for specified number of frames (50 fps PAL) */
void wait_frames(uint8_t count) {
    uint8_t i;
    for (i = 0; i < count; i++) {
        while (PEEK(0xD012) != 255);
        while (PEEK(0xD012) == 255);
    }
}

/* Fade in sprite with blink effect */
void sprite_fade_in(uint8_t sprite_num, uint16_t x, uint8_t y) {
    uint8_t i;
    for (i = 0; i < 20; i++) {
        sprite_set_position(sprite_num, x, y);
        sprite_enable(sprite_num, (i % 4) < 2);
        wait_frames(2);
    }
    sprite_enable(sprite_num, 1);
}

/* Slide sprite from start to end position over given frames */
void sprite_slide_to(uint8_t sprite_num, uint16_t start_x, uint8_t start_y,
                     uint16_t end_x, uint8_t end_y, uint8_t frames) {
    uint8_t i;
    uint16_t x;
    uint8_t y;

    for (i = 0; i < frames; i++) {
        if (start_x <= end_x) {
            x = start_x + ((end_x - start_x) * i / frames);
        } else {
            x = start_x - ((start_x - end_x) * i / frames);
        }

        if (start_y <= end_y) {
            y = start_y + ((end_y - start_y) * i / frames);
        } else {
            y = start_y - ((start_y - end_y) * i / frames);
        }

        sprite_set_position(sprite_num, x, y);
        sprite_enable(sprite_num, 1);
        wait_frames(1);
    }
    sprite_set_position(sprite_num, end_x, end_y);
}

/* Wait for key or check for quit */
uint8_t wait_for_key_or_quit(void) {
    uint8_t key;

    while (!kbhit()) {
        wait_frames(1);
    }

    key = cgetc();

    if (key == 'q' || key == 'Q') {
        return 1;
    }

    return 0;
}

/* Typewriter text effect */
void type_text(uint8_t x, uint8_t y, const char *text, uint8_t color) {
    uint8_t i;
    uint8_t len = strlen(text);

    for (i = 0; i < len; i++) {
        screen_set_char(x + i, y, text[i], color);
        wait_frames(2);
    }
}

/* Type IRC line with nick and message */
void type_irc_line(uint8_t y, const char *nick, const char *msg, uint8_t nick_color, uint8_t msg_color) {
    uint8_t nick_len;
    char nick_buf[16];

    nick_buf[0] = '<';
    strcpy(nick_buf + 1, nick);
    nick_len = strlen(nick_buf);
    nick_buf[nick_len] = '>';
    nick_buf[nick_len + 1] = ' ';
    nick_buf[nick_len + 2] = '\0';
    nick_len = strlen(nick_buf);

    screen_print_string(1, y, nick_buf, nick_color);
    type_text(1 + nick_len, y, msg, msg_color);
}

/* Draw a progress bar emptying right-to-left */
void reverse_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t delay) {
    uint8_t i;

    /* Draw full bar first */
    for (i = 0; i < width; i++) {
        screen_set_char(x + i, y, 0xA0, COLOR_GREEN);
    }
    wait_frames(delay);

    /* Empty from right to left */
    for (i = width; i > 0; i--) {
        screen_set_char(x + i - 1, y, ' ', COLOR_BLACK);

        /* Change color as bar shrinks */
        if (i < width / 3) {
            /* Last third: red */
            if (i > 1) {
                screen_set_char(x + i - 2, y, 0xA0, COLOR_RED);
            }
        } else if (i < (width * 2) / 3) {
            /* Middle third: yellow */
            if (i > 1) {
                screen_set_char(x + i - 2, y, 0xA0, COLOR_YELLOW);
            }
        }

        wait_frames(delay);
    }
}

/* Flash border between two colors */
void screen_flash_border(uint8_t color1, uint8_t color2, uint8_t cycles) {
    uint8_t i;

    for (i = 0; i < cycles; i++) {
        POKE(0xD020, color1);
        wait_frames(3);
        POKE(0xD020, color2);
        wait_frames(3);
    }
    POKE(0xD020, COLOR_BLACK);
}

/* Print dossier line: label instantly, value with typewriter */
void type_dossier_line(uint8_t y, const char *label, const char *value,
                       uint8_t label_color, uint8_t value_color) {
    uint8_t label_len = strlen(label);

    screen_print_string(10, y, label, label_color);
    type_text(10 + label_len, y, value, value_color);
}
