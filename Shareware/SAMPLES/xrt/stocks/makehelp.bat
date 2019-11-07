@echo off
REM -- First make map file from App Studio generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by STOCKS.HPJ. >hlp\stocks.hm
echo. >>hlp\stocks.hm
echo // Commands (ID_* and IDM_*) >>hlp\stocks.hm
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>hlp\stocks.hm
echo. >>hlp\stocks.hm
echo // Prompts (IDP_*) >>hlp\stocks.hm
makehm IDP_,HIDP_,0x30000 resource.h >>hlp\stocks.hm
echo. >>hlp\stocks.hm
echo // Resources (IDR_*) >>hlp\stocks.hm
makehm IDR_,HIDR_,0x20000 resource.h >>hlp\stocks.hm
echo. >>hlp\stocks.hm
echo // Dialogs (IDD_*) >>hlp\stocks.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\stocks.hm
echo. >>hlp\stocks.hm
echo // Frame Controls (IDW_*) >>hlp\stocks.hm
makehm IDW_,HIDW_,0x50000 resource.h >>hlp\stocks.hm
REM -- Make help for Project STOCKS
call hc31 stocks.hpj
echo.
