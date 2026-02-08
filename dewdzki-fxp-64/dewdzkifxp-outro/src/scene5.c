#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>

/* Scene 5: "DOSSIER: ZZZ" (Comedy Beat) */
uint8_t scene5_dossier_zzz(void) {
    uint8_t result;
    uint8_t i;

    clrscr();
    sprite_init();

    /* Dossier box */
    screen_draw_box(0, 0, 40, 3, COLOR_WHITE);
    screen_print_string(2, 1, "CLASSIFIED - DEWDZKI FXP DOSSIER", COLOR_WHITE);

    wait_frames(20);

    /* Load ZZZ sprite */
    sprite_set_multicolor_shared(10, 0);
    sprite_set_multicolor(0, 1);
    sprite_load(0, sprite_zzz, SPRITE_DATA_BASE);
    sprite_set_color(0, 4);  /* Purple */
    sprite_fade_in(0, 60, 108);

    /* Dozing bob animation - Y oscillates */
    for (i = 0; i < 40; i++) {
        uint8_t y = 108 + (i % 8 < 4 ? (i % 4) : (4 - (i % 4)));
        sprite_set_position(0, 60, y);
        wait_frames(2);
    }
    sprite_set_position(0, 60, 110);

    /* Dossier fields */
    type_dossier_line(5, "HANDLE:  ", "ZZZ", COLOR_GRAY2, COLOR_PURPLE);
    wait_frames(15);
    type_dossier_line(7, "ROLE:    ", "... UH ...", COLOR_GRAY2, COLOR_WHITE);
    wait_frames(15);
    type_dossier_line(9, "STATUS:  ", "ASLEEP", COLOR_GRAY2, COLOR_RED);

    wait_frames(30);

    /* Flashback section */
    screen_print_string(16, 12, "----- FLASHBACK -----", COLOR_GRAY1);
    wait_frames(20);

    type_text(16, 14, "\"NAT EVERYWHERE NOW\"", COLOR_GRAY1);
    wait_frames(25);
    type_text(16, 15, "\"MICROSOFT EMERGENCY", COLOR_GRAY1);
    wait_frames(15);
    type_text(16, 16, " PATCHES\"", COLOR_GRAY1);

    wait_frames(40);

    /* Current fate */
    screen_print_string(16, 18, "----- CURRENT -----", COLOR_GRAY1);
    wait_frames(20);

    type_text(16, 20, "WOKE UP LATE", COLOR_LIGHTGREEN);
    wait_frames(40);
    type_text(16, 21, "MISSED ALL THE PLANS", COLOR_LIGHTGREEN);
    wait_frames(40);
    type_text(16, 22, "STILL SMOKING", COLOR_LIGHTGREEN);

    wait_frames(30);

    /* Smoke text animation */
    for (i = 0; i < 60; i++) {
        uint8_t col = 3 + (i * 7) % 11;
        uint8_t row = 24 - (i % 3);
        screen_set_char(col, row, '~', COLOR_GRAY1);
        wait_frames(2);
        if (i > 3) {
            uint8_t old_col = 3 + ((i - 4) * 7) % 11;
            uint8_t old_row = 24 - ((i - 4) % 3);
            screen_set_char(old_col, old_row, ' ', COLOR_BLACK);
        }
    }
    /* Clean up smoke */
    for (i = 22; i < 25; i++) {
        screen_draw_hline(0, i, 14, ' ', COLOR_BLACK);
    }

    screen_print_centered(24, "PRESS ANY KEY", COLOR_YELLOW);
    result = wait_for_key_or_quit();

    sprite_enable(0, 0);
    screen_fade_to_black();
    return result;
}
