;************************************************************************
;*
;*    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
;*
;*-----------------------------------------------------------------------
;*
;*     Project:  APACHE, Apache For Windows
;*
;*      Module:  post.inc
;*
;*   Author(s):  Bryan A. Woodruff (baw)
;*               Charles E. Kindel, Jr. (cek)
;*
;*
;*     Remarks:  PostNetBIOS NCB and XNCB definitions
;*
;*   Revisions:  
;*     01.00.000  9/21/91 baw   Wrote it.
;*
;************************************************************************

NCB     STRUC

        ncb_bCommand    db      ?
        ncb_bReturn     db      ?
        ncb_bLSN        db      ?
        ncb_bNumber     db      ?
        ncb_lpBuffer    dd      ?
        ncb_wLength     dw      ?
        ncb_abCallName  db      16 dup (?)
        ncb_abName      db      16 dup (?)
        ncb_bRTO        db      ?
        ncb_bSTO        db      ?
        ncb_lpPostFunc  dd      ?
        ncb_bLanaNum    db      ?
        ncb_bCmdCmplt   db      ?
        ncb_abReserved  db      14 dup (?)

NCB     ENDS

XNCB    STRUC

        xncb_NCB        db      size NCB dup (?)
        xncb_hWnd       dw      ?
        xncb_hNCB       dw      ?
        xncb_lpPostFunc dd      ?
        xncb_bLana      db      ?
        xncb_bAsync     db      ?

XNCB    ENDS

WM_CONNECTOR_EVENT      EQU     WM_USER + 1000h
CE_RXBLK                EQU     100h
CE_TXEMPTY              EQU     101h
CE_CONNECT              EQU     102h
CE_DISCONNECT           EQU     103h

NBC_CALL                EQU	10h
NBC_LISTEN              EQU	11h
NBC_HANG_UP             EQU	12h
NBC_SEND                EQU	14h
NBC_RECEIVE             EQU	15h
NBC_RECEIVE_ANY         EQU	16h
NBC_CHAIN_SEND          EQU	17h
NBC_SEND_DATAGRAM       EQU	20h
NBC_SEND_BCST_DATAGRAM  EQU	22h
NBC_RCV_DATAGRAM        EQU	21h
NBC_RCV_BCST_DATAGRAM   EQU	23h
NBC_ADD_NAME            EQU	30h
NBC_DELETE_NAME         EQU	31h
NBC_RESET               EQU	32h
NBC_ADAPTER_STATUS      EQU	33h
NBC_SESSION_STATUS      EQU	34h
NBC_CANCEL              EQU	35h
NBC_ADD_GROUP_NAME      EQU	36h
NBC_UNLINK              EQU	70h
NBC_SEND_NO_ACK         EQU	71h
NBC_CHAIN_SEND_NO_ACK   EQU	72h
NBC_CALL_NIU            EQU	74h    ; Ungerman-Bass only       
NBC_SEND_PKTU           EQU	77h    ; Ungerman-Bass only       
NBC_RECEIVE_PKTU        EQU	78h    ; Ungerman-Bass only       
NBC_TRACE               EQU	79h
NBC_INVALID_COMMAND     EQU	7Fh    ; for presence test        
NBC_ASYNC               EQU     80h
NBC_NO_WAIT             EQU	80h

;************************************************************************
;* End of File: post.inc
;************************************************************************

