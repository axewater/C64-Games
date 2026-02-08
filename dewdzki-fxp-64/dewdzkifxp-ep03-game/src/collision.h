#ifndef COLLISION_H
#define COLLISION_H

#include <stdint.h>

/* Collision results */
#define COLL_NONE    0
#define COLL_DEFLECT 1
#define COLL_DAMAGE  2

/* Check VIC-II sprite-sprite collisions and process results.
 * Returns number of server hits (damage taken this frame). */
uint8_t collision_check(void);

#endif /* COLLISION_H */
