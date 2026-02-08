#include "game.h"
#include "irc.h"
#include "level.h"
#include "screen.h"
#include "sprite.h"
#include "chainload.h"
#include <conio.h>
#include <peekpoke.h>

/* Show title screen */
static void show_title(void) {
    screen_init();
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    sprite_init();

    screen_print_centered(5, "DEWDZKI FXP-64", COLOR_BLUE);
    screen_print_centered(7, "CHAPTER 3", COLOR_GRAY2);

    screen_print_centered(10, "P A C K E T  S T O R M", COLOR_RED);

    screen_print_centered(14, "DEFEND THE C&C SERVER", COLOR_GRAY2);
    screen_print_centered(15, "FROM DETECTION PACKETS", COLOR_GRAY2);

    screen_print_centered(18, "USE JOYSTICK TO MOVE SHIELD", COLOR_YELLOW);
    screen_print_centered(19, "DEFLECT SCANS AWAY FROM SERVER", COLOR_YELLOW);

    screen_print_centered(22, "PRESS ANY KEY TO START", COLOR_WHITE);
    screen_print_centered(23, "Q TO QUIT", COLOR_GRAY1);

    while (!kbhit()) {}
    if (cgetc() == 'q') {
        game_state = STATE_QUIT;
    } else {
        game_state = STATE_IRC;
    }
}

/* Show game over screen */
static void show_gameover(void) {
    sprite_init();
    screen_init();
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_RED);

    screen_print_centered(8, "*** SERVER COMPROMISED ***", COLOR_RED);
    screen_print_centered(10, "THE C&C SERVER WAS DETECTED", COLOR_GRAY2);
    screen_print_centered(11, "842 BOTS DISCONNECTED", COLOR_GRAY2);

    screen_print_centered(15, "R - RETRY LEVEL", COLOR_YELLOW);
    screen_print_centered(16, "Q - QUIT", COLOR_GRAY1);

    for (;;) {
        uint8_t key;
        while (!kbhit()) {}
        key = cgetc();
        if (key == 'r' || key == 'R') {
            game_state = STATE_IRC;
            return;
        }
        if (key == 'q' || key == 'Q') {
            game_state = STATE_QUIT;
            return;
        }
    }
}

/* Show victory and chain to outro */
static void show_victory(void) {
    uint8_t key;

    sprite_init();
    irc_show_victory();

    screen_print_centered(22, "PRESS ANY KEY", COLOR_WHITE);

    while (!kbhit()) {}
    key = cgetc();
    (void)key;

    /* Chain load to outro (credits) */
    screen_init();
    screen_print_centered(12, "LOADING...", COLOR_GRAY2);

    chainload("outro");
}

int main(void) {
    game_init();

    /* Title screen */
    show_title();

    /* Main state machine */
    while (game_state != STATE_QUIT) {
        switch (game_state) {
            case STATE_IRC:
                if (irc_show_briefing(current_level)) {
                    game_state = STATE_QUIT;
                } else if (current_level >= NUM_LEVELS) {
                    game_state = STATE_BOSS;
                } else {
                    game_state = STATE_PLAYING;
                }
                break;

            case STATE_PLAYING:
                game_start_level();
                while (game_state == STATE_PLAYING) {
                    game_run_frame();
                }
                break;

            case STATE_BOSS:
                game_start_boss();
                while (game_state == STATE_BOSS) {
                    game_run_boss_frame();
                }
                break;

            case STATE_GAMEOVER:
                show_gameover();
                break;

            case STATE_VICTORY:
                show_victory();
                game_state = STATE_QUIT;
                break;
        }
    }

    /* Clean exit */
    sprite_init();
    screen_init();

    return 0;
}
