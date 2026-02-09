#include "intro.h"
#include "sprite.h"
#include "sprite_data.h"
#include "screen.h"
#include <conio.h>
#include <string.h>

/* ===== ANIMATION HELPER FUNCTIONS ===== */

/* Wait specified number of VSync frames */
void wait_frames(uint8_t count) {
    uint8_t i;
    for (i = 0; i < count; i++) {
        waitvsync();
    }
}

/* Slide sprite smoothly from (start_x, start_y) to (end_x, end_y) over specified frames */
void sprite_slide_to(uint8_t sprite_num, uint16_t start_x, uint8_t start_y,
                     uint16_t end_x, uint8_t end_y, uint8_t frames) {
    uint8_t i;
    uint16_t x;
    uint8_t y;

    for (i = 0; i < frames; i++) {
        /* Linear interpolation */
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

/* Helper function to wait for key press or check for quit (Q key exits intro) */
static uint8_t wait_for_key_or_quit(void) {
    char ch;
    while (!kbhit()) {
        /* Wait for keypress */
    }
    ch = cgetc();  /* Store key once to avoid double-read bug */
    if (ch == 'q' || ch == 'Q') {
        return 1;  /* User pressed Q - quit intro */
    }
    return 0;  /* Continue to next scene */
}

/* Helper function to type out text character by character with typewriter effect */
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
        wait_frames(TYPING_DELAY);  /* Use configured typing speed */
    }
}

/* ===== SCENE FUNCTIONS ===== */

/* Scene 1: Library Exterior - Person walks to library entrance */
static uint8_t intro_scene1_library(void) {
    uint8_t i;
    uint16_t x;
    uint8_t y;
    uint8_t base_y;
    uint8_t hop_offset;
    const uint8_t* walk_frames[3];
    uint8_t frame_index;

    screen_clear();

    /* Draw library building using enhanced PETSCII art */
    screen_print_centered(5, "+----------------------+", COLOR_CYAN);
    screen_print_centered(6, "|   PUBLIC LIBRARY     |", COLOR_CYAN);
    screen_print_centered(7, "+----------------------+", COLOR_CYAN);
    screen_print_centered(8, "| [##][##][##][##][##] |", COLOR_WHITE);
    screen_print_centered(9, "| [##][##][##][##][##] |", COLOR_WHITE);
    screen_print_centered(10, "| [##][##][##][##][##] |", COLOR_WHITE);
    screen_print_centered(11, "+----------------------+", COLOR_CYAN);
    screen_print_centered(12, "|       .----.         |", COLOR_BLUE);
    screen_print_centered(13, "|       |    |         |", COLOR_BLUE);
    screen_print_centered(14, "+-------+----+---------+", COLOR_CYAN);

    /* Setup walking animation frames */
    walk_frames[0] = sprite_person_frame1;
    walk_frames[1] = sprite_person_frame2;
    walk_frames[2] = sprite_person_frame3;

    /* Setup multicolor mode for person sprite */
    sprite_set_multicolor_shared(10, 0);  /* MC0=light red (skin), MC1=black (hair/shoes) */
    sprite_set_multicolor(SPRITE_CURSOR, 1);  /* Enable multicolor for this sprite */

    /* Load initial frame */
    sprite_load(SPRITE_CURSOR, walk_frames[0], SPRITE_DATA_BASE);
    sprite_set_color(SPRITE_CURSOR, 14);  /* Light blue for clothes */

    /* Phase 1: Walk left along top (above building) */
    /* From top-right (280, 60) to top-left (80, 60) with hopping animation */
    for (i = 0; i < 50; i++) {
        x = 280 - ((280 - 80) * i / 50);
        base_y = 60;

        /* Hopping motion: every 10 frames = one hop cycle */
        hop_offset = (i % 10) < 5 ? 0 : 8;
        y = base_y + hop_offset;

        /* Cycle through walking animation frames (every 5 frames) */
        frame_index = (i / 5) % 3;
        sprite_load(SPRITE_CURSOR, walk_frames[frame_index], SPRITE_DATA_BASE);

        sprite_set_position(SPRITE_CURSOR, x, y);
        sprite_enable(SPRITE_CURSOR, 1);
        wait_frames(WALK_SPEED);
    }

    /* Phase 2: Walk down left side */
    /* From (80, 60) to (80, 140) */
    for (i = 0; i < 40; i++) {
        x = 80;
        base_y = 60 + ((140 - 60) * i / 40);

        hop_offset = (i % 10) < 5 ? 0 : 8;
        y = base_y + hop_offset;

        /* Cycle through walking animation frames */
        frame_index = (i / 5) % 3;
        sprite_load(SPRITE_CURSOR, walk_frames[frame_index], SPRITE_DATA_BASE);

        sprite_set_position(SPRITE_CURSOR, x, y);
        wait_frames(WALK_SPEED);
    }

    /* Phase 3: Walk right toward entrance */
    /* From (80, 140) to (160, 180) - diagonal approach to door */
    for (i = 0; i < 50; i++) {
        x = 80 + ((160 - 80) * i / 50);
        base_y = 140 + ((180 - 140) * i / 50);

        hop_offset = (i % 10) < 5 ? 0 : 8;
        y = base_y + hop_offset;

        /* Cycle through walking animation frames */
        frame_index = (i / 5) % 3;
        sprite_load(SPRITE_CURSOR, walk_frames[frame_index], SPRITE_DATA_BASE);

        sprite_set_position(SPRITE_CURSOR, x, y);
        wait_frames(WALK_SPEED);
    }

    /* Phase 4: Final hop into entrance */
    for (i = 0; i < 15; i++) {
        x = 160 + (i * 2);  /* Move into door */

        /* Big final hop */
        if (i < 7) {
            y = 180 - (i * 2);  /* Jump up */
        } else {
            y = 166 + ((i - 7) * 2);  /* Land down (disappear into door) */
        }

        /* Cycle through walking animation frames */
        frame_index = (i / 5) % 3;
        sprite_load(SPRITE_CURSOR, walk_frames[frame_index], SPRITE_DATA_BASE);

        sprite_set_position(SPRITE_CURSOR, x, y);
        wait_frames(WALK_SPEED);
    }

    /* Person enters library - fade out sprite */
    sprite_enable(SPRITE_CURSOR, 0);

    /* Wait a moment */
    wait_frames(30);

    screen_print_centered(17, "1995. YOUR LOCAL LIBRARY.", COLOR_WHITE);
    screen_print_centered(20, "PRESS ANY KEY", COLOR_YELLOW);

    return wait_for_key_or_quit();
}

/* Scene 2: At the Terminal - Card insertion and login */
static uint8_t intro_scene2_terminal(void) {
    uint16_t frame;
    char ch;

    screen_clear();

    /* Draw terminal */
    screen_draw_box(8, 5, 24, 10, COLOR_GRAY2);
    screen_print_string(10, 7, "INTERNET TERMINAL", COLOR_CYAN);
    screen_print_string(10, 9, "INSERT CARD...", COLOR_WHITE);

    /* Draw card slot */
    screen_print_string(15, 11, "[======]", COLOR_GRAY2);
    screen_print_string(15, 12, "[      ]", COLOR_GRAY2);

    screen_print_centered(17, "YOU SIT DOWN AT THE FREE", COLOR_WHITE);
    screen_print_centered(18, "INTERNET TERMINAL...", COLOR_WHITE);

    /* Animate card insertion using helper function */
    sprite_set_multicolor(SPRITE_CURSOR, 1);  /* Enable multicolor */
    sprite_load(SPRITE_CURSOR, sprite_card, SPRITE_DATA_BASE);
    sprite_set_color(SPRITE_CURSOR, 7);  /* Yellow border */
    sprite_slide_to(SPRITE_CURSOR, 185, 200, 185, 136, 50);

    /* Hold card in slot briefly */
    wait_frames(30);

    /* Card accepted - remove sprite */
    sprite_enable(SPRITE_CURSOR, 0);

    /* Change text to show card accepted */
    screen_print_string(10, 9, "CARD ACCEPTED  ", COLOR_GREEN);

    /* Wait a moment */
    wait_frames(20);

    screen_print_centered(21, "PRESS ANY KEY", COLOR_YELLOW);

    /* Load cursor sprite and animate blinking using BLINK_CYCLE constant */
    sprite_set_multicolor(SPRITE_CURSOR, 0);  /* Disable multicolor for cursor */
    sprite_load(SPRITE_CURSOR, sprite_cursor, SPRITE_DATA_BASE);
    sprite_set_color(SPRITE_CURSOR, COLOR_WHITE);
    sprite_set_position(SPRITE_CURSOR, 230, 140);

    frame = 0;
    while (!kbhit()) {
        /* Blink cursor on/off based on cycle */
        if ((frame % BLINK_CYCLE) < (BLINK_CYCLE / 2)) {
            sprite_enable(SPRITE_CURSOR, 1);
        } else {
            sprite_enable(SPRITE_CURSOR, 0);
        }
        waitvsync();
        frame++;
    }

    sprite_enable(SPRITE_CURSOR, 0);

    /* Read key and check for quit (fixed double-cgetc bug) */
    ch = cgetc();
    if (ch == 'q' || ch == 'Q') {
        return 1;
    }
    return 0;
}

/* Scene 3: Login Screen - Connection spinner animation */
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

    sprite_set_multicolor(SPRITE_CURSOR, 0);  /* Disable multicolor for spinner */
    sprite_set_color(SPRITE_CURSOR, COLOR_YELLOW);
    sprite_set_position(SPRITE_CURSOR, 180, 140);
    sprite_enable(SPRITE_CURSOR, 1);

    /* Animate spinner for 3 seconds (180 frames) using SPINNER_SPEED constant */
    for (frame = 0; frame < 180; frame++) {
        sprite_load(SPRITE_CURSOR, spinner_frames[(frame / SPINNER_SPEED) % 4], SPRITE_DATA_BASE);
        waitvsync();
    }

    sprite_enable(SPRITE_CURSOR, 0);

    screen_print_string(7, 12, "CONNECTED!", COLOR_GREEN);

    /* Wait 1 second */
    wait_frames(60);

    return 0;  /* Auto-advance to next scene */
}

/* Scene 4: Channel List - Arrow points to #hacking channel */
static uint8_t intro_scene4_channels(void) {
    screen_clear();

    screen_draw_box(2, 2, 36, 20, COLOR_CYAN);
    screen_print_string(4, 3, "CHANNEL LIST", COLOR_YELLOW);
    screen_print_string(4, 5, "#coding", COLOR_WHITE);
    screen_print_string(4, 6, "#linux", COLOR_WHITE);
    screen_print_string(4, 7, "#hacking", COLOR_CYAN);
    screen_print_string(4, 8, "#music", COLOR_WHITE);
    screen_print_string(4, 9, "#general", COLOR_WHITE);

    screen_print_centered(22, "PRESS ANY KEY", COLOR_YELLOW);

    /* Load arrow sprite and slide it to point at #coding first */
    sprite_set_multicolor(SPRITE_CURSOR, 1);  /* Enable multicolor */
    sprite_load(SPRITE_CURSOR, sprite_arrow, SPRITE_DATA_BASE);
    sprite_set_color(SPRITE_CURSOR, COLOR_YELLOW);
    sprite_slide_to(SPRITE_CURSOR, 280, 89, 200, 89, 20);

    /* Pause on first line, then move down to #hacking */
    wait_frames(30);
    sprite_slide_to(SPRITE_CURSOR, 200, 89, 200, 105, 15);

    /* Keep arrow visible and wait for key */
    if (wait_for_key_or_quit()) {
        sprite_enable(SPRITE_CURSOR, 0);
        return 1;
    }

    sprite_enable(SPRITE_CURSOR, 0);
    return 0;
}

/* Scene 5: Join #hacking - Chat messages appear */
static uint8_t intro_scene5_join(void) {
    screen_clear();

    screen_draw_box(2, 2, 36, 20, COLOR_CYAN);
    screen_print_string(4, 3, "#HACKING - CRACKING & PHREAKING", COLOR_YELLOW);

    screen_print_string(4, 6, "*** YOU JOINED #HACKING", COLOR_GREEN);

    /* Wait a moment */
    wait_frames(5);

    /* Type out chat messages with typewriter effect */
    type_text(4, 8, "<ZEROCOOL> SUP DUDES", COLOR_WHITE);
    type_text(4, 9, "<ACIDBURN> ANYONE GOT NEW WAREZ?", COLOR_WHITE);
    type_text(4, 10, "<PHANTOM> CHECK FTP.ELITE.ORG", COLOR_CYAN);

    /* Auto-advance to next scene after typing */
    return 0;
}

/* Scene 6: Lewis Appears - Mentor character introduction */
static uint8_t intro_scene6_lewis(void) {
    /* Load Lewis sprite and slide in from right to left, above his name */
    sprite_set_multicolor(SPRITE_LEWIS, 1);  /* Enable multicolor */
    sprite_load(SPRITE_LEWIS, sprite_lewis, SPRITE_DATA_BASE + SPRITE_DATA_SIZE);
    sprite_set_color(SPRITE_LEWIS, 6);  /* Blue for sunglasses */
    sprite_slide_to(SPRITE_LEWIS, 320, 155, 55, 155, 30);

    /* Lewis talks to the player */
    type_text(4, 16, "<LEWIS> HEY NEWBIE, FIRST TIME?", COLOR_YELLOW);
    type_text(4, 17, "<YOU> YEAH... WHAT'S A TOP SITE?", COLOR_WHITE);

    screen_print_centered(22, "PRESS ANY KEY", COLOR_YELLOW);

    if (wait_for_key_or_quit()) {
        sprite_enable(SPRITE_LEWIS, 0);
        return 1;
    }

    return 0;
}

/* Scene 7: TOP SITE Lesson - Lewis explains the warez scene */
static uint8_t intro_scene7_lesson(void) {
    screen_clear();

    /* Move Lewis to left side */
    sprite_set_position(SPRITE_LEWIS, 60, 80);
    sprite_enable(SPRITE_LEWIS, 1);

    /* Load FTP icon sprite */
    sprite_set_multicolor(SPRITE_FTP, 1);  /* Enable multicolor */
    sprite_load(SPRITE_FTP, sprite_ftp, SPRITE_DATA_BASE + SPRITE_DATA_SIZE * 2);
    sprite_set_color(SPRITE_FTP, 11);  /* Dark gray for box frame */
    sprite_set_position(SPRITE_FTP, 250, 120);
    sprite_enable(SPRITE_FTP, 1);

    /* Text box explaining warez scene */
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

    /* Keep FTP icon large (Y-expanded) */
    sprite_set_expand_y(SPRITE_FTP, 1);

    screen_print_centered(22, "PRESS ANY KEY", COLOR_YELLOW);

    if (wait_for_key_or_quit()) {
        sprite_set_expand_y(SPRITE_FTP, 0);
        sprite_enable(SPRITE_LEWIS, 0);
        sprite_enable(SPRITE_FTP, 0);
        return 1;
    }

    sprite_set_expand_y(SPRITE_FTP, 0);

    return 0;
}

/* Scene 8: FTP Scan - Radar sweep finds an FTP server */
static uint8_t intro_scene8_scan(void) {
    uint16_t frame;
    uint8_t radar_base_ptr;
    char ch;

    screen_clear();

    /* Disable all sprites from previous scenes */
    sprite_enable(SPRITE_LEWIS, 0);
    sprite_enable(SPRITE_FTP, 0);
    sprite_enable(SPRITE_CURSOR, 0);

    /* Draw title */
    screen_print_centered(1, "FTP PORT SCANNER", COLOR_YELLOW);

    /* Draw radar display box */
    screen_draw_box(5, 3, 30, 10, COLOR_CYAN);

    /* Draw small crosshair at center of radar box (col 20, row 8) */
    screen_set_char(19, 8, '-', COLOR_BLUE);
    screen_set_char(20, 7, '|', COLOR_BLUE);
    screen_set_char(20, 8, '+', COLOR_BLUE);
    screen_set_char(21, 8, '-', COLOR_BLUE);
    screen_set_char(20, 9, '|', COLOR_BLUE);

    /* Preload all 4 radar hand frames to consecutive memory for fast switching */
    sprite_load(SPRITE_CURSOR, sprite_radar_frame1, SPRITE_DATA_BASE);
    sprite_load(SPRITE_CURSOR, sprite_radar_frame2, SPRITE_DATA_BASE + 64);
    sprite_load(SPRITE_CURSOR, sprite_radar_frame3, SPRITE_DATA_BASE + 128);
    sprite_load(SPRITE_CURSOR, sprite_radar_frame4, SPRITE_DATA_BASE + 192);

    /* Setup radar hand sprite at center of box - expanded 2x for visibility */
    radar_base_ptr = (uint8_t)(SPRITE_DATA_BASE / 64);
    sprite_set_multicolor(SPRITE_CURSOR, 0);  /* Hires mode for clean line */
    sprite_set_color(SPRITE_CURSOR, COLOR_GREEN);
    sprite_set_expand_y(SPRITE_CURSOR, 1);
    *VIC_SPRITE_EXPAND_X |= (1 << SPRITE_CURSOR);  /* X expand too */
    /* Center expanded sprite (48x42): box center pixel (184,114), offset by half */
    sprite_set_position(SPRITE_CURSOR, 161, 93);
    sprite_enable(SPRITE_CURSOR, 1);

    /* Main radar sweep with scanning text events */
    for (frame = 0; frame < 310; frame++) {
        /* Rotate radar hand - change frame every 10 vsyncs (clockwise: |, \, -, /) */
        SPRITE_PTRS[SPRITE_CURSOR] = radar_base_ptr + ((frame / 10) % 4);

        /* Scanning text events at specific frames */
        if (frame == 80)
            screen_print_string(3, 14, "SCANNING 207.15.33.x...", COLOR_WHITE);
        if (frame == 110)
            screen_print_string(5, 15, "TIMEOUT", COLOR_RED);
        if (frame == 170)
            screen_print_string(3, 16, "SCANNING 195.42.17.x...", COLOR_WHITE);
        if (frame == 200)
            screen_print_string(5, 17, "TIMEOUT", COLOR_RED);
        if (frame == 250)
            screen_print_string(3, 18, "SCANNING 207.15.33.42:21", COLOR_WHITE);
        if (frame == 290)
            screen_print_string(5, 19, "*** FTP FOUND! ***", COLOR_GREEN);

        waitvsync();
    }

    /* Stop radar hand and reset expansion */
    sprite_enable(SPRITE_CURSOR, 0);
    sprite_set_expand_y(SPRITE_CURSOR, 0);
    *VIC_SPRITE_EXPAND_X &= ~(1 << SPRITE_CURSOR);

    /* Show blinking dot on radar where FTP was detected */
    sprite_load(SPRITE_FTP, sprite_dot, SPRITE_DATA_BASE + 256);
    sprite_set_multicolor(SPRITE_FTP, 0);  /* Hires mode */
    sprite_set_color(SPRITE_FTP, COLOR_GREEN);
    sprite_set_position(SPRITE_FTP, 195, 90);  /* Upper-right of radar center */
    sprite_enable(SPRITE_FTP, 1);

    screen_print_centered(22, "PRESS ANY KEY", COLOR_YELLOW);

    /* Blink dot while waiting for keypress */
    frame = 0;
    while (!kbhit()) {
        if ((frame % 20) < 10) {
            sprite_enable(SPRITE_FTP, 1);
        } else {
            sprite_enable(SPRITE_FTP, 0);
        }
        waitvsync();
        frame++;
    }

    sprite_enable(SPRITE_FTP, 0);

    /* Read key and check for quit */
    ch = cgetc();
    if (ch == 'q' || ch == 'Q') {
        return 1;
    }

    /* Final transition message */
    screen_clear();
    screen_print_centered(12, "WELCOME TO THE SCENE...", COLOR_CYAN);
    wait_frames(120);

    return 0;  /* Intro complete */
}

/* ===== INITIALIZATION AND MAIN SEQUENCE ===== */

void intro_init(void) {
    screen_init();
    sprite_init();
}

/*
 * Run the complete 8-scene intro sequence
 *
 * Scene flow:
 * 1. Library Exterior - Person walks to library and enters
 * 2. Terminal Login - Card insertion and blinking cursor
 * 3. IRC Connection - Spinner animation while connecting
 * 4. Channel List - Arrow points to #hacking channel
 * 5. Join #hacking - Chat messages appear with typing effect
 * 6. Lewis Appears - Mentor character slides in and speaks
 * 7. TOP SITE Lesson - Lewis explains warez scene with pulsing FTP icon
 * 8. FTP Scan Demo - Radar sweep finds an FTP server, ending message
 *
 * Each scene can be exited early by pressing 'Q' key
 *
 * Returns: 0 if completed successfully, 1 if user quit with Q
 */
uint8_t intro_run(void) {
    intro_init();

    /* Run each scene in sequence - exit early if user presses Q */
    if (intro_scene1_library()) return 1;
    if (intro_scene2_terminal()) return 1;
    if (intro_scene3_login()) return 1;
    if (intro_scene4_channels()) return 1;
    if (intro_scene5_join()) return 1;
    if (intro_scene6_lewis()) return 1;
    if (intro_scene7_lesson()) return 1;
    if (intro_scene8_scan()) return 1;

    return 0;  /* Intro completed successfully */
}
