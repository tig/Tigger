	page	64,130
;  bios.asm   ROM  Basic I/O  System interface
;  EXPORTS:
	public	_BIOBEL,  _BIODEOL, _BIODWN, _BIOKEY, _BIOPOS
	public  _BIOPUTC, _BIOPUTS, _BIOUP
	public  _BIO21, _syserr, _bioattr

;  DEFINES:
video	equ	10H		; video i/o interrupt
fcn	equ	21H		; MSDOS function call

; Commands for video:
setpos	equ	2		; set cursor position
readpos equ	3		; read cursor position
scrup	equ	6		; scroll up
scrdn	equ	7		; scroll down
wrchattr equ	9		; write char and attribute
writec  equ	14		; write char

DGROUP	group	_DATA
	assume  ds:DGROUP

_DATA	segment word public 'DATA'
_bioattr	db	7	; default attribute = normal
_syserr db	0FFH

	even
_DATA	ends

	assume	cs:_TEXT

_TEXT	segment byte public 'CODE'

;	BIO21 - MSDOS function call to set int handler address
;	Usage   BIO21()
;	Returns nothing
_BIO21	PROC	NEAR
	push	bp
	mov	bp,sp
	mov	ax,cs		; put addr in ds:dx
	mov	ds,ax
	mov	dx, offset BIO24 ; code to handle disk error
	mov	ax,2524H	; set vector for int24
	int	fcn
	pop	bp
	ret
_BIO21	endP

;	BIO24 - Critical error handler.
;	Called by disk fcns on errors via int24
BIO24:	add	sp,20		; strip bp ib cs flags ax-dx si di
	pop	bp
	pop	ds
	pop	es
	mov	ax,di		; put error code in syserr
	mov	_syserr,al
	mov	al,-1
	iret

;	BIOBEL - Send the BEL char to buzzer
;	Usage: BIOBEL()
;	Returns: nothing
_BIOBEL PROC	NEAR
	push	bp
	mov	bp,sp
	push	di
	push	si		; Use mov ax,[bp+4] to get arg.

	mov	al,7		; Send BEL to writec
	mov	bx,0		; Set page and color
	mov	ah,writec	; Write char
	int	video
	pop	si
	pop	di
	pop     bp
	ret
_BIOBEL endP

;	BIODEOL  -  delete chars to end-of-line
;	Usage:  BIODEOL(num)
;	num = number of characters to be deleted
;	Returns: nothing
_BIODEOL PROC	NEAR
	push	bp
	mov	bp,sp
	push	di
	push	si

	mov	cx,[bp+4]	; set character count
	mov	bl,_bioattr	; set attribute
	mov	bh,0		; set page
	mov	al,' '		; blank character
	mov	ah,wrchattr	; write char and attribute command
	int	video		; do command

	pop	si
	pop	di
	pop	bp
	ret
_BIODEOL endP


;	BIODWN - Scroll display down.
;	Usage:	BIODWN(num, start, end)
;
;	al = num of lines to scroll down, 0 erases screen
;	ch, cl = row, col of upper left corner of scroll.
;	dh, dl = row, col of lower right corner of scroll.
;	bh = attribute byte of erased line 
;	Returns: nothing
_BIODWN	PROC	NEAR
	push	bp
	mov	bp,sp
	push	di
	push	si 
	mov	al,[bp+4]	; num of lines to scroll
	mov	cl,0		; starting column
	mov	ch,[bp+6]	; starting row
	mov	dl,79		; ending column
	mov	dh,[bp+8]	; ending row
	mov	bh,7		; attribute of erased line 
	mov	ah,scrdn	; scroll down command
	int	video		; do it
	pop	si
	pop	di
	pop	bp
	ret
_BIODWN	endP


;	BIOPOS  -  Set row and column position of cursor
;	Usage:	BIOPOS(xpos, ypos)
;	dl = column position (0 - 79), 0 on left
;	dh = row position (0 - 24), 0 at top
;	bh = page number
;	Returns: nothing
_BIOPOS	PROC	NEAR
	push	bp
	mov	bp,sp
	mov	dx,[bp+4]	; fetch column
	mov	ax,[bp+6]	; fetch row
	mov	dh,al
	mov	bh,0		; set page 0
	mov	ah,setpos	; cursor position command
	int	video		; do command
	pop	bp
	ret
_BIOPOS	endP

;	BIOPUTC  -  Write character, move cursor
;	Usage:  BIOPUTC(ch)
;	Warning: NO BEL, \r, or \n
;
;	Returns: nothing
_BIOPUTC PROC	NEAR
	push	bp
	mov	bp,sp
	push	di
	push	si

	mov	bh,0		; page number
	mov	ah,readpos	; get cursor position
	int	video		; dh, dl = row, col of cursor

	mov	cx,1		; write one char
	cld
	mov	ax,[bp+4]	; char to write
	mov	bh,0		; set page
	mov	bl,_bioattr	; set attribute
	mov	ah,wrchattr	; write char and attribute command
	int	video		; do command

	inc	dl		; move to next column
	cmp	dl,80		; but not past end
	jnc	putc3
	mov	bh,0		; set page no.
	mov	ah,setpos	; dh, dl = row, col of cursor
	int	video		; set cursor position

putc3:	mov	ax,dx		; return cursor position
	pop	si
	pop	di
	pop	bp
	ret
_BIOPUTC endP

;	BIOPUTS  -  Write string
;	Usage:  BIOPUTS(sptr)
;	Warning: does not do BEL, \r, or \n
;	Returns: new cursor position
_BIOPUTS PROC	NEAR
	push	bp
	mov	bp,sp
	push	di
	push	si
	mov	bh,0		; page number
	mov	ah,readpos	; get cursor position
	int	video		; dh, dl = row, col of cursor

	mov	si,[bp+4]	; ptr to string
	mov	cx,1		; write one char each
puts1:	cld
	lodsb			; get next char
	or	al,al		; quit on null
	jz	puts3
	mov	bh,0		; set page
	mov	bl,_bioattr	; set attribute
	mov	ah,wrchattr	; write char and attribute command
	int	video		; do command

	inc	dl		; move to next column
	cmp	dl,80		; but not past end
	jnc	puts3
	mov	bh,0		; set page no.
	mov	ah,setpos	; dh, dl = row, col of cursor
	int	video		; set cursor position
	jmp	puts1

puts3:	mov	ax,dx		; return cursor position
	pop	si
	pop	di
	pop	bp
	ret
_BIOPUTS endP


;	BIOUP   -  Scroll display up.
;	Usage:	BIOUP(num, start, end)
;	al = num of lines to scroll down, 0 erases screen
;	ch, cl = row, col of upper left corner of scroll.
;	dh, dl = row, col of lower right corner of scroll.
;	bh = attribute byte of erased line 
;	Returns: nothing
_BIOUP	PROC	NEAR
	push	bp
	mov	bp,sp
	push	di
	push	si
	mov	al,[bp+4]	; num of lines to scroll
	mov	cl,0		; starting column
	mov	ch,[bp+6]	; starting row
	mov	dl,79		; ending column
	mov	dh,[bp+8]	; ending row
	mov	bh,7		; attribute of erased line 
	mov	ah,scrup	; scroll command
	int	video		; do command
	pop	si
	pop	di
	pop	bp
	ret
_BIOUP	endP


;  Assembly language interface to the PC ROM

;  KEYBOARD routine for the PC
;  In this implementation, the int 16H i/o BIOS routine is invoked.
;  biokey returns an ASCII code in the lower byte and a scan code
;  in the upper byte. Often, but not always, the scan code is the
;  same as the position of the key on the keyboard as noted on p. 2-16
;  of the IBM tech reference manual. There are 83 keys numbered 1-83.
;  In a most cases the scan code is affected by the ALT key.

;  The ASCII codes for each combination of keypresses
;  is found in the IBM technical reference manual pages 3-10.
;  Without going into great detail, most unshifted, shifted, and
;  control keys yield the expected ASCII code AND the scan code.
;  Control keys that do not have an ASCII code, return nothing.
;  (e.g. control-#). These control keys are thus useless (the keyboard
;  routine ignores these keypresses just as though they did not happen).

;  The ALT key pressed with another key returns 0 for the ASCII code
;  and a scan code for almost all of the keys that have an ASCII
;  value in the control mode (with control key pressed).
;  The ALT mode also returns scan code for numbers.

;  As with ALT, the function keys return a 0 ASCII code with the scan code.
;  The scan code depends upon the Shift, Ctrl, and Alt keys. See p 3-14.

;  When shifted (or NUMLOCK), the numeric keypad gives the regular ASCII
;  code and scan code.
;  In the unshifted mode, all but key labeled 5 return 0 ASCII code and
;  a scan code.
;  See page 3-14 in the tech reference for the scan codes. They go up
;  to 132.

;	BIOKEY - get key input from keyboard
;	Usage:	x = BIOKEY(peek);
;	peek = 0, wait for key input.
;	peek = 1, check for key waiting
;	arg passed in [bp + 4]
;	Returns: ascii code and scan code

_BIOKEY	PROC	NEAR
	push	bp
	mov	bp,sp
	cmp	word ptr [bp + 4],1
	je	test
	mov	ah,0		; get char
	int	16H
	jmp	done
test:	mov	ah,1		; test for char
	int	16H
	jnz	done
	xor	ax,ax		; clear ax, if none
done:	mov	sp,bp		; codes are in ax
	pop	bp
	ret
_BIOKEY	endP



_TEXT	ends

	end
;*
