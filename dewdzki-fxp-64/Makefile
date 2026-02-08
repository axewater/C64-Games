# Dewdzki FXP - Root Build System
# Builds all subprojects and creates D64 disk image

# Tools
MAKE := /c/bin/GnuWin32/bin/make.exe
C1541 := C:/bin/vice/bin/c1541.exe

# Directories
DIST_DIR := dist
EP01_INTRO_DIR := dewdzkifxp-ep01-intro
EP01_GAME_DIR := dewdzkifxp-ep01-game
EP02_INTRO_DIR := dewdzkifxp-ep02-intro
EP02_GAME_DIR := dewdzkifxp-ep02-game

# Output files
D64_IMAGE := $(DIST_DIR)/dewdzkifxp.d64
EP01_INTRO_PRG := $(EP01_INTRO_DIR)/build/intro.prg
EP01_GAME_PRG := $(EP01_GAME_DIR)/build/dewdzkifxp.prg
EP02_INTRO_PRG := $(EP02_INTRO_DIR)/build/ch02-intro.prg
EP02_GAME_PRG := $(EP02_GAME_DIR)/build/ep02-game.prg

# All PRG files
PRGS := $(EP01_INTRO_PRG) $(EP01_GAME_PRG) $(EP02_INTRO_PRG) $(EP02_GAME_PRG)

# Default target: build everything and create D64
all: build d64
	@echo ========================================
	@echo Build complete!
	@echo D64 image: $(D64_IMAGE)
	@echo ========================================

# Build all subprojects
build: $(PRGS)
	@echo All subprojects built successfully

# Build episode 1 intro
$(EP01_INTRO_PRG):
	@echo Building episode 1 intro...
	@cd $(EP01_INTRO_DIR) && $(MAKE)

# Build episode 1 game
$(EP01_GAME_PRG):
	@echo Building episode 1 game...
	@cd $(EP01_GAME_DIR) && $(MAKE)

# Build episode 2 intro
$(EP02_INTRO_PRG):
	@echo Building episode 2 intro...
	@cd $(EP02_INTRO_DIR) && $(MAKE)

# Build episode 2 game (placeholder - empty for now)
$(EP02_GAME_PRG):
	@echo Building episode 2 game...
	@cd $(EP02_GAME_DIR) && $(MAKE)

# Create D64 disk image from PRG files
d64: $(PRGS) | $(DIST_DIR)
	@echo Creating D64 disk image...
	@rm -f $(D64_IMAGE)
	$(C1541) -format "dewdzki fxp,00" d64 $(D64_IMAGE)
	$(C1541) -attach $(D64_IMAGE) -write $(EP01_INTRO_PRG) intro
	$(C1541) -attach $(D64_IMAGE) -write $(EP01_GAME_PRG) game
	$(C1541) -attach $(D64_IMAGE) -write $(EP02_INTRO_PRG) ch02-intro
	$(C1541) -attach $(D64_IMAGE) -write $(EP02_GAME_PRG) ch02-game
	@echo D64 created: $(D64_IMAGE)

# List contents of D64
list: $(D64_IMAGE)
	$(C1541) -attach $(D64_IMAGE) -list

# Create dist directory
$(DIST_DIR):
	@mkdir -p $(DIST_DIR)

# Clean all build artifacts
clean:
	@echo Cleaning episode 1 intro...
	@cd $(EP01_INTRO_DIR) && $(MAKE) clean
	@echo Cleaning episode 1 game...
	@cd $(EP01_GAME_DIR) && $(MAKE) clean
	@echo Cleaning episode 2 intro...
	@cd $(EP02_INTRO_DIR) && $(MAKE) clean
	@echo Cleaning episode 2 game...
	@cd $(EP02_GAME_DIR) && $(MAKE) clean
	@echo Cleaning dist...
	@rm -rf $(DIST_DIR)
	@echo Clean complete

# Rebuild everything from scratch
rebuild: clean all

# Run the D64 in VICE
run: $(D64_IMAGE)
	x64sc $(D64_IMAGE)

# Help
help:
	@echo Dewdzki FXP Build System
	@echo ========================
	@echo.
	@echo Targets:
	@echo   all      - Build all subprojects and create D64 (default)
	@echo   build    - Build all subprojects only
	@echo   d64      - Create D64 from existing PRG files
	@echo   list     - List contents of D64 image
	@echo   clean    - Clean all build artifacts
	@echo   rebuild  - Clean and build everything
	@echo   run      - Run D64 in VICE emulator
	@echo   help     - Show this help message
	@echo.
	@echo Individual projects can be built with:
	@echo   cd dewdzkifxp-ep01-intro ^&^& make
	@echo   cd dewdzkifxp-ep01-game ^&^& make
	@echo   cd dewdzkifxp-ep02-intro ^&^& make
	@echo   cd dewdzkifxp-ep02-game ^&^& make

.PHONY: all build d64 list clean rebuild run help
