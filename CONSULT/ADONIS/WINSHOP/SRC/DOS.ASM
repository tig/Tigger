;****************************************************************************
;*                                                                          *
;*  WFDOSDIR.ASM -                                                          *
;*                                                                          *
;*      Directory searching primitives                                      *
;*                                                                          *
;****************************************************************************

?PLM=1      ; PASCAL Calling convention is DEFAULT
?WIN=1      ; Windows calling convention
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

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  DosMkDir()                                                              *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc DosMkDir, <FAR, PUBLIC>
ParmD   szDir
cBegin
        lds     dx,szDir
        mov     ah,39h
        cCall   DOS3Call
        jc      mderror
        xor     ax,ax
        jmp     short mdexit

mderror:
        mov     ah,59h
        xor     bx,bx
        cCall   DOS3Call
           
mdexit:

cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  GetCurrentDrive() -                                                     *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc GetCurrentDrive, <FAR, PUBLIC>

cBegin
        mov     ah,19h              ; Get Current Drive
        cCall   DOS3Call
        sub     ah,ah               ; Zero out AH
cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  GetCurrentDirectory() -                                                 *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc DosCwd, <FAR, PUBLIC>, <SI, DI>

ParmD lpDest

cBegin
            push    ds                  ; Preserve DS

            call    GetCurrentDrive
            mov     si,ax               ; SI = Current drive

            les     di,lpDest           ; ES:DI = lpDest
            push    es
            pop     ds                  ; DS:DI = lpDest
            cld
            mov     ax,si               ; AX = Current drive
            inc     al                  ; Convert to logical drive number
            mov     dl,al               ; DL = Logical Drive Number
            add     al,'@'              ; Convert to ASCII drive letter
            stosb
            mov     al,':'
            stosb
            mov     al,'\'              ; Start string with a backslash
            stosb
            mov     byte ptr es:[di],0  ; Null terminate in case of error
            mov     si,di               ; DS:SI = lpDest[1]
            mov     ah,47h              ; Get Current Directory
            cCall   DOS3Call
            jc      CDExit              ; Skip if error
            xor     ax,ax               ; Return FALSE if no error
CDExit:
            pop     ds                  ; Restore DS
cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  SetCurrentDrive() -                                                     *
;*                                                                          *
;*--------------------------------------------------------------------------*

; Returns the number of drives in AX.

cProc SetCurrentDrive, <FAR, PUBLIC>

ParmW Drive

cBegin
            mov     dx,Drive
            mov     ah,0Eh              ; Set Current Drive
            cCall   DOS3Call
            sub     ah,ah               ; Zero out AH
cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  SetCurrentDirectory() -                                                 *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc DosChDir, <FAR, PUBLIC>

ParmD lpDirName

cBegin
            push    ds                  ; Preserve DS
            lds     dx,lpDirName        ; DS:DX = lpDirName

            mov     bx,dx
            mov     ax,ds:[bx]
            cmp     ah,':'
            jnz     cdnodrive

            ;
            ;       Convert drive letter to drive index
            ;
            or      al,20h
            sub     al,'a'
            xor     ah,ah

            push    dx
            cCall   SetCurrentDrive,<ax>
            pop     dx
cdnodrive:
            mov     ah,3Bh              ; Change Current Directory
            cCall   DOS3Call
            jc      SCDExit             ; Skip on error
            xor     ax,ax               ; Return FALSE if successful
SCDExit:
            pop     ds                  ; Restore DS
cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  DosValidDir()                                                           *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc DosValidDir, <FAR, PUBLIC>, <SI, DI>
ParmD       szDir
LocalV      szCwd, 128
cBegin
    lea     si,szCwd
    cCall   DosCwd,<ss,si>
    push    szDir.sel
    push    szDir.off
    call    DosChDir
    or      ax,ax
    pushf
    cCall   DosChDir,<ss,si>
    ;
    ;   return TRUE if DosChdir returns 0, FALSE otherwise
    ;
    xor     ax,ax                ; don't care about this return val.
    popf
    jnz     vdexit
    inc     ax
vdexit:

cEnd

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
