#*****************************************************************************
#
#     Project:  DBEDIT 1.x
#
#      Module:  makefile
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

# if PUTEXE is defined the dll/exe will be copied from the root to
PUTEXE  = l:\BUILD\

# if putlib is defined the lib will be copied from the root to putlib
PUTLIB  = l:\LIB\

# if pubinc is define the include file will be copied there
PUTINC  = l:\INC\

#
#  Build tool macros.  These control how the tools build files.
#
#
#   DBEDIT.EXE
#

!IFDEF DEBUG

LAS =   masm /Zi
LCC =   cl -c -DDEBUG -AM -G2sw -Owd -W3 -Zip -DDBEDIT
LLN =   link /NOD/NOE/A:16/CO/M
LRC1=   rc -r -DDEBUG -iSRC;INC
LRC2=   rc -v -t
LXX =   DEBUG

!ELSE

LAS =   masm
LCC =   cl -c -AM -G2sw -Owt -W3 -Zp -DDBEDIT
LLN =   link /NOD/NOE/A:16/M
LRC1=   rc -r -iSRC;INC
LRC2=   rc -v -t 
LXX =   RELEASE

!ENDIF

LZZ =   DBEDIT: **** $(LXX) *** $(LXX) *** $(LXX) *** $(LXX) ****

#
# Directories
#
MYRES = SRC\ 
MYINC = SRC\ 
MYOBJ = OBJ\  
MYSRC = SRC\ 
MYHELP = HELP\ 

#
# Version stamping stuff
#
VERINCS = $(MYRES)DBEDIT.RCV

#
# Resources
#

LRCINCS = $(VERINCS)\
          $(MYINC)mainwnd.h\
          $(MYRES)DBEDIT.rc\
          $(MYRES)DBEDIT.ico

# 
# Library dependencies
# 
LMISC   =   makefile\
            DBEDIT.lnk\
            DBEDIT.def

LINCS   =   $(MYINC)GLOBAL.H  $(MYINC)VERSION.H

LOBJS   =\
            $(MYOBJ)dbedit.obj\
            $(MYOBJ)bitmap.obj\
            $(MYOBJ)config.obj\
            $(MYOBJ)fontview.obj\
            $(MYOBJ)init.obj\
            $(MYOBJ)mainwnd.obj\
            $(MYOBJ)pubinfo.obj\
            $(MYOBJ)pubsave.obj


#
#   BUILD RULES
#
#all : $(MAINTARGET) $(MYHELP)DBEDIT.hlp
all : $(MAINTARGET) 

#   DBEDIT.EXE build rules

$(MAINTARGET)  :   $(LOBJS) $(MYRES)DBEDIT.res DBEDIT.def
                   $(LLN) @DBEDIT.lnk
                   $(LRC2) $(MYRES)DBEDIT.res DBEDIT.exe
                   @echo. >$(OUTPUT)
                   @echo $(LZZ) >$(OUTPUT)
                   @echo. >$(OUTPUT)
                   @mapsym DBEDIT.map
                   @type $(MYINC)VERSION.H
                   copy dbedit.exe ..\BUILD

$(MYHELP)DBEDIT.HLP: help\DBEDIT.rtf help\map.h help\DBEDIT.hpj
   chdir HELP
   $(MAKE)
   copy DBEDIT.HLP ..
   chdir ..

$(MYRES)DBEDIT.res:  $(LINCS) $(LRCINCS)
    @echo. >$(OUTPUT)
    @echo *** Building $(MYRES)DBEDIT.RES
    $(LRC1) $(MYRES)DBEDIT.rc $(MYRES)DBEDIT.RES

$(MYOBJ)DBEDIT.OBJ:  $(MYSRC)DBEDIT.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)BITMAP.OBJ:  $(MYSRC)BITMAP.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)CONFIG.OBJ:  $(MYSRC)CONFIG.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)INIT.OBJ:  $(MYSRC)INIT.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)FONTVIEW.OBJ:  $(MYSRC)FONTVIEW.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)MAINWND.OBJ:  $(MYSRC)MAINWND.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)PUBINFO.OBJ:  $(MYSRC)PUBINFO.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)

$(MYOBJ)PUBSAVE.OBJ:  $(MYSRC)PUBSAVE.C $(LINCS) $(VERINCS)
    @echo. >$(OUTPUT)
    @echo *** $(LXX) *** Compiling $(@B).C to $@... >$(OUTPUT)
    $(LCC) -Fo$@ -NT$(@B) $(MYSRC)$(@B).C > $(MYSRC)$(@B).ERR
    @type $(MYSRC)$(@B).ERR > $(OUTPUT)


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

#*****************************************************************************
# End of 
#*****************************************************************************
        
