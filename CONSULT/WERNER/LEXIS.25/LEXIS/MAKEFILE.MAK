PROJ	=MAKEFILE
DEBUG	=1
PROGTYPE	=1
CALLER	= 
ARGS	= 
DLLS	= 
CVPACK	=1
CC	=cl -qc
RC	=rc
CFLAGS_G_WEXE	= /AM /W3 /Ze /G2w /Zp 
CFLAGS_D_WEXE	= /Zi /Od /DDEBUG
CFLAGS_R_WEXE	= /Od /Gs /DNDEBUG
CFLAGS_G_WDLL	=/AS /G2w /Zp /Aw /W3
CFLAGS_D_WDLL	=/Od /Zi
CFLAGS_R_WDLL	=/O /Os /DNDEBUG
CFLAGS_G_WTTY	=/AS /G2w /W3
CFLAGS_D_WTTY	=/Od /Zi
CFLAGS_R_WTTY	=/O /Os /DNDEBUG
CFLAGS_G_DEXE	=/AS /W2
CFLAGS_D_DEXE	=/Od /Zi
CFLAGS_R_DEXE	=/O /Ot /DNDEBUG
CFLAGS	=$(CFLAGS_G_WEXE) $(CFLAGS_D_WEXE)
LFLAGS_G_WEXE	=/A:16/ST:5120 
LFLAGS_D_WEXE	=/CO 
LFLAGS_R_WEXE	= 
LFLAGS_G_WDLL	=/ST:5120 /A:16
LFLAGS_D_WDLL	=/CO
LFLAGS_R_WDLL	= 
LFLAGS_G_WTTY	=/ST:5120 /A:16
LFLAGS_D_WTTY	=/CO
LFLAGS_R_WTTY	= 
LFLAGS_G_DEXE	=/NOI /ST:2048
LFLAGS_D_DEXE	=/CO
LFLAGS_R_DEXE	= 
LFLAGS	=$(LFLAGS_G_WEXE) $(LFLAGS_D_WEXE)
RCFLAGS	=  
RESFLAGS	= /t 
RUNFLAGS	= 
H = 	inc\botstat.h inc\button.h inc\command.h inc\connect.h inc\direct.h inc\filedlgs.h \
	inc\global.h inc\hardware.h inc\help.h inc\init.h inc\keyboard.h inc\lexdlgd.h inc\lexis.h \
	inc\lexisd.h inc\lexprnt.h inc\mainwnd.h inc\menu.h inc\print.h inc\profile.h inc\state.h \
	inc\term.h inc\timer.h inc\topstat.h inc\undef.h inc\userpref.h 
RESFILES = 	res\connect.dlg res\direct.dlg res\hardware.dlg res\lexis.dlg res\modem.dlg \
	res\page.dlg res\phonenum.dlg res\userpref.dlg res\lexis.ico res\lexis.bmp res\stop1.bmp 
DEFFILE = 	lexis.def
OBJS_EXT = 	
LIBS_EXT = 	c:\windev30\lib\cmndll.lib c:\windev30\lib\wdb.lib lexprnt.lib 

.rc.res: ; $(RC) $(RCFLAGS) -r $*.rc

all:	$(PROJ).EXE

botstat.obj:	src\botstat.c $(H)

button.obj:	src\button.c $(H)

command.obj:	src\command.c $(H)

connect.obj:	src\connect.c $(H)

direct.obj:	src\direct.c $(H)

filedlgs.obj:	src\filedlgs.c $(H)

global.obj:	src\global.c $(H)

hardware.obj:	src\hardware.c $(H)

help.obj:	src\help.c $(H)

init.obj:	src\init.c $(H)

keyboard.obj:	src\keyboard.c $(H)

lexis.obj:	src\lexis.c $(H)

mainwnd.obj:	src\mainwnd.c $(H)

menu.obj:	src\menu.c $(H)

print.obj:	src\print.c $(H)

profile.obj:	src\profile.c $(H)

state.obj:	src\state.c $(H)

term.obj:	src\term.c $(H)

timer.obj:	src\timer.c $(H)

topstat.obj:	src\topstat.c $(H)

userpref.obj:	src\userpref.c $(H)

lexis.res:	res\lexis.rc $(RESFILES) $(H)

$(PROJ).EXE:	botstat.obj button.obj command.obj connect.obj direct.obj filedlgs.obj \
	global.obj hardware.obj help.obj init.obj keyboard.obj lexis.obj mainwnd.obj menu.obj \
	print.obj profile.obj state.obj term.obj timer.obj topstat.obj userpref.obj $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).crf
botstat.obj +
button.obj +
command.obj +
connect.obj +
direct.obj +
filedlgs.obj +
global.obj +
hardware.obj +
help.obj +
init.obj +
keyboard.obj +
lexis.obj +
mainwnd.obj +
menu.obj +
print.obj +
profile.obj +
state.obj +
term.obj +
timer.obj +
topstat.obj +
userpref.obj +
$(OBJS_EXT)
$(PROJ).EXE

d:\qcwin\lib\+
c:\WINDEV30\LIB\+
c:\c600\lib\+
d:\ddk300\lib\+
..\lib\+
c:\mdktools\lib\+
/NOD libw mlibcew+
C:\WINDEV30\LIB\CMNDLL.LIB+
C:\WINDEV30\LIB\WDB.LIB+
LEXPRNT.LIB
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).crf
	rc $(RESFLAGS) lexis.res $(PROJ).exe

$(PROJ).EXE:	lexis.res
	rc $(RESFLAGS) lexis.res $(PROJ).exe

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)

