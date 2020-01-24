@echo off
:NEXTCHECK
if NOT xxx%1 == xxx goto DOIT
echo usage: %0 [archive_name]
goto ERR

:DOIT
: NOTE that DOS (Doodoo On Silicon) doesn't set the errorlevel
: for these commands, so error checking doesn't work

:UNIX (Upscale Nuveaux Interface eXpectorant)

echo Restoring from %1...
echo *.c
pkxarc -r %1 .\src *.c > nul
if errorlevel 1 goto ERR

echo *.asm
pkxarc -r %1 .\src *.asm > nul
if errorlevel 1 goto ERR

echo *.lib
pkxarc -r %1 .\src *.lib > nul
if errorlevel 1 goto ERR

echo *.h
pkxarc -r %1 *.h inc > nul
if errorlevel 1 goto ERR

echo *.def
pkxarc -r %1 *.def > nul
if errorlevel 1 goto ERR

echo *.cur
pkxarc -r %1 *.cur > nul
if errorlevel 1 goto ERR

echo *.mak
pkxarc -r %1 *.mak > nul
if errorlevel 1 goto ERR

echo *.lnk
pkxarc -r %1 *.lnk > nul
if errorlevel 1 goto ERR

:echo database
:pkxarc -r %1 0* > nul
:pkxarc -r %1 0*.* > nul
:pkxarc -r %1 1*.* > nul
:pkxarc -r %1 filename.txt > nul
if errorlevel 1 goto ERR

echo *.hlp
pkxarc -r %1 *.hlp > nul
if errorlevel 1 goto ERR

echo *.bat
pkxarc -r %1 *.bat > nul
if errorlevel 1 goto ERR

echo *.pif
pkxarc -r %1 *.pif > nul
if errorlevel 1 goto ERR

echo *.ico
pkxarc -r %1 *.ico > nul
if errorlevel 1 goto ERR

echo *.rc
pkxarc -r %1 res *.rc > nul
if errorlevel 1 goto ERR

echo *.dlg
pkxarc -r %1 res *.dlg > nul
if errorlevel 1 goto ERR

echo *.bmp
pkxarc -r %1 *.bmp > nul
if errorlevel 1 goto ERR
goto DONE

:ERR
echo ERROR
goto DONE

:END
echo DONE

:DONE


