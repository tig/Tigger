#
#   Makefile for Generic
#

CC     =   cl -c -AM -Gsw -nologo -Od -W3 -Zipe
LN     =   link /m/CO/NOD/NOE/NOL/A:16
SRC    =   main.c init.c
OBJ    =   main.obj init.obj

.c.obj      :
				echo Compiling: $*.c
				$(CC) -NT $* $*.c >$*.err
				type $*.err >aux
				del $*.err

main.exe    :   $(OBJ) main.res main.def
				echo Linking: main.exe
				$(LN) $(OBJ), main.exe, tm.map, hlsqstr.lib mlibcew libw, main.def
				echo Adding Resources
				rc -v main.res main.exe

main.res    :   main.rc global.h main.ico
				echo RCing: main.rc
				rc -r main.rc

clean       :
				echo on
				del *.obj
				del *.err
				del *.res
				del *.map
				del *.sym
				del *.arc
				del *.~??
				echo off

bak         :
				copy makefile Backup\makefile
				copy *.c   Backup\*.*
				copy *.h   Backup\*.*
				copy *.rc  Backup\*.*
				copy *.def Backup\*.*
				copy *.res Backup\*.*
				copy *.ico Backup\*.*
				copy *.wv? Backup\*.*
				copy *.exe Backup\*.*
