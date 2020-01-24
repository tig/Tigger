@echo off

rem goto Begin

if "!%1"=="!" goto ErrMsg

goto Begin

:ErrMsg
@echo.
@echo This batch file will archive the Font Connection source tree to four
@echo floppy diskettes.  Use the fromflop.bat batch file to reconstruct
@echo the source tree.
@echo.
@echo Usage:   TOFLOP path options
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
REM ************   Build Disk # 1     *******************************
REM *****************************************************************

:Disk1

if "%2"=="/X:1" goto Disk2
if "%3"=="/X:1" goto Disk2
if "%4"=="/X:1" goto Disk2
if "%2"=="/x:1" goto Disk2
if "%3"=="/x:1" goto Disk2
if "%4"=="/x:1" goto Disk2

BEEP 2000
@echo.
@echo Place Font Connection source archive diskette #1 in drive %1
@echo.
pause

copy fromflop.bat %1

del disk1.zip 
pkzip -P -xORDER.LOG disk1.zip BUILD\*.*
pkzip -Pr -x*.pch -x*.obj -x*.res -x*.err -x*.map -x*.sym -x*.lib -x*.dll -x*.exe disk1.zip @disk1.lst 
if ERRORLEVEL GT 0 goto Error

del %1disk1.zip
copy disk1.zip %1
if NOT exist %1disk1.zip goto SizeError

if "%2"=="/I:1" goto Ok
if "%3"=="/I:1" goto Ok
if "%4"=="/I:1" goto Ok
if "%2"=="/i:1" goto Ok
if "%3"=="/i:1" goto Ok
if "%4"=="/i:1" goto Ok


REM *****************************************************************
REM ************   Build Disk # 2     *******************************
REM *****************************************************************

:Disk2

if "%2"=="/X:2" goto Disk3
if "%3"=="/X:2" goto Disk3
if "%4"=="/X:2" goto Disk3
if "%2"=="/x:2" goto Disk3
if "%3"=="/x:2" goto Disk3
if "%4"=="/x:2" goto Disk3

BEEP 2000
@echo.
@echo Place Font Connection source archive diskette #2 in drive %1
@echo.
pause

del disk2.zip
pkzip -Pr -x*.pch -x*.obj -x*.res -x*.err -x*.map -x*.sym disk2.zip @disk2.lst 

if ERRORLEVEL GT 0 goto Error

del %1disk2.zip
copy disk2.zip %1
if NOT exist %1disk2.zip goto SizeError

if "%2"=="/I:2" goto Ok
if "%3"=="/I:2" goto Ok
if "%4"=="/I:2" goto Ok
if "%2"=="/i:2" goto Ok
if "%3"=="/i:2" goto Ok
if "%4"=="/i:2" goto Ok


REM *****************************************************************
REM ************   Build Disk #3     *******************************
REM *****************************************************************

:disk3

if "%2"=="/X:3" goto Ok
if "%3"=="/X:3" goto Ok
if "%4"=="/X:3" goto Ok
if "%2"=="/x:3" goto Ok
if "%3"=="/x:3" goto Ok
if "%4"=="/x:3" goto Ok

BEEP 2000
@echo.
@echo Place Font Connection archive diskette #3 (database) in drive %1
@echo.
pause

del disk3.zip 
pkzip -xFSPUB*.* -Pr disk3.zip BUILD\DATABASE\FS*.*

if ERRORLEVEL GT 0 goto Error

del %1disk3.zip
copy disk3.zip %1
if NOT exist %1disk3.zip goto SizeError

REM *****************************************************************
REM ************   Build Disk #4     *******************************
REM *****************************************************************

:disk4

if "%2"=="/X:4" goto Ok
if "%3"=="/X:4" goto Ok
if "%4"=="/X:4" goto Ok
if "%2"=="/x:4" goto Ok
if "%3"=="/x:4" goto Ok
if "%4"=="/x:4" goto Ok

BEEP 2000
@echo.
@echo Place Font Connection archive diskette #4 (database) in drive %1
@echo.
pause

del disk4.zip 
pkzip -Pr disk4.zip BUILD\DATABASE\FSPUB*.* BUILD\DATABASE\FF*.* BUILD\DATABASE\WHATSNEW.*

if ERRORLEVEL GT 0 goto Error

del %1disk4.zip
copy disk4.zip %1
if NOT exist %1disk4.zip goto SizeError

goto Ok

:SizeError
beep 150 5
beep 100 5
@echo ERROR:  Disk set not complete (File too big??) !!!!

goto TheEnd

:Error
beep 150 5
beep 100 5
@echo ERROR:  Disk set not complete!!!!

Goto TheEnd

:Ok
BEEP 600
BEEP 700
BEEP 800
BEEP 900
BEEP 1000
@echo.
@echo Disk set created successfully.


:TheEnd

