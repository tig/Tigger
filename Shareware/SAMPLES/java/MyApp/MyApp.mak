# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Java Virtual Machine Java Workspace" 0x0809

!IF "$(CFG)" == ""
CFG=MyApp - Java Virtual Machine Debug
!MESSAGE No configuration specified.  Defaulting to MyApp - Java Virtual\
 Machine Debug.
!ENDIF 

!IF "$(CFG)" != "MyApp - Java Virtual Machine Release" && "$(CFG)" !=\
 "MyApp - Java Virtual Machine Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MyApp.mak" CFG="MyApp - Java Virtual Machine Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MyApp - Java Virtual Machine Release" (based on\
 "Java Virtual Machine Java Workspace")
!MESSAGE "MyApp - Java Virtual Machine Debug" (based on\
 "Java Virtual Machine Java Workspace")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
JAVA=jvc.exe

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\MyApp.class" "$(OUTDIR)\MyAppFrame.class"

CLEAN : 
	-@erase "$(INTDIR)\MyApp.class"
	-@erase "$(INTDIR)\MyAppFrame.class"

# ADD BASE JAVA /O
# ADD JAVA /O

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\MyApp.class" "$(OUTDIR)\MyAppFrame.class"

CLEAN : 
	-@erase "$(INTDIR)\MyApp.class"
	-@erase "$(INTDIR)\MyAppFrame.class"

# ADD BASE JAVA /g
# ADD JAVA /g

!ENDIF 

################################################################################
# Begin Target

# Name "MyApp - Java Virtual Machine Release"
# Name "MyApp - Java Virtual Machine Debug"

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\MyApp.java

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"


"$(INTDIR)\MyApp.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"


"$(INTDIR)\MyApp.class" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MyAppFrame.java

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"


"$(INTDIR)\MyAppFrame.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"


"$(INTDIR)\MyAppFrame.class" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MyApp.html

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0001.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0002.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0003.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0004.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0005.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0006.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0007.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0008.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0009.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0010.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0011.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0012.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0013.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0014.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0015.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0016.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0017.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images\img0018.gif

!IF  "$(CFG)" == "MyApp - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "MyApp - Java Virtual Machine Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
