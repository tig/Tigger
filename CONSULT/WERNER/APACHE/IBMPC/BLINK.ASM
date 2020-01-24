;************************************************************************
;*
;*    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
;*
;*-----------------------------------------------------------------------
;*
;*     Project:  APACHE, Apache For Windows
;*
;*      Module:  blink.asm
;*
;*   Author(s):  Bryan A. Woodruff (baw)
;*               Charles E. Kindel, Jr. (cek)
;*
;*
;*     Remarks:  Paint functions for IBMPC Emulation
;*
;*   Revisions:  
;*     01.00.000  9/22/91 baw   Written with CMACROS.INC
;*
;************************************************************************

NAME BLINK

.286p

;-------------------------------------------------------------------------
.xlist                                  ; suspend listing
?PLM=1                                  ; support Pascal calling
?WIN=1                                  ; support Windows
memM=1                                  ; medium model
include CMacros.Inc
include Windows.Inc
include IBMPC.Inc
.list
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
; dynalinked externals
;-------------------------------------------------------------------------

externNP        <_GetStdCharRect>
externNP        <PaintRun>

externFP        <GetClientRect>
externFP        <ExtTextOut>
externFP        <GetWindowWord>
externFP        <LocalLock>
externFP        <LocalUnlock>

;-------------------------------------------------------------------------
; segment definition
;-------------------------------------------------------------------------

createSeg SCREEN, SCREEN, BYTE, PUBLIC, CODE

sBegin  SCREEN
assumes CS, SCREEN

;************************************************************************
;*  BOOL NEAR PASCAL BlinkEmulator( HWND hWnd )
;*
;*  Description: 
;*     Called from a WM_TIMER message.  This routine changes the
;*     blink state and invalidates corresponding areas.
;*
;*  Comments:
;*     10/11/91  baw  Re-written in assembly for speed.
;*
;************************************************************************

cProc   BlinkEmulator, <NEAR, PUBLIC>, <es, di, si>
        ParmW   hWnd                                    ; hWnd

        LocalW  nRow                                    ; starting row
        LocalW  nCol                                    ; starting col
        LocalW  nEndRow                                 ; ending row
        LocalW  nEndCol                                 ; ending col
        LocalW  nMaxCol                                 ; max col per row
        LocalW  fFirstPass                              ; first pass flag
        LocalW  nRun                                    ; run counter
        LocalW  hECB                                    ; ECB handle
        LocalV  rcClient, %(size RECT)                  ; client rect
cBegin

        cCall   GetWindowWord, <hWnd, GWW_ECB>          ; get ECB
        or      ax, ax                                  ; Q: NULL?
        jnz     BE_LockECB                              ;    N: continue
        jmp     BE_Exit                                 ;    Y: exit

BE_LockECB:
        mov     hECB, ax                                ; store handle
        cCall   LocalLock, <hECB>                       ; lock structure
        or      ax, ax                                  ; Q: NULL?
        jnz     BE_StartBlink                           ;    N: continue
        jmp     BE_Exit                                 ;    Y: exit

BE_StartBlink:
        mov     di, ax                                  ; di = -> ECB
        not     ds:[di.ecb_fBlinkState]                 ; invert blink flag
        push    hWnd                                    ; push parameters
        push    ss                                      ; for GetClientRect()
        lea     bx, rcClient                            ; call 
        push    bx
        cCall   GetClientRect                           ; do it

        ; nRow = max( 0, (rcClient.top + YOFFSET) / YCHAR )

        lea     bx, rcClient                            ; bx = -> rcClient
        mov     ax, word ptr ss:[bx.rcTop]              ; ax = rcClient.top
        add     ax, word ptr ds:[di.ecb_nOffsetY]       ; ax += YOFFSET
        cwd
        idiv    word ptr ds:[di.ecb_nCharY]             ; ax /= YCHAR
        cmp     ax, 0                                   ; Q: ax > 0
        jae     BE_StoreRow                             ;    Y: store it
        xor     ax, ax                                  ; ax = 0

BE_StoreRow:
        mov     nRow, ax                                ; store start row

        ; nEndRow = min( MAXROWS - 1, 
        ;                ((rcClient.bottom + YOFFSET) / YCHAR) - 1 )

        mov     ax, word ptr ss:[bx.rcBottom]           ; ax = rcClient.bottom
        add     ax, word ptr ds:[di.ecb_nOffsetY]       ; ax += YOFFSET
        cwd
        idiv    word ptr ds:[di.ecb_nCharY]             ; ax /= YCHAR
        dec     ax                                      ; ax--
        mov     cx, word ptr ds:[di.ecb_nRows]          ; cx = MAXROWS
        dec     cx                                      ; cx--
        cmp     ax, cx                                  ; Q: ax > cx
        jbe     BE_StoreEndRow                          ;    N: store it
        mov     ax, cx                                  ;    Y: ax = cx

BE_StoreEndRow:
        mov     nEndRow, ax                             ; store end row

        ; nCol = max( 0, (rcClient.left + XOFFSET) / XCHAR )

        mov     ax, word ptr ss:[bx.rcLeft]             ; ax = rcClient.left
        add     ax, word ptr ds:[di.ecb_nOffsetX]       ; ax += XOFFSET
        cwd
        idiv    word ptr ds:[di.ecb_nCharX]             ; ax /= XCHAR
        cmp     ax, 0                                   ; Q: ax > 0
        jae     BE_StoreCol                             ;    Y: store it
        xor     ax, ax                                  ; ax = 0

BE_StoreCol:
        mov     nCol, ax                                ; store start col

        ; nEndCol = min( MAXCOLS - 1, 
        ;                ((rcClient.right + XOFFSET) / XCHAR) - 1 )

        mov     ax, word ptr ss:[bx.rcRight]            ; ax = rcClient.right
        add     ax, word ptr ds:[di.ecb_nOffsetX]       ; ax += XOFFSET
        cwd
        idiv    word ptr ds:[di.ecb_nCharX]             ; ax /= XCHAR
        dec     ax                                      ; ax--
        mov     cx, word ptr ds:[di.ecb_nColumns]       ; cx = MAXCOLS
        dec     cx                                      ; cx--
        cmp     ax, cx                                  ; Q: ax > cx
        jbe     BE_StoreEndCol                          ;    N: store it
        mov     ax, cx                                  ;    Y: ax = cx

BE_StoreEndCol:
        mov     nEndCol, ax                             ; store end col

;------------------------------------------------------------------------
; now, we compute the run for each common attribute per row and
; determine if it is a blinking attribute, invalidate the
; corresponding rectangle if so.
;------------------------------------------------------------------------

        mov     fFirstPass, 1                           ; flag first pass
        les     bx, dword ptr ds:[di.ecb_lpAttrBuf]     ; es:bx = lpAttrBuf
        mov     cx, nRow                                ; cx = starting row

BE_RowLoop:
        cmp     cx, nEndRow                             ; Q: cx <= nEndRow
        jbe     BE_GetStartCol                          ;    Y: continue
        jmp     BE_CleanUp                              ; clean and exit

BE_GetStartCol:
        mov     dx, nCol                                ; dx = starting col
        mov     ax, nEndCol                             ; ax = nEndCol
        mov     nMaxCol, ax                             ; nMaxCol = ax

BE_ColLoop:
        cmp     dx, nMaxCol                             ; Q: dx <= nMaxCol
        jbe     BE_GetStartAttr                         ;    Y: continue
        inc     cx                                      ;    N: cur row++
        jmp     BE_RowLoop                              ;       loop

BE_GetStartAttr:
        mov     ax, cx                                  ; ax = cur row
        push    dx                                      ; save dx
        imul    word ptr ds:[di.ecb_nColumns]           ; ax *= MAXCOLS
        pop     dx                                      ; restore dx
        add     ax, dx                                  ; ax += current col
        shl     ax, 1                                   ; ax *= 2 (attr size)
        mov     si, ax                                  ; si = ax
        mov     ax, word ptr es:[bx + si]               ; ax <- es:[bx + si]

BE_ComputeRun:
        mov     nRun, 0                                 ; run length = 0
        push    di                                      ; save di
        mov     di, dx                                  ; di = current col

BE_RunLoop:
        inc     nRun                                    ; nRun++
        inc     di                                      ; di++
        cmp     di, nMaxCol                             ; Q: di <= nMaxCol
        ja      BE_RunComplete                          ;    N: break
        inc     si                                      ; index++
        inc     si                                      ; index++
        cmp     ax, word ptr es:[bx + si]               ; Q: same attr?
        je      BE_RunLoop                              ;    Y: loop

BE_RunComplete:
        pop     di                                      ; restore di
        test    ax, ATTRIBUTE_BLINKING                  ; Q: blinking?
        jz      BE_AddRunToPos                          ;    N: don't paint

        push    di                                      ; ptr to ECB
        push    word ptr ds:[di.ecb_hScreenDC]          ; device context
        push    ax                                      ; attribute
        push    cx                                      ; start row
        push    dx                                      ; start col
        push    nRun                                    ; length
        push    fFirstPass                              ; first pass (reset)

        cCall   PaintRun
        
        cmp     fFirstPass, 1                           ; Q: fFirstPass?
        jne     BE_AddRunToPos                          ;    N: continue
        mov     fFirstPass, 0                           ;    Y: not FirstPass

BE_AddRunToPos:
        add     dx, nRun                                ; current col += nRun
        jmp     BE_ColLoop                              ; loop

BE_CleanUp:
        cCall   LocalUnlock, <hECB>                     ; unlock block
        mov     ax, 1                                   ; return value

BE_Exit:

cEnd

sEnd    SCREEN

end                

