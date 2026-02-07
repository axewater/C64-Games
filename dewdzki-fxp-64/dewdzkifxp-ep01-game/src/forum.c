#include "forum.h"
#include "ftp.h"
#include "release.h"
#include "gamestate.h"
#include "random.h"

/* Access to release groups for reputation calculation */
extern const GroupData groups[5];

ForumPost posts[MAX_POSTS];

const ForumData forums[4] = {
    {"UNDERGROUND-WAREZ", 3, 1},   /* Medium traffic, base rep */
    {"ELITE-SCENE-BBS",   5, 2},   /* High traffic, double rep */
    {"0DAY-HEAVEN",       7, 2},   /* Very high traffic, double rep */
    {"INNER-CIRCLE",      10, 3}   /* Maximum traffic, triple rep */
};

void forum_init(void) {
    uint8_t i;

    for (i = 0; i < MAX_POSTS; i++) {
        posts[i].active = 0;
        posts[i].nuked = 0;
    }
}

uint8_t forum_create_post(uint8_t release_id, uint8_t ftp_id, uint8_t forum_id) {
    uint8_t i;

    /* Override forum if VIP access active (NEW) */
    if (gamestate_has_vip_access()) {
        forum_id = FORUM_VIP;
    }

    /* Find empty slot */
    for (i = 0; i < MAX_POSTS; i++) {
        if (!posts[i].active) {
            posts[i].active = 1;
            posts[i].release_id = release_id;
            posts[i].ftp_id = ftp_id;       /* Store FTP source (NEW) */
            posts[i].forum_id = forum_id;
            posts[i].downloads = 0;
            posts[i].replies = 0;
            posts[i].age = 0;
            posts[i].rep_earned = 5;  /* Initial +5 rep bonus */
            posts[i].nuked = 0;

            game_state.posts_active_count++;

            /* Mark FTP as used for posts */
            if (ftp_id < MAX_FTP_SERVERS) {
                FTPServer* ftp = ftp_get(ftp_id);
                if (ftp) {
                    ftp->used_for_posts = 1;
                }
            }

            /* Grant immediate reputation */
            gamestate_add_reputation(5);

            return i;
        }
    }

    return 255;  /* Pool full */
}

void forum_update_all_posts(void) {
    uint8_t i;
    uint8_t freshness;
    uint8_t traffic;
    uint8_t downloads;
    Release* rel;
    uint16_t rep;

    for (i = 0; i < MAX_POSTS; i++) {
        if (!posts[i].active) {
            continue;
        }

        posts[i].age++;

        /* Decay after 20 turns */
        if (posts[i].age >= 20) {
            posts[i].active = 0;
            game_state.posts_active_count--;
            continue;
        }

        /* Skip nuked posts (FTP was raided) */
        if (posts[i].nuked) {
            continue;
        }

        /* Generate downloads */
        freshness = (20 - posts[i].age) / 4;  /* 5 -> 0 */
        traffic = forums[posts[i].forum_id].traffic;
        downloads = traffic * (freshness + 1);
        downloads += random_range(0, 5);  /* Variance */

        /* Apply FTP trait modifiers (NEW) */
        if (posts[i].ftp_id < MAX_FTP_SERVERS && ftps[posts[i].ftp_id].active) {
            uint8_t trait = ftps[posts[i].ftp_id].trait;
            if (trait == TRAIT_POPULAR) {
                downloads = (downloads * 150) / 100;  /* +50% */
            } else if (trait == TRAIT_SECURE) {
                downloads = (downloads * 75) / 100;   /* -25% */
            }
        }

        posts[i].downloads += downloads;

        /* Generate replies (less frequent) */
        if (random_range(0, 10) < 3) {
            posts[i].replies += random_range(1, 4);
        }

        /* Calculate rep gain */
        rel = release_get(posts[i].release_id);
        if (rel && rel->active) {
            uint8_t forum_mult;
            uint8_t group_mult;
            uint32_t rep_calc;

            forum_mult = forums[posts[i].forum_id].rep_multiplier;
            group_mult = groups[rel->group].rep_multiplier;

            /* Apply VIP multiplier (NEW) */
            if (posts[i].forum_id == FORUM_VIP) {
                forum_mult *= VIP_REP_MULTIPLIER;
            }

            /* Use uint32_t for intermediate calculation to prevent overflow */
            rep_calc = ((uint32_t)downloads * (rel->quality + 1) * forum_mult * group_mult) / 20;
            /* Cap at uint16_t max for rep variable */
            if (rep_calc > 65535U) {
                rep_calc = 65535U;
            }
            rep = (uint16_t)rep_calc;
            posts[i].rep_earned += rep;
            gamestate_add_reputation(rep);
        }
    }
}

ForumPost* forum_get_post(uint8_t index) {
    if (index >= MAX_POSTS) {
        return 0;
    }
    return &posts[index];
}

const char* forum_get_name(uint8_t forum_id) {
    if (forum_id >= 4) {
        forum_id = 3;
    }
    return forums[forum_id].name;
}

uint8_t forum_has_post(uint8_t release_id, uint8_t ftp_id) {
    uint8_t i;

    for (i = 0; i < MAX_POSTS; i++) {
        if (posts[i].active &&
            posts[i].release_id == release_id &&
            posts[i].ftp_id == ftp_id &&
            !posts[i].nuked) {  /* Don't count nuked posts */
            return 1;  /* Already posted (and still active) */
        }
    }

    return 0;  /* Not posted yet (or nuked) */
}

uint8_t forum_unnuke_post(void) {
    uint8_t i;

    /* Find first nuked post */
    for (i = 0; i < MAX_POSTS; i++) {
        if (posts[i].active && posts[i].nuked) {
            posts[i].nuked = 0;
            return 1;
        }
    }

    return 0;  /* No nuked posts found */
}
