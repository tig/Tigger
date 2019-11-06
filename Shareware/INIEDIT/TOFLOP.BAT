@echo off
if %1!==! goto Error
@echo Updating ZIP file on floppy with %2 %3 %4 %5 %6 %7 %8 %9 files
pkzip -ouprszzyzx %1\iniedit.zip %2 %3 %4 %5 %6 %7 %8 %9
pkunzip -t -szzyzx %1\iniedit.zip


Goto End
:Error

@echo ToFlop USAGE: TOFLOP drive [spec]

Goto End

:End
