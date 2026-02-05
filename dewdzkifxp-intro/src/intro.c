#include "intro.h"
#include "sprite.h"
#include "screen.h"
#include <conio.h>
#include <string.h>

/* Sprite data definitions */

/* Cursor sprite - solid 8x8 block */
const uint8_t sprite_cursor[63] = {
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00
};

/* Spinner frame 1 - vertical line */
const uint8_t sprite_spinner_frame1[63] = {
    0x00, 0x18, 0x00,
    0x00, 0x18, 0x00,
    0x00, 0x18, 0x00,
    0x00, 0x3C, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x3C, 0x00,
    0x00, 0x18, 0x00,
    0x00, 0x18, 0x00,
    0x00, 0x18, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00
};

/* Spinner frame 2 - diagonal \ */
const uint8_t sprite_spinner_frame2[63] = {
    0x60, 0x00, 0x00,
    0x30, 0x00, 0x00,
    0x18, 0x00, 0x00,
    0x0C, 0x00, 0x00,
    0x06, 0x00, 0x00,
    0x03, 0x00, 0x00,
    0x01, 0x80, 0x00,
    0x00, 0xC0, 0x00,
    0x00, 0x60, 0x00,
    0x00, 0x30, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00
};

/* Spinner frame 3 - horizontal line */
const uint8_t sprite_spinner_frame3[63] = {
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00
};

/* Spinner frame 4 - diagonal / */
const uint8_t sprite_spinner_frame4[63] = {
    0x00, 0x00, 0x60,
    0x00, 0x00, 0x30,
    0x00, 0x00, 0x18,
    0x00, 0x00, 0x0C,
    0x00, 0x00, 0x06,
    0x00, 0x00, 0x03,
    0x00, 0x01, 0x80,
    0x00, 0x0C, 0x00,
    0x00, 0x60, 0x00,
    0x00, 0x30, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00
};

/* Arrow sprite - pointing right */
const uint8_t sprite_arrow[63] = {
    0x00, 0x00, 0x00,
    0x00, 0x80, 0x00,
    0x00, 0xC0, 0x00,
    0x00, 0xE0, 0x00,
    0xFF, 0xF0, 0x00,
    0xFF, 0xF8, 0x00,
    0xFF, 0xFC, 0x00,
    0xFF, 0xF8, 0x00,
    0xFF, 0xF0, 0x00,
    0x00, 0xE0, 0x00,
    0x00, 0xC0, 0x00,
    0x00, 0x80, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00
};

/* Lewis avatar - face with sunglasses and smile */
const uint8_t sprite_lewis[63] = {
    0x00, 0x3C, 0x00,  /* Top of head */
    0x00, 0x7E, 0x00,
    0x00, 0xFF, 0x00,
    0x01, 0xFF, 0x80,
    0x01, 0xFF, 0x80,
    0x03, 0xFF, 0xC0,
    0x03, 0xFF, 0xC0,  /* Sunglasses start */
    0x7F, 0xFF, 0xFE,  /* Sunglasses bar */
    0x7F, 0xFF, 0xFE,
    0x03, 0xFF, 0xC0,  /* Sunglasses end */
    0x03, 0xFF, 0xC0,
    0x03, 0xFF, 0xC0,  /* Face */
    0x03, 0xFF, 0xC0,
    0x03, 0x00, 0xC0,  /* Smile start */
    0x03, 0x00, 0xC0,
    0x01, 0x81, 0x80,
    0x01, 0x81, 0x80,
    0x00, 0xC3, 0x00,
    0x00, 0x7E, 0x00,  /* Smile end */
    0x00, 0x3C, 0x00,
    0x00
};

/* FTP server icon - server box with lights */
const uint8_t sprite_ftp[63] = {
    0x00, 0x00, 0x00,
    0x3F, 0xFF, 0xFC,  /* Top */
    0x20, 0x00, 0x04,
    0x27, 0x3C, 0xE4,  /* Status lights */
    0x20, 0x00, 0x04,
    0x3F, 0xFF, 0xFC,  /* Divider */
    0x20, 0x00, 0x04,
    0x27, 0x3C, 0xE4,  /* More lights */
    0x20, 0x00, 0x04,
    0x3F, 0xFF, 0xFC,  /* Divider */
    0x20, 0x00, 0x04,
    0x27, 0x3C, 0xE4,  /* More lights */
    0x20, 0x00, 0x04,
    0x3F, 0xFF, 0xFC,  /* Bottom */
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00
};

/* Helper function to wait for key press or check for quit */
static uint8_t wait_for_key_or_quit(void) {
    while (!kbhit()) {
        /* Wait */
    }
    if (cgetc() == 'q' || cgetc() == 'Q') {
        return 1;  /* Quit */
    }
    return 0;  /* Continue */
}

/* Helper function to type out text character by character */
static void type_text(uint8_t x, uint8_t y, const char* text, uint8_t color) {
    uint8_t i;
    char buffer[41];
    uint8_t len;

    len = strlen(text);
    if (len > 40) len = 40;

    buffer[len] = '\0';

    for (i = 0; i < len; i++) {
        buffer[i] = text[i];
        buffer[i + 1] = '\0';
        screen_print_string(x, y, buffer, color);
        waitvsync();
        waitvsync();
        waitvsync();
    }
}

/* Scene 1: Library Exterior */
static uint8_t intro_scene1_library(void) {
    screen_clear();

    /* Draw library building using PETSCII */
    screen_print_centered(5, "+----------------------+", COLOR_CYAN);
    screen_print_centered(6, "|   PUBLIC LIBRARY     |", COLOR_CYAN);
    screen_print_centered(7, "+----------------------+", COLOR_CYAN);
    screen_print_centered(8, "|  []  []  []  []  []  |", COLOR_WHITE);
    screen_print_centered(9, "|  []  []  []  []  []  |", COLOR_WHITE);
    screen_print_centered(10, "|  []  []  []  []  []  |", COLOR_WHITE);
    screen_print_centered(11, "+----------------------+", COLOR_CYAN);
    screen_print_centered(12, "|        ____          |", COLOR_BLUE);
    screen_print_centered(13, "|       |    |         |", COLOR_BLUE);
    screen_print_centered(14, "+-------+----+---------+", COLOR_CYAN);

    screen_print_centered(17, "1995. YOUR LOCAL LIBRARY.", COLOR_WHITE);
    screen_print_centered(20, "PRESS ANY KEY", COLOR_YELLOW);

    return wait_for_key_or_quit();
}

/* Scene 2: At the Terminal */
static uint8_t intro_scene2_terminal(void) {
    uint16_t frame;

    screen_clear();

    /* Draw terminal */
    screen_draw_box(8, 5, 24, 10, COLOR_GRAY2);
    screen_print_string(10, 7, "INTERNET TERMINAL", COLOR_CYAN);
    screen_print_string(10, 9, "INSERT CARD...", COLOR_WHITE);

    screen_print_centered(17, "YOU SIT DOWN AT THE FREE", COLOR_WHITE);
    screen_print_centered(18, "INTERNET TERMINAL...", COLOR_WHITE);
    screen_print_centered(21, "PRESS ANY KEY", COLOR_YELLOW);

    /* Load cursor sprite and animate blinking */
    sprite_load(SPRITE_CURSOR, sprite_cursor, SPRITE_DATA_BASE);
    sprite_set_color(SPRITE_CURSOR, COLOR_WHITE);
    sprite_set_position(SPRITE_CURSOR, 230, 140);

    frame = 0;
    while (!kbhit()) {
        if ((frame % 30) < 15) {
            sprite_enable(SPRITE_CURSOR, 1);
        } else {
            sprite_enable(SPRITE_CURSOR, 0);
        }
        waitvsync();
        frame++;
    }

    sprite_enable(SPRITE_CURSOR, 0);

    if (cgetc() == 'q' || cgetc() == 'Q') {
        return 1;
    }
    return 0;
}

/* Scene 3: Login Screen */
static uint8_t intro_scene3_login(void) {
    uint8_t frame;
    const uint8_t* spinner_frames[4];

    screen_clear();

    screen_draw_box(5, 5, 30, 12, COLOR_CYAN);
    screen_print_string(7, 7, "IRC CLIENT v2.1", COLOR_YELLOW);
    screen_print_string(7, 10, "CONNECTING TO SERVER...", COLOR_WHITE);

    /* Setup spinner frames */
    spinner_frames[0] = sprite_spinner_frame1;
    spinner_frames[1] = sprite_spinner_frame2;
    spinner_frames[2] = sprite_spinner_frame3;
    spinner_frames[3] = sprite_spinner_frame4;

    sprite_set_color(SPRITE_CURSOR, COLOR_YELLOW);
    sprite_set_position(SPRITE_CURSOR, 180, 140);
    sprite_enable(SPRITE_CURSOR, 1);

    /* Animate spinner for 3 seconds (180 frames) */
    for (frame = 0; frame < 180; frame++) {
        sprite_load(SPRITE_CURSOR, spinner_frames[(frame / 8) % 4], SPRITE_DATA_BASE);
        waitvsync();
    }

    sprite_enable(SPRITE_CURSOR, 0);

    screen_print_string(7, 12, "CONNECTED!", COLOR_GREEN);

    /* Wait 1 second */
    for (frame = 0; frame < 60; frame++) {
        waitvsync();
    }

    return 0;
}

/* Scene 4: Channel List */
static uint8_t intro_scene4_channels(void) {
    uint8_t i;
    uint16_t x;

    screen_clear();

    screen_draw_box(2, 2, 36, 20, COLOR_CYAN);
    screen_print_string(4, 3, "CHANNEL LIST", COLOR_YELLOW);
    screen_print_string(4, 5, "#coding", COLOR_WHITE);
    screen_print_string(4, 6, "#linux", COLOR_WHITE);
    screen_print_string(4, 7, "#hacking", COLOR_CYAN);
    screen_print_string(4, 8, "#music", COLOR_WHITE);
    screen_print_string(4, 9, "#general", COLOR_WHITE);

    screen_print_centered(22, "PRESS ANY KEY", COLOR_YELLOW);

    /* Load arrow sprite */
    sprite_load(SPRITE_CURSOR, sprite_arrow, SPRITE_DATA_BASE);
    sprite_set_color(SPRITE_CURSOR, COLOR_YELLOW);

    /* Slide arrow from right to point at #hacking */
    for (i = 0; i < 20; i++) {
        x = 280 - ((280 - 200) * i / 20);
        sprite_set_position(SPRITE_CURSOR, x, 105);
        sprite_enable(SPRITE_CURSOR, 1);
        waitvsync();
    }

    /* Keep arrow visible */
    if (wait_for_key_or_quit()) {
        sprite_enable(SPRITE_CURSOR, 0);
        return 1;
    }

    sprite_enable(SPRITE_CURSOR, 0);
    return 0;
}

/* Scene 5: Join #hacking */
static uint8_t intro_scene5_join(void) {
    screen_clear();

    screen_draw_box(2, 2, 36, 20, COLOR_CYAN);
    screen_print_string(4, 3, "#HACKING - WAREZ, CRACKS, PHREAKING", COLOR_YELLOW);

    screen_print_string(4, 6, "*** YOU JOINED #HACKING", COLOR_GREEN);

    /* Wait a moment */
    waitvsync();
    waitvsync();
    waitvsync();
    waitvsync();
    waitvsync();

    type_text(4, 8, "<ZEROCOOL> SUP DUDES", COLOR_WHITE);
    type_text(4, 9, "<ACIDBURN> ANYONE GOT NEW WAREZ?", COLOR_WHITE);
    type_text(4, 10, "<PHANTOM> CHECK FTP.ELITE.ORG", COLOR_CYAN);

    /* Auto-advance after typing */
    return 0;
}

/* Scene 6: Lewis Appears */
static uint8_t intro_scene6_lewis(void) {
    uint8_t i;
    uint16_t x;

    /* Load Lewis sprite */
    sprite_load(SPRITE_LEWIS, sprite_lewis, SPRITE_DATA_BASE + SPRITE_DATA_SIZE);
    sprite_set_color(SPRITE_LEWIS, COLOR_CYAN);

    /* Slide Lewis in from right */
    for (i = 0; i < 20; i++) {
        x = 320 - ((320 - 280) * i / 20);
        sprite_set_position(SPRITE_LEWIS, x, 150);
        sprite_enable(SPRITE_LEWIS, 1);
        waitvsync();
    }

    /* Lewis talks */
    type_text(4, 12, "<LEWIS> HEY NEWBIE, FIRST TIME?", COLOR_YELLOW);
    type_text(4, 13, "<YOU> YEAH... WHAT'S A TOP SITE?", COLOR_WHITE);

    screen_print_centered(22, "PRESS ANY KEY", COLOR_YELLOW);

    if (wait_for_key_or_quit()) {
        sprite_enable(SPRITE_LEWIS, 0);
        return 1;
    }

    return 0;
}

/* Scene 7: TOP SITE Lesson */
static uint8_t intro_scene7_lesson(void) {
    uint16_t frame;

    screen_clear();

    /* Move Lewis to left side */
    sprite_set_position(SPRITE_LEWIS, 60, 80);
    sprite_enable(SPRITE_LEWIS, 1);

    /* Load FTP icon sprite */
    sprite_load(SPRITE_FTP, sprite_ftp, SPRITE_DATA_BASE + SPRITE_DATA_SIZE * 2);
    sprite_set_color(SPRITE_FTP, COLOR_GRAY2);
    sprite_set_position(SPRITE_FTP, 250, 120);
    sprite_enable(SPRITE_FTP, 1);

    /* Text box explaining */
    screen_draw_box(8, 2, 24, 16, COLOR_CYAN);
    screen_print_string(10, 3, "WAREZ 101", COLOR_YELLOW);
    screen_print_string(10, 5, "TOP SITES ARE", COLOR_WHITE);
    screen_print_string(10, 6, "ELITE FTP SERVERS", COLOR_CYAN);
    screen_print_string(10, 8, "THEY STORE:", COLOR_WHITE);
    screen_print_string(10, 9, "- NEW RELEASES", COLOR_GREEN);
    screen_print_string(10, 10, "- CRACKED GAMES", COLOR_GREEN);
    screen_print_string(10, 11, "- PIRATED SOFTWARE", COLOR_GREEN);
    screen_print_string(10, 13, "YOUR MISSION:", COLOR_YELLOW);
    screen_print_string(10, 14, "SCAN FOR SITES", COLOR_WHITE);
    screen_print_string(10, 15, "BUILD REPUTATION", COLOR_WHITE);

    screen_print_centered(22, "PRESS ANY KEY", COLOR_YELLOW);

    /* Pulse FTP icon while waiting */
    frame = 0;
    while (!kbhit()) {
        if ((frame / 15) % 2 == 0) {
            sprite_set_expand_y(SPRITE_FTP, 1);
        } else {
            sprite_set_expand_y(SPRITE_FTP, 0);
        }
        waitvsync();
        frame++;
    }

    sprite_set_expand_y(SPRITE_FTP, 0);

    if (cgetc() == 'q' || cgetc() == 'Q') {
        sprite_enable(SPRITE_LEWIS, 0);
        sprite_enable(SPRITE_FTP, 0);
        return 1;
    }

    return 0;
}

/* Scene 8: FTP Scan Demo */
static uint8_t intro_scene8_scan(void) {
    uint8_t frame;
    uint8_t x, y;
    uint8_t ch;

    screen_clear();

    /* Move Lewis to top-right */
    sprite_set_position(SPRITE_LEWIS, 280, 50);
    sprite_enable(SPRITE_LEWIS, 1);
    sprite_enable(SPRITE_FTP, 0);

    screen_print_string(2, 1, "LEWIS: LET ME SHOW YOU...", COLOR_YELLOW);

    screen_draw_box(5, 4, 30, 15, COLOR_CYAN);
    screen_print_string(7, 5, "SCANNING FTP PORTS...", COLOR_WHITE);

    /* Radar sweep animation (simplified) */
    for (frame = 0; frame < 60; frame++) {
        /* Random matrix characters */
        x = 7 + (frame % 28);
        y = 7 + (frame % 12);
        ch = '/';
        if (frame % 4 == 0) ch = '\\';
        else if (frame % 4 == 1) ch = '|';
        else if (frame % 4 == 2) ch = '-';

        screen_set_char(x, y, ch, COLOR_CYAN);
        waitvsync();
    }

    screen_print_string(7, 17, "*** FTP FOUND! ***", COLOR_GREEN);
    screen_print_string(7, 18, "207.15.33.42:21", COLOR_YELLOW);

    /* Wait 2 seconds */
    for (frame = 0; frame < 120; frame++) {
        waitvsync();
    }

    sprite_enable(SPRITE_LEWIS, 0);

    screen_clear();
    screen_print_centered(12, "WELCOME TO THE SCENE...", COLOR_CYAN);

    /* Wait 2 seconds before ending */
    for (frame = 0; frame < 120; frame++) {
        waitvsync();
    }

    return 0;
}

void intro_init(void) {
    screen_init();
    sprite_init();
}

uint8_t intro_run(void) {
    intro_init();

    if (intro_scene1_library()) return 1;
    if (intro_scene2_terminal()) return 1;
    if (intro_scene3_login()) return 1;
    if (intro_scene4_channels()) return 1;
    if (intro_scene5_join()) return 1;
    if (intro_scene6_lewis()) return 1;
    if (intro_scene7_lesson()) return 1;
    if (intro_scene8_scan()) return 1;

    return 0;  /* Completed successfully */
}
