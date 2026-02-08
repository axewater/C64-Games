#ifndef RANGE_H
#define RANGE_H

#include <stdint.h>
#include "gamestate.h"

typedef struct {
    const char* name;       /* e.g. "EDU-US" */
    uint8_t hit_rate;       /* base % chance to find a box */
    uint8_t bandwidth;      /* in 100KB/s units */
    uint8_t risk_per_turn;  /* risk growth per turn */
    uint8_t octet1;         /* IP prefix first octet */
    uint8_t octet2;         /* IP prefix second octet */
} RangeInfo;

/* Global range data */
extern const RangeInfo ranges[MAX_RANGES];

/* Mutable per-range state */
extern uint8_t range_unlocked[MAX_RANGES];
extern uint8_t range_has_creds[MAX_RANGES];

/* Initialize range state */
void range_init(void);

/* Get effective hit rate (3x if has_creds) */
uint8_t range_get_hit_rate(uint8_t idx);

/* Check and unlock ranges based on box count and tools */
void range_check_unlocks(void);

#endif /* RANGE_H */
