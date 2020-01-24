/***************************************************************************
 * ELSEMAP.H - Base definitions for ElseWare PANOSE(tm) Font Mapper.
 *
 * $keywords: elsemap.h 1.1 14-Feb-92 1:40:16 PM$
 *
 * Copyright (C) 1991 ElseWare Corporation.  All rights reserved.
 ***************************************************************************/

#ifndef _elsemap_def
#define _elsemap_def

/***************************************************************************
 * ELSEUSER
 *
 * Rather than modify elsemap.h, put flags and additions in elseuser.h, or
 * disable inclusion of elseuser.h by placing "#define _elseuser_def" ahead
 * of the #include for elsemap.h.
 ***************************************************************************/
#ifndef _elseuser_def
#include "elseuser.h"
#endif

/***************************************************************************
 * ELSETYPE
 *
 * Basic system level types.  If already defined, albeit differently by
 * the system, redefine them (or #include a 'types' header file) in
 * elseuser.h and #define _elsetype_def.
 ***************************************************************************/
#ifndef _elsetype_def
#include "elsetype.h"
#endif

/***************************************************************************
 * COMPILER FLAG OVERRIDES
 *
 * If we are compiling one of the core ElseWare Font Mapper source files,
 * then override flags that turn off some constants and structures as we
 * always need them inside the mapper.
 ***************************************************************************/
#ifdef ELSE_MAPPER_CORE

#undef NOELSEPANCONST
#undef NOELSENIBBLES
#undef NOELSEPENALTY

/* This needed if disk conversion routines have not been disabled.
 */
#ifndef NOELSEDISKROUTINES
#undef NOELSEDISK
#endif

#endif

/***************************************************************************
 * COMPILATION FLAGS
 *
 * These flags control various aspects of compiling the font mapper.  You
 * turn these flags on by #defining them in elseuser.h.
 ***************************************************************************/

/* ELSEARGS macro
 *
 * Disable argument lists on prototypes by #defining NOELSEARGS.
 */
#ifdef ELSEARGS
#undef ELSEARGS
#endif
#ifndef NOELSEARGS
#define ELSEARGS(args) args
#else
#define ELSEARGS(args) ()
#endif

/* ELSEEXTERN
 *
 * Disable explicit extern on prototypes by #defining NOELSEEXTERN.
 */
#ifdef ELSEEXTERN
#undef ELSEEXTERN
#endif
#ifndef NOELSEEXTERN
#define ELSEEXTERN extern
#else
#define ELSEEXTERN
#endif

/* ELSELOCAL
 *
 * Disable explicit static on local data and procedures by
 * #defining NOELSELOCAL.
 */
#ifdef ELSELOCAL
#undef ELSELOCAL
#endif
#ifndef NOELSELOCAL
#define ELSELOCAL static
#else
#define ELSELOCAL
#endif

/* ELSEPASCAL
 *
 * Disable explicit pascal on procs by #defining NOELSEPASCAL.
 */
#ifdef ELSEPASCAL
#undef ELSEPASCAL
#endif
#ifndef NOELSEPASCAL
#define ELSEPASCAL pascal
#else
#define ELSEPASCAL
#endif

/* ELSEFARPROC
 *
 * Disable explicit far on routines by #defining NOELSEFARPROC.
 */
#ifdef ELSEFARPROC
#undef ELSEFARPROC
#endif
#ifndef NOELSEFARPROC
#define ELSEFARPROC far
#else
#define ELSEFARPROC
#endif

/* ELSENEARPROC
 *
 * Disable explicit near on local routines by #defining NOELSEFARPROC.
 */
#ifdef ELSENEARPROC
#undef ELSENEARPROC
#endif
#ifndef NOELSENEARPROC
#define ELSENEARPROC near
#else
#define ELSENEARPROC
#endif

/* ELSEFARPTR
 *
 * Disable explicit far on pointers by #defining NOELSEFARPTR.
 */
#ifndef ELSEFAR
#define ELSEFAR far
#endif
#ifdef ELSEFARPTR
#undef ELSEFARPTR
#endif
#ifndef NOELSEFARPTR
#define ELSEFARPTR ELSEFAR
#else
#define ELSEFARPTR
#endif

/* ELSEINITDEFSTATE
 *
 * The initial state of using the default font.  It is 0 if the default
 * font should *not* be used, 1 if it should.
 */
#ifndef ELSEINITDEFSTATE
#define ELSEINITDEFSTATE 1
#endif

/* ELSEINITTHRESHSTATE
 * 
 * The initial state of relaxing the threshold.  It is 0 if the threshold
 * should be used, 1 if it should be relaxed (i.e., *not* used).
 */
#ifndef ELSEINITTHRESHSTATE
#define ELSEINITTHRESHSTATE 0
#endif

/* ELSEINITTHRESHOLD
 *
 * The initial threshold value.
 */
#ifndef ELSEINITTHRESHOLD
#define ELSEINITTHRESHOLD 30
#endif

/* ELSEDEFCOUNTMORE
 *
 * Default size for the 'more' attributes digits in the Expanded PANOSE
 * memory structure.  It is strongly recommended that this be an even
 * number (this is 'cleaner' for bouncing between NIBBLE and BYTE format).
 */
#ifndef ELSEDEFCOUNTMORE
#define ELSEDEFCOUNTMORE 32
#endif

/* ELSEMATCHERROR
 *
 * This constant marks an error.
 */
#ifndef ELSEMATCHERROR
#define ELSEMATCHERROR 0xFFFF
#endif

/* ELSEMAXINT
 *
 * Maximum legal value for an INT.
 */
#ifndef ELSEMAXINT
#define ELSEMAXINT 0x7FFF
#endif

/* ELSELMUL
 *
 * a * b long math operation.
 */
#ifndef ELSELMUL
#define ELSELMUL(a, b) \
   (LONG)((LONG)(a) * (LONG)(b))
#endif

/* ELSELDIV
 *
 * a / b long math operation.
 */
#ifndef ELSELDIV
#define ELSELDIV(a, b) \
   (LONG)((LONG)(a) / (LONG)(b))
#endif

/* ELSEMULDIV
 *
 * (a * b) / c math operation.
 */
#ifndef ELSEMULDIV
#define ELSEMULDIV(a, b, c) \
   (INT)(ELSELDIV((ELSELMUL((a), (b)) + ELSELDIV((c), 2)), (c)))
#endif

/* ELSEMEMCPY
 *
 * lmemcpy operation.
 */
#undef ELSEUSELOCALMEMCPY
#ifndef ELSEMEMCPY
#define ELSEUSELOCALMEMCPY
#define ELSEMEMCPY(dst, src, len) uiELSEMemCpy((dst), (src), (len))
#endif

/***************************************************************************
 * PANOSE CONSTANTS
 *
 * These constants represent the individual values of the PANOSE digits.
 ***************************************************************************/
#define PANOSE_ANY             0
#define PANOSE_NOFIT           1

#ifndef NOELSEPANCONST

#define FAMILY_LATTEXTANDDISP  2
#define FAMILY_LATSCRIPT       3
#define FAMILY_LATDECORATIVE   4
#define FAMILY_LATPICTORIAL    5
#define FAMILY__LAST           5

#define SERIF_COVE             2
#define SERIF_OBTUSECOVE       3
#define SERIF_SQUARECOVE       4
#define SERIF_OBTSQUARECOVE    5
#define SERIF_SQUARE           6
#define SERIF_THIN             7
#define SERIF_BONE             8
#define SERIF_EXAGGERATED      9
#define SERIF_TRIANGLE         10
#define SERIF_NORMALSANS       11
#define SERIF_OBTUSESANS       12
#define SERIF_PERPSANS         13
#define SERIF_FLARED           14
#define SERIF_ROUNDED          15
#define SERIF__LAST            15

#define WEIGHT_VERYLIGHT       2
#define WEIGHT_LIGHT           3
#define WEIGHT_THIN            4
#define WEIGHT_BOOK            5
#define WEIGHT_MEDIUM          6
#define WEIGHT_DEMI            7
#define WEIGHT_BOLD            8
#define WEIGHT_HEAVY           9
#define WEIGHT_BLACK           10
#define WEIGHT_NORD            11
#define WEIGHT__LAST           11

#define PROPORTION_OLDSTYLE    2
#define PROPORTION_MODERN      3
#define PROPORTION_EVENWIDTH   4
#define PROPORTION_EXPANDED    5
#define PROPORTION_CONDENSED   6
#define PROPORTION_VERYEXP     7
#define PROPORTION_VERYCOND    8
#define PROPORTION_MONOSPACE   9
#define PROPORTION__LAST       9

#define CONTRAST_NONE          2
#define CONTRAST_VERYLOW       3
#define CONTRAST_LOW           4
#define CONTRAST_MEDIUMLOW     5
#define CONTRAST_MEDIUM        6
#define CONTRAST_MEDIUMHIGH    7
#define CONTRAST_HIGH          8
#define CONTRAST_VERYHIGH      9
#define CONTRAST__LAST         9

#define STROKE_NOVARIATION     2
#define STROKE_GRADDIAG        3
#define STROKE_GRADTRANS       4
#define STROKE_GRADVERT        5
#define STROKE_GRADHORZ        6
#define STROKE_RAPIDVERT       7
#define STROKE_RAPIDHORZ       8
#define STROKE_INSTANTVERT     9
#define STROKE__LAST           9

#define ARM_STRAIGHTHORZ       2
#define ARM_STRAIGHTWEDGE      3
#define ARM_STRAIGHTVERT       4
#define ARM_STRAIGHTSGLSERIF   5
#define ARM_STRAIGHTDBLSERIF   6
#define ARM_NONSTRTHORZ        7
#define ARM_NONSTRTWEDGE       8
#define ARM_NONSTRTVERT        9
#define ARM_NONSTRTSGLSERIF    10
#define ARM_NONSTRTDBLSERIF    11
#define ARM__LAST              11

#define LTRFORM_NORMCONTACT    2
#define LTRFORM_NORMWEIGHTED   3
#define LTRFORM_NORMBOXED      4
#define LTRFORM_NORMFLATTENED  5
#define LTRFORM_NORMROUNDED    6
#define LTRFORM_NORMOFFCENTER  7
#define LTRFORM_NORMSQUARE     8
#define LTRFORM_OBLQCONTACT    9
#define LTRFORM_OBLQWEIGHTED   10
#define LTRFORM_OBLQBOXED      11
#define LTRFORM_OBLQFLATTENED  12
#define LTRFORM_OBLQROUNDED    13
#define LTRFORM_OBLQOFFCENTER  14
#define LTRFORM_OBLQSQUARE     15
#define LTRFORM__LAST          15

#define MIDLINE_STDTRIMMED     2
#define MIDLINE_STDPOINTED     3
#define MIDLINE_STDSERIFED     4
#define MIDLINE_HIGHTRIMMED    5
#define MIDLINE_HIGHPOINTED    6
#define MIDLINE_HIGHSERIFED    7
#define MIDLINE_CONSTTRIMMED   8
#define MIDLINE_CONSTPOINTED   9
#define MIDLINE_CONSTSERIFED   10
#define MIDLINE_LOWTRIMMED     11
#define MIDLINE_LOWPOINTED     12
#define MIDLINE_LOWSERIFED     13
#define MIDLINE__LAST          13

#define XHEIGHT_CONSTSMALL     2
#define XHEIGHT_CONSTSTD       3
#define XHEIGHT_CONSTLARGE     4
#define XHEIGHT_DUCKSMALL      5
#define XHEIGHT_DUCKSTD        6
#define XHEIGHT_DUCKLARGE      7
#define XHEIGHT__LAST          7

/*** Expanded PANOSE values.
 ***/
#define FAM_SCRIPT_LATIN       2
#define FAM_SCRIPT__LAST       2

#define FAM_GENRE_TEXTANDDISP  2
#define FAM_GENRE_SCRIPT       3
#define FAM_GENRE_DECORATIVE   4
#define FAM_GENRE_PICTORIAL    5
#define FAM_GENRE__LAST        5

#define PROP_MONOSPACE_NO      2
#define PROP_MONOSPACE_YES     3
#define PROP_MONOSPACE__LAST   3

#define PROP_RATIO_OLDSTYLE    2
#define PROP_RATIO_MODERN      3
#define PROP_RATIO_EVENWIDTH   4
#define PROP_RATIO__LAST       4

#define PROP_DISTORT_VERYCOND  2
#define PROP_DISTORT_CONDENSED 3
#define PROP_DISTORT_NORMAL    4
#define PROP_DISTORT_EXPANDED  5
#define PROP_DISTORT_VERYEXP   6
#define PROP_DISTORT__LAST     6

#define STROKE_SPEED_NONE      2
#define STROKE_SPEED_GRADUAL   3
#define STROKE_SPEED_RAPID     4
#define STROKE_SPEED_INSTANT   5
#define STROKE_SPEED__LAST     5

#define STROKE_STRESS_NONE     2
#define STROKE_STRESS_DIAG     3
#define STROKE_STRESS_TRANS    4
#define STROKE_STRESS_VERT     5
#define STROKE_STRESS_HORZ     6
#define STROKE_STRESS__LAST    6

#define ARM_TYPE_NONSTRT       2
#define ARM_TYPE_STRAIGHT      3
#define ARM_TYPE__LAST         3

#define ARM_CTERM_HORZ         2
#define ARM_CTERM_WEDGE        3
#define ARM_CTERM_VERT         4
#define ARM_CTERM_SGLSERIF     5
#define ARM_CTERM_DBLSERIF     6
#define ARM_CTERM__LAST        6

#define LTRFM_SLANT_NORMAL     2
#define LTRFM_SLANT_OBLIQUE    3
#define LTRFM_SLANT__LAST      3

#define LTRFM_ROUND_CONTACT    2
#define LTRFM_ROUND_WEIGHTED   3
#define LTRFM_ROUND_BOXED      4
#define LTRFM_ROUND_FLATTENED  5
#define LTRFM_ROUND_ROUNDED    6
#define LTRFM_ROUND_SQUARE     7
#define LTRFM_ROUND__LAST      7

#define LTRFM_SYMMET_OFFCENTER 2
#define LTRFM_SYMMET_ONCENTER  3
#define LTRFM_SYMMET__LAST     3

#define MIDLN_POS_STANDARD     2
#define MIDLN_POS_HIGH         3
#define MIDLN_POS_CONSTANT     4
#define MIDLN_POS_LOW          5
#define MIDLN_POS__LAST        5

#define MIDLN_APEX_TRIMMED     2
#define MIDLN_APEX_POINTED     3
#define MIDLN_APEX_SERIFED     4
#define MIDLN_APEX__LAST       4

#define XHT_HEIGHT_SMALL       2
#define XHT_HEIGHT_STANDARD    3
#define XHT_HEIGHT_LARGE       4
#define XHT_HEIGHT__LAST       4

#define XHT_DIACRIT_CONSTANT   2
#define XHT_DIACRIT_DUCKING    3
#define XHT_DIACRIT__LAST      3
#endif  /* ifndef NOELSEPANCONST */

/***************************************************************************
 * 10-DIGIT PANOSE INDICES
 *
 * Indices into an array containing the standard 10-digit PANOSE number.
 ***************************************************************************/
#define PANOSE_IND_FAMILY      0
#define PANOSE_IND_SERIF       1
#define PANOSE_IND_WEIGHT      2
#define PANOSE_IND_PROPORTION  3
#define PANOSE_IND_CONTRAST    4
#define PANOSE_IND_STROKE      5
#define PANOSE_IND_ARMSTYLE    6
#define PANOSE_IND_LTRFORM     7
#define PANOSE_IND_MIDLINE     8
#define PANOSE_IND_XHEIGHT     9
#define PANOSE_IND__LAST       9

/***************************************************************************
 * SHARED EXPANDED PANOSE DIGIT INDICES
 *
 * Indices into the weight array for the shared Expanded PANOSE digits.
 * These digits are in every Expanded PANOSE number.  These indices are
 * used to access arrays (like the mapper weights) that contain both the
 * shared and 'more' digits in one array.
 ***************************************************************************/
#define PAN_IND_SHARED_WEIGHT  0
#define PAN_IND_SHARED_DISTORT 1
#define PAN_IND_SHARED_MONOSPC 2
#define PAN_IND_SHARED_CONTRST 3
#define PAN_IND_SHARED__LAST   3
#define PAN_COUNT_SHARED       4

/***************************************************************************
 * EXPANDED PANOSE INDICES (LATIN TEXT AND DISPLAY)
 *
 * Indices into the 'more' attributes array for Latin script PANOSE.  These
 * are for indexing the MEMORY copy of the PANOSE number.  To access the
 * 'more' attributes on the DISK copy, they should be added to the constant
 * PAN_DISK_IND_MORE_ATTR.
 ***************************************************************************/
#define PAN_IND_LATIN_SERIF    0
#define PAN_IND_LATIN_RATIO    1
#define PAN_IND_LATIN_SPEED    2
#define PAN_IND_LATIN_STRESS   3
#define PAN_IND_LATIN_ARMTYPE  4
#define PAN_IND_LATIN_CTERM    5
#define PAN_IND_LATIN_SLANT    6
#define PAN_IND_LATIN_ROUND    7
#define PAN_IND_LATIN_SYMMETRY 8
#define PAN_IND_LATIN_MIDLNPOS 9
#define PAN_IND_LATIN_APEX     10
#define PAN_IND_LATIN_XHEIGHT  11
#define PAN_IND_LATIN_DIACRIT  12
#define PAN_IND_LATIN__LAST    12

#ifndef NOELSENIBBLES
/* Indices for nibbles.
 */
#define PAN_NIB_IND_LATIN_SERIF_RATIO   0
#define PAN_NIB_IND_LATIN_SPEED_STRESS  1
#define PAN_NIB_IND_LATIN_ARMTYPE_CTERM 2
#define PAN_NIB_IND_LATIN_SLANT_ROUND   3
#define PAN_NIB_IND_LATIN_SYMMET_MIDLN  4
#define PAN_NIB_IND_LATIN_APEX_XHEIGHT  5
#define PAN_NIB_IND_LATIN_DIACRIT       6
#define PAN_NIB_IND_LATIN__LAST         6
#endif  /* ifndef NOELSENIBBLES */

/***************************************************************************
 * EXPANDED PANOSE DISK INDICES
 *
 * These are indices into the DISK structure for the Expanded PANOSE
 * number.
 ***************************************************************************/
#ifndef NOELSEDISK

#define PAN_DISK_IND_ATTR_MSB   0
#define PAN_DISK_IND_ATTR_LSB   1
#define PAN_DISK_IND_SCRIPT_MSB 2
#define PAN_DISK_IND_SCRIPT_LSB 3
#define PAN_DISK_IND_GENRE_MSB  4
#define PAN_DISK_IND_GENRE_LSB  5
#define PAN_DISK_IND_UNIQUE_MSB 6
#define PAN_DISK_IND_UNIQUE_LSB 7
#define PAN_DISK_IND_WEIGHT     8
#define PAN_DISK_IND_DISTORTION 9
#define PAN_DISK_IND_MONOSPACE  10
#define PAN_DISK_IND_CONTRAST   11
#define PAN_DISK_IND__LAST      11
#define PAN_DISK_COUNT          12

/* This is the base for the 'more' attributes array.  It should
 * be added to the indices for the 'more' digits.
 */
#define PAN_DISK_IND_MORE_ATTR  12

#ifndef NOELSENIBBLES
/* Constants for NIBBLES structure.
 */
#define PAN_NIB_DISK_IND_WEIGHT_DISTORT 8
#define PAN_NIB_DISK_IND_MONOSPC_CNTRST 9
#define PAN_NIB_DISK_IND__LAST          9
#define PAN_NIB_DISK_COUNT              10
#endif  /* ifndef NOELSENIBBLES */

#endif  /* ifndef NOELSEDISK */

/***************************************************************************
 * PENALTY DATABASE DISK INDICES
 *
 * These are the indices into the DISK version of the penalty database
 * structures.
 ***************************************************************************/
#ifndef NOELSEPENALTY
#ifndef NOELSEDISK

/* C2 compression penalty table, which supports a penalty table
 * symmetrical about the diagonal, so only the lower left hand
 * corner of the table is provided.  It is followed by a variable
 * length array of bytes containing the penalty values.  The
 * entire size of the structure is stored in the penalty range
 * record that points to it.
 */
#define PAN_DISK_C2_ANY        0
#define PAN_DISK_C2_NOFIT      1
#define PAN_DISK_C2_MATCH      2
#define PAN_DISK_C2_COUNT      3

/* Penalty range record, which describes the format of the penalty
 * table and contains its offset and size.
 */
#define PAN_DISK_PRNG_LAST            0
#define PAN_DISK_PRNG_COMPRESS        1
#define PAN_DISK_PRNG_OFFSTBL_MSW_MSB 2
#define PAN_DISK_PRNG_OFFSTBL_MSW_LSB 3
#define PAN_DISK_PRNG_OFFSTBL_LSW_MSB 4
#define PAN_DISK_PRNG_OFFSTBL_LSW_LSB 5
#define PAN_DISK_PRNG_TBLSIZE_MSB     6
#define PAN_DISK_PRNG_TBLSIZE_LSB     7
#define PAN_DISK_PRNG_COUNT           8

/* A-to-B remapping array, which maps the 'more' attributes from
 * scriptA to scriptB for a penalty database where scriptA != scriptB.
 * The first two bytes are the length and the next four-byte sets
 * relate A to B.
 */
#define PAN_DISK_ATOB_NUM_MSB    0
#define PAN_DISK_ATOB_NUM_LSB    1
#define PAN_DISK_ATOB_COUNT      2

#define PAN_DISK_ATOB_ITEM_A_MSB 0
#define PAN_DISK_ATOB_ITEM_A_LSB 1
#define PAN_DISK_ATOB_ITEM_B_MSB 2
#define PAN_DISK_ATOB_ITEM_B_LSB 3
#define PAN_DISK_ATOB_ITEM_COUNT 4

/* Index record, which references the penalty database for one
 * scriptA to scriptB combination.
 */
#define PAN_DISK_PIND_NUMATTR_MSB 0
#define PAN_DISK_PIND_NUMATTR_LSB 1
#define PAN_DISK_PIND_SCRIPTA_MSB 2
#define PAN_DISK_PIND_SCRIPTA_LSB 3
#define PAN_DISK_PIND_SCRIPTB_MSB 4
#define PAN_DISK_PIND_SCRIPTB_LSB 5
#define PAN_DISK_PIND_GENREA_MSB  6
#define PAN_DISK_PIND_GENREA_LSB  7
#define PAN_DISK_PIND_GENREB_MSB  8
#define PAN_DISK_PIND_GENREB_LSB  9
#define PAN_DISK_PIND_DEF_ANY     10
#define PAN_DISK_PIND_DEF_NOFIT   11
#define PAN_DISK_PIND_DEF_MATCH   12
#define PAN_DISK_PIND_RESERVED_0  13
#define PAN_DISK_PIND_OFFSRNG_MSB 14
#define PAN_DISK_PIND_OFFSRNG_LSB 15
#define PAN_DISK_PIND_OFFSWT_MSB  16
#define PAN_DISK_PIND_OFFSWT_LSB  17
#define PAN_DISK_PIND_OFFSATOB_MSB 18
#define PAN_DISK_PIND_OFFSATOB_LSB 19
#define PAN_DISK_PIND_COUNT        20

/* Dictionary header.  This is the first thing in the file.
 * it is followed immediately by the array of index records.
 */
#define PAN_DISK_DICT_VERSION_MSB  0
#define PAN_DISK_DICT_VERSION_LSB  1
#define PAN_DISK_DICT_NUMDICTS_MSB 2
#define PAN_DISK_DICT_NUMDICTS_LSB 3
#define PAN_DISK_DICT_SIZE_IND_MSB 4
#define PAN_DISK_DICT_SIZE_IND_LSB 5
#define PAN_DISK_DICT_SIZE_HEAD_MSB   6
#define PAN_DISK_DICT_SIZE_HEAD_LSB   7
#define PAN_DISK_DICT_SIZE_DB_MSW_MSB 8
#define PAN_DISK_DICT_SIZE_DB_MSW_LSB 9
#define PAN_DISK_DICT_SIZE_DB_LSW_MSB 10
#define PAN_DISK_DICT_SIZE_DB_LSW_LSB 11
#define PAN_DISK_DICT_COUNT        12

#endif  /* ifndef NOELSEDISK */
#endif  /* ifndef NOELSEPENALTY */

/***************************************************************************
 * MEM-TO-DISK CONVERSION CONSTANTS
 *
 * These flags may be OR'd together.  The TO_BYTE and TO_NIBBLE flags
 * only apply to conversion of a PANOSE number.
 ***************************************************************************/
#ifndef NOELSEDISK

#define ELSE_CVT_TO_SAME       0
#define ELSE_CVT_TO_BYTE       0x0001
#define ELSE_CVT_TO_NIBBLE     0x0002
#define ELSE_CVT_APPEND_ARRAY  0x0004

#endif  /* ifndef NOELSEDISK */

/***************************************************************************
 * COMPRESSION types for penalty tables
 ***************************************************************************/
#ifndef NOELSEPENALTY

#define PANOSE_COMPRESS_C0     0
#define PANOSE_COMPRESS_C1     1
#define PANOSE_COMPRESS_C2     2
#define PANOSE_COMPRESS__LAST  2

#endif  /* ifndef NOELSEPENALTY */

/***************************************************************************
 * PANOSE_NIBBLES_FLAG
 *
 * This number is OR'd to the uiNumAttr field of the PANOSE number to
 * indicate that the DIGITS are packed into nibbles.  All other parts of
 * the number stay the way they are.
 ***************************************************************************/
#define PANOSE_NIBBLES_FLAG    0x8000

/***************************************************************************
 * PANOSE_PENALTY_VERS
 *
 * Database format version number.
 ***************************************************************************/
#define PANOSE_PENALTY_VERS    0x0100

/***************************************************************************
 * CONSTANTS
 ***************************************************************************/
#define TRUE                   1
#define FALSE                  0

/***************************************************************************
 * Basic types
 ***************************************************************************/
typedef BYTE ELSEFARPTR *LPEBYTE;
typedef BOOL ELSEFARPTR *LPEBOOL;
typedef UINT ELSEFARPTR *LPEUINT;

/***************************************************************************
 * PANOSE NUMBER data structures
 ***************************************************************************/
#ifndef NOELSEDISK
typedef struct _tagPANOSE_NUM_DISK {
   BYTE aj[PAN_DISK_COUNT];
} PANOSE_NUM_DISK, ELSEFARPTR *LPPANOSE_NUM_DISK;
#else
#define LPPANOSE_NUM_DISK LPEBYTE
#endif  /* ifndef NOELSEDISK */

typedef struct _tagPANOSE_NUM_MEM {
   UINT uiNumAttr;
   UINT uiScript;
   UINT uiGenre;
   UINT uiUniqueID;
   BYTE jWeight;
   BYTE jDistortion;
   BYTE jMonospace;
   BYTE jContrast;
   BYTE ajMoreAttr[ELSEDEFCOUNTMORE];
} PANOSE_NUM_MEM, ELSEFARPTR *LPPANOSE_NUM_MEM;

#ifndef NOELSENIBBLES
/* Nibbles versions of these structures.
 */
#ifndef NOELSEDISK
typedef struct _tagPANOSE_NIB_NUM_DISK {
   BYTE aj[PAN_NIB_DISK_COUNT];
} PANOSE_NIB_NUM_DISK, ELSEFARPTR *LPPANOSE_NIB_NUM_DISK;
#else
#define LPPANOSE_NIB_NUM_DISK LPEBYTE
#endif  /* ifndef NOELSEDISK */

typedef struct _tagPANOSE_NIB_NUM_MEM {
   UINT uiNumAttr;
   UINT uiScript;
   UINT uiGenre;
   UINT uiUniqueID;
   BYTE jWeightDistort;
   BYTE jMonospcContrast;
   BYTE ajMoreNibAttr[(ELSEDEFCOUNTMORE + 1) / 2];
} PANOSE_NIB_NUM_MEM, ELSEFARPTR *LPPANOSE_NIB_NUM_MEM;
#endif  /* ifndef NOELSENIBBLES */

/***************************************************************************
 * PANOSE PENALTY TABLE data structures
 *
 * The following structures are fused together into one big block.  See
 * the sample code in ptbl\ptbl.c (provided with the ElseWare Font Mapper
 * source code) for an example of how to build this structure.
 ***************************************************************************/

#ifndef NOELSEPENALTY
#ifndef NOELSEDISK

/* This is followed immediately by an array of bytes (penalty values).
 */
typedef struct _tagPANOSE_PTABLE_C2_DISK {
   BYTE aj[PAN_DISK_C2_COUNT];
} PANOSE_PTABLE_C2_DISK, ELSEFARPTR *LPPANOSE_PTABLE_C2_DISK;

typedef struct _tagPANOSE_PRANGE_DISK {
   BYTE aj[PAN_DISK_PRNG_COUNT];
} PANOSE_PRANGE_DISK, ELSEFARPTR *LPPANOSE_PRANGE_DISK;

typedef struct _tagPANOSE_ATOB_ITEM_DISK {
   BYTE aj[PAN_DISK_ATOB_ITEM_COUNT];
} PANOSE_ATOB_ITEM_DISK, ELSEFARPTR *LPPANOSE_ATOB_ITEM_DISK;

/* This is followed immediately by the array of item records.
 */
typedef struct _tagPANOSE_ATOB_DISK {
   BYTE aj[PAN_DISK_ATOB_COUNT];
} PANOSE_ATOB_DISK, ELSEFARPTR *LPPANOSE_ATOB_DISK;

typedef struct _tagPANOSE_PINDEX_DISK {
   BYTE aj[PAN_DISK_PIND_COUNT];
} PANOSE_PINDEX_DISK, ELSEFARPTR *LPPANOSE_PINDEX_DISK;

/* The header is followed immediately by an array of index records.
 */
typedef struct _tagPANOSE_PDICT_DISK {
   BYTE aj[PAN_DISK_DICT_COUNT];
} PANOSE_PDICT_DISK, ELSEFARPTR *LPPANOSE_PDICT_DISK;

#else

#ifndef NOELSEDISKROUTINES
#define LPPANOSE_PTABLE_C2_DISK LPEBYTE
#define LPPANOSE_PRANGE_DISK LPEBYTE
#define LPPANOSE_ATOB_ITEM_DISK LPEBYTE
#define LPPANOSE_ATOB_DISK LPEBYTE
#define LPPANOSE_PINDEX_DISK LPEBYTE
#define LPPANOSE_PDICT_DISK LPEBYTE
#endif

#endif  /* ifndef NOELSEDISK */

/* C0 compression == full table provided, no structure for it,
 * it is an array of bytes with a size of uiNumAttr * uiNumAttr.
 */

/* C1 compression == smooth penalty range symetrical about the
 * diagonal where closest to diagonal == 1 and each increment
 * away from the diagonal == increment penalty by one.  NO table
 * is provided, and the any, no-fit, and match penalties take
 * on the defaults listed in the index entry for the dictionary.
 * The variables ulOffsTbl and uiTblSize in the range record
 * should be zero.
 */

/* C2 penalties symmetrical around the diagonal but without
 * a smooth range, lower left half specified.
 */
typedef struct _tagPANOSE_PTABLE_C2_MEM {
   BYTE jAnyPenalty;           /* penalty value for 'any' match */
   BYTE jNoFitPenalty;         /* penalty value for 'no fit' match */
   BYTE jMatchPenalty;         /* penalty value for exact match */
   BYTE jPenalties[1];         /* individual penalties (variable length) */
} PANOSE_PTABLE_C2_MEM, ELSEFARPTR *LPPANOSE_PTABLE_C2_MEM;

/* Penalty Range record, which describes the format of the penalty
 * table and contains its offset and size.  There is an array of
 * these at the offset uiOffsRngArray stored in the index record.
 * The first four records in the array are the shared digits weight,
 * distortion, monospace, contrast.  The rest are the 'more' attribute
 * digits which may be accessed with more_attr_ind + PAN_COUNT_SHARED.
 */
typedef struct _tagPANOSE_PRANGE_MEM {
   BYTE jRangeLast;            /* highest possible PANOSE value */
   BYTE jCompress;             /* type of compression */
   ULONG ulOffsTbl;            /* byte offset to penalty table */
   UINT uiTblSize;             /* # bytes in penalty table */
} PANOSE_PRANGE_MEM, ELSEFARPTR *LPPANOSE_PRANGE_MEM;

/* Mapper weights are stored in an array of 1-byte values uiNumAttr
 * long.  The first 4 are for the shared digits, the rest are at
 * more_attr_ind + PAN_COUNT_SHARED.
 */

/* A to B array for 'ajMoreAttr' numbers, which remaps the 'more'
 * attributes from the PANOSE number represented by ScriptA to the
 * PANOSE number represented by ScriptB.  It makes no sense to have
 * a remapping array when ScriptA == ScriptB.  This array is stored
 * at offset uiOffsAttrAtoB.  It will not be present if uiOffsAttrAtoB
 * is zero.  The shared attributes are NOT in this array as they are
 * the same in all numbering systems.  The indices in this array
 * are the 'more' attribute indices.
 */
typedef struct _tagPANOSE_ATOB_ITEM_MEM {
   UINT uiAttrA;               /* PANOSE number index from script A */
   UINT uiAttrB;               /* related PANOSE number ind from script B */
} PANOSE_ATOB_ITEM_MEM, ELSEFARPTR *LPPANOSE_ATOB_ITEM_MEM;

typedef struct _tagPANOSE_ATOB_MEM {
   UINT uiNumAtoB;             /* count of array items */
   PANOSE_ATOB_ITEM_MEM AtoBItem[1]; /* remapping array (variable length) */
} PANOSE_ATOB_MEM, ELSEFARPTR *LPPANOSE_ATOB_MEM;

/* Penalty index record.  Each record contains information for a
 * penalty database, which contains all the penalty tables for one
 * given script & genre combo.  For most databases, scriptA will
 * equal scriptB, which is the penalty database for comparing two
 * PANOSE numbers with the same script and genre values.  But it is
 * possible to have scriptA != scriptB which is for comparing two
 * PANOSE numbers with different script and genres.  In those cases
 * and A-to-B mapping array must be provided which maps the 'more'
 * digits from scriptA to the 'more' digits from script B.
 */
typedef struct _tagPANOSE_PINDEX_MEM {
   UINT uiNumAttr;             /* number of PANOSE numbers for this script */
   UINT uiScriptA;             /* script class of penalty tables */
   UINT uiScriptB;             /* != ScriptA if tbls for unmatched scripts */
   UINT uiGenreA;              /* genre for script A */
   UINT uiGenreB;              /* genre for script B */
   BYTE jDefAnyPenalty;        /* default penalty value for 'any' match */
   BYTE jDefNoFitPenalty;      /* default penalty value for 'no fit' match */
   BYTE jDefMatchPenalty;      /* default penalty value for exact match */
   BYTE jReserved;             /* just to balance out to an even # bytes */
   UINT uiOffsRngArray;        /* byte offs to array of PANOSE_PRANGE recs */
   UINT uiOffsWeights;         /* byte offs to mapper weights */
   UINT uiOffsAttrAtoB;        /* byte offs to array relates script A to B */
} PANOSE_PINDEX_MEM, ELSEFARPTR *LPPANOSE_PINDEX_MEM;

/* Penalty dictionary header.  This is the header to the entire
 * structure.  Its last item is a variable length array of index
 * records.  Each index record points to a penalty database.
 * Each database contains an array of range records that offset
 * to penalty tables.
 */
typedef struct _tagPANOSE_PDICT_MEM {
   UINT uiVersion;             /* version # of this structure */
   UINT uiNumDicts;            /* number of entries in PANOSE_PINDEX array */
   UINT uiSizeIndex;           /* byte count of this + ind */
   UINT uiSizeFullHd;          /* byte count of this + ind + suppl arrays */
   ULONG ulSizeFullDB;         /* byte count of entire structure */
   PANOSE_PINDEX_MEM pindex[1]; /* array PANOSE_PINDEX recs (variable len) */
} PANOSE_PDICT_MEM, ELSEFARPTR *LPPANOSE_PDICT_MEM;

#else

#ifndef NOELSEDISKROUTINES
#define LPPANOSE_PTABLE_C2_DISK LPEBYTE
#define LPPANOSE_PRANGE_DISK LPEBYTE
#define LPPANOSE_ATOB_ITEM_DISK LPEBYTE
#define LPPANOSE_ATOB_DISK LPEBYTE
#define LPPANOSE_PINDEX_DISK LPEBYTE
#define LPPANOSE_PDICT_DISK LPEBYTE
#define LPPANOSE_PTABLE_C2_MEM LPEBYTE
#define LPPANOSE_PRANGE_MEM LPEBYTE
#define LPPANOSE_ATOB_ITEM_MEM LPEBYTE
#define LPPANOSE_ATOB_MEM LPEBYTE
#define LPPANOSE_PINDEX_MEM LPEBYTE
#define LPPANOSE_PDICT_MEM LPEBYTE
#endif

#endif  /* ifndef NOELSEPENALTY */

/***************************************************************************
 * MAP STATE structure
 *
 * This is the data structure passed into most of the mapper routines.
 * It contains within it the ELSE_RES_STATE data structure which must
 * be defined in elseuser.h by the opsys/program vendor including this
 * code.  That structure is subsequently passed into all the resource
 * manager routines (also defined by the opsys/program vendor).
 ***************************************************************************/
typedef struct _tagELSE_MAP_STATE {
   UINT uiThreshold;
   UINT uiRelaxThresholdCount;
   BOOL bUseDefault;
   ELSE_RES_STATE resState;
   PANOSE_NUM_MEM panDefault;
   BYTE ajDefMore[ELSEDEFCOUNTMORE]; /* allow for double the suggested */
   PANOSE_NUM_MEM panTmpWant;
   BYTE ajTmpWantMore[ELSEDEFCOUNTMORE]; /* allow for double the suggested */
   PANOSE_NUM_MEM panTmpThis;
   BYTE ajTmpThisMore[ELSEDEFCOUNTMORE]; /* allow for double the suggested */
} ELSE_MAP_STATE, ELSEFARPTR *LPELSE_MAP_STATE;

/***************************************************************************
 * BYTE vs NIBBLE PANOSE NUMBER DETECTION
 ***************************************************************************/
#define M_PAN_IS_NIBBLE(uiNumAttr) \
   (((UINT)(uiNumAttr) & PANOSE_NIBBLES_FLAG) != 0)
#define M_PAN_SET_NIBBLE(uiNumAttr) \
   ((UINT)(uiNumAttr) | PANOSE_NIBBLES_FLAG)
#define M_PAN_CLEAR_NIBBLE(uiNumAttr) \
   ((UINT)(uiNumAttr) & ~(PANOSE_NIBBLES_FLAG))

/***************************************************************************
 * BYTE AND NIBBLE PACK/UNPACK MACROS
 ***************************************************************************/
#define M_ELSE_GET_MSW(ulNum) ((WORD)((ULONG)(ulNum) >> 16))
#define M_ELSE_GET_LSW(ulNum) ((WORD)(ULONG)(ulNum))
#define M_ELSE_PACK_DWORD(jMSWMSB, jMSWLSB, jLSWMSB, jLSWLSB) \
   ((ULONG)(((ULONG)M_ELSE_PACK_WORD((jMSWMSB), (jMSWLSB)) << 16) | \
   (ULONG)M_ELSE_PACK_WORD((jLSWMSB), (jLSWLSB))))

#define M_ELSE_GET_MSB(uiNum) ((BYTE)((UINT)(uiNum) >> 8))
#define M_ELSE_GET_LSB(uiNum) ((BYTE)(UINT)(uiNum))
#define M_ELSE_PACK_WORD(jMSB, jLSB) \
   ((WORD)(((WORD)(BYTE)(jMSB) << 8) | (WORD)(BYTE)(jLSB)))

#define M_ELSE_GET_MSN(jNum) ((BYTE)((BYTE)(jNum) >> 4))
#define M_ELSE_GET_LSN(jNum) ((BYTE)((BYTE)(jNum) & 0x0F))
#define M_ELSE_PACK_BYTE(jMSN, jLSN) \
   ((BYTE)((((BYTE)(jMSN) & 0x0F) << 4) | ((BYTE)(jLSN) & 0x0F)))

/***************************************************************************
 * SIZE MACROS
 ***************************************************************************/
#define M_SIZEOF_MIN_PANOSE_BYTE() \
   (sizeof(PANOSE_NUM_MEM) - (ELSEDEFCOUNTMORE * sizeof(BYTE)))

#define M_COUNT_MORE_PANOSE_BYTE(uiSize) \
   (((uiSize) < M_SIZEOF_MIN_PANOSE_BYTE()) ? 0 : \
   ((UINT)(((uiSize) - M_SIZEOF_MIN_PANOSE_BYTE()) / sizeof(BYTE))))

#define M_SIZEOF_PANOSE_NUM_BYTE(uiNumAttr) \
   (((uiNumAttr) < PAN_COUNT_SHARED) ? 0 : \
   ((((uiNumAttr) - PAN_COUNT_SHARED) * sizeof(BYTE)) + \
   M_SIZEOF_MIN_PANOSE_BYTE()))

#ifndef NOELSENIBBLES

#define M_SIZEOF_MIN_PANOSE_NIB() \
   (sizeof(PANOSE_NIB_NUM_MEM) - \
   (((ELSEDEFCOUNTMORE + 1) / 2) * sizeof(BYTE)))

#define M_COUNT_MORE_PANOSE_NIB(uiSize) \
   (((uiSize) < M_SIZEOF_MIN_PANOSE_NIB()) ? 0 : \
   ((UINT)((((uiSize) - M_SIZEOF_MIN_PANOSE_NIB()) / sizeof(BYTE)) * 2)))

#define M_SIZEOF_PANOSE_NUM_NIB(uiNumAttr) \
   (((uiNumAttr) < PAN_COUNT_SHARED) ? 0 : \
   (((((uiNumAttr) - PAN_COUNT_SHARED + 1) / 2) * sizeof(BYTE)) + \
   M_SIZEOF_MIN_PANOSE_NIB()))

#endif  /* ifndef NOELSENIBBLES */

/***************************************************************************
 * ELSECVT1.C: Conversion between 10-digit and Expanded PANOSE.
 ***************************************************************************/

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvt10DigitToExpanded
   ELSEARGS (( LPPANOSE_NUM_MEM lpPanThis, UINT uiSizePanThis,
      UINT uiUniqueID, BYTE jFamily, BYTE jSerif, BYTE jWeight,
      BYTE jProportion, BYTE jContrast, BYTE jStroke, BYTE jArmStyle,
      BYTE jLetterform, BYTE jMidline, BYTE jXHeight ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvt10DigitToExpIndirect
   ELSEARGS (( LPPANOSE_NUM_MEM lpPanThis, UINT uiSizePanThis,
      UINT uiUniqueID, LPEBYTE lpjPAN10 ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtExpTo10DigitIndirect
   ELSEARGS (( LPEBYTE lpjPAN10, LPPANOSE_NUM_MEM lpPanThis ));

/***************************************************************************
 * ELSECVT2.C: Conversion between NIBBLE and BYTE format of the
 *             Expanded PANOSE number.
 ***************************************************************************/
#ifndef NOELSENIBBLES

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtExpNibbleToByte
   ELSEARGS (( LPPANOSE_NUM_MEM lpPanByte, UINT uiSizePanByte,
      LPPANOSE_NIB_NUM_MEM lpPanNib ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtExpByteToNibble
   ELSEARGS (( LPPANOSE_NIB_NUM_MEM lpPanNib, UINT uiSizePanNib,
      LPPANOSE_NUM_MEM lpPanByte ));

#endif  /* ifndef NOELSENIBBLES */

#ifndef NOELSEGETSIZEINFO
ELSEEXTERN UINT ELSEFARPROC ELSEPASCAL uiELSECvtExpGetSizeInfo
   ELSEARGS (( LPPANOSE_NUM_MEM lpPanThis, LPEBOOL lpbIsByteFormat,
      LPEUINT lpuiSizeBytes, LPEUINT lpuiSizeNibs ));
#endif

/***************************************************************************
 * ELSECVT3.C: Expanded PANOSE number conversion between memory and disk
 *             format and vice versa.
 ***************************************************************************/
#ifndef NOELSEDISKROUTINES

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtExpPANDiskToMem
   ELSEARGS (( LPPANOSE_NUM_MEM lpPanDst, UINT uiSizeDst, LPEBYTE lpPanSrc,
      UINT uiSizeSrc, UINT uiFlags ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtExpPANMemToDisk
   ELSEARGS (( LPEBYTE lpPanDst, UINT uiSizeDst, LPPANOSE_NUM_MEM lpPanSrc,
      UINT uiSizeSrc, UINT uiFlags ));

#endif

/***************************************************************************
 * ELSECVT4.C: Penalty dictionary conversion between memory and disk
 *             format and vice versa.
 ***************************************************************************/

#ifndef NOELSEDISKROUTINES

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtPHeadDiskToMem
   ELSEARGS (( LPPANOSE_PDICT_MEM lpPHeadDst, LPEBYTE lpPHeadSrc,
      UINT uiSize, UINT uiFlags ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtPIndDiskToMem
   ELSEARGS (( LPPANOSE_PINDEX_MEM lpPIndDst, LPEBYTE lpPIndSrc,
      UINT uiSize, UINT uiFlags, UINT uiNumRecs ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtPA2BDiskToMem
   ELSEARGS (( LPPANOSE_ATOB_MEM lpPA2BDst, LPEBYTE lpPA2BSrc,
      UINT uiSize, UINT uiFlags ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtPRngDiskToMem
   ELSEARGS (( LPPANOSE_PRANGE_MEM lpPRngDst, LPEBYTE lpPRngSrc,
      UINT uiSize, UINT uiFlags, UINT uiNumRecs ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtPC2DiskToMem
   ELSEARGS (( LPPANOSE_PTABLE_C2_MEM lpPC2Dst, LPEBYTE lpPC2Src,
      UINT uiSize, UINT uiFlags ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtPHeadMemToDisk
   ELSEARGS (( LPEBYTE lpPHeadDst, LPPANOSE_PDICT_MEM lpPHeadSrc,
      UINT uiSize, UINT uiFlags ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtPIndMemToDisk
   ELSEARGS (( LPEBYTE lpPIndDst, LPPANOSE_PINDEX_MEM lpPIndSrc,
      UINT uiSize, UINT uiFlags, UINT uiNumRecs ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtPA2BMemToDisk
   ELSEARGS (( LPEBYTE lpPA2BDst, LPPANOSE_ATOB_MEM lpPA2BSrc,
      UINT uiSize, UINT uiFlags ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtPRngMemToDisk
   ELSEARGS (( LPEBYTE lpPRngDst, LPPANOSE_PRANGE_MEM lpPRngSrc,
      UINT uiSize, UINT uiFlags, UINT uiNumRecs ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSECvtPC2MemToDisk
   ELSEARGS (( LPEBYTE lpPC2Dst, LPPANOSE_PTABLE_C2_MEM lpPC2Src,
      UINT uiSize, UINT uiFlags ));

ELSEEXTERN LONG ELSEFARPROC ELSEPASCAL lELSECvtFullPDictDiskToMem
   ELSEARGS (( LPEBYTE lpPDictDst, LPEBYTE lpPDictSrc, UINT uiSize ));

ELSEEXTERN LONG ELSEFARPROC ELSEPASCAL lELSECvtFullPDictMemToDisk
   ELSEARGS (( LPEBYTE lpPDictDst, LPEBYTE lpPDictSrc, UINT uiSize ));

#endif

/***************************************************************************
 * ELSEMAP.C: Font mapping routines.
 ***************************************************************************/

ELSEEXTERN BOOL ELSEFARPROC ELSEPASCAL bELSEMapInit
   ELSEARGS (( LPELSE_MAP_STATE lpMapState ));

ELSEEXTERN VOID ELSEFARPROC ELSEPASCAL vELSEMapClose
   ELSEARGS (( LPELSE_MAP_STATE lpMapState ));

ELSEEXTERN UINT ELSEFARPROC ELSEPASCAL uiELSEMatchFonts
   ELSEARGS (( LPELSE_MAP_STATE lpMapState, LPPANOSE_NUM_MEM lpPanWant,
      LPPANOSE_NUM_MEM lpPanThis ));

ELSEEXTERN VOID ELSEFARPROC ELSEPASCAL vELSEMakeExpPANDummy
   ELSEARGS (( LPPANOSE_NUM_MEM lpPanThis, UINT uiSize ));

#ifndef NOELSEDEFAULTROUTINES
ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSEGetMapDefault
   ELSEARGS (( LPELSE_MAP_STATE lpMapState,
      LPPANOSE_NUM_MEM lpPanDef, UINT uiSizePanDef ));

ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSESetMapDefault
   ELSEARGS (( LPELSE_MAP_STATE lpMapState, LPPANOSE_NUM_MEM lpPanDef ));

ELSEEXTERN BOOL ELSEFARPROC ELSEPASCAL bELSEEnableMapDefault
   ELSEARGS (( LPELSE_MAP_STATE lpMapState, BOOL bEnable ));

ELSEEXTERN BOOL ELSEFARPROC ELSEPASCAL bELSEIsDefaultEnabled
   ELSEARGS (( LPELSE_MAP_STATE lpMapState ));
#endif

#ifndef NOELSETHRESHOLDROUTINES
ELSEEXTERN UINT ELSEFARPROC ELSEPASCAL uiELSEGetMapThreshold
   ELSEARGS (( LPELSE_MAP_STATE lpMapState ));

ELSEEXTERN BOOL ELSEFARPROC ELSEPASCAL bELSESetMapThreshold
   ELSEARGS (( LPELSE_MAP_STATE lpMapState, UINT uiThreshold ));

ELSEEXTERN BOOL ELSEFARPROC ELSEPASCAL bELSEIsThresholdRelaxed
   ELSEARGS (( LPELSE_MAP_STATE lpMapState ));
#endif

#if (!defined(NOELSETHRESHOLDROUTINES) || !defined(NOELSEPICKFONTROUTINE))
ELSEEXTERN VOID ELSEFARPROC ELSEPASCAL vELSERelaxThreshold
   ELSEARGS (( LPELSE_MAP_STATE lpMapState ));

ELSEEXTERN BOOL ELSEFARPROC ELSEPASCAL bELSERestoreThreshold
   ELSEARGS (( LPELSE_MAP_STATE lpMapState ));
#endif

#ifndef NOELSEPICKFONTROUTINE
ELSEEXTERN UINT ELSEFARPROC ELSEPASCAL uiELSEPickFonts
   ELSEARGS (( LPELSE_MAP_STATE lpMapState, LPEUINT lpIndsBest,
      LPEUINT lpMatchValues, LPPANOSE_NUM_MEM lpPanWant,
      UINT uiNumInds, LPPANOSE_NUM_MEM lpPanFirst, UINT uiNumEntries,
      INT iRecSize ));
#endif

#if (!defined(NOELSEPANCPYROUTINE) || !defined(NOELSEDEFAULTROUTINES))
ELSEEXTERN INT ELSEFARPROC ELSEPASCAL iELSEPanCpy
   ELSEARGS (( LPPANOSE_NUM_MEM lpPanDst, LPPANOSE_NUM_MEM lpPanSrc,
      UINT uiSizePanDst ));
#endif

#endif /* ifndef _elsemap_def */

/***************************************************************************
 * Revision log:
 ***************************************************************************/
/*
 * $lgb$
 * 1.0     3-Feb-92  msd Version 1.00 mapper placed under version control.
 * 1.1    14-Feb-92  msd Added ELSEFAR to the definition of ELSEFARPTR,
 *    which explicitly sets the key word 'far' unless changed by the user.
 * $lge$
 */

