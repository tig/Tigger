/***************************************************************************
 * ELSEUSER.H - Supplemental header file with compiler flags for the
 *              ElseWare PANOSE(tm) font mapper.
 *
 * $keywords: elseuser.h 1.1 14-Feb-92 1:40:13 PM$
 *
 * Copyright (C) 1991 ElseWare Corporation.  All rights reserved.
 ***************************************************************************/

/***************************************************************************
 * NO flags -- define these to disable the following features:
 *
 * NOELSEARGS      - Suppress argument list in procedure prototypes.
 * NOELSEEXTERN    - Suppress key word 'extern' on external proc prototypes.
 * NOELSELOCAL     - Suppress key word 'static' on local proc prototypes.
 * NOELSEPASCAL    - Suppress key word 'pascal' on procedures.
 * NOELSEFARPROC   - Suppress key word 'far' on external procedures.
 * NOELSENEARPROC  - Suppress key word 'near' on local procedures.
 * NOELSEFARPTR    - Suppress key word 'far' on far pointers.
 * NOELSEPANCONST  - Suppress long list of PANOSE constants.
 * NOELSEDISK      - Suppress constants and structures for disk support.
 * NOELSENIBBLES   - Suppress constants and structures for nibbles support.
 * NOELSEPENALTY   - Suppress constants and structures for penalty tables.
 *
 * NOELSEDEFAULTROUTINES - No get/set/enable default font routines.
 * NOELSETHRESHOLDROUTINES - No get/set threshold routines.
 * NOELSEPICKFONTROUTINE - No uiELSEPickFonts.
 * NOELSEPANCPYROUTINE - No iELSEPanCpy.
 * NOELSEGETSIZEINFO - No uiELSECvtExpGetSizeInfo.
 * NOELSEDISKROUTINES - No disk-to-mem conversion routines.
 *
 * Redefine these constants/macros to reconfigure for your software.
 *
 * ELSEFAR <far> - Defines the key word for the term 'far' in a far pointer.
 * ELSEINITDEFSTATE <0/1> - Initial state of using the default font.
 * ELSEINITTHRESHSTATE <0/1> - Initial state of relaxing the threshold.
 * ELSEINITTHRESHOLD <val> - Initial threshold value.
 * ELSEDEFCOUNTMORE <val> - Default # of 'more' digits in Expanded PAN num.
 * ELSEMATCHERROR <val>  - Return value for errors.
 * ELSEMAXINT <val> - Max value for an integer.
 * ELSELMUL(a, b)   - Replace (a * b) long math operation.
 * ELSELDIV(a, b)   - Replace (a / b) long operation.
 * ELSEMULDIV(a, b, c)   - Replace (a * b) / c rounded int math operation.
 * ELSEMEMCPY(dst, src, len) - Replace lmemcpy operation.
 *
 * Define these resource manager routines to take over control of how
 * the penalty tables database is accessed.
 *
 * bELSEResInit(lpResState) - Initialize resource manager.
 * vELSEResClose(lpResState) - Shutdown resource manager.
 *
 * lpELSEResLockHead(lpResState) - Lock down PANOSE_PDICT_MEM struct.
 * vELSEResUnlockHead(lpResState, lpPHead) - Unlock.
 *
 * lpELSEResLockHdTbl(lpResState, uiOffs, uiSize) - Lock down A-to-B or
 *         PRange array, technically 'header' tables.
 * vELSEResUnlockHdTbl(lpResState, lpPHdTbl) - Unlock.
 *
 * lpELSEResLockPWeightTbl(lpResState, lpPIndRec, uiOffs, uiSize) - Lock
 *         down the mapper penalty weights table -- you must REDEFINE THIS
 *         if you want the user to be able to modify mapper weights.
 * vELSEResUnlockPWeightTbl(lpResState, lpPWeightTbl) - Unlock.
 *
 * lpELSEResLockPTbl(lpResState, ulOffs, uiSize) - Lock a penalty table.
 * vELSEResUnlockPTbl(lpResState, lpPTbl) - Unlock.
 *
 *
 * ANSI C SYNTAX
 *
 * Define the following flags to conform with ANSI C syntax:
 *
 * #define NOELSEPASCAL
 * #define NOELSENEARPROC
 * #define NOELSEFARPROC
 * #define NOELSEFARPTR
 ***************************************************************************/

#ifndef _elseuser_def
#define _elseuser_def

/**************** SAMPLE FONT MAPPER (Windows) Definitions ****************/

/* Use Windows MulDiv() routine.
 *
 * (ANSI C syntax does not like the explicit 'far').
 */
int far pascal MulDiv(int, int, int);
#define ELSEMULDIV(a, b, c) MulDiv((a), (b), (c))

/* Use far pointers.
 */
#define ELSEFAR _far

/* Resource manager definitions for the sample font mapper.
 *
 * Most of these routines just directly access the memory copy of the
 * table.
 *
 * In the case of Latin text and display PANOSE, the weight table is stored
 * in sample\main.c so the sample program can modify it.  The resource
 * routine for weights returns a pointer to those weights.
 */
#include "elsetype.h"
extern BYTE g_jPDB[];         /* from fontmap\elseres.c */

typedef struct _tagELSE_RES_STATE {
   UINT uiDoNothing;
} ELSE_RES_STATE;
#define LPELSE_RES_STATE ELSEFARPTR *ELSE_RES_STATE

#define bELSEResInit(lpResState) (TRUE)
#define vELSEResClose(lpResState)
#define lpELSEResLockHead(lpResState) ((LPPANOSE_PDICT_MEM)g_jPDB)
#define vELSEResUnlockHead(lpResState, lpPHead)
#define lpELSEResLockHdTbl(lpResState, uiOffs, uiSize) (&g_jPDB[(uiOffs)])
#define vELSEResUnlockHdTbl(lpResState, lpPHdTbl)
#define lpELSEResLockPTbl(lpResState, ulOffs, uiSize) (&g_jPDB[(UINT)(ulOffs)])
#define vELSEResUnlockPTbl(lpResState, lpPTbl)

#define lpELSEResLockPWeightTbl(lpResState, lpPIndRec, uiOffs, uiSize) (&g_jPDB[(uiOffs)])

#define vELSEResUnlockPWeightTbl(lpResState, lpPWeightTbl)

#endif

/***************************************************************************
 * Revision log:
 ***************************************************************************/
/*
 * $lgb$
 * 1.0     3-Feb-92  msd Version 1.00 mapper placed under version control.
 * 1.1    14-Feb-92  msd Added flag ELSEFAR which explicitly defines what
 *    'far' is for far pointers, and for the Windows demo program, set it
 *    to '_huge.'
 * $lge$
 */

