#include "release.h"
#include "random.h"
#include <stdio.h>

Release releases[MAX_RELEASES];

/* Category prefixes */
const char* category_prefixes[] = {"GAME", "APP", "MOVIE", "XXX"};

/* Topsite quality ranges */
typedef struct {
    uint8_t quality_min;
    uint8_t quality_max;
} TopsiteQuality;

const TopsiteQuality topsite_quality[3] = {
    {0, 1},  /* NEWBIE.DUMP.US - low quality */
    {1, 2},  /* ELITE.WAREZ.NL - good quality */
    {2, 3}   /* 0DAY.TOPSITE.SE - best quality */
};

void release_init(void) {
    uint8_t i;

    for (i = 0; i < MAX_RELEASES; i++) {
        releases[i].active = 0;
    }
}

void release_generate(Release* rel, uint8_t topsite_idx) {
    uint8_t quality_min;
    uint8_t quality_max;
    uint16_t num;

    /* Ensure topsite index is valid */
    if (topsite_idx >= 3) {
        topsite_idx = 2;
    }

    quality_min = topsite_quality[topsite_idx].quality_min;
    quality_max = topsite_quality[topsite_idx].quality_max;

    rel->active = 1;
    rel->quality = random_range(quality_min, quality_max + 1);
    rel->category = random_range(0, 4);

    /* Size based on category */
    if (rel->category == CATEGORY_MOVIE) {
        rel->size_mb = random_range(700, 1501);
    } else if (rel->category == CATEGORY_APP) {
        rel->size_mb = random_range(50, 301);
    } else {
        rel->size_mb = random_range(200, 701);
    }

    /* Generate name */
    num = random_range(100, 1000);
    sprintf(rel->name, "%s.%03d.ISO", category_prefixes[rel->category], num);
}

uint8_t release_add(Release* rel) {
    uint8_t i;

    for (i = 0; i < MAX_RELEASES; i++) {
        if (!releases[i].active) {
            releases[i] = *rel;
            return i;
        }
    }

    return 255;  /* Pool full */
}

Release* release_get(uint8_t index) {
    if (index >= MAX_RELEASES) {
        return 0;
    }
    return &releases[index];
}

uint8_t release_count_active(void) {
    uint8_t i;
    uint8_t count;

    count = 0;
    for (i = 0; i < MAX_RELEASES; i++) {
        if (releases[i].active) {
            count++;
        }
    }

    return count;
}
