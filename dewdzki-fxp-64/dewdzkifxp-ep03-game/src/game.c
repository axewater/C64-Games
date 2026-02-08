#include "game.h"
#include "player.h"
#include "packet.h"
#include "boss.h"
#include "collision.h"
#include "level.h"
#include "hud.h"
#include "background.h"
#include "sprite.h"
#include "sprite_data.h"
#include "screen.h"
#include <peekpoke.h>
#include <conio.h>
#include <stdlib.h>

/* Jiffy clock at $A2 (updated by KERNAL IRQ, 50 ticks/sec PAL) */
#define JIFFY_CLOCK 0xA2

/* Server sprite */
#define SERVER_SPRITE_NUM 1
#define SERVER_SPRITE_LOC (SPRITE_DATA_BASE + SPRITE_DATA_SIZE)

/* Server center position */
#define SERVER_X 172
#define SERVER_Y 140

uint8_t game_state;
uint8_t current_level;
uint8_t server_health;

/* Frame timing */
static uint8_t last_jiffy;

/* Level timer */
static uint16_t timer_frames;
static uint8_t timer_secs;
static uint8_t spawn_counter;
static uint8_t damage_flash;

/* Seed random from jiffy clock */
static void seed_random(void) {
    srand(PEEK(JIFFY_CLOCK) ^ PEEK(0xD012));
}

void sync_frame(void) {
    while (PEEK(JIFFY_CLOCK) == last_jiffy) {}
    last_jiffy = PEEK(JIFFY_CLOCK);
}

void game_init(void) {
    game_state = STATE_TITLE;
    current_level = 0;
    server_health = 3;
    seed_random();
}

static void server_sprite_init(void) {
    sprite_load(SERVER_SPRITE_NUM, sprite_server, SERVER_SPRITE_LOC);
    sprite_set_color(SERVER_SPRITE_NUM, COLOR_GREEN);
    sprite_set_multicolor(SERVER_SPRITE_NUM, 0);
    sprite_set_position(SERVER_SPRITE_NUM, SERVER_X, SERVER_Y);
    sprite_enable(SERVER_SPRITE_NUM, 1);
}

static uint8_t pick_edge(uint8_t edges_mask) {
    uint8_t available[4];
    uint8_t count = 0;
    uint8_t i;

    for (i = 0; i < 4; i++) {
        if (edges_mask & (1 << i)) {
            available[count++] = i;
        }
    }
    if (count == 0) return EDGE_TOP;
    return available[rand() % count];
}

void game_start_level(void) {
    const LevelConfig* cfg = level_get_config(current_level);

    /* Reset health for each level */
    server_health = 3;

    /* Init systems */
    sprite_init();
    background_draw();

    /* Init player and server */
    player_init();
    server_sprite_init();

    /* Init packets */
    packet_init();

    /* Draw HUD */
    timer_secs = cfg->duration_secs;
    timer_frames = 0;
    spawn_counter = 0;
    damage_flash = 0;
    hud_draw(server_health, cfg->name, timer_secs);

    /* Reset jiffy sync */
    last_jiffy = PEEK(JIFFY_CLOCK);
}

void game_start_boss(void) {
    const LevelConfig* cfg = level_get_config(3);

    /* Reset health for boss */
    server_health = 3;

    /* Init systems */
    sprite_init();
    background_draw();

    /* Init player and server */
    player_init();
    server_sprite_init();

    /* Init boss (loads boss sprites + its packet sprites) */
    boss_init();

    /* Init packets with reduced slots for boss fight
     * Boss uses sprites 2-5, packets use 6-7 = 2 slots */
    packet_init();

    /* Draw HUD */
    timer_secs = cfg->duration_secs;
    timer_frames = 0;
    spawn_counter = 0;
    damage_flash = 0;
    hud_draw(server_health, cfg->name, timer_secs);

    last_jiffy = PEEK(JIFFY_CLOCK);
}

void game_run_frame(void) {
    const LevelConfig* cfg = level_get_config(current_level);
    uint8_t dmg;

    sync_frame();

    /* Player input and movement */
    player_update();

    /* Spawn packets */
    spawn_counter++;
    if (spawn_counter >= cfg->spawn_interval) {
        spawn_counter = 0;
        packet_spawn(pick_edge(cfg->edges_mask), cfg->speed);
    }

    /* Move packets */
    packet_update_all();

    /* Check collisions */
    dmg = collision_check();
    if (dmg > 0) {
        if (server_health > dmg) {
            server_health -= dmg;
        } else {
            server_health = 0;
        }
        hud_update_health(server_health);
        damage_flash = 6;

        /* Flash border on damage */
        POKE(0xD020, COLOR_RED);
    }

    /* Damage flash effect */
    if (damage_flash > 0) {
        damage_flash--;
        if (damage_flash == 0) {
            POKE(0xD020, COLOR_BLACK);
        }
    }

    /* Update timer (50 frames = 1 second PAL) */
    timer_frames++;
    if (timer_frames >= 50) {
        timer_frames = 0;
        if (timer_secs > 0) {
            timer_secs--;
            hud_update_timer(timer_secs);
        }
    }

    /* Check game over */
    if (server_health == 0) {
        game_state = STATE_GAMEOVER;
        return;
    }

    /* Check level complete */
    if (timer_secs == 0) {
        packet_disable_all();
        player_disable();
        sprite_enable(SERVER_SPRITE_NUM, 0);

        current_level++;
        if (current_level >= NUM_LEVELS) {
            /* All regular levels done, go to boss */
            game_state = STATE_IRC;  /* Boss briefing first */
        } else {
            game_state = STATE_IRC;  /* Next level briefing */
        }
    }
}

void game_run_boss_frame(void) {
    uint8_t dmg;

    sync_frame();

    /* Player input */
    player_update();

    /* Boss movement and attacks */
    boss_update();
    boss_attack();

    /* Move packets (boss's rapid-fire packets) */
    packet_update_all();

    /* Check collisions */
    dmg = collision_check();
    if (dmg > 0) {
        if (server_health > dmg) {
            server_health -= dmg;
        } else {
            server_health = 0;
        }
        hud_update_health(server_health);
        damage_flash = 6;
        POKE(0xD020, COLOR_RED);
    }

    if (damage_flash > 0) {
        damage_flash--;
        if (damage_flash == 0) {
            POKE(0xD020, COLOR_BLACK);
        }
    }

    /* Timer */
    timer_frames++;
    if (timer_frames >= 50) {
        timer_frames = 0;
        if (timer_secs > 0) {
            timer_secs--;
            hud_update_timer(timer_secs);
        }
    }

    /* Game over check */
    if (server_health == 0) {
        boss_cleanup();
        game_state = STATE_GAMEOVER;
        return;
    }

    /* Boss defeated (survived timer) */
    if (timer_secs == 0) {
        boss_cleanup();
        packet_disable_all();
        player_disable();
        sprite_enable(SERVER_SPRITE_NUM, 0);
        game_state = STATE_VICTORY;
    }
}
