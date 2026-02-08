#include "intro.h"
#include "chainload.h"

int main(void) {
    uint8_t result = intro_run();

    /* Chain load to Chapter 2 game unless user quit */
    if (result == 0) {
        chainload("ch02-game");
    }

    return 0;
}
