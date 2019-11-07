;Assembly language file for TPHELP unit
;Written by Kim Kokkonen, TurboPower Software
;Version 1.0, 2/7/88
;  initial release
;Version 1.1
;  support enhanced keyboard calls in int16 handler
;Copyright (c) 1988 TurboPower Software
;may be used freely by owners of Turbo Professional

StackSize       =       2000            ;Size of alternate stack

DATA    SEGMENT BYTE PUBLIC
        EXTRN   CurrentTopic : WORD     ;Current help topic
        EXTRN   SaveInt16 : DWORD       ;Old value of int 16
        EXTRN   HelpSystem : DWORD      ;Pointer to initialized help system
        EXTRN   HelpActive : BYTE       ;1 if help system on screen, else 0
        EXTRN   HelpKey : WORD          ;Scan word for help
        EXTRN   StackSpace : DWORD      ;Pointer to alternate stack

CurSP   DW      ?                       ;Stack position when help activated
CurSS   DW      ?
AXentry DW      ?                       ;AX on entry to interrupt

DATA    ENDS

CODE    SEGMENT BYTE PUBLIC

        ASSUME  CS:CODE, DS:DATA
        EXTRN   ShowHelp : NEAR         ;Actually FAR, be careful!
        PUBLIC  Int16Handler

CSInt16 LABEL   DWORD
Int16Ofs        DW      ?
Int16Seg        DW      ?

;*********************************************************** NewInt16
;Handle software keyboard interrupts
Int16Handler PROC FAR
        PUSH    DS                      ;Save registers we use
        PUSH    AX
        MOV     AX,SEG DATA
        MOV     DS,AX                   ;Set DS to Turbo segment
        POP     AX                      ;Restore AX
        STI                             ;Allow interrupts
        CMP     HelpActive,0            ;Help already on screen?
        JNZ     Chain16                 ;Yes, pass on to old int 16
        MOV     AXentry,AX              ;Save AX for retry
        OR      AH,AH                   ;Reading a key?
        JZ      GetKey                  ;Yes, read it
        CMP     AH,10h                  ;Reading a key with enhanced BIOS?
        JZ      GetKey                  ;Yes, read it
Chain16:                                ;Transfer to old int16
        PUSH    AX
        MOV     AX,WORD PTR [SaveInt16]
        MOV     Int16Ofs,AX
        MOV     AX,WORD PTR [SaveInt16+2]
        MOV     Int16Seg,AX             ;Set up CS-relative int16
        POP     AX
        POP     DS
        JMP     CSInt16                 ;Back to old int16
GetKey:
        PUSHF                           ;Keep IRET happy
        CALL    SaveInt16               ;Emulate call to old int16
        CMP     AX,HelpKey              ;Is it help key?
        JZ      GotHelpKey              ;Yes, put up help
        POP     DS                      ;No, restore DS
        IRET                            ;Back to caller
GotHelpKey:
        MOV     HelpActive,1            ;Set flag to avoid re-entry
        MOV     CurSS,SS                ;Save current stack position
        MOV     CurSP,SP
        MOV     AX,WORD PTR [StackSpace]
        ADD     AX,StackSize
        CLI                             ;Switch to big stack
        MOV     SS,WORD PTR [StackSpace+2]
        MOV     SP,AX
        STI
        PUSH    BX                      ;Save rest of registers
        PUSH    CX
        PUSH    DX
        PUSH    SI
        PUSH    DI
        PUSH    ES
        PUSH    BP
        PUSH    WORD PTR [HelpSystem+2] ;Parameters for call
        PUSH    WORD PTR [HelpSystem]
        PUSH    CurrentTopic
        PUSH    CS                      ;Simulate far call
        CALL    ShowHelp                ;Call display routine, ignore function result
        POP     BP
        POP     ES
        POP     DI
        POP     SI
        POP     DX
        POP     CX
        POP     BX
        CLI
        MOV     SS,CurSS                ;Back to original stack
        MOV     SP,CurSP
        STI
        MOV     HelpActive,0            ;Help no longer active
        MOV     AX,AXentry              ;Restore initial AX
        JMP     GetKey                  ;Get another key before returning
Int16Handler ENDP

CODE    ENDS
        END
