#include "chainload.h"
#include <string.h>
#include <peekpoke.h>

/*
 * Chain load another program from disk.
 *
 * IMPORTANT: The KERNAL LOAD routine loads the new program to $0801,
 * which overwrites the currently running program's code (including this
 * function). To avoid executing overwritten code, we copy a small machine
 * code stub to $0380 (cassette buffer, safe from LOAD) and execute from
 * there. The stub performs: CLALL -> RESTOR -> LOAD -> CINT -> reset stack -> JMP $080D.
 * RESTOR must come BEFORE LOAD because LOAD overwrites $0801+ (the CODE
 * segment), which may contain IRQ handler code. RESTOR points the IRQ vector
 * back to the safe ROM handler before the overwrite begins.
 */
void chainload(const char* filename) {
    unsigned char i;
    unsigned char len = (unsigned char)strlen(filename);

    /* Filename buffer at $0340, loader stub at $0380 */
    unsigned char* name_buf = (unsigned char*)0x0340;
    unsigned char* stub = (unsigned char*)0x0380;

    /* Machine code stub that will run from safe memory ($0380):
     *   JSR $FFE7     ; CLALL - Close all files, reset I/O channels
     *   SEI           ; Disable IRQs
     *   JSR $FF8A     ; RESTOR - Restore KERNAL vectors BEFORE load
     *                 ; (LOAD overwrites $0801+ including old IRQ handler code;
     *                 ;  RESTOR points IRQ back to safe ROM handler first)
     *   CLI           ; Re-enable IRQs (now safe - handler is in ROM)
     *   LDA #$00      ; 0 = load (not verify)
     *   LDX #$01      ; Load address low ($0801)
     *   LDY #$08      ; Load address high
     *   JSR $FFD5     ; LOAD
     *   JSR $FF81     ; CINT - Initialize screen
     *   LDX #$FB      ; Reset stack pointer
     *   TXS
     *   JMP $080D     ; Jump to cc65 program start
     */
    static const unsigned char loader_stub[] = {
        0x20, 0xE7, 0xFF,       /* JSR $FFE7 (CLALL) */
        0x78,                   /* SEI */
        0x20, 0x8A, 0xFF,       /* JSR $FF8A (RESTOR) */
        0x58,                   /* CLI */
        0xA9, 0x00,             /* LDA #$00 */
        0xA2, 0x01,             /* LDX #$01 */
        0xA0, 0x08,             /* LDY #$08 */
        0x20, 0xD5, 0xFF,       /* JSR $FFD5 */
        0x20, 0x81, 0xFF,       /* JSR $FF81 */
        0xA2, 0xFB,             /* LDX #$FB */
        0x9A,                   /* TXS */
        0x4C, 0x0D, 0x08        /* JMP $080D */
    };

    /* Copy filename to $0340 */
    for (i = 0; i < len; i++) {
        name_buf[i] = filename[i];
    }

    /* Copy loader stub to $0380 (safe from LOAD overwrite) */
    for (i = 0; i < sizeof(loader_stub); i++) {
        stub[i] = loader_stub[i];
    }

    /* SETNAM - set filename (runs from current code, still safe) */
    __asm__("lda %v", len);
    __asm__("ldx #$40");      /* Low byte of $0340 */
    __asm__("ldy #$03");      /* High byte of $0340 */
    __asm__("jsr $FFBD");     /* SETNAM */

    /* SETLFS - set file parameters */
    __asm__("lda #$01");      /* Logical file number */
    __asm__("ldx #$08");      /* Device 8 (disk) */
    __asm__("ldy #$01");      /* Secondary address 1 */
    __asm__("jsr $FFBA");     /* SETLFS */

    /* Jump to stub in safe memory - it will do LOAD + init + start */
    __asm__("jmp $0380");
}
