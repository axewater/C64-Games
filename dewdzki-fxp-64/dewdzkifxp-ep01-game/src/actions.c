#include "actions.h"
#include <stdint.h>
#include "screen.h"
#include "input.h"
#include "random.h"
#include "gamestate.h"
#include "rank.h"
#include "ftp.h"
#include "topsite.h"
#include "release.h"
#include "forum.h"
#include "ui.h"

/* Temporary release buffer for topsite browsing */
Release temp_releases[3];
uint8_t selected_release_id;

void handle_playing_state(void) {
    uint8_t choice;
    uint8_t has_hardware;

    has_hardware = (game_state.hardware_tier < 255);

    ui_show_hub();
    choice = input_read_menu(7);

    if (choice == 0) {
        /* Scan for FTPs - requires hardware */
        if (!has_hardware) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "NO INTERNET CONNECTION!", COLOR_RED);
            ui_print_centered(12, "VISIT HARDWARE SHOP FIRST", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            return;
        }
        game_state.state = STATE_SCAN;
    } else if (choice == 1) {
        /* Browse topsites - requires hardware */
        if (!has_hardware) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "NO INTERNET CONNECTION!", COLOR_RED);
            ui_print_centered(12, "VISIT HARDWARE SHOP FIRST", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            return;
        }
        game_state.state = STATE_TOPSITE_LIST;
    } else if (choice == 2) {
        /* Post to forum (multi-step) */
        game_state.state = STATE_RELEASE_SELECT;
    } else if (choice == 3) {
        /* View stats */
        game_state.state = STATE_VIEW_STATS;
    } else if (choice == 4) {
        /* View FTPs */
        game_state.state = STATE_VIEW_FTPS;
    } else if (choice == 5) {
        /* Hardware shop */
        game_state.state = STATE_HARDWARE_SHOP;
    } else if (choice == 6) {
        /* Reputation shop */
        game_state.state = STATE_REP_SHOP;
    } else if (choice == 255) {
        /* Quit */
        game_state.state = STATE_GAME_OVER;
    }
}

void action_scan(void) {
    FTPServer* ftp;
    uint8_t num_found;
    uint8_t i;
    uint8_t start_idx;

    /* Check if player has actions remaining */
    if (!gamestate_can_use_action()) {
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "OUT OF ACTIONS!", COLOR_RED);
        ui_print_centered(12, "WAIT FOR NEW SESSION", COLOR_WHITE);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        game_state.state = STATE_PLAYING;
        return;
    }

    ui_show_scan_anim();

    /* Remember starting FTP count */
    start_idx = game_state.ftps_known_count;

    /* Scan for FTPs */
    num_found = ftp_scan();

    screen_clear();
    ui_render_hud();

    if (num_found > 0) {
        /* Success - show found FTP(s) */
        if (num_found == 1) {
            ui_print_centered(10, "FTP FOUND!", COLOR_GREEN);
        } else {
            ui_print_centered(10, "MULTIPLE FTPS FOUND!", COLOR_GREEN);
        }

        /* Show up to 3 FTP names */
        for (i = 0; i < num_found && i < 3; i++) {
            if (start_idx + i < game_state.ftps_known_count) {
                ftp = ftp_get(start_idx + i);
                if (ftp) {
                    ui_print_centered(12 + i, ftp->name, COLOR_CYAN);
                }
            }
        }

        /* Cost: 1 action, 1 turn */
        gamestate_use_action();
        gamestate_advance_turn(1);
    } else {
        /* Failure - lose 1 REP */
        ui_print_centered(10, "SCAN FAILED!", COLOR_RED);
        ui_print_centered(12, "NO FTP FOUND", COLOR_WHITE);

        if (game_state.reputation > 0) {
            game_state.reputation--;
            ui_print_centered(14, "-1 REP", COLOR_RED);
        }

        /* Cost: 1 action, 1 turn (same as success) */
        gamestate_use_action();
        gamestate_advance_turn(1);
    }

    ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();

    game_state.state = STATE_PLAYING;
}

void action_browse_topsite(void) {
    uint8_t i;
    uint8_t choice;
    uint8_t turns;
    uint8_t ftp_idx;
    Release* rel;
    uint8_t ftp_index_map[MAX_FTP_SERVERS];
    uint8_t ftp_count;

    /* Multi-step flow based on current state */
    if (game_state.state == STATE_TOPSITE_LIST) {
        /* Step 1: Select topsite */
        choice = ui_show_topsite_list();

        if (choice == 255) {
            /* Cancelled */
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Store selected topsite and generate releases */
        game_state.selected_topsite_idx = choice;

        for (i = 0; i < 3; i++) {
            release_generate(&temp_releases[i], choice);
        }

        /* Move to release browse */
        game_state.state = STATE_TOPSITE;

    } else if (game_state.state == STATE_TOPSITE) {
        /* Step 2: Browse releases */
        choice = ui_show_topsite(game_state.selected_topsite_idx, temp_releases, 3);

        if (choice == 255) {
            /* Cancelled - back to topsite list */
            game_state.state = STATE_TOPSITE_LIST;
            return;
        }

        if (choice >= 3) {
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Check if any FTPs discovered */
        if (game_state.ftps_known_count == 0) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "NO FTPS DISCOVERED!", COLOR_RED);
            ui_print_centered(12, "SCAN FOR FTPS FIRST", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Store selected release */
        game_state.selected_release_idx = choice;

        /* Move to FTP selection */
        game_state.state = STATE_FTP_SELECT;

    } else if (game_state.state == STATE_FTP_SELECT) {
        /* Step 3: Select FTP for transfer */

        /* Check if player has actions remaining */
        if (!gamestate_can_use_action()) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "OUT OF ACTIONS!", COLOR_RED);
            ui_print_centered(12, "WAIT FOR NEW SESSION", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Build FTP index mapping BEFORE showing menu to avoid race condition */
        ftp_count = 0;
        for (i = 0; i < MAX_FTP_SERVERS; i++) {
            if (ftps[i].active) {
                ftp_index_map[ftp_count++] = i;
            }
        }

        choice = ui_show_ftp_select(&temp_releases[game_state.selected_release_idx]);

        if (choice == 255) {
            /* Cancelled - back to release browse */
            game_state.state = STATE_TOPSITE;
            return;
        }

        /* Use pre-built mapping (safe from race conditions) */
        if (choice >= ftp_count) {
            /* Invalid choice somehow */
            game_state.state = STATE_PLAYING;
            return;
        }
        ftp_idx = ftp_index_map[choice];

        /* Check if FTP is still active (not raided) */
        if (!ftps[ftp_idx].active) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "FTP WAS RAIDED!", COLOR_RED);
            ui_print_centered(12, "CHOOSE ANOTHER FTP", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_FTP_SELECT;
            return;
        }

        /* Check if FTP has space */
        if (!ftp_has_space(ftp_idx)) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "FTP IS FULL!", COLOR_RED);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_FTP_SELECT;
            return;
        }

        /* Add release to global pool if needed */
        selected_release_id = release_add(&temp_releases[game_state.selected_release_idx]);

        /* Check if release pool is full */
        if (selected_release_id == 255) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "RELEASE POOL FULL!", COLOR_RED);
            ui_print_centered(12, "MAX CAPACITY REACHED", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_FTP_SELECT;
            return;
        }

        /* Get the release from global pool */
        rel = release_get(selected_release_id);

        /* Safety check: verify release was retrieved */
        if (!rel || !rel->active) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "ERROR: RELEASE NOT FOUND!", COLOR_RED);
            ui_print_centered(12, "TRY AGAIN", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_FTP_SELECT;
            return;
        }

        /* Calculate transfer time - FXP uses MIN(topsite_bw, ftp_bw) */
        {
            uint16_t topsite_bw;
            uint16_t ftp_bw;
            uint16_t transfer_bw;

            topsite_bw = topsites[game_state.selected_topsite_idx].bandwidth;
            ftp_bw = ftps[ftp_idx].bandwidth;

            /* FXP speed limited by slower of the two servers */
            transfer_bw = (topsite_bw < ftp_bw) ? topsite_bw : ftp_bw;

            /* Calculate turns based on transfer speed */
            turns = (rel->size_mb / transfer_bw) + 1;
            if (turns > 10) turns = 10;  /* Cap at 10 turns */
        }

        /* Show FXP animation */
        ui_show_fxp_anim(rel, turns);

        /* Add release to FTP */
        if (!ftp_add_release(ftp_idx, selected_release_id)) {
            /* Failed to add (duplicate or FTP full) */
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "ERROR: RELEASE NOT ADDED!", COLOR_RED);
            ui_print_centered(12, "DUPLICATE OR FTP ISSUE", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Cost: 1 action, N turns */
        gamestate_use_action();
        gamestate_advance_turn(turns);

        /* Move to FXP state (will return to hub) */
        game_state.state = STATE_FXP;
    }
}

void action_post_to_forum(void) {
    uint8_t choice;
    uint8_t ftp_idx;
    uint8_t release_idx;
    Release* rel;
    uint8_t i;
    uint8_t has_releases;

    /* Multi-step flow based on current state */
    if (game_state.state == STATE_RELEASE_SELECT) {
        /* Step 1: Check if any FTPs have releases */
        has_releases = 0;
        for (i = 0; i < MAX_FTP_SERVERS; i++) {
            if (ftps[i].active && ftps[i].release_count > 0) {
                has_releases = 1;
                break;
            }
        }

        if (!has_releases) {
            /* No releases on any FTP */
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "NO RELEASES ON FTPS!", COLOR_RED);
            ui_print_centered(12, "FXP WAREZ FIRST", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Show release selection */
        choice = ui_show_release_select(&ftp_idx, &release_idx);

        if (choice == 255) {
            /* Cancelled */
            game_state.state = STATE_PLAYING;
            return;
        }

        /* Store selections */
        game_state.selected_ftp_idx = ftp_idx;
        game_state.selected_release_idx = release_idx;

        /* Get release */
        rel = release_get(release_idx);

        if (rel && rel->active) {
            /* Check if already posted */
            if (forum_has_post(release_idx, ftp_idx)) {
                screen_clear();
                ui_render_hud();
                ui_print_centered(10, "ALREADY POSTED!", COLOR_RED);
                ui_print_centered(12, "CHOOSE DIFFERENT RELEASE", COLOR_WHITE);
                ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
                input_wait_key();
                game_state.state = STATE_RELEASE_SELECT;
                return;
            }

            /* Create forum post with FTP tracking */
            forum_create_post(release_idx, ftp_idx, game_state.current_forum);

            /* Show confirmation */
            ui_show_forum_post(rel, ftp_idx);

            /* Cost: 0 actions, 1 turn */
            gamestate_advance_turn(1);
        }

        /* Move to forum state (will return to hub) */
        game_state.state = STATE_FORUM;
    } else if (game_state.state == STATE_FORUM) {
        /* Step 2: Return to hub after posting */
        game_state.state = STATE_PLAYING;
    }
}

void action_hardware_shop(void) {
    uint8_t choice;
    const HardwareTier* tier;
    extern const HardwareTier hardware_tiers[8];
    uint16_t old_bw;
    uint8_t old_actions;

    /* Show hardware shop */
    choice = ui_show_hardware_shop();

    /* Check for cancel */
    if (choice == 255) {
        game_state.state = STATE_PLAYING;
        return;
    }

    /* Validate choice (should be 0-7) */
    if (choice >= 8) {
        game_state.state = STATE_PLAYING;
        return;
    }

    tier = &hardware_tiers[choice];

    /* Check if already owned */
    if (gamestate_has_hardware(1 << choice)) {
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "INVALID PURCHASE!", COLOR_RED);
        ui_print_centered(12, "ALREADY OWNED", COLOR_WHITE);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        game_state.state = STATE_HARDWARE_SHOP;
        return;
    }

    /* Check if player has enough REP */
    if (game_state.reputation < tier->rep_cost) {
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "INSUFFICIENT REP!", COLOR_RED);
        ui_print_string(10, 12, "NEED:", COLOR_WHITE);
        screen_print_number(16, 12, tier->rep_cost, 3, COLOR_YELLOW);
        ui_print_string(10, 13, "HAVE:", COLOR_WHITE);
        screen_print_number(16, 13, game_state.reputation, 3, COLOR_YELLOW);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        game_state.state = STATE_HARDWARE_SHOP;
        return;
    }

    /* Store old values for display */
    old_bw = game_state.bandwidth;
    old_actions = game_state.actions_remaining;

    /* Purchase successful */
    game_state.reputation -= tier->rep_cost;
    gamestate_purchase_hardware(choice);
    gamestate_update_bandwidth();

    /* Update hardware_tier for backward compatibility (use highest tier) */
    if (choice < 6 && (game_state.hardware_tier == 255 || choice > game_state.hardware_tier)) {
        game_state.hardware_tier = choice;
    }

    /* Show success message */
    screen_clear();
    ui_render_hud();
    ui_print_centered(10, "PURCHASE SUCCESSFUL!", COLOR_GREEN);
    ui_print_string(8, 12, "NEW HARDWARE:", COLOR_WHITE);
    ui_print_string(22, 12, tier->name, COLOR_CYAN);

    /* Show effect based on tier type */
    if (choice == 6) {
        /* Second Phone Line */
        ui_print_string(8, 14, "ACTIONS:", COLOR_WHITE);
        screen_print_number(17, 14, old_actions, 1, COLOR_GRAY2);
        ui_print_string(19, 14, "->", COLOR_WHITE);
        screen_print_number(22, 14, gamestate_calculate_actions(), 1, COLOR_GREEN);
    } else {
        /* Bandwidth upgrade */
        ui_print_string(8, 14, "BANDWIDTH:", COLOR_WHITE);
        screen_print_number(19, 14, old_bw, 3, COLOR_GRAY2);
        ui_print_string(23, 14, "->", COLOR_WHITE);
        screen_print_number(26, 14, game_state.bandwidth, 3, COLOR_GREEN);
        ui_print_string(30, 14, "KB/S", COLOR_WHITE);
    }

    ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();

    game_state.state = STATE_PLAYING;
}

void action_move_releases(void) {
    /* Check if player has actions remaining */
    if (!gamestate_can_use_action()) {
        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "OUT OF ACTIONS!", COLOR_RED);
        ui_print_centered(12, "WAIT FOR NEW SESSION", COLOR_WHITE);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
        game_state.state = STATE_PLAYING;
        return;
    }

    /* Show move releases UI */
    if (ui_show_move_releases()) {
        /* Success - cost 1 action, 1 turn */
        gamestate_use_action();
        gamestate_advance_turn(1);
    }

    game_state.state = STATE_PLAYING;
}


void action_rep_shop(void) {
    uint8_t choice;

    /* Show reputation shop */
    choice = ui_show_rep_shop();

    /* Check for cancel */
    if (choice == 255) {
        game_state.state = STATE_PLAYING;
        return;
    }

    /* Handle purchases */
    if (choice == 0) {
        /* Bribe Sysop - remove nuke */
        if (!gamestate_can_afford_rep(REP_COST_BRIBE_SYSOP)) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "INSUFFICIENT REP!", COLOR_RED);
            ui_print_centered(12, "NEED 50 REP", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_REP_SHOP;
            return;
        }

        if (forum_unnuke_post()) {
            gamestate_spend_rep(REP_COST_BRIBE_SYSOP);
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "SYSOP BRIBED!", COLOR_GREEN);
            ui_print_centered(12, "NUKE TAG REMOVED", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
        } else {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "NO NUKED POSTS FOUND!", COLOR_RED);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_REP_SHOP;
            return;
        }
    } else if (choice == 1) {
        /* Inside Info - simplified version */
        if (!gamestate_can_afford_rep(REP_COST_INSIDE_INFO)) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "INSUFFICIENT REP!", COLOR_RED);
            ui_print_centered(12, "NEED 30 REP", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_REP_SHOP;
            return;
        }

        gamestate_spend_rep(REP_COST_INSIDE_INFO);
        ui_show_inside_info();
    } else if (choice == 2) {
        /* VIP Access */
        if (!gamestate_can_afford_rep(REP_COST_VIP_ACCESS)) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "INSUFFICIENT REP!", COLOR_RED);
            ui_print_centered(12, "NEED 100 REP", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_REP_SHOP;
            return;
        }

        gamestate_spend_rep(REP_COST_VIP_ACCESS);
        game_state.vip_access_turns_left = VIP_DURATION;

        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "VIP ACCESS GRANTED!", COLOR_GREEN);
        ui_print_centered(12, "20 TURNS, 5X REPUTATION", COLOR_WHITE);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
    } else if (choice == 3) {
        /* Security Audit */
        if (!gamestate_can_afford_rep(REP_COST_SECURITY_AUDIT)) {
            screen_clear();
            ui_render_hud();
            ui_print_centered(10, "INSUFFICIENT REP!", COLOR_RED);
            ui_print_centered(12, "NEED 75 REP", COLOR_WHITE);
            ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
            input_wait_key();
            game_state.state = STATE_REP_SHOP;
            return;
        }

        gamestate_spend_rep(REP_COST_SECURITY_AUDIT);
        game_state.security_audit_turns_left = AUDIT_DURATION;

        screen_clear();
        ui_render_hud();
        ui_print_centered(10, "SECURITY AUDIT ACTIVE!", COLOR_GREEN);
        ui_print_centered(12, "10 TURNS, -25% FTP RISK", COLOR_WHITE);
        ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
        input_wait_key();
    }

    game_state.state = STATE_PLAYING;
}
