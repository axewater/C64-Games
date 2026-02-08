#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>

/* Scene 7: "DOSSIER: RIZZ" */
uint8_t scene7_dossier_rizz(void) {
    uint8_t result;

    clrscr();
    sprite_init();

    /* Dossier box */
    screen_draw_box(0, 0, 40, 3, COLOR_WHITE);
    screen_print_string(2, 1, "CLASSIFIED - DEWDZKI FXP DOSSIER", COLOR_WHITE);

    wait_frames(20);

    /* Load RIZZ sprite */
    sprite_set_multicolor_shared(10, 0);
    sprite_set_multicolor(0, 1);
    sprite_load(0, sprite_rizz, SPRITE_DATA_BASE);
    sprite_set_color(0, 7);  /* Yellow */
    sprite_fade_in(0, 60, 110);

    /* Dossier fields */
    type_dossier_line(5, "HANDLE:  ", "RIZZ", COLOR_GRAY2, COLOR_YELLOW);
    wait_frames(15);
    type_dossier_line(7, "ROLE:    ", "CODER / BOT AUTHOR", COLOR_GRAY2, COLOR_WHITE);
    wait_frames(15);
    type_dossier_line(9, "STATUS:  ", "ACTIVE", COLOR_GRAY2, COLOR_GREEN);

    wait_frames(30);

    /* Flashback section - code snippets */
    screen_print_string(10, 12, "----- FLASHBACK -----", COLOR_GRAY1);
    wait_frames(20);

    type_text(10, 14, "// DEWDZKI BOT V1.0", COLOR_GREEN);
    wait_frames(20);
    type_text(10, 15, "CONNECT(\"203.12.45.78\")", COLOR_GREEN);
    wait_frames(20);
    type_text(10, 16, "JOIN(\"#DEWDZKI-BOTS\")", COLOR_GREEN);

    wait_frames(40);

    /* Current fate */
    screen_print_string(10, 18, "----- CURRENT -----", COLOR_GRAY1);
    wait_frames(20);

    type_text(10, 20, "IT SECURITY PROFESSIONAL", COLOR_LIGHTGREEN);
    wait_frames(25);
    type_text(10, 21, "BUILDS DEFENSES NOW", COLOR_LIGHTGREEN);
    wait_frames(25);
    type_text(10, 23, "SAME SKILLS. OTHER SIDE.", COLOR_YELLOW);

    wait_frames(50);

    screen_print_centered(24, "PRESS ANY KEY", COLOR_YELLOW);
    result = wait_for_key_or_quit();

    sprite_enable(0, 0);
    screen_fade_to_black();
    return result;
}
