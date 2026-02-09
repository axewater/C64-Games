#include "actions.h"
#include "gamestate.h"
#include "tools.h"
#include "input.h"
#include "sound.h"
#include "ui.h"

void handle_playing_state(void) {
    uint8_t choice;

    ui_show_hub();
    choice = input_read_menu(7);

    switch (choice) {
        case 0: /* Scan IP Range - costs 1 action */
            if (game.actions_remaining > 0) {
                game.state = STATE_SCAN_RANGE;
            } else {
                sound_play_back();
            }
            break;

        case 1: /* Manage Boxes - free */
            game.state = STATE_BOX_LIST;
            break;

        case 2: /* Dump Credentials - costs 1 action */
            if (tools_has(TOOL_LOPHTCRACK) && game.actions_remaining > 0) {
                game.state = STATE_CRED_DUMP;
            } else {
                sound_play_back();
            }
            break;

        case 3: /* Post Proxy - free */
            if (tools_has(TOOL_PROXY)) {
                game.state = STATE_PROXY_POST;
            } else {
                sound_play_back();
            }
            break;

        case 4: /* View Stats - free */
            game.state = STATE_STATS;
            break;

        case 5: /* Shop - free */
            game.state = STATE_SHOP;
            break;

        case 6: /* Skip Day - free, ends turn immediately */
            game.actions_remaining = 0;
            break;

        case 255: /* Quit */
            game.state = STATE_CONFIRM_QUIT;
            break;

        default:
            break;
    }
}
