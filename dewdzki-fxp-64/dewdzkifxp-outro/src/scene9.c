#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>

/* Scene 9: "THE CREW" - Sine wave group portrait */
uint8_t scene9_the_crew(void) {
    uint8_t result;
    uint8_t frame, i, phase;
    uint8_t amplitude;

    /* 32-entry sine wave LUT (amplitude 26, range 0-52) */
    static const uint8_t wave_y[32] = {
        26, 31, 36, 40, 44, 48, 50, 52,
        52, 52, 50, 48, 44, 40, 36, 31,
        26, 21, 16, 12,  8,  4,  2,  1,
         0,  1,  2,  4,  8, 12, 16, 21
    };

    /* Sprite X positions: 6 sprites spread across screen */
    static const uint16_t sprite_x[6] = {50, 95, 140, 185, 230, 275};

    /* Character colors */
    static const uint8_t sprite_colors[6] = {9, 15, 4, 3, 7, 9};
    /* SICK0=brown, AXE=gray3, ZZZ=purple, LEWIS=cyan, RIZZ=yellow, MOON=brown */

    clrscr();
    sprite_init();

    screen_print_centered(1, "T H E   C R E W", COLOR_WHITE);
    screen_draw_hline(5, 2, 30, 0xC0, COLOR_GRAY1);

    wait_frames(20);

    /* Load all 6 sprites */
    sprite_set_multicolor_shared(10, 0);

    sprite_load(0, sprite_sick0, SPRITE_DATA_BASE);
    sprite_load(1, sprite_axe, SPRITE_DATA_BASE + 64);
    sprite_load(2, sprite_zzz, SPRITE_DATA_BASE + 128);
    sprite_load(3, sprite_lewis, SPRITE_DATA_BASE + 192);
    sprite_load(4, sprite_rizz, SPRITE_DATA_BASE + 256);
    sprite_load(5, sprite_moon, SPRITE_DATA_BASE + 320);

    for (i = 0; i < 6; i++) {
        sprite_set_multicolor(i, 1);
        sprite_set_color(i, sprite_colors[i]);
        sprite_enable(i, 1);
    }

    /* Phase 1: Sine wave animation (120 frames) */
    phase = 0;
    for (frame = 0; frame < 120; frame++) {
        for (i = 0; i < 6; i++) {
            uint8_t y_offset = wave_y[(32 + phase - i * 5) % 32];
            sprite_set_position(i, sprite_x[i], 90 + y_offset);
        }
        phase = (phase + 1) % 32;
        waitvsync();
    }

    /* Phase 2: Gradually reduce amplitude to 0 (40 frames) */
    for (frame = 0; frame < 40; frame++) {
        amplitude = 26 - (26 * frame / 40);
        for (i = 0; i < 6; i++) {
            uint8_t raw_y = wave_y[(32 + phase - i * 5) % 32];
            uint8_t scaled_y = 26 + (((int16_t)raw_y - 26) * amplitude / 26);
            sprite_set_position(i, sprite_x[i], 104 + scaled_y);
        }
        phase = (phase + 1) % 32;
        waitvsync();
    }

    /* Phase 3: Static group portrait at y=130 */
    for (i = 0; i < 6; i++) {
        sprite_set_position(i, sprite_x[i], 130);
    }

    wait_frames(20);

    /* Print character names below sprites */
    screen_print_string(4, 20, "SICK0", COLOR_RED);
    screen_print_string(10, 20, "AXE", COLOR_CYAN);
    screen_print_string(15, 20, "ZZZ", COLOR_PURPLE);
    screen_print_string(20, 20, "LEWIS", COLOR_CYAN);
    screen_print_string(26, 20, "RIZZ", COLOR_YELLOW);
    screen_print_string(32, 20, "MOON", COLOR_BROWN);

    wait_frames(30);

    screen_print_centered(22, "THE CREW. 1995-2004.", COLOR_WHITE);

    wait_frames(60);

    screen_print_centered(24, "PRESS ANY KEY", COLOR_YELLOW);
    result = wait_for_key_or_quit();

    for (i = 0; i < 6; i++) {
        sprite_enable(i, 0);
    }

    screen_fade_to_black();
    return result;
}
