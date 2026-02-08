#include "intro.h"
#include "scenes.h"
#include "sprite.h"
#include "screen.h"
#include <peekpoke.h>

/* Initialize intro system */
void intro_init(void) {
    screen_init();
    sprite_init();

    /* Set multicolor mode colors */
    POKE(0xD025, COLOR_GRAY2);  /* MC0 - skin tone / gray */
    POKE(0xD026, COLOR_BLACK);  /* MC1 - dark features */
}

/* Run the full intro sequence */
uint8_t intro_run(void) {
    intro_init();

    /* Scene 1: Failed Exploits */
    if (scene1_failed_exploits()) return 1;

    /* Scene 2: MS Blaster News */
    if (scene2_blaster_news()) return 1;

    /* Scene 3: The Lockdown */
    if (scene3_lockdown()) return 1;

    /* Scene 4: AXE's Pitch */
    if (scene4_axe_pitch()) return 1;

    /* Scene 5: Meeting MOON */
    if (scene5_meet_moon()) return 1;

    /* Scene 6: RIZZ Codes Bot */
    if (scene6_rizz_codes()) return 1;

    /* Scene 7: P2P Upload */
    if (scene7_p2p_upload()) return 1;

    /* Scene 8: Success Counter */
    if (scene8_success_counter()) return 1;

    /* Scene 9: Title Card */
    if (scene9_title()) return 1;

    return 0;  /* Completed normally */
}
