#include "ftp.h"
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
