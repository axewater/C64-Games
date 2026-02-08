#include "range.h"

/* Range definitions
 * #  Name     Hit%  BW(100KB/s)  Risk/turn  IP prefix
 * 0  EDU-US    20      10          1        128.112
 * 1  UNI-NL    15      20          2        145.97
 * 2  UNI-SE    10     100          2        130.235
 * 3  CORP-US    8      50          4        199.45
 * 4  ISP-EU    12      30          3        195.12
 * 5  GOV-MIL    5     200          5        164.50
 */
const RangeInfo ranges[MAX_RANGES] = {
    { "EDU-US",  20,  10, 1, 128, 112 },
    { "UNI-NL",  15,  20, 2, 145,  97 },
    { "UNI-SE",  10, 100, 2, 130, 235 },
    { "CORP-US",  8,  50, 4, 199,  45 },
    { "ISP-EU",  12,  30, 3, 195,  12 },
    { "GOV-MIL",  5, 200, 5, 164,  50 }
};

uint8_t range_unlocked[MAX_RANGES];
uint8_t range_has_creds[MAX_RANGES];

void range_init(void) {
    uint8_t i;
    for (i = 0; i < MAX_RANGES; ++i) {
        range_unlocked[i] = 0;
        range_has_creds[i] = 0;
    }
    range_unlocked[0] = 1; /* EDU-US always available */
}

uint8_t range_get_hit_rate(uint8_t idx) {
    uint8_t rate;
    if (idx >= MAX_RANGES) return 0;
    rate = ranges[idx].hit_rate;
    if (range_has_creds[idx]) {
        /* 3x hit rate with cracked credentials */
        rate = rate * 3;
        if (rate > 100) rate = 100;
    }
    return rate;
}

void range_check_unlocks(void) {
    uint8_t total = game.total_boxes_ever;

    /* UNI-NL: 5 boxes total */
    if (total >= 5) range_unlocked[1] = 1;

    /* UNI-SE: 10 boxes total */
    if (total >= 10) range_unlocked[2] = 1;

    /* CORP-US: 15 boxes total */
    if (total >= 15) range_unlocked[3] = 1;

    /* ISP-EU: need Proxy tool */
    if (game.tools_unlocked & TOOL_PROXY) range_unlocked[4] = 1;

    /* GOV-MIL: 20 boxes + all tools */
    if (total >= 20 && game.tools_unlocked == TOOL_ALL) range_unlocked[5] = 1;
}
