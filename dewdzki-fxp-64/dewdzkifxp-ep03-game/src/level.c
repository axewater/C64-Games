#include "level.h"

/* Edge mask bits */
#define E_TOP    0x01
#define E_RIGHT  0x02
#define E_BOTTOM 0x04
#define E_LEFT   0x08

static const LevelConfig levels[] = {
    /* Level 1: Script Kiddies - Slow, top only, 30 sec */
    {
        1,              /* speed */
        2,              /* max_packets */
        90,             /* spawn_interval (frames) */
        30,             /* duration_secs */
        E_TOP,          /* edges */
        "SCRIPT KIDDIES"
    },
    /* Level 2: Security Researchers - Medium, top+sides, 45 sec */
    {
        1,              /* speed */
        4,              /* max_packets */
        45,             /* spawn_interval */
        45,             /* duration_secs */
        E_TOP | E_RIGHT | E_LEFT,
        "SECURITY RESEARCHERS"
    },
    /* Level 3: FBI Cyber Unit - Fast, all edges, 60 sec */
    {
        2,              /* speed */
        6,              /* max_packets */
        38,             /* spawn_interval */
        60,             /* duration_secs */
        E_TOP | E_RIGHT | E_BOTTOM | E_LEFT,
        "FBI CYBER UNIT"
    },
    /* Boss: THE HONEYPOT - separate handling, but packets config here */
    {
        2,              /* speed (boss packet speed) */
        2,              /* max_packets (boss uses sprite 6-7 only) */
        38,             /* spawn_interval */
        60,             /* duration_secs */
        E_TOP | E_RIGHT | E_BOTTOM | E_LEFT,
        "THE HONEYPOT"
    }
};

const LevelConfig* level_get_config(uint8_t level_num) {
    if (level_num > 3) level_num = 3;
    return &levels[level_num];
}
