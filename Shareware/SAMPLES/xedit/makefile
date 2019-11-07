 #*****************************************************************************
 #
 #    Copyright (C) 1991 Charles E. Kindel, Jr.  All Rights Reserved.
 #
 #-----------------------------------------------------------------------
 #
 #     Project:  XEDIT 1.x
 #
 #      Module:  makefile
 #
 #   Author(s):  Charles E. Kindel, Jr. (cek)
 #
 #      System:  Microsoft Windows 3.x, MS-DOS 5.0, Compiled with Microsoft  
 #               C 6.00A and Microsoft MASM 5.1.                             
 #                                                                           
 #*****************************************************************************


MAINTARGET=XEDIT.EXE

# by commenting out the following line you can turn off silent mode
!CMDSWITCHES +s

# comment the following macros out if this is a RELEASE make.
!IFDEF DEBUG
   #DEBUG = TRUE
!ELSE
   #DEBUG = FALSE
!ENDIF


# set these macros to control where stuff is placed/redirected

!IFNDEF OUTPUT
OUTPUT  =   con
!ENDIF

# if putdll is defined the dll/exe will be copied from the root to
# PUTDLL
PUTDLL  =   TEST

# if putlib is defined the lib will be copied from the root to putlib
PUTLIB  =   OBJ

#
#  Build tool macros.  These control how the tools build files.
#
#
#   XEDIT.EXE
#

!IFDEF DEBUG

LAS =   masm /Zi
LCC =   cl -c -DDEBUG -AM -G2sw -Owd -W3 -Zip
LLN =   link /NOD/NOE/A:16/CO
LRC1=   rc -r -DDEBUG -iINC;RES
LRC2=   rc -30 -v -t
LXX =   DEBUG

!ELSE

LAS =   masm
LCC =   cl -c -AM -G2sw -Owt -W3 -Zp
LLN =   link /NOD/NOE/A:16
LRC1=   rc -r -iINC;RES
LRC2=   rc -30 -v -t 
LXX =   RELEASE

!ENDIF

LZZ =   XEDIT: **** $(LXX) *** $(LXX) *** $(LXX) *** $(LXX) ****

#
# Directories
#
MYRES = RES\ 
MYINC = INC\ 
MYOBJ = OBJ\  
MYSRC = SRC\ 
MYHELP = HELP\ 

#
# Version stamping stuff
#
VERINCS = $(MYINC)VERSION.H $(MYRES)XEDIT.RCV

#
# Resources
#

LRCINCS = $(VERINCS)\
          $(MYINC)isz.h\
          $(MYINC)dlgs.h\
          $(MYRES)XEDIT.rc\
          $(MYRES)XEDIT.dlg\
          $(MYRES)XEDIT.str\
          $(MYRES)XEDIT.ico\
          $(MYRES)bitmap.bmp\
          $(MYRES)order.frm

# 
# Library dependencies
# 
LMISC   =   makefile\
            XEDIT.lnk\
            XEDIT.def

LINCS   =   $(MYINC)XEDIT.h $(MYINC)dlgs.h

LOBJS   =\
            $(MYOBJ)XEDIT.obj\
            $(MYOBJ)config.obj\
            $(MYOBJ)about.obj\
            $(MYOBJ)init.obj\
            $(MYOBJ)dlghelp.obj\
            $(MYOBJ)botstat.obj\
            $(MYOBJ)wndmgr.obj\
            $(MYOBJ)font.obj\
            $(MYOBJ)status.obj\
            $(MYOBJ)menu.obj\
            $(MYOBJ)cmd.obj\
            $(MYOBJ)toolbar.obj\
            $(MYOBJ)subdlgs.obj\
            $(MYOBJ)toolpal.obj\
            $(MYOBJ)bmutil.obj\
            $(MYOBJ)wsscanf.obj

#
#   BUILD RULES
#
#all : $(MAINTARGET) $(MYHELP)XEDIT.hlp
all : $(MAINTARGET) 

#   XEDIT.EXE build rules

$(MAINTARGET)  :   $(LOBJS) $(MYRES)XEDIT.res XEDIT.def $(MYOBJ)WINSTUB.EXE
                   $(LLN) @XEDIT.lnk
                   $(LRC2) $(MYRES)XEDIT.res XEDIT.EXE
                   @echo. >$(OUTPUT)
                   @echo $(LZZ) >$(OUTPUT)
                   @echo. >$(OUTPUT)
                   @mapsym XEDIT.map
                   @type inc\version.h >$(OUTPUT)

$(MYHELP)XEDIT.HLP: help\XEDIT.rtf help\map.h help\XEDIT.hpj
   chdir HELP
   $(MAKE)
   copy XEDIT.HLP ..
   chdir ..

$(MYOBJ)WINSTUB.EXE:  $(MYSRC)WINSTUB.ASM
    masm -I.\  $(MYSRC)winstub.asm,$(MYOBJ)WINSTUB.OBJ;
    link $(MYOBJ)WINSTUB.OBJ,$(MYOBJ)WINSTUB.EXE;

$(MYRES)XEDIT.res:  $(LINCS) $(LRCINCS)
    @echo. >$(OUTPUT)
    @echo *** Building $(MYRES)XEDIT.RES
    $(LRC1) $(MYRES)XEDIT.rc $(MYRES)XEDIT.RES

$(MYOBJ)XEDIT.OBJ:  $(MYSRC)XEDIT.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)CONFIG.OBJ:  $(MYSRC)CONFIG.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)ABOUT.OBJ:  $(MYSRC)ABOUT.C $(MYINC)ABOUT.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)INIT.OBJ:  $(MYSRC)INIT.C $(MYINC)INIT.H $(LINCS) $(MYINC)ISZ.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)DLGHELP.OBJ:  $(MYSRC)DLGHELP.C $(MYINC)DLGHELP.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)WNDMGR.OBJ:  $(MYSRC)WNDMGR.C $(MYINC)WNDMGR.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)BOTSTAT.OBJ:  $(MYSRC)BOTSTAT.C $(MYINC)BOTSTAT.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)FONT.OBJ:  $(MYSRC)FONT.C $(MYINC)FONT.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)CMD.OBJ:  $(MYSRC)CMD.C $(MYINC)CMD.H $(LINCS) $(VERINCS) $(MYINC)MENU.H
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)MENU.OBJ:  $(MYSRC)MENU.C $(MYINC)MENU.H $(LINCS) $(VERINCS) $(MYINC)MENU.H
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)STATUS.OBJ:  $(MYSRC)STATUS.C $(MYINC)STATUS.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)TOOLBAR.OBJ:  $(MYSRC)TOOLBAR.C $(MYINC)TOOLBAR.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)SUBDLGS.OBJ:  $(MYSRC)SUBDLGS.C $(MYINC)SUBDLGS.H $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)WSSCANF.OBJ:  $(MYSRC)WSSCANF.C $(MYINC)WSSCANF.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)TOOLPAL.OBJ:  $(MYSRC)TOOLPAL.C $(MYINC)TOOLPAL.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)BMUTIL.OBJ:  $(MYSRC)BMUTIL.C $(MYINC)BMUTIL.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

#   MISC. STUFF

zip         :   clean
                pkzip -psr XEDIT.zip *.*

.ignore : 
clean       :  
                del $(MYOBJ)*.obj
                del *.map
                del $(MYRES)*.res
                del *.EXE
                del $(MYOBJ)*.lib
                del $(MYSRC)*.err
                del *.zip
                @echo *** DONE ***

ship        :   all
                !del $(MYOBJ)*.obj
                !del *.map
                !del $(MYRES)*.res
                !del $(MYSRC)*.err
                !del *.zip
                touch *.*
                pkzip -prs XEDIT.zip *.*

 #*****************************************************************************
 # End of 
 #*****************************************************************************
        
