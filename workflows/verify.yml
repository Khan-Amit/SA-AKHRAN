@echo off
:: =============================================================================
:: SA-AKHRAN (Sluice Protected Guard) NATIVE DESKTOP CONTAINER LAUNCHER
:: Copyright Seliim Ahmed. All rights reserved. (seliim.ahmed@gmail.com)
:: =============================================================================
title SA-AKHRAN DESKTOP CORE MANAGER
color 0C
clear

echo ======================================================
echo       SA-AKHRAN LOCAL SYSTEM RUNTIME LAUNCHER
echo         Copyright Seliim Ahmed (2026)             
echo ======================================================
echo [SYSTEM] Probing structural local path constraints...

:: Step 1: Structural Path Boundary Check
if not exist "hidden\main.c" (
    echo [ERROR] Critical source folders missing. Execution aborted.
    pause
    exit /b 1
)
if not exist "calculations\dump" (
    echo [WARNING] Data pipeline calculations path missing. Provisioning now...
    mkdir "calculations\dump"
)

:: Step 2: Auto-Compile Core Modules via GCC/Clang
echo [SYSTEM] Assembling hidden security binaries natively...
gcc hidden\main.c hidden\gateway.c hidden\engine.c hidden\collector.c hidden\voip.c -o sa_akhran_core.exe 2>nul

if errorlevel 1 (
    echo [COMPILER WARNING] GCC toolset not detected locally on this machine.
    echo [SYSTEM] Falling back to browser-only sandbox tracking execution mode.
    timeout /t 3 >nul
) else (
    echo [SUCCESS] Standalone native system binary compiled flawlessly.
    echo ------------------------------------------------------
    echo Launching core backend sluice simulation loop...
    timeout /t 1 >nul
    sa_akhran_core.exe
    pause
    exit /b 0
)

:: Step 3: Launch Local Browser UI Canvas
echo [SYSTEM] Booting public control dashboard dashboard interface...
start "" "index.html"
echo ------------------------------------------------------
echo [ACTIVE] SA-AKHRAN dashboard is now active in your web browser.
echo [ACTIVE] Telemetry metrics are hard-locked to calculations\dump\
echo ------------------------------------------------------
pause
