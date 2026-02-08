#ifndef UI_H
#define UI_H

#include <stdint.h>

/* --- ui_core.c --- */
void ui_init(void);
void ui_render_hud(void);
void ui_print_string(uint8_t x, uint8_t y, const char* str, uint8_t color);
void ui_print_centered(uint8_t y, const char* str, uint8_t color);
void ui_draw_hline(uint8_t x, uint8_t y, uint8_t len, uint8_t color);
void ui_draw_progress_bar(uint8_t x, uint8_t y, uint8_t current, uint8_t max, uint8_t color);
void ui_print_ip(uint8_t x, uint8_t y, uint8_t range_id, uint8_t o3, uint8_t o4, uint8_t color);
void ui_clear_body(void);

/* --- ui_hub.c --- */
void ui_show_splash(void);
void ui_show_menu(void);
void ui_show_help(void);
void ui_show_hub(void);

/* --- ui_scan.c --- */
void ui_show_scan_range(void);
void ui_show_scan_anim(void);
void ui_show_scan_result(void);

/* --- ui_boxes.c --- */
void ui_show_box_list(void);
void ui_show_box_detail(void);
void ui_show_install_service(void);

/* --- ui_events.c --- */
void ui_show_tool_unlock(void);
void ui_show_discovery(void);
void ui_show_event(void);
void ui_show_cred_dump(void);
void ui_show_cred_status(void);
void ui_show_proxy_post(void);

/* --- ui_shop.c --- */
void ui_show_shop(void);

/* --- ui_stats.c --- */
void ui_show_stats(void);

/* --- ui_events.c (win screen) --- */
void ui_show_game_won(void);

/* --- ui_hub.c (quit confirm) --- */
void ui_show_confirm_quit(void);

#endif /* UI_H */
