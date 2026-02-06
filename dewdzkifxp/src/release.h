#ifndef RELEASE_H
#define RELEASE_H

#include <stdint.h>

#define MAX_RELEASES 32  /* Optimized: 8 FTPs × 5 releases = 40 max, cleanup handles overflow */

/* Release categories */
#define CATEGORY_GAME  0
#define CATEGORY_APP   1
#define CATEGORY_MOVIE 2
#define CATEGORY_XXX   3

/* Release quality */
#define QUALITY_NUKED  0
#define QUALITY_OK     1
#define QUALITY_PROPER 2
#define QUALITY_SCENE  3

/* Release groups */
#define GROUP_IND  0  /* Independent */
#define GROUP_FLT  1  /* Fairlight */
#define GROUP_RZR  2  /* Razor 1911 */
#define GROUP_TRSi 3  /* Tristar & Red Sector Inc */
#define GROUP_FRY  4  /* Fairylight */

/* Group data structure */
typedef struct {
    const char* tag;         /* 3-4 char tag (IND, FLT, RZR, etc) */
    uint8_t rep_multiplier;  /* 1-3: reputation bonus */
    uint8_t min_topsite;     /* Minimum topsite tier (0=newbie, 1=elite, 2=0day) */
} GroupData;

/* Release structure */
typedef struct {
    uint8_t active;
    char name[20];
    uint16_t size_mb;
    uint8_t quality;      /* 0=NUKED, 1=OK, 2=PROPER, 3=SCENE */
    uint8_t category;     /* 0=GAME, 1=APP, 2=MOVIE, 3=XXX */
    uint8_t group;        /* 0=IND, 1=FLT, 2=RZR, 3=TRSi, 4=FRY */
} Release;

/* Global release inventory */
extern Release releases[MAX_RELEASES];

/* Global group data */
extern const GroupData groups[5];

/* Initialize release system */
void release_init(void);

/* Generate random release based on topsite index */
void release_generate(Release* rel, uint8_t topsite_idx);

/* Add release to inventory */
uint8_t release_add(Release* rel);

/* Get release by index */
Release* release_get(uint8_t index);

/* Count active releases */
uint8_t release_count_active(void);

/* Cleanup unused releases (not on any FTP, not in any active post) */
uint8_t release_cleanup_unused(void);

#endif /* RELEASE_H */
