@echo off
:NEXTCHECK
if NOT xxx%1 == xxx goto DOIT
echo usage: %0 [destination path_name (with *NO* trailing backslash)]
goto ERR

:DOIT
: NOTE that DOS (Doodoo On Silicon) doesn't set the errorlevel
: for these commands, so error checking doesn't work

: copy directory structure
md %1\src
if errorlevel 1 goto ERR
md %1\inc
if errorlevel 1 goto ERR
md %1\res
if errorlevel 1 goto ERR
md %1\doc
if errorlevel 1 goto ERR

echo Copying files...
copy src\*.c %1\src > nul
if errorlevel 1 goto ERR

echo .
copy src\*.asm %1\src > nul
if errorlevel 1 goto ERR

echo .
copy inc\*.h %1\inc > nul
if errorlevel 1 goto ERR

echo .
copy *.mak %1 > nul
if errorlevel 1 goto ERR

echo .
copy *.lnk %1 > nul
if errorlevel 1 goto ERR

echo .
copy *.hlp %1 > nul
if errorlevel 1 goto ERR

echo .
copy *.bat %1 > nul
if errorlevel 1 goto ERR

echo .
copy *.ico %1 > nul
if errorlevel 1 goto ERR

echo .
copy *.cur %1 > nul
if errorlevel 1 goto ERR

echo .
copy res\*.rc %1\res > nul
if errorlevel 1 goto ERR

echo .
copy res\*.dlg %1\res > nul
if errorlevel 1 goto ERR

echo .
copy *.def %1 > nul
if errorlevel 1 goto ERR

echo .
copy update.* %1 > nul
if errorlevel 1 goto ERR

goto END

:ERR
echo ERROR!

:END
echo DONE

