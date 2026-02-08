#ifndef BOX_H
#define BOX_H

#include <stdint.h>
#include "gamestate.h"

/* Box status */
#define BOX_EMPTY      0
#define BOX_ACTIVE     1
#define BOX_DISCOVERED 2

typedef struct {
    uint8_t active;       /* 0=empty, 1=active, 2=discovered */
    uint8_t range_id;
    uint8_t octet3;       /* for IP display x.x.OCTET3.OCTET4 */
    uint8_t octet4;
    uint8_t bandwidth;    /* in 100KB/s units */
    uint8_t services;     /* bitmask: SVC_FTP|SVC_PROXY|SVC_SCANNER */
    uint8_t stealth;      /* 0-3 */
    uint8_t risk;         /* 0-100 */
    uint8_t days_alive;
} Box;

/* Global box pool */
extern Box boxes[MAX_BOXES];

/* Initialize all boxes to empty */
void box_init(void);

/* Create a new box in the given range. Returns index or 255 if full. */
uint8_t box_create(uint8_t range_id);

/* Destroy a box (mark discovered, decrement counters) */
void box_destroy(uint8_t idx);

/* Install a service on a box. Returns 1 on success. */
uint8_t box_install_service(uint8_t idx, uint8_t svc_bit);

/* Count active boxes */
uint8_t box_count_active(void);

/* Count boxes with a specific service */
uint8_t box_count_with_service(uint8_t svc_bit);

#endif /* BOX_H */
