clNORM = cl -Fo$@ -FPa -c -AM -W4 -Gw -Oax -Zpe -DINTERNAL -DWIN30 -NT 
OBJ=.\obj.int
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

wsdevelp.exe:	shopper.def\
		$(OBJ)\wsinit.obj\
		$(OBJ)\availdsk.obj \
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
		$(OBJ)\busy.obj \
		$(OBJ)\error.obj\
		$(OBJ)\help.obj\
		$(OBJ)\messages.obj \
		$(OBJ)\misc.obj \
		$(OBJ)\order.obj \
		$(OBJ)\settings.obj \
		$(OBJ)\viewbox.obj \
        $(OBJ)\custinfo.obj \
        $(OBJ)\pref.obj \
        $(OBJ)\info.obj \
        $(OBJ)\billing.obj \
        $(OBJ)\libmgr2.obj \
        $(OBJ)\libmgr.obj \
        $(OBJ)\subscr.obj \
        $(OBJ)\ksearch.obj \
		shopper.res
        link  @internal.lnk
        rc -fe WSintern.EXE shopper.res

$(OBJ)\dilute.obj:  $(SRC)\dilute.lib
   copy  $(SRC)\dilute.lib $(OBJ)\dilute.obj

$(OBJ)\lmemcpy.obj:  $(SRC)\lmemcpy.asm
   masm  $(SRC)\lmemcpy.asm,$(OBJ)\lmemcpy.obj,NUL,NUL

$(OBJ)\availdsk.obj:  $(SRC)\availdsk.asm
   masm $(SRC)\availdsk.asm,$(OBJ)\availdsk.obj,NUL,NUL

$(OBJ)\comm.obj:  $(SRC)\comm.c
   $(clNORM) _COMM $(SRC)\comm.c

$(OBJ)\subscr.obj:  $(SRC)\subscr.c
   $(clNORM) _SUBSCR $(SRC)\subscr.c

$(OBJ)\comm2.obj:  $(SRC)\comm2.c
   $(clNORM) _COMM $(SRC)\comm2.c

$(OBJ)\comm3.obj:  $(SRC)\comm3.c
   $(clNORM) _COMM $(SRC)\comm3.c

$(OBJ)\modem.obj:  $(SRC)\modem.c
   $(clNORM) _COMM $(SRC)\modem.c

$(OBJ)\xmodem.obj:  $(SRC)\xmodem.c
   $(clNORM) _COMM $(SRC)\xmodem.c

$(OBJ)\download.obj:  $(SRC)\download.c
   $(clNORM) _DOWNLOAD $(SRC)\download.c

$(OBJ)\error.obj:  $(SRC)\error.c
   $(clNORM) _ERROR $(SRC)\error.c

$(OBJ)\file.obj:  $(SRC)\file.c
    $(clNORM) _FILE  $(SRC)\file.c

$(OBJ)\file2.obj:  $(SRC)\file2.c
    $(clNORM) _FILE  $(SRC)\file2.c

$(OBJ)\file3.obj:  $(SRC)\file3.c
    $(clNORM) _FILE  $(SRC)\file3.c

$(OBJ)\help.obj:  $(SRC)\help.c
   $(clNORM) _HELP $(SRC)\help.c

$(OBJ)\messages.obj:  $(SRC)\messages.c
   $(clNORM) _WSRES $(SRC)\messages.c

$(OBJ)\misc.obj:  $(SRC)\misc.c
   $(clNORM) _MISC $(SRC)\misc.c

$(OBJ)\order.obj:  $(SRC)\order.c
   $(clNORM) _ORDER $(SRC)\order.c

$(OBJ)\settings.obj:  $(SRC)\settings.c
   $(clNORM) _SETTINGS $(SRC)\settings.c

$(OBJ)\viewbox.obj:  $(SRC)\viewbox.c
   $(clNORM) _VIEWBOX $(SRC)\viewbox.c

$(OBJ)\wsinit.obj: $(SRC)\wsinit.c   
    $(clNORM) _INIT  $(SRC)\wsinit.c 

$(OBJ)\wsres.obj: $(SRC)\wsres.c   $(INC)\ws.h
    $(clNORM) _RES  $(SRC)\wsres.c 

$(OBJ)\custinfo.obj: $(SRC)\custinfo.c   $(INC)\ws.h $(INC)\custinfo.h
    $(clNORM) _CUSTINFO  $(SRC)\custinfo.c 

$(OBJ)\pref.obj: $(SRC)\pref.c   $(INC)\ws.h $(INC)\custinfo.h $(INC)\pref.h
    $(clNORM) _CUSTINFO  $(SRC)\pref.c 

$(OBJ)\info.obj: $(SRC)\info.c   $(INC)\ws.h $(INC)\custinfo.h $(INC)\info.h
    $(clNORM) _INFO  $(SRC)\info.c 

$(OBJ)\billing.obj: $(SRC)\billing.c  $(INC)\ws.h $(INC)\custinfo.h $(INC)\billing.h
    $(clNORM) _BILLING  $(SRC)\billing.c 

$(OBJ)\libmgr.obj: $(SRC)\libmgr.c   
    $(clNORM) _LIBMGR  $(SRC)\libmgr.c 

$(OBJ)\libmgr2.obj: $(SRC)\libmgr2.c   
    $(clNORM) _LIBMGR  $(SRC)\libmgr2.c 

$(OBJ)\busy.obj:  $(SRC)\busy.c
   $(clNORM) _BUSY $(SRC)\busy.c

$(OBJ)\ksearch.obj:  $(SRC)\ksearch.c
   $(clNORM) _SEARCH $(SRC)\ksearch.c

shopper.res: $(RES)\shopper.rc  $(RES)\shopper.dlg $(INC)\rc_symb.h $(RES_FILES)
    rc -r -fo shopper.res -dWIN30 -dINTERNAL res\shopper.rc 

