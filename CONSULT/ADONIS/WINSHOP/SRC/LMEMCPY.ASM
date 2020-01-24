;*************************************************************\
; ** FILE:  lmemcpy.asm
; **
; ** COPYRIGHT (c) 1988-89 Mechatronix International, Inc.
; **
; ** DESCRIPTION:
; **
; ** HISTORY:   
; **
;*************************************************************/


_TEXT	SEGMENT		PUBLIC		'CODE'

	ASSUME	CS:_TEXT


PUBLIC	_lmemcpy

lmemcpy_Struct		STRUC
	lmemcpy_BP		DW	?
	lmemcpy_RA		DD	?
	lmemcpy_DestPtr		DD	?
	lmemcpy_SrcPtr		DD	?
	lmemcpy_BCnt		DW	?
lmemcpy_Struct		ENDS

;-------------------------------------------------------------
; lmemcpy:   
; 
;   Input:  Far pointers to two arrays and the length
;           of the strings (expressed in bytes).
;
;   Output: Copies the contents of the source array into the
;           the destination array.
;           
;     

_lmemcpy	PROC	FAR
	
	CLD
	PUSH	BP                              ; save c environ
	MOV	BP, SP
	PUSH	DS
	PUSH	ES
	PUSH	SI
	PUSH	DI

        XOR     AX, AX                          ; assume count multiple of
                                                ; two

	LDS	SI, [BP].lmemcpy_SrcPtr		; ds:si = source
	LES	DI, [BP].lmemcpy_DestPtr		; es:di = dest
	MOV	CX, [BP].lmemcpy_BCnt		; cx = count in bytes

        SHR     CX, 1                           ; convert byte count to
                                                ; word count
        JC      one_more                        ; if carry, not multiple of
                                                ; two

memcpy_words:        
        REP	MOVSW                           ; move the words
        OR      AX, AX                          ; if flag not set 
        JZ      memcpy_exit                     ;   done
        MOVSB                                   ; else 
                                                ;   move last one
memcpy_exit:
	POP	DI
	POP	SI
	POP	ES
	POP	DS
	POP	BP
	RET

one_more:
        OR      AX, 1
        JMP     memcpy_words


_lmemcpy	ENDP

_TEXT   ENDS        
        END
