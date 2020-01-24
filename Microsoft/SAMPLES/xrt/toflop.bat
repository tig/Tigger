@echo off
global del *.obj *.pch *.sbr *.bsc *.exe *.dll *.map
format a: /q /u /v:""
xcopy *.* a:\ /s
beep
beep
beep
beep
pause