#ifndef FORUM_H
#define FORUM_H

#include <stdint.h>

#define MAX_POSTS 8

/* Forum data structure */
typedef struct {
    const char* name;
    uint8_t traffic;         /* 1-10: affects downloads */
    uint8_t rep_multiplier;  /* 1-3: affects rep gain */
} ForumData;

/* Forum post structure */
typedef struct {
    uint8_t active;
    uint8_t release_id;
    uint8_t forum_id;
    uint16_t downloads;
    uint8_t replies;
    uint8_t age;           /* Age in turns (decay at 20) */
    uint16_t rep_earned;
} ForumPost;

/* Global forum data */
extern const ForumData forums[4];

/* Global post pool */
extern ForumPost posts[MAX_POSTS];

/* Initialize forum system */
void forum_init(void);

/* Create forum post */
uint8_t forum_create_post(uint8_t release_id, uint8_t forum_id);

/* Update all active posts (called each turn) */
void forum_update_all_posts(void);

/* Get post by index */
ForumPost* forum_get_post(uint8_t index);

/* Get forum name */
const char* forum_get_name(uint8_t forum_id);

#endif /* FORUM_H */
