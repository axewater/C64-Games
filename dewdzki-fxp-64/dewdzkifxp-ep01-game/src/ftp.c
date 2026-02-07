#include "ftp.h"
#include "forum.h"
#include "random.h"
#include "gamestate.h"
#include "screen.h"
#include <string.h>
#include <stdio.h>

FTPServer ftps[MAX_FTP_SERVERS];

/* FTP name templates */
const char* ftp_prefixes[] = {"FTP", "PUB", "OPEN", "FREE"};
const char* ftp_domains[] = {".SCENE.ORG", ".WAREZ.NET", ".0DAY.COM", ".UPLOAD.NL"};

/* Hardware upgrade tiers - bandwidth in KB/s (realistic values) */
/* NOTE: Tiers 0-5 are base connections, tiers 6-7 are addons */
const HardwareTier hardware_tiers[8] = {
    /* Base connection upgrades (cumulative bandwidth) */
    {"FREE DIALUP",     0,   0, 1, 25, 1},     /* +1 KB/s */
    {"14.4K MODEM",     30,  0, 1, 20, 2},     /* +2 KB/s */
    {"56K MODEM",       80,  1, 2, 15, 7},     /* +7 KB/s */
    {"ISDN",            180, 1, 2, 10, 16},    /* +16 KB/s */
    {"DSL",             350, 2, 3, 8,  128},   /* +128 KB/s */
    {"T1 LINE",         700, 2, 3, 5,  193},   /* +193 KB/s */

    /* Addon hardware (special effects) */
    {"2ND PHONE LINE",  80,  0, 0, 0, 0},      /* +1 action */
    {"DUAL ISDN",       150, 0, 0, 0, 64}      /* +64 KB/s */
};

void ftp_init(void) {
    uint8_t i;

    for (i = 0; i < MAX_FTP_SERVERS; i++) {
        ftps[i].active = 0;
        ftps[i].total_downloads = 0;
        ftps[i].raid_risk = 0;
        ftps[i].used_for_posts = 0;
        ftps[i].trait = TRAIT_NONE;
    }
}

uint8_t ftp_scan(void) {
    const HardwareTier* tier;
    FTPServer new_ftp;
    uint8_t prefix_idx;
    uint8_t domain_idx;
    uint8_t num_ftps;
    uint8_t i;
    uint8_t added;
    uint8_t roll;

    /* Safety check - should never be called without hardware */
    if (game_state.hardware_tier >= 6) {
        return 0;
    }

    /* Get current hardware tier */
    tier = &hardware_tiers[game_state.hardware_tier];

    /* Roll for failure */
    roll = random_range(0, 100);
    if (roll < tier->fail_rate) {
        return 0;  /* Scan failed */
    }

    /* Success - determine how many FTPs to find */
    num_ftps = random_range(tier->ftps_min, tier->ftps_max + 1);

    /* Generate and add FTPs */
    added = 0;
    for (i = 0; i < num_ftps; i++) {
        /* Check if pool is full */
        if (game_state.ftps_known_count >= MAX_FTP_SERVERS) {
            break;  /* Pool full, can't add more */
        }

        /* Generate random FTP */
        new_ftp.active = 1;
        new_ftp.bandwidth = random_range(64, 513);  /* Public FTPs: 64-512 KB/s */
        new_ftp.status = (random_range(0, 10) < 7) ? 1 : 0;  /* 70% open */

        /* Initialize release storage */
        new_ftp.release_count = 0;

        /* Initialize raid mechanic fields */
        new_ftp.total_downloads = 0;
        new_ftp.raid_risk = 0;
        new_ftp.used_for_posts = 0;

        /* Initialize trait (NEW) */
        new_ftp.trait = TRAIT_NONE;

        /* Generate name */
        prefix_idx = random_range(0, 4);
        domain_idx = random_range(0, 4);
        sprintf(new_ftp.name, "%s%s", ftp_prefixes[prefix_idx], ftp_domains[domain_idx]);

        /* Add to pool */
        if (ftp_add(&new_ftp)) {
            /* Assign random trait after adding (40% chance) */
            ftp_assign_trait(game_state.ftps_known_count - 1);
            added++;
        }
    }

    return added;
}

FTPServer* ftp_get(uint8_t index) {
    if (index >= MAX_FTP_SERVERS) {
        return 0;
    }
    return &ftps[index];
}

uint8_t ftp_add(FTPServer* ftp) {
    uint8_t i;

    for (i = 0; i < MAX_FTP_SERVERS; i++) {
        if (!ftps[i].active) {
            ftps[i] = *ftp;
            game_state.ftps_known_count++;
            return 1;
        }
    }

    return 0;  /* Pool full */
}

uint8_t ftp_add_release(uint8_t ftp_idx, uint8_t release_id) {
    FTPServer* ftp;
    uint8_t i;

    if (ftp_idx >= MAX_FTP_SERVERS) {
        return 0;
    }

    ftp = &ftps[ftp_idx];

    if (!ftp->active) {
        return 0;
    }

    /* Check if full */
    if (ftp->release_count >= MAX_RELEASES_PER_FTP) {
        return 0;
    }

    /* Check for duplicate */
    for (i = 0; i < ftp->release_count; i++) {
        if (ftp->releases[i] == release_id) {
            return 0;  /* Already have this release */
        }
    }

    /* Add release */
    ftp->releases[ftp->release_count++] = release_id;
    return 1;
}

uint8_t ftp_has_space(uint8_t ftp_idx) {
    if (ftp_idx >= MAX_FTP_SERVERS) {
        return 0;
    }

    if (!ftps[ftp_idx].active) {
        return 0;
    }

    return ftps[ftp_idx].release_count < MAX_RELEASES_PER_FTP;
}

uint8_t ftp_get_release_count(uint8_t ftp_idx) {
    if (ftp_idx >= MAX_FTP_SERVERS) {
        return 0;
    }

    if (!ftps[ftp_idx].active) {
        return 0;
    }

    return ftps[ftp_idx].release_count;
}

uint8_t ftp_has_release(uint8_t ftp_idx, uint8_t release_id) {
    FTPServer* ftp;
    uint8_t i;

    if (ftp_idx >= MAX_FTP_SERVERS) {
        return 0;
    }

    ftp = &ftps[ftp_idx];

    if (!ftp->active) {
        return 0;
    }

    for (i = 0; i < ftp->release_count; i++) {
        if (ftp->releases[i] == release_id) {
            return 1;
        }
    }

    return 0;
}

/* Raid mechanic functions */

uint16_t ftp_calculate_downloads(uint8_t ftp_idx) {
    uint16_t total = 0;
    uint8_t i;

    /* Sum downloads from all posts on this FTP */
    for (i = 0; i < MAX_POSTS; i++) {
        if (posts[i].active && posts[i].ftp_id == ftp_idx) {
            total += posts[i].downloads;
        }
    }

    /* Cap at 60000 to prevent overflow */
    if (total > 60000U) {
        total = 60000U;
    }

    return total;
}

void ftp_update_risk(uint8_t ftp_idx) {
    uint16_t downloads;
    uint8_t risk;

    if (ftp_idx >= MAX_FTP_SERVERS || !ftps[ftp_idx].active) {
        return;
    }

    /* Calculate total downloads */
    downloads = ftp_calculate_downloads(ftp_idx);

    /* Calculate risk: min(downloads / 50, 95) */
    if (downloads >= 4750) {
        risk = 95;  /* Cap at 95% */
    } else {
        risk = (uint8_t)(downloads / 50);
    }

    /* Apply trait modifiers (NEW) */
    risk = ftp_apply_trait_to_risk(ftp_idx, risk);

    ftps[ftp_idx].raid_risk = risk;
}

void ftp_update_all_risks(void) {
    uint8_t i;

    /* Update risk for all FTPs that have been used for posts */
    for (i = 0; i < MAX_FTP_SERVERS; i++) {
        if (ftps[i].active && ftps[i].used_for_posts) {
            ftp_update_risk(i);
        }
    }
}

uint8_t ftp_check_raid(uint8_t ftp_idx) {
    uint8_t risk;
    uint8_t roll;

    if (ftp_idx >= MAX_FTP_SERVERS || !ftps[ftp_idx].active) {
        return 0;
    }

    /* HIDDEN trait: skip raid check if turn % 3 != 0 (NEW) */
    if (ftps[ftp_idx].trait == TRAIT_HIDDEN) {
        if (game_state.turn % 3 != 0) {
            return 0;
        }
    }

    risk = ftps[ftp_idx].raid_risk;

    /* Roll random 0-100, raid occurs if roll < risk */
    roll = random_range(0, 100);

    return roll < risk;
}

void ftp_execute_raid(uint8_t ftp_idx) {
    uint8_t i;

    if (ftp_idx >= MAX_FTP_SERVERS) {
        return;
    }

    /* Store raid info for display (NEW) */
    game_state.last_raid_ftp_idx = ftp_idx;
    game_state.last_raid_downloads = ftp_calculate_downloads(ftp_idx);
    game_state.last_raid_turn = game_state.turn;
    game_state.total_raids++;

    /* Mark FTP as inactive */
    ftps[ftp_idx].active = 0;
    game_state.ftps_known_count--;

    /* Mark all posts on this FTP as nuked */
    for (i = 0; i < MAX_POSTS; i++) {
        if (posts[i].active && posts[i].ftp_id == ftp_idx) {
            posts[i].nuked = 1;
        }
    }
}

void ftp_process_raids(void) {
    uint8_t i;

    /* Check each FTP that's been used for posts */
    for (i = 0; i < MAX_FTP_SERVERS; i++) {
        if (ftps[i].active && ftps[i].used_for_posts) {
            if (ftp_check_raid(i)) {
                ftp_execute_raid(i);
            }
        }
    }
}

/* Heat management functions (NEW) */

uint8_t ftp_move_release(uint8_t src_ftp_idx, uint8_t dst_ftp_idx, uint8_t release_id) {
    FTPServer* src;
    FTPServer* dst;
    uint8_t i;
    uint8_t found;

    /* Validate indices */
    if (src_ftp_idx >= MAX_FTP_SERVERS || dst_ftp_idx >= MAX_FTP_SERVERS) {
        return 0;
    }

    if (src_ftp_idx == dst_ftp_idx) {
        return 0;  /* Can't move to same FTP */
    }

    src = &ftps[src_ftp_idx];
    dst = &ftps[dst_ftp_idx];

    /* Validate FTPs */
    if (!src->active || !dst->active) {
        return 0;
    }

    /* Check if destination has space */
    if (dst->release_count >= MAX_RELEASES_PER_FTP) {
        return 0;
    }

    /* Find release in source FTP */
    found = 0;
    for (i = 0; i < src->release_count; i++) {
        if (src->releases[i] == release_id) {
            found = 1;
            /* Remove from source (shift array) */
            for (; i < src->release_count - 1; i++) {
                src->releases[i] = src->releases[i + 1];
            }
            src->release_count--;
            break;
        }
    }

    if (!found) {
        return 0;  /* Release not found on source FTP */
    }

    /* Add to destination */
    dst->releases[dst->release_count++] = release_id;

    return 1;  /* Success */
}

const char* ftp_get_heat_text(uint8_t ftp_idx) {
    uint8_t risk;

    if (ftp_idx >= MAX_FTP_SERVERS || !ftps[ftp_idx].active) {
        return "    ";
    }

    risk = ftps[ftp_idx].raid_risk;

    if (risk < HEAT_LOW) {
        return "LOW ";
    } else if (risk < HEAT_MEDIUM) {
        return "MED ";
    } else if (risk < HEAT_HIGH) {
        return "HGH ";
    } else {
        return "CRIT";
    }
}

uint8_t ftp_get_heat_color(uint8_t ftp_idx) {
    uint8_t risk;

    if (ftp_idx >= MAX_FTP_SERVERS || !ftps[ftp_idx].active) {
        return COLOR_GRAY3;
    }

    risk = ftps[ftp_idx].raid_risk;

    if (risk < HEAT_LOW) {
        return COLOR_GREEN;
    } else if (risk < HEAT_MEDIUM) {
        return COLOR_YELLOW;
    } else if (risk < HEAT_HIGH) {
        return COLOR_ORANGE;
    } else {
        return COLOR_RED;
    }
}

/* FTP trait functions (NEW) */

void ftp_assign_trait(uint8_t ftp_idx) {
    uint8_t roll;

    if (ftp_idx >= MAX_FTP_SERVERS || !ftps[ftp_idx].active) {
        return;
    }

    /* 40% chance to get a trait */
    roll = random_range(0, 100);
    if (roll < TRAIT_SPAWN_RATE) {
        /* Assign random trait 1-4 */
        ftps[ftp_idx].trait = random_range(1, 5);
    } else {
        ftps[ftp_idx].trait = TRAIT_NONE;
    }
}

const char* ftp_get_trait_tag(uint8_t trait) {
    switch (trait) {
        case TRAIT_FAST:    return "[FAST]";
        case TRAIT_SECURE:  return "[SECURE]";
        case TRAIT_POPULAR: return "[POPULAR]";
        case TRAIT_HIDDEN:  return "[HIDDEN]";
        default:            return "";
    }
}

uint8_t ftp_get_trait_color(uint8_t trait) {
    switch (trait) {
        case TRAIT_FAST:    return COLOR_CYAN;
        case TRAIT_SECURE:  return COLOR_GREEN;
        case TRAIT_POPULAR: return COLOR_YELLOW;
        case TRAIT_HIDDEN:  return COLOR_GRAY3;
        default:            return COLOR_WHITE;
    }
}

uint16_t ftp_get_effective_bandwidth(uint8_t ftp_idx) {
    uint16_t bw;

    if (ftp_idx >= MAX_FTP_SERVERS || !ftps[ftp_idx].active) {
        return 0;
    }

    bw = ftps[ftp_idx].bandwidth;

    /* Apply trait modifiers */
    switch (ftps[ftp_idx].trait) {
        case TRAIT_FAST:
            bw = bw * 2;  /* 2x bandwidth */
            break;
        case TRAIT_HIDDEN:
            bw = (bw * 75) / 100;  /* -25% bandwidth */
            break;
        default:
            break;
    }

    return bw;
}

uint8_t ftp_apply_trait_to_risk(uint8_t ftp_idx, uint8_t base_risk) {
    uint8_t risk = base_risk;

    if (ftp_idx >= MAX_FTP_SERVERS || !ftps[ftp_idx].active) {
        return base_risk;
    }

    /* Apply trait modifiers */
    switch (ftps[ftp_idx].trait) {
        case TRAIT_FAST:
            /* +50% raid risk */
            risk = (risk * 150) / 100;
            if (risk > 95) {
                risk = 95;  /* Cap at 95% */
            }
            break;
        case TRAIT_SECURE:
            /* Risk capped at 50% */
            if (risk > 50) {
                risk = 50;
            }
            break;
        default:
            break;
    }

    /* Apply security audit reduction (NEW) */
    if (gamestate_has_security_audit()) {
        risk = (risk * 75) / 100;  /* -25% */
    }

    return risk;
}
