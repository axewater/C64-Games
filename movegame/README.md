# C64 Arena Shooter

A wave-based arena shooter for the Commodore 64 with sprite-based enemies, character-based bullets, and hardware collision detection.

## Game Description

Battle through increasingly difficult waves of enemies in this fast-paced arena shooter. Move freely around the screen, shoot in four directions, and survive as long as you can. Three enemy types with unique behaviors challenge your reflexes as waves intensify.

## Controls

### Keyboard
- **W/A/S/D** - Move up/left/down/right
- **Space** - Shoot (in direction of last movement)
- **Q** - Quit game

### Joystick (Port 2)
- **Joystick directions** - Move
- **Fire button** - Shoot
- **Q key** - Quit game

## Game Features

### Enemy Types
- **Basic Enemy** (Red Diamond) - 1 HP, normal speed, 10 points
- **Fast Enemy** (Orange Elongated) - 1 HP, 2x speed, 20 points
- **Tank Enemy** (Purple Blocky) - 3 HP, normal speed, 30 points

### Wave System
- **Wave 1**: 3 basic enemies
- **Wave 2**: 4 enemies (basic + fast)
- **Wave 3+**: 5-7 enemies (all types), capped at 7 concurrent enemies

### Gameplay Mechanics
- Start with 3 lives (shown as hearts in HUD)
- Lose 1 life when hit by enemy
- Game over when all lives lost
- Enemies spawn from random screen edges
- Enemies chase player using simple AI
- Bullets fire in 4 directions based on movement
- Shooting cooldown: 10 frames between shots
- Max 8 active bullets on screen

### HUD Display
- **Score**: Running total (top left)
- **Wave**: Current wave number (top center)
- **Lives**: Heart indicators (top right)

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

## Project Structure

```
movegame/
├── src/
│   ├── main.c          # Main game loop and integration
│   ├── screen.c/h      # Screen memory and text rendering
│   ├── player.c/h      # Player movement and direction tracking
│   ├── sprite.c/h      # VIC-II sprite system
│   ├── input.c/h       # Keyboard and joystick input
│   ├── bullet.c/h      # Bullet spawning and physics
│   ├── enemy.c/h       # Enemy spawning, AI, and types
│   ├── collision.c/h   # Hardware and software collision detection
│   ├── gamestate.c/h   # State machine, score, lives, waves
│   └── random.c/h      # Pseudo-random number generator
├── build/
│   └── movegame.prg    # Compiled C64 program (8.6 KB)
├── Makefile            # Build system
├── README.md           # This file
└── IMPLEMENTATION.md   # Detailed implementation notes

```

## Technical Details

### Memory Usage
- **Program size**: 8.6 KB
- **CODE segment**: 8,336 bytes
- **RODATA segment**: 256 bytes (sprite data, constants)
- **DATA + BSS**: 277 bytes (variables)

### Sprite System
- **Sprite 0**: Player (yellow spaceship)
- **Sprites 1-7**: Enemies (dynamically assigned)
- **Player sprite data**: 0x0340
- **Enemy sprite data**: 0x3000, 0x3040, 0x3080

### Collision Detection
- **Player vs Enemies**: Hardware sprite collision (VIC-II $D01E)
- **Bullets vs Enemies**: Software bounding box (12-pixel tolerance)

### Coordinate Systems
- **Sprites**: Pixel coordinates (x: 24-320, y: 50-229)
- **Bullets**: Rendered as PETSCII '*' on character grid
- **Conversion**: char_x = (pixel_x - 24) / 8, char_y = (pixel_y - 50) / 8

### Performance
- **Target frame rate**: 30+ FPS
- **Optimizations**:
  - Hardware collision (zero CPU cost)
  - Early exit on inactive objects
  - Limited bullet pool (8 max)
  - Simple chase AI (no pathfinding)

### VIC-II Registers Used
- **$D000-$D00F**: Sprite X/Y positions
- **$D010**: Sprite X MSB (for X > 255)
- **$D015**: Sprite enable register
- **$D01E**: Sprite-sprite collision
- **$D027-$D02E**: Sprite colors

## Development

See `IMPLEMENTATION.md` for detailed implementation notes including:
- Phase-by-phase development breakdown
- Architecture decisions
- File-by-file descriptions
- Memory layout details
- Future enhancement ideas

## Tips & Strategy

1. **Keep moving** - Enemies chase you, so constant movement is key
2. **Control spawn areas** - Position yourself to manage enemy spawns from edges
3. **Prioritize fast enemies** - They're worth more points and harder to avoid
4. **Lead your shots** - Account for enemy movement when shooting
5. **Tank enemies require multiple hits** - Focus fire to take them down
6. **Watch your lives** - Only 3 lives, no continues!

## Credits

Built with cc65 compiler for authentic Commodore 64 development.
Uses VIC-II hardware sprites for smooth 60 Hz movement.
Character-based bullets for unlimited projectiles.

Enjoy the game! 🎮
