;************************************************************************
;*
;*    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
;*
;*-----------------------------------------------------------------------
;*
;*     Project:  APACHE, Apache For Windows
;*
;*      Module:  invoke.asm
;*
;*   Author(s):  Bryan A. Woodruff (baw)
;*               Charles E. Kindel, Jr. (cek)
;*
;*
;*     Remarks:  NetBIOS Communications DLL, NetBIOS interface
;*
;*   Revisions:  
;*     01.00.000  9/21/91 baw   Invoke written with CMACROS.INC
;*
;************************************************************************

NAME INVOKE

.286p

;-------------------------------------------------------------------------
.xlist                                  ; suspend listing
?PLM=1                                  ; support Pascal calling
?WIN=1                                  ; support Windows
memM=1                                  ; medium model
include CMacros.Inc
include Windows.Inc
.list
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
; dynalinked externals
;-------------------------------------------------------------------------

externFP        <NetBIOSCall>

;-------------------------------------------------------------------------
; segment definition
;-------------------------------------------------------------------------

createSeg INVOKE, INVOKE, BYTE, PUBLIC, CODE

sBegin  INVOKE
assumes CS, INVOKE
assumes DS, NOTHING

;************************************************************************
;*  WORD FAR PASCAL InvokeNetBIOS( LPNCB lpNCB )
;*
;*  Description: 
;*     Invokes the NetBIOSCall command.  NetBIOSCall is used to
;*     provide the entry point for compatibility layers.
;*
;*  Comments:
;*      9/21/91  baw  Wrote it.
;*
;************************************************************************

cProc   InvokeNetBIOS, <PUBLIC, FAR>, <es, bx>
        parmD   lpNCB
cBegin
        mov     es, word ptr lpNCB + 2
        mov     bx, word ptr lpNCB

        call    NetBIOSCall

        xor     ah, ah
        mov     al, byte ptr es:[bx+1]  ; return code

cEnd    NetBIOSCall
sEnd    INVOKE

end                

