# Dewdzki FXP - Complete Chainloading Flow

## D64 Contents
- `intro` - Episode 1 intro (32 blocks)
- `game` - Episode 1 game (140 blocks)
- `ch02-intro` - Episode 2 intro (35 blocks)
- `ch02-game` - Episode 2 game (94 blocks)
- `ch03-intro` - Episode 3 intro (51 blocks) **✓ NEW!**

## Chainloading Sequence

### Complete Flow Path:
```
1. LOAD "INTRO",8,1     ← User starts here
   ↓
2. EP01 Intro plays
   ↓ (chainload)
3. EP01 Game - Menu appears
   ├─ 1. Play Chapter 1 (turn-based game)
   ├─ 2. Chapter 2        ← User selects this
   │  ↓ (chainload "ch02-intro")
   └─ Q. Quit

4. EP02 Intro plays (cinematic with IRC scenes)
   ↓ (chainload "ch02-game")

5. EP02 Game - Menu appears
   ├─ 1. New Game (tycoon mechanics)
   ├─ 2. How to Play
   ├─ 3. Chapter 3        ← User selects this **✓ NOW WORKING!**
   │  ↓ (chainload "ch03-intro")
   └─ Q. Quit

6. EP03 Intro plays (9 cinematic scenes)
   ↓ (chainload "ch03-game" - TODO when game exists)

7. EP03 Game (not yet implemented)
```

## Chainload Status

### ✓ Working Connections:
- EP01 Intro → EP01 Game ✓
- EP01 Game (menu option 2) → EP02 Intro ✓
- EP02 Intro → EP02 Game ✓
- EP02 Game (menu option 3) → EP03 Intro ✓ **NEWLY ADDED**

### ⚠ Pending:
- EP03 Intro → EP03 Game (game not yet implemented)

## Recent Changes

### Improved IRC Pacing (EP02 & EP03):
- Added dramatic pauses at key story moments
- Extended wait times for important revelations
- Better tension building in dialogue scenes

### EP03 Intro Features:
- Modular architecture (9 separate scene files + helpers)
- 9 cinematic scenes telling "The Botnet" story
- New sprites: MOON (friendly hacker), eDonkey (mule), Firewall
- Reused sprites: AXE, SICK0, ZZZ, RIZZ, LEWIS
- Dark tone: trojanizing innocent users, P2P distribution
- Timeline: 2002 (MS Blaster worm aftermath)

### EP02 Game Integration:
- Chapter 3 menu option now **enabled** (was grayed out)
- Changed from COLOR_GRAY2 to COLOR_LIGHTGREEN
- Wired chainload to "ch03-intro"

## Build System

Build everything with:
```bash
cd dewdzki-fxp-64
make rebuild
```

Run D64 in VICE:
```bash
make run
```

List D64 contents:
```bash
make list
```
