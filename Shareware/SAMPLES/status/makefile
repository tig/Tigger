#*************************************************************
#  
# MAKEFILE for stattest application
#
# Note this sample is protect mode only!!
#
#*************************************************************

#DEBUG=1

!ifdef CPU

!include <ntwin32.mak>

   ###################################################
   #                                                 #
   #  Build STATTEST for Win32                       #
   #                                                 #
   ###################################################

stattest.exe:   stattest.obj\
                status.obj\
                stattest.def\
                stattest.res 
    $(link) $(guiflags) -out:stattest.exe stattest.obj status.obj stattest.res $(guilibs)

stattest.obj: stattest.c resource.h
    $(cc) $(cflags) $(cvars) stattest.c > stattest.err
    $(cvtobj) stattest.obj

status.obj: status.c status.h
    $(cc) $(cflags) $(cvars) status.c > status.err
    $(cvtobj) status.obj

stattest.res: stattest.rc stattest.ico stattest.dlg resource.h
    rc -r -fo stattest.tmp stattest.rc
    cvtres -$(CPU) stattest.tmp -o stattest.res
    del stattest.tmp

!else

   ###################################################
   #                                                 #
   #  Build STATTEST for Win16			     #
   #                                                 #
   ###################################################

MEM      =-AM

!ifdef DEBUG
LIBS     =wdb mlibcew
CLOPTS   =-f $(MEM) -c -G2sw -Owd -Zpei -W3 -DDEBUG
LINKOPTS =/NOD/NOE/CO
!else
LIBS     =mlibcew
CLOPTS   =$(MEM) -c -G2sw -Owt -Zp -W3 
LINKOPTS =/NOD/NOE
!endif

stattest.exe:   stattest.obj\
                status.obj\
                stattest.def\
                stattest.res 
    link $(LINKOPTS) stattest status,,,$(LIBS) libw, stattest.def
    rc -v -30 stattest.res

stattest.obj: stattest.c resource.h
    cl $(CLOPTS) stattest.c > stattest.err

status.obj: status.c status.h
    cl $(CLOPTS) status.c > status.err

stattest.res: stattest.rc stattest.ico stattest.dlg resource.h
    rc -r stattest.rc

!endif
