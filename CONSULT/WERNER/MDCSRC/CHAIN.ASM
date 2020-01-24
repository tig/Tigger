	page	60,80
	title	chaining routines for int 1CH

	public	_CHAIN
        public  _LINKUP
        public  _DELINK

;       These routines are used for chaining INT 1CH isr's together


.MODEL MEDIUM
.CODE
;
;
;	CHAIN ();
;

_CHAIN  PROC	FAR
	PUSH	BP
	MOV	BP,SP
        push    bx

        mov     bx, WORD PTR [bp+6]
        pushf
        call    dword ptr ds:[bx]      ; specific for medium memory model

        pop     bx
	POP	BP
	RET
_CHAIN  ENDP
;
;
;	LINKUP ();
;
_LINKUP PROC    FAR
	PUSH	BP
	MOV	BP,SP
        push    ax
        push    bx
        push    dx
        push    es

        mov     ah, 35h                  ; get value for Old_Clock
        mov     al, 1Ch
        int     21h
        mov     ax, es
        mov     dx, bx

	mov	bx, WORD PTR [bp+6]      ; this is pointer to Old_Clock
	mov	WORD PTR [bx+2], ax
	mov	WORD PTR [bx],   dx

        pop     es
        pop     dx
        pop     bx
        pop     ax
	POP	BP
	RET
_LINKUP ENDP

_DELINK PROC    FAR
        PUSH    BP
        MOV     BP,SP
        push    ax
        push    bx
        push    dx
        push    ds

        mov     ah, 25h
        mov     al, 1Ch
	mov	bx, WORD PTR [bp+6]      ; this is pointer to Old_Clock
	mov	dx, WORD PTR [bx]
	mov	ds, WORD PTR [bx+2]
        int     21h

        pop     ds
        pop     dx
        pop     bx
        pop     ax
        POP     BP
        RET
_DELINK ENDP

	end
