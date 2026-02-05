#include "release.h"
#include "random.h"
#include <stdio.h>
#include <string.h>

Release releases[MAX_RELEASES];

/* Category prefixes - using 2D array instead of pointer array for C64 reliability */
const char category_prefixes[4][6] = {
    "GAME",
    "APP",
    "MOVIE",
    "XXX"
};

/* Release groups */
const GroupData groups[5] = {
    {"IND",  1, 0},  /* Independent: base rep, any topsite */
    {"FLT",  2, 1},  /* Fairlight: double rep, elite+ sites */
    {"RZR",  2, 1},  /* Razor 1911: double rep, elite+ sites */
    {"TRSi", 3, 2},  /* Tristar: triple rep, 0day sites only */
    {"FRY",  3, 2}   /* Fairylight: triple rep, 0day sites only */
};

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
    uint8_t available_groups[5];
    uint8_t group_count;
    uint8_t group_idx;
    uint8_t i;

    /* Ensure topsite index is valid */
    if (topsite_idx >= 3) {
        topsite_idx = 2;
    }

    quality_min = topsite_quality[topsite_idx].quality_min;
    quality_max = topsite_quality[topsite_idx].quality_max;

    /* Build list of available groups for this topsite */
    group_count = 0;
    for (i = 0; i < 5; i++) {
        if (groups[i].min_topsite <= topsite_idx) {
            available_groups[group_count++] = i;
        }
    }

    /* Select random group from available */
    group_idx = random_range(0, group_count);
    rel->group = available_groups[group_idx];

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

    /* Generate name - manual string building for C64 reliability */
    num = random_range(100, 1000);
    {
        const char* prefix;
        const char* group_tag;
        uint8_t pos;
        uint8_t j;

        prefix = category_prefixes[rel->category];
        group_tag = groups[rel->group].tag;
        pos = 0;

        /* Copy prefix */
        j = 0;
        while (prefix[j] != '\0' && pos < 19) {
            rel->name[pos++] = prefix[j++];
        }

        /* Add dot */
        if (pos < 19) rel->name[pos++] = '.';

        /* Add three-digit number */
        if (pos < 19) rel->name[pos++] = '0' + (num / 100);
        if (pos < 19) rel->name[pos++] = '0' + ((num / 10) % 10);
        if (pos < 19) rel->name[pos++] = '0' + (num % 10);

        /* Add dash and group tag */
        if (pos < 19) rel->name[pos++] = '-';
        j = 0;
        while (group_tag[j] != '\0' && pos < 19) {
            rel->name[pos++] = group_tag[j++];
        }

        /* Null terminate */
        if (pos < 20) rel->name[pos] = '\0';
    }
}

uint8_t release_add(Release* rel) {
    uint8_t i;

    for (i = 0; i < MAX_RELEASES; i++) {
        if (!releases[i].active) {
            /* Use memcpy to copy entire struct */
            memcpy(&releases[i], rel, sizeof(Release));

            /* Explicitly ensure critical fields are copied (defensive programming for C64) */
            releases[i].active = 1;
            releases[i].size_mb = rel->size_mb;
            releases[i].quality = rel->quality;
            releases[i].category = rel->category;
            releases[i].group = rel->group;

            /* Ensure name is null-terminated */
            releases[i].name[19] = '\0';

            return i;
        }
    }

    return 255;  /* Pool full */
}

Release* release_get(uint8_t index) {
    if (index >= MAX_RELEASES) {
        return 0;
    }
    /* Return NULL if release is not active */
    if (!releases[index].active) {
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
