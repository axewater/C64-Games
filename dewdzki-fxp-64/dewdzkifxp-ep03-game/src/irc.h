#ifndef IRC_H
#define IRC_H

#include <stdint.h>

/* Show IRC briefing screen for a level.
 * level_num: 0-3 (0=before level 1, 3=before boss)
 * Returns: 0 = continue, 1 = user quit */
uint8_t irc_show_briefing(uint8_t level_num);

/* Show victory IRC screen */
void irc_show_victory(void);

#endif /* IRC_H */
