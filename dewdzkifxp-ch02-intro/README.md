# Dewdzki FXP - Chapter 2 Intro

Simple placeholder intro screen for Chapter 2.

## Display
- Title: "CHAPTER 2"
- Subtitle: "THE RISE"
- Instruction: "PRESS ANY KEY TO CONTINUE"

## Building
```bash
cd C:\Scripts\c64\dewdzkifxp-ch02-intro
/c/bin/GnuWin32/bin/make.exe
```

Output: `build/ch02-intro.prg` (~2 KB)

## Running
```bash
# Via Makefile
/c/bin/GnuWin32/bin/make.exe run

# Or directly in VICE
x64sc build/ch02-intro.prg
```

## File Structure
- `src/main.c` - Entry point and title display
- `src/screen.c/h` - Screen utilities (copied from dewdzkifxp-intro)
- `Makefile` - cc65 build configuration
- `build/` - Compiled output

## Future Enhancements
This is a placeholder that can be expanded with:
- Chapter-specific narrative/story text
- Sprite animations
- Scene transitions
- Effects (similar to main intro)
