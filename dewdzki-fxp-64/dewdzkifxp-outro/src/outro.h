#ifndef OUTRO_H
#define OUTRO_H

#include <stdint.h>

/* Initialize outro system (screen + sprites) */
void outro_init(void);

/* Run the full outro/credits sequence
 * Returns: 0 on normal completion, 1 if user quit with Q */
uint8_t outro_run(void);

#endif /* OUTRO_H */
