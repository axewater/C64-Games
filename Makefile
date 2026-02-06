# Dewdzki FXP - Root Build System
# Builds all subprojects and creates D64 disk image

# Tools
MAKE := /c/bin/GnuWin32/bin/make.exe
C1541 := C:/bin/vice/bin/c1541.exe

# Directories
DIST_DIR := dist
INTRO_DIR := dewdzkifxp-intro
GAME_DIR := dewdzkifxp
CH02_INTRO_DIR := dewdzkifxp-ch02-intro

# Output files
D64_IMAGE := $(DIST_DIR)/dewdzkifxp.d64
INTRO_PRG := $(INTRO_DIR)/build/intro.prg
GAME_PRG := $(GAME_DIR)/build/dewdzkifxp.prg
CH02_INTRO_PRG := $(CH02_INTRO_DIR)/build/ch02-intro.prg

# All PRG files
PRGS := $(INTRO_PRG) $(GAME_PRG) $(CH02_INTRO_PRG)

# Default target: build everything and create D64
all: build d64
	@echo ========================================
	@echo Build complete!
	@echo D64 image: $(D64_IMAGE)
	@echo ========================================

# Build all subprojects
build: $(PRGS)
	@echo All subprojects built successfully

# Build intro
$(INTRO_PRG):
	@echo Building intro...
	@cd $(INTRO_DIR) && $(MAKE)

# Build main game
$(GAME_PRG):
	@echo Building main game...
	@cd $(GAME_DIR) && $(MAKE)

# Build chapter 2 intro
$(CH02_INTRO_PRG):
	@echo Building chapter 2 intro...
	@cd $(CH02_INTRO_DIR) && $(MAKE)

# Create D64 disk image from PRG files
d64: $(PRGS) | $(DIST_DIR)
	@echo Creating D64 disk image...
	@rm -f $(D64_IMAGE)
	$(C1541) -format "dewdzki fxp,00" d64 $(D64_IMAGE)
	$(C1541) -attach $(D64_IMAGE) -write $(INTRO_PRG) intro
	$(C1541) -attach $(D64_IMAGE) -write $(GAME_PRG) game
	$(C1541) -attach $(D64_IMAGE) -write $(CH02_INTRO_PRG) ch02-intro
	@echo D64 created: $(D64_IMAGE)

# List contents of D64
list: $(D64_IMAGE)
	$(C1541) -attach $(D64_IMAGE) -list

# Create dist directory
$(DIST_DIR):
	@mkdir -p $(DIST_DIR)

# Clean all build artifacts
clean:
	@echo Cleaning intro...
	@cd $(INTRO_DIR) && $(MAKE) clean
	@echo Cleaning main game...
	@cd $(GAME_DIR) && $(MAKE) clean
	@echo Cleaning chapter 2 intro...
	@cd $(CH02_INTRO_DIR) && $(MAKE) clean
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
	@echo   cd dewdzkifxp-intro ^&^& make
	@echo   cd dewdzkifxp ^&^& make
	@echo   cd dewdzkifxp-ch02-intro ^&^& make

.PHONY: all build d64 list clean rebuild run help
