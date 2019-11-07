#*************************************************************
#  
# MAKEFILE for the ToolPal minitask application
#
#
#*************************************************************

MEM      =-AM
LIBS     =mlibcew wdb

CLOPTS   =$(MEM) -c -Gsw -Oad -Zpei -W3 -nologo -DDEBUG
LINKOPTS =/NOD/NOE/CO

#CLOPTS   =$(MEM) -c -Gsw -Oad -Zp -W3 -nologo
#LINKOPTS =/NOD/NOE

minitask.exe:      minitask.obj\
               minitask.def\
               minitask.res 
    link $(LINKOPTS) minitask,,, $(LIBS) libw, minitask.def
    rc -v minitask.res

minitask.obj: minitask.c resource.h 
    cl $(CLOPTS) minitask.c > minitask.err

minitask.res: minitask.rc minitask.ico
    rc -r minitask.rc

