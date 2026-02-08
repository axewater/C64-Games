#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>

/* Scene 4: AXE's Pitch */
uint8_t scene4_axe_pitch(void) {
    uint8_t result;

    clrscr();

    /* IRC header */
    textcolor(COLOR_YELLOW);
    gotoxy(12, 1);
    cputs("#DEWDZKI-FXP");
    screen_draw_hline(0, 2, 40, 0xC0, COLOR_GRAY1);

    wait_frames(20);

    /* AXE sprite appears */
    sprite_load(0, sprite_axe, SPRITE_DATA_BASE);
    sprite_set_color(0, COLOR_GRAY3);
    sprite_set_multicolor(0, 1);
    sprite_fade_in(0, 80, 100);

    wait_frames(20);

    /* AXE's pitch */
    type_irc_line(4, "AXE", "I HAVE A PLAN", COLOR_CYAN, COLOR_WHITE);
    wait_frames(50);  /* Dramatic pause - major announcement */

    type_irc_line(6, "AXE", "WE CANT SCAN IN ANYMORE", COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);
    type_irc_line(7, "AXE", "SO WE BRING THEM TO US", COLOR_CYAN, COLOR_WHITE);
    wait_frames(50);  /* Dramatic pause - key strategy reveal */

    type_irc_line(9, "LEWIS", "HOW?", COLOR_YELLOW, COLOR_WHITE);
    wait_frames(35);  /* Suspense build */

    type_irc_line(11, "AXE", "P2P NETWORKS. EDONKEY.", COLOR_CYAN, COLOR_WHITE);
    wait_frames(50);  /* Dramatic pause - solution reveal */

    type_irc_line(13, "AXE", "TROJANIZE POPULAR APPS", COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);
    type_irc_line(14, "AXE", "WINZIP, WINRAR, CUTEFTP", COLOR_CYAN, COLOR_WHITE);
    wait_frames(40);  /* Let the implications sink in */

    type_irc_line(16, "SICK0", "BIND OUR INSTALLER?", COLOR_RED, COLOR_WHITE);
    wait_frames(35);

    type_irc_line(18, "AXE", "EXACTLY. NEED A BINDER.", COLOR_CYAN, COLOR_WHITE);
    wait_frames(40);

    textcolor(COLOR_LIGHTGREEN);
    gotoxy(0, 20);
    cputs("<AXE> THIS IS THE NEW WAY.");
    wait_frames(60);  /* Dramatic pause - climax of pitch */

    /* Prompt */
    textcolor(COLOR_YELLOW);
    gotoxy(8, 24);
    cputs("PRESS ANY KEY");

    result = wait_for_key_or_quit();

    /* Disable sprite */
    sprite_enable(0, 0);

    return result;
}
