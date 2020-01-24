;       Static Name Aliases
;
         TITLE   test
;        NAME    test.C

public   _post_rcv

STACKFRAME EQU   200                        ; each stack frame has 200 bytes

lsnumoff   EQU   595                        ; isr_glob.lsnum offset


_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS

_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS

CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS

_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS

STACK           SEGMENT
        public  topstck
        public  btmstck

btmstck DB      2000 DUP (?)
topstck LABEL   WORD
STACK           ENDS

DGROUP  GROUP   CONST,   _BSS,   _DATA
        ASSUME  CS: _TEXT, DS: DGROUP, SS: STACK, ES: DGROUP

EXTRN   _isr_glob:BYTE
EXTRN	_nb_isr:FAR

; These constants are offsets into isr_glob for global structures
CONST   SEGMENT
CONST   ENDS

_TEXT   SEGMENT
        PUBLIC   _post
_post   PROC FAR
        cli
        push    bp
        mov     bp,sp
        push    ax
        push    dx
        push    ds
        push    bx
        push    si
        push    di
        push    cx
        push    es
        push    ss
        pushf

        mov     ax, seg _isr_glob
        mov     ds, ax

        mov     al,BYTE Ptr es:[bx]
        cmp     al,91h
        jnz     next1
        mov     WORD PTR _isr_glob+4,es
        mov     WORD PTR _isr_glob+2,bx
        mov     al, es:[bx+2]
        mov     si, OFFSET _isr_glob        ; get offset of isr_glob for si
        add     si, lsnumoff                ; lsnum is ds:di
        mov     ds:[si], al                 ; put lsnum in ds:si
        jmp     done
next1:  mov     WORD PTR _isr_glob+8,es
        mov     WORD PTR _isr_glob+6,bx
done:   popf
        pop     ss
        pop     es
        pop     cx
        pop     di
        pop     si
        pop     bx
        pop     ds
        pop     dx
        pop     ax
        mov     sp,bp
        pop     bp
        sti
        iret
_post   ENDP
_TEXT   ENDS


_TEXT   SEGMENT
        PUBLIC   _post_rcv

busy    dw      0

_post_rcv   PROC FAR

        cli
        inc     cs:busy                     ; let us know if re-entrant

        push    bp
        mov     bp,sp
        push    ax
        push    dx
        push    ds
        push    bx
        push    si
        push    di
        push    cx
        push    es
        push    bx


; save off the return code
go_on:
        mov     si, 0
        mov     si, ax

; now call nb_isr

        mov     ax, ss                      ; save old stack pointers
        mov     ds, ax                      ; in temp storage
        mov     dx, sp
        mov     di, bp

        mov     ax, STACK                   ; create new stack
        mov     ss, ax                      ; get the new stack segment
        lea     cx, STACK:topstck           ; get the new stack offset
        mov     ax, STACKFRAME              ; get the new stack frame
        mul     cs:busy
        sub     cx, ax
        mov     sp, cx                      ; sp is new stack frame offset
        mov     bp, cx                      ; bp is new stack frame offset

        push    ds                          ; save old stack pointers
        push    dx                          ; onto the new safe stack
        push    di


        push    si                          ; return code (origanally al)
        push    es                          ; pointer to NCB (segment)
        push    bx                          ; pointer to NCB (offset)

        mov     ax, seg _isr_glob           ; reset ds because it might be
        mov     ds, ax                      ; messed up

	call	FAR PTR _nb_isr             ; issue another receive
        add     sp, 6                       ; remove parms from the stack

        cmp     dx, 0                       ; if es:[bx] == NULL
        je      old_stack                   ;     do not do int 5Ch
        cmp     ax, 0
        je      old_stack

        mov     bx, ax                      ; get returned pointer
        mov     es, dx
        int     5Ch                         ; call netbios again

old_stack:
        pop     ax                          ; pop off old stack info
        pop     bx
        pop     cx

        mov     bp, ax                      ; restore old stack info
        mov     sp, bx
        mov     ss, cx


; now exit
        pop     bx
        pop     es
        pop     cx
        pop     di
        pop     si
        pop     bx
        pop     ds
        pop     dx
        pop     ax
        mov     sp,bp
        pop     bp

        dec     cs:busy
        test    cs:busy, 0
        jnz     ending
        sti
ending:
        iret

_post_rcv   ENDP
_TEXT   ENDS


END
