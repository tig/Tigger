clNORM = cl -Fo$@ -FPa -c -AM -W2 -Gw -Oax -Zpe -DINTERNAL -NT 
clCVIEW = cl -Fo$@ -FPa -c -AM -W3 -Gsw -Od -Zpie -DWIN30 -DINTERNAL -DDEBUG -NT 
OBJ=.\obj.dbg
SRC=.\src
RES=.\res
INC=.\inc
HELP_FILES = ADONIS.HLP BILLINFO.HLP CHANGEPW.HLP COMM.HLP CUSTINFO.HLP \
             DEMO.HLP DLCLIP.HLP DLDIR.HLP LIBDRIVE.HLP LIBMGR.HLP MAIN.HLP \
             ORDERLIB.HLP ORDERWS.HLP SELBILL.HLP SHIPINFO.HLP UPDATEDB.HLP \
             VERIFYPW.HLP VIEW.HLP WELCOME.HLP NEWUSER.HLP SEARCH.HLP DECISION.HLP
ICON_FILES = connect.ico pencil.ico thumb30.ico db30.ico calc.ico credit.ico \
             bag30.ico
BMP_FILES  = wsbag.bmp db.bmp logoovl1.bmp logoovl2.bmp logoovl3.bmp
CURS_FILES = phone301.cur phone302.cur phone303.cur phone304.cur
RES_FILES  = $(HELP_FILES) $(ICON_FILES) $(BMP_FILES) $(CURS_FILES)

WSDEBUG.exe:	shopper.def\
		$(OBJ)\availdsk.obj\
		$(OBJ)\wsinit.obj\
		$(OBJ)\wsres.obj \
		$(OBJ)\dilute.obj \
		$(OBJ)\lmemcpy.obj \
		$(OBJ)\comm.obj \
		$(OBJ)\comm2.obj \
		$(OBJ)\comm3.obj \
		$(OBJ)\modem.obj \
		$(OBJ)\xmodem.obj \
		$(OBJ)\download.obj \
		$(OBJ)\file.obj \
		$(OBJ)\file2.obj \
		$(OBJ)\file3.obj \
		$(OBJ)\error.obj\
		$(OBJ)\help.obj\
		$(OBJ)\messages.obj \
		$(OBJ)\misc.obj \
		$(OBJ)\order.obj \
		$(OBJ)\settings.obj \
		$(OBJ)\viewbox.obj \
		$(OBJ)\busy.obj \
        $(OBJ)\custinfo.obj \
        $(OBJ)\pref.obj \
        $(OBJ)\info.obj \
        $(OBJ)\billing.obj \
        $(OBJ)\libmgr.obj \
        $(OBJ)\libmgr2.obj \
        $(OBJ)\subscr.obj \
        $(OBJ)\ksearch.obj \
		shopper.res
#		ilink /a /v /i WSDEBUG.exe
        link  @debug.lnk
        rc -fe wsDEBUG.EXE shopper.res
#        mapsym shopper

$(OBJ)\dilute.obj:  $(SRC)\dilute.lib
   copy $(SRC)\dilute.lib $(OBJ)\dilute.obj

$(OBJ)\lmemcpy.obj:  $(SRC)\lmemcpy.asm
   masm $(SRC)\lmemcpy.asm,$(OBJ)\lmemcpy.obj,NUL,NUL

$(OBJ)\availdsk.obj:  $(SRC)\availdsk.asm
   masm $(SRC)\availdsk.asm,$(OBJ)\availdsk.obj,NUL,NUL

$(OBJ)\subscr.obj:  $(SRC)\subscr.c
   $(clCVIEW) _SUBSCR $(SRC)\subscr.c

$(OBJ)\comm.obj:  $(SRC)\comm.c
   $(clCVIEW) _COMM $(SRC)\comm.c

$(OBJ)\comm2.obj:  $(SRC)\comm2.c
   $(clCVIEW) _COMM $(SRC)\comm2.c

$(OBJ)\comm3.obj:  $(SRC)\comm3.c
   $(clCVIEW) _COMM $(SRC)\comm3.c

$(OBJ)\modem.obj:  $(SRC)\modem.c
   $(clCVIEW) _COMM $(SRC)\modem.c

$(OBJ)\xmodem.obj:  $(SRC)\xmodem.c
   $(clCVIEW) _DOWNLOAD $(SRC)\xmodem.c

$(OBJ)\download.obj:  $(SRC)\download.c
   $(clCVIEW) _DOWNLOAD $(SRC)\download.c

$(OBJ)\error.obj:  $(SRC)\error.c
   $(clCVIEW) _ERROR $(SRC)\error.c

$(OBJ)\file.obj:  $(SRC)\file.c
    $(clCVIEW) _FILE  $(SRC)\file.c

$(OBJ)\file2.obj:  $(SRC)\file2.c
    $(clCVIEW) _FILE  $(SRC)\file2.c

$(OBJ)\file3.obj:  $(SRC)\file3.c
    $(clCVIEW) _FILE  $(SRC)\file3.c

$(OBJ)\help.obj:  $(SRC)\help.c
   $(clCVIEW) _HELP $(SRC)\help.c

$(OBJ)\messages.obj:  $(SRC)\messages.c
   $(clCVIEW) _WSRES $(SRC)\messages.c

$(OBJ)\misc.obj:  $(SRC)\misc.c
   $(clCVIEW) _MISC $(SRC)\misc.c

$(OBJ)\order.obj:  $(SRC)\order.c
   $(clCVIEW) _ORDER $(SRC)\order.c

$(OBJ)\settings.obj:  $(SRC)\settings.c
   $(clCVIEW) _SETTINGS $(SRC)\settings.c

$(OBJ)\viewbox.obj:  $(SRC)\viewbox.c
   $(clCVIEW) _VIEWBOX $(SRC)\viewbox.c

$(OBJ)\wsinit.obj: $(SRC)\wsinit.c   
    $(clCVIEW) _INIT  $(SRC)\wsinit.c 

$(OBJ)\wsres.obj: $(SRC)\wsres.c   $(INC)\ws.h
    $(clCVIEW) _RES  $(SRC)\wsres.c 

$(OBJ)\custinfo.obj: $(SRC)\custinfo.c   $(INC)\ws.h $(INC)\custinfo.h
    $(clCVIEW) _CUSTINFO  $(SRC)\custinfo.c 

$(OBJ)\pref.obj: $(SRC)\pref.c   $(INC)\ws.h $(INC)\custinfo.h $(INC)\pref.h
    $(clCVIEW) _PREF  $(SRC)\pref.c 

$(OBJ)\info.obj: $(SRC)\info.c   $(INC)\ws.h $(INC)\custinfo.h $(INC)\info.h
    $(clCVIEW) _INFO  $(SRC)\info.c 

$(OBJ)\billing.obj: $(SRC)\billing.c  $(INC)\ws.h $(INC)\custinfo.h $(INC)\billing.h
    $(clCVIEW) _BILLING  $(SRC)\billing.c 

$(OBJ)\libmgr.obj: $(SRC)\libmgr.c   
    $(clCVIEW) _LIBMGR  $(SRC)\libmgr.c 

$(OBJ)\libmgr2.obj: $(SRC)\libmgr2.c   
    $(clCVIEW) _LIBMGR  $(SRC)\libmgr2.c 

$(OBJ)\busy.obj:  $(SRC)\busy.c
   $(clCVIEW) _BUSY $(SRC)\busy.c

$(OBJ)\ksearch.obj:  $(SRC)\ksearch.c
   $(clCVIEW) _SEARCH $(SRC)\ksearch.c

shopper.res: $(RES)\shopper.rc  $(RES)\shopper.dlg $(INC)\rc_symb.h $(RES_FILES)
    rc -r -fo shopper.res -dWIN30 -dINTERNAL -dDEBUG res\shopper.rc 

