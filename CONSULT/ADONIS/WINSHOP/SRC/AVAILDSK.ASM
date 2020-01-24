;****************************************************************************
;*                                                                          *
;*  WFDOSDIR.ASM -                                                          *
;*                                                                          *
;*      Directory searching primitives                                      *
;*                                                                          *
;****************************************************************************

PLM=1      ; PASCAL Calling convention is DEFAULT
WIN=1      ; Windows calling convention
?386=0      ; Use 386 code?

include cmacros.inc

; The following structure should be used to access high and low
; words of a DWORD.  This means that "word ptr foo[2]" -> "foo.hi".

externFP    <DOS3CALL>

LONG    struc
lo      dw      ?
hi      dw      ?
LONG    ends

FARPOINTER      struc
off     dw      ?
sel     dw      ?
FARPOINTER      ends

ifndef SEGNAME
    SEGNAME equ <TEXT>
endif

if ?386
    createSeg _%SEGNAME, CodeSeg, word, use16, CODE
else
    createSeg _%SEGNAME, CodeSeg, word, public, CODE
endif

NOT_SUPPORTED     =  2h      ; Return code from IsDeviceRemote function.
REMOTE            =  3h      ; Return code for remote drive found.
TRUE              =  1h      ; TRUE Definition
FALSE             =  0h      ; False Definition.

;=============================================================================

sBegin DATA

VolExtendedFCB  db  0FFh
                db  0, 0, 0, 0, 0
                db  1000b
                db  0
                db  11 dup('?')
                db  5 dup(0)
                db  11 dup('?')
                db  9 dup(0)

DTA             db  64 dup(0)

sEnd   DATA

;=============================================================================

sBegin CodeSeg

assumes CS,CodeSeg
assumes DS,DATA

;*--------------------------------------------------------------------------
;*
;*  LONG DosDiskFreeSpace(Drive)
;*
;*  note:
;*      Drive == 0      default
;*      Drive == 1      A
;*      Drive == 2      B
;*--------------------------------------------------------------------------

; Returns the number of bytes free in DX:AX

cProc DosDiskFreeSpace, <FAR, PUBLIC>
ParmW Drive
cBegin
            mov     dx,Drive
            mov     ah,36h              ; Set Current Drive
            cCall   DOS3Call
            cmp     ax, 0ffffh          ; bogus drive?
            je      error
            mul     cx                  ;
            mul     bx
            jmp     done                ; DX:AX contains free space
error:
            mov     dx, ax              ; return dx:ax = -1L
done:
cEnd

sEnd CodeSeg

end
