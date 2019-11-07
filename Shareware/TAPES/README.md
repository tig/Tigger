# TAPES - Cassette Tape J-Card Printer for DOS

_This is the DOS version of the first commercial software I wrote. The original version was written in UCSD p-System Pascal. The source for the original only exists on unreadable Apple ][ floppies and fading dot matrix print outs..._

(c) 1986-1990 Charles E. Kindel, Jr. 

Written by Charles E. Kindel 
 Kindlco Software Systems 
 4225 N. First Ave #1315 
 Tucson, AZ 85719 

Tapes is copyrighted by Charles E. Kindel, Jr. (1987,88,89,90). The Source code may is not be distributed, copied, or altered in any form. The executable code (TAPES.EXE) can be copied and distributed freely. This program may not be sold, rented, or offered as as an incitement to buy any other product.

## Revisions:

* 1/29/88 - V3.22
  * Made RETURN go to the beginning of next line, as apposed to going to same position in next line.Fixed bug that would prompt to clear and exit even if no data had been entered.
  * Fixed print routines so the printer gets "reset" to uncompressed and 1/6 inch line spacing. 
* 2/22/88 - V3.30\
  * Converted to Turbo Pascal 4.0
* 5/28/88 - V3.31 
  * Added code to detect 43 line EGA or 50 line VGA. TAPES now switches out of these modes into 25 line mode when it starts and goes back to original mode when it is done.
  * The cursor now changes size when Insert/Overwrite is toggled with the Insert key. Small cursor means insert mode is on, block cursor means overwrite is on. 
  * Made the prompts for Clear, and Quit open windows. 
* 7/18/88 - V3.32
  * Enhanced ability to detect screen mode.
* 10/5/88 - V3.33
  * Recompiled in Turbo Pascal 5.0 
* V3.34 - Fixed PgUp and PgDown so they worked more intuitively.
* 2/1/89 - V3.35 - 
  * Added option to print Tape Type and Noise Reduction
  * And added a menu window for print,clear,etc...
* 2/27/89 - V3.36
  * Now can save default configuration of noise reduction and tape type. Searches current dir, path, and dir where program was executed. Added option to change lpt port, user can now select between lpt1: lpt2:, com1:, and com2:. This can be saved to the .EXE file. Now supports multiple printers. 
* 5/15/89 - V3.37
  * Added tab key support. Tab key now inserts 5 spaces 
* 11/30/89 - V3.38
  * Fixed bug in clear where after clear the cursor would return to field 1. Fixed keyboard handling problem in Menu. Previously if the down arrrow was pressed in Menu Print would happen. Fixed copyright notices. 
* 4/27/90 - V3.39
  * Updated address and fixed color problems 
