@echo off
setlocal
pushd "%~dp0"
REM ============================================================
REM  make_platform_local.bat
REM  Place in  WazamonoCore\megaavr\  and run once.
REM
REM  WHY: WazamonoCore is a manual (sketchbook hardware) install, so the
REM  IDE does NOT resolve toolchain dependencies. platform.txt's
REM  {runtime.tools.avr-gcc.path} then falls back to the stock Arduino
REM  avr-gcc 7.3.0, which has no AVR DU support -> the build fails with
REM    "device-specs/specs-avr64du32: No such file or directory".
REM
REM  This script auto-detects the avr-gcc 15.x toolchain at C:\avr-gcc
REM  (override root with:  set "AVRGCC_ROOT=D:\path"  before running)
REM  and the avrdude under Arduino\tools\avrdude, then writes a
REM  platform.local.txt pointing at them. platform.local.txt is
REM  environment-specific (absolute paths) and is listed in .gitignore.
REM
REM  NOTE: do NOT add -mrodata-in-ram here. That flag is per-board
REM  (build.rodata_flags in boards.txt): valid for avrxmega2/4 parts
REM  (avr64du32) but rejected by GCC for avrxmega3 parts (avr32du20).
REM ============================================================

REM --- avr-gcc under C:\avr-gcc (version auto-detected) ---
if not defined AVRGCC_ROOT set "AVRGCC_ROOT=C:\avr-gcc"
set "GCCDIR="
for /d %%d in ("%AVRGCC_ROOT%\avr-gcc-*") do set "GCCDIR=%%~fd"
if not defined GCCDIR if exist "%AVRGCC_ROOT%\bin\avr-gcc.exe" set "GCCDIR=%AVRGCC_ROOT%"
if not defined GCCDIR goto :nogcc
if not exist "%GCCDIR%\bin\avr-gcc.exe" goto :nogcc
set "GCCFWD=%GCCDIR:\=/%"

REM --- avrdude (optional; needed to UPLOAD over the USB-CDC bootloader) ---
REM     megaavr -> WazamonoCore -> hardware -> Arduino -> tools
set "TOOLS=%~dp0..\..\..\tools"
set "DUDEDIR="
for /d %%d in ("%TOOLS%\avrdude\*") do set "DUDEDIR=%%~fd"

> platform.local.txt echo # WazamonoCore platform.local.txt  (machine-local - listed in .gitignore)
>> platform.local.txt echo # Points the build at the local avr-gcc 15.x toolchain (AVR DU support,
>> platform.local.txt echo # avr-libc 2.3.1 with merged WDT fixes) instead of the IDE default 7.3.0.
>> platform.local.txt echo compiler.path=%GCCFWD%/bin/
if not defined DUDEDIR goto :nodude
set "DUDEFWD=%DUDEDIR:\=/%"
>> platform.local.txt echo.
>> platform.local.txt echo # avrdude with avr64du32 support (sketch upload + burn bootloader)
>> platform.local.txt echo tools.avrdude.path=%DUDEFWD%/
:nodude

echo.
echo Wrote platform.local.txt:
echo ------------------------------------------------------------
type platform.local.txt
echo ------------------------------------------------------------
if not defined DUDEDIR echo NOTE: no avrdude found under "%TOOLS%\avrdude" - upload over USB-CDC bootloader will use the IDE default avrdude (no avr64du32 support).
popd
endlocal
goto :eof

:nogcc
echo ERROR: no avr-gcc-*\bin\avr-gcc.exe found under "%AVRGCC_ROOT%"
echo   Put the toolchain at  C:\avr-gcc\avr-gcc-15.2.0\  (or set AVRGCC_ROOT).
popd
endlocal
exit /b 1
