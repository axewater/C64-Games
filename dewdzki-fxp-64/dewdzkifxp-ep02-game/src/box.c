#include "box.h"
#include "range.h"
#include "random.h"

Box boxes[MAX_BOXES];

void box_init(void) {
    uint8_t i;
    for (i = 0; i < MAX_BOXES; ++i) {
        boxes[i].active = BOX_EMPTY;
        boxes[i].range_id = 0;
        boxes[i].octet3 = 0;
        boxes[i].octet4 = 0;
        boxes[i].bandwidth = 0;
        boxes[i].services = SVC_NONE;
        boxes[i].stealth = 0;
        boxes[i].risk = 0;
        boxes[i].days_alive = 0;
    }
}

uint8_t box_create(uint8_t range_id) {
    uint8_t i;

    /* Find empty slot */
    for (i = 0; i < MAX_BOXES; ++i) {
        if (boxes[i].active == BOX_EMPTY) {
            boxes[i].active = BOX_ACTIVE;
            boxes[i].range_id = range_id;
            boxes[i].octet3 = random_range(1, 255);
            boxes[i].octet4 = random_range(1, 255);
            boxes[i].bandwidth = ranges[range_id].bandwidth;
            boxes[i].services = SVC_NONE;
            boxes[i].stealth = 0;
            boxes[i].risk = 0;
            boxes[i].days_alive = 0;
            ++game.total_boxes_ever;
            ++game.active_box_count;
            return i;
        }
    }
    return 255; /* Pool full */
}

void box_destroy(uint8_t idx) {
    if (idx >= MAX_BOXES) return;
    if (boxes[idx].active != BOX_ACTIVE) return;

    /* Decrement service counters */
    if (boxes[idx].services & SVC_FTP) --game.ftp_count;
    if (boxes[idx].services & SVC_SCANNER) --game.scanner_count;
    if (boxes[idx].services & SVC_PROXY) --game.proxy_count;

    boxes[idx].active = BOX_DISCOVERED;
    --game.active_box_count;
}

uint8_t box_install_service(uint8_t idx, uint8_t svc_bit) {
    if (idx >= MAX_BOXES) return 0;
    if (boxes[idx].active != BOX_ACTIVE) return 0;
    if (boxes[idx].services & svc_bit) return 0; /* Already installed */

    boxes[idx].services |= svc_bit;

    if (svc_bit == SVC_FTP) ++game.ftp_count;
    if (svc_bit == SVC_SCANNER) ++game.scanner_count;
    if (svc_bit == SVC_PROXY) ++game.proxy_count;

    return 1;
}

uint8_t box_count_active(void) {
    uint8_t i, count = 0;
    for (i = 0; i < MAX_BOXES; ++i) {
        if (boxes[i].active == BOX_ACTIVE) ++count;
    }
    return count;
}

uint8_t box_count_with_service(uint8_t svc_bit) {
    uint8_t i, count = 0;
    for (i = 0; i < MAX_BOXES; ++i) {
        if (boxes[i].active == BOX_ACTIVE && (boxes[i].services & svc_bit)) {
            ++count;
        }
    }
    return count;
}
