# Dewdzki FXP - Multi-Part C64 Game

An episodic Commodore 64 game about the warez/FXP scene, consisting of multiple interconnected programs.

## Project Structure

```
C:\Scripts\c64\
├── dewdzkifxp-intro/          # Animated intro sequence
│   ├── src/                   # Source files
│   ├── build/                 # Build output
│   │   └── intro.prg         # ~6 KB
│   └── Makefile              # Independent build system
│
├── dewdzkifxp/                # Chapter 1: Main game
│   ├── src/                   # Source files
│   ├── build/                 # Build output
│   │   └── dewdzkifxp.prg    # Main game PRG
│   └── Makefile              # Independent build system
│
├── dewdzkifxp-ch02-intro/     # Chapter 2: Intro
│   ├── src/                   # Source files
│   ├── build/                 # Build output
│   │   └── ch02-intro.prg    # ~2 KB
│   └── Makefile              # Independent build system
│
├── dist/                      # Release output
│   └── dewdzkifxp.d64        # Complete disk image
│
├── Makefile                   # Root build orchestrator
└── README.md                  # This file
```

## Build System

### Philosophy
Each subproject maintains its own build system and can be built independently during development. The root Makefile orchestrates building all projects and packaging them into a D64 disk image for distribution.

### Building Everything

```bash
# From root directory
cd C:\Scripts\c64
/c/bin/GnuWin32/bin/make.exe
```

This will:
1. Build dewdzkifxp-intro
2. Build dewdzkifxp
3. Build dewdzkifxp-ch02-intro
4. Create dist/dewdzkifxp.d64 with all programs

### Building Individual Projects

During development, you can build projects independently:

```bash
# Build just the intro
cd dewdzkifxp-intro
/c/bin/GnuWin32/bin/make.exe

# Build just the main game
cd dewdzkifxp
/c/bin/GnuWin32/bin/make.exe

# Build just chapter 2 intro
cd dewdzkifxp-ch02-intro
/c/bin/GnuWin32/bin/make.exe
```

### Available Targets

From the root directory:

| Target | Description |
|--------|-------------|
| `make` or `make all` | Build all projects and create D64 |
| `make build` | Build all projects only (no D64) |
| `make d64` | Create D64 from existing PRG files |
| `make list` | List contents of D64 image |
| `make clean` | Clean all build artifacts |
| `make rebuild` | Clean and build everything fresh |
| `make run` | Run D64 in VICE emulator |
| `make help` | Show help message |

## Development Workflow

### Typical Development Session

1. **Work on a specific project:**
   ```bash
   cd dewdzkifxp
   /c/bin/GnuWin32/bin/make.exe
   # Test in VICE
   /c/bin/GnuWin32/bin/make.exe run
   ```

2. **Create release build:**
   ```bash
   cd C:\Scripts\c64
   /c/bin/GnuWin32/bin/make.exe rebuild
   ```

3. **Test complete D64:**
   ```bash
   /c/bin/GnuWin32/bin/make.exe run
   ```

### Quick Iteration

If you're rapidly iterating on one project:
```bash
cd dewdzkifxp
while true; do
    /c/bin/GnuWin32/bin/make.exe && /c/bin/GnuWin32/bin/make.exe run
    read -p "Press enter to rebuild..."
done
```

## D64 Disk Image

The D64 image (`dist/dewdzkifxp.d64`) contains all program files:

| Filename | Content | Size |
|----------|---------|------|
| INTRO | Animated intro sequence | ~6 KB |
| GAME | Chapter 1 main game | ~40 KB |
| CH02-INTRO | Chapter 2 intro | ~2 KB |

### Loading Programs

In VICE or real C64:
```basic
LOAD "INTRO",8,1
RUN

LOAD "GAME",8,1
RUN

LOAD "CH02-INTRO",8,1
RUN
```

## Chain Loading (Future)

To create a seamless experience, each program will be enhanced to automatically load the next:

- **intro** → loads **game** when complete
- **game** → loads **ch02-intro** when chapter 1 ends
- **ch02-intro** → loads **chapter 2 game** (future)

This requires adding chain loading code to each program's exit point.

## Dependencies

- **cc65** (C compiler for 6502): `C:/bin/cc65/`
- **GNU Make**: `C:/bin/GnuWin32/bin/make.exe`
- **VICE** (C64 emulator): `x64sc`
- **c1541** (D64 tool): `C:/bin/vice/bin/c1541.exe`

## Adding New Chapters

To add a new chapter:

1. **Create new directory:**
   ```bash
   mkdir dewdzkifxp-ch03
   cd dewdzkifxp-ch03
   # Set up src/, Makefile, etc.
   ```

2. **Update root Makefile:**
   - Add new directory variable
   - Add new PRG to PRGS list
   - Add build target
   - Add to d64 target

3. **Build:**
   ```bash
   cd C:\Scripts\c64
   /c/bin/GnuWin32/bin/make.exe
   ```

## Project Components

### dewdzkifxp-intro
Animated intro with sprites and effects. Shows the "Dewdzki FXP" logo and sets the scene.

### dewdzkifxp
Chapter 1 main game. Warez scene simulation with:
- FTP raids
- Forum interactions
- Release management
- Top site rankings
- Sound effects

### dewdzkifxp-ch02-intro
Chapter 2 intro. Simple placeholder showing "CHAPTER 2 - THE RISE". Will be enhanced with story/animations.

## License

Homebrew C64 game project.

## Credits

Built with cc65 toolchain for Commodore 64.
