#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>
#include "gamestate.h"

/* Tool count */
#define TOOL_COUNT 6

/* Tool names (indexed 0-5 matching bit positions) */
extern const char* tool_names[TOOL_COUNT];

/* Initialize tools (X-Scan unlocked at start) */
void tools_init(void);

/* Check all milestone conditions. Returns newly unlocked tool bit, or 0. */
uint8_t tools_check_milestones(void);

/* Check if a specific tool is unlocked */
uint8_t tools_has(uint8_t tool_bit);

/* Count unlocked tools */
uint8_t tools_count_unlocked(void);

#endif /* TOOLS_H */
