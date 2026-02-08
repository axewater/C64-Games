#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>

/* Scene 4: "DOSSIER: AXE" */
uint8_t scene4_dossier_axe(void) {
    uint8_t result;

    clrscr();
    sprite_init();

    /* Dossier box */
    screen_draw_box(0, 0, 40, 3, COLOR_WHITE);
    screen_print_string(2, 1, "CLASSIFIED - DEWDZKI FXP DOSSIER", COLOR_WHITE);

    wait_frames(20);

    /* Load AXE sprite - slides in from right */
    sprite_set_multicolor_shared(10, 0);
    sprite_set_multicolor(0, 1);
    sprite_load(0, sprite_axe, SPRITE_DATA_BASE);
    sprite_set_color(0, 15);  /* Gray3 */
    sprite_slide_to(0, 340, 110, 60, 110, 30);

    /* Firewall sprite slides across bottom */
    sprite_set_multicolor(1, 1);
    sprite_load(1, sprite_firewall, SPRITE_DATA_BASE + 64);
    sprite_set_color(1, 2);  /* Red */
    sprite_slide_to(1, 340, 200, 60, 200, 40);

    /* Dossier fields */
    type_dossier_line(5, "HANDLE:  ", "AXE", COLOR_GRAY2, COLOR_CYAN);
    wait_frames(15);
    type_dossier_line(7, "ROLE:    ", "STRATEGIST / HACKER", COLOR_GRAY2, COLOR_WHITE);
    wait_frames(15);
    type_dossier_line(9, "STATUS:  ", "ACTIVE", COLOR_GRAY2, COLOR_GREEN);

    wait_frames(30);

    /* Flashback section */
    screen_print_string(10, 12, "----- FLASHBACK -----", COLOR_GRAY1);
    wait_frames(20);

    type_text(10, 14, "\"WE CANT SCAN IN ANYMORE\"", COLOR_GRAY1);
    wait_frames(25);
    type_text(10, 15, "\"SO WE BRING THEM TO US\"", COLOR_GRAY1);
    wait_frames(25);
    type_text(10, 16, "\"THIS IS THE NEW WAY.\"", COLOR_GRAY1);

    wait_frames(40);

    /* Current fate */
    screen_print_string(10, 18, "----- CURRENT -----", COLOR_GRAY1);
    wait_frames(20);

    type_text(10, 20, "WORKS IN A SOC", COLOR_LIGHTGREEN);
    wait_frames(20);
    type_text(10, 21, "SECURITY OPERATIONS CENTER", COLOR_LIGHTGREEN);

    /* Dramatic pause */
    wait_frames(50);
    type_text(10, 23, "NOW DEFENDS WHAT HE ONCE", COLOR_YELLOW);
    wait_frames(15);
    screen_print_string(10, 24, "ATTACKED.", COLOR_YELLOW);

    wait_frames(50);

    screen_print_centered(24, "PRESS ANY KEY", COLOR_YELLOW);
    result = wait_for_key_or_quit();

    sprite_enable(0, 0);
    sprite_enable(1, 0);
    screen_fade_to_black();
    return result;
}
