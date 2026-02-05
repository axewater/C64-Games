# Dewdzki FXP - Intro Sequence

Standalone 8-scene episodic intro that tells the story of entering the warez scene.

## Story

1. **Library Exterior** - Establishing shot, 1995 public library
2. **At the Terminal** - Blinking cursor sprite at free internet terminal
3. **Login Screen** - IRC client connecting with spinning loader animation
4. **Channel List** - Arrow sprite slides to point at #hacking channel
5. **Join #hacking** - Chat window with typing effect showing warez discussion
6. **Lewis Appears** - Elite warez trader avatar slides in, offers to teach
7. **TOP SITE Lesson** - Lewis explains warez scene with pulsing FTP server icon
8. **FTP Scan Demo** - Radar sweep animation showing how to find TOP SITES

## Technical Details

### Hardware Sprites
- **Sprite 0**: Multi-purpose (cursor/spinner/arrow) - WHITE/YELLOW
- **Sprite 1**: Lewis avatar (24×21 face) - CYAN
- **Sprite 2**: FTP server icon - GRAY2

### Features
- PETSCII character art for backgrounds
- Hardware sprite animations (blink, rotate, slide, pulse)
- Character-by-character typing effects
- Radar sweep visualization
- Q key to skip at any time

### Memory Usage
- **Total size**: 6.0 KB
- **Code**: ~4.2 KB
- **Sprite data**: ~1.3 KB (7 sprites × 63 bytes each)
- **BSS**: ~0.1 KB

## Building

```bash
cd dewdzkifxp-intro
/c/bin/GnuWin32/bin/make.exe
```

Output: `build/intro.prg`

## Running

### In VICE Emulator
```bash
/c/bin/GnuWin32/bin/make.exe run
```

Or manually:
```bash
x64sc build/intro.prg
```

### Controls
- **Any key**: Advance through scenes (where prompted)
- **Q**: Quit intro at any time

## File Structure

```
dewdzkifxp-intro/
├── src/
│   ├── main.c          # Entry point
│   ├── intro.c/h       # Intro sequence logic & sprite data
│   ├── sprite.c/h      # VIC-II sprite system
│   └── screen.c/h      # Screen utilities & PETSCII drawing
├── build/
│   ├── intro.prg       # C64 executable
│   └── intro.map       # Memory map
├── Makefile
└── README.md
```

## Integration Plan

To integrate into main `dewdzkifxp` game:

1. Copy `intro.c/h` and `sprite.c/h` to `dewdzkifxp/src/`
2. Add `STATE_INTRO` to `gamestate.h`
3. Modify `main.c` state machine:
   - Change SPLASH → INTRO transition
   - Add INTRO → MENU transition
4. Reuse existing `screen.c` functions instead of standalone versions

## Animation Details

### Typing Effect
3 frames per character for realistic chat message appearance

### Sprite Slide
20 frames smooth interpolation from start to end position

### Cursor Blink
30-frame cycle (15 on, 15 off)

### Spinner Rotation
4 frames rotating at 8 vsyncs each

### FTP Pulse
Toggle Y expand every 15 frames for breathing effect

### Radar Sweep
Random matrix characters with cycling glyphs (/, \, |, -)

## Color Scheme

Matches main game aesthetic:
- **Cyan** - Elite/emphasis (titles, Lewis, #hacking)
- **White** - Standard text
- **Blue** - Borders/decorative
- **Yellow** - Highlights/prompts
- **Green** - Success messages

## Next Steps

After testing and refinement:
1. Test all 8 scenes for visual clarity
2. Adjust timing of auto-advance scenes if needed
3. Verify Q-to-skip works in all scenes
4. Polish sprite graphics if needed
5. Integrate into main game as STATE_INTRO
