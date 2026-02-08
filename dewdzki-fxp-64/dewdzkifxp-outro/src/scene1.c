#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include <conio.h>
#include <peekpoke.h>

/* Scene 1: "THE DECLINE" - Botnet status monitor dying
 * Mirrors EP03 intro scene8's success counter, but in reverse */
uint8_t scene1_decline(void) {
    uint8_t result;
    static const uint16_t counts[] = {847, 612, 342, 128, 47, 12, 3};
    uint8_t i;

    clrscr();
    bordercolor(COLOR_BLACK);

    /* Header */
    screen_print_string(3, 1, "#DEWDZKI-BOTS STATUS MONITOR", COLOR_GREEN);
    screen_draw_hline(0, 2, 40, 0xC0, COLOR_GREEN);

    wait_frames(20);

    /* C&C info */
    screen_print_string(2, 4, "C&C SERVER: IRC.EFNET.ORG:6667", COLOR_GRAY2);
    screen_print_string(2, 5, "PROXY: 203.12.45.78:1080", COLOR_GRAY2);

    wait_frames(30);

    /* Bot disconnect messages appearing one by one */
    screen_print_string(2, 7, "[BOT-847] 195.88.23.145  TIMEOUT", COLOR_RED);
    wait_frames(12);
    screen_print_string(2, 8, "[BOT-612] 84.122.67.201  TIMEOUT", COLOR_RED);
    wait_frames(12);
    screen_print_string(2, 9, "[BOT-342] 217.45.99.12   TIMEOUT", COLOR_RED);
    wait_frames(12);
    screen_print_string(2, 10, "[BOT-128] 195.201.34.89  TIMEOUT", COLOR_RED);
    wait_frames(12);
    screen_print_string(2, 11, "[BOT-047] 88.156.23.67   TIMEOUT", COLOR_RED);
    wait_frames(12);
    screen_print_string(2, 12, "[BOT-012] 203.78.45.123  TIMEOUT", COLOR_RED);
    wait_frames(12);
    screen_print_string(2, 13, "[BOT-003] 142.88.67.234  TIMEOUT", COLOR_RED);
    wait_frames(20);

    /* Countdown counter */
    for (i = 0; i < 7; i++) {
        uint8_t color;

        if (counts[i] > 300) {
            color = COLOR_GREEN;
        } else if (counts[i] > 50) {
            color = COLOR_YELLOW;
        } else {
            color = COLOR_RED;
        }

        gotoxy(11, 15);
        textcolor(color);
        cprintf("ACTIVE BOTS: %-4u", counts[i]);
        wait_frames(25);
    }

    wait_frames(20);

    /* Progress bar - C&C proxy health draining */
    screen_print_string(2, 17, "C&C PROXY HEALTH:", COLOR_GRAY2);
    reverse_progress_bar(2, 18, 36, 3);

    wait_frames(20);

    /* Explanation */
    type_text(2, 20, "HOME USERS CANT KEEP SERVERS RUNNING", COLOR_GRAY2);
    wait_frames(30);
    type_text(2, 21, "CONNECTIONS DROP WHEN PCS SLEEP", COLOR_GRAY2);
    wait_frames(40);

    /* Prompt */
    screen_print_centered(24, "PRESS ANY KEY", COLOR_YELLOW);
    result = wait_for_key_or_quit();

    screen_fade_to_black();
    return result;
}
