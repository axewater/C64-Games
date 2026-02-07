#include "topsite.h"
#include "release.h"

/* Topsite progression:
 * 0: NEWBIE.DUMP.US - APPS only (small files for beginners)
 * 1: ELITE.WAREZ.NL - APPS + MOVIES (add CD-R releases)
 * 2: 0DAY.TOPSITE.SE - ALL categories (games, large releases)
 */
const TopsiteData topsites[3] = {
    {"NEWBIE.DUMP.US",  512,  TOPSITE_APPS},                                    /* 512 KB/s */
    {"ELITE.WAREZ.NL",  1024, TOPSITE_APPS | TOPSITE_MOVIES},                  /* 1 MB/s */
    {"0DAY.TOPSITE.SE", 2048, TOPSITE_APPS | TOPSITE_MOVIES | TOPSITE_GAMES}   /* 2 MB/s */
};

uint8_t topsite_allows_category(uint8_t topsite_idx, uint8_t category) {
    uint8_t category_bit;

    if (topsite_idx >= 3) {
        return 0;
    }

    /* Convert category to bitmask */
    switch (category) {
        case CATEGORY_APP:   category_bit = TOPSITE_APPS;   break;
        case CATEGORY_MOVIE: category_bit = TOPSITE_MOVIES; break;
        case CATEGORY_GAME:  category_bit = TOPSITE_GAMES;  break;
        case CATEGORY_XXX:   category_bit = TOPSITE_XXX;    break;
        default: return 0;
    }

    /* Check if topsite allows this category */
    return (topsites[topsite_idx].categories & category_bit) != 0;
}
