#ifndef FTP_H
#define FTP_H

#include <stdint.h>

#define MAX_FTP_SERVERS 8
#define MAX_RELEASES_PER_FTP 5  /* Limited storage (NEW) */

/* Heat level thresholds */
#define HEAT_LOW       25   /* 0-24% = LOW */
#define HEAT_MEDIUM    50   /* 25-49% = MED */
#define HEAT_HIGH      75   /* 50-74% = HGH */
#define HEAT_CRITICAL  75   /* 75%+ = CRIT */

/* FTP Traits */
#define TRAIT_NONE     0
#define TRAIT_FAST     1  /* 2x bandwidth, +50% raid risk */
#define TRAIT_SECURE   2  /* Risk capped at 50%, -25% downloads */
#define TRAIT_POPULAR  3  /* +50% downloads, normal risk */
#define TRAIT_HIDDEN   4  /* Raids every 3rd turn, -25% bandwidth */

#define TRAIT_SPAWN_RATE 40  /* 40% chance for trait on discovery */

/* Hardware ownership bitmask (NEW) */
#define HW_DIALUP    (1 << 0)
#define HW_14K       (1 << 1)
#define HW_56K       (1 << 2)
#define HW_ISDN      (1 << 3)
#define HW_DSL       (1 << 4)
#define HW_T1        (1 << 5)
#define HW_PHONE2    (1 << 6)  /* +1 action per session */
#define HW_DUAL_ISDN (1 << 7)  /* +64 KB/s bonus */

/* Temporary equipment types (NEW) */
#define TEMP_EQUIP_NONE      0
#define TEMP_EQUIP_T1_LOAN   1  /* +193 KB/s for 10 turns */
#define TEMP_EQUIP_CABLE     2  /* +256 KB/s for 5 turns */
#define TEMP_EQUIP_T3        3  /* +512 KB/s for 3 turns */

/* Hardware upgrade tier structure */
typedef struct {
    const char* name;          /* e.g., "FREE DIALUP" */
    uint16_t rep_cost;         /* Reputation required */
    uint8_t ftps_min;          /* Min FTPs found on success */
    uint8_t ftps_max;          /* Max FTPs found on success */
    uint8_t fail_rate;         /* Failure % (0-100) */
    uint16_t bandwidth;        /* Bandwidth in KB/s */
} HardwareTier;

extern const HardwareTier hardware_tiers[8];

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

    /* FTP trait (NEW) */
    uint8_t trait;                  /* 0=none, 1=FAST, 2=SECURE, 3=POPULAR, 4=HIDDEN */
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

/* Heat management functions (NEW) */
/* Move release from one FTP to another */
uint8_t ftp_move_release(uint8_t src_ftp_idx, uint8_t dst_ftp_idx, uint8_t release_id);

/* Get heat level text: "LOW ", "MED ", "HGH ", "CRIT" */
const char* ftp_get_heat_text(uint8_t ftp_idx);

/* Get heat level color: GREEN, YELLOW, ORANGE, RED */
uint8_t ftp_get_heat_color(uint8_t ftp_idx);

/* FTP trait functions (NEW) */
/* Assign random trait (40% chance) - called during ftp_scan */
void ftp_assign_trait(uint8_t ftp_idx);

/* Get trait display tag: "", "[FAST]", etc. */
const char* ftp_get_trait_tag(uint8_t trait);

/* Get trait color */
uint8_t ftp_get_trait_color(uint8_t trait);

/* Apply trait modifiers to bandwidth */
uint16_t ftp_get_effective_bandwidth(uint8_t ftp_idx);

/* Apply trait modifiers to raid risk */
uint8_t ftp_apply_trait_to_risk(uint8_t ftp_idx, uint8_t base_risk);

#endif /* FTP_H */
