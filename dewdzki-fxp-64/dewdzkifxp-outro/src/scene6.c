#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>

/* Scene 6: "DOSSIER: LEWIS" */
uint8_t scene6_dossier_lewis(void) {
    uint8_t result;

    clrscr();
    sprite_init();

    /* Dossier box */
    screen_draw_box(0, 0, 40, 3, COLOR_WHITE);
    screen_print_string(2, 1, "CLASSIFIED - DEWDZKI FXP DOSSIER", COLOR_WHITE);

    wait_frames(20);

    /* Load LEWIS sprite */
    sprite_set_multicolor_shared(10, 0);
    sprite_set_multicolor(0, 1);
    sprite_load(0, sprite_lewis, SPRITE_DATA_BASE);
    sprite_set_color(0, 3);  /* Cyan */
    sprite_fade_in(0, 60, 110);

    /* Dossier fields */
    type_dossier_line(5, "HANDLE:  ", "LEWIS", COLOR_GRAY2, COLOR_CYAN);
    wait_frames(15);
    type_dossier_line(7, "ROLE:    ", "MENTOR / VETERAN", COLOR_GRAY2, COLOR_WHITE);
    wait_frames(15);
    type_dossier_line(9, "STATUS:  ", "ACTIVE", COLOR_GRAY2, COLOR_GREEN);

    wait_frames(30);

    /* Flashback section */
    screen_print_string(16, 12, "----- FLASHBACK -----", COLOR_GRAY1);
    wait_frames(20);

    screen_print_string(16, 14, "#HACKING - 1995", COLOR_YELLOW);
    wait_frames(30);
    type_text(16, 15, "\"HEY NEWBIE, FIRST TIME?\"", COLOR_GRAY1);
    wait_frames(25);
    type_text(16, 16, "\"LET ME SHOW YOU...\"", COLOR_GRAY1);

    wait_frames(40);

    /* Current fate */
    screen_print_string(16, 18, "----- CURRENT -----", COLOR_GRAY1);
    wait_frames(20);

    type_text(16, 20, "STILL IN HIS KIBBUTZ", COLOR_LIGHTGREEN);
    wait_frames(20);
    type_text(16, 21, "IN ISRAEL", COLOR_LIGHTGREEN);

    /* Warm pause */
    wait_frames(50);
    type_text(16, 23, "SOME THINGS NEVER CHANGE.", COLOR_YELLOW);

    wait_frames(50);

    screen_print_centered(24, "PRESS ANY KEY", COLOR_YELLOW);
    result = wait_for_key_or_quit();

    sprite_enable(0, 0);
    screen_fade_to_black();
    return result;
}
