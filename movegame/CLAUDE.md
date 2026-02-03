# C64 Arena Shooter - Developer Quickstart Guide

## Overview
A wave-based arena shooter for the Commodore 64, written in C using the cc65 toolchain. Features sprite-based player, enemies, and bullets with 8-directional firing, hardware collision detection, chase AI, and progressive wave difficulty.

## Prerequisites

### Required Tools
- **cc65 toolchain**: C compiler for 6502/C64 (installed at `C:\bin\cc65`)
- **GNU Make**: Build automation (installed at `C:\bin\GnuWin32\bin`)
- **VICE Emulator**: x64sc for testing (must be in PATH or adjust Makefile)

### Installation Check
```bash
C:\bin\cc65\bin\cl65.exe --version
C:\bin\GnuWin32\bin\make.exe --version
x64sc --version
```

## Quick Start

### Build & Run
```bash
cd C:\Scripts\c64\movegame
/c/bin/GnuWin32/bin/make.exe        # Build
/c/bin/GnuWin32/bin/make.exe run    # Build and launch in VICE
/c/bin/GnuWin32/bin/make.exe clean  # Clean build artifacts
```

### Build Output
- **Binary**: `build/movegame.prg` (C64 executable)
- **Map File**: `build/movegame.map` (memory layout and symbols)
- **Memory Budget**: ~8.7 KB used of 38 KB available

## Project Structure

```
src/
├── main.c           # Entry point, game loop, system integration
├── gamestate.c/h    # State machine (menu/playing/game over), score, lives, waves
├── player.c/h       # Player movement, direction tracking, sprite management
├── enemy.c/h        # Enemy spawning, AI, three types, wave management
├── bullet.c/h       # Bullet pool, spawning, physics, rendering
├── collision.c/h    # Hardware (player-enemy) and software (bullet-enemy) collision
├── sprite.c/h       # VIC-II sprite system, sprite data for player and enemies
├── input.c/h        # Keyboard (WASD/Space/Q) and joystick input
├── screen.c/h       # Screen memory management, text rendering, HUD
└── random.c/h       # PRNG for enemy spawning and AI variation

build/               # Compiled objects and final .prg
```

## Key Systems

### Enemy System
- **3 Enemy Types**:
  - Basic (Red, Diamond): 1 HP, speed 1, 10 points
  - Fast (Orange, Elongated): 1 HP, speed 2, 20 points
  - Tank (Purple, Blocky): 3 HP, speed 1, 30 points
- **Max 6 concurrent enemies** (sprites 1-6, sprite 7 reserved for bullet)
- **Chase AI**: Enemies move toward player position each frame
- **Edge spawning**: Spawn from random screen edge (top/bottom/left/right)
- **Sprite data locations**: 0x3000, 0x3040, 0x3080

### Bullet System
- **Sprite-based rendering**: Uses sprite 7 with diamond/circle shape (yellow)
- **Max 1 active bullet** (era-appropriate, single-shot gameplay)
- **8-directional**: Fire in cardinal directions (up/down/left/right) and diagonals (up-left/up-right/down-left/down-right)
- **Speed**: 4 pixels per frame (cardinal), 3 pixels per frame (diagonal for balanced speed)
- **Cooldown**: 10 frames between shots
- **Cleanup**: Automatic sprite disable when off-screen or on hit
- **Sprite data location**: 0x3100 (63 bytes)
- **Direction tracking**: Fires in direction of last player movement, including diagonals
- **Direction constants** (defined in bullet.h):
  - DIR_UP (1), DIR_DOWN (2), DIR_LEFT (3), DIR_RIGHT (4)
  - DIR_UP_LEFT (5), DIR_UP_RIGHT (6), DIR_DOWN_LEFT (7), DIR_DOWN_RIGHT (8)
  - Player's last_dir field tracks current facing direction

### Collision System
- **Hardware collision**: Player vs enemies using VIC-II register $D01E
  - Zero CPU cost, instant detection
  - Filters out bullet sprite collisions (verifies actual enemy proximity)
  - Triggers life loss only when enemy is within 12-pixel tolerance
- **Software collision**: Bullets vs enemies using bounding box
  - 12-pixel tolerance for hit detection
  - Destroys bullet (disables sprite) and damages enemy

### Wave System
- **Wave 1**: 3 basic enemies
- **Wave 2**: 4 enemies (basic + fast mix)
- **Wave 3+**: 5-6 enemies (all types), capped at 6
- **Progression**: Automatic when all enemies destroyed
- **Enemy composition**: Changes based on wave number

### Game State
- **3 States**: Menu (unused), Playing, Game Over
- **Score tracking**: 10/20/30 points per enemy type
- **Lives**: Start with 3, lose 1 per hit, game over at 0
- **Wave counter**: Increments on wave completion

## Memory Layout

### Sprite Memory
- **Player sprite**: 0x0340 (sprite 0)
- **Enemy sprites**: 0x3000, 0x3040, 0x3080 (sprite data for 3 types, sprites 1-6)
- **Bullet sprite**: 0x3100 (sprite 7, diamond/circle shape)
- **Sprite pointers**: 0x07F8-0x07FF (screen memory)

### Screen Memory
- **Screen**: 0x0400-0x07E7 (1000 bytes, 40x25 chars)
- **Color RAM**: 0xD800-0xDBE7 (1000 bytes)
- **HUD**: Row 0 (score, wave, lives)
- **Play area**: Rows 1-24

### VIC-II Registers
- **$D000-$D00F**: Sprite X/Y positions (8 sprites)
- **$D010**: Sprite X MSB (for X > 255)
- **$D015**: Sprite enable register
- **$D01E**: Sprite-sprite collision register
- **$D027-$D02E**: Sprite color registers

### Coordinate Systems
- **Sprite coordinates**: Pixels (x: 24-320, y: 50-229)
- **Bullet rendering**: Direct pixel positioning (no conversion needed, sprite-based)
- **Screen chars**: 40x25 grid (0-39, 0-24) for HUD and text

## Adding New Features

### Add a New Enemy Type
1. **Define sprite data**: Add 63-byte array in `enemy.c` (e.g., `enemy_sniper_sprite[63]`)
2. **Load sprite**: Add to `sprite_load_enemy_data()` at new memory location
3. **Add type constant**: Define in `enemy.h` (e.g., `#define ENEMY_TYPE_SNIPER 3`)
4. **Set stats**: Add case in `enemy_spawn()` for HP, speed, color, sprite pointer
5. **Update score**: Add case in `enemy_hit()` for point value
6. **Update spawn logic**: Modify `enemy_spawn_wave()` to include new type

### Add Power-Ups
1. **Create powerup.c/h**: Similar structure to bullet.c/h
2. **Define types**: Health, rapid fire, shield, etc.
3. **Spawn system**: Random or on enemy kill
4. **Collision**: Check player-powerup collision in `collision.c`
5. **Effects**: Modify player stats or add temporary flags
6. **Render**: Character-based like bullets, or use sprites

### Add Sound Effects
1. **Create sound.c/h**: SID chip control functions
2. **Define effects**: Shoot, hit, death, wave complete
3. **SID registers**: $D400-$D41C (see cardbattler for reference)
4. **Trigger**: Call from game events (shoot, collision, etc.)
5. **Note**: Use simple waveforms (triangle, pulse, noise) for effects

### Modify Enemy AI
1. **Current**: Simple chase in `enemy_update_all()`
2. **Add patterns**: Wander, patrol, flee, surround
3. **AI type field**: Add `ai_type` to Enemy struct
4. **Behavior switch**: Check type in update, apply different movement
5. **State machine**: Add enemy states (idle, chase, attack, flee)

### Modify Direction System
1. **Current**: 8-directional detection in `player_move()` in player.c
2. **Critical order**: Check diagonals FIRST (dx && dy), then cardinals (dx || dy)
3. **Why order matters**: If you check `dy < 0` before `dx < 0 && dy < 0`, it matches the cardinal direction before checking for diagonal
4. **Direction usage**: `player.last_dir` is used by bullet_spawn() to set bullet velocity
5. **Adding directions**: Define new constants in bullet.h, add cases to bullet_spawn() switch statement
6. **Testing**: Move diagonally (W+A, W+D, etc.) then fire - bullet should travel at 45° angle

## Development Tips

### Debugging
- **Map File**: Check `build/movegame.map` for symbol addresses
- **Memory Usage**: Look at segment sizes (CODE, RODATA, BSS)
- **VICE Monitor**: Press ALT+H in VICE for machine language monitor
- **Print Debug**: Use `screen_set_char()` for debug output

### Performance
- **Direct Memory Write**: Use `SCREEN_MEM[]` and `COLOR_MEM[]` arrays
- **Early Exits**: Check `active` flag first in all loops
- **Pool Patterns**: Fixed-size arrays for bullets, enemies
- **Hardware Features**: Use VIC-II collision when possible

### Code Style
- **C89 Standard**: cc65 uses ANSI C (no C99 features like `//` comments or variable declarations mid-function)
- **Declare at Top**: All variables must be declared at function start
- **8-bit Types**: Use `uint8_t` for most values (0-255)
- **16-bit Types**: Use `uint16_t` for scores, coordinates > 255
- **Const Data**: Mark read-only data as `const` to store in ROM

### Memory Optimization
- **Current Usage**: ~8.7 KB (plenty of headroom)
- **Sprite Data**: 63 bytes per sprite (player + 3 enemy types + bullet = 315 bytes total)
- **If Tight**: Simplify sprite shapes, reduce bullet sprite data
- **Check Growth**: Monitor map file after changes
- **Sprite Reuse**: Share sprite data between similar enemies
- **Memory Map**: Player: 0x0340, Enemies: 0x3000-0x3080, Bullet: 0x3100

## Common Tasks

### Test with More/Fewer Enemies
1. Modify `gamestate_start_game()` in `gamestate.c`
2. Change `enemies_to_spawn` initial value
3. Rebuild and run

### Adjust Difficulty
1. **Enemy speed**: Modify speed multiplier in `enemy_update_all()`
2. **Enemy HP**: Change values in `enemy_spawn()` type switch
3. **Spawn rate**: Modify `gamestate_next_wave()` progression formula
4. **Player lives**: Change initial value in `gamestate_start_game()`

### Change Bullet Behavior
1. **Speed**: Modify `BULLET_SPEED` constant in `bullet.c` (affects cardinal directions)
2. **Diagonal speed**: Adjust diagonal velocity values in `bullet_spawn()` (currently 3, vs 4 for cardinal)
3. **Max count**: Change `MAX_BULLETS` in `bullet.h` (currently 1 for single-shot gameplay)
4. **Cooldown**: Modify cooldown value in `main.c` shooting logic
5. **Sprite appearance**: Modify `bullet_sprite_data[]` array in `bullet.c` to change bullet shape
6. **Color**: Change `sprite_set_color()` call in `bullet_init()` to use different color

### Modify HUD Layout
1. Edit `gamestate_render_ui()` in `gamestate.c`
2. Change screen_set_char() positions for text
3. Update screen_print_number() calls for values
4. Keep Row 0 reserved for HUD

## Build Flags

### Current Flags (in Makefile)
- `-t c64`: Target Commodore 64
- `-O`: Enable basic optimizations
- `-Or`: Enable register optimizations
- `-Cl`: Make local variables static when possible
- `-I$(SRC_DIR)`: Include path for headers
- `-m`: Generate map file

### Debug Build (if needed)
Remove `-O -Or -Cl` and add `-g` for debug symbols (increases size).

## Architecture Decisions

### Why Hardware Collision for Player?
VIC-II provides instant sprite-sprite collision detection at zero CPU cost. The register auto-clears on read, perfect for one-time events like losing a life.

### Why Sprite Bullets?
By reducing MAX_ENEMIES from 7 to 6, we free sprite 7 for a bullet. Sprite-based bullets provide:
- **Pixel-perfect positioning**: Smooth movement at any position, not limited to 8-pixel character grid
- **8-directional firing**: Diagonal bullets at true 45° angles
- **Better visuals**: Custom diamond sprite shape vs PETSCII asterisk
- **Simpler rendering**: Direct sprite positioning, no coordinate conversion needed
- **Era-appropriate**: Single bullet limit (like classic arcade games) compensates for smoother, more precise shooting

### Why Fixed Pools?
No dynamic allocation on C64. Fixed-size arrays prevent fragmentation and make memory usage predictable. Max limits (8 bullets, 7 enemies) tuned for performance.

### Why Simple Chase AI?
Complex pathfinding too expensive for 6502. Direct chase toward player creates engaging gameplay while maintaining 30+ FPS on real hardware.

### Why Edge Spawning?
Prevents enemies spawning on top of player. Edge spawning gives player time to react and creates natural difficulty progression as enemies converge.

## Performance Benchmarks

- **Target**: 30+ FPS on real C64
- **Achieved**: Smooth gameplay at 30+ FPS
- **Bottlenecks**:
  - Bullet-enemy collision (single bullet vs 6 enemies, minimal)
  - Enemy AI updates (6 enemies * chase calc)
  - Player-enemy collision verification (checks proximity when hardware collision detected)
- **Optimizations Applied**:
  - Early exit on inactive objects
  - Hardware collision detection with software verification
  - Limited pool sizes (1 bullet, 6 enemies)

## Troubleshooting

### Game Too Easy/Hard
- Adjust `BULLET_SPEED` in bullet.c
- Modify enemy HP in enemy_spawn()
- Change cooldown in main.c
- Adjust wave progression in gamestate_next_wave()

### Bullets Not Rendering
- Check bullet active flag in bullets[0]
- Verify sprite 7 is enabled (check $D015 register in VICE monitor)
- Check sprite color is set (should be yellow, color 7)
- Verify sprite data loaded at 0x3100
- Check sprite pointer: SPRITE_PTRS[7] should be 0xC4 (0x3100/64)
- Use VICE monitor: `d D00E D00F` to check sprite 7 X/Y position

### Enemies Not Spawning
- Check enemy_count_active() < MAX_ENEMIES
- Verify sprite pointer setup in enemy_spawn()
- Check sprite enable register ($D015)

### Collision Not Working
- Player-enemy: Check VIC register $D01E, verify sprite enable
  - Collision system filters out bullet sprite (sprite 7) to prevent false positives
  - Verifies enemy is within 12-pixel tolerance before triggering life loss
- Bullet-enemy: Check tolerance value, verify active flags
  - Uses software bounding box detection (12-pixel tolerance)
- Ensure collision_update() called every frame
- If player loses life when shooting: Bullet sprite may be colliding - check collision filtering logic

## Future Enhancements

### High Priority
- Sound effects using SID chip
- Boss enemies at wave milestones
- Power-ups (extra life, rapid fire, shield)

### Medium Priority
- More enemy AI types (wander, patrol, flee)
- Particle effects on death
- Screen shake on hit
- Title screen with instructions

### Low Priority
- High score persistence
- Multiple lives display
- Enemy spawn animations
- Multicolor sprites

## Testing Checklist

### Movement & Controls
- [ ] Movement in all 4 cardinal directions (W/A/S/D)
- [ ] Diagonal movement works (W+A, W+D, S+A, S+D)
- [ ] Joystick controls work

### 8-Directional Shooting
- [ ] Shoot up (W → Space)
- [ ] Shoot down (S → Space)
- [ ] Shoot left (A → Space)
- [ ] Shoot right (D → Space)
- [ ] Shoot diagonal up-left (W+A → Space)
- [ ] Shoot diagonal up-right (W+D → Space)
- [ ] Shoot diagonal down-left (S+A → Space)
- [ ] Shoot diagonal down-right (S+D → Space)
- [ ] Only 1 bullet on screen at a time
- [ ] Bullet sprite is visible (yellow diamond shape)
- [ ] Diagonal bullets travel at 45° angle smoothly

### Collision & Combat
- [ ] Bullet-enemy collision works from all directions
- [ ] Player-enemy collision works (lose life)
- [ ] Bullet sprite doesn't cause false player collision
- [ ] Bullet disappears on enemy hit
- [ ] Bullet disappears when off-screen
- [ ] Tank enemy takes 3 hits
- [ ] Score increases on enemy death (10/20/30 points)

### Game Progression
- [ ] Lives decrease on enemy hit
- [ ] Game over at 0 lives
- [ ] Wave progresses when enemies cleared
- [ ] Wave counter increments
- [ ] Max 6 enemies spawn (not 7)
- [ ] All 3 enemy types spawn
- [ ] Fast enemy moves faster than basic
- [ ] HUD displays correctly (score, wave, lives)
- [ ] Game over screen shows

### System
- [ ] Q key quits
- [ ] No sprite flickering
- [ ] Smooth 30+ FPS gameplay

## References

- **cc65 Documentation**: https://cc65.github.io/doc/
- **C64 Memory Map**: https://sta.c64.org/cbm64mem.html
- **VIC-II Guide**: https://www.c64-wiki.com/wiki/VIC
- **Sprite Tutorial**: https://www.c64-wiki.com/wiki/Sprite
- **VICE Manual**: https://vice-emu.sourceforge.io/vice_toc.html
