#include "player.h"
#include "sprite.h"
#include <conio.h>

static Player g_player;

/* Screen boundaries in pixels */
#define MIN_X 24
#define MAX_X 320
#define MIN_Y 50
#define MAX_Y 229

void player_init(void) {
    /* Initialize sprite system */
    sprite_init();

    /* Start player in center of screen (pixel coordinates) */
    g_player.x = 172;
    g_player.y = 140;
    g_player.color = COLOR_YELLOW;
    g_player.sprite_num = 0;

    /* Configure sprite 0 for player */
    sprite_set_color(g_player.sprite_num, g_player.color);
    sprite_set_position(g_player.sprite_num, g_player.x, g_player.y);
    sprite_enable(g_player.sprite_num, 1);
}

void player_move(int16_t dx, int16_t dy) {
    int16_t new_x, new_y;

    /* Calculate new position */
    new_x = (int16_t)g_player.x + dx;
    new_y = (int16_t)g_player.y + dy;

    /* Boundary checking - keep sprite on screen */
    if (new_x < MIN_X) new_x = MIN_X;
    if (new_x > MAX_X) new_x = MAX_X;
    if (new_y < MIN_Y) new_y = MIN_Y;
    if (new_y > MAX_Y) new_y = MAX_Y;

    g_player.x = new_x;
    g_player.y = new_y;
}

void player_update(void) {
    /* Update sprite position */
    sprite_set_position(g_player.sprite_num, g_player.x, g_player.y);
}

Player* player_get(void) {
    return &g_player;
}
