 #*****************************************************************************
 #
 #     Project:  TTVIEW 1.x
 #
 #      Module:  makefile
 #
 #   Author(s):  Charles E. Kindel, Jr. (cek)
 #
 #      System:  Microsoft Windows 3.x, MS-DOS 5.0, Compiled with Microsoft  
 #               C 6.00A and Microsoft MASM 5.1.                             
 #                                                                           
 #
 # NOTE:  This is a very generic makefile.  Just define MAINTARGET and
 #        specify your object files $(OBJS1) and $(OBJS2) and that's it.
 #*****************************************************************************

MAINTARGET=TTVIEW

# by commenting out the following line you can turn off silent mode
!CMDSWITCHES +s

# set these macros to control where stuff is placed/redirected

!IFNDEF OUTPUT
OUTPUT  =   con
!ENDIF

# if PUTEXE is defined the dll/exe will be copied from the root to
PUTEXE  = ..\BUILD\

# if putlib is defined the lib will be copied from the root to putlib
PUTLIB  = ..\LIB\

# if pubinc is define the include file will be copied there
PUTINC  = ..\INC\

#
#  Build tool macros.  These control how the tools build files.
#
!IF "$(DEBUG)"=="1"

CFLAGS      =   -f -c -nologo -DDEBUG -AM -G2sw -Owd -W3 -Zip -D$(MAINTARGET)
LINKOPTS    =   /NOLOGO/BATCH/NOD/NOE/A:16/CO/M
RCOPTS      =   -r -DDEBUG -iSRC;INC;RES
BUILDTYPE   =   DEBUG

!ELSE

CFLAGS      =   -c -nologo -AM -G2sw -Owt -W3 -Zp -D$(MAINTARGET)
LINKOPTS    =   /NOLOGO/BATCH/NOD/NOE/A:16/M
RCOPTS      =   -r -iSRC;INC;RES
BUILDTYPE   =   RELEASE

!ENDIF

PCHFLAGS    =  -YuPRECOMP.H -Fp$(MYOBJ)PRECOMP.PCH
RESOPTS     =  -v -t

BANNER      = $(MAINTARGET): **** $(BUILDTYPE) *** $(BUILDTYPE) *** $(BUILDTYPE) *** $(BUILDTYPE) ****

LIBS        = libw mlibcew commdlg wdb cek

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
VERINCS = $(MYINC)VERSION.H $(MYRES)$(MAINTARGET).RCV

#
# Resources
#

LRCINCS = $(VERINCS)\
          $(MYINC)dlgs.h\
          $(MYRES)$(MAINTARGET).rc\
          $(MYRES)$(MAINTARGET).dlg\
          $(MYRES)$(MAINTARGET).str\
          $(MYRES)$(MAINTARGET).ico

# 
# Library dependencies
# 
LMISC   =   MAKEFILE\
            $(MAINTARGET).DEF

LINCS   =   $(MYOBJ)PRECOMP.PCH $(MYINC)GLOBAL.H

LOBJS   =\
            $(MYOBJ)PRECOMP.OBJ\
            $(MYOBJ)$(MAINTARGET).OBJ\
            $(MYOBJ)CONFIG.OBJ\
            $(MYOBJ)INIT.OBJ\
            $(MYOBJ)BOTSTAT.OBJ\
            $(MYOBJ)WNDMGR.OBJ\
            $(MYOBJ)MENU.OBJ\
            $(MYOBJ)CMD.OBJ\
            $(MYOBJ)SUBDLGS.OBJ\
            $(MYOBJ)dlghelp.obj\
            $(MYOBJ)LISTVIEW.obj\
            $(MYOBJ)MDI.OBJ

LOBJS2  =

#
#   BUILD RULES
#
all : $(MAINTARGET).EXE 

#
# Main inference rule for .C files.  Looks a bit silly but gets
# around the 128 char command line restriction for CL
#
# Note that if the compile fails the CL environment varaible will
# be set...
#
{$(MYSRC)}.c{$(MYOBJ)}.obj: 
   @echo *** $(BUILDTYPE) *** Compiling $(@B).C to $@... >$(OUTPUT)
   @SET CL=$(CFLAGS) $(PCHFLAGS)
   $(CC) -Fo$@ $(MYSRC)$(@B).C >$(MYSRC)$(@B).ERR
   @TYPE $(MYSRC)$(@B).ERR
   @SET CL=


#   $(MAINTARGET).EXE build rules

$(MAINTARGET).EXE : $(LOBJS) $(LOBJS2) $(MYRES)$(MAINTARGET).res $(MAINTARGET).def $(MYOBJ)WINSTUB.EXE
   @echo *** Linking $(MAINTARGET) ($(LLN))
   link @<<
$(LOBJS)+
$(LOBJS2)
$(MAINTARGET)
$(MAINTARGET).MAP
$(LIBS)
$(MAINTARGET).def $(LINKOPTS)
<<
                   rc $(RESOPTS) $(MYRES)$(MAINTARGET).res $(MAINTARGET).EXE
                   @mapsym $(MAINTARGET).map
#                   @copy $(MAINTARGET).EXE ..\build
#                   @copy $(MAINTARGET).SYM ..\build
                   @type $(MYINC)version.h
                   @echo.>$(OUTPUT)
                   @echo $(BANNER)>$(OUTPUT)

$(MYOBJ)PRECOMP.OBJ :  $(MYINC)PRECOMP.H
    @echo *** $(BUILDTYPE) *** Pre-compiling $(@B).C to $@ and $(MYOBJ)PRECOMP.PCH... >$(OUTPUT)
    @SET CL=$(CFLAGS)
    $(CC) -YcPRECOMP.H -Fp$(MYOBJ)PRECOMP.PCH -Fo$(MYOBJ)PRECOMP.OBJ -NT PRECOMP $(MYSRC)PRECOMP.C>$(MYSRC)$(@B).ERR
    @SET CL=
    @type $(MYSRC)$(@B).ERR>$(OUTPUT)

$(MYHELP)$(MAINTARGET).HLP: help\$(MAINTARGET).rtf help\map.h help\$(MAINTARGET).hpj
   chdir HELP
   $(MAKE)
   copy $(MAINTARGET).HLP ..
   chdir ..

$(MYOBJ)WINSTUB.EXE:  $(MYSRC)WINSTUB.ASM
    masm -I.\  $(MYSRC)winstub.asm,$(MYOBJ)WINSTUB.OBJ;
    link $(MYOBJ)WINSTUB.OBJ,$(MYOBJ)WINSTUB.EXE;

$(MYRES)$(MAINTARGET).res:  $(LINCS) $(LRCINCS)
    @echo *** $(LXX) *** Building $(MYRES)$(MAINTARGET).RES
    rc $(RCOPTS) $(MYRES)$(MAINTARGET).rc $(MYRES)$(MAINTARGET).RES

$(MYSRC)INIT.OBJ:  $(MYRES)$(MAINTARGET).STR
   @echo *** $(BUILDTYPE) *** Compiling $(@B).C to $@... >$(OUTPUT)
   @SET CL=$(CFLAGS) $(PCHFLAGS)
   $(CC) -Fo$@ $(MYSRC)$(@B).C >$(MYSRC)$(@B).ERR
   @TYPE $(MYSRC)$(@B).ERR
   @SET CL=


#   MISC. STUFF

zip         :   clean
                pkzip -psr $(MAINTARGET).zip *.*

version     :   clean
                bumpver $(MYINC)VERSION.H

.ignore : 
clean       :  
                !del $(MYOBJ)*.obj
                !del $(MYOBJ)*.exe
                !del $(MYOBJ)*.pch
                !del *.map
                !del *.sym
                !del $(MYRES)*.res
                !del *.EXE
                !del *.DLL
                !del *.lib
                !del $(MYSRC)*.err
                @echo *** DONE ***

ship        :   all
                !copy $(MAINTARGET).EXE $(PUTEXE)
                !bumpver $(MYINC)VERSION.H
                @echo *** DONE ***

        
