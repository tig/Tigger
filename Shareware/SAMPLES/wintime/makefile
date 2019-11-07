#*************************************************************
#  
# MAKEFILE for the WinTime test application
#
#
#*************************************************************

MEM      =-AM
LIBS     =mlibcaw

CLOPTS   =$(MEM) -c -Gsw -Oad -Zpei -W3 -FPa -nologo -DDEBUG -DSAMPLE
#CLOPTS   =$(MEM) -c -Gsw -Oad -Zp -W3 -FPa -nologo -DSAMPLE

LINKOPTS =/NOD/NOE/CO
#LINKOPTS =/NOD/NOE

test.exe: test.obj wintime.obj test.def test.res 
    link $(LINKOPTS) test wintime,,, libw $(LIBS), test.def
    rc -v -30 test.res

test.obj: test.c test.h testd.h
    cl $(CLOPTS) test.c > test.err

wintime.obj: wintime.c wintime.h 
    cl $(CLOPTS) wintime.c > wintime.err

test.res: test.rc test.ico 
    rc -r test.rc
