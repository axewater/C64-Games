@echo off
setlocal
set CC=C:\bin\cc65\bin\cl65.exe
set CFLAGS=-t c64 -O -Or -Cl -Isrc
set BUILD_DIR=build
set SRC_DIR=src

echo Building Dewdzki FXP...

REM Clean build directory
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
mkdir %BUILD_DIR%

REM Compile all C files
for %%f in (%SRC_DIR%\*.c) do (
    echo Compiling %%f...
    %CC% %CFLAGS% -c -o %BUILD_DIR%\%%~nf.o %%f
    if errorlevel 1 (
        echo Error compiling %%f
        exit /b 1
    )
)

REM Link program
echo Linking dewdzkifxp.prg...
%CC% -t c64 -m %BUILD_DIR%\dewdzkifxp.map -o %BUILD_DIR%\dewdzkifxp.prg %BUILD_DIR%\actions.o %BUILD_DIR%\forum.o %BUILD_DIR%\ftp.o %BUILD_DIR%\gamestate.o %BUILD_DIR%\input.o %BUILD_DIR%\main.o %BUILD_DIR%\random.o %BUILD_DIR%\rank.o %BUILD_DIR%\release.o %BUILD_DIR%\screen.o %BUILD_DIR%\topsite.o %BUILD_DIR%\ui_core.o %BUILD_DIR%\ui_events.o %BUILD_DIR%\ui_forums.o %BUILD_DIR%\ui_ftps.o %BUILD_DIR%\ui_hub.o %BUILD_DIR%\ui_releases.o %BUILD_DIR%\ui_shops.o
if errorlevel 1 (
    echo Error linking
    exit /b 1
)

echo Build complete: %BUILD_DIR%\dewdzkifxp.prg
