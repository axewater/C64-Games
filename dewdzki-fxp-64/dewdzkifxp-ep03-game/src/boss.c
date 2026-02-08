#include "boss.h"
#include "sprite.h"
#include "sprite_data.h"
#include "packet.h"
#include <conio.h>

/* Boss uses sprites 2-5 in a 2x2 grid */
#define BOSS_SPRITE_TL 2
#define BOSS_SPRITE_TR 3
#define BOSS_SPRITE_BL 4
#define BOSS_SPRITE_BR 5

/* Boss sprite data locations */
#define BOSS_SPRITE_LOC_TL (SPRITE_DATA_BASE + (2 * SPRITE_DATA_SIZE))
#define BOSS_SPRITE_LOC_TR (SPRITE_DATA_BASE + (3 * SPRITE_DATA_SIZE))
#define BOSS_SPRITE_LOC_BL (SPRITE_DATA_BASE + (4 * SPRITE_DATA_SIZE))
#define BOSS_SPRITE_LOC_BR (SPRITE_DATA_BASE + (5 * SPRITE_DATA_SIZE))

/* Boss packet sprites (6-7) */
#define BOSS_PKT_SPRITE_0 6
#define BOSS_PKT_SPRITE_1 7
#define BOSS_PKT_LOC_0 (SPRITE_DATA_BASE + (6 * SPRITE_DATA_SIZE))
#define BOSS_PKT_LOC_1 (SPRITE_DATA_BASE + (7 * SPRITE_DATA_SIZE))

/* Movement path: clockwise around screen edge */
#define BOSS_SPEED 2

/* Movement phases: 0=right along top, 1=down right, 2=left along bottom, 3=up left */
static uint8_t move_phase;

/* Attack cooldown */
static uint8_t attack_timer;

/* Screen bounds for boss movement */
#define BOSS_MIN_X 30
#define BOSS_MAX_X 280
#define BOSS_MIN_Y 50
#define BOSS_MAX_Y 200

uint16_t boss_x;
uint8_t boss_y;

static void boss_update_sprite_positions(void) {
    /* TL = base position */
    sprite_set_position(BOSS_SPRITE_TL, boss_x, boss_y);
    /* TR = offset right by 24 pixels */
    sprite_set_position(BOSS_SPRITE_TR, boss_x + 24, boss_y);
    /* BL = offset down by 21 pixels */
    sprite_set_position(BOSS_SPRITE_BL, boss_x, boss_y + 21);
    /* BR = offset right and down */
    sprite_set_position(BOSS_SPRITE_BR, boss_x + 24, boss_y + 21);
}

void boss_init(void) {
    /* Load boss sprite quadrants */
    sprite_load(BOSS_SPRITE_TL, sprite_boss_tl, BOSS_SPRITE_LOC_TL);
    sprite_load(BOSS_SPRITE_TR, sprite_boss_tr, BOSS_SPRITE_LOC_TR);
    sprite_load(BOSS_SPRITE_BL, sprite_boss_bl, BOSS_SPRITE_LOC_BL);
    sprite_load(BOSS_SPRITE_BR, sprite_boss_br, BOSS_SPRITE_LOC_BR);

    /* Set boss colors (orange/red) */
    sprite_set_color(BOSS_SPRITE_TL, COLOR_ORANGE);
    sprite_set_color(BOSS_SPRITE_TR, COLOR_ORANGE);
    sprite_set_color(BOSS_SPRITE_BL, COLOR_RED);
    sprite_set_color(BOSS_SPRITE_BR, COLOR_RED);

    /* Hires mode for boss */
    sprite_set_multicolor(BOSS_SPRITE_TL, 0);
    sprite_set_multicolor(BOSS_SPRITE_TR, 0);
    sprite_set_multicolor(BOSS_SPRITE_BL, 0);
    sprite_set_multicolor(BOSS_SPRITE_BR, 0);

    /* Load packet sprite data for boss's rapid-fire packets (sprites 6-7) */
    sprite_load(BOSS_PKT_SPRITE_0, sprite_packet, BOSS_PKT_LOC_0);
    sprite_load(BOSS_PKT_SPRITE_1, sprite_packet, BOSS_PKT_LOC_1);
    sprite_set_color(BOSS_PKT_SPRITE_0, COLOR_RED);
    sprite_set_color(BOSS_PKT_SPRITE_1, COLOR_RED);
    sprite_set_multicolor(BOSS_PKT_SPRITE_0, 0);
    sprite_set_multicolor(BOSS_PKT_SPRITE_1, 0);

    /* Start position: top-left of screen */
    boss_x = BOSS_MIN_X;
    boss_y = BOSS_MIN_Y;
    move_phase = 0;
    attack_timer = 0;

    /* Enable boss sprites */
    sprite_enable(BOSS_SPRITE_TL, 1);
    sprite_enable(BOSS_SPRITE_TR, 1);
    sprite_enable(BOSS_SPRITE_BL, 1);
    sprite_enable(BOSS_SPRITE_BR, 1);

    boss_update_sprite_positions();
}

void boss_update(void) {
    switch (move_phase) {
        case 0:  /* Moving right along top */
            boss_x += BOSS_SPEED;
            if (boss_x >= BOSS_MAX_X) {
                boss_x = BOSS_MAX_X;
                move_phase = 1;
            }
            break;
        case 1:  /* Moving down along right */
            boss_y += BOSS_SPEED;
            if (boss_y >= BOSS_MAX_Y) {
                boss_y = BOSS_MAX_Y;
                move_phase = 2;
            }
            break;
        case 2:  /* Moving left along bottom */
            if (boss_x > BOSS_MIN_X + BOSS_SPEED) {
                boss_x -= BOSS_SPEED;
            } else {
                boss_x = BOSS_MIN_X;
                move_phase = 3;
            }
            break;
        case 3:  /* Moving up along left */
            if (boss_y > BOSS_MIN_Y + BOSS_SPEED) {
                boss_y -= BOSS_SPEED;
            } else {
                boss_y = BOSS_MIN_Y;
                move_phase = 0;
            }
            break;
    }

    boss_update_sprite_positions();
}

uint8_t boss_attack(void) {
    uint8_t edge;

    attack_timer++;
    if (attack_timer < 25) return 0;
    attack_timer = 0;

    /* Spawn packet from boss position toward center.
     * Use the edge closest to boss's current position. */
    if (move_phase == 0) edge = EDGE_TOP;
    else if (move_phase == 1) edge = EDGE_RIGHT;
    else if (move_phase == 2) edge = EDGE_BOTTOM;
    else edge = EDGE_LEFT;

    /* Spawn uses the regular packet system but with limited slots.
     * During boss fight, packets array slots 0-1 are used (sprites 6-7). */
    return (packet_spawn(edge, 3) != 0xFF) ? 1 : 0;
}

void boss_cleanup(void) {
    sprite_enable(BOSS_SPRITE_TL, 0);
    sprite_enable(BOSS_SPRITE_TR, 0);
    sprite_enable(BOSS_SPRITE_BL, 0);
    sprite_enable(BOSS_SPRITE_BR, 0);
    sprite_enable(BOSS_PKT_SPRITE_0, 0);
    sprite_enable(BOSS_PKT_SPRITE_1, 0);
}
