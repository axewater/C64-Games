# Dewdzki FXP - Warez Scene Simulator

A text-based C64 game inspired by Neuromancer's cyberspace aesthetics, where you play as a warez scene hacker distributing pirated software through FTP servers and underground forums.

## Game Overview

**Genre**: Turn-based strategy/simulation
**Platform**: Commodore 64
**Interface**: Text-only with PETSCII graphics
**Input**: Keyboard (number keys 1-3, Q to quit)

## Story

You start as a lowly LEECH in the underground warez scene. Your goal: distribute releases, build reputation, and climb through the scene ranks to become an ELITE - a legendary figure in the digital underground.

## Gameplay

### Core Loop
1. **Scan for public FTP servers** - Discover new upload destinations (80% success rate)
2. **Browse topsites** - Access warez from scene topsites (quality improves with rank)
3. **FXP transfer** - Move files from topsite to public FTP (speed based on bandwidth)
4. **Post to forums** - Share links and gain reputation from downloads

### Progression System

**Ranks** (reputation required):
- **LEECH** (0) - Starting rank, basic access
- **USER** (50) - Unlock second forum
- **UPLOADER** (150) - Unlock second topsite
- **TRADER** (350) - Unlock third forum
- **COURIER** (700) - Unlock third topsite
- **ELITE** (1500) - Win! You are scene legend

### Mechanics

**Reputation**: Earned from forum post downloads
- Formula: downloads × quality × forum_multiplier ÷ 10
- Higher rank = better forums = more rep per download

**Resources**:
- Actions: 3 per session (scan/FXP cost 1 action each)
- Bandwidth: Increases with rank, affects FXP transfer speed
- Turns: Track game progression

**Posts Decay**: Forum posts become inactive after 20 turns
- Newer posts = more downloads (freshness factor)
- Keep posting new releases to maintain rep flow

**Release Quality**:
- NUKED: Worst quality
- OK: Basic releases
- PROPER: Good quality
- SCENE: Best quality (highest rep)

## How to Play

### Controls
- **1-3**: Select menu options
- **Q**: Quit
- **SPACE**: Skip animations, continue dialogs

### HUD Information
```
Row 0: RANK:LEECH    REP:0023    BW:45MB/s
Row 1: ACT:3  SITES:2  POSTS:5   TURN:012
```

- **RANK**: Current scene rank
- **REP**: Reputation points (0-9999)
- **BW**: Bandwidth in MB/s (affects transfer speed)
- **ACT**: Actions remaining
- **SITES**: Number of public FTPs known
- **POSTS**: Number of active forum posts
- **TURN**: Turn counter

### Strategy Tips

**Early Game** (LEECH/USER):
- Scan often to build FTP pool
- Post basic releases frequently
- Focus on quantity over quality
- Target: Reach 50 rep for USER rank

**Mid Game** (UPLOADER/TRADER):
- Use better topsites for PROPER releases
- Post to high-traffic forums (2x multiplier)
- Balance scanning vs posting
- Target: Reach 350 rep for TRADER rank

**Late Game** (COURIER/ELITE):
- Only post SCENE quality from best topsite
- Use INNER-CIRCLE forum (3x multiplier)
- Optimize turn efficiency
- Target: Reach 1500 rep for ELITE rank (win!)

## Building from Source

### Prerequisites
- cc65 toolchain (C:\bin\cc65)
- GNU Make (C:\bin\GnuWin32\bin\make.exe)
- VICE emulator (x64sc in PATH)

### Build Commands
```bash
cd C:\Scripts\c64\dewdzkifxp
/c/bin/GnuWin32/bin/make.exe        # Build
/c/bin/GnuWin32/bin/make.exe run    # Build and launch in VICE
/c/bin/GnuWin32/bin/make.exe clean  # Clean build artifacts
```

### Build Output
- Binary: `build/dewdzkifxp.prg` (11 KB)
- Map file: `build/dewdzkifxp.map`

## Project Structure

```
dewdzkifxp/
├── Makefile              # Build configuration
├── README.md             # This file
├── build/                # Compiled binaries
└── src/
    ├── main.c            # Entry point and game loop
    ├── gamestate.c/h     # Game state machine
    ├── rank.c/h          # Rank progression system
    ├── ftp.c/h           # FTP server management
    ├── release.c/h       # Warez generation
    ├── forum.c/h         # Forum posting and stats
    ├── ui.c/h            # Screen rendering
    ├── screen.c/h        # Low-level display (from movegame)
    ├── input.c/h         # Keyboard input
    └── random.c/h        # PRNG (from movegame)
```

## Technical Details

**Language**: C (C89/ANSI C)
**Compiler**: cc65 toolchain
**Memory**: ~11 KB binary, ~10 KB runtime
**Graphics**: Text-only, PETSCII characters
**Screen**: 40×25 characters
**Colors**: Cyan/blue matrix aesthetic

**Data Pools**:
- Max 8 FTP servers
- Max 12 releases in inventory
- Max 8 active forum posts

## Design Principles

1. **Simple arcade flow** - Streamlined scan → FXP → post loop
2. **Clear progression** - Visible rank milestones and unlocks
3. **Risk/reward** - Post decay creates urgency
4. **Replayable** - Short sessions (~20-30 min to USER rank)
5. **Neuromancer aesthetic** - Matrix grid, geometric PETSCII, cyan/blue colors

## Credits

Developed with Claude Code (Sonnet 4.5)
Inspired by Neuromancer's cyberspace battles
Built with cc65 toolchain for Commodore 64

## License

Free to distribute, modify, and share. This is warez scene culture after all! 🏴‍☠️
