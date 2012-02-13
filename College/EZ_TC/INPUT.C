/* input.c

                    INPUT - input conversion (In)

*/

/* IMPORTS -----------------------------------------------------------------*/
#include "ez.h"
#include "xsys.h"
#include "input.h"

/* LOCAL SYMBOLIC DEFS ----------------------------------------------------*/
#define WAIT 0          /* wait on Xgetc */

/*==========================================================================*/
/*                            InpCmnd                                       */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*       Converts input from keyboard to a command.  Returns command.       */
/*                                                                          */
/*  RETURNS                                                                 */
/*       Returns the command.                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int InpCmnd (void)
{/*   - Table of default commands for each key press
        Definitions are in input.h.
 */
static short inpcmtbl [] =
{
/* ^@ */ '?',    /* ^A */ MBOB,   /* ^B */ MBOL,   /* ^C */ QUIT,
/* ^D */ DCHR,   /* ^E */ DEOW,   /* ^F */ MEOL,   /* ^G */ DEOL,
/* ^H */ MVLF,   /* ^I */ '\t',   /* ^J */ MVDN,   /* ^K */ MVUP,
/* ^L */ MVRT,   /* ^M */ '\n',   /* ^N */ MVNP,   /* ^O */ MBOW,
/* ^P */ MEOW,   /* ^Q */  17 ,   /* ^R */ 18  ,   /* ^S */ 19  ,
/* ^T */ 20  ,   /* ^U */ DBOL,   /* ^V */ VERS,   /* ^W */ DBOW,
/* ^X */ 24  ,   /* ^Y */ MVPP,   /* ^Z */ MEOB,   /* ^[ */ ESC ,
/* ^\ */ 28  ,   /* ^] */  29 ,   /* ^^ */ 30  ,   /* ^_ */ 31  ,
' ',    '!',    '"',    '#',    '$',    '%',    '&',    '\'',
'(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',
'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',
'8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',
'@',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',
'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
'X',    'Y',    'Z',    '[',    '\\',   ']',    '^',    '_',
'`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',
'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
'x',    'y',    'z',    '{',    '|',    '}',    '~',    DELP,
128,    129,    130,    131,    132,    133,    134,    135,
136,    137,    138,    139,    140,    141,    142,    143,
144,    145,    146,    147,    148,    149,    150,    151,
152,    153,    154,    155,    156,    157,    158,    159,
160,    161,    162,    163,    164,    165,    166,    167,
168,    169,    170,    171,    172,    173,    174,    175,
176,    177,    178,    179,    180,    181,    182,    183,
184,    185,    186,    187,    188,    189,    190,    191,
192,    193,    ABRT,   195,    196,    197,    MTLN,  HELP,
200,    201,    202,    203,    204,    205,    206,    207,
208,    209,    210,    211,    212,    213,    WRIT,   215,
216,    217,    218,    219,    220,    221,    222,    223,
224,    225,    ABRT,   227,    228,    229,    MTLN,  HELP,
232,    233,    234,    235,    236,    237,    238,    239,
240,    241,    242,    243,    244,    245,    WRIT,   247,
248,    249,    250,    251,    252,    253,    254,    255
};
    int code;

    code = Xgetc (WAIT);  /* Wait for a character and get it */
    if (code == ESC)
       switch (code = Xgetc (WAIT))
       {   case 'C' : code += 127; break;
           case 'c' : code += 127; break;
           case 'W' : code += 127; break;
           case 'w' : code += 127; break;
           case 'G' : code += 127; break;
           case 'g' : code += 127; break;
           case 'H' : code += 127; break;
           case 'h' : code += 127; break;
           default  : break;
       }
    return (inpcmtbl [code]);
}

/*==========================================================================*/
/*  END OF  input.c               (C) KindelCo Software Systems, 1987       */
/*==========================================================================*/



