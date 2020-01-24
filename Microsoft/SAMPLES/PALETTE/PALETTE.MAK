MEM      =-AS
LIB      =slibcew
CLOPTS   =$(MEM) -c -Gsw -Od -W3 -Zpei
LINKOPTS =/NOD/NOE/CO

all: palette.exe

palette.res: palette.rc paletted.h palette.dlg palette.ico
    rc -r palette.rc

palette.obj: palette.c palette.h paletted.h
    cl $(CLOPTS) palette.c > palette.err

palette.exe: palette.obj palette.def palette.res
    link $(LINKOPTS) palette, /align:16, /map, $(LIB) libw, palette.def
    rc -v palette.res
