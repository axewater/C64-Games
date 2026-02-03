#include "gamestate.h"
#include "screen.h"
#include <conio.h>

GameState game_state;

void gamestate_init(void) {
    game_state.state = STATE_MENU;
    game_state.score = 0;
    game_state.lives = 3;
    game_state.wave = 0;
    game_state.enemies_killed_this_wave = 0;
    game_state.enemies_to_spawn = 0;
}

void gamestate_start_game(void) {
    game_state.state = STATE_PLAYING;
    game_state.score = 0;
    game_state.lives = 3;
    game_state.wave = 1;
    game_state.enemies_killed_this_wave = 0;
    game_state.enemies_to_spawn = 3; // Wave 1 starts with 3 enemies
}

void gamestate_game_over(void) {
    game_state.state = STATE_GAME_OVER;
}

void gamestate_next_wave(void) {
    game_state.wave++;
    game_state.enemies_killed_this_wave = 0;

    // Increase enemies per wave, max 7
    game_state.enemies_to_spawn = game_state.wave + 2;
    if (game_state.enemies_to_spawn > 7) {
        game_state.enemies_to_spawn = 7;
    }
}

void gamestate_add_score(uint16_t points) {
    game_state.score += points;
}

void gamestate_lose_life(void) {
    if (game_state.lives > 0) {
        game_state.lives--;
        if (game_state.lives == 0) {
            gamestate_game_over();
        }
    }
}

void gamestate_render_ui(void) {
    uint8_t i;

    /* Row 0: SCORE: 00000  WAVE: 01  LIVES: 3 */

    /* SCORE: */
    screen_set_char(0, 0, 'S', COLOR_WHITE);
    screen_set_char(1, 0, 'C', COLOR_WHITE);
    screen_set_char(2, 0, 'O', COLOR_WHITE);
    screen_set_char(3, 0, 'R', COLOR_WHITE);
    screen_set_char(4, 0, 'E', COLOR_WHITE);
    screen_set_char(5, 0, ':', COLOR_WHITE);
    screen_print_number(7, 0, game_state.score, 5, COLOR_YELLOW);

    /* WAVE: */
    screen_set_char(15, 0, 'W', COLOR_WHITE);
    screen_set_char(16, 0, 'A', COLOR_WHITE);
    screen_set_char(17, 0, 'V', COLOR_WHITE);
    screen_set_char(18, 0, 'E', COLOR_WHITE);
    screen_set_char(19, 0, ':', COLOR_WHITE);
    screen_print_number(21, 0, game_state.wave, 2, COLOR_CYAN);

    /* LIVES: */
    screen_set_char(26, 0, 'L', COLOR_WHITE);
    screen_set_char(27, 0, 'I', COLOR_WHITE);
    screen_set_char(28, 0, 'V', COLOR_WHITE);
    screen_set_char(29, 0, 'E', COLOR_WHITE);
    screen_set_char(30, 0, 'S', COLOR_WHITE);
    screen_set_char(31, 0, ':', COLOR_WHITE);

    /* Draw hearts for lives */
    for (i = 0; i < 3; i++) {
        if (i < game_state.lives) {
            screen_set_char(33 + i, 0, 3, COLOR_RED); /* Heart character */
        } else {
            screen_set_char(33 + i, 0, ' ', COLOR_WHITE);
        }
    }
}

void gamestate_render_game_over(void) {
    /* Clear center area */
    screen_set_char(12, 10, 'G', COLOR_RED);
    screen_set_char(13, 10, 'A', COLOR_RED);
    screen_set_char(14, 10, 'M', COLOR_RED);
    screen_set_char(15, 10, 'E', COLOR_RED);
    screen_set_char(16, 10, ' ', COLOR_WHITE);
    screen_set_char(17, 10, 'O', COLOR_RED);
    screen_set_char(18, 10, 'V', COLOR_RED);
    screen_set_char(19, 10, 'E', COLOR_RED);
    screen_set_char(20, 10, 'R', COLOR_RED);

    /* Show final score */
    screen_set_char(10, 12, 'F', COLOR_WHITE);
    screen_set_char(11, 12, 'I', COLOR_WHITE);
    screen_set_char(12, 12, 'N', COLOR_WHITE);
    screen_set_char(13, 12, 'A', COLOR_WHITE);
    screen_set_char(14, 12, 'L', COLOR_WHITE);
    screen_set_char(15, 12, ' ', COLOR_WHITE);
    screen_set_char(16, 12, 'S', COLOR_WHITE);
    screen_set_char(17, 12, 'C', COLOR_WHITE);
    screen_set_char(18, 12, 'O', COLOR_WHITE);
    screen_set_char(19, 12, 'R', COLOR_WHITE);
    screen_set_char(20, 12, 'E', COLOR_WHITE);
    screen_set_char(21, 12, ':', COLOR_WHITE);
    screen_print_number(23, 12, game_state.score, 5, COLOR_YELLOW);

    /* Show wave reached */
    screen_set_char(11, 14, 'W', COLOR_WHITE);
    screen_set_char(12, 14, 'A', COLOR_WHITE);
    screen_set_char(13, 14, 'V', COLOR_WHITE);
    screen_set_char(14, 14, 'E', COLOR_WHITE);
    screen_set_char(15, 14, ' ', COLOR_WHITE);
    screen_set_char(16, 14, 'R', COLOR_WHITE);
    screen_set_char(17, 14, 'E', COLOR_WHITE);
    screen_set_char(18, 14, 'A', COLOR_WHITE);
    screen_set_char(19, 14, 'C', COLOR_WHITE);
    screen_set_char(20, 14, 'H', COLOR_WHITE);
    screen_set_char(21, 14, 'E', COLOR_WHITE);
    screen_set_char(22, 14, 'D', COLOR_WHITE);
    screen_set_char(23, 14, ':', COLOR_WHITE);
    screen_print_number(25, 14, game_state.wave, 2, COLOR_CYAN);

    /* Quit instruction */
    screen_set_char(13, 18, 'P', COLOR_GREEN);
    screen_set_char(14, 18, 'R', COLOR_GREEN);
    screen_set_char(15, 18, 'E', COLOR_GREEN);
    screen_set_char(16, 18, 'S', COLOR_GREEN);
    screen_set_char(17, 18, 'S', COLOR_GREEN);
    screen_set_char(18, 18, ' ', COLOR_WHITE);
    screen_set_char(19, 18, 'Q', COLOR_YELLOW);
    screen_set_char(20, 18, ' ', COLOR_WHITE);
    screen_set_char(21, 18, 'T', COLOR_GREEN);
    screen_set_char(22, 18, 'O', COLOR_GREEN);
    screen_set_char(23, 18, ' ', COLOR_WHITE);
    screen_set_char(24, 18, 'Q', COLOR_GREEN);
    screen_set_char(25, 18, 'U', COLOR_GREEN);
    screen_set_char(26, 18, 'I', COLOR_GREEN);
    screen_set_char(27, 18, 'T', COLOR_GREEN);
}
