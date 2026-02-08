#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

/* Maximum number of packet slots */
#define MAX_PACKETS 6

/* Edge spawn directions */
#define EDGE_TOP    0
#define EDGE_RIGHT  1
#define EDGE_BOTTOM 2
#define EDGE_LEFT   3

/* Packet state */
typedef struct {
    uint16_t x;
    uint8_t y;
    int8_t dx;
    int8_t dy;
    uint8_t active;
    uint8_t sprite_num;  /* VIC-II sprite number (2-7) */
} Packet;

/* Packet array */
extern Packet packets[MAX_PACKETS];

/* Initialize all packets as inactive */
void packet_init(void);

/* Spawn a new packet from a given edge (0=top, 1=right, 2=bottom, 3=left)
 * speed: base movement speed (1-4)
 * Returns: index of spawned packet, or 0xFF if no slot available */
uint8_t packet_spawn(uint8_t edge, uint8_t speed);

/* Update all active packets (move them) */
void packet_update_all(void);

/* Deactivate a packet and hide its sprite */
void packet_deactivate(uint8_t index);

/* Deflect a packet (reverse direction + push away from center) */
void packet_deflect(uint8_t index);

/* Disable all packet sprites */
void packet_disable_all(void);

#endif /* PACKET_H */
