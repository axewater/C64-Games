#include <conio.h>
#include <stdint.h>
#include "screen.h"
#include "player.h"
#include "input.h"
#include "gamestate.h"
#include "random.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"

int main(void) {
    uint8_t running = 1;
    uint8_t joy_enabled;
    uint8_t shoot_cooldown = 0;
    Player *p;
    InputState input;

    /* Initialize subsystems */
    screen_init();
    player_init();
    joy_enabled = input_init();
    gamestate_init();
    random_init();
    bullet_init();
    enemy_init();

    /* Draw title */
    screen_set_char(12, 2, 'M', COLOR_CYAN);
    screen_set_char(13, 2, 'O', COLOR_CYAN);
    screen_set_char(14, 2, 'V', COLOR_CYAN);
    screen_set_char(15, 2, 'E', COLOR_CYAN);
    screen_set_char(16, 2, ' ', COLOR_WHITE);
    screen_set_char(17, 2, 'G', COLOR_CYAN);
    screen_set_char(18, 2, 'A', COLOR_CYAN);
    screen_set_char(19, 2, 'M', COLOR_CYAN);
    screen_set_char(20, 2, 'E', COLOR_CYAN);

    /* Draw instructions at bottom */
    if (joy_enabled) {
        /* Show joystick + keyboard instructions */
        screen_set_char(6, 22, 'J', COLOR_GREEN);
        screen_set_char(7, 22, 'O', COLOR_GREEN);
        screen_set_char(8, 22, 'Y', COLOR_GREEN);
        screen_set_char(9, 22, '/', COLOR_WHITE);
        screen_set_char(10, 22, 'W', COLOR_GREEN);
        screen_set_char(11, 22, 'A', COLOR_GREEN);
        screen_set_char(12, 22, 'S', COLOR_GREEN);
        screen_set_char(13, 22, 'D', COLOR_GREEN);
        screen_set_char(14, 22, ':', COLOR_WHITE);
        screen_set_char(15, 22, 'M', COLOR_WHITE);
        screen_set_char(16, 22, 'O', COLOR_WHITE);
        screen_set_char(17, 22, 'V', COLOR_WHITE);
        screen_set_char(18, 22, 'E', COLOR_WHITE);

        screen_set_char(20, 22, 'F', COLOR_RED);
        screen_set_char(21, 22, 'I', COLOR_RED);
        screen_set_char(22, 22, 'R', COLOR_RED);
        screen_set_char(23, 22, 'E', COLOR_RED);
        screen_set_char(24, 22, '/', COLOR_WHITE);
        screen_set_char(25, 22, 'Q', COLOR_RED);
        screen_set_char(26, 22, ':', COLOR_WHITE);
        screen_set_char(27, 22, 'Q', COLOR_WHITE);
        screen_set_char(28, 22, 'U', COLOR_WHITE);
        screen_set_char(29, 22, 'I', COLOR_WHITE);
        screen_set_char(30, 22, 'T', COLOR_WHITE);
    } else {
        /* Show keyboard-only instructions */
        screen_set_char(8, 22, 'W', COLOR_GREEN);
        screen_set_char(9, 22, 'A', COLOR_GREEN);
        screen_set_char(10, 22, 'S', COLOR_GREEN);
        screen_set_char(11, 22, 'D', COLOR_GREEN);
        screen_set_char(12, 22, ':', COLOR_WHITE);
        screen_set_char(13, 22, 'M', COLOR_WHITE);
        screen_set_char(14, 22, 'O', COLOR_WHITE);
        screen_set_char(15, 22, 'V', COLOR_WHITE);
        screen_set_char(16, 22, 'E', COLOR_WHITE);

        screen_set_char(20, 22, 'Q', COLOR_RED);
        screen_set_char(21, 22, ':', COLOR_WHITE);
        screen_set_char(22, 22, 'Q', COLOR_WHITE);
        screen_set_char(23, 22, 'U', COLOR_WHITE);
        screen_set_char(24, 22, 'I', COLOR_WHITE);
        screen_set_char(25, 22, 'T', COLOR_WHITE);
    }

    /* Initial draw */
    player_update();

    /* Start game */
    gamestate_start_game();

    /* Spawn wave 1 */
    enemy_spawn_wave();

    /* Initial UI draw */
    gamestate_render_ui();

    /* Main game loop */
    while (running) {
        input_read(&input);

        if (game_state.state == STATE_PLAYING) {
            /* Handle movement */
            if (input.dx != 0 || input.dy != 0) {
                player_move(input.dx, input.dy);
                player_update();
            }

            /* Handle shooting */
            if (input.fire && shoot_cooldown == 0) {
                p = player_get();
                bullet_spawn(p->x, p->y, p->last_dir);
                shoot_cooldown = 10; // 10 frames between shots
            }

            /* Update cooldown */
            if (shoot_cooldown > 0) {
                shoot_cooldown--;
            }

            /* Update bullets */
            bullet_update_all();
            bullet_render_all();

            /* Update enemies */
            p = player_get();
            enemy_update_all(p->x, p->y);

            /* Check collisions */
            collision_update();

            /* Check for wave completion */
            if (enemy_count_active() == 0) {
                gamestate_next_wave();
                enemy_spawn_wave();
            }

            /* Update UI */
            gamestate_render_ui();
        } else if (game_state.state == STATE_GAME_OVER) {
            /* Show game over screen */
            gamestate_render_game_over();
        }

        if (input.quit) {
            running = 0;
        }
    }

    /* Cleanup */
    input_cleanup();

    /* Exit message */
    screen_clear();
    screen_set_char(10, 12, 'T', COLOR_WHITE);
    screen_set_char(11, 12, 'H', COLOR_WHITE);
    screen_set_char(12, 12, 'A', COLOR_WHITE);
    screen_set_char(13, 12, 'N', COLOR_WHITE);
    screen_set_char(14, 12, 'K', COLOR_WHITE);
    screen_set_char(15, 12, 'S', COLOR_WHITE);
    screen_set_char(16, 12, '!', COLOR_WHITE);

    return 0;
}
