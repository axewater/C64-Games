#ifndef TOPSITE_H
#define TOPSITE_H

#include <stdint.h>

/* Category bitmasks for topsite filtering */
#define TOPSITE_APPS   1  /* 0001 - APP releases */
#define TOPSITE_MOVIES 2  /* 0010 - MOVIE releases */
#define TOPSITE_GAMES  4  /* 0100 - GAME releases */
#define TOPSITE_XXX    8  /* 1000 - XXX releases */

/* Topsite data structure */
typedef struct {
    const char* name;
    uint16_t bandwidth;      /* Bandwidth in KB/s */
    uint8_t categories;      /* Bitmask of allowed categories */
} TopsiteData;

/* Global topsite data */
extern const TopsiteData topsites[3];

/* Check if topsite allows a category */
uint8_t topsite_allows_category(uint8_t topsite_idx, uint8_t category);

#endif /* TOPSITE_H */
