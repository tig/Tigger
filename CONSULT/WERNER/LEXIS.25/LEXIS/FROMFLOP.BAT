@echo off
if %1!==! goto Error

touch update.bat
@echo copying from floppy (ZIP)
pkunzip -n -d -srenrew %1\lexis.zip %2 %3 %4 %5 %6 %7 %8 %9


Goto End
:Error

@echo FromFlop USAGE: FROMFLOP drive [specs]

Goto End

:End
