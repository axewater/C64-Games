#include "intro.h"
#include "sprite.h"
#include "sprite_data.h"
#include "screen.h"
#include <conio.h>
#include <string.h>

/* Sine lookup table - values 0-7 for wave animation */
static const uint8_t sine_table[SINE_TABLE_SIZE] = {
    4, 5, 6, 6, 7, 7, 7, 6,
    6, 5, 4, 3, 2, 1, 1, 0,
    0, 0, 1, 1, 2, 3, 4, 5,
    6, 6, 7, 7, 7, 6, 6, 5
};

/* ===== ANIMATION HELPER FUNCTIONS ===== */

void wait_frames(uint8_t count) {
    uint8_t i;
    for (i = 0; i < count; i++) {
        waitvsync();
    }
}

/* Sprite fade-in effect */
static void sprite_fade_in(uint8_t sprite_num, uint16_t x, uint8_t y) {
    uint8_t i;
    for (i = 0; i < 20; i++) {
        sprite_set_position(sprite_num, x, y);
        sprite_enable(sprite_num, (i % 4) < 2);  /* Blink pattern */
        wait_frames(2);
    }
    sprite_enable(sprite_num, 1);  /* Final: on */
}

/* Sprite bounce animation */
static void sprite_bounce(uint8_t sprite_num, uint16_t x, uint8_t start_y) {
    uint8_t i;
    uint8_t y_offset;
    for (i = 0; i < 40; i++) {
        /* Simple parabolic bounce */
        y_offset = (i < 20) ? (20 - i) : (i - 20);
        sprite_set_position(sprite_num, x, start_y - y_offset);
        wait_frames(2);
    }
}

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
        wait_frames(SLIDE_SPEED);
    }
}

static uint8_t wait_for_key_or_quit(void) {
    char ch;
    while (!kbhit()) {
        /* Wait for keypress */
    }
    ch = cgetc();
    if (ch == 'q' || ch == 'Q') {
        return 1;
    }
    return 0;
}

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
        wait_frames(TYPING_DELAY);
    }
}

/* Type an IRC line: <NICK> in nick_color, then message in msg_color */
static void type_irc_line(uint8_t y, const char* nick, const char* msg,
                          uint8_t nick_color, uint8_t msg_color) {
    uint8_t nick_len;
    char nick_buf[16];

    /* Build "<NICK> " string */
    nick_buf[0] = '<';
    strcpy(nick_buf + 1, nick);
    nick_len = strlen(nick_buf);
    nick_buf[nick_len] = '>';
    nick_buf[nick_len + 1] = ' ';
    nick_buf[nick_len + 2] = '\0';
    nick_len = strlen(nick_buf);

    /* Print nick instantly */
    screen_print_string(1, y, nick_buf, nick_color);

    /* Type message with typewriter effect */
    type_text(1 + nick_len, y, msg, msg_color);
}

/* ===== PETSCII ART DRAWING FUNCTIONS ===== */

/* Draw server rack (empty FTP servers) */
static void draw_server_rack(void) {
    screen_print_string(12, 5, "+------------+", COLOR_GRAY2);
    screen_print_string(12, 6, "|  /\\/\\/\\/\\  |", COLOR_GRAY2);
    screen_print_string(12, 7, "|   EMPTY    |", COLOR_RED);
    screen_print_string(12, 8, "|  /\\/\\/\\/\\  |", COLOR_GRAY2);
    screen_print_string(12, 9, "+------------+", COLOR_GRAY2);
}

/* Draw computer diagram (Windows PC with port) */
static void draw_computer_diagram(void) {
    screen_print_string(4, 10, "+--------------+", COLOR_BLUE);
    screen_print_string(4, 11, "| WINDOWS PC   |", COLOR_CYAN);
    screen_print_string(4, 12, "|  PORT 139   |", COLOR_YELLOW);
    screen_print_string(4, 13, "| SMB ENABLED  |", COLOR_GREEN);
    screen_print_string(4, 14, "+--------------+", COLOR_BLUE);
    screen_print_string(4, 15, "    |_____|     ", COLOR_GRAY2);
}

/* ===== SCENE FUNCTIONS ===== */

/* Scene 1: #DEWDZKI-FXP - Chat + Server Rack PETSCII Art (auto-advance) */
static uint8_t intro_scene1_chat(void) {
    screen_clear();

    screen_print_string(1, 0, "#DEWDZKI-FXP", COLOR_YELLOW);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);

    /* Draw server rack art */
    draw_server_rack();

    type_irc_line(3, "SICK0", "PUB FTPS ARE DYING",
                  COLOR_RED, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(15, "AXE", "I GOT AN IDEA...",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(60);

    screen_fade_to_black();
    return 0;  /* Auto-advance */
}

/* Scene 2: The Diss - DARKPH4NT mocks them (wait for key) */
static uint8_t intro_scene2_diss(void) {
    screen_clear();

    screen_print_string(1, 0, "#DEWDZKI-FXP", COLOR_YELLOW);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);

    screen_print_string(1, 3, "*** DARKPH4NT HAS JOINED", COLOR_GREEN);
    wait_frames(30);

    type_irc_line(5, "DARKPH4NT", "NOOBS IN KIDDIE POOL!",
                  COLOR_LIGHTRED, COLOR_WHITE);
    wait_frames(30);

    screen_print_string(1, 7, "*** DARKPH4NT HAS LEFT", COLOR_RED);
    wait_frames(30);

    type_irc_line(9, "AXE", "TIME TO LEVEL UP",
                  COLOR_CYAN, COLOR_WHITE);

    screen_print_centered(23, "PRESS ANY KEY", COLOR_YELLOW);

    return wait_for_key_or_quit();
}

/* Scene 3: SMB Lesson - Windows sprite + computer diagram + condensed text (wait for key) */
static uint8_t intro_scene3_smb(void) {
    screen_clear();

    /* Load and show Windows logo sprite */
    sprite_set_multicolor(SPRITE_FTP, 0);  /* Hires mode */
    sprite_load(SPRITE_FTP, sprite_windows, SPRITE_DATA_BASE);
    sprite_set_color(SPRITE_FTP, COLOR_CYAN);
    sprite_fade_in(SPRITE_FTP, 280, 60);

    screen_print_string(8, 1, "SMB HACKING 101", COLOR_CYAN);
    screen_draw_hline(8, 2, 15, '-', COLOR_GRAY2);

    /* Draw computer diagram */
    draw_computer_diagram();

    /* Condensed explanation */
    type_irc_line(4, "AXE", "SMB = WINDOWS FILE SHARES",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(6, "AXE", "PORT 139, WEAK SECURITY",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(17, "LEWIS", "LETS DO IT",
                  COLOR_YELLOW, COLOR_WHITE);

    screen_print_centered(23, "PRESS ANY KEY", COLOR_YELLOW);

    if (wait_for_key_or_quit()) {
        sprite_enable(SPRITE_FTP, 0);
        return 1;
    }

    sprite_enable(SPRITE_FTP, 0);
    screen_fade_to_black();
    return 0;
}

/* Scene 4: SMB Scan - Sine wave scanning animation with treasure (auto-advance) */
static uint8_t intro_scene4_scan(void) {
    uint8_t frame;
    uint8_t x, sy;
    uint8_t prev_y[28];
    uint8_t phase;
    const uint8_t* spinner_frames[4];

    screen_clear();

    screen_print_string(1, 0, "SMB SCANNER v1.0", COLOR_CYAN);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);
    screen_print_string(1, 2, "SCANNING PORT 139...", COLOR_WHITE);
    screen_print_string(1, 3, "RANGE: 192.168.0.0/16", COLOR_GRAY2);

    /* Draw scan area border */
    screen_draw_box(SCAN_AREA_X - 1, SCAN_AREA_Y - 1,
                    SCAN_AREA_W + 2, SCAN_AREA_H + 2, COLOR_GRAY2);

    /* Setup spinner */
    spinner_frames[0] = sprite_spinner_frame1;
    spinner_frames[1] = sprite_spinner_frame2;
    spinner_frames[2] = sprite_spinner_frame3;
    spinner_frames[3] = sprite_spinner_frame4;

    sprite_set_multicolor(SPRITE_CURSOR, 0);
    sprite_set_color(SPRITE_CURSOR, COLOR_YELLOW);
    sprite_set_position(SPRITE_CURSOR, 290, 50);
    sprite_enable(SPRITE_CURSOR, 1);

    /* Initialize previous Y positions */
    for (x = 0; x < SCAN_AREA_W; x++) {
        prev_y[x] = 0xFF;  /* No previous position */
    }

    /* Animate sine wave scan for ~4 seconds */
    phase = 0;
    for (frame = 0; frame < 240; frame++) {
        /* Update spinner every SPINNER_SPEED frames */
        if (frame % SPINNER_SPEED == 0) {
            sprite_load(SPRITE_CURSOR,
                        spinner_frames[(frame / SPINNER_SPEED) % 4],
                        SPRITE_DATA_BASE);
        }

        /* Draw sine wave across scan area */
        for (x = 0; x < SCAN_AREA_W; x++) {
            /* Clear previous position */
            if (prev_y[x] != 0xFF) {
                screen_set_char(SCAN_AREA_X + x,
                                SCAN_AREA_Y + prev_y[x], ' ', COLOR_BLACK);
            }

            /* Calculate new Y from sine table */
            sy = sine_table[(phase + x) % SINE_TABLE_SIZE];

            /* Scale to scan area height */
            if (sy >= SCAN_AREA_H) sy = SCAN_AREA_H - 1;

            /* Draw asterisk at new position */
            screen_set_char(SCAN_AREA_X + x,
                            SCAN_AREA_Y + sy, '*', COLOR_CYAN);
            prev_y[x] = sy;
        }

        phase = (phase + 1) % SINE_TABLE_SIZE;

        /* Show IP addresses periodically */
        if (frame == 60) {
            screen_print_string(1, 17, "192.168.1.44  - TIMEOUT", COLOR_GRAY2);
        }
        if (frame == 100) {
            screen_print_string(1, 18, "192.168.3.17  - CLOSED", COLOR_GRAY2);
        }
        if (frame == 140) {
            screen_print_string(1, 19, "192.168.5.102 - CLOSED", COLOR_GRAY2);
        }
        if (frame == 180) {
            screen_print_string(1, 20, "192.168.12.55 - TIMEOUT", COLOR_GRAY2);
        }

        waitvsync();
    }

    /* Found a box! */
    sprite_enable(SPRITE_CURSOR, 0);

    screen_print_string(1, 22, "*** OPEN SHARE FOUND! ***", COLOR_GREEN);
    screen_print_string(1, 23, "192.168.14.201:139 C$ WRITABLE", COLOR_YELLOW);

    /* Load and animate treasure sprite */
    sprite_set_multicolor_shared(10, 0);  /* MC0 = brown */
    sprite_set_multicolor(SPRITE_LEWIS, 1);  /* Enable multicolor */
    sprite_load(SPRITE_LEWIS, sprite_treasure, SPRITE_DATA_BASE);
    sprite_set_color(SPRITE_LEWIS, COLOR_YELLOW);  /* Gold accents */
    sprite_fade_in(SPRITE_LEWIS, 300, 180);
    sprite_bounce(SPRITE_LEWIS, 300, 180);

    wait_frames(60);  /* Auto-advance after animation */

    sprite_enable(SPRITE_LEWIS, 0);
    screen_fade_to_black();
    return 0;
}

/* Scene 5: Crew Rallies - Merged forum + warning + rally with all crew sprites (auto-advance) */
static uint8_t intro_scene5_crew_rallies(void) {
    screen_clear();

    /* Show all crew sprites across screen */
    sprite_set_multicolor_shared(10, 0);

    /* Lewis on left */
    sprite_set_multicolor(SPRITE_LEWIS, 1);
    sprite_load(SPRITE_LEWIS, sprite_lewis, SPRITE_DATA_BASE);
    sprite_set_color(SPRITE_LEWIS, 6);
    sprite_fade_in(SPRITE_LEWIS, 80, 150);

    /* Axe in center */
    sprite_set_multicolor(SPRITE_FTP, 1);
    sprite_load(SPRITE_FTP, sprite_axe, SPRITE_DATA_BASE + 64);
    sprite_set_color(SPRITE_FTP, 3);  /* Cyan hood */
    sprite_fade_in(SPRITE_FTP, 160, 150);

    /* SICK0 on right */
    sprite_set_multicolor(SPRITE_CURSOR, 1);
    sprite_load(SPRITE_CURSOR, sprite_sick0, SPRITE_DATA_BASE + 128);
    sprite_set_color(SPRITE_CURSOR, 2);  /* Red cyber visor */
    sprite_fade_in(SPRITE_CURSOR, 240, 150);

    wait_frames(20);

    /* Condensed dialogue */
    screen_print_centered(18, "HIDE DEEP IN SYSTEM32", COLOR_CYAN);
    wait_frames(40);

    screen_print_centered(20, "YOU'RE RUNNING THIS SHOW NOW", COLOR_YELLOW);
    wait_frames(80);

    /* Disable sprites */
    sprite_enable(SPRITE_LEWIS, 0);
    sprite_enable(SPRITE_FTP, 0);
    sprite_enable(SPRITE_CURSOR, 0);

    screen_fade_to_black();
    return 0;  /* Auto-advance */
}

/* Scene 6: Title Card - "CHAPTER 2 / THE RISE" with blinking prompt (wait for key) */
static uint8_t intro_scene6_title(void) {
    uint8_t frame;
    char ch;

    screen_clear();

    /* Dramatic title reveal */
    wait_frames(30);

    screen_print_centered(8, "- - - - - - - - - - -", COLOR_GRAY2);
    wait_frames(15);

    screen_print_centered(10, "CHAPTER 2", COLOR_CYAN);
    wait_frames(20);

    screen_print_centered(12, "THE RISE", COLOR_WHITE);
    wait_frames(15);

    screen_print_centered(14, "- - - - - - - - - - -", COLOR_GRAY2);
    wait_frames(30);

    /* Blinking prompt */
    frame = 0;
    while (!kbhit()) {
        if ((frame % BLINK_CYCLE) < (BLINK_CYCLE / 2)) {
            screen_print_centered(20, "PRESS ANY KEY TO BEGIN", COLOR_YELLOW);
        } else {
            screen_print_centered(20, "                      ", COLOR_BLACK);
        }
        waitvsync();
        frame++;
    }

    ch = cgetc();
    if (ch == 'q' || ch == 'Q') return 1;
    return 0;
}

/* ===== INITIALIZATION AND MAIN SEQUENCE ===== */

void intro_init(void) {
    screen_init();
    sprite_init();
}

uint8_t intro_run(void) {
    intro_init();

    if (intro_scene1_chat()) return 1;             /* Chat + server rack art (auto) */
    if (intro_scene2_diss()) return 1;             /* Darkph4nt diss (wait key) */
    if (intro_scene3_smb()) return 1;              /* SMB lesson + visuals (wait key) */
    if (intro_scene4_scan()) return 1;             /* Scanner + treasure (auto) */
    if (intro_scene5_crew_rallies()) return 1;     /* Crew sprites + rally (auto) */
    if (intro_scene6_title()) return 1;            /* Title card (wait key) */

    return 0;
}
