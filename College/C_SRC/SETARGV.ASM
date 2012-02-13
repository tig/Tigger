        NAME    setargv
        PAGE    60,132
;[]------------------------------------------------------------[]
;|      SETARGV.ASM -- Parse Command Line                       |
;|                                                              |
;|      Turbo-C Run Time Library        version 1.0             |
;|                                                              |
;|      Copyright (c) 1987 by Borland International Inc.        |
;|      All Rights Reserved.                                    |
;[]------------------------------------------------------------[]

        INCLUDE RULES.ASI

;       Segment and Group declarations

Header@

;       External references

ExtSym@         _argc, WORD, __CDECL__
dPtrExt@        _argv, __CDECL__
ExtSym@         _psp, WORD, __CDECL__
ExtSym@         _envseg, WORD, __CDECL__
ExtSym@         _envLng, WORD, __CDECL__
ExtSym@         _osmajor, BYTE, __CDECL__
ExtProc@        abort, __CDECL__

;*** Begin addition
; Here we declare our new C routine, expwild, as well as the fact 
; that we will use the library routine sbrk (for memory allocation). 
; We also need to know the size of the stack, and so access it.  
; The macros ExtProc and ExtSym are defined in RULES.ASI.
;
ExtProc@        expwild, __CDECL__
ExtProc@        sbrk, __CDECL__
ExtSym@         _stklen, WORD, __CDECL__
;
;*** End addition


        SUBTTL  Parse Command Line
        PAGE
;/*                                                     */
;/*-----------------------------------------------------*/
;/*                                                     */
;/*     Parse Command Line                              */
;/*     ------------------                              */
;/*                                                     */
;/*-----------------------------------------------------*/
;/*                                                     */
PSPCmd          equ     00080h


CSeg@

IF      LPROG
SavedReturn     dd      ?
ELSE
SavedReturn     dw      ?
ENDIF
SavedDS         dw      ?
SavedBP         dw      ?

;*** Begin addition
; Here we declare a double word pointer to hold the address of our 
; command line string and a word to hold the string's size.  We 
; do not initialize either one.  
;
NewCmdLn        dd      ?
NewCmdSz        dw      ?
;
;*** End addition

PubProc@        _setargv, __CDECL__

;       First, save caller context and Return Address

        pop     word ptr SavedReturn
IF      LPROG
        pop     word ptr SavedReturn+2
ENDIF
        mov     SavedDS, ds

;*** Begin deletion
; This block of code got the address of the command line, zeroed 
; the registers, got the command line's length, appended a null to 
; the command line, and set up the registers for later.  cx ended 
; up containing the size of the command line string, including the 
; null, and other registers were set to zero.  The code we give 
; below to replace this code will end up in the same state, but will 
; first call our routine expwild to preprocess the command line.  
;
;       cld
;       mov     es, _psp@
;       mov     si, PSPCmd      ; ES: SI = Command Line address
;       xor     ax, ax
;       mov     bx, ax
;       mov     dx, ax          ; AX = BX = CX = DX = 0
;       mov     cx, ax          ; AX = BX = CX = DX = 0
;       lods    byte ptr es:[si]
;       mov     di, si
;       xchg    ax, bx
;       mov     es:[di+bx], al  ; Append a \0 at the end
;       inc     bx
;       xchg    bx, cx          ; CX = Command Line size including \0
;
;*** End deletion

;*** Begin addition
; Preprocess the command line for wild card expansion
;
; First, we get the stack size to see how large the expanded text 
; can be.  We limit ourselves to one-half of the available stack.  

IFDEF   __HUGE__
        mov     ax, seg _stklen@   ; if we are using the huge model,
        mov     es, ax             ; we need the segment that holds 
        mov     ax, es:_stklen@    ; the stack data
ELSE
        mov     ax, _stklen@       ; here we need only the stack's 
                                   ; length, as all data in one seg
ENDIF
        sar     ax, 1           ; Divide by two to leave room

        mov     NewCmdSz, ax    ; Save the maximum command size

; Now we call sbrk to allocate the desired memory.  We push the 
; amount we need, which was in ax, onto the stack and do the call.  

        push    ax
        call    sbrk@           ; Get some memory

; After a call we must restore the stack to its previous state.  
; Rather than popping the argument we just adjust the stack pointer.
; Also, because we called a C routine, which could have messed up 
; our data segment, we restore it to the segment value saved earlier 
; by the existing code.  Finally, we save the address of the new 
; command line buffer we allocated, which was returned in ax.  

        add     sp, 2           ; Clean up the stack
        mov     ds, SavedDS     ; Necessary?
        mov     word ptr NewCmdLn, ax   ; Save the pointer offset

; sbrk returns the segment in dx for large memory model code. 
; So, if we're not using that model, set dx to our segment so that 
; the following code will work.  

IFE     LDATA
        mov     dx, ds          ; Get our own segment if not returned
ENDIF
        mov     word ptr NewCmdLn+2, dx ; Save the pointer offset 
                                        ; segment just after the 
                                        ; pointer offset
        cld                     ; tell it to increment pointers 
                                ; after string operations

; Now we do basically what the earlier deleted code did.  We get 
; the command line address and append a null to the command line. 

        mov     es, _psp@       ; Get the DOS command line
        mov     si, PSPCmd      ;  address
        xor     ax, ax          ; Zero AX and BX
        mov     bx, ax
        lods    byte ptr es:[si]
        xchg    ax, bx
        mov     es:[si+bx], al  ; Append a \0 at the end

; Here we push the arguments for expwild and then make the call.  
; The third arg, and so the first to be pushed, is the maximum 
; command line size.

        mov     ax, NewCmdSz    ; Get the maximum command size
        push    ax

; The second arg is the destination string.  We push first its 
; segment if we are in large model.  In any case, we then push its 
; offset.  

IF LDATA
        mov     ax, word ptr NewCmdLn+2 ; Segment for large data
        push    ax
ENDIF
        mov     ax, word ptr NewCmdLn   ; Destination offset
        push    ax

; The first arg is the source command line string.  We push both its 
; segment and offset in all cases because expwild expects a far ptr
; for this arg.  

        mov     ax, _psp@       ; Segment of DOS command line
        push    ax
        mov     ax, PSPCmd+1    ; Offset (+ 1 to pass the count byte)
        push    ax
        call    expwild@        ; Finally, call expwild to expand 
                                ; the command line

; Clean up the stack after the call.  We adjust by 10 if in large 
; model because of the extra segment id we pushed.  

IF LDATA
        add     sp, 10          ; Clean up the stack
ELSE
        add     sp, 8           ; Clean up the stack
ENDIF
        mov     ds, SavedDS     ; Restore our data segment

; Then, process the command line.  We put the size in cx and the 
; string itself in es:[si], just as their code did.  Finally, we 
; zero the same registers as the original (now deleted) code.  

        mov     cx, ax          ; Put size in CX
        mov     es, word ptr NewCmdLn+2
        mov     si, word ptr NewCmdLn   ; Get the address of the 
                                        ; expanded string
        mov     di, si          ; Set up the registers correctly
        xor     ax, ax          ; AX = BX = CX = DX = 0
        mov     bx, ax
        mov     dx, ax
;
;*** End addition

Processing      label   near
        call    NextChar
        ja      NotQuote        ; Not a quote and there are more
InString        label   near
        jb      GetArg0Lgth     ; Command line is empty now
        call    NextChar
        ja      InString        ; Not a quote and there are more
NotQuote        label   near
        cmp     al, ' '
        je      EndArgument     ; Space is an argument separator
        cmp     al, 9
        jne     Processing      ; TAB   is an argument separator
EndArgument     label   near
        xor     al, al          ; Space and TAB are argument separators
        jmp     short Processing

;       Character test function used in SetArgs
;               On entry AL holds the previous character
;               On exit  AL holds the next character
;                        ZF on if the next character is quote (") and AL = 0
;                        CF on if end of command line and AL = 0

NextChar        PROC    NEAR
        or      ax, ax
        jz      NextChar0
        inc     dx              ; DX = Actual length of CmdLine
        stosb
        or      al, al
        jnz     NextChar0
        inc     bx              ; BX = Number of parameters
NextChar0       label   near
        xchg    ah, al
        xor     al, al
        stc
        jcxz    NextChar2       ; End of command line --> CF ON
        lods    byte ptr es:[si]
        dec     cx
        sub     al, '"'
        jz      NextChar2       ; Quote found --> AL = 0 and ZF ON
        add     al, '"'
        cmp     al,'\'
        jne     NextChar1       ; It is not a \
        cmp     byte ptr es:[si], '"'
        jne     NextChar1       ; Only " is transparent after \
        lods    byte ptr es:[si]
        dec     cx
NextChar1       label   near
        or      si, si          ; Be sure both CF & ZF are OFF
NextChar2       label   near
        ret
NextChar        ENDP

;       Invalid program name

BadProgName     label   near
        jmp     abort@

;       Now, Compute Argv[0] length

GetArg0Lgth     label   near
        mov     bp, es          ; BP = Program Segment Prefix address
        mov     si, _envLng@
        add     si, 2           ; SI = Program name offset
        mov     cx, 1           ; CX = Filename size (includes \0)
        cmp     _osmajor@, 3
        jb      NoProgramName
        mov     es, _envseg@
        mov     di, si          ; SI = argv[0] address
        mov     cl, 07fh
        repnz   scasb
        jcxz    BadProgName
        xor     cl, 07fh        ; CX = Filename size (includes \0)
NoProgramName   label   near

;       Now, reserve space for the arguments

ReserveSpace    label   near
        inc     bx              ; argv[0] = PgmName
        mov     _argc@, bx
        inc     bx              ; argv ends with NULL
        mov     ax, cx          ; Size = PgmNameLgth +
        add     ax, dx          ;        CmdLineLgth +
        add     bx, bx          ;        argc * 2       (LDATA = 0)
IF      LDATA
        add     bx, bx          ;        argc * 4       (LDATA = 1)
ENDIF
        add     ax, 1
        and     ax, not 1       ; Keep stack word aligned
        add     bx, ax
        mov     di, sp
        sub     di, ax          ; SS:DI = DestAddr for PgmName
        sub     sp, bx          ; SS:SP = &argv[0]
        xchg    bx, bp          ; BX = Program Segment Prefix address
        mov     bp, sp          ; BP = &argv[0]
        mov     word ptr _argv@, sp
IF      LDATA
        mov     word ptr _argv@+2, ss
ENDIF
        mov     ax, ss
        mov     es, ax          ; ES:DI = Argument's area

;       Copy program name

CopyArg0        label   near
        mov     [bp], di        ; Set argv[n]
IF      LDATA
        mov     [bp+2], es
        add     bp, 4
ELSE
        add     bp, 2
ENDIF
        mov     ds, _envseg@
        dec     cx
        rep     movsb
        xor     al, al
        stosb

;       Copy the command line

;*** Begin deletion
; This code used to ready the command line size and address.  
; Because we have changed these two, however, we must delete this 
; code and replace it with our own.  
;
;       mov     ds, bx
;       xchg    cx, dx          ; CX     = Command Line size
;       mov     si, PSPCmd + 1  ; DS: SI = Command Line address
;
;*** End deletion

;*** Begin addition
; We ready the size and address of our newly expanded command line.  
; The code below ours puts all of the arguments on the stack.  It
; uses the stack pointer that previous code has already adjusted 
; to be in the correct position to hold all of the arguments.  

        xchg    cx, dx          ; CX     = Command Line size
        push    ax              ; Save AX
        mov     ax, word ptr NewCmdLn+2 ; Use expanded command line
        mov     ds, ax
        mov     si, word ptr NewCmdLn   ; DS: SI = Command Line address
        pop     ax
;
;*** End addition

CopyArgs        label   near
        jcxz    SetLastArg
        mov     [bp], di        ; Set argv[n]
IF      LDATA
        mov     [bp+2], es
        add     bp, 4
ELSE
        add     bp, 2
ENDIF
CopyArg         label   near
        lodsb
        or      al, al
        stosb
        loopnz  CopyArg
        jz      CopyArgs
SetLastArg      label   near
        xor     ax, ax
        mov     [bp], ax
IF      LDATA
        mov     [bp+2], ax
ENDIF

;       Restore caller context and exit

;*** Begin addition
; We are finished.  We call sbrk with the negative of the amount 
; of memory it gave us, which causes that memory to be freed. 

        mov     ax, NewCmdSz    ; Get the command size
        neg     ax              ; Negate
        push    ax
        call    sbrk@           ; Free up any storage we use
        add     sp, 2           ; Clean up the stack after the call
;
;*** End addition

        mov     ds, SavedDS
IF      LPROG
        jmp     dword ptr SavedReturn
ELSE
        jmp     word ptr SavedReturn
ENDIF
EndProc@        _setargv, __CDECL__
CSegEnd@
        END
