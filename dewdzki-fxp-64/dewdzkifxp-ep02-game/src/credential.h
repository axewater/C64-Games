#ifndef CREDENTIAL_H
#define CREDENTIAL_H

#include <stdint.h>
#include "gamestate.h"

typedef struct {
    uint8_t active;
    uint8_t range_id;
    uint8_t turns_left;   /* 0=complete */
} CredJob;

/* Global credential job pool */
extern CredJob cred_jobs[MAX_CRED_JOBS];

/* Initialize credential system */
void credential_init(void);

/* Start a new credential dump job. Returns 1 on success. */
uint8_t credential_start(uint8_t range_id);

/* Tick all active jobs. Returns number completed this tick. */
uint8_t credential_tick(void);

/* Count active (in-progress) jobs */
uint8_t credential_count_active(void);

/* Rush-complete one job (for shop). Returns 1 if a job was completed. */
uint8_t credential_rush_one(void);

#endif /* CREDENTIAL_H */
