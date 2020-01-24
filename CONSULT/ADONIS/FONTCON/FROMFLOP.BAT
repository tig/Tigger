@echo off

if "!%1"=="!" goto ErrMsg

goto Begin

:ErrMsg
@echo.
@echo This batch file construct the Font Connection source tree from the four
@echo floppy disks that were created by the toflop.bat batch file.
@echo.
@echo Usage:   FROMFLOP sourcepath options
@echo.
@echo          Options are:
@echo.            /I:n - process disk #n only
@echo.            /X:n - exclude disk #n
goto TheEnd

:Begin
if "%2"=="/I:2" goto Disk2
if "%3"=="/I:2" goto Disk2
if "%4"=="/I:2" goto Disk2
if "%2"=="/i:2" goto Disk2
if "%3"=="/i:2" goto Disk2
if "%4"=="/i:2" goto Disk2

if "%2"=="/I:3" goto Disk3
if "%3"=="/I:3" goto Disk3
if "%4"=="/I:3" goto Disk3
if "%2"=="/i:3" goto Disk3
if "%3"=="/i:3" goto Disk3
if "%4"=="/i:3" goto Disk3

if "%2"=="/I:4" goto Disk4
if "%3"=="/I:4" goto Disk4
if "%4"=="/I:4" goto Disk4
if "%2"=="/i:4" goto Disk4
if "%3"=="/i:4" goto Disk4
if "%4"=="/i:4" goto Disk4

REM *****************************************************************
REM ************   Extract Disk #1     ******************************
REM *****************************************************************

:Disk1

if "%2"=="/X:1" goto Disk2
if "%3"=="/X:1" goto Disk2
if "%4"=="/X:1" goto Disk2
if "%2"=="/x:1" goto Disk2
if "%3"=="/x:1" goto Disk2
if "%4"=="/x:1" goto Disk2

BEEP 2000
@echo Place Font Connection source archive diskette #1 in drive %1
@echo.
pause

copy %1disk1.zip .
pkunzip -d -o -n disk1.zip
if ERRORLEVEL GT 0 goto Error

if "%2"=="/I:1" goto Ok
if "%3"=="/I:1" goto Ok
if "%4"=="/I:1" goto Ok
if "%2"=="/i:1" goto Ok
if "%3"=="/i:1" goto Ok
if "%4"=="/i:1" goto Ok

REM *****************************************************************
REM ************   Extract Disk #2     ******************************
REM *****************************************************************

:disk2

if "%2"=="/X:2" goto Disk3
if "%3"=="/X:2" goto Disk3
if "%4"=="/X:2" goto Disk3
if "%2"=="/x:2" goto Disk3
if "%3"=="/x:2" goto Disk3
if "%4"=="/x:2" goto Disk3

BEEP 2000
@echo Place Font Connection source archive diskette #2 in drive %1
@echo.
pause

copy %1disk2.zip .
pkunzip -d -o -n disk2.zip
if ERRORLEVEL GT 0 goto Error

if "%2"=="/I:2" goto Ok
if "%3"=="/I:2" goto Ok
if "%4"=="/I:2" goto Ok
if "%2"=="/i:2" goto Ok
if "%3"=="/i:2" goto Ok
if "%4"=="/i:2" goto Ok

REM *****************************************************************
REM ************   Extract Disk #3     ******************************
REM *****************************************************************

:disk3

if "%2"=="/X:3" goto Ok
if "%3"=="/X:3" goto Ok
if "%4"=="/X:3" goto Ok
if "%2"=="/x:3" goto Ok
if "%3"=="/x:3" goto Ok
if "%4"=="/x:3" goto Ok

BEEP 2000
@echo Place Font Connection source archive diskette #3 in drive %1
@echo.
pause

copy %1disk3.zip .
pkunzip -d -o -n disk3.zip
if ERRORLEVEL GT 0 goto Error

REM *****************************************************************
REM ************   Extract Disk #4     ******************************
REM *****************************************************************

:disk4

if "%2"=="/X:4" goto Ok
if "%3"=="/X:4" goto Ok
if "%4"=="/X:4" goto Ok
if "%2"=="/x:4" goto Ok
if "%3"=="/x:4" goto Ok
if "%4"=="/x:4" goto Ok

BEEP 2000
@echo Place Font Connection source archive diskette #4 in drive %1
@echo.
pause

copy %1disk4.zip .
pkunzip -d -o -n disk4.zip
if ERRORLEVEL GT 0 goto Error

goto Ok

:Error
beep 150 5
beep 100 5
@echo ERROR:  Source Tree not completely constructed!!!

Goto TheEnd

:Ok
BEEP 600
BEEP 700
BEEP 800
BEEP 900
BEEP 1000
@echo.
@echo Files extracted successfully.

:TheEnd
