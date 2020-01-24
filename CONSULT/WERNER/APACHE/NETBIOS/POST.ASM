;************************************************************************
;*
;*    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
;*
;*-----------------------------------------------------------------------
;*
;*     Project:  APACHE, Apache For Windows
;*
;*      Module:  post.asm
;*
;*   Author(s):  Bryan A. Woodruff (baw)
;*               Charles E. Kindel, Jr. (cek)
;*
;*
;*     Remarks:  NetBIOS Communications DLL, NetBIOS interface
;*
;*   Revisions:  
;*     01.00.000  9/21/91 baw   PostNetBIOS written with CMACROS.INC
;*
;************************************************************************

NAME POST

.286p

;-------------------------------------------------------------------------
.xlist                                  ; suspend listing
?PLM=1                                  ; support Pascal calling
?WIN=1                                  ; support Windows
memM=1                                  ; medium model
include CMacros.Inc
include Windows.Inc
include Post.Inc
.list
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
; dynalinked externals
;-------------------------------------------------------------------------

externFP        <PostMessage>

;-------------------------------------------------------------------------
; segment definition
;-------------------------------------------------------------------------

createSeg POST, POST, BYTE, PUBLIC, CODE

sBegin  POST
assumes CS, POST
assumes DS, NOTHING

;************************************************************************
;*  VOID FAR PASCAL interrupt PostNetBIOS( VOID )
;*
;*  Description: 
;*     Interrupt handling routine for NCB POSTs.
;*
;*  Comments:
;*      9/21/91  baw  Wrote it.
;*
;************************************************************************

cProc   PostNetBIOS, <PUBLIC, FAR>
cBegin  nogen

        pusha           ; save all registers

        mov     al, word ptr es:[bx.ncb_bCommand]       ; get NCB command
        and     al, 7Fh                                 ; strip off ASYNC bit
        cmp     al, NBC_CALL_NIU                        ; Q: CALL?
        jne     PNB_TestListen                          ;    N: Test listen
        cmp     al, NBC_CALL                            ; Q: CALL?
        jne     PNB_TestListen                          ;    N: Test listem
        mov     cx, CE_CONNECT                          ;    Y: CONNECT ntfy
        jmp     PNB_PostMessage                         ; Post notification

PNB_TestListen:
        cmp     al, NBC_LISTEN                          ; Q: LISTEN?
        jne     PNB_TestReceive                         ;    N: Test receive
        mov     cx, CE_CONNECT                          ;    Y: CONNECT ntfy
        jmp     PNB_PostMessage                         ; Post notification

PNB_TestReceive:
        cmp     al, NBC_RECEIVE                         ; Q: RECEIVE?
        jne     PNB_DefaultCmd                          ;    N: Test datagram
        cmp     al, NBC_RECEIVE_ANY                     ; Q: RECEIVE?
        jne     PNB_DefaultCmd                          ;    N: Test datagram
        mov     cx, CE_RECEIVE                          ;    Y: NBDATA ntfy
        jmp     PNB_PostMessage                         ; Post notification

PNB_Default:
        mov     cx, CE_DEFAULT                          ; Default

PNB_PostMessage:

        ; PostMessage( lpXNCB -> hWnd, WM_CONNECTOR_EVENT, wEvent,
        ;              (LONG) lpXNCB -> hNCB ) ;

        push    word ptr es:[bx.xncb_hWnd]
        push    WM_CONNECTOR_EVENT                      ; event notification
        push    cx                                      ; event type
        push    0
        push    word ptr es:[bx.xncb_hNCB]              ; handle to NCB

        cCall   PostMessage                             ; do it

        popa            ; restore all registers

        iret            ; return from interrupt

cEnd    nogen
sEnd    POST

end                
