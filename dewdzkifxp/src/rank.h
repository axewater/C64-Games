#ifndef RANK_H
#define RANK_H

#include <stdint.h>

/* Rank data structure */
typedef struct {
    const char* name;
    uint16_t rep_required;
    uint8_t topsites_unlocked;
    uint8_t forums_unlocked;
} RankData;

/* Rank progression table */
extern const RankData ranks[6];

/* Get rank name */
const char* rank_get_name(uint8_t rank);

/* Get next rank based on reputation, returns current rank if no change */
uint8_t rank_get_next(uint8_t current_rank, uint16_t reputation);

/* Apply bonuses for current rank */
void rank_apply_bonuses(void);

#endif /* RANK_H */
