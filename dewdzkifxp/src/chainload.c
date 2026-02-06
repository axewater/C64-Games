#include "chainload.h"
#include <string.h>
#include <peekpoke.h>

void chainload(const char* filename) {
    unsigned char i;
    unsigned char len = (unsigned char)strlen(filename);

    /* Copy filename to a safe location in memory ($0340-$03FF is usuall safe) */
    unsigned char* name_buf = (unsigned char*)0x0340;
    for (i = 0; i < len; i++) {
        name_buf[i] = filename[i];
    }

    /* Call KERNAL routines via inline assembly */
    __asm__("lda %v", len);
    __asm__("ldx #$40");      /* Low byte of $0340 */
    __asm__("ldy #$03");      /* High byte of $0340 */
    __asm__("jsr $FFBD");     /* SETNAM */

    __asm__("lda #$01");      /* Logical file number */
    __asm__("ldx #$08");      /* Device 8 (disk) */
    __asm__("ldy #$01");      /* Secondary address 1 */
    __asm__("jsr $FFBA");     /* SETLFS */

    __asm__("lda #$00");      /* 0 = load */
    __asm__("ldx #$01");      /* Load address low ($0801) */
    __asm__("ldy #$08");      /* Load address high */
    __asm__("jsr $FFD5");     /* LOAD */

    __asm__("jmp $080D");     /* Jump to cc65 program start (after BASIC stub) */
}
