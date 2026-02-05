#ifndef ROOM_H
#define ROOM_H

#include <stdint.h>

/* Room constants */
#define MAX_ROOMS 5
#define ROOM_WIDTH 40
#define ROOM_HEIGHT 24

/* Exit directions */
#define EXIT_NONE 0
#define EXIT_NORTH 1
#define EXIT_SOUTH 2
#define EXIT_EAST 4
#define EXIT_WEST 8

/* Wall character */
#define WALL_CHAR 160  /* Solid block in PETSCII */
#define EMPTY_CHAR 32  /* Space */
#define EXIT_OPEN_CHAR 46  /* Period */
#define EXIT_CLOSED_CHAR 120  /* 'X' */

/* Room structure */
typedef struct {
    uint8_t id;
    uint8_t exits;  /* Bitmask of available exits */
    const uint8_t *layout;  /* Pointer to character layout */
} Room;

/* Global functions */
void room_init(void);
void room_load(uint8_t room_id);
void room_render(void);
uint8_t room_check_collision(uint16_t x, uint16_t y);
uint8_t room_check_exit(uint16_t x, uint16_t y);
void room_set_exits_locked(uint8_t locked);
uint8_t room_get_current(void);

#endif
