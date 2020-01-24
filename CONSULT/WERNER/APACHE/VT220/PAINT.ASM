;************************************************************************
;*
;*    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
;*
;*-----------------------------------------------------------------------
;*
;*     Project:  APACHE, Apache For Windows
;*
;*      Module:  paint.asm
;*
;*   Author(s):  Bryan A. Woodruff (baw)
;*               Charles E. Kindel, Jr. (cek)
;*
;*
;*     Remarks:  Paint functions for DECVT220
;*
;*   Revisions:  
;*     01.00.000  9/22/91 baw   Written with CMACROS.INC
;*
;************************************************************************

NAME PAINT

.286p

;-------------------------------------------------------------------------
.xlist                                  ; suspend listing
?PLM=1                                  ; support Pascal calling
?WIN=1                                  ; support Windows
memM=1                                  ; medium model
include CMacros.Inc
include Windows.Inc
include Paint.Inc
.list
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
; dynalinked externals
;-------------------------------------------------------------------------

externNP        <_GetDblCharRect>
externNP        <_GetStdCharRect>

externFP        <BeginPaint>
externFP        <EndPaint>
externFP        <ExtTextOut>
externFP        <GetWindowWord>
externFP        <LocalLock>
externFP        <LocalUnlock>
externFP        <SelectObject>
externFP        <SetBkColor>
externFP        <SetTextColor>
externFP        <UnionRect>

;-------------------------------------------------------------------------
; data segment
;-------------------------------------------------------------------------

sBegin      DATA
            nLastFont       dw      0       ; Last font selected into DC
            bLastAttr       db      0       ; Last attribute use in paint
sEnd        DATA

assumes DS, DATA

;-------------------------------------------------------------------------
; segment definition
;-------------------------------------------------------------------------

createSeg SCREEN, SCREEN, BYTE, PUBLIC, CODE

sBegin  SCREEN
assumes CS, SCREEN

;************************************************************************
;*  BOOL FAR PASCAL PaintEmulator( HWND hWnd )
;*
;*  Description: 
;*     This function paints the current invalid rect.  It handles
;*     all attribute processing and performs output by determining
;*     the run length of common attribute strings.
;*
;*     Discovery: rects are not inclusive - this code computes the
;*     characters to be displayed by using a modified equation
;*     (subtracts 1 from top and right).
;*
;*  Comments:
;*      9/22/91  baw  Re-written in assembly for speed.
;*
;************************************************************************

cProc   PaintEmulator, <FAR, PUBLIC>, <es, di, si>
        ParmW   hWnd                                    ; hWnd

        LocalW  nRow                                    ; starting row
        LocalW  nCol                                    ; starting col
        LocalW  nEndRow                                 ; ending row
        LocalW  nEndCol                                 ; ending col
        LocalW  nMaxCol                                 ; max col per row
        LocalW  fFirstPass                              ; first pass flag
        LocalW  nRun                                    ; run counter
        LocalW  hDC                                     ; device context
        LocalW  hECB                                    ; ECB handle
        LocalV  psArea, %(size PAINTSTRUCT)             ; paint structure
cBegin

        cCall   GetWindowWord, <hWnd, GWW_ECB>          ; get ECB
        mov     hECB, ax                                ; store handle
        cCall   LocalLock, <hECB>                       ; lock structure
        or      ax, ax                                  ; Q: NULL?
        jnz     PE_StartPaint                           ;    N: continue
        jmp     PE_Exit                                 ;    Y: exit

PE_StartPaint:
        mov     di, ax                                  ; di = -> ECB
        push    hWnd                                    ; push parameters
        push    ss                                      ; for BeginPaint()
        lea     bx, psArea                              ; call
        push    bx
        cCall   BeginPaint                              ; do it
        mov     hDC, ax                                 ; hDC returned in AX

        ; nRow = min( MAXROWS - 1,
        ;             max( 0, (ps.rcPaint.top + YOFFSET) / YCHAR ) )

        lea     bx, psArea                              ; bx = -> psArea 
        mov     ax, word ptr ss:[bx.PSrcPaint.rcTop]    ; ax = ps.rcPaint.top
        add     ax, word ptr ds:[di.ecb_nOffsetY]       ; ax += YOFFSET
        xor     dx, dx                                  ; dx = 0
        idiv    word ptr ds:[di.ecb_nCharY]             ; ax /= YCHAR
        mov     cx, word ptr ds:[di.ecb_nRows]          ; cx = MAXROWS
        cmp     ax, cx                                  ; Q: ax > cx
        jbe     @F                                      ;    N: check ax > 0
        mov     ax, cx
        jmp     short PE_StoreRow       
@@:
        cmp     ax, 0                                   ; Q: ax > 0
        jae     PE_StoreRow                             ;    Y: store it
        xor     ax, ax                                  ; ax = 0

PE_StoreRow:
        mov     nRow, ax                                ; store start row

;       ; nEndRow = min( MAXROWS - 1, 
;       ;                ((ps.rcPaint.bottom + YOFFSET) / YCHAR) - 1 )

        ; nEndRow = min( MAXROWS - 1, 
        ;                ((ps.rcPaint.bottom + YOFFSET - 1) / YCHAR) )

        mov     ax, word ptr ss:[bx.PSrcPaint.rcBottom] ; ax = ps.rcPaint.bottom
        add     ax, word ptr ds:[di.ecb_nOffsetY]       ; ax += YOFFSET
        xor     dx, dx                                  ; dx = 0
        dec     ax                                      ; ax--
        idiv    word ptr ds:[di.ecb_nCharY]             ; ax /= YCHAR
;       dec     ax                                      ; ax--
        mov     cx, word ptr ds:[di.ecb_nRows]          ; cx = MAXROWS
        dec     cx                                      ; cx--
        cmp     ax, cx                                  ; Q: ax > cx
        jbe     PE_StoreEndRow                          ;    N: store it
        mov     ax, cx                                  ;    Y: ax = cx

PE_StoreEndRow:
        mov     nEndRow, ax                             ; store end row

        ; nCol = min( MAXCOLS - 1,
        ;             max( 0, (ps.rcPaint.left + XOFFSET) / XCHAR ) )

        mov     ax, word ptr ss:[bx.PSrcPaint.rcLeft]   ; ax = ps.rcPaint.left
        add     ax, word ptr ds:[di.ecb_nOffsetX]       ; ax += XOFFSET
        xor     dx, dx                                  ; dx = 0
        idiv    word ptr ds:[di.ecb_nCharX]             ; ax /= XCHAR
        mov     cx, word ptr ds:[di.ecb_nColumns]       ; cx = MAXCOLS
        cmp     ax, cx                                  ; Q: ax > cx
        jbe     @F                                      ;    N: check ax > 0
        mov     ax, cx                                  ;    Y: ax = cx
        jmp     short PE_StoreCol
@@:
        cmp     ax, 0                                   ; Q: ax > 0
        jae     PE_StoreCol                             ;    Y: store it
        xor     ax, ax                                  ; ax = 0

PE_StoreCol:
        mov     nCol, ax                                ; store start col

;       ; nEndCol = min( MAXCOLS - 1, 
;       ;                ((ps.rcPaint.right + XOFFSET) / XCHAR) - 1 )

        ; nEndCol = min( MAXCOLS - 1, 
        ;                ((ps.rcPaint.right + XOFFSET - 1) / XCHAR) )

        mov     ax, word ptr ss:[bx.PSrcPaint.rcRight]  ; ax = ps.rcPaint.right
        add     ax, word ptr ds:[di.ecb_nOffsetX]       ; ax += XOFFSET
        xor     dx, dx                                  ; dx = 0
        dec     ax                                      ; ax--
        idiv    word ptr ds:[di.ecb_nCharX]             ; ax /= XCHAR
;       dec     ax                                      ; ax--
        mov     cx, word ptr ds:[di.ecb_nColumns]       ; cx = MAXCOLS
        dec     cx                                      ; cx--
        cmp     ax, cx                                  ; Q: ax > cx
        jbe     PE_StoreEndCol                          ;    N: store it
        mov     ax, cx                                  ;    Y: ax = cx

PE_StoreEndCol:
        mov     nEndCol, ax                             ; store end col

;------------------------------------------------------------------------
; now, we compute the run for each common attribute per row
; and then call PaintRun()
;------------------------------------------------------------------------

        mov     fFirstPass, 1                           ; flag first pass
        les     bx, dword ptr ds:[di.ecb_lpAttrBuf]     ; es:bx = lpAttrBuf
        mov     cx, nRow                                ; cx = starting row

PE_RowLoop:
        cmp     cx, nEndRow                             ; Q: cx <= nEndRow
        jbe     PE_GetStartCol                          ;    Y: continue
        jmp     PE_CleanUp                              ; clean and exit

PE_GetStartCol:
        mov     dx, nCol                                ; dx = starting col
        mov     ax, nEndCol                             ; ax = nEndCol
        mov     nMaxCol, ax                             ; nMaxCol = ax

PE_ColLoop:
        cmp     dx, nMaxCol                             ; Q: dx <= nMaxCol
        jbe     PE_GetStartAttr                         ;    Y: continue
        inc     cx                                      ;    N: cur row++
        jmp     PE_RowLoop                              ;       loop

PE_GetStartAttr:
        mov     ax, cx                                  ; ax = cur row
        push    dx                                      ; save dx
        imul    word ptr ds:[di.ecb_nColumns]           ; ax *= MAXCOLS
        pop     dx                                      ; restore dx
        add     ax, dx                                  ; ax += current col
        mov     si, ax                                  ; si = ax
        mov     al, byte ptr es:[bx + si]               ; al <- es:[bx + si]
        test    al, ATTRIBUTE_DBLSIZE                   ; Q: Double size row?
        jz      PE_ComputeRun                           ;    N: continue
        push    ax                                      ; save ax
        mov     ax, nEndCol                             ; ax = nEndCol
        shr     ax, 1                                   ; ax /= 2
        mov     nMaxCol, ax                             ; nMaxCol = ax
        pop     ax                                      ; restore ax

PE_ComputeRun:
        mov     nRun, 0                                 ; run length = 0
        push    di                                      ; save di
        mov     di, dx                                  ; di = current col

PE_RunLoop:
        inc     nRun                                    ; nRun++
        inc     di                                      ; di++
        cmp     di, nMaxCol                             ; Q: di <= nMaxCol
        ja      PE_RunComplete                          ;    N: break
        inc     si                                      ; index++
        cmp     al, byte ptr es:[bx + si]               ; Q: same attr?
        je      PE_RunLoop                              ;    Y: loop

PE_RunComplete:
        pop     di                                      ; restore di

        push    di                                      ; ptr to ECB
        push    hDC                                     ; device context
        push    ax                                      ; attribute
        push    cx                                      ; start row
        push    dx                                      ; start col
        push    nRun                                    ; length
        push    fFirstPass                              ; first pass (reset)

        cCall   PaintRun
        
        cmp     fFirstPass, 1                           ; Q: fFirstPass?
        jne     PE_AddRunToPos                          ;    N: continue
        mov     fFirstPass, 0                           ;    Y: not FirstPass

PE_AddRunToPos:
        add     dx, nRun                                ; current col += nRun
        jmp     PE_ColLoop                              ; loop

PE_CleanUp:
        push    hWnd                                    ; push parameters
        push    ss                                      ; for EndPaint()
        lea     bx, psArea                              ; call
        push    bx
        cCall   EndPaint                                ; do it
        cCall   LocalUnlock, <hECB>                     ; unlock block
        mov     ax, 1

PE_Exit:

cEnd

;************************************************************************
;*  VOID NEAR PASCAL PaintRun( NPECB npECB, HDC hDC, BYTE bAttr,
;*                             int nRow, int nCol, int nRun,
;*                             BOOL fReset )
;*
;*  Description: 
;*     Paints the specified line with font and colors based
;*     on the given attribute.
;*
;*     Preserves: es, si, di, bx, cx, dx
;*     Trashes:   ax
;*
;*  Comments:
;*      9/22/91  baw  Written in assembly for speed.
;*
;************************************************************************

cProc   PaintRun, <NEAR, PUBLIC>, <es, di, si, bx, cx, dx>
        ParmW   npECB                                   ; near ptr to ECB
        ParmW   hDC                                     ; handle to DC
        ParmB   bAttr                                   ; attribute to use
        ParmW   nRow                                    ; starting row
        ParmW   nCol                                    ; starting column
        ParmW   nRun                                    ; length
        ParmW   fReset                                  ; reset font/attr

        LocalW  nHorzPos                                ; horizontal position
        LocalW  nVertPos                                ; vertical position
        LocalW  nFont                                   ; font selected
        LocalV  rcTemp, %(size RECT)                    ; temp rect
        LocalV  rcPaint, %(size RECT)                   ; rect to paint
cBegin
        mov     di, npECB                               ; npECB -> di

        mov     al, bAttr                               ; prepare param
        push    ax
        cCall   GetFontNumber                           ; get font in AX

        cmp     fReset, 1                               ; Q: Reset?
        je      PR_SelectFont                           ;    Y: Select font
        cmp     ax, nLastFont                           ; Q: AX == nLastFont
        je      PR_CheckAttr                            ;    Y: Check attr

PR_SelectFont:
        mov     nLastFont, ax
        mov     bx, ax
        push    hDC
        shl     bx, 1                                    ; multiply by 2
        push    WORD PTR ds:[di.ecb_ahEmulatorFonts][bx] ; get font
        cCall   SelectObject                             ; call SelectObject

PR_CheckAttr:
        cmp     fReset, 1                               ; Q: Reset?
        je      PR_SetAttribute                         ;    Y: Set attribute

        mov     cl, bAttr                               ; get attribute
        cmp     cl, bLastAttr                           ; Q: same as before?
        jne     PR_SetAttribute                         ;    N: Set attribute
        jmp     PR_GetRect                              ;    Y: Compute rects

PR_SetAttribute:
        mov     bLastAttr, cl                           ; store attribute
        test    cl, ATTRIBUTE_BLINKING                  ; Q: blinking?
        je      PR_NotBlinking                          ;    N: continue
        cmp     WORD PTR ds:[di.ecb_fBlinkState], 0     ; Q: blinked?
        jne     PR_Blinking                             ;    Y: set it
        jmp     PR_NotBlinking                          ;    N: continue

PR_Blinking:

        ; SetTextColor

        push    hDC
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfFGColor + 2]
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfFGcolor]
        cCall   SetTextColor

        ; SetBkColor

        push    hDC
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfBGcolor + 2]
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfBGcolor]
        cCall   SetBkColor

        jmp     PR_GetRect

PR_NotBlinking:
        test    cl, ATTRIBUTE_INVERSE
        je      PR_NotInverse

        test    cl, ATTRIBUTE_BOLD
        je      PR_InverseButNotBold

        ; SetTextColor

        push    hDC
        push    WORD PTR ds:[di.ecb_lfHighlightFont.elf_lfBGcolor + 2]
        push    WORD PTR ds:[di.ecb_lfHighlightFont.elf_lfBGcolor]
        cCall   SetTextColor

        ; SetBkColor

        push    hDC
        push    WORD PTR ds:[di.ecb_lfHighlightFont.elf_lfFGcolor + 2]
        push    WORD PTR ds:[di.ecb_lfHighlightFont.elf_lfFGcolor]
        cCall   SetBkColor

        jmp     PR_GetRect

PR_InverseButNotBold:

        ; SetTextColor

        push    hDC
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfBGcolor + 2]
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfBGcolor]
        cCall   SetTextColor

        ; SetBkColor

        push    hDC
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfFGcolor + 2]
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfFGcolor]
        cCall   SetBkColor

        jmp     PR_GetRect

PR_NotInverse:
        test    cl, ATTRIBUTE_BOLD
        je      PR_NotBold

        ; SetTextColor

        push    hDC
        push    WORD PTR ds:[di.ecb_lfHighlightFont.elf_lfFGcolor + 2]
        push    WORD PTR ds:[di.ecb_lfHighlightFont.elf_lfFGcolor]
        cCall   SetTextColor

        ; SetBkColor

        push    hDC
        push    WORD PTR ds:[di.ecb_lfHighlightFont.elf_lfBGcolor + 2]
        push    WORD PTR ds:[di.ecb_lfHighlightFont.elf_lfBGcolor]
        cCall   SetBkColor

        jmp     PR_GetRect

PR_NotBold:

        ; SetTextColor

        push    hDC
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfFGcolor + 2]
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfFGcolor]
        cCall   SetTextColor

        ; SetBkColor

        push    hDC
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfBGcolor + 2]
        push    WORD PTR ds:[di.ecb_lfNormalFont.elf_lfBGcolor]
        cCall   SetBkColor

PR_GetRect:
        mov     cl, bAttr                               ; reload attribute
        test    cl, ATTRIBUTE_DBLSIZE                   ; Q: Double size char        
        jne     PR_DblSize                              ;    Y: calc double
        jmp     PR_StdSize                              ;    N: calc normal

PR_DblSize:

        ; GetDblCharRect( npECB, &rcTemp, nCol, nRow )

        push    nRow                                    ; this is _cdecl -
        push    nCol                                    ; remember reverse
        push    ss                                      ; order parameters
        lea     bx, rcTemp
        push    bx
        push    npECB                                           
        cCall   _GetDblCharRect                         ; call it
        add     sp, 10                                  ; clean stack

        lea     bx, rcTemp
        mov     ax, word ptr ss:[bx.rcTop]              ; nVertPos =
        mov     nVertPos, ax                            ;   rcTemp.top
        mov     ax, word ptr ss:[bx.rcLeft]             ; nHorzPos =
        mov     nHorzPos, ax                            ;   rcTemp.left

        ; GetDblCharRect( npECB, &rcPaint, nCol + nRun - 1, nRow )

        push    nRow                                    ; this is _cdecl -
        mov     bx, nRun                                ; remember reverse
        add     bx, nCol                                ; order parameters
        dec     bx                                      ; bx--
        push    bx
        push    ss
        lea     bx, rcPaint
        push    bx
        push    npECB                                           
        cCall   _GetDblCharRect                         ; call it
        add     sp, 10                                  ; clean stack

        ; UnionRect( &rcPaint, &rcPaint, &rcTemp ) ;

        push    ss                                      ; dest rect
        lea     bx, rcPaint
        push    bx
        push    ss                                      ; src rect 1
        push    bx
        push    ss                                      ; src rect 2
        lea     bx, rcTemp
        push    bx
        cCall   UnionRect

        ; if double height bottom half, subtract height of standard
        ; character to get bottom half of double height character

        mov     cl, bAttr
        mov     ch, cl
        and     ch, ATTRIBUTE_DBLHEIGHTBTM
        cmp     cl, ATTRIBUTE_DBLHEIGHTBTM
        jne     PR_DblPaint

        mov     bx, nVertPos                            ; compute position
        sub     bx, ds:[di.ecb_nCharY]                  ; for bottom half
        mov     nVertPos, bx                            ; of character

PR_DblPaint:
        test    cl, ATTRIBUTE_BLINKING
        jz      PR_DblPaintNoBlink
        cmp     WORD PTR ds:[di.ecb_fBlinkState], 0     ; Q: blinked?
        jne     PR_DblPaintNoBlink

PR_DblClearPaintRect:
        push    hDC                                     ; device context
        push    nHorzPos                                ; horizontal position
        push    nVertPos                                ; vertical position
        push    (ETO_OPAQUE + ETO_CLIPPED)              ; options
        push    ss                                      ; paint rect segment
        lea     bx, rcPaint                             ; paint rect offset
        push    bx
        push    0                                       ; string segment
        push    0                                       ; string offset
        push    0                                       ; length
        push    0                                       ; lpDX ptr array seg
        push    0                                       ; lpDX ptr array ofs
        cCall   ExtTextOut
        jmp     PR_Exit

PR_DblPaintNoBlink:
        push    hDC                                     ; device context
        push    nHorzPos                                ; horizontal position
        push    nVertPos                                ; vertical position
        push    (ETO_OPAQUE + ETO_CLIPPED)              ; options
        push    ss                                      ; paint rect segment
        lea     bx, rcPaint                             ; paint rect offset
        push    bx
        mov     ax, nRow                                ; nRow *
        imul    word ptr ds:[di.ecb_nColumns]           ; MAXCOLS +
        add     ax, nCol                                ; nCol +
        add     ax, word ptr ds:[di.ecb_lpCharBuf]      ; buffer offset
        push    word ptr ds:[di.ecb_lpCharBuf + 2]      ; buffer segment
        push    ax                                      ; offset into buffer
        push    nRun                                    ; length
        push    0                                       ; lpDX ptr array seg
        push    0                                       ; lpDX ptr array ofs
        cCall   ExtTextOut
        jmp     PR_Exit

PR_StdSize:
        ; GetStdCharRect( npECB, &rcTemp, nCol, nRow )

        push    nRow                                    ; this is _cdecl -
        push    nCol                                    ; remember reverse
        push    ss                                      ; order parameters
        lea     bx, rcTemp
        push    bx
        push    npECB                                           
        cCall   _GetStdCharRect                         ; call it
        add     sp, 10                                  ; clean stack

        lea     bx, rcTemp
        mov     ax, word ptr ss:[bx.rcTop]              ; nVertPos =
        mov     nVertPos, ax                            ;   rcTemp.top
        mov     ax, word ptr ss:[bx.rcLeft]             ; nHorzPos =
        mov     nHorzPos, ax                            ;   rcTemp.left

        ; GetStdCharRect( npECB, &rcPaint, nCol + nRun - 1, nRow )

        push    nRow                                    ; this is _cdecl -
        mov     bx, nRun                                ; remember reverse
        add     bx, nCol                                ; order parameters
        dec     bx                                      ; bx--
        push    bx
        push    ss
        lea     bx, rcPaint
        push    bx
        push    npECB                                           
        cCall   _GetStdCharRect                         ; call it
        add     sp, 10                                  ; clean stack

        ; UnionRect( &rcPaint, &rcPaint, &rcTemp )

        push    ss                                      ; dest rect
        lea     bx, rcPaint
        push    bx
        push    ss                                      ; src rect 1
        push    bx
        push    ss                                      ; src rect 2
        lea     bx, rcTemp
        push    bx
        cCall   UnionRect

PR_StdPaint:
        mov     cl, bAttr
        test    cl, ATTRIBUTE_BLINKING
        jz      PR_StdPaintNoBlink
        cmp     WORD PTR ds:[di.ecb_fBlinkState], 0     ; Q: blinked?
        jne     PR_StdPaintNoBlink

PR_StdClearPaintRect:
        push    hDC                                     ; device context
        push    nHorzPos                                ; horizontal position
        push    nVertPos                                ; vertical position
        push    ETO_OPAQUE                              ; options
        push    ss                                      ; paint rect segment
        lea     bx, rcPaint                             ; paint rect offset
        push    bx
        push    0                                       ; string segment
        push    0                                       ; string offset
        push    0                                       ; length
        push    0                                       ; lpDX ptr array seg
        push    0                                       ; lpDX ptr array ofs
        cCall   ExtTextOut
        jmp     PR_Exit

PR_StdPaintNoBlink:
        push    hDC                                     ; device context
        push    nHorzPos                                ; horizontal position
        push    nVertPos                                ; vertical position
        push    ETO_OPAQUE                              ; options
        push    ss                                      ; paint rect segment
        lea     bx, rcPaint                             ; paint rect offset
        push    bx
        mov     ax, nRow                                ; nRow *
        imul    word ptr ds:[di.ecb_nColumns]           ; MAXCOLS +
        add     ax, nCol                                ; nCol +
        add     ax, word ptr ds:[di.ecb_lpCharBuf]      ; buffer offset
        push    word ptr ds:[di.ecb_lpCharBuf + 2]      ; buffer segment
        push    ax                                      ; offset into buffer
        push    nRun                                    ; length
        push    0                                       ; lpDX ptr array seg
        push    0                                       ; lpDX ptr array ofs
        cCall   ExtTextOut

PR_Exit:
        
cEnd

;************************************************************************
;*  int NEAR PASCAL GetFontNumber( BYTE bAttr )
;*
;*  Description: 
;*     Returns the font number associated with the character set
;*     and the attribute.
;*
;*  Comments:
;*      9/22/91  baw  Written in assembly for speed.
;*
;************************************************************************

cProc   GetFontNumber, <NEAR>
        ParmB   bAttr                                   ; attribute

cBegin
        mov     al, bAttr                               ; al = bAttr
        test    al, ATTRIBUTE_OEMCHARSET                ; Q: OEM charset?
        jnz     GFN_OEMCharSet                          ;    Y: continue
        jmp     GFN_NotOEM                              ;    N: outta here

GFN_OEMCharSet:
        test    al, ATTRIBUTE_UNDERLINE                 ; Q: Underline?
        jz      GFN_OEMNotUL                            ;    N: not this one
        test    al, ATTRIBUTE_DBLSIZE                   ; Q: DblSize?
        jz      GFN_OEMULNotDBL                         ;    N: nope
        mov     ah, al                                  ; ah = attribute
        and     ah, ATTRIBUTE_DBLSIZE                   ; ah &= DBLSIZE
        cmp     ah, ATTRIBUTE_DBLWIDTH                  ; Q: DBLWidth?
        jne     GFN_OEMDblHeightUL                      ;    N: we know then
        mov     ax, FONT_DBLWIDTHULOEM                  ; return font value
        jmp     GFN_Exit                                ; got it!

GFN_OEMDblHeightUL:
        mov     ax, FONT_DBLHEIGHTULOEM
        jmp     GFN_Exit                                ; got it!

GFN_OEMULNotDBL:
        mov     ax, FONT_UNDERLINEOEM
        jmp     GFN_Exit                                ; got it!

GFN_OEMNotUL:
        test    al, ATTRIBUTE_DBLSIZE                   ; Q: DblSize?
        jz      GFN_OEMNotULNotDBL                      ;    N: Nope
        mov     ah, al                                  ; ah = attribute
        and     ah, ATTRIBUTE_DBLSIZE                   ; ah &= DBLSIZE
        cmp     ah, ATTRIBUTE_DBLWIDTH                  ; Q: DblWidth?
        jne     GFN_OEMNotULDblHeight                   ;    N: we know then
        mov     ax, FONT_DBLWIDTHOEM                    ; ax = font
        jmp     GFN_Exit                                ; got it!

GFN_OEMNotULDblHeight:
        mov     ax, FONT_DBLHEIGHTOEM                   ; ax = font
        jmp     GFN_Exit                                ; got it!

GFN_OEMNotULNotDbl:
        mov     ax, FONT_STANDARDOEM                    ; ax = font
        jmp     GFN_Exit                                ; got it!
        
GFN_NotOEM:
        test    al, ATTRIBUTE_UNDERLINE                 ; Q: Underline?
        jz      GFN_NotUL                               ;    N: not this one
        test    al, ATTRIBUTE_DBLSIZE                   ; Q: DblSize?
        jz      GFN_ULNotDBL                            ;    N: nope
        mov     ah, al                                  ; ah = attribute
        and     ah, ATTRIBUTE_DBLSIZE                   ; ah &= DBLSIZE
        cmp     ah, ATTRIBUTE_DBLWIDTH                  ; Q: DBLWidth?
        jne     GFN_DblHeightUL                         ;    N: we know then
        mov     ax, FONT_DBLWIDTHUL                     ; return font value
        jmp     GFN_Exit                                ; got it!

GFN_DblHeightUL:
        mov     ax, FONT_DBLHEIGHTUL
        jmp     GFN_Exit                                ; got it!

GFN_ULNotDBL:
        mov     ax, FONT_UNDERLINE
        jmp     GFN_Exit                                ; got it!

GFN_NotUL:
        test    al, ATTRIBUTE_DBLSIZE                   ; Q: DblSize?
        jz      GFN_NotULNotDBL                         ;    N: Nope
        mov     ah, al                                  ; ah = attribute
        and     ah, ATTRIBUTE_DBLSIZE                   ; ah &= DBLSIZE
        cmp     ah, ATTRIBUTE_DBLWIDTH                  ; Q: DblWidth?
        jne     GFN_NotULDblHeight                      ;    N: we know then
        mov     ax, FONT_DBLWIDTH                       ; ax = font
        jmp     GFN_Exit                                ; got it!

GFN_NotULDblHeight:
        mov     ax, FONT_DBLHEIGHT                      ; ax = font
        jmp     GFN_Exit                                ; got it!

GFN_NotULNotDbl:
        mov     ax, FONT_STANDARD                       ; ax = font

GFN_Exit:

cEnd

sEnd    SCREEN

end                

