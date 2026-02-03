#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

/* Input state structure */
typedef struct {
    int8_t dx;      /* Horizontal movement delta */
    int8_t dy;      /* Vertical movement delta */
    uint8_t quit;   /* Quit flag */
} InputState;

/* Initialize input system (loads joystick driver if available) */
uint8_t input_init(void);

/* Read current input state from both joystick and keyboard */
void input_read(InputState *state);

/* Cleanup input system (unloads joystick driver) */
void input_cleanup(void);

#endif /* INPUT_H */
