# Testing Guide - Chain Loading and Chapter Flow

## Overview

The Dewdzki FXP project now supports automatic chain loading between programs, creating a seamless episodic experience.

## Chain Loading Flow

```
INTRO → GAME → CHAPTER 2 INTRO
  ↑              ↓
  └──────────────┘
  (menu option)
```

## Testing the Complete Flow

### Method 1: Full Episodic Experience

1. **Start from the beginning:**
   ```bash
   cd C:\Scripts\c64
   /c/bin/GnuWin32/bin/make.exe run
   ```

2. **Watch the intro sequence:**
   - PUBLIC LIBRARY scene with sprites
   - Animated effects
   - When complete, shows "LOADING GAME..."
   - **Automatically loads the main game** (no keypress needed)

3. **In the main game:**
   - Splash screen appears: "DEWDZKI FXP - WAREZ SCENE SIMULATOR"
   - Press any key
   - Main menu appears with:
     - [1] NEW GAME
     - [2] HELP
     - **[3] PLAY CHAPTER 2** (yellow - new!)
     - [Q] QUIT

4. **Test Chapter 2 loading:**
   - Press **3**
   - Game **automatically loads Chapter 2 intro**
   - "CHAPTER 2 - THE RISE" screen appears
   - Press any key to exit

### Method 2: Direct Loading (For Quick Testing)

Test individual programs from the D64:

```basic
REM Load intro
LOAD "INTRO",8,1
RUN

REM Load game directly
LOAD "GAME",8,1
RUN

REM Load chapter 2 directly
LOAD "CH02-INTRO",8,1
RUN
```

## What to Verify

### ✅ Intro → Game Chain
- [ ] Intro plays through completely
- [ ] Shows "LOADING GAME..." message
- [ ] Automatically loads main game (no manual intervention)
- [ ] Game splash screen appears
- [ ] No errors or crashes

### ✅ Game Menu
- [ ] Menu displays all 4 options correctly
- [ ] Option [3] shows "PLAY CHAPTER 2" in yellow
- [ ] Selecting [1] starts new game normally
- [ ] Selecting [2] shows help screen
- [ ] Selecting [3] chains to Chapter 2 intro
- [ ] Selecting [Q] quits

### ✅ Chapter 2 Loading
- [ ] Selecting [3] from menu loads Chapter 2
- [ ] "CHAPTER 2 - THE RISE" displays correctly
- [ ] Centered text in proper colors (cyan/white/yellow)
- [ ] Press any key exits cleanly
- [ ] No screen corruption

## Technical Details

### Chain Loading Mechanism

The `chainload()` function uses C64 KERNAL routines:

```c
1. Copy filename to safe memory location ($0340)
2. Call $FFBD (SETNAM) - set filename
3. Call $FFBA (SETLFS) - set logical file (device 8, disk)
4. Call $FFD5 (LOAD) - load program to $0801
5. Jump to $0801 - execute loaded program
```

### File Locations on D64

```
0 "dewdzki fxp     " 00 2a
31   "intro"            prg  (chains to "game")
140  "game"             prg  (menu option chains to "ch02-intro")
7    "ch02-intro"       prg  (standalone exit)
```

## Troubleshooting

### Problem: Intro doesn't load game
**Symptoms:** Intro completes but returns to BASIC "READY."
**Solution:** Ensure `dist/dewdzkifxp.d64` contains "GAME" file:
```bash
/c/bin/GnuWin32/bin/make.exe list
```

### Problem: Menu option 3 doesn't work
**Symptoms:** Pressing 3 exits or does nothing
**Solution:** Rebuild the game:
```bash
cd dewdzkifxp
/c/bin/GnuWin32/bin/make.exe clean && /c/bin/GnuWin32/bin/make.exe
cd ..
/c/bin/GnuWin32/bin/make.exe d64
```

### Problem: "?FILE NOT FOUND ERROR"
**Symptoms:** Chain load shows file not found
**Solution:** Check filename spelling in chainload calls:
- Intro chains to: `"game"` (not "GAME" or "game.prg")
- Game chains to: `"ch02-intro"` (not "CH02-INTRO")
- Filenames are case-insensitive on C64 but must be exact

### Problem: Programs load but don't run
**Symptoms:** Screen clears but nothing happens
**Solution:** Verify KERNAL load addresses in chainload.c:
- Should load to $0801 (BASIC start)
- Should jump to $0801 after load

## Performance Notes

- **Loading time:** 2-5 seconds between programs (emulator)
- **Loading time:** 10-30 seconds on real hardware (1541 drive)
- **Memory usage:** Each program uses full 64KB space
- **No state preservation:** Each program starts fresh (by design)

## Future Enhancements

Potential improvements:
1. Add loading screen with progress indicator
2. Save/load game state between chapters
3. Chapter selection screen (skip to any chapter)
4. Persistent statistics across chapters
5. Chapter 2 game content (currently just placeholder intro)

## Building for Testing

```bash
# Full rebuild
cd C:\Scripts\c64
/c/bin/GnuWin32/bin/make.exe rebuild

# Build and run
/c/bin/GnuWin32/bin/make.exe all && /c/bin/GnuWin32/bin/make.exe run

# Just rebuild D64 (if PRGs already built)
/c/bin/GnuWin32/bin/make.exe d64
```

## VICE Shortcuts

Useful VICE emulator shortcuts during testing:
- **Alt+Q** - Quit VICE
- **Alt+H** - Hard reset (power cycle)
- **Alt+R** - Soft reset (reset button)
- **Alt+W** - Warp mode (fast loading)
- **Alt+M** - Start monitor (debugger)

Enable warp mode for faster testing - loading will be nearly instant!
