/* ui_events.c - Random events, raids, and rank-up screens */

#include "ui.h"
#include "screen.h"
#include "gamestate.h"
#include "rank.h"
#include "forum.h"
#include "input.h"
#include <conio.h>

void ui_show_rank_up(void) {
    const char* rank_name;

    screen_clear();

    ui_draw_hline(0, 8, 40, 160, COLOR_CYAN);
    ui_print_centered(10, "RANK UP!", COLOR_YELLOW);
    ui_draw_hline(0, 12, 40, 160, COLOR_CYAN);

    rank_name = rank_get_name(game_state.rank);
    ui_print_string(12, 14, "NEW RANK:", COLOR_WHITE);
    ui_print_string(22, 14, rank_name, COLOR_YELLOW);

    ui_print_centered(18, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();
}

void ui_show_game_over(void) {
    screen_clear();

    ui_print_centered(8, "CONGRATULATIONS!", COLOR_GREEN);
    ui_print_centered(10, "YOU REACHED ELITE RANK!", COLOR_YELLOW);

    ui_print_string(10, 13, "FINAL REP:", COLOR_WHITE);
    screen_print_number(21, 13, game_state.reputation, 4, COLOR_YELLOW);

    ui_print_string(10, 14, "TURNS:", COLOR_WHITE);
    screen_print_number(21, 14, game_state.turn, 3, COLOR_CYAN);

    ui_print_centered(18, "YOU ARE NOW SCENE LEGEND!", COLOR_CYAN);

    ui_print_centered(22, "[Q] QUIT", COLOR_WHITE);
}

void ui_show_raid_alert(void) {
    uint8_t i;
    uint8_t posts_nuked;
    uint8_t flash;

    screen_clear();
    ui_render_hud();

    /* Flashing red border effect */
    for (flash = 0; flash < 3; flash++) {
        /* Draw red border */
        ui_draw_hline(0, 2, 40, 160, COLOR_RED);
        ui_draw_hline(0, 18, 40, 160, COLOR_RED);

        ui_print_centered(8, "!!! POLICE RAID !!!", COLOR_RED);
        ui_print_centered(10, "FTP SERVER SEIZED", COLOR_WHITE);

        /* Show downloads that triggered raid */
        ui_print_string(8, 12, "DOWNLOADS:", COLOR_GRAY2);
        screen_print_number(19, 12, game_state.last_raid_downloads, 5, COLOR_YELLOW);

        /* Count nuked posts */
        posts_nuked = 0;
        for (i = 0; i < MAX_POSTS; i++) {
            if (posts[i].active && posts[i].nuked &&
                posts[i].ftp_id == game_state.last_raid_ftp_idx) {
                posts_nuked++;
            }
        }

        ui_print_string(8, 13, "POSTS NUKED:", COLOR_GRAY2);
        screen_print_number(21, 13, posts_nuked, 2, COLOR_RED);

        ui_print_centered(16, "YOUR WAREZ ARE GONE!", COLOR_RED);

        /* Flash delay */
        for (i = 0; i < 10; i++) {
            waitvsync();
        }

        /* Clear for next flash */
        if (flash < 2) {
            screen_clear();
            ui_render_hud();
            for (i = 0; i < 5; i++) {
                waitvsync();
            }
        }
    }

    ui_print_centered(20, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();

    /* Reset raid tracking */
    game_state.last_raid_ftp_idx = 255;
}

void ui_show_event(void) {
    const char* event_titles[13] = {
        "ELITE NOTICED YOUR WORK!",
        "RELEASE WENT VIRAL!",
        "FORUM FEATURED POST!",
        "ACCUSED OF DUPING!",
        "RELEASE WAS FAKE!",
        "FORUM DRAMA!",
        "BANDWIDTH UPGRADE!",
        "ISP THROTTLING!",
        "HOT TIP: NEW FTP!",
        "COURIER HINT!",
        "FRIEND LENDS T1 LINE!",
        "CABLE MODEM ACCESS!",
        "CORPORATE T3 HOOKUP!"
    };

    const char* event_descriptions[13] = {
        "AN ELITE MEMBER SAW YOUR POST",
        "YOUR RELEASE IS TRENDING",
        "MODERATORS LIKED YOUR CONTENT",
        "SOMEONE CLAIMS YOU'RE A FAKER",
        "YOUR UPLOAD WAS CORRUPTED",
        "YOU GOT INTO AN ARGUMENT",
        "YOUR ISP UPGRADED SERVICE",
        "CONNECTION ISSUES DETECTED",
        "A COURIER SHARED FTP INFO",
        "INSIDER TIP ON TOPSITE",
        "TEMPORARY BANDWIDTH BOOST",
        "NEIGHBOR SHARING INTERNET",
        "OFFICE LETS YOU USE THEIR NET"
    };

    uint8_t event_type;

    screen_clear();
    ui_render_hud();

    event_type = game_state.current_event - 1;  /* 1-13 -> 0-12 */

    if (event_type >= 13) {
        return;  /* Invalid event */
    }

    ui_draw_hline(0, 6, 40, 160, COLOR_CYAN);
    ui_print_centered(8, "*** RANDOM EVENT ***", COLOR_YELLOW);
    ui_draw_hline(0, 10, 40, 160, COLOR_CYAN);

    ui_print_centered(12, event_titles[event_type], COLOR_WHITE);
    ui_print_centered(14, event_descriptions[event_type], COLOR_GRAY2);

    /* Show effect based on event type */
    if (event_type == 0) {
        /* Elite noticed: +REP */
        ui_print_string(12, 16, "REP:", COLOR_WHITE);
        ui_print_string(17, 16, "+", COLOR_GREEN);
        screen_print_number(18, 16, game_state.event_value, 3, COLOR_GREEN);
    } else if (event_type == 1) {
        /* Viral release: double downloads message */
        ui_print_centered(16, "DOWNLOADS DOUBLED!", COLOR_GREEN);
    } else if (event_type == 2) {
        /* Featured post: +REP */
        ui_print_string(14, 16, "REP: +25", COLOR_GREEN);
    } else if (event_type == 3 || event_type == 5) {
        /* Duping/Drama: -REP */
        ui_print_string(12, 16, "REP:", COLOR_WHITE);
        ui_print_string(17, 16, "-", COLOR_RED);
        screen_print_number(18, 16, game_state.event_value, 2, COLOR_RED);
    } else if (event_type == 4) {
        /* Fake release: nuked */
        ui_print_centered(16, "POST NUKED!", COLOR_RED);
    } else if (event_type == 6) {
        /* BW upgrade */
        ui_print_string(10, 16, "BANDWIDTH +20 FOR 5 TURNS", COLOR_GREEN);
    } else if (event_type == 7) {
        /* ISP throttle */
        ui_print_string(10, 16, "BANDWIDTH -15 FOR 3 TURNS", COLOR_RED);
    } else if (event_type == 8) {
        /* New FTP discovered */
        ui_print_centered(16, "FTP AUTO-DISCOVERED!", COLOR_GREEN);
    } else if (event_type == 9) {
        /* Topsite preview */
        ui_print_centered(16, "TOPSITE UNLOCKED!", COLOR_GREEN);
    } else if (event_type == 10) {
        /* T1 line rental (NEW) */
        ui_print_string(8, 16, "+193 KB/S FOR 10 TURNS", COLOR_GREEN);
    } else if (event_type == 11) {
        /* Cable modem rental (NEW) */
        ui_print_string(8, 16, "+256 KB/S FOR 5 TURNS", COLOR_GREEN);
    } else if (event_type == 12) {
        /* T3 line rental (NEW) */
        ui_print_string(8, 16, "+512 KB/S FOR 3 TURNS", COLOR_GREEN);
    }

    ui_print_centered(20, "[SPACE] CONTINUE", COLOR_WHITE);
    input_wait_key();

    /* Reset event */
    game_state.current_event = 0;
    game_state.event_value = 0;
}
