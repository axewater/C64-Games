#include "rank.h"
#include "gamestate.h"

const RankData ranks[6] = {
    {"LEECH",    0,    1, 1, 0},   /* Start: 1 topsite, 1 forum, base BW */
    {"USER",     50,   1, 2, 10},  /* +1 forum, +10 BW */
    {"UPLOADER", 150,  2, 2, 15},  /* +1 topsite, +15 BW */
    {"TRADER",   350,  2, 3, 25},  /* +1 forum, +25 BW */
    {"COURIER",  700,  3, 3, 35},  /* +1 topsite, +35 BW */
    {"ELITE",    1500, 3, 4, 50}   /* +1 forum, +50 BW, WIN! */
};

const char* rank_get_name(uint8_t rank) {
    if (rank >= 6) {
        rank = 5;
    }
    return ranks[rank].name;
}

uint8_t rank_get_next(uint8_t current_rank, uint16_t reputation) {
    uint8_t i;

    /* Check from highest rank down to current */
    for (i = 5; i > current_rank; i--) {
        if (reputation >= ranks[i].rep_required) {
            return i;
        }
    }

    return current_rank;
}

void rank_apply_bonuses(void) {
    const RankData *rank;

    rank = &ranks[game_state.rank];

    /* Unlock new topsite if available */
    if (rank->topsites_unlocked > game_state.current_topsite + 1) {
        game_state.current_topsite++;
    }

    /* Unlock new forum if available */
    if (rank->forums_unlocked > game_state.current_forum + 1) {
        game_state.current_forum++;
    }

    /* Increase bandwidth */
    game_state.bandwidth = 30 + rank->bandwidth_bonus;
}
