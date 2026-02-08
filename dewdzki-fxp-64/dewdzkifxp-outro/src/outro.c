#include "outro.h"
#include "scenes.h"
#include "sprite.h"
#include "screen.h"
#include <peekpoke.h>

void outro_init(void) {
    screen_init();
    sprite_init();

    /* Set multicolor mode colors */
    POKE(0xD025, COLOR_GRAY2);  /* MC0 - skin tone / gray */
    POKE(0xD026, COLOR_BLACK);  /* MC1 - dark features */
}

uint8_t outro_run(void) {
    outro_init();

    /* Scene 1: The Decline */
    if (scene1_decline()) return 1;

    /* Scene 2: Last Days in IRC */
    if (scene2_last_irc()) return 1;

    /* Scene 3: Dossier - SICK0 */
    if (scene3_dossier_sick0()) return 1;

    /* Scene 4: Dossier - AXE */
    if (scene4_dossier_axe()) return 1;

    /* Scene 5: Dossier - ZZZ */
    if (scene5_dossier_zzz()) return 1;

    /* Scene 6: Dossier - LEWIS */
    if (scene6_dossier_lewis()) return 1;

    /* Scene 7: Dossier - RIZZ */
    if (scene7_dossier_rizz()) return 1;

    /* Scene 8: Dossier - MOON */
    if (scene8_dossier_moon()) return 1;

    /* Scene 9: The Crew - Group Portrait */
    if (scene9_the_crew()) return 1;

    /* Scene 10: FIN */
    if (scene10_fin()) return 1;

    return 0;
}
