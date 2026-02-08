#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>

/* Scene 5: Meeting MOON */
uint8_t scene5_meet_moon(void) {
    uint8_t result;
    uint8_t i;

    clrscr();

    /* Scene header */
    textcolor(COLOR_LIGHTRED);
    gotoxy(5, 1);
    cputs("*** HOUSE PARTY - AMSTERDAM ***");
    screen_draw_hline(0, 2, 40, 0xC0, COLOR_GRAY1);

    wait_frames(30);

    /* AXE sprite on left */
    sprite_load(0, sprite_axe, SPRITE_DATA_BASE);
    sprite_set_color(0, COLOR_GRAY3);
    sprite_set_multicolor(0, 1);
    sprite_fade_in(0, 60, 100);

    wait_frames(20);

    /* MOON sprite on right */
    sprite_load(1, sprite_moon, SPRITE_DATA_BASE + SPRITE_DATA_SIZE);
    sprite_set_color(1, COLOR_BROWN);
    sprite_set_multicolor(1, 1);
    sprite_fade_in(1, 180, 100);

    wait_frames(30);

    /* Dialogue */
    type_text(2, 6, "AXE:", COLOR_CYAN);
    wait_frames(10);
    type_text(2, 7, "NEED CUSTOM BINDER", COLOR_WHITE);
    wait_frames(30);

    type_text(20, 9, "MOON:", COLOR_GREEN);
    wait_frames(10);
    type_text(20, 10, "WHAT FOR?", COLOR_WHITE);
    wait_frames(30);

    type_text(2, 12, "AXE:", COLOR_CYAN);
    wait_frames(10);
    type_text(2, 13, "P2P DISTRIBUTION", COLOR_WHITE);
    wait_frames(30);

    type_text(20, 15, "MOON:", COLOR_GREEN);
    wait_frames(10);
    type_text(20, 16, "RISKY...", COLOR_WHITE);
    wait_frames(25);

    type_text(2, 18, "AXE:", COLOR_CYAN);
    wait_frames(10);
    type_text(2, 19, "UNDETECTED ONLY", COLOR_WHITE);
    wait_frames(30);

    type_text(20, 21, "MOON:", COLOR_GREEN);
    wait_frames(10);
    type_text(20, 22, "I CAN DO IT", COLOR_WHITE);
    wait_frames(15);
    gotoxy(20, 23);
    textcolor(COLOR_GREEN);
    cputs("DEWDZKI EXCLUSIVE");
    wait_frames(30);

    type_text(2, 25, "AXE:", COLOR_CYAN);
    wait_frames(10);
    type_text(2, 26, "DEAL.", COLOR_LIGHTGREEN);
    wait_frames(20);

    /* Handshake animation */
    for (i = 0; i < 20; i++) {
        sprite_set_position(0, 60 + (i * 2), 100);
        sprite_set_position(1, 180 - (i * 2), 100);
        wait_frames(2);
    }

    wait_frames(20);

    /* Final text */
    textcolor(COLOR_GREEN);
    gotoxy(6, 29);
    cputs("*** MOON JOINS THE OPERATION ***");
    wait_frames(40);

    /* Prompt */
    textcolor(COLOR_YELLOW);
    gotoxy(8, 31);
    cputs("PRESS ANY KEY");

    result = wait_for_key_or_quit();

    /* Disable sprites */
    sprite_enable(0, 0);
    sprite_enable(1, 0);

    return result;
}
