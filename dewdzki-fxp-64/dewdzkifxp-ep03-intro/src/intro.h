#ifndef INTRO_H
#define INTRO_H

#include <stdint.h>

/* Initialize intro system (screen + sprites) */
void intro_init(void);

/* Run the full intro sequence
 * Returns: 0 on normal completion, 1 if user quit with Q */
uint8_t intro_run(void);

#endif /* INTRO_H */
