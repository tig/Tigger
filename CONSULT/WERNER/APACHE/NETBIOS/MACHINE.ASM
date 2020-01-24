        title   machine.asm

;********************************************************************
;*
;*    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
;*
;*-------------------------------------------------------------------
;*
;*     Project:  APACHE, Apache For Windows
;*
;*      Module:  machine.asm
;*
;*   Author(s):  Bryan A. Woodruff (baw)
;*               Charles E. Kindel, Jr. (cek)
;*
;*     Remarks:  NetBIOS - GetMachineName() function
;*
;*   Revisions:
;*     01.00.000  3/ 6/91 baw   Initial version, initial build
;*
;********************************************************************

?WIN = 1

.xlist
        include cmacros.inc
.list

sBegin  DATA

sEnd    DATA

sBegin  CODE

        assumes cs,code
        assumes ds,data
        
cProc   GetMachineName, <PUBLIC, FAR, PASCAL>, <ds>
        ParmD   lpBuffer
cBegin 
        lds     dx, lpBuffer    ; ds:dx <- lpBuffer
        mov     ax, 5E00h       ; get machine name
        int     21h
        jnc     GMNexit         ; return cx on success
        xor     cx, cx          ; return failure

GMNexit:
        mov     ax, cx          ; return CX

cEnd    GetMachineName

sEnd    CODE

end



