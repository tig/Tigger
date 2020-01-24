# Makefile for the ZMODEM DLL - ZMODEM protocol support
# Written by Bryan A. Woodruff

# inference rules

#DEBUG=TRUE

.asm.obj:
        masm -Zi $*.asm ;

.c.obj:
        cl $(CFLAGS) $(PCHFLAGS) -NT $* $*.c >$*.err
        type $*.err

.rc.res:
        rc $(RCFLAGS) $*.rc

!IFDEF DEBUG
CFLAGS = -f -c -AMw -Owd -G2 -Gsw -W3 -Zipe -nologo -DDEBUG
LFLAGS = /CO /NOD /align:16 /m
PCHFLAGS = -YuPRECOMP.H
!ELSE
CFLAGS = -c -AMw -Owt -G2 -Gsw -W3 -Zpe -nologo
LFLAGS = /NOD /align:16 /m
PCHFLAGS = -YuPRECOMP.H
!ENDIF

RCFLAGS = -r

INCFILES =     precomp.pch zmodem.h izmodem.h version.h

OBJECTS =      precomp.obj libentry.obj libmain.obj zmodem.obj

all:           zmodem.dll
               copy zmodem.dll ..\build
               copy zmodem.sym ..\build
               copy zmodem.dll tty
               copy zmodem.h ..\inc
               copy zmodem.h tty
               copy zmodem.lib ..\lib
               copy zmodem.lib tty

.ignore : 
clean:
               !del *.obj
               !del *.map
               !del *.sym
               !del *.err
               !del *.dll
               !del zmodem.lib

version:       clean
               bumpver version.h

#----------------------------------------------------------------------------
#  PreComp.C is used to create the precompiled information
#----------------------------------------------------------------------------

precomp.obj precomp.pch:   precomp.h
               cl $(CFLAGS) -YcPRECOMP.H -NT precomp precomp.c

libentry.obj:  libentry.asm

libmain.obj:   libmain.c $(INCFILES)

zmodem.obj:    zmodem.c $(INCFILES)

zmodem.res:    zmodem.rc zmodem.dlg res.h version.h

zmodem.dll:    $(OBJECTS) zmodem.res zmodem.def
               link $(LFLAGS) @<<
$(OBJECTS),
zmodem.dll,
zmodem.map,
conxlib wdb libw mdllcew,
zmodem.def
<<
               rc -v zmodem.res zmodem.dll
               mapsym zmodem
               implib ZMODEM.LIB ZMODEM.DEF



