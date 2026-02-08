#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>
#include <peekpoke.h>

/* Scene 8: "DOSSIER: MOON" (Mystery) */
uint8_t scene8_dossier_moon(void) {
    uint8_t result;
    uint8_t i;

    clrscr();
    sprite_init();

    /* Dossier box */
    screen_draw_box(0, 0, 40, 3, COLOR_WHITE);
    screen_print_string(2, 1, "CLASSIFIED - DEWDZKI FXP DOSSIER", COLOR_WHITE);

    wait_frames(20);

    /* Load MOON sprite */
    sprite_set_multicolor_shared(10, 0);
    sprite_set_multicolor(0, 1);
    sprite_load(0, sprite_moon, SPRITE_DATA_BASE);
    sprite_set_color(0, 9);  /* Brown */
    sprite_fade_in(0, 60, 110);

    /* Dossier fields */
    type_dossier_line(5, "HANDLE:  ", "MOON", COLOR_GRAY2, COLOR_BROWN);
    wait_frames(15);
    type_dossier_line(7, "ROLE:    ", "SOCIAL ENGINEER", COLOR_GRAY2, COLOR_WHITE);
    wait_frames(15);

    /* STATUS: UNKNOWN in red + border flash */
    type_dossier_line(9, "STATUS:  ", "UNKNOWN", COLOR_GRAY2, COLOR_RED);
    screen_flash_border(COLOR_RED, COLOR_BLACK, 3);

    wait_frames(30);

    /* Flashback section */
    screen_print_string(16, 12, "----- FLASHBACK -----", COLOR_GRAY1);
    wait_frames(20);

    screen_print_string(16, 14, "*** HOUSE PARTY -", COLOR_GRAY1);
    screen_print_string(16, 15, "    AMSTERDAM", COLOR_GRAY1);
    wait_frames(30);
    type_text(16, 16, "\"I CAN DO IT\"", COLOR_GRAY1);
    wait_frames(25);
    type_text(16, 17, "\"DEWDZKI EXCLUSIVE\"", COLOR_GRAY1);

    wait_frames(40);

    /* Current fate */
    screen_print_string(16, 19, "----- CURRENT -----", COLOR_GRAY1);
    wait_frames(20);

    type_text(16, 21, "WENT DARK.", COLOR_LIGHTGREEN);
    wait_frames(60);

    type_text(16, 22, "NO CONTACT SINCE 2004.", COLOR_LIGHTGREEN);
    wait_frames(60);

    type_text(16, 23, "NOBODY KNOWS WHAT HAPPENED.", COLOR_LIGHTGREEN);

    /* Longest pause in the outro */
    wait_frames(80);

    /* Progressive sprite fade-out: blink cycles getting longer */
    /* on8/off4 */
    for (i = 0; i < 3; i++) {
        sprite_enable(0, 1);
        wait_frames(8);
        sprite_enable(0, 0);
        wait_frames(4);
    }
    /* on4/off8 */
    for (i = 0; i < 3; i++) {
        sprite_enable(0, 1);
        wait_frames(4);
        sprite_enable(0, 0);
        wait_frames(8);
    }
    /* on2/off12 */
    for (i = 0; i < 2; i++) {
        sprite_enable(0, 1);
        wait_frames(2);
        sprite_enable(0, 0);
        wait_frames(12);
    }
    /* Gone */
    sprite_enable(0, 0);

    /* Hold on empty screen */
    wait_frames(40);

    screen_print_centered(24, "PRESS ANY KEY", COLOR_YELLOW);
    result = wait_for_key_or_quit();

    screen_fade_to_black();
    return result;
}
