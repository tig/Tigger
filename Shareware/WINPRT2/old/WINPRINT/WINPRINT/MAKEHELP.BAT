@echo off
REM -- First make map file from App Studio generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by WINPRINT.HPJ. >hlp\winprint.hm
echo. >>hlp\winprint.hm
echo // Commands (ID_* and IDM_*) >>hlp\winprint.hm
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>hlp\winprint.hm
echo. >>hlp\winprint.hm
echo // Prompts (IDP_*) >>hlp\winprint.hm
makehm IDP_,HIDP_,0x30000 resource.h >>hlp\winprint.hm
echo. >>hlp\winprint.hm
echo // Resources (IDR_*) >>hlp\winprint.hm
makehm IDR_,HIDR_,0x20000 resource.h >>hlp\winprint.hm
echo. >>hlp\winprint.hm
echo // Dialogs (IDD_*) >>hlp\winprint.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\winprint.hm
echo. >>hlp\winprint.hm
echo // Frame Controls (IDW_*) >>hlp\winprint.hm
makehm IDW_,HIDW_,0x50000 resource.h >>hlp\winprint.hm
REM -- Make help for Project WINPRINT
call hc31 winprint.hpj
echo.
