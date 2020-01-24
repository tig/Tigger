;************************************************************************
;*
;*    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
;*
;*-----------------------------------------------------------------------
;*
;*     Project:  APACHE, Apache For Windows
;*
;*      Module:  handler.inc
;*
;*   Author(s):  Bryan A. Woodruff (baw)
;*               Charles E. Kindel, Jr. (cek)
;*
;*
;*     Remarks:  System Timer Handler EVENT structure
;*
;*   Revisions:  
;*     01.00.000  9/22/91 baw   Wrote it.
;*
;************************************************************************

EVENTID STRUC

        event_nCID              dw      ?       ; nCid
        event_fPostFlag         dw      ?       ; notification posted flag
        event_hWnd              dw      ?       ; receiving window
        event_wEvent            dw      ?       ; event posted to window
        event_wEventMask        dw      ?       ; used for CommEventMask()
        event_lpEventPtr        dd      ?       ; event pointer

EVENTID ENDS

WM_CONNECTOR_NOTIFY     EQU     044h
IFNDEF CN_EVENT
CN_EVENT                EQU     04h
ENDIF
CN_RXBLK                EQU     100h
CN_TXEMPTY              EQU     101h
CN_CONNECT              EQU     102h
CN_DISCONNECT           EQU     103h

MAXEVENTS               EQU     10h

;************************************************************************
;* End of File: handler.inc
;************************************************************************


