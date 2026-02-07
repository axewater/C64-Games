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

/* ===== SCENE FUNCTIONS ===== */

/* Scene 1: #DEWDZKI-FXP - Crew chatting, pub FTPs getting stale (auto-advance) */
static uint8_t intro_scene1_chat(void) {
    screen_clear();

    screen_print_string(1, 0, "#DEWDZKI-FXP", COLOR_YELLOW);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);

    type_irc_line(3, "LEWIS", "ANOTHER DAY ANOTHER SCAN...",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(4, "SICK0", "PUB FTPS ARE DRY AF LATELY",
                  COLOR_RED, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(5, "FERRE070", "MAYBE WE SHOULD TRY FISHING",
                  COLOR_GREEN, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(6, "ZZZ", "ARROWS",
                  COLOR_GRAY2, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(7, "LEWIS", "EVERYONE SCANNING THE SAME",
                  COLOR_YELLOW, COLOR_WHITE);
    type_irc_line(8, "LEWIS", "RANGES... WE NEED SOMETHING NEW",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(10, "AXE", "I MIGHT HAVE AN IDEA...",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(40);

    return 0;  /* Auto-advance */
}

/* Scene 2: The Diss - DARKPH4NT from ROSEVALLEY-FXP mocks them (wait for key) */
static uint8_t intro_scene2_diss(void) {
    screen_clear();

    screen_print_string(1, 0, "#DEWDZKI-FXP", COLOR_YELLOW);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);

    screen_print_string(1, 3, "*** DARKPH4NT HAS JOINED", COLOR_GREEN);
    wait_frames(30);

    type_irc_line(5, "DARKPH4NT", "LMAO STILL SCANNING PUBS?",
                  COLOR_LIGHTRED, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(6, "DARKPH4NT", "ROSEVALLEY-FXP MOVED TO SMB",
                  COLOR_LIGHTRED, COLOR_WHITE);
    type_irc_line(7, "DARKPH4NT", "MONTHS AGO. YOU GUYS ARE DONE",
                  COLOR_LIGHTRED, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(9, "SICK0", "WTF IS SMB?",
                  COLOR_RED, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(10, "DARKPH4NT", "LOL EXACTLY. STAY IN THE",
                  COLOR_LIGHTRED, COLOR_WHITE);
    type_irc_line(11, "DARKPH4NT", "KIDDIE POOL NOOBS",
                  COLOR_LIGHTRED, COLOR_WHITE);
    wait_frames(15);

    screen_print_string(1, 13, "*** DARKPH4NT HAS LEFT", COLOR_RED);
    wait_frames(20);

    type_irc_line(15, "LEWIS", "...",
                  COLOR_YELLOW, COLOR_WHITE);
    type_irc_line(16, "FERRE070", "THAT GUY IS SUCH A TOOL",
                  COLOR_GREEN, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(18, "AXE", "HE'S NOT WRONG THOUGH",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(19, "AXE", "WE NEED TO LEVEL UP",
                  COLOR_CYAN, COLOR_WHITE);

    screen_print_centered(23, "PRESS ANY KEY", COLOR_YELLOW);

    return wait_for_key_or_quit();
}

/* Scene 3: SMB Lesson - Axe explains SMB hacking with sprites (wait for key) */
static uint8_t intro_scene3_smb(void) {
    screen_clear();

    /* Position Lewis sprite on left */
    sprite_set_multicolor_shared(10, 0);
    sprite_set_multicolor(SPRITE_LEWIS, 1);
    sprite_load(SPRITE_LEWIS, sprite_lewis, SPRITE_DATA_BASE + SPRITE_DATA_SIZE);
    sprite_set_color(SPRITE_LEWIS, 6);
    sprite_set_position(SPRITE_LEWIS, 40, 60);
    sprite_enable(SPRITE_LEWIS, 1);

    /* Slide Axe sprite in from right */
    sprite_set_multicolor(SPRITE_FTP, 1);
    sprite_load(SPRITE_FTP, sprite_axe, SPRITE_DATA_BASE + SPRITE_DATA_SIZE * 2);
    sprite_set_color(SPRITE_FTP, 3);  /* Cyan hood */
    sprite_slide_to(SPRITE_FTP, 320, 60, 280, 60, 20);

    screen_print_string(8, 1, "PRIVATE LESSON", COLOR_CYAN);
    screen_draw_hline(8, 2, 14, '-', COLOR_GRAY2);

    type_irc_line(4, "AXE", "SMB = SERVER MESSAGE BLOCK",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(5, "AXE", "WINDOWS FILE SHARING ON",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(6, "AXE", "PORT 139. MOST BOXES HAVE",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(7, "AXE", "NO PASSWORD OR ADMIN/ADMIN",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(9, "LEWIS", "SO WE CAN ACCESS THEIR",
                  COLOR_YELLOW, COLOR_WHITE);
    type_irc_line(10, "LEWIS", "HARD DRIVES REMOTELY?",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(12, "AXE", "EXACTLY. SCAN FOR PORT 139",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(13, "AXE", "FIND OPEN SHARES, USE THEM",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(14, "AXE", "AS DUMP SITES FOR RELEASES",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(16, "LEWIS", "LETS DO IT",
                  COLOR_YELLOW, COLOR_WHITE);

    screen_print_centered(23, "PRESS ANY KEY", COLOR_YELLOW);

    if (wait_for_key_or_quit()) {
        sprite_enable(SPRITE_LEWIS, 0);
        sprite_enable(SPRITE_FTP, 0);
        return 1;
    }

    sprite_enable(SPRITE_LEWIS, 0);
    sprite_enable(SPRITE_FTP, 0);
    return 0;
}

/* Scene 4: SMB Scan - Sine wave scanning animation (wait for key) */
static uint8_t intro_scene4_scan(void) {
    uint8_t frame;
    uint8_t x, sy;
    uint8_t prev_y[28];
    uint8_t phase;
    const uint8_t* spinner_frames[4];
    char ch;

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

    /* Wait for key with blinking text */
    frame = 0;
    while (!kbhit()) {
        if ((frame % BLINK_CYCLE) < (BLINK_CYCLE / 2)) {
            screen_print_centered(24, "PRESS ANY KEY", COLOR_YELLOW);
        } else {
            screen_print_centered(24, "             ", COLOR_BLACK);
        }
        waitvsync();
        frame++;
    }

    ch = cgetc();
    if (ch == 'q' || ch == 'Q') return 1;
    return 0;
}

/* Scene 5: The Forum - Axe explains underground forum (wait for key) */
static uint8_t intro_scene5_forum(void) {
    screen_clear();

    screen_print_string(1, 0, "#DEWDZKI-FXP", COLOR_YELLOW);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);

    type_irc_line(3, "AXE", "THERE'S MORE. YOU KNOW THE",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(4, "AXE", "UNDERGROUND FORUM?",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(6, "LEWIS", "HEARD OF IT. NEVER BEEN",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(8, "AXE", "THE GOOD TOOLS ARE THERE.",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(9, "AXE", "SCANNERS, EXPLOITS, CONFIGS",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(10, "AXE", "BUT EVERYTHING IS GATED BY",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(11, "AXE", "REPUTATION POINTS",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(13, "SICK0", "HOW DO WE GET REP?",
                  COLOR_RED, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(15, "AXE", "CONTRIBUTE. SHARE CONFIGS,",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(16, "AXE", "POST WORKING SCANS, HELP",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(17, "AXE", "OTHERS. THE MORE YOU GIVE",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(18, "AXE", "THE MORE ACCESS YOU GET",
                  COLOR_CYAN, COLOR_WHITE);

    screen_print_centered(23, "PRESS ANY KEY", COLOR_YELLOW);

    return wait_for_key_or_quit();
}

/* Scene 6: The Warning - Lewis warns about admins (wait for key) */
static uint8_t intro_scene6_warning(void) {
    screen_clear();

    /* Show Lewis sprite */
    sprite_set_multicolor_shared(10, 0);
    sprite_set_multicolor(SPRITE_LEWIS, 1);
    sprite_load(SPRITE_LEWIS, sprite_lewis, SPRITE_DATA_BASE + SPRITE_DATA_SIZE);
    sprite_set_color(SPRITE_LEWIS, 6);
    sprite_set_position(SPRITE_LEWIS, 40, 50);
    sprite_enable(SPRITE_LEWIS, 1);

    screen_print_string(1, 0, "#DEWDZKI-FXP", COLOR_YELLOW);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);

    type_irc_line(3, "LEWIS", "ONE THING THOUGH...",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(5, "LEWIS", "SMB BOXES HAVE ADMINS.",
                  COLOR_YELLOW, COLOR_WHITE);
    type_irc_line(6, "LEWIS", "THEY CHECK LOGS. IF THEY",
                  COLOR_YELLOW, COLOR_WHITE);
    type_irc_line(7, "LEWIS", "FIND OUR STUFF WE LOSE",
                  COLOR_YELLOW, COLOR_WHITE);
    type_irc_line(8, "LEWIS", "THE BOX",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(10, "AXE", "THATS WHY WE HIDE FILES IN",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(11, "AXE", "DEEP FOLDERS. NAMES LIKE",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(12, "AXE", "SYSTEM32\\DRIVERS\\ETC",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(14, "LEWIS", "AND IF THEY PATCH IT?",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(16, "AXE", "THEN WE FIND NEW BOXES.",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(17, "AXE", "THERE ARE MILLIONS OUT THERE",
                  COLOR_CYAN, COLOR_WHITE);

    screen_print_centered(23, "PRESS ANY KEY", COLOR_YELLOW);

    if (wait_for_key_or_quit()) {
        sprite_enable(SPRITE_LEWIS, 0);
        return 1;
    }

    sprite_enable(SPRITE_LEWIS, 0);
    return 0;
}

/* Scene 7: Crew Rallies - Everyone's on board (auto-advance) */
static uint8_t intro_scene7_rally(void) {
    screen_clear();

    screen_print_string(1, 0, "#DEWDZKI-FXP", COLOR_YELLOW);
    screen_draw_hline(1, 1, 38, '-', COLOR_GRAY2);

    type_irc_line(3, "LEWIS", "ALRIGHT CREW. NEW ERA.",
                  COLOR_YELLOW, COLOR_WHITE);
    type_irc_line(4, "LEWIS", "WE'RE GOING SMB",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(6, "SICK0", "HELL YEAH LETS WRECK IT",
                  COLOR_RED, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(7, "FERRE070", "DARKPH4NT CAN KISS MY ASS",
                  COLOR_GREEN, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(8, "ZZZ", "ARROWS",
                  COLOR_GRAY2, COLOR_WHITE);
    wait_frames(15);

    type_irc_line(10, "AXE", "ILL SET EVERYONE UP WITH",
                  COLOR_CYAN, COLOR_WHITE);
    type_irc_line(11, "AXE", "THE TOOLS. LETS DO THIS",
                  COLOR_CYAN, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(13, "YOU", "WHAT DO I DO?",
                  COLOR_WHITE, COLOR_LIGHTBLUE);
    wait_frames(15);

    type_irc_line(15, "LEWIS", "YOU? YOU'RE GONNA RUN",
                  COLOR_YELLOW, COLOR_WHITE);
    type_irc_line(16, "LEWIS", "THE WHOLE OPERATION.",
                  COLOR_YELLOW, COLOR_WHITE);
    wait_frames(40);

    return 0;  /* Auto-advance */
}

/* Scene 8: Title Card - "CHAPTER 2 / THE RISE" with blinking prompt (wait for key) */
static uint8_t intro_scene8_title(void) {
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

    if (intro_scene1_chat()) return 1;
    if (intro_scene2_diss()) return 1;
    if (intro_scene3_smb()) return 1;
    if (intro_scene4_scan()) return 1;
    if (intro_scene5_forum()) return 1;
    if (intro_scene6_warning()) return 1;
    if (intro_scene7_rally()) return 1;
    if (intro_scene8_title()) return 1;

    return 0;
}
