#include "scenes.h"
#include "helpers.h"
#include "screen.h"
#include "sprite.h"
#include "sprite_data.h"
#include <conio.h>

/* Scene 7: P2P Upload */
uint8_t scene7_p2p_upload(void) {
    uint8_t result;
    uint8_t bar;

    clrscr();

    /* Header */
    textcolor(COLOR_CYAN);
    gotoxy(10, 1);
    cputs("EDONKEY P2P NETWORK");
    screen_draw_hline(0, 2, 40, 0xC0, COLOR_GRAY1);

    wait_frames(20);

    /* eDonkey mule sprite */
    sprite_load(0, sprite_edonkey, SPRITE_DATA_BASE);
    sprite_set_color(0, COLOR_BROWN);
    sprite_set_multicolor(0, 1);
    sprite_fade_in(0, 160, 80);

    wait_frames(20);

    /* Upload files with progress bars */
    textcolor(COLOR_WHITE);
    gotoxy(2, 5);
    cputs("WinZip-8.1-install.exe");
    wait_frames(10);

    gotoxy(2, 6);
    textcolor(COLOR_GREEN);
    cputc('[');
    for (bar = 0; bar < 20; bar++) {
        cputc('#');
        wait_frames(2);
    }
    cputs("] 100%");
    wait_frames(20);

    textcolor(COLOR_WHITE);
    gotoxy(2, 8);
    cputs("WinRAR-3.0-keygen.exe");
    wait_frames(10);
    gotoxy(2, 9);
    textcolor(COLOR_GREEN);
    cputc('[');
    for (bar = 0; bar < 20; bar++) {
        cputc('#');
        wait_frames(2);
    }
    cputs("] 100%");
    wait_frames(20);

    textcolor(COLOR_WHITE);
    gotoxy(2, 11);
    cputs("CuteFTP-5.0-crack.exe");
    wait_frames(10);
    gotoxy(2, 12);
    textcolor(COLOR_GREEN);
    cputc('[');
    for (bar = 0; bar < 20; bar++) {
        cputc('#');
        wait_frames(2);
    }
    cputs("] 100%");
    wait_frames(20);

    textcolor(COLOR_WHITE);
    gotoxy(2, 14);
    cputs("UltraEdit-9.0-patch.exe");
    wait_frames(10);
    gotoxy(2, 15);
    textcolor(COLOR_GREEN);
    cputc('[');
    for (bar = 0; bar < 20; bar++) {
        cputc('#');
        wait_frames(2);
    }
    cputs("] 100%");
    wait_frames(20);

    textcolor(COLOR_WHITE);
    gotoxy(2, 17);
    cputs("Nero-6-serial.exe");
    wait_frames(10);
    gotoxy(2, 18);
    textcolor(COLOR_GREEN);
    cputc('[');
    for (bar = 0; bar < 20; bar++) {
        cputc('#');
        wait_frames(2);
    }
    cputs("] 100%");
    wait_frames(30);

    /* Bottom text */
    screen_draw_hline(0, 20, 40, 0xC0, COLOR_GRAY1);

    textcolor(COLOR_LIGHTRED);
    gotoxy(9, 22);
    cputs("TROJANIZED RELEASES");
    wait_frames(15);

    textcolor(COLOR_YELLOW);
    gotoxy(8, 23);
    cputs("INFECTED: FTP + IRC BOT");
    wait_frames(15);

    textcolor(COLOR_WHITE);
    gotoxy(9, 24);
    cputs("SPREADING ACROSS P2P");
    wait_frames(40);

    /* Prompt */
    textcolor(COLOR_YELLOW);
    gotoxy(8, 26);
    cputs("PRESS ANY KEY");

    result = wait_for_key_or_quit();

    /* Disable sprite */
    sprite_enable(0, 0);

    return result;
}
