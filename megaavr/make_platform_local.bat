@echo off
setlocal
pushd "%~dp0"
REM ============================================================
REM  make_platform_local.bat
REM  Place in  WazamonoCore\megaavr\  and run once (re-run after re-clone).
REM
REM  WHY: WazamonoCore is a manual (sketchbook hardware) install, so the
REM  IDE does NOT resolve toolchain dependencies. platform.txt's
REM  {runtime.tools.avr-gcc.path} then falls back to the stock Arduino
REM  avr-gcc 7.3.0, which has no AVR DU support -> the build fails with
REM    "device-specs/specs-avr64du32: No such file or directory".
REM
REM  This script locates the avr-gcc 15.x toolchain and avrdude under the
REM  sketchbook tools folder (same place for both):
REM      Arduino\tools\avr-gcc\avr-gcc-15.2.0\bin\avr-gcc.exe
REM      Arduino\tools\avrdude\<version>\bin\avrdude.exe
REM  (fallback: C:\avr-gcc\avr-gcc-* ; override root with
REM   set "AVRGCC_ROOT=D:\path"  before running)
REM  and writes a platform.local.txt pointing at them.
REM  platform.local.txt is machine-local (absolute paths) and is listed
REM  in .gitignore - it does NOT survive a re-clone; just run this again.
REM
REM  NOTE: do NOT add -mrodata-in-ram here. That flag is per-board
REM  (build.rodata_flags in boards.txt): valid for avrxmega2/4 parts
REM  (avr64du32) but rejected by GCC for avrxmega3 parts (avr32du20).
REM ============================================================

REM     megaavr -> WazamonoCore -> hardware -> Arduino -> tools
set "TOOLS=%~dp0..\..\..\tools"

REM --- avr-gcc: prefer tools\avr-gcc\avr-gcc-15.2.0, else newest avr-gcc-* there,
REM     else the legacy C:\avr-gcc location ---
set "GCCDIR="
if exist "%TOOLS%\avr-gcc\avr-gcc-15.2.0\bin\avr-gcc.exe" (
  for %%d in ("%TOOLS%\avr-gcc\avr-gcc-15.2.0") do set "GCCDIR=%%~fd"
)
if not defined GCCDIR for /d %%d in ("%TOOLS%\avr-gcc\avr-gcc-*") do set "GCCDIR=%%~fd"
if not defined GCCDIR (
  if not defined AVRGCC_ROOT set "AVRGCC_ROOT=C:\avr-gcc"
  for /d %%d in ("%AVRGCC_ROOT%\avr-gcc-*") do set "GCCDIR=%%~fd"
  if not defined GCCDIR if exist "%AVRGCC_ROOT%\bin\avr-gcc.exe" set "GCCDIR=%AVRGCC_ROOT%"
)
if not defined GCCDIR goto :nogcc
if not exist "%GCCDIR%\bin\avr-gcc.exe" goto :nogcc
set "GCCFWD=%GCCDIR:\=/%"

REM --- avrdude (optional; needed to UPLOAD over the USB-CDC bootloader) ---
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
echo Using avr-gcc: %GCCDIR%\bin\avr-gcc.exe
"%GCCDIR%\bin\avr-gcc.exe" --version | findstr /r "avr-gcc"
echo.
echo Wrote platform.local.txt:
echo ------------------------------------------------------------
type platform.local.txt
echo ------------------------------------------------------------
if not defined DUDEDIR echo NOTE: no avrdude found under "%TOOLS%\avrdude" - upload over USB-CDC bootloader will use the IDE default avrdude (no avr64du32 support).
echo Restart the Arduino IDE so the new platform.local.txt is picked up.
popd
endlocal
goto :eof

:nogcc
echo ERROR: avr-gcc not found. Looked for:
echo   "%TOOLS%\avr-gcc\avr-gcc-15.2.0\bin\avr-gcc.exe"
echo   "%TOOLS%\avr-gcc\avr-gcc-*\bin\avr-gcc.exe"
echo   "%AVRGCC_ROOT%\avr-gcc-*\bin\avr-gcc.exe"
echo Put the toolchain at  Arduino\tools\avr-gcc\avr-gcc-15.2.0\  (or set AVRGCC_ROOT).
popd
endlocal
exit /b 1
