	page	60,132
	title	Network Communications Adapter 8250 I/O operations
	
;*******************************************************************
;        Copyright 1987 J & L Information Systems, Inc.		   *
;                       (c) All Rights Reserved                    *
;*******************************************************************
; File name    : c14lib.asm
; Function name: _INITPT14
;		 _RCVCHR14RDY
;		 _RCVCHR14
;		 _SENDBRK14
;		 _SETFLOW14
;		 _CHKSTAT14
;		 _SNDCHR14
;		 _SETVEC
;		 _RSETVEC
; Version      : 
; Date         : Monday, March 13, 1989  14:55:37
; Written by   : Rex Jackson
; Modified by  :
; Last update  : 
; Description  : Assembler routines for 'C' calling programs
; Parameters   : 
; Comments     :
; SPF          :-----Software Problem Fixes-----
;
;	things to do...
;
;******************************************************************/

	public	_INITPT14
	public	_RCVCHR14RDY
	public	_RCVCHR14
	public	_SENDBRK14
	public	_SETFLOW14
	public	_CHKSTAT14
	public	_SNDCHR14
	public	_SETVEC
	public	_RSETVEC

.MODEL MEDIUM
.CODE
;
;	int  PARMS   = 0x83;	/* 1200,n,8,1				*/
;	int  PORT    = 0;	/* can be 0-3				*/
;
;	/* INIT PORT FROM C PGM VIA INT14 */
;	INITPT14(PORT, PARMS);
;
_INITPT14	PROC	FAR
	PUSH	BP
	MOV	BP,SP
	PUSH	DI
	PUSH	SI
	MOV	DX,[BP+6]	; port
	MOV	AX,[BP+8]	; PARM VALUES
	MOV	AH,0
	INT	14H
	POP	SI
	POP	DI
	POP	BP
	RET
_INITPT14	ENDP
;
;	int  PORT    = 0;	/* can be 0-3				*/
;	int  CHR     = 0;
;	int  RSTAT14 = 0;
;
;	/* RECEIVE CHAR READY FROM C PGM VIA INT14 */
;
;	{	if (RCVCHR14RDY	(PORT))
;		{	if (!RCVCHR14(PORT,&CHAR,&RSTAT14))
;				return(ERROR);	/* SEND <R> FOR REC ERROR */
;		}
;	}
;	return (TRUE);
;
_RCVCHR14RDY	PROC	FAR
	PUSH	BP
	MOV	BP,SP
	PUSH	DI
	PUSH	SI
	MOV	DX,[BP+6]	; port
	MOV	AH,3
	INT	14H
	TEST	AH,01		;DATA RDY ?
	MOV	AX,-1		;SETUP FOR YES EXIT
	JNZ	CHAR_RDY
	xor	ax,ax		;NO CHAR
CHAR_RDY:
	POP	SI
	POP	DI
	POP	BP
	RET
_RCVCHR14RDY	ENDP

_RCVCHR14	PROC	FAR
	PUSH	BP
	MOV	BP,SP
	PUSH	DI
	PUSH	SI
	MOV	DX,[BP+6]	; port
	MOV	AH,2
	INT	14H
	mov	di,word ptr [bp+8]
	stosb
	mov	di,word ptr [bp+10]
	xchg	al,ah
	stosb
	and	al,al		;ANY ERRORS ?
	MOV	AX,-1		;SETUP FOR NO & EXIT
	JZ	RCV_GOOD
	xor	ax,ax		;ERRORS OR TIME-OUT
RCV_GOOD:
	POP	SI
	POP	DI
	POP	BP
	RET
_RCVCHR14	ENDP


;	/* SEND BREAK FROM C PGM VIA INT14 */
;
;	int  PORT    = 0;	/* can be 0-3				*/
;
;	{	SENDBRK14(PORT);
;	}
;
_SENDBRK14	PROC	FAR
	PUSH	BP
	MOV	BP,SP
	PUSH	DI
	PUSH	SI
	MOV	DX,[BP+6]	; port
	MOV	AH,0FAH		;SEND BREAK
	INT	14H
	POP	SI
	POP	DI
	POP	BP
	RET
_SENDBRK14	ENDP

;
;	/* SET FLOW FROM C PGM VIA INT14 */
;
;	int  PORT    = 0;	/* can be 0-3				*/
;	int  FLOW    = 0;	/* 0x01 = set dtr high			*/
;				/* 0x02 = set rts high			*/
;				/* 0x10 = check for cts before sending	*/
;				/* 0x20 = check for dsr before sending	*/
;
;	{	SETFLOW14(PORT, FLOW);
;	}
;
_SETFLOW14	PROC	FAR
	PUSH	BP
	MOV	BP,SP
	PUSH	DI
	PUSH	SI
	MOV	DX,[BP+6]	; port
	MOV	AX,[BP+8]	;SET/RESET DTR/RTS; CHK CTS/DSR
	MOV	AH,0FBH
	INT	14H
	POP	SI
	POP	DI
	POP	BP
	RET
_SETFLOW14	ENDP


;	/* CHECK STATUS OF INT14 FROM C PGM */
;
;	int  PORT    = 0;	/* can be 0-3				*/
;	int  MSTAT14 = 0;	/* modem status				*/
;	int  LSTAT14 = 0;	/* line status				*/
;
;	{	CHKSTAT14(PORT,&MSTAT14,&LSTAT14);
;	}
;
_CHKSTAT14	PROC	FAR
	PUSH	BP
	MOV	BP,SP
	PUSH	DI
	PUSH	SI
	MOV	DX,[BP+6]	; port
	MOV	AH,3
	INT	14H
	mov	di,word ptr [bp+8]	; modem status
	stosb
	mov	di,word ptr [bp+10]	; line status
	xchg	al,ah
	stosb
	POP	SI
	POP	DI
	POP	BP
	RET
_CHKSTAT14	ENDP


;	/* SEND CHAR FROM C PGM VIA INT14 */
;
;	int  PORT    = 0;	/* can be 0-3				*/
;	int  CHR     = 0;	/* output character			*/
;	int  TIMEOUT = 0;	/* timeout loop count (HALF SEC INCS)	*/
;
;	{	if (!SNDCHR14(PORT,CHR,TIMEOUT))
;			return	(ERROR);	/* SEND <X> FOR TIME-OUT */
;	}
;	return	(TRUE);
;
;----------------------------------------------------------------------------
;
; >>>	if the routine would let the 'c' program do the timeouts...
;
;	int  PORT    = 0;	/* can be 0-3				*/
;	int  CHR     = 0;	/* output character			*/
;	unsigned int	i;
;
;	i = 65000;
;	while (i--)
;	{	if (SNDCHR14(PORT,CHR))
;			i = 0;
;	}
;
;----------------------------------------------------------------------------
;
_SNDCHR14	PROC	FAR
	PUSH	BP
	MOV	BP,SP
	PUSH	DI
	PUSH	SI
	MOV	DX,[BP+6]	; port
	MOV	CX,[BP+10]	; TIME-OUT VALUE
	INC	CX		; MAKE SURE AT LEAST 1
THRNE:
	push	cx
	xor	cx,cx		; 65536 iterations
THRNE_001:
	MOV	AH,3
	INT	14H
	TEST	AH,20H		;THR EMPTY ?
	JNZ	THRE		;YES
	LOOP	THRNE_001	;LOOP UNTIL TIME-OUT
	pop	cx
	LOOP	THRNE		;LOOP UNTIL TIME-OUT
	JMP 	SHORT SND_BAD
THRE:
	add	sp,2		; adjust stack
	MOV	AX,[BP+8]	;CHAR TO SEND
	MOV	AH,1
	INT	14H
	TEST	AH,80H		;TIME-OUT ? (MODEM LINES NOT READY) ?
	MOV	AX,-1		;SETUP FOR SEND OK
	JZ	SND_GOOD
SND_BAD:
	xor	ax,ax		;
SND_GOOD:
	POP	SI
	POP	DI
	POP	BP
	RET
_SNDCHR14	ENDP

;	int  VSEGMENT   = PREVIOUS SEGMENT */
;	int  VOFFSET    = PREVIOUS OFFSET  */
;	int  ISRSEG     = NEW SEGMENT */
;	int  ISROFF     = NEW OFFSET  */
;	int  INTVEC     = INT VECTOR HEX NUMBER */
;
;	/* SAVE INT 'XX' OLD VECTORS AND POINT TO ISR FROM C PGM */
;	SETVEC(INTVEC, ISRSEG, ISROFF, &VSEGMENT, &VOFFSET);
;
_SETVEC		PROC	FAR
	CLI
	PUSH	BP
	MOV	BP,SP
	PUSH	ES
	PUSH	DS
	MOV	AX,[BP+6]	; INTVEC
	MOV	AH,35H		; GETVEC
	INT	21H
	MOV WORD PTR [BP+12],ES ; VSEGMENT
	MOV WORD PTR [BP+14],BX ; VOFFSET
	MOV	AX,[BP+8]	; ISRSEG
	MOV	DS,AX
	MOV	DX,[BP+10]	; ISROFF
	MOV	AX,[BP+6]	; SETVEC
	MOV	AH,25H
	INT	21H
	POP	DS
	POP	ES
	POP	BP
	STI
	RET
_SETVEC		ENDP
;
;	int  VSEGMENT   = PREVIOUS SEGMENT */
;	int  VOFFSET    = PREVIOUS OFFSET  */
;	int  INTVEC     = INT VECTOR HEX NUMBER */
;
;	/* RESTORE INT 'XX' OLD VECTORS FROM C PGM */
;	RSETVEC(INTVEC, VSEGMENT, VOFFSET);
;
_RSETVEC	PROC	FAR
	CLI
	PUSH	BP
	MOV	BP,SP
	PUSH	ES
	PUSH	DS
	MOV	AX,[BP+8]	; ISRSEG
	MOV	DS,AX
	MOV	DX,[BP+10]	; ISROFF
	MOV	AX,[BP+6]	; SETVEC
	MOV	AH,25H
	INT	21H
	POP	DS
	POP	ES
	POP	BP
	STI
	RET
_RSETVEC	ENDP
;

	end

