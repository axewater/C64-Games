#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

/* Initialize input system (keyboard only) */
void input_init(void);

/* Read menu selection (1-9 keys). Returns 0-based index, 255 for quit (Q) */
uint8_t input_read_menu(uint8_t num_options);

/* Wait for any key press */
void input_wait_key(void);

/* Check if quit was requested (Q key) */
uint8_t input_check_quit(void);

#endif /* INPUT_H */
