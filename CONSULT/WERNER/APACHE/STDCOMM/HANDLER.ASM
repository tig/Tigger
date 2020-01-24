;************************************************************************
;*
;*    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
;*
;*-----------------------------------------------------------------------
;*
;*     Project:  APACHE, Apache For Windows
;*
;*      Module:  handler.asm
;*
;*   Author(s):  Bryan A. Woodruff (baw)
;*               Charles E. Kindel, Jr. (cek)
;*
;*
;*     Remarks:  System Timer Handler for STDCOMM
;*
;*   Revisions:  
;*     01.00.000  9/22/91 baw   Written with CMACROS.INC
;*
;************************************************************************

NAME HANDLER

.286p

;-------------------------------------------------------------------------
.xlist                                  ; suspend listing
?PLM=1                                  ; support Pascal calling
?WIN=1                                  ; support Windows
memM=1                                  ; medium model
WIN31=1                                 ; Windows 3.1
include CMacros.Inc
include Windows.Inc
include Handler.Inc
.list
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
; dynalinked externals
;-------------------------------------------------------------------------

externFP        <PostMessage>
externB         <_Events>

;-------------------------------------------------------------------------
; segment definition
;-------------------------------------------------------------------------

createSeg HANDLER, HANDLER, BYTE, PUBLIC, CODE

sBegin  HANDLER
assumes CS, HANDLER
assumes DS, NOTHING

;************************************************************************
;*  VOID FAR PASCAL TimerHandler( VOID )
;*
;*  Description: 
;*     This is the handler function for the System Timer Event
;*     Manager.
;*
;*  Comments:
;*      9/21/91  baw  Wrote it.
;*
;************************************************************************

cProc   TimerHandler, <PUBLIC, FAR>, <es, di, bx, cx>
cBegin
        xor     cx, cx                                  ; clear counter
TH_Loop:
        mov     di, cx                                  ; calculate offset
        imul    di, size EVENTID                        ; into Events
        add     di, OFFSET _Events                     ; array

        cmp     WORD PTR ds:[di.event_hWnd], 0          ; Q: hWnd NULL?
        jne     TH_CheckPostFlag                        ;    N: continue
        jmp     TH_NextEvent                            ;    Y: next event

TH_CheckPostFlag:
        cmp     WORD PTR ds:[di.event_fPostFlag], 0     ; Q: Already post?
        je      TH_TestEventMask                        ;    N: continue
        jmp     TH_NextEvent                            ;    Y: next event

TH_TestEventMask:
        les     bx, DWORD PTR ds:[di.event_lpEventPtr]  ; mask ptr -> ES:BX
        mov     ax, WORD PTR es:[bx]                    ; ax <- EventMask
        test    ax, WORD PTR ds:[di.event_wEventMask]   ; Q: Event flagged?
        jne     TH_PostNotification                     ;    Y: Notify Wnd
        jmp     TH_NextEvent                            ;    N: next event

TH_PostNotification:

        ; PostMessage( pEvent -> hWnd, WM_CONNECTOR_NOTIFY,
        ;              pEvent -> nCid, *(pEvent -> lpEventPtr) ) ;

        push    cx                                      ; cx gets trashed

        push    word ptr ds:[di.event_hWnd]
        push    WM_CONNECTOR_NOTIFY
        push    word ptr ds:[di.event_nCid]
        push    CN_EVENT
        push    ax

        cCall   PostMessage                             ; Post the message

        pop     cx                                      ; restore count
        mov     word ptr ds:[di.event_fPostFlag], 1     ; indicate post status

TH_NextEvent:
        inc     cx                                      ; increment count
        cmp     cx, MAXEVENTS                           ; Q: cx < MAXEVENTS
        je      TH_Exit                                 ;    N: Exit
        jmp     TH_Loop                                 ;    Y: Loop

TH_Exit:

cEnd
sEnd    HANDLER

end                
