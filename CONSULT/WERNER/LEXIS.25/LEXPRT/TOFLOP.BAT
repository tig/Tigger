@echo off
if %1!==! goto Error
@echo Updating ZIP file on floppy with %2 %3 %4 %5 %6 %7 %8 %9 files
global del *.err *.map *.tmp *.bak *.~* *.asv
pkzip -oupr %1\winprt2.zip %2 %3 %4 %5 %6 %7 %8 %9


Goto End
:Error

@echo ToFlop USAGE: TOFLOP drive [spec]

Goto End

:End
