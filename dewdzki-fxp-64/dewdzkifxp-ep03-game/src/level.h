#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>

/* Level configuration */
typedef struct {
    uint8_t speed;          /* Packet movement speed (1-4) */
    uint8_t max_packets;    /* Max simultaneous packets */
    uint8_t spawn_interval; /* Frames between spawns */
    uint8_t duration_secs;  /* Level duration in seconds */
    uint8_t edges_mask;     /* Which edges can spawn (bit 0=top, 1=right, 2=bottom, 3=left) */
    const char* name;       /* Level display name */
} LevelConfig;

/* Total number of regular levels (not counting boss) */
#define NUM_LEVELS 3

/* Get level config (0-2 = regular levels, 3 = boss) */
const LevelConfig* level_get_config(uint8_t level_num);

#endif /* LEVEL_H */
