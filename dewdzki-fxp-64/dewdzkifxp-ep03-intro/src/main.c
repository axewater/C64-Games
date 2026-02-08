#include "intro.h"
#include "chainload.h"

int main(void) {
    uint8_t result = intro_run();

    if (result == 0) {
        chainload("ch03-game");
    }

    return 0;
}
