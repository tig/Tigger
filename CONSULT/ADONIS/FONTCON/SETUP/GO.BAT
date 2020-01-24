@echo off

@echo.
if "%1"=="12"  goto DoIt12
if "%1"=="720" goto DoIt720
if "%1"=="144" goto DoIt144
if "%1"=="*" goto DoIt144

@echo Usage: 
@echo        GO 720             - Makes 720K disk images
@echo        GO 12              - Makes 1.2MB disk images
@echo        GO *               - Makes 720K and 1.2MB images (not 1.44MB)
@echo        GO 144             - Makes 144MB disk images

Goto TheEnd 

:DoIt720
@echo Building 720K Disk Images in the 720K sub-directory...
@echo.
dsklayt2 layout\fontcon.lyt layout\fontcon.inf /k720 /f /d720K
if "%1"=="720" goto TheEnd

:DoIt12
@echo Building 1.2MB Disk Images in the 12MB sub-directory...
@echo.
dsklayt2 layout\fontcon.lyt layout\fontcon.inf /k12 /f /d12MB
goto TheEnd

:DoIt144
@echo Building 1.44MB Disk Images in the 144 sub-directory...
@echo.
dsklayt2 layout\fontcon.lyt layout\fontcon.inf /k144 /f /d144MB

:TheEnd
