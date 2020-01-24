@echo off

if "%1"=="/?" goto Help
if "%1"=="/help" goto Help
if "%1"=="/HELP" goto Help

SET DEBUG_SAVE=%DEBUG%
SET DEBUG=

if "%1"=="/DEBUG" SET DEBUG=TRUE
if "%2"=="/DEBUG" SET DEBUG=TRUE
if "%3"=="/DEBUG" SET DEBUG=TRUE
if "%4"=="/DEBUG" SET DEBUG=TRUE
if "%1"=="/debug" SET DEBUG=TRUE
if "%2"=="/debug" SET DEBUG=TRUE
if "%3"=="/debug" SET DEBUG=TRUE
if "%4"=="/debug" SET DEBUG=TRUE

@echo This batch file will build The Font Connection.  It will
@echo first delete the current version and all temporary files, and
@echo then it will build each component. 

if "%DEBUG%" == "TRUE" goto WeDoDebug
@echo.
@echo You have choosen to build a NON-DEBUG build.  Press CTRL-C to cancel or
pause

goto DoIt

:WeDoDebug
@echo.
@echo You have choosen to build a DEBUG build.  Press CTRL-C to cancel or
pause

goto DoIt


:DoIt

chdir build
del FCDB.*
del CONXMEM.*
del CONXLIB.*
del CONXPRO.*
del FONTCON.*
del CONXSRV.*
del CONXUP.*
chdir ..

chdir lib
del FCDB.lib
del CONXMEM.lib
del CONXLIB.lib
del CONXPRO.lib
chdir ..

chdir inc
del FCDB.h
del CONXMEM.h
del CONXLIB.h
del CONXPRO.h
del STRUCT.h
chdir ..

:BuildEm

chdir CONXMEM
mkdir obj
nmake clean
nmake 
if ERRORLEVEL GT 0 goto Error
chdir ..

chdir CONXLIB
mkdir obj
nmake clean
nmake
if ERRORLEVEL GT 0 goto Error
chdir ..

chdir FCDB
mkdir obj
nmake clean
nmake
if ERRORLEVEL GT 0 goto Error
chdir ..

chdir CONXPRO
mkdir obj
nmake clean
nmake
if ERRORLEVEL GT 0 goto Error
chdir ..

chdir CONXSRV
mkdir obj
nmake clean
nmake
if ERRORLEVEL GT 0 goto Error
chdir ..

chdir FONTCON
mkdir obj
nmake clean
nmake
if ERRORLEVEL GT 0 goto Error
chdir ..

chdir CONXUP
mkdir obj
nmake clean
nmake
if ERRORLEVEL GT 0 goto Error
chdir ..

if "%1"=="disks" goto BuildDisks
if "%2"=="disks" goto BuildDisks
if "%3"=="disks" goto BuildDisks
if "%4"=="disks" goto BuildDisks

Goto Success

:BuildDisks

copy build\*.* setup\layout\
chdir setup
go *
chdir ..

Goto Success

:Help
@echo.
@echo USAGE:  MAKEALL options
@echo         Where options can be:
@echo           /DEBUG - Build a debug build.
@echo           disks - Makes setup disk images
@echo.
goto TheEnd

:Error
beep 150 5
beep 100 5
@echo.
@echo An Error occurred!  Build aborted
@echo.
Goto TheEnd

:Success

BEEP 600
BEEP 700
BEEP 800
BEEP 900
BEEP 1000

dir BUILD\*.exe BUILD\*.dll BUILD\*.mod

if "X%DEBUG%" NE "X" Goto DidDebug

@echo *********************************************************************
@echo ***** NON-DEBUG ***** NON-DEBUG ***** NON-DEBUG ***** NON-DEBUG *****
@echo *********************************************************************

goto TheEnd

:DidDebug
@echo *****************************************************
@echo ***** DEBUG ***** DEBUG ***** DEBUG ***** DEBUG *****
@echo *****************************************************

:TheEnd

SET DEBUG=%DEBUG_SAVE%
SET DEBUG_SAVE=
