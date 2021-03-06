#*************************************************************************
#
#   NOTE::  THIS MAKEFILE IS EXPERIMENTAL.  It is designed as an
#           experiment in creating 'single source' WinApps (i.e. apps
#           that will compile for either Win16 or Win32.  Use at
#           your own risk.
#
#   This is a sample makefile that uses W16W32.MAK to buld a .DLL
#
#   This makefile uses the following macros to determine the type of build.
#   You can define these in the makefile that includes this file, or 
#   as environment variables.
#
#       _CPUBITS_   - Number of bits for the target processor (16 or 32)
#                     Typically set as an environment variable.
#
#       Cpu         - Either MIPS or i386.  
#                     Only valid if _CPUBITS_ = 32.
#                     Default is i386.
#                     Typically set as an environment variable.
#
#       _BUILDTYPE_ - Either DEBUG or NODEBUG.  
#                     Default is NODEBUG.
#                     Typically set as an environment variable.
#
#   For example, if you are on a MIPS machine and you type the following:
#      
#       nmake _CPUBITS_=32 _BUILDTYPE_=DEBUG
#
#   you would build a 32 bit MIPS image with debug information.  MIPS
#   is assumed because by default on a MIPS machine WinNT sets the
#   Cpu environment variable for you.
#
#*************************************************************************

#----------------------------------------------------------------------
# Configure directories (modify to suite your environment)
#
# _RES_ is where all resource source files are (.RC, .DLG, .ICO...)
# _SRC_ is where all source code is (.C, .ASM...)
# _INC_ is where the include files reside, by default this is same as _SRC_
# _OBJ_ is where all intermediate files are put (.OBJ, .RES)
#
# NOTE:  MIPS\ will be appended to _OBJ_ if we are compiling for mips
#
# If you choose not to define these, W16W32.MAK will define them
# for you.
#----------------------------------------------------------------------

!IFNDEF _RES_
_RES_ =     RES\ 
!ENDIF

!IFNDEF _INC_
_INC_ =     SRC\ 
!ENDIF

!IF "$(_CPUBITS_)" == "32"
!IF "$(CPU)" == "MIPS"
_OBJ_ =     OBJ32\MIPS\ 
!ELSE
_OBJ_ =     OBJ32\ 
!ENDIF
!ELSE
_OBJ_ =     OBJ\ 
!ENDIF

!IFNDEF _SRC_
_SRC_ =     SRC\ 
!ENDIF

!IFNDEF _HLP_
_HLP_ =     HELP\ 
!ENDIF

#----------------------------------------------------------------------
# Where do you want the 'built' components put?
#
# _PUTEXE_ is where the .DLL or .EXE files will be copied.
# _PUTINC_ is where the .H files will be copied.
# _PUTLIB_ is where the .LIB files will be copied.
#
# If you don't define these no files will be copied.
#----------------------------------------------------------------------
_PUTEXE_    =   TEST
_PUTLIB_    =   TEST
_PUTINC_    =   TEST\SRC

#----------------------------------------------------------------------
# Specify the name of the main target and it's type.
#
# By default W16W32.MAK will synthesize a '32 bit' main target name
# from the one defined in _MAINTARGET_ by appending '32' to the end.
# You can override this default behavior by defining _MAINTARGET32_.
#----------------------------------------------------------------------
_MAINTARGET_    = TD
_MAINTARGET32_  = TD32
_EXT_           = DLL
_USE_PRECOMP_   = TRUE

_OBJ_DEPEND1_   =\
!IF "$(_CPUBITS_)" != "32"
            $(_OBJ_)PRECOMP.OBJ\
!ENDIF
            $(_OBJ_)TDUTIL.OBJ\
            $(_OBJ_)WEP.OBJ\
            $(_OBJ_)SHADOW.OBJ\
            $(_OBJ_)3DCTRL.OBJ\
!IF "$(_CPUBITS_)" != "32"
            $(_OBJ_)LIBENTRY.OBJ
!ENDIF            

_RC_DEPEND_    =\
            $(_RES_)TD.RC\
            $(_RES_)3DBTN.BMP\
            $(_RES_)3DBTNS.BMP\
            $(_SRC_)VERSION.H\
            $(_RES_)TDUTIL.RCV

#----------------------------------------------------------------------
# For Win16 apps, specify the memory model
#----------------------------------------------------------------------
!IF "$(_CPUBITS_)" != "32"

_MODEL_     = M

!ENDIF

#----------------------------------------------------------------------
# Specify the LIB files you need to link with.
#
# By default WIN32 apps will link with the guilibs and WIN16 apps
# will link with libw.lib and $(_MODEL_)libcew.lib.
#----------------------------------------------------------------------
!IF "$(_CPUBITS_)" == "32"

_LIBS_      = 
_DEBUGLIBS_ =  WDB32.LIB

!ELSE

# For the 16 bit version, you will probably want to include
# 'commdlg', 'ver', etc... here.
_LIBS_      = 
_DEBUGLIBS_ = WDB.LIB

!ENDIF

#----------------------------------------------------------------------
# Optionally specify flags for RC to use when attaching resources
# to a Win16 image with _RESFLAGS_.
#----------------------------------------------------------------------

!IF "$(_CPUBITS_)" != "32"
# _RESFLAGS_  = -v -t -31
!ENDIF

#----------------------------------------------------------------------
# Optionally specify flags for CL.
#----------------------------------------------------------------------

# _CVARS_ = 


#----------------------------------------------------------------------
# Include W16W32.MAK
#----------------------------------------------------------------------
!INCLUDE <W16W32.MAK>


#----------------------------------------------------------------------
# Specify dependencies if needed.
#
# It would be nice if we could do this above where we define
# _OBJ_DEPEND1_ and _OBJ_DEPEND2_ but since _OBJ_ changes depending on build
# type we have to do it after we include W16W32.MAK.
#----------------------------------------------------------------------
$(_OBJ_)TDUTIL.OBJ  : $(_SRC_)$(@B).C 

$(_OBJ_)WEP.OBJ     : $(_SRC_)$(@B).C

$(_OBJ_)SHADOW.OBJ  : $(_SRC_)$(@B).C

$(_OBJ_)3DCTRL.OBJ  : $(_SRC_)$(@B).C



