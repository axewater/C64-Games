#include "gamestate.h"
#include "box.h"

GameState game;

void gamestate_init(void) {
    game.state = STATE_SPLASH;
    game.turn = 0;
    game.actions_remaining = BASE_ACTIONS;
    game.actions_per_turn = BASE_ACTIONS;
    game.reputation = 0;
    game.tools_unlocked = 0;
    game.total_boxes_ever = 0;
    game.active_box_count = 0;
    game.ftp_count = 0;
    game.scanner_count = 0;
    game.proxy_count = 0;
    game.creds_cracked_total = 0;
    game.selected_range = 0;
    game.selected_box = 0;
    game.current_event = 0;
    game.event_value = 0;
    game.event_turns_left = 0;
    game.event_range = 0;
    game.last_discovered_box = 255;
    game.discovery_count = 0;
    game.pending_tool = 0;
    game.last_scan_found = 0;
}

void gamestate_start_game(void) {
    game.state = STATE_PLAYING;
    game.turn = 1;
    game.actions_remaining = BASE_ACTIONS;
    game.actions_per_turn = BASE_ACTIONS;
    game.reputation = 0;
    game.tools_unlocked = TOOL_XSCAN;  /* Start with X-Scan */
    game.total_boxes_ever = 0;
    game.active_box_count = 0;
    game.ftp_count = 0;
    game.scanner_count = 0;
    game.proxy_count = 0;
    game.creds_cracked_total = 0;
    game.selected_range = 0;
    game.selected_box = 0;
    game.current_event = 0;
    game.event_value = 0;
    game.event_turns_left = 0;
    game.event_range = 0;
    game.last_discovered_box = 255;
    game.discovery_count = 0;
    game.pending_tool = 0;
    game.last_scan_found = 0;
}

void gamestate_use_action(void) {
    if (game.actions_remaining > 0) {
        --game.actions_remaining;
    }
}

void gamestate_end_turn(void) {
    ++game.turn;
    game.actions_remaining = game.actions_per_turn;
    game.last_discovered_box = 255;
    game.discovery_count = 0;
}

void gamestate_add_reputation(uint16_t rep) {
    game.reputation += rep;
    if (game.reputation > 9999) {
        game.reputation = 9999;
    }
}

uint8_t gamestate_spend_rep(uint16_t cost) {
    if (game.reputation < cost) {
        return 0;
    }
    game.reputation -= cost;
    return 1;
}

void gamestate_recount(void) {
    uint8_t i;
    game.active_box_count = 0;
    game.ftp_count = 0;
    game.scanner_count = 0;
    game.proxy_count = 0;
    for (i = 0; i < MAX_BOXES; ++i) {
        if (boxes[i].active == 1) {
            ++game.active_box_count;
            if (boxes[i].services & SVC_FTP) ++game.ftp_count;
            if (boxes[i].services & SVC_SCANNER) ++game.scanner_count;
            if (boxes[i].services & SVC_PROXY) ++game.proxy_count;
        }
    }
}
