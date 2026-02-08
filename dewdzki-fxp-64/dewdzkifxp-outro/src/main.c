#include "outro.h"
#include "screen.h"
#include "sprite.h"

int main(void) {
    outro_run();

    /* Clean exit to BASIC */
    sprite_init();
    screen_init();

    return 0;
}
