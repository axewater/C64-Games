#include "tools.h"
#include "range.h"

const char* tool_names[TOOL_COUNT] = {
    "X-SCAN",
    "SERV-U INSTALLER",
    "LOPHTCRACK",
    "REMOTE SCANNER",
    "PROXY MODULE",
    "HACKDB"
};

void tools_init(void) {
    game.tools_unlocked = TOOL_XSCAN;
}

uint8_t tools_check_milestones(void) {
    uint8_t new_tool = 0;

    /* Serv-U: 5 boxes hacked total */
    if (!(game.tools_unlocked & TOOL_SERVU) &&
        game.total_boxes_ever >= 5) {
        new_tool = TOOL_SERVU;
    }

    /* LophtCrack: 3 active FTPs + 100 rep */
    if (!(game.tools_unlocked & TOOL_LOPHTCRACK) &&
        (game.tools_unlocked & TOOL_SERVU) &&
        game.ftp_count >= 3 &&
        game.reputation >= 100) {
        new_tool = TOOL_LOPHTCRACK;
    }

    /* Remote Scanner: 10 boxes total + used creds */
    if (!(game.tools_unlocked & TOOL_SCANNER) &&
        (game.tools_unlocked & TOOL_LOPHTCRACK) &&
        game.total_boxes_ever >= 10 &&
        game.creds_cracked_total >= 1) {
        new_tool = TOOL_SCANNER;
    }

    /* Proxy Module: 5 scanners + 300 rep */
    if (!(game.tools_unlocked & TOOL_PROXY) &&
        (game.tools_unlocked & TOOL_SCANNER) &&
        game.scanner_count >= 5 &&
        game.reputation >= 300) {
        new_tool = TOOL_PROXY;
    }

    /* HackDB: 15 active boxes + all other tools + 500 rep = WIN */
    if (!(game.tools_unlocked & TOOL_HACKDB) &&
        (game.tools_unlocked & TOOL_PROXY) &&
        game.active_box_count >= 15 &&
        game.reputation >= 500) {
        new_tool = TOOL_HACKDB;
    }

    if (new_tool) {
        game.tools_unlocked |= new_tool;
    }

    return new_tool;
}

uint8_t tools_has(uint8_t tool_bit) {
    return (game.tools_unlocked & tool_bit) != 0;
}

uint8_t tools_count_unlocked(void) {
    uint8_t count = 0;
    uint8_t bits = game.tools_unlocked;
    while (bits) {
        count += (bits & 1);
        bits >>= 1;
    }
    return count;
}
