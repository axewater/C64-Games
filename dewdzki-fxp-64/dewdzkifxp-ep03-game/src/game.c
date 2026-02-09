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

/* Server orbit center */
#define SERVER_CENTER_X 172
#define SERVER_CENTER_Y 140

/* Orbit: 32-entry sine table, amplitude 25, advance every 9 frames
 * Full orbit = 32 * 9 / 50 ≈ 5.8 seconds */
#define ORBIT_TABLE_SIZE 32
#define ORBIT_FRAME_SKIP 9

static const int8_t orbit_sin[32] = {
     0,   5,  10,  14,  18,  21,  23,  24,
    25,  24,  23,  21,  18,  14,  10,   5,
     0,  -5, -10, -14, -18, -21, -23, -24,
   -25, -24, -23, -21, -18, -14, -10,  -5
};

/* Health pickup (PETSCII heart on screen) */
#define PICKUP_SPAWN_INTERVAL 750      /* 15 sec * 50 fps (levels 1-2) */
#define PICKUP_SPAWN_INTERVAL_FAST 250 /* 5 sec * 50 fps (level 3+boss) */
#define PICKUP_TIMEOUT 500         /* 10 sec */
#define PICKUP_BLINK_RATE 10
#define PICKUP_CHAR 0x53           /* Heart */

uint8_t game_state;
uint8_t current_level;
uint8_t server_health;
uint16_t server_x;
uint8_t server_y;

/* Frame timing */
static uint8_t last_jiffy;

/* Level timer */
static uint16_t timer_frames;
static uint8_t timer_secs;
static uint8_t spawn_counter;
static uint8_t damage_flash;

/* Orbit state */
static uint8_t orbit_angle;
static uint8_t orbit_sub;

/* Pickup state */
static uint8_t pickup_active;
static uint8_t pickup_cx;
static uint8_t pickup_cy;
static uint16_t pickup_timer;
static uint16_t pickup_spawn_timer;
static uint8_t pickup_blink;
static uint8_t pickup_visible;

/* Seed random from jiffy clock */
static void seed_random(void) {
    srand(PEEK(JIFFY_CLOCK) ^ PEEK(0xD012));
}

void sync_frame(void) {
    while (PEEK(JIFFY_CLOCK) == last_jiffy) {}
    last_jiffy = PEEK(JIFFY_CLOCK);
}

/* === Server orbit === */

static void server_update_orbit(void) {
    uint8_t cos_idx;

    orbit_sub++;
    if (orbit_sub >= ORBIT_FRAME_SKIP) {
        orbit_sub = 0;
        orbit_angle = (orbit_angle + 1) & 0x1F;
    }

    cos_idx = (orbit_angle + 8) & 0x1F;  /* cosine = sine shifted quarter cycle */
    server_x = SERVER_CENTER_X + orbit_sin[cos_idx];
    server_y = SERVER_CENTER_Y + orbit_sin[orbit_angle];

    sprite_set_position(SERVER_SPRITE_NUM, server_x, server_y);
}

/* === Health pickup === */

/* Restore the background grid character at a position */
static void restore_bg_char(uint8_t cx, uint8_t cy) {
    if (cy % 4 == 3 && cy >= 3 && cy <= 23) {
        /* Grid row: dots at multiples of 4, h-lines elsewhere */
        if (cx % 4 == 0) {
            screen_set_char(cx, cy, 0x51, COLOR_BLUE);
        } else {
            screen_set_char(cx, cy, 0xC0, COLOR_BLUE);
        }
    } else if (cx % 4 == 0 && cy >= 4 && cy < 23) {
        /* Vertical line between grid rows */
        screen_set_char(cx, cy, 0x42, COLOR_BLUE);
    } else {
        screen_set_char(cx, cy, ' ', COLOR_BLACK);
    }
}

static void pickup_clear(void) {
    if (pickup_active) {
        restore_bg_char(pickup_cx, pickup_cy);
        pickup_active = 0;
    }
}

static void pickup_spawn(void) {
    if (pickup_active) return;

    pickup_cx = 3 + (rand() % 34);
    pickup_cy = 4 + (rand() % 18);
    pickup_active = 1;
    pickup_timer = PICKUP_TIMEOUT;
    pickup_blink = 0;
    pickup_visible = 1;

    screen_set_char(pickup_cx, pickup_cy, PICKUP_CHAR, COLOR_WHITE);
}

/* Update pickup: spawn timer, collection check, blink, timeout */
static void pickup_update(void) {
    uint8_t pcx, pcy;
    int8_t dx, dy;

    if (!pickup_active) {
        uint16_t interval = (current_level >= 2) ?
            PICKUP_SPAWN_INTERVAL_FAST : PICKUP_SPAWN_INTERVAL;
        pickup_spawn_timer++;
        if (pickup_spawn_timer >= interval) {
            pickup_spawn_timer = 0;
            pickup_spawn();
        }
        return;
    }

    /* Check collection: player shield center proximity */
    pcx = (uint8_t)((player_x - 12) >> 3);
    pcy = (player_y - 40) >> 3;

    dx = (int8_t)pcx - (int8_t)pickup_cx;
    dy = (int8_t)pcy - (int8_t)pickup_cy;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    if (dx <= 1 && dy <= 1) {
        pickup_clear();
        if (server_health < 4) {
            server_health = 4;
            hud_update_health(server_health);
            POKE(0xD020, COLOR_GREEN);
            damage_flash = 6;
        }
        return;
    }

    /* Timeout */
    if (pickup_timer > 0) {
        pickup_timer--;
    }
    if (pickup_timer == 0) {
        pickup_clear();
        return;
    }

    /* Blink effect */
    pickup_blink++;
    if (pickup_timer < 150) {
        /* Fast blink when about to expire */
        if (pickup_blink >= 5) {
            pickup_blink = 0;
            pickup_visible ^= 1;
            if (pickup_visible) {
                screen_set_char(pickup_cx, pickup_cy, PICKUP_CHAR, COLOR_RED);
            } else {
                restore_bg_char(pickup_cx, pickup_cy);
            }
        }
    } else {
        /* Normal: alternate bright white / dim gray */
        if (pickup_blink >= PICKUP_BLINK_RATE) {
            pickup_blink = 0;
            pickup_visible ^= 1;
            if (pickup_visible) {
                screen_set_char(pickup_cx, pickup_cy, PICKUP_CHAR, COLOR_WHITE);
            } else {
                screen_set_char(pickup_cx, pickup_cy, PICKUP_CHAR, COLOR_GRAY1);
            }
        }
    }
}

/* === Game init / level setup === */

void game_init(void) {
    game_state = STATE_TITLE;
    current_level = 0;
    server_health = 4;
    server_x = SERVER_CENTER_X;
    server_y = SERVER_CENTER_Y;
    seed_random();
}

static void server_sprite_init(void) {
    sprite_load(SERVER_SPRITE_NUM, sprite_server, SERVER_SPRITE_LOC);
    sprite_set_color(SERVER_SPRITE_NUM, COLOR_GREEN);
    sprite_set_multicolor(SERVER_SPRITE_NUM, 0);
    sprite_set_position(SERVER_SPRITE_NUM, server_x, server_y);
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
    server_health = 4;

    /* Reset orbit */
    orbit_angle = 0;
    orbit_sub = 0;
    server_x = SERVER_CENTER_X + orbit_sin[8];
    server_y = SERVER_CENTER_Y + orbit_sin[0];

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

    /* Reset pickup */
    pickup_active = 0;
    pickup_spawn_timer = 0;

    hud_draw(server_health, cfg->name, timer_secs);

    /* Reset jiffy sync */
    last_jiffy = PEEK(JIFFY_CLOCK);
}

void game_start_boss(void) {
    const LevelConfig* cfg = level_get_config(3);

    /* Reset health for boss */
    server_health = 4;

    /* Reset orbit */
    orbit_angle = 0;
    orbit_sub = 0;
    server_x = SERVER_CENTER_X + orbit_sin[8];
    server_y = SERVER_CENTER_Y + orbit_sin[0];

    /* Init systems */
    sprite_init();
    background_draw();

    /* Init player and server */
    player_init();
    server_sprite_init();

    /* Init boss (loads boss sprites + its packet sprites) */
    boss_init();

    /* Init packets for boss fight: 2 slots using sprites 6-7 only
     * (boss body occupies sprites 2-5, must not conflict) */
    packet_init_boss();

    /* Draw HUD */
    timer_secs = cfg->duration_secs;
    timer_frames = 0;
    spawn_counter = 0;
    damage_flash = 0;

    /* Reset pickup */
    pickup_active = 0;
    pickup_spawn_timer = 0;

    hud_draw(server_health, cfg->name, timer_secs);

    last_jiffy = PEEK(JIFFY_CLOCK);
}

/* === Frame loops === */

void game_run_frame(void) {
    const LevelConfig* cfg = level_get_config(current_level);
    uint8_t dmg;

    sync_frame();

    /* Debug: X to skip level */
    if (kbhit() && cgetc() == 'x') {
        timer_secs = 0;
    }

    /* Player input and movement */
    player_update();

    /* Orbit server */
    server_update_orbit();

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

    /* Damage/heal flash effect */
    if (damage_flash > 0) {
        damage_flash--;
        if (damage_flash == 0) {
            POKE(0xD020, COLOR_BLACK);
        }
    }

    /* Health pickup */
    pickup_update();

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
        pickup_clear();
        game_state = STATE_GAMEOVER;
        return;
    }

    /* Check level complete */
    if (timer_secs == 0) {
        pickup_clear();
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

    /* Debug: X to skip boss */
    if (kbhit() && cgetc() == 'x') {
        timer_secs = 0;
    }

    /* Player input */
    player_update();

    /* Orbit server */
    server_update_orbit();

    /* Boss movement, animation, and attacks */
    boss_update();
    boss_animate();
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

    /* Health pickup */
    pickup_update();

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
        pickup_clear();
        boss_cleanup();
        game_state = STATE_GAMEOVER;
        return;
    }

    /* Boss defeated (survived timer) */
    if (timer_secs == 0) {
        pickup_clear();
        boss_cleanup();
        packet_disable_all();
        player_disable();
        sprite_enable(SERVER_SPRITE_NUM, 0);
        game_state = STATE_VICTORY;
    }
}
