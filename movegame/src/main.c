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
#include "room.h"

int main(void) {
    uint8_t running = 1;
    uint8_t joy_enabled;
    uint8_t shoot_cooldown = 0;
    uint8_t exit_dir;
    uint8_t next_room;
    Player *p;
    InputState input;

    /* Initialize subsystems */
    screen_init();
    room_init();
    player_init();
    joy_enabled = input_init();
    gamestate_init();
    random_init();
    bullet_init();
    enemy_init();

    /* Render initial room */
    room_render();

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

            /* Check for wave completion and unlock exits */
            if (enemy_count_active() == 0) {
                room_set_exits_locked(0);  /* Unlock exits when all enemies dead */

                /* Check if player is at an exit */
                p = player_get();
                exit_dir = room_check_exit(p->x, p->y);

                if (exit_dir != EXIT_NONE) {
                    /* Determine next room */
                    next_room = game_state.current_room + 1;
                    if (next_room >= MAX_ROOMS) {
                        next_room = 0;  /* Loop back to start */
                    }

                    /* Clear enemies and bullets */
                    enemy_clear_all();

                    /* Load new room */
                    game_state.current_room = next_room;
                    room_load(next_room);
                    room_render();

                    /* Lock exits for new room */
                    room_set_exits_locked(1);

                    /* Spawn next wave */
                    gamestate_next_wave();
                    enemy_spawn_wave();
                }
            } else {
                /* Enemies still alive, keep exits locked */
                room_set_exits_locked(1);
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
