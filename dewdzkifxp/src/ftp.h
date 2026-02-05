#ifndef FTP_H
#define FTP_H

#include <stdint.h>

#define MAX_FTP_SERVERS 8
#define MAX_RELEASES_PER_FTP 5  /* Limited storage (NEW) */

/* Hardware upgrade tier structure */
typedef struct {
    const char* name;          /* e.g., "FREE DIALUP" */
    uint16_t rep_cost;         /* Reputation required */
    uint8_t ftps_min;          /* Min FTPs found on success */
    uint8_t ftps_max;          /* Max FTPs found on success */
    uint8_t fail_rate;         /* Failure % (0-100) */
    uint16_t bandwidth;        /* Bandwidth in KB/s */
} HardwareTier;

extern const HardwareTier hardware_tiers[6];

/* FTP Server structure */
typedef struct {
    uint8_t active;
    char name[20];
    uint16_t bandwidth;             /* Bandwidth in KB/s (64-512) */
    uint8_t status;                 /* 0=closed, 1=open */

    /* Release storage on this FTP (NEW) */
    uint8_t releases[MAX_RELEASES_PER_FTP];  /* Release IDs stored here */
    uint8_t release_count;                   /* Number of releases (0-5) */

    /* Raid mechanic fields */
    uint16_t total_downloads;       /* Accumulated downloads across all posts */
    uint8_t raid_risk;              /* 0-100 percentage risk (cached for UI) */
    uint8_t used_for_posts;         /* 1 if FTP has been used for posting */
} FTPServer;

/* Global FTP pool */
extern FTPServer ftps[MAX_FTP_SERVERS];

/* Initialize FTP system */
void ftp_init(void);

/* Scan for new public FTP - returns number of FTPs found (0 on failure) */
uint8_t ftp_scan(void);

/* Get FTP server by index */
FTPServer* ftp_get(uint8_t index);

/* Add FTP to pool */
uint8_t ftp_add(FTPServer* ftp);

/* Add release to FTP (NEW) */
uint8_t ftp_add_release(uint8_t ftp_idx, uint8_t release_id);

/* Check if FTP has space (NEW) */
uint8_t ftp_has_space(uint8_t ftp_idx);

/* Get release count on FTP (NEW) */
uint8_t ftp_get_release_count(uint8_t ftp_idx);

/* Check if FTP has specific release (NEW) */
uint8_t ftp_has_release(uint8_t ftp_idx, uint8_t release_id);

/* Raid mechanic functions */
uint16_t ftp_calculate_downloads(uint8_t ftp_idx);
void ftp_update_risk(uint8_t ftp_idx);
void ftp_update_all_risks(void);
uint8_t ftp_check_raid(uint8_t ftp_idx);
void ftp_execute_raid(uint8_t ftp_idx);
void ftp_process_raids(void);

#endif /* FTP_H */
