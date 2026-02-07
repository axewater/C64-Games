#include "release.h"
#include "topsite.h"
#include "random.h"
#include "ftp.h"
#include "forum.h"
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
    uint8_t available_categories[4];
    uint8_t category_count;
    uint8_t category_idx;
    uint8_t i;
    uint8_t game_size_choice;

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

    /* Build list of allowed categories for this topsite */
    category_count = 0;
    for (i = 0; i < 4; i++) {
        if (topsite_allows_category(topsite_idx, i)) {
            available_categories[category_count++] = i;
        }
    }

    /* Select random category from allowed */
    if (category_count > 0) {
        category_idx = random_range(0, category_count);
        rel->category = available_categories[category_idx];
    } else {
        rel->category = CATEGORY_APP;  /* Fallback */
    }

    rel->active = 1;
    rel->quality = random_range(quality_min, quality_max + 1);

    /* Size based on category - realistic scene sizes */
    if (rel->category == CATEGORY_MOVIE) {
        /* Movies are 700 MB (CD-R size) */
        rel->size_mb = 700;
    } else if (rel->category == CATEGORY_APP) {
        /* Apps are small: 5-50 MB */
        rel->size_mb = random_range(5, 51);
    } else if (rel->category == CATEGORY_GAME) {
        /* Games: 700 MB (1 CD), 1400 MB (2 CDs), or 2300 MB (3+ CDs) */
        game_size_choice = random_range(0, 3);
        if (game_size_choice == 0) {
            rel->size_mb = 700;
        } else if (game_size_choice == 1) {
            rel->size_mb = 1400;
        } else {
            rel->size_mb = 2300;
        }
    } else {
        /* XXX: similar to movies */
        rel->size_mb = 700;
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

    /* First try to find empty slot */
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

    /* Pool full - try cleanup and retry once */
    if (release_cleanup_unused() > 0) {
        /* Cleanup successful, try again */
        for (i = 0; i < MAX_RELEASES; i++) {
            if (!releases[i].active) {
                memcpy(&releases[i], rel, sizeof(Release));
                releases[i].active = 1;
                releases[i].size_mb = rel->size_mb;
                releases[i].quality = rel->quality;
                releases[i].category = rel->category;
                releases[i].group = rel->group;
                releases[i].name[19] = '\0';
                return i;
            }
        }
    }

    return 255;  /* Pool full even after cleanup */
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

uint8_t release_cleanup_unused(void) {
    uint8_t i, j, k;
    uint8_t in_use;
    uint8_t cleaned = 0;

    /* Cleanup releases that are:
     * - Not on any active FTP
     * - Not referenced by any active non-nuked post
     */
    for (i = 0; i < MAX_RELEASES; i++) {
        if (!releases[i].active) {
            continue;  /* Already inactive */
        }

        in_use = 0;

        /* Check if release is on any active FTP */
        for (j = 0; j < MAX_FTP_SERVERS; j++) {
            if (!ftps[j].active) {
                continue;
            }

            for (k = 0; k < ftps[j].release_count; k++) {
                if (ftps[j].releases[k] == i) {
                    in_use = 1;
                    break;
                }
            }

            if (in_use) {
                break;
            }
        }

        if (in_use) {
            continue;  /* Release is on an FTP, keep it */
        }

        /* Check if release has any active non-nuked posts */
        for (j = 0; j < MAX_POSTS; j++) {
            if (posts[j].active && posts[j].release_id == i && !posts[j].nuked) {
                in_use = 1;
                break;
            }
        }

        if (!in_use) {
            /* Release is not used anywhere, free it */
            releases[i].active = 0;
            cleaned++;
        }
    }

    return cleaned;
}
