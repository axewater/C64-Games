#include "gamestate.h"
#include "rank.h"
#include "ftp.h"

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
    game_state.hardware_owned = 0;  /* No hardware owned */
    game_state.temp_equipment_type = TEMP_EQUIP_NONE;
    game_state.temp_equipment_turns_left = 0;
    game_state.vip_access_turns_left = 0;
    game_state.security_audit_turns_left = 0;
    game_state.inside_info_available = 0;
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
    game_state.hardware_owned = 0;  /* No hardware owned */
    game_state.temp_equipment_type = TEMP_EQUIP_NONE;
    game_state.temp_equipment_turns_left = 0;
    game_state.vip_access_turns_left = 0;
    game_state.security_audit_turns_left = 0;
    game_state.inside_info_available = 0;
}

void gamestate_advance_turn(uint8_t turns) {
    game_state.turn += turns;
    gamestate_update_temp_equipment();
    gamestate_update_rep_purchases();
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
            game_state.actions_remaining = gamestate_calculate_actions();
        }
    }
}

/* Cumulative hardware functions (NEW) */

uint16_t gamestate_calculate_bandwidth(void) {
    uint16_t total_bw = 0;
    uint8_t i;
    extern const HardwareTier hardware_tiers[8];

    /* Add bandwidth from all owned base connections (tiers 0-5) */
    for (i = 0; i < 6; i++) {
        if (game_state.hardware_owned & (1 << i)) {
            total_bw += hardware_tiers[i].bandwidth;
        }
    }

    /* Add DUAL ISDN addon (tier 7) */
    if (game_state.hardware_owned & HW_DUAL_ISDN) {
        total_bw += hardware_tiers[7].bandwidth;
    }

    /* Add temporary equipment bonus */
    total_bw += gamestate_get_temp_bandwidth();

    return total_bw;
}

uint8_t gamestate_calculate_actions(void) {
    uint8_t actions = BASE_ACTIONS;

    /* Add Second Phone Line bonus */
    if (game_state.hardware_owned & HW_PHONE2) {
        actions += BONUS_ACTIONS_PHONE2;
    }

    return actions;
}

uint8_t gamestate_has_hardware(uint8_t hw_bit) {
    return (game_state.hardware_owned & hw_bit) != 0;
}

void gamestate_purchase_hardware(uint8_t tier_idx) {
    if (tier_idx < 8) {
        game_state.hardware_owned |= (1 << tier_idx);
    }
}

void gamestate_update_bandwidth(void) {
    game_state.bandwidth = gamestate_calculate_bandwidth();
}

void gamestate_apply_temp_equipment(uint8_t equip_type, uint8_t turns) {
    game_state.temp_equipment_type = equip_type;
    game_state.temp_equipment_turns_left = turns;
    gamestate_update_bandwidth();
}

void gamestate_update_temp_equipment(void) {
    if (game_state.temp_equipment_turns_left > 0) {
        game_state.temp_equipment_turns_left--;
        if (game_state.temp_equipment_turns_left == 0) {
            game_state.temp_equipment_type = TEMP_EQUIP_NONE;
            gamestate_update_bandwidth();
        }
    }
}

uint16_t gamestate_get_temp_bandwidth(void) {
    switch (game_state.temp_equipment_type) {
        case TEMP_EQUIP_T1_LOAN:
            return 193;
        case TEMP_EQUIP_CABLE:
            return 256;
        case TEMP_EQUIP_T3:
            return 512;
        default:
            return 0;
    }
}

/* Reputation economy functions (NEW) */

uint8_t gamestate_can_afford_rep(uint16_t cost) {
    return game_state.reputation >= cost;
}

uint8_t gamestate_spend_rep(uint16_t cost) {
    if (!gamestate_can_afford_rep(cost)) {
        return 0;
    }

    game_state.reputation -= cost;
    return 1;
}

void gamestate_update_rep_purchases(void) {
    /* Tick down VIP access */
    if (game_state.vip_access_turns_left > 0) {
        game_state.vip_access_turns_left--;
    }

    /* Tick down security audit */
    if (game_state.security_audit_turns_left > 0) {
        game_state.security_audit_turns_left--;
    }
}

uint8_t gamestate_has_vip_access(void) {
    return game_state.vip_access_turns_left > 0;
}

uint8_t gamestate_has_security_audit(void) {
    return game_state.security_audit_turns_left > 0;
}
