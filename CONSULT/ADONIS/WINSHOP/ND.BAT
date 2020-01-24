@echo off
echo ** Windows 3.0 version of nd.bat ***
call setpath 30 adsf
echo Copying rc_symb.h to tmp.h...
copy inc\rc_symb.h tmp.h
echo Edit tmp.h (rc_symb.h), delete all except #define's...
pause
c:\bin\brief\b inc\tmp.h
win dialog shopper.res
rc -r -fo shopper.res -DWIN30 res\shopper.rc
goto done

:error
echo rc failed.

:done
