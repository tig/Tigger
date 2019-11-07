## WinGrep!
#
#  
#
## Grog! #


cp     =   cl -c -AS -Gsw -Zipe

.c.obj :
           $(cp) $*.c

wg.exe     :   wg.obj fgrep.obj wg.res wg.def
               link /NOD/NOE/CO wg fgrep,/align:16,,slibcew libw,wg.def
               rc wg.res

wg.obj     :   wg.c wg.h

fgrep.obj  :   fgrep.c fgrep.h

wg.res     :   wg.rc wg.h wg.dlg
               rc -r wg.rc

