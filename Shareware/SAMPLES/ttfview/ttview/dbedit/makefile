 #*****************************************************************************
 #
 #     Project:  DBEDIT 1.x
 #
 #      Module:  makefile
 #
 #   Author(s):  Charles E. Kindel, Jr. (cek)
 #
 #      System:  Microsoft Windows 3.x, MS-DOS 5.0, Compiled with Microsoft  
 #               C 6.00A and Microsoft MASM 5.1.                             
 #                                                                           
 #*****************************************************************************


MAINTARGET=DBEDIT.EXE

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
#   DBEDIT.EXE
#

!IFDEF DEBUG

LAS =   masm /Zi
LCC =   cl -c -DDEBUG -AM -G2sw -Owd -W3 -Zip -DDBEDIT
LLN =   link /NOD/NOE/A:16/CO
LRC1=   rc -r -DDEBUG -iSRC;RES
LRC2=   rc -v -t
LXX =   DEBUG

!ELSE

LAS =   masm
LCC =   cl -c -AM -G2sw -Owt -W3 -Zp -DDBEDIT
LLN =   link /NOD/NOE/A:16
LRC1=   rc -r -iSRC;RES
LRC2=   rc -v -t 
LXX =   RELEASE

!ENDIF

LZZ =   DBEDIT: **** $(LXX) *** $(LXX) *** $(LXX) *** $(LXX) ****

#
# Directories
#
MYRES = RES\ 
MYINC = SRC\ 
MYOBJ = OBJ\  
MYSRC = SRC\ 
MYHELP = HELP\ 

#
# Version stamping stuff
#
VERINCS = $(MYINC)VERSION.H $(MYRES)DBEDIT.RCV

#
# Resources
#

LRCINCS = $(VERINCS)\
          $(MYINC)isz.h\
          $(MYINC)dlgs.h\
          $(MYRES)DBEDIT.rc\
          $(MYRES)DBEDIT.dlg\
          $(MYRES)DBEDIT.str\
          $(MYRES)DBEDIT.ico

# 
# Library dependencies
# 
LMISC   =   makefile\
            DBEDIT.lnk\
            DBEDIT.def

LINCS   =   $(MYINC)DBEDIT.h $(MYINC)dlgs.h

LOBJS   =\
            $(MYOBJ)DBEDIT.obj\
            $(MYOBJ)config.obj\
            $(MYOBJ)init.obj\
            $(MYOBJ)botstat.obj\
            $(MYOBJ)wndmgr.obj\
            $(MYOBJ)menu.obj\
            $(MYOBJ)listbox.obj\
            $(MYOBJ)file.obj\
            $(MYOBJ)fileio.obj\
            $(MYOBJ)cmd.obj\
            $(MYOBJ)wintime.obj\
            $(MYOBJ)subdlgs.obj

#
#   BUILD RULES
#
#all : $(MAINTARGET) $(MYHELP)DBEDIT.hlp
all : $(MAINTARGET) 

#   DBEDIT.EXE build rules

$(MAINTARGET)  :   $(LOBJS) $(MYRES)DBEDIT.res DBEDIT.def $(MYOBJ)WINSTUB.EXE
                   $(LLN) @DBEDIT.lnk
                   $(LRC2) $(MYRES)DBEDIT.res DBEDIT.EXE
                   @echo. >$(OUTPUT)
                   @echo $(LZZ) >$(OUTPUT)
                   @echo. >$(OUTPUT)
                   @mapsym DBEDIT.map
                   @type $(MYINC)version.h

$(MYHELP)DBEDIT.HLP: $(MYHELP)DBEDIT.rtf $(MYHELP)map.h $(MYHELP)DBEDIT.hpj
   chdir HELP
   $(MAKE)
   copy DBEDIT.HLP ..
   chdir ..

$(MYOBJ)WINSTUB.EXE:  $(MYSRC)WINSTUB.ASM
    masm -I.\  $(MYSRC)winstub.asm,$(MYOBJ)WINSTUB.OBJ;
    link $(MYOBJ)WINSTUB.OBJ,$(MYOBJ)WINSTUB.EXE;

$(MYRES)DBEDIT.res:  $(LINCS) $(LRCINCS)
    @echo. >$(OUTPUT)
    @echo *** Building $(MYRES)DBEDIT.RES
    $(LRC1) $(MYRES)DBEDIT.rc $(MYRES)DBEDIT.RES

$(MYOBJ)DBEDIT.OBJ:  $(MYSRC)DBEDIT.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)CONFIG.OBJ:  $(MYSRC)CONFIG.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)INIT.OBJ:  $(MYSRC)INIT.C $(MYINC)INIT.H $(MYINC)ISZ.H $(LINCS) $(MYINC)ISZ.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)LISTBOX.OBJ:  $(MYSRC)LISTBOX.C $(MYINC)LISTBOX.H $(MYINC)ISZ.H $(LINCS) $(MYINC)ISZ.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)FILE.OBJ:  $(MYSRC)FILE.C $(MYINC)FILE.H $(MYINC)ISZ.H $(LINCS) $(MYINC)ISZ.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)FILEIO.OBJ:  $(MYSRC)FILEIO.C $(MYINC)FILEIO.H $(MYINC)ISZ.H $(LINCS) $(MYINC)ISZ.H $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)WINTIME.OBJ:  $(MYSRC)WINTIME.C $(MYINC)WINTIME.H $(MYINC)ISZ.H $(LINCS) $(MYINC)ISZ.H $(VERINCS)
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

$(MYOBJ)CMD.OBJ:  $(MYSRC)CMD.C $(MYINC)CMD.H $(LINCS) $(VERINCS) $(MYINC)MENU.H
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)MENU.OBJ:  $(MYSRC)MENU.C $(MYINC)MENU.H $(MYINC)ISZ.H $(LINCS) $(VERINCS) $(MYINC)MENU.H
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)SUBDLGS.OBJ:  $(MYSRC)SUBDLGS.C $(MYINC)SUBDLGS.H $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

#   MISC. STUFF

zip         :   clean
                pkzip -psr DBEDIT.zip *.*

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
                pkzip -prs DBEDIT.zip *.*

 #*****************************************************************************
 # End of 
 #*****************************************************************************
        
