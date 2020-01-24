@echo on
if NOT xxx%1 == xxx goto :DOIT
echo usage: %0 [archive_name (minus ext)] 
goto ERR

:DOIT
: NOTE that DOS (disabled operating system) doesn't set the errorlevel
: for these commands, so error checking doesn't work

echo Creating \tmp\tmp...
mkdir \tmp\tmp
if exist %1.arc del %1.arc
if errorlevel 1 goto ERR

echo Copying files...
copy src\*.* \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy inc\*.* \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy docs\*.* \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy *.mak \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy *.lnk \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy *.bmp \tmp\tmp > nul
if errorlevel 1 goto ERR

:echo .
:copy 0* \tmp\tmp > nul
:copy 0*.* \tmp\tmp > nul
:copy 1*.* \tmp\tmp > nul
:if errorlevel 1 goto ERR

echo .
copy *.hlp \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy *.bat \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy *.ico \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy res\*.rc \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy res\*.dlg \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy *.def \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy *.cur \tmp\tmp > nul
if errorlevel 1 goto ERR

echo .
copy *.pif \tmp\tmp > nul
if errorlevel 1 goto ERR

echo Archiving files...
pkarc -a %1.arc \tmp\tmp\*.* > nul
if errorlevel 1 goto ERR

echo Deleting \tmp\tmp...
delnode /q \tmp\tmp > nul
echo Backup successful, unarchive using pkxarc.
goto END

:ERR
echo Backup failed...

:END
