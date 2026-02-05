#include "ftp.h"
#include "forum.h"
#include "random.h"
#include "gamestate.h"
#include <string.h>
#include <stdio.h>

FTPServer ftps[MAX_FTP_SERVERS];

/* FTP name templates */
const char* ftp_prefixes[] = {"FTP", "PUB", "OPEN", "FREE"};
const char* ftp_domains[] = {".SCENE.ORG", ".WAREZ.NET", ".0DAY.COM", ".UPLOAD.NL"};

void ftp_init(void) {
    uint8_t i;

    for (i = 0; i < MAX_FTP_SERVERS; i++) {
        ftps[i].active = 0;
        ftps[i].total_downloads = 0;
        ftps[i].raid_risk = 0;
        ftps[i].used_for_posts = 0;
    }
}

uint8_t ftp_scan(void) {
    FTPServer new_ftp;
    uint8_t success;
    uint8_t prefix_idx;
    uint8_t domain_idx;

    /* 80% success rate */
    success = random_range(0, 10) < 8;

    if (!success) {
        return 0;  /* Failed to find FTP */
    }

    /* Check if pool is full */
    if (game_state.ftps_known_count >= MAX_FTP_SERVERS) {
        return 0;  /* Pool full */
    }

    /* Generate random FTP */
    new_ftp.active = 1;
    new_ftp.bandwidth = random_range(30, 101);
    new_ftp.status = (random_range(0, 10) < 7) ? 1 : 0;  /* 70% open */

    /* Initialize release storage (NEW) */
    new_ftp.release_count = 0;

    /* Initialize raid mechanic fields */
    new_ftp.total_downloads = 0;
    new_ftp.raid_risk = 0;
    new_ftp.used_for_posts = 0;

    /* Generate name */
    prefix_idx = random_range(0, 4);
    domain_idx = random_range(0, 4);
    sprintf(new_ftp.name, "%s%s", ftp_prefixes[prefix_idx], ftp_domains[domain_idx]);

    /* Add to pool */
    return ftp_add(&new_ftp);
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
