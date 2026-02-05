#ifndef UI_H
#define UI_H

#include <stdint.h>
#include <conio.h>
#include "release.h"

/* Initialize UI */
void ui_init(void);

/* Render HUD (always visible) */
void ui_render_hud(void);

/* Print string at position */
void ui_print_string(uint8_t x, uint8_t y, const char* str, uint8_t color);

/* Print centered string */
void ui_print_centered(uint8_t y, const char* str, uint8_t color);

/* Draw horizontal line */
void ui_draw_hline(uint8_t x, uint8_t y, uint8_t width, uint8_t ch, uint8_t color);

/* Draw progress bar */
void ui_draw_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t percent);

/* Animate matrix effect */
void ui_animate_matrix(uint8_t frame);

/* Screen: Splash */
void ui_show_splash(void);

/* Screen: Main Menu */
void ui_show_menu(void);

/* Screen: Playing (hub) */
void ui_show_hub(void);

/* Screen: Scanning for FTP */
void ui_show_scan_anim(void);

/* Screen: Topsite list, returns selected topsite index or 255 for cancel (NEW) */
uint8_t ui_show_topsite_list(void);

/* Screen: Browse topsite, returns selected release index or 255 for cancel */
uint8_t ui_show_topsite(uint8_t topsite_idx, Release* releases, uint8_t count);

/* Screen: FTP selection, returns selected FTP index or 255 for cancel (NEW) */
uint8_t ui_show_ftp_select(Release* rel);

/* Screen: FXP transfer animation */
void ui_show_fxp_anim(Release* rel, uint8_t turns);

/* Screen: Release selection for forum, outputs FTP and release indices (NEW) */
uint8_t ui_show_release_select(uint8_t* out_ftp_idx, uint8_t* out_release_idx);

/* Screen: Forum post confirmation */
void ui_show_forum_post(Release* rel, uint8_t ftp_idx);

/* Screen: Rank up celebration */
void ui_show_rank_up(void);

/* Screen: Game over (win) */
void ui_show_game_over(void);

/* Screen: View detailed stats */
void ui_show_stats(void);

/* Screen: View FTP servers and releases */
void ui_show_ftps(void);

/* Screen: Hardware shop, returns selected tier index or 255 for cancel */
uint8_t ui_show_hardware_shop(void);

/* Screen: Raid alert notification (NEW) */
void ui_show_raid_alert(void);

/* Screen: Random event (NEW) */
void ui_show_event(void);

/* Screen: Move releases between FTPs, returns 1 on success, 0 on cancel (NEW) */
uint8_t ui_show_move_releases(void);

/* Screen: Reputation shop menu, returns selected option or 255 for cancel (NEW) */
uint8_t ui_show_rep_shop(void);

/* Screen: Inside info preview (NEW) */
void ui_show_inside_info(void);

#endif /* UI_H */
