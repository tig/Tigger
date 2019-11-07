        TITLE   WINSTUB - Assembly stub program for new format EXE files

.xlist
include cmacros.inc
.list

stubstack segment stack
        dw      80h dup (?)
stubstack ends

sBegin  CODE
assumes CS,CODE

WINSTUB PROC    NEAR
        call    ws1
        DB      'Font Version 1.0',13,10
        DB      'Copyright (c) 1992 Charles E. Kindel, Jr. All Rights Reserved.',13,10,10
        DB      'Portions Copyright (c) 1985-1992 Microsoft Corporation.',13,10,10
        DB      'Font requires Microsoft Windows 3.0 or higher to run.',13,10,'$'

; extra space for internationalization
        DB      '                                        '

ws1:    pop     dx
        push    cs
        pop     ds          ; DS:DX -> error message
        mov     ah,9        ; Print error message
        int     21h
        mov     ax,4C01h    ; Terminate program, exit code = 1
        int     21h
WINSTUB ENDP

sEnd

END WINSTUB
