PROJ = ttfdump
PROJFILE = makefile
DEBUG = 0
FAPI = 1
MEM_MODEL = M

STACK_D = 4096
STACK_R = 4096
CC  = cl
CFLAGS_G  = /A$(MEM_MODEL) /BATCH
CFLAGS_D  = /qc /Od /Zi /Zr /Gi$(PROJ).mdt
CFLAGS_R  = /Otlgic /W2
LINKER  = link
ILINK  = ilink
LRF  = echo > NUL
LFLAGS_G  = /NOI /BATCH
LFLAGS_D  = /CO /INC /FAR /PACKC
LFLAGS_R  = /EXE /FAR /PACKC
MAPFILE_D  = $(PROJ)
MAPFILE_R  = NUL

!if $(FAPI)
DEF_FILE = ttfdump.def
LIBS = $(MEM_MODEL)libce
BIND = bind
!else
DEF_FILE = NUL
BIND = rem
LIBS = $(MEM_MODEL)libcer
!endif

OBJS  = DMP_CMAP.obj DMP_GLYF.obj tdumps.obj ttfdump.obj TTIUA.obj\
        dmp_vdmx.obj do_time.obj psnames.obj

all: $(PROJ).exe

.SUFFIXES: .c .obj

do_time.obj : do_time.c

DMP_CMAP.obj : DMP_CMAP.C

DMP_GLYF.obj : DMP_GLYF.C

tdumps.obj : tdumps.c

ttfdump.obj : ttfdump.C

TTIUA.obj : TTIUA.C

dmp_vdmx.obj : dmp_vdmx.c

psnames.obj : psnames.c


$(PROJ).exe : $(OBJS)
!IF $(DEBUG)
	$(LRF) @<<$(PROJ).lrf
$(OBJS: = +^
)
$@
$(MAPFILE_D)
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_D) /ST:$(STACK_D);
<<
!ELSE
	$(LRF) @<<$(PROJ).lrf
$(OBJS: = +^
)
$@
$(MAPFILE_R)
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_R) /ST:$(STACK_R);
<<
!ENDIF
!IF $(DEBUG)
	$(ILINK) -a -e "$(LINKER) @$(PROJ).lrf" $@
	$(BIND) $(PROJ) api.lib os2.lib
!ELSE
	$(LINKER) @$(PROJ).lrf
	$(BIND) $(PROJ) api.lib os2.lib
!ENDIF


.c.obj :
!IF $(DEBUG)
	$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo$@ $<
!ELSE
	$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo$@ $<
!ENDIF
