# Microsoft Developer Studio Generated NMAKE File, Format Version 42001
# ** DO NOT EDIT **

# TARGTYPE "Java Virtual Machine Java Workspace" 0x0809

!IF "$(CFG)" == ""
CFG=ImprovedFibonacci - Java Virtual Machine Debug
!MESSAGE No configuration specified.  Defaulting to ImprovedFibonacci - Java\
 Virtual Machine Debug.
!ENDIF 

!IF "$(CFG)" != "ImprovedFibonacci - Java Virtual Machine Release" && "$(CFG)"\
 != "ImprovedFibonacci - Java Virtual Machine Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ImprovedFibonacci.mak"\
 CFG="ImprovedFibonacci - Java Virtual Machine Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ImprovedFibonacci - Java Virtual Machine Release" (based on\
 "Java Virtual Machine Java Workspace")
!MESSAGE "ImprovedFibonacci - Java Virtual Machine Debug" (based on\
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
# PROP Target_Last_Scanned "ImprovedFibonacci - Java Virtual Machine Debug"
OUTDIR=.\JavaRel
INTDIR=.\JavaRel

ALL : "$(OUTDIR)\ImprovedFibonacci.class"

CLEAN : 
	-@erase "$(INTDIR)\ImprovedFibonacci.class"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

JAVA=jvc.exe

!IF  "$(CFG)" == "ImprovedFibonacci - Java Virtual Machine Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "JavaRel"
# PROP BASE Intermediate_Dir "JavaRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "JavaRel"
# PROP Intermediate_Dir "JavaRel"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "ImprovedFibonacci - Java Virtual Machine Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "JavaDbg"
# PROP BASE Intermediate_Dir "JavaDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "JavaDbg"
# PROP Intermediate_Dir "JavaDbg"
# PROP Target_Dir ""
# ADD BASE JAVA /g
# ADD JAVA /g

!ENDIF 

################################################################################
# Begin Target

# Name "ImprovedFibonacci - Java Virtual Machine Release"
# Name "ImprovedFibonacci - Java Virtual Machine Debug"

!IF  "$(CFG)" == "ImprovedFibonacci - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "ImprovedFibonacci - Java Virtual Machine Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ImprovedFibonacci.java

"$(INTDIR)\ImprovedFibonacci.class" : $(SOURCE) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
