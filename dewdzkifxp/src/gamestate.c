#include "gamestate.h"
#include "rank.h"

GameState game_state;

void gamestate_init(void) {
    game_state.state = STATE_SPLASH;
    game_state.rank = 0;  /* LEECH */
    game_state.reputation = 0;
    game_state.bandwidth = 0;  /* No bandwidth until hardware purchased */
    game_state.actions_remaining = 3;
    game_state.turn = 0;
    game_state.ftps_known_count = 0;
    game_state.posts_active_count = 0;
    game_state.current_topsite = 0;
    game_state.current_forum = 0;
    game_state.hardware_tier = 255;  /* Start with no hardware */
    game_state.last_raid_ftp_idx = 255;  /* No raid */
    game_state.last_raid_downloads = 0;
    game_state.last_raid_turn = 0;
    game_state.total_raids = 0;
    game_state.current_event = 0;  /* No event */
    game_state.event_value = 0;
}

void gamestate_start_game(void) {
    game_state.state = STATE_PLAYING;
    game_state.rank = 0;
    game_state.reputation = 0;
    game_state.bandwidth = 0;  /* No bandwidth until hardware purchased */
    game_state.actions_remaining = 3;
    game_state.turn = 1;  /* Start at turn 1 */
    game_state.ftps_known_count = 0;
    game_state.posts_active_count = 0;
    game_state.current_topsite = 0;
    game_state.current_forum = 0;
    game_state.hardware_tier = 255;  /* Start with no hardware */
    game_state.last_raid_ftp_idx = 255;  /* No raid */
    game_state.last_raid_downloads = 0;
    game_state.last_raid_turn = 0;
    game_state.total_raids = 0;
    game_state.current_event = 0;  /* No event */
    game_state.event_value = 0;
}

void gamestate_advance_turn(uint8_t turns) {
    game_state.turn += turns;
}

void gamestate_check_rank_up(void) {
    uint8_t new_rank;

    /* Check if eligible for next rank */
    new_rank = rank_get_next(game_state.rank, game_state.reputation);

    if (new_rank != game_state.rank) {
        /* Rank up! */
        game_state.rank = new_rank;

        /* Apply rank bonuses */
        rank_apply_bonuses();

        /* Show rank-up screen */
        game_state.state = STATE_RANK_UP;

        /* Check for win (reached ELITE) */
        if (game_state.rank == 5) {  /* ELITE rank */
            game_state.state = STATE_GAME_OVER;
        }
    }
}

void gamestate_add_reputation(uint16_t rep) {
    game_state.reputation += rep;

    /* Cap at 9999 */
    if (game_state.reputation > 9999) {
        game_state.reputation = 9999;
    }
}

uint8_t gamestate_can_use_action(void) {
    return game_state.actions_remaining > 0;
}

void gamestate_use_action(void) {
    if (game_state.actions_remaining > 0) {
        game_state.actions_remaining--;

        /* Reset actions when depleted (new session) */
        if (game_state.actions_remaining == 0) {
            game_state.actions_remaining = 3;
        }
    }
}
