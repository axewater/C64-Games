#include "credential.h"
#include "range.h"
#include "random.h"

CredJob cred_jobs[MAX_CRED_JOBS];

void credential_init(void) {
    uint8_t i;
    for (i = 0; i < MAX_CRED_JOBS; ++i) {
        cred_jobs[i].active = 0;
        cred_jobs[i].range_id = 0;
        cred_jobs[i].turns_left = 0;
    }
}

uint8_t credential_start(uint8_t range_id) {
    uint8_t i;

    /* Check if already have creds for this range */
    if (range_has_creds[range_id]) return 0;

    /* Check if already cracking this range */
    for (i = 0; i < MAX_CRED_JOBS; ++i) {
        if (cred_jobs[i].active && cred_jobs[i].range_id == range_id) {
            return 0;
        }
    }

    /* Find empty slot */
    for (i = 0; i < MAX_CRED_JOBS; ++i) {
        if (!cred_jobs[i].active) {
            cred_jobs[i].active = 1;
            cred_jobs[i].range_id = range_id;
            cred_jobs[i].turns_left = random_range(2, 4); /* 2-3 turns */
            return 1;
        }
    }
    return 0; /* All slots full */
}

uint8_t credential_tick(void) {
    uint8_t i;
    uint8_t completed = 0;

    for (i = 0; i < MAX_CRED_JOBS; ++i) {
        if (cred_jobs[i].active) {
            if (cred_jobs[i].turns_left > 0) {
                --cred_jobs[i].turns_left;
            }
            if (cred_jobs[i].turns_left == 0) {
                /* Job complete */
                range_has_creds[cred_jobs[i].range_id] = 1;
                ++game.creds_cracked_total;
                cred_jobs[i].active = 0;
                ++completed;
            }
        }
    }
    return completed;
}

uint8_t credential_count_active(void) {
    uint8_t i, count = 0;
    for (i = 0; i < MAX_CRED_JOBS; ++i) {
        if (cred_jobs[i].active) ++count;
    }
    return count;
}

uint8_t credential_rush_one(void) {
    uint8_t i;
    for (i = 0; i < MAX_CRED_JOBS; ++i) {
        if (cred_jobs[i].active && cred_jobs[i].turns_left > 0) {
            cred_jobs[i].turns_left = 0;
            range_has_creds[cred_jobs[i].range_id] = 1;
            ++game.creds_cracked_total;
            cred_jobs[i].active = 0;
            return 1;
        }
    }
    return 0;
}
