@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by OLE2VIEW.HPJ. >"hlp\Ole2View.hm"
echo. >>"hlp\Ole2View.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\Ole2View.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\Ole2View.hm"
echo. >>"hlp\Ole2View.hm"
echo // Prompts (IDP_*) >>"hlp\Ole2View.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\Ole2View.hm"
echo. >>"hlp\Ole2View.hm"
echo // Resources (IDR_*) >>"hlp\Ole2View.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\Ole2View.hm"
echo. >>"hlp\Ole2View.hm"
echo // Dialogs (IDD_*) >>"hlp\Ole2View.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\Ole2View.hm"
echo. >>"hlp\Ole2View.hm"
echo // Frame Controls (IDW_*) >>"hlp\Ole2View.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\Ole2View.hm"
REM -- Make help for Project OLE2VIEW


echo Building Win32 Help files
start /wait hcrtf -x "hlp\Ole2View.hpj"
echo.
if exist Debug\nul copy "hlp\Ole2View.hlp" Debug
if exist Debug\nul copy "hlp\Ole2View.cnt" Debug
if exist Release\nul copy "hlp\Ole2View.hlp" Release
if exist Release\nul copy "hlp\Ole2View.cnt" Release
echo.


