#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>

/* Scene 3: "DOSSIER: SICK0" */
uint8_t scene3_dossier_sick0(void) {
    uint8_t result;

    clrscr();
    sprite_init();

    /* Dossier box */
    screen_draw_box(0, 0, 40, 3, COLOR_WHITE);
    screen_print_string(2, 1, "CLASSIFIED - DEWDZKI FXP DOSSIER", COLOR_WHITE);

    wait_frames(20);

    /* Load and show sprite */
    sprite_set_multicolor_shared(10, 0);
    sprite_set_multicolor(0, 1);
    sprite_load(0, sprite_sick0, SPRITE_DATA_BASE);
    sprite_set_color(0, 9);  /* Brown */
    sprite_fade_in(0, 60, 110);

    /* Dossier fields */
    type_dossier_line(5, "HANDLE:  ", "SICK0", COLOR_GRAY2, COLOR_RED);
    wait_frames(15);
    type_dossier_line(7, "ROLE:    ", "LEADER / SCANNER", COLOR_GRAY2, COLOR_WHITE);
    wait_frames(15);
    type_dossier_line(9, "STATUS:  ", "ACTIVE", COLOR_GRAY2, COLOR_GREEN);

    wait_frames(30);

    /* Flashback section */
    screen_print_string(16, 12, "----- FLASHBACK -----", COLOR_GRAY1);
    wait_frames(20);

    type_text(16, 14, "\"PUB FTPS ARE DYING\"", COLOR_GRAY1);
    wait_frames(30);
    type_text(16, 15, "\"URGENT - MS BLASTER WORM\"", COLOR_GRAY1);

    wait_frames(40);

    /* Current fate */
    screen_print_string(16, 18, "----- CURRENT -----", COLOR_GRAY1);
    wait_frames(20);

    type_text(16, 20, "TOP TIER DEVOPS ENGINEER", COLOR_LIGHTGREEN);
    wait_frames(25);
    type_text(16, 21, "BUILDS INFRA THAT NEVER", COLOR_LIGHTGREEN);
    wait_frames(15);
    type_text(16, 22, "GOES DOWN.", COLOR_LIGHTGREEN);

    /* Comedic pause */
    wait_frames(40);
    type_text(27, 22, " IRONIC.", COLOR_YELLOW);

    wait_frames(50);

    screen_print_centered(24, "PRESS ANY KEY", COLOR_YELLOW);
    result = wait_for_key_or_quit();

    sprite_enable(0, 0);
    screen_fade_to_black();
    return result;
}
