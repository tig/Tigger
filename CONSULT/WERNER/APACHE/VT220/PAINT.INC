;************************************************************************
;*
;*    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
;*
;*-----------------------------------------------------------------------
;*
;*     Project:  APACHE, Apache For Windows
;*
;*      Module:  paint.inc
;*
;*   Author(s):  Bryan A. Woodruff (baw)
;*               Charles E. Kindel, Jr. (cek)
;*
;*
;*     Remarks:  System Timer Handler EVENT structure
;*
;*   Revisions:  
;*     01.00.000  9/22/91 baw   Wrote it.
;*
;************************************************************************

MAXLEN_FONTS            equ     12
MAXLEN_PARAMS           equ     10
MAXLEN_ANSWERBACK       equ     81
MAXLEN_CHARSETS         equ     4
MAXLEN_CONTROLSEQ       equ     81
MAXLEN_KEYBOARDSTATE    equ     256

ATTRIBUTE_BOLD          equ     80h
ATTRIBUTE_UNDERLINE     equ     40h
ATTRIBUTE_BLINKING      equ     20h
ATTRIBUTE_INVERSE       equ     10h
ATTRIBUTE_ERASEABLE     equ     08h
ATTRIBUTE_OEMCHARSET    equ     04h
ATTRIBUTE_DBLHEIGHTBTM  equ     03h
ATTRIBUTE_DBLHEIGHTTOP  equ     02h
ATTRIBUTE_DBLWIDTH      equ     01h

ATTRIBUTE_DBLSIZE       equ     03h

FONT_STANDARD           equ     00h
FONT_UNDERLINE          equ     01h
FONT_DBLWIDTH           equ     02h
FONT_DBLWIDTHUL         equ     03h
FONT_DBLHEIGHT          equ     04h
FONT_DBLHEIGHTUL        equ     05h
FONT_STANDARDOEM        equ     06h
FONT_UNDERLINEOEM       equ     07h
FONT_DBLWIDTHOEM        equ     08h
FONT_DBLWIDTHULOEM      equ     09h
FONT_DBLHEIGHTOEM       equ     0Ah
FONT_DBLHEIGHTULOEM     equ     0Bh

VT220_COLUMNS_132       equ     132

GWW_ECB                 equ     0

EXTLOGFONT STRUC

        elf_LogFont             db      size LOGFONT dup (?)
        elf_lfFGcolor           dd      ?
        elf_lfBGcolor           dd      ?

EXTLOGFONT ENDS


CURSORSAVE STRUC

        cs_nCurrentRow  dw      ?
        cs_nCurrentCol  dw      ?
        cs_nDECGL       dw      ?
        cs_nDECGR       dw      ?
        cs_fAutoWrap    dw      ?
        cs_fOrigin      dw      ?
        cs_fSelErase    dw      ?
        cs_bAttr        db      ?

CURSORSAVE ENDS

ECB STRUC

        ecb_nEmulatorType       dw      ?
        ecb_nColumns            dw      ?
        ecb_nRows               dw      ?
        ecb_nParamCt            dw      ?
        ecb_nScrollRegionTop    dw      ?
        ecb_nScrollRegionBottom dw      ?
        ecb_anParams            dw      MAXLEN_PARAMS dup (?)
        ecb_nCurrentCol         dw      ?
        ecb_nCurrentRow         dw      ?
        ecb_nSaveRow            dw      ?
        ecb_nSaveCol            dw      ?
        ecb_nCharX              dw      ?
        ecb_nCharY              dw      ?
        ecb_nOffsetX            dw      ?
        ecb_nOffsetY            dw      ?
        ecb_nScrollX            dw      ?
        ecb_nScrollY            dw      ?
        ecb_nSizeX              dw      ?
        ecb_nSizeY              dw      ?
        ecb_nDECGL              dw      ?
        ecb_nDECGR              dw      ?
        ecb_nSnglShft           dw      ?
        ecb_szAnswerBack        db      MAXLEN_ANSWERBACK dup (?)
        ecb_fLocalEcho          dw      ?
        ecb_fAutoWrap           dw      ?
        ecb_fNewLine            dw      ?
        ecb_fUseNumLock         dw      ?
        ecb_fOrigin             dw      ?
        ecb_fSelErase           dw      ?
        ecb_fBlinkState         dw      ?
        ecb_fCursorStateSaved   dw      ?
        ecb_abCharSets          db      MAXLEN_CHARSETS dup (?)
        ecb_abControlSeq        db      MAXLEN_CONTROLSEQ dup (?)
        ecb_abTabStops          db      VT220_COLUMNS_132 / 8 + 1 dup (?)
        ecb_abKeyboardState     db      MAXLEN_KEYBOARDSTATE dup (?)
        ecb_bAttr               db      ?
        ecb_bCursorState        db      ?
        ecb_bKeypadType         db      ?
        ecb_lfHighlightFont     db      size EXTLOGFONT dup (?)
        ecb_lfNormalFont        db      size EXTLOGFONT dup (?)
        ecb_lfOEMFont           db      size EXTLOGFONT dup (?)
        ecb_hScreenDC           dw      ?
        ecb_lpfnErrorProc       dd      ?
        ecb_hScreenBuffer       dw      ?
        ecb_ahEmulatorFonts     dw      MAXLEN_FONTS dup (?)
        ecb_hDefaultFont        dw      ?
        ecb_wSequenceState      dw      ?
        ecb_wSequenceFlag       dw      ?
        ecb_wCompatibilityLevel dw      ?
        ecb_wResponseLevel      dw      ?
        ecb_csSaveState         db      size CURSORSAVE dup (?)
        ecb_lpCharBuf           dd      ?
        ecb_lpAttrBuf           dd      ?

ECB ENDS

;************************************************************************
;* End of File: paint.inc
;************************************************************************

