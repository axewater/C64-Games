#ifndef SCENES_H
#define SCENES_H

#include <stdint.h>

/* Scene functions - each returns 0 to continue, 1 if user quit */
uint8_t scene1_decline(void);
uint8_t scene2_last_irc(void);
uint8_t scene3_dossier_sick0(void);
uint8_t scene4_dossier_axe(void);
uint8_t scene5_dossier_zzz(void);
uint8_t scene6_dossier_lewis(void);
uint8_t scene7_dossier_rizz(void);
uint8_t scene8_dossier_moon(void);
uint8_t scene9_the_crew(void);
uint8_t scene10_fin(void);

#endif /* SCENES_H */
