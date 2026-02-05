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

/* Screen: Browse topsite, returns selected index or 255 for cancel */
uint8_t ui_show_topsite(Release* releases, uint8_t count);

/* Screen: FXP transfer animation */
void ui_show_fxp_anim(Release* rel, uint8_t turns);

/* Screen: Forum post confirmation */
void ui_show_forum_post(Release* rel);

/* Screen: Rank up celebration */
void ui_show_rank_up(void);

/* Screen: Game over (win) */
void ui_show_game_over(void);

#endif /* UI_H */
