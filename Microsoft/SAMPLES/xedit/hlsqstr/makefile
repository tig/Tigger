# makefile for hlsqstr.dll

all: hlsqstr.dll

CC=cl -AMw -c -Gsw -Od -W3 -Zpi 
LN=link /ALIGN:16/NOD/NOE/CO/MAP

SRC    =   hlsqstr.c   heap.c   list.c   stack.c   queue.c
OBJ    =   hlsqstr.obj heap.obj list.obj stack.obj queue.obj

.asm.obj:
    masm $<;

.c.obj:
	echo Compiling: $*.c
	$(CC) -NT $* $*.c >$*.err
	type $*.err >aux
	del $*.err

hlsqstr.dll: $(OBJ) hlsqstr.def
    $(LN) $(OBJ) libentry, hlsqstr.dll,, mdllcew libw, hlsqstr.def
    rc -v -30 hlsqstr.dll
    implib hlsqstr.lib hlsqstr.def
#    copy hlsqstr.dll c:\win30
#    copy hlsqstr.dll c:\win31
#    copy hlsqstr.lib c:\windev30\lib
#    copy hlsqstr.h   c:\windev30\include
