#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include <conio.h>

/* Scene 2: "LAST DAYS IN IRC" - Final crew conversation */
uint8_t scene2_last_irc(void) {
    uint8_t result;

    clrscr();

    /* Channel header */
    screen_print_string(1, 0, "#DEWDZKI-FXP", COLOR_YELLOW);
    screen_draw_hline(0, 1, 40, 0xC0, COLOR_GRAY1);

    /* Topic */
    screen_print_string(1, 2, "*** TOPIC: ITS BEEN A GOOD RUN", COLOR_GRAY2);

    wait_frames(40);

    /* Somber start */
    type_irc_line(4, "SICK0", "C&C IS BARELY RESPONDING", COLOR_RED, COLOR_WHITE);
    wait_frames(40);

    type_irc_line(6, "AXE", "PROXY NEEDS STABLE CONNECTIONS", COLOR_CYAN, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(7, "AXE", "HOME USERS REBOOT THEIR PCS", COLOR_CYAN, COLOR_WHITE);
    wait_frames(40);

    type_irc_line(9, "RIZZ", "3 BOTS LEFT LOL", COLOR_YELLOW, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(11, "LEWIS", "SO THIS IS IT?", COLOR_CYAN, COLOR_WHITE);
    wait_frames(40);

    type_irc_line(13, "AXE", "YEAH. THE NETWORK IS DONE.", COLOR_CYAN, COLOR_WHITE);

    /* Long dramatic pause */
    wait_frames(80);

    /* Warm ending */
    type_irc_line(15, "SICK0", "IT WAS FUN THO", COLOR_RED, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(16, "RIZZ", "BEST CREW ON EFNET", COLOR_YELLOW, COLOR_WHITE);
    wait_frames(30);

    type_irc_line(17, "ZZZ", "ZZZ", COLOR_LIGHTRED, COLOR_WHITE);
    wait_frames(20);

    type_irc_line(18, "LEWIS", "HAHA ZZZ ALWAYS SLEEPING", COLOR_CYAN, COLOR_WHITE);

    /* Pause before MOON's quit */
    wait_frames(40);

    /* MOON's mysterious disconnect */
    screen_print_string(1, 20, "*** MOON HAS QUIT", COLOR_RED);
    screen_print_string(20, 20, "(CONNECTION RESET)", COLOR_GRAY2);

    wait_frames(60);

    /* Prompt */
    screen_print_centered(24, "PRESS ANY KEY", COLOR_YELLOW);
    result = wait_for_key_or_quit();

    screen_fade_to_black();
    return result;
}
