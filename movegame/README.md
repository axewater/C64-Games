# C64 Movement Game

A simple character movement game for the Commodore 64.

## Controls

- **W** - Move up
- **A** - Move left
- **S** - Move down
- **D** - Move right
- **Q** - Quit

## Building

```bash
cd movegame
make
```

## Running

```bash
make run
```

This will compile the game and launch it in the VICE emulator.

## Requirements

- cc65 compiler (installed at C:\bin\cc65)
- VICE emulator (x64sc in PATH)
- GNU Make

## Files

- `src/main.c` - Main game loop and input handling
- `src/screen.c/h` - Screen memory management
- `src/player.c/h` - Player movement logic
- `build/movegame.prg` - Compiled C64 program

## Technical Details

- Screen: 40x25 characters
- Screen RAM: $0400
- Color RAM: $D800
- Player character: '@' (yellow)
- Background: Blue border, black screen
