#include "intro.h"
#include "sprite.h"
#include "sprite_data.h"
#include "screen.h"
#include <conio.h>
#include <string.h>

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

/* Draw pub FTP scanner results */
static void draw_pub_scanner(void) {
    screen_print_string(1, 3, "PUB FTP SCANNER v2.1", COLOR_CYAN);
    screen_draw_hline(1, 4, 38, '-', COLOR_GRAY2);

    wait_frames(20);
    screen_print_string(1, 6, "ftp.warez.ru:21", COLOR_GRAY2);
    screen_print_string(18, 6, "USER ANON FAILED", COLOR_RED);
    wait_frames(15);

    screen_print_string(1, 7, "ftp.0day.cn:21", COLOR_GRAY2);
    screen_print_string(18, 7, "USER ANON FAILED", COLOR_RED);
    wait_frames(15);

    screen_print_string(1, 8, "ftp.gamez.pl:21", COLOR_GRAY2);
    screen_print_string(18, 8, "USER ANON FAILED", COLOR_RED);
    wait_frames(15);

    screen_print_string(1, 9, "ftp.apps.se:21", COLOR_GRAY2);
    screen_print_string(18, 9, "USER ANON FAILED", COLOR_RED);
    wait_frames(20);

    /* Found one! */
    screen_print_string(1, 11, "ftp.files.org:21", COLOR_GREEN);
    screen_print_string(19, 11, "PUBLIC FTP FOUND", COLOR_GREEN);
    wait_frames(30);

    screen_print_string(3, 12, "SPEEDTEST: 2KB/s", COLOR_YELLOW);
    wait_frames(15);
    screen_print_string(3, 13, "FREE SPACE: 25MB", COLOR_YELLOW);
    wait_frames(40);
}


/* ===== SCENE FUNCTIONS ===== */

/* Scene 1: #DEWDZKI-FXP - Pub FTP Scanner + Chat (wait for key) */
static uint8_t intro_scene1_chat(void) {
    screen_clear();

    screen_print_string(1, 0, "#DEWDZKI-FXP", COLOR_YELLOW);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);

    /* Draw pub scanner results */
    draw_pub_scanner();

    /* Chat at bottom */
    type_irc_line(16, "SICK0", "PUB FTPS ARE DYING",
                  COLOR_RED, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(18, "AXE", "how are other teams still",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(19, "AXE", "posting releases?..",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);

    screen_print_centered(23, "PRESS ANY KEY", COLOR_YELLOW);

    if (wait_for_key_or_quit()) return 1;

    screen_fade_to_black();
    return 0;
}

/* Scene 2: The Diss - nijntje mocks them (wait for key) */
static uint8_t intro_scene2_diss(void) {
    screen_clear();

    screen_print_string(1, 0, "#DEWDZKI-FXP", COLOR_YELLOW);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);

    screen_print_string(1, 3, "*** NIJNTJE HAS JOINED", COLOR_GREEN);
    wait_frames(30);

    type_irc_line(5, "NIJNTJE", "U GUYS STILL POSTING LAME",
                  COLOR_LIGHTRED, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(6, "NIJNTJE", "PUBFTPS? AT ROSEVALLEY-FXP",
                  COLOR_LIGHTRED, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(7, "NIJNTJE", "TEAMS POST HACKED BOXES!",
                  COLOR_LIGHTRED, COLOR_WHITE);
    wait_frames(40);

    screen_print_string(1, 9, "*** NIJNTJE HAS LEFT", COLOR_RED);
    wait_frames(30);

    type_irc_line(11, "AXE", "HACKED BOXES?",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(13, "LEWIS", "YOU SHOULD RESEARCH THIS :)",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(15, "AXE", "I HAVE AN IDEA ALREADY!",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);

    screen_print_centered(23, "PRESS ANY KEY", COLOR_YELLOW);

    return wait_for_key_or_quit();
}

/* Simple U sprite data for Serv-U FTP */
static const uint8_t sprite_servu[63] = {
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x18, 0x18, 0x00,  /* U shape start */
    0x18, 0x18, 0x00,
    0x18, 0x18, 0x00,
    0x18, 0x18, 0x00,
    0x18, 0x18, 0x00,
    0x18, 0x18, 0x00,
    0x18, 0x18, 0x00,
    0x18, 0x18, 0x00,
    0x0C, 0x30, 0x00,
    0x07, 0xE0, 0x00,  /* Bottom curve */
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00
};

/* Computer/PC sprite (24x21 multicolor) */
static const uint8_t sprite_computer[63] = {
    0x00, 0x00, 0x00,
    0x03, 0xFF, 0xC0,  /* Monitor frame */
    0x0F, 0xFF, 0xF0,
    0x3F, 0xFF, 0xFC,
    0x3F, 0xFF, 0xFC,
    0x3C, 0x00, 0x3C,
    0x3C, 0x00, 0x3C,
    0x3C, 0x00, 0x3C,  /* Screen area */
    0x3C, 0x00, 0x3C,
    0x3C, 0x00, 0x3C,
    0x3C, 0x00, 0x3C,
    0x3C, 0x00, 0x3C,
    0x3F, 0xFF, 0xFC,
    0x0F, 0xFF, 0xF0,
    0x03, 0xFF, 0xC0,
    0x00, 0xFF, 0x00,  /* Stand */
    0x03, 0xFF, 0xC0,  /* Base */
    0x0F, 0xFF, 0xF0,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00
};

/* Scene 3: SMB Lesson - Private lesson on hacking SMB + installing Serv-U (wait for key) */
static uint8_t intro_scene3_smb(void) {
    screen_clear();

    screen_print_string(6, 1, "*** PRIVATE CHAT ***", COLOR_LIGHTRED);
    screen_draw_hline(6, 2, 20, '-', COLOR_GRAY2);

    /* Show computer sprite */
    sprite_set_multicolor(SPRITE_FTP, 0);  /* Hires mode for single color */
    sprite_load(SPRITE_FTP, sprite_computer, SPRITE_DATA_BASE);
    sprite_set_color(SPRITE_FTP, COLOR_CYAN);
    sprite_fade_in(SPRITE_FTP, 100, 162);

    /* Private lesson from AXE */
    type_irc_line(4, "AXE", "ok lewis, SMB hacking 101",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(6, "AXE", "find weak passwords on port 139",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(7, "AXE", "try admin, password, 12345678",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(9, "LEWIS", "then what?",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(11, "AXE", "install Serv-U FTP on the box",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(12, "AXE", "now you got a dump site!",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);

    /* Show U sprite moving into the computer */
    sprite_set_multicolor(SPRITE_CURSOR, 0);  /* Hires mode */
    sprite_load(SPRITE_CURSOR, sprite_servu, SPRITE_DATA_BASE + 64);
    sprite_set_color(SPRITE_CURSOR, COLOR_YELLOW);
    sprite_slide_to(SPRITE_CURSOR, 320, 177, 100, 177, 60);
    wait_frames(30);

    screen_print_centered(23, "PRESS ANY KEY", COLOR_YELLOW);

    if (wait_for_key_or_quit()) {
        sprite_enable(SPRITE_CURSOR, 0);
        sprite_enable(SPRITE_FTP, 0);
        return 1;
    }

    sprite_enable(SPRITE_CURSOR, 0);
    sprite_enable(SPRITE_FTP, 0);

    screen_fade_to_black();
    return 0;
}

/* Scene 4: SMB Scan - X-SCAN with sprite sine wave (wait for key) */
static uint8_t intro_scene4_scan(void) {
    uint8_t frame;
    uint8_t i, phase;

    /* Pixel-accurate sine wave Y offsets (32 entries, amplitude 26, range 0-52) */
    static const uint8_t wave_y[32] = {
        26, 31, 36, 40, 44, 48, 50, 52,
        52, 52, 50, 48, 44, 40, 36, 31,
        26, 21, 16, 12,  8,  4,  2,  1,
         0,  1,  2,  4,  8, 12, 16, 21
    };

    /* Scan dot sprite - filled circle, 12px wide, 8px tall (hires) */
    static const uint8_t sprite_scan_dot[63] = {
        0x00, 0x3C, 0x00,
        0x00, 0xFF, 0x00,
        0x01, 0xFF, 0x80,
        0x03, 0xFF, 0xC0,
        0x03, 0xFF, 0xC0,
        0x01, 0xFF, 0x80,
        0x00, 0xFF, 0x00,
        0x00, 0x3C, 0x00,
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
        0x00, 0x00, 0x00
    };

    screen_clear();

    screen_print_string(1, 0, "X-SCAN SMB v2.0", COLOR_CYAN);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);
    screen_print_string(1, 2, "SCANNING PORT 139...", COLOR_WHITE);
    screen_print_string(1, 3, "RANGE: 130.235.138.0/24", COLOR_GRAY2);

    /* Draw scan area border */
    screen_draw_box(SCAN_AREA_X - 1, SCAN_AREA_Y - 1,
                    SCAN_AREA_W + 2, SCAN_AREA_H + 2, COLOR_GRAY2);

    /* Setup 8 wave sprites - load dot once, point all sprites to same data */
    sprite_load(0, sprite_scan_dot, SPRITE_DATA_BASE);
    for (i = 0; i < 8; i++) {
        if (i > 0) {
            SPRITE_PTRS[i] = SPRITE_DATA_BASE / 64;
        }
        sprite_set_multicolor(i, 0);
        sprite_set_color(i, COLOR_CYAN);
        sprite_enable(i, 1);
    }

    /* Animate sine wave scan */
    phase = 0;
    for (frame = 0; frame < 240; frame++) {
        /* Update all 8 wave dot positions */
        for (i = 0; i < 8; i++) {
            sprite_set_position(i,
                42 + (uint16_t)i * 28,
                108 + wave_y[(32 + phase - i * 4) % 32]);
        }
        phase = (phase + 1) % 32;

        /* Show IP addresses with password attempts */
        if (frame == 40) {
            screen_print_string(1, 15, "130.235.138.12", COLOR_GRAY2);
        }
        if (frame == 50) {
            screen_print_string(17, 15, "admin - FAIL", COLOR_RED);
        }
        if (frame == 60) {
            screen_print_string(17, 15, "12345678 - FAIL", COLOR_RED);
        }
        if (frame == 70) {
            screen_print_string(17, 15, "password - FAIL", COLOR_RED);
        }

        if (frame == 90) {
            screen_print_string(1, 16, "130.235.138.44", COLOR_GRAY2);
        }
        if (frame == 100) {
            screen_print_string(17, 16, "admin - FAIL", COLOR_RED);
        }
        if (frame == 110) {
            screen_print_string(17, 16, "12345678 - FAIL", COLOR_RED);
        }

        if (frame == 130) {
            screen_print_string(1, 17, "130.235.138.67", COLOR_GRAY2);
        }
        if (frame == 140) {
            screen_print_string(17, 17, "admin - FAIL", COLOR_RED);
        }

        if (frame == 160) {
            screen_print_string(1, 18, "130.235.138.89", COLOR_GRAY2);
        }
        if (frame == 170) {
            screen_print_string(17, 18, "admin - FAIL", COLOR_RED);
        }
        if (frame == 180) {
            screen_print_string(17, 18, "12345678 - FAIL", COLOR_RED);
        }

        if (frame == 200) {
            screen_print_string(1, 19, "130.235.138.201", COLOR_GRAY2);
        }
        if (frame == 210) {
            screen_print_string(17, 19, "admin - FAIL", COLOR_RED);
        }
        if (frame == 220) {
            screen_print_string(17, 19, "password - OK!", COLOR_GREEN);
            /* Wave turns green on success */
            for (i = 0; i < 8; i++) {
                sprite_set_color(i, COLOR_GREEN);
            }
        }

        waitvsync();
    }

    /* Disable all wave sprites */
    for (i = 0; i < 8; i++) {
        sprite_enable(i, 0);
    }

    /* Success result */
    screen_print_string(1, 21, "*** SMB LOGIN SUCCESS ***", COLOR_GREEN);
    screen_print_string(1, 22, "130.235.138.201:139 C$ WRITABLE", COLOR_YELLOW);

    wait_frames(60);

    screen_print_centered(23, "PRESS ANY KEY", COLOR_YELLOW);

    if (wait_for_key_or_quit()) return 1;

    screen_fade_to_black();
    return 0;
}

/* Scene 5: The Crew Forms - DEWDZKI-FXP is born (wait for key) */
static uint8_t intro_scene5_crew_rallies(void) {
    screen_clear();

    sprite_set_multicolor_shared(10, 0);

    /* Axe - sprite 0 (hooded hacker) */
    sprite_set_multicolor(0, 1);
    sprite_load(0, sprite_axe, SPRITE_DATA_BASE);
    sprite_set_color(0, 6);  /* Blue (dark) */
    sprite_fade_in(0, 80, 50);
    screen_print_string(16, 0, "AXE", COLOR_BLUE);
    wait_frames(30);

    /* Sick0 - sprite 1 (long brown hair) */
    sprite_set_multicolor(1, 1);
    sprite_load(1, sprite_sick0, SPRITE_DATA_BASE + 64);
    sprite_set_color(1, 9);  /* Brown (hair) */
    sprite_fade_in(1, 80, 80);
    screen_print_string(16, 4, "SICK0", COLOR_RED);
    wait_frames(30);

    /* ZzZ - sprite 2 (smoking, heavy-lidded) */
    sprite_set_multicolor(2, 1);
    sprite_load(2, sprite_zzz, SPRITE_DATA_BASE + 128);
    sprite_set_color(2, 4);  /* Purple */
    sprite_fade_in(2, 80, 110);
    screen_print_string(16, 8, "ZZZ", COLOR_PURPLE);
    wait_frames(30);

    /* Rizz - sprite 3 (cap and grin) */
    sprite_set_multicolor(3, 1);
    sprite_load(3, sprite_rizz, SPRITE_DATA_BASE + 192);
    sprite_set_color(3, 7);  /* Yellow */
    sprite_fade_in(3, 80, 140);
    screen_print_string(16, 12, "RIZZ", COLOR_YELLOW);
    wait_frames(30);

    /* Lewis - sprite 4 (sunglasses + huge nose) */
    sprite_set_multicolor(4, 1);
    sprite_load(4, sprite_lewis, SPRITE_DATA_BASE + 256);
    sprite_set_color(4, 3);  /* Cyan */
    sprite_fade_in(4, 80, 170);
    screen_print_string(16, 15, "LEWIS", COLOR_CYAN);
    wait_frames(40);

    /* The birth announcement */
    screen_print_centered(23, "DEWDZKI-FXP IS BORN", COLOR_WHITE);
    wait_frames(60);

    screen_print_centered(23, "PRESS ANY KEY      ", COLOR_YELLOW);

    if (wait_for_key_or_quit()) {
        sprite_enable(0, 0);
        sprite_enable(1, 0);
        sprite_enable(2, 0);
        sprite_enable(3, 0);
        sprite_enable(4, 0);
        return 1;
    }

    /* Disable all sprites */
    sprite_enable(0, 0);
    sprite_enable(1, 0);
    sprite_enable(2, 0);
    sprite_enable(3, 0);
    sprite_enable(4, 0);

    screen_fade_to_black();
    return 0;
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

    if (intro_scene1_chat()) return 1;             /* Pub scanner + chat (wait key) */
    if (intro_scene2_diss()) return 1;             /* Konijntje diss (wait key) */
    if (intro_scene3_smb()) return 1;              /* Private SMB lesson (wait key) */
    if (intro_scene4_scan()) return 1;             /* X-SCAN password attempts (wait key) */
    if (intro_scene5_crew_rallies()) return 1;     /* The crew forms - DEWDZKI-FXP (wait key) */
    if (intro_scene6_title()) return 1;            /* Title card (wait key) */

    return 0;
}
