 #*****************************************************************************
 #
 #  W16W32.MAK
 #
 #      This file is a makefile 'include' that allows easy development of
 #      single source Win16/Win32 executables.
 #
 #      You can treat this file somewhat like a .H file.  It is generic
 #      enough that most projects can include it without modification.
 #
 #      The end of this file contains two 'sample' makefiles, one for a DLL 
 #      and one for an EXE. 
 #
 #  NAMING CONVETIONS USED
 #      All nmake macros implemented by W16W32.MAK are defined in UPPER case.
 #      A macro in lower case indicates that that macro was derived from
 #      an external source such as the NTWIN32.MAK file or default nmake 
 #      macros.
 #
 #      Macros bracketed by underscores ('_') are user defineable (i.e. the
 #      makefile that includes this file can define them).
 #
 #      Macros that do not have the underscores are 'local' to this file.
 #
 #  Author(s):  
 #      Charles E. Kindel, Jr. (ckindel)       
 #
 #  Revisions:
 #      10/31/92    cek     Wrote it.
 #
 #  Comments:
 #
 #      NOTE:   This thing is 'Early Beta Quality'.  In otherwords, I'm
 #              still working on it.  I have not been able to test
 #              if it works correctly on MIPS.
 #  
 #              If you start using this thing PLEASE keep me posted
 #              on any changes you make!
 #
 #              Thanks, charlie
 #
 #*****************************************************************************

#!CMDSWITCHES +s

#----------------------------------------------------------------------
# If we're compiling for Win32 then include the Win32 SDK makefile
# template.
#----------------------------------------------------------------------
!IFNDEF _CPUBITS_
_CPUBITS_=16
!ENDIF

!IF "$(_CPUBITS_)" == "32"
!INCLUDE <ntwin32.mak>
!ENDIF

!IFDEF _USE_PRECOMP_
!IF "$(_CPUBITS_)" != "32"
### PCH is busted in 340
PCH_FLAGS    =  -YuPRECOMP.H -Fp$(_OBJ_)PRECOMP.PCH
!ENDIF
!ENDIF

!IFNDEF _BUILDTYPE_
_BUILDTYPE_ =   NODEBUG
!ENDIF

!IFNDEF _EXT_
_EXT_ =   EXE
!ENDIF

#----------------------------------------------------------------------
# There are significant differences between the tool switches and
# flags for Win16 and Win32 apps.  We branch here depending on 
# which platform we are building for.
#
# Within each platform branch we also branch depending on whether we
# are trying to build an EXE or a DLL.
#----------------------------------------------------------------------
!IF "$(_CPUBITS_)" == "32"

### PCH is busted in 340
!UNDEF PCH_FLAGS

!   IF "$(MAINTARGET32)" == ""

MAINTARGET32 =    $(_MAINTARGET_)32

!   ENDIF

!   IF "$(_EXT_)" == "DLL"

C_FLAGS    = $(cflags) $(cvarsdll) $(_CVARS_)
LINK_FLAGS = -dll $(guilibsdll) $(_LIBS_) -entry:LibMain$(DLLENTRY)
RC_FLAGS   = -r $(cvarsdll)

!   ELSE

C_FLAGS    = $(cflags) $(cvars) $(_CVARS_)
LINK_FLAGS = $(guilibs) $(_LIBS_)
RC_FLAGS   = -r $(cvars)

!   ENDIF

!   IF "$(_BUILDTYPE_)" == "DEBUG"

C_FLAGS    = $(C_FLAGS) $(cdebug) $(cvtdebug) -DDEBUG
LINK_FLAGS = $(LINK_FLAGS) $(linkdebug) $(_DEBUGLIBS_)
RC_FLAGS   = -i$(_RES_) $(RC_FLAGS) -DDEBUG

!   ENDIF

all : $(MAINTARGET32).$(_EXT_)

!ELSE

#----------------------------------------------------------------------
# Build a 16 bit executable
#----------------------------------------------------------------------

!   IF "$(_EXT_)" == "DLL"

LIBS      =  libw $(_MODEL_)dllcew

!   ELSE

LIBS      =  libw $(_MODEL_)libcew

!   ENDIF

!   IF "$(_BUILDTYPE_)" == "DEBUG"

C_FLAGS    = -nologo -c -W3 -A$(_MODEL_)w -G2sw -Owd -Zip -DDEBUG $(_CVARS_)
LINK_FLAGS = /NOD/NOE/MAP/nologo/CO 
RC_FLAGS   = -r -i$(_RES_) -DDEBUG $(_CVARS_)
LIBS      = $(LIBS) $(_DEBUGLIBS_)

!   ELSE

C_FLAGS    = -nologo -c -W3 -A$(_MODEL_)w -G2sw -Owt $(_CVARS_)
LINK_FLAGS = /NOD/NOE/MAP/nologo 
RC_FLAGS   = -r -i$(_RES_) $(_CVARS_)
LIBS      = $(LIBS) $(_LIBS_)

!   ENDIF

!   IFNDEF _RESFLAGS_
_RESFLAGS_  = -v -t -31
!   ENDIF

all : $(_MAINTARGET_).$(_EXT_)

!ENDIF


#----------------------------------------------------------------------
# Main inference rule for .C files.  Looks a bit silly but gets
# around the 128 char command line restriction for CL
#
# Note that if the compile fails the CL environment varaible will
# be set...
#----------------------------------------------------------------------
{$(_SRC_)}.c{$(_OBJ_)}.obj: 
   @echo *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Compiling $(@B).C... 
!IF "$(_CPUBITS_)" == "32"
   $(cc)  $(_SRC_)$(@B).C $(C_FLAGS) -Fo$@ > $(_SRC_)$(@B).ERR
   @TYPE $(_SRC_)$(@B).ERR
!ELSE
   @SET CL=$(C_FLAGS) $(PCH_FLAGS)
   $(CC) -Fo$@ $(_SRC_)$(@B).C >$(_SRC_)$(@B).ERR
   @TYPE $(_SRC_)$(@B).ERR
   @SET CL=
!ENDIF

#-----------------------------------------------------------------------
# If PCH_FLAGS is defined, this will build the pre-compiled headers
#-----------------------------------------------------------------------
$(_OBJ_)PRECOMP.OBJ:  $(_SRC_)PRECOMP.H
    @echo *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Pre-compiling $(@B).C... 
    @SET CL=$(C_FLAGS)
    $(CC) -YcPRECOMP.H -Fp$(_OBJ_)PRECOMP.PCH -Fo$@ $(_SRC_)$(@B).C > $(_SRC_)$(@B).ERR
    @SET CL=

#-----------------------------------------------------------------------
# Compile resources
#-----------------------------------------------------------------------
$(_OBJ_)$(_MAINTARGET_).RES: $(_RC_DEPEND_) 
    @echo *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Building Resources : $(@B).RES... 
!IF "$(_CPUBITS_)" == "32"
    rc $(RC_FLAGS) -fo$(_OBJ_)$(_MAINTARGET_).tmp $(_RES_)$(_MAINTARGET_).rc
    cvtres -$(CPU) $(_OBJ_)$(_MAINTARGET_).TMP -o $(_OBJ_)$(_MAINTARGET_).RES
    del $(_OBJ_)$(_MAINTARGET_).TMP
!ELSE
    rc  $(RC_FLAGS) -fo$(_OBJ_)$(_MAINTARGET_).RES $(_RES_)$(_MAINTARGET_).rc $(_OBJ_)$(_MAINTARGET_).RES
!ENDIF

#-----------------------------------------------------------------------
# Maintarget build rule
#-----------------------------------------------------------------------
!IF "$(_EXT_)" == "DLL"
$(MAINTARGET32).$(_EXT_) : $(MAINTARGET32).LIB $(_OBJ_DEPEND1_) $(_OBJ_DEPEND2_) $(_OBJ_)$(_MAINTARGET_).RES $(MAINTARGET32).DEF
!ELSE
$(MAINTARGET32).$(_EXT_) : $(_OBJ_DEPEND1_) $(_OBJ_DEPEND2_) $(_OBJ_)$(_MAINTARGET_).RES $(MAINTARGET32).DEF
!ENDIF
    @echo *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Linking $(@B).$(_EXT_)... 
!IF "$(_EXT_)" == "DLL"
    $(link) $(LINK_FLAGS) -out:$(MAINTARGET32).$(_EXT_) $(_OBJ_)$(_MAINTARGET_).RES $(MAINTARGET32).EXP $(_OBJ_DEPEND1_) $(_OBJ_DEPEND2_)
!ELSE
    $(link) $(LINK_FLAGS) -out:$(MAINTARGET32).$(_EXT_) $(_OBJ_)$(_MAINTARGET_).RES $(_OBJ_DEPEND1_) $(_OBJ_DEPEND2_)
!ENDIF
!   IFDEF _PUTLIB_
    @copy $(MAINTARGET32).lib $(_PUTLIB_)
!   ENDIF
!   IFDEF _PUTEXE_
    @copy $(MAINTARGET32).$(_EXT_) $(_PUTEXE_)
#    @copy $(MAINTARGET32).SYM $(_PUTEXE_)
!   ENDIF
!   IFDEF _PUTINC_
    @copy $(_SRC_)$(_MAINTARGET_).H $(_PUTINC_) 
!   ENDIF
    @echo *** $(_MAINTARGET32_) *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Done!
    @echo *** $(_MAINTARGET32_) *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Done!
    @echo *** $(_MAINTARGET32_) *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Done!

!IF "$(_EXT_)" == "DLL"
$(MAINTARGET32).LIB: $(_OBJ_DEPEND1_) $(_OBJ_DEPEND2_) $(MAINTARGET32).DEF
    @echo *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Building Import Library for $(@B).$(_EXT_)... 
    lib -machine:$(CPU) -def:$(MAINTARGET32).DEF $(_OBJ_DEPEND1_) $(_OBJ_DEPEND2_) -out:$(MAINTARGET32).LIB
!ENDIF
    
$(_MAINTARGET_).$(_EXT_) : $(_OBJ_DEPEND1_) $(_OBJ_DEPEND2_) $(_OBJ_)$(_MAINTARGET_).RES $(_MAINTARGET_).DEF
    @echo *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Linking $(@B).$(_EXT_)... 
    link  @<<
$(LINK_FLAGS)+
$(_OBJ_DEPEND1_)+
$(_OBJ_DEPEND2_)
$(_MAINTARGET_).$(_EXT_)
$(_MAINTARGET_).MAP
$(LIBS)
$(_MAINTARGET_).DEF
<<
    @echo *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Adding Resources to $(@B).$(_EXT_)... 
    rc $(_RESFLAGS_) $(_OBJ_)$(_MAINTARGET_).RES $(_MAINTARGET_).$(_EXT_)
!IF "$(_EXT_)" == "DLL"
    @echo *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Building Import Library for $(@B).$(_EXT_)... 
    implib $(_MAINTARGET_).LIB $(_MAINTARGET_).DEF
!ENDIF
    @echo *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Building symbol file $(@B).SYM... 
    mapsym $(_MAINTARGET_).map
!   IFDEF _PUTLIB_
    @copy $(_MAINTARGET_).lib $(_PUTLIB_)
!   ENDIF
!   IFDEF _PUTEXE_
    @copy $(_MAINTARGET_).$(_EXT_) $(_PUTEXE_)
    @copy $(_MAINTARGET_).SYM $(_PUTEXE_)
!   ENDIF
!   IFDEF _PUTINC_
    @copy $(_SRC_)$(_MAINTARGET_).H $(_PUTINC_) 
!   ENDIF
    @echo *** $(_MAINTARGET_) *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Done!
    @echo *** $(_MAINTARGET_) *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Done!
    @echo *** $(_MAINTARGET_) *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Done!

$(_OBJ_)LIBENTRY.OBJ: $(_SRC_)LIBENTRY.ASM
    @echo *** $(_BUILDTYPE_) *** Win$(_CPUBITS_) $(CPU) *** Assembling $(@B).ASM... 
    masm -Mx $(_SRC_)LIBENTRY, $(_OBJ_)LIBENTRY;

#-----------------------------------------------------------------------
#   MISC. STUFF
#-----------------------------------------------------------------------
version     :   clean
                bumpver $(_INC_)VERSION.H


.ignore : 
clean       :  
                !del $(_OBJ_)*.obj
                !del $(_OBJ_)*.res
                !del *.map
                !del *.sym
                !del *.$(_EXT_)
                !del *.lib
                !del $(_SRC_)*.err
                @echo *** DONE ***


