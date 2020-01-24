/***************************************************************************
 * ELSEMAP.C - ElseWare PANOSE(tm) Font Mapper routines.
 *
 * $keywords: elsemap.c 1.0  3-Feb-92 4:58:17 PM$
 *
 * Copyright (C) 1991 ElseWare Corporation.  All rights reserved.
 ***************************************************************************/

#define ELSE_MAPPER_CORE
#include "elsemap.h"

/***************************************************************************/
/**************************** LOCAL DECLARATIONS ***************************/
/***************************************************************************/

ELSELOCAL BOOL ELSENEARPROC ELSEPASCAL bELSEGetIndRec
   ELSEARGS (( LPELSE_MAP_STATE lpMapState,
      LPPANOSE_NUM_MEM ELSEFARPTR *lplpPanWant,
      LPPANOSE_NUM_MEM ELSEFARPTR *lplpPanThis,
      LPPANOSE_PINDEX_MEM lpPIndRec ));

ELSELOCAL BOOL ELSENEARPROC ELSEPASCAL bELSEGetPenaltyC0
   ELSEARGS (( LPEBYTE lpPTbl, LPEUINT lpuiMatch,
      UINT uiTblSize, UINT uiLast, UINT uiAttrA, UINT uiAttrB ));

ELSELOCAL UINT ELSENEARPROC ELSEPASCAL uiELSEGetPenaltyC1
   ELSEARGS (( LPPANOSE_PINDEX_MEM lpPIndRec, UINT uiAttrA, UINT uiAttrB ));

ELSELOCAL BOOL ELSENEARPROC ELSEPASCAL bELSEGetPenaltyC2
   ELSEARGS (( LPPANOSE_PTABLE_C2_MEM lpPC2, LPEUINT lpuiMatch,
      UINT uiTblSize, UINT uiLast, UINT uiAttrA, UINT uiAttrB ));

ELSELOCAL BOOL ELSENEARPROC ELSEPASCAL bELSEMatchNumber
   ELSEARGS (( LPELSE_MAP_STATE lpMapState,
      UINT ELSEFARPTR *lpuiMatchTotal,
      LPPANOSE_PINDEX_MEM lpPIndRec,
      LPPANOSE_PRANGE_MEM lpPRngRec,
      UINT uiWeight, UINT uiAttrA, UINT uiAttrB ));

#ifdef ELSEUSELOCALMEMCPY
ELSELOCAL UINT ELSENEARPROC ELSEPASCAL uiELSEMemCpy
   ELSEARGS (( LPEBYTE lpjDst, LPEBYTE lpjSrc, UINT uiLen ));
#endif

/***************************************************************************/
/*************************** CORE MAPPER ROUTINES **************************/
/***************************************************************************/

/***************************************************************************
 * FUNCTION: bELSEMapInit
 *
 * PURPOSE:  Initialize the font mapper.  Fill in the default settings
 *           and initialize the resource manager.
 *
 * RETURNS:  Return TRUE if successfully initialized, FALSE otherwise.
 ***************************************************************************/
BOOL ELSEFARPROC ELSEPASCAL bELSEMapInit (
   LPELSE_MAP_STATE lpMapState )
{
   /* Initialize resource manager.
    */
   if (!bELSEResInit(&lpMapState->resState))
      return (FALSE);

   /* Fill in defaults.
    */
   lpMapState->uiThreshold = ELSEINITTHRESHOLD;

#if ELSEINITTHRESHSTATE
   lpMapState->uiRelaxThresholdCount = 1;
#else
   lpMapState->uiRelaxThresholdCount = 0;
#endif

#if ELSEINITDEFSTATE
   lpMapState->bUseDefault = TRUE;
#else
   lpMapState->bUseDefault = FALSE;
#endif

   /* Initial default font is PANOSE number for Courier.
    */
   iELSECvt10DigitToExpanded(&lpMapState->panDefault,
      sizeof(lpMapState->panDefault) + sizeof(lpMapState->ajDefMore),
      PANOSE_ANY,                /* unique id  */
      FAMILY_LATTEXTANDDISP,     /* family     */
      SERIF_THIN,                /* serif      */
      WEIGHT_THIN,               /* weight     */
      PROPORTION_MONOSPACE,      /* proportion */
      CONTRAST_NONE,             /* contrast   */
      STROKE_GRADVERT,           /* stroke     */
      ARM_STRAIGHTSGLSERIF,      /* arm style  */
      LTRFORM_NORMCONTACT,       /* letterform */
      MIDLINE_STDSERIFED,        /* midline    */
      XHEIGHT_CONSTLARGE         /* x-height   */
   );
   return (TRUE);
}

/***************************************************************************
 * FUNCTION: vELSEMapClose
 *
 * PURPOSE:  Close the font mapper.
 *
 * RETURNS:  Nothing.
 ***************************************************************************/
VOID ELSEFARPROC ELSEPASCAL vELSEMapClose (
   LPELSE_MAP_STATE lpMapState )
{
   vELSEResClose(&lpMapState->resState);
}

/***************************************************************************
 * FUNCTION: uiELSEMatchFonts
 *
 * PURPOSE:  Match two PANOSE numbers.
 *
 * RETURNS:  Return a match value if the fonts are successfully compared
 *           and are within range of the threshold, otherwise return
 *           ELSEMATCHERROR if there is an error or the fonts are out
 *           of range.
 ***************************************************************************/
UINT ELSEFARPROC ELSEPASCAL uiELSEMatchFonts (
   LPELSE_MAP_STATE lpMapState,
   LPPANOSE_NUM_MEM lpPanWant,
   LPPANOSE_NUM_MEM lpPanThis )
{
   UINT uiMatch = ELSEMATCHERROR;
   UINT uiThreshold;
   UINT uiNumAttrWant;
   UINT uiNumAttrThis;
   UINT uiNum;
   UINT i;
   LPPANOSE_PRANGE_MEM lpPRngRec;
   LPEBYTE lpjWeights;
   LPEBYTE lpjAttrWant;
   LPEBYTE lpjAttrThis;
   LPPANOSE_ATOB_MEM lpPA2BRec;
   LPPANOSE_ATOB_ITEM_MEM lpPA2BItem;
   PANOSE_PINDEX_MEM panIndRec;

   /* Both numbers must be in BYTE format.  Convert from
    * NIBBLE to BYTE.
    */
   if (M_PAN_IS_NIBBLE(lpPanWant->uiNumAttr)) {
      if (iELSECvtExpNibbleToByte(&lpMapState->panTmpWant,
         sizeof(lpMapState->panTmpWant) + sizeof(lpMapState->ajTmpWantMore),
         (LPPANOSE_NIB_NUM_MEM)lpPanWant) == 0) {
         goto backout0;
      }
      lpPanWant = &lpMapState->panTmpWant;
   }
   if (M_PAN_IS_NIBBLE(lpPanThis->uiNumAttr)) {
      if (iELSECvtExpNibbleToByte(&lpMapState->panTmpThis,
         sizeof(lpMapState->panTmpThis) + sizeof(lpMapState->ajTmpThisMore),
         (LPPANOSE_NIB_NUM_MEM)lpPanThis) == 0) {
         goto backout0;
      }
      lpPanThis = &lpMapState->panTmpThis;
   }
   /* Each number must contain at least the base 4 digits.
    */
   if ((lpPanWant->uiNumAttr < PAN_COUNT_SHARED) ||
      (lpPanThis->uiNumAttr < PAN_COUNT_SHARED)) {
      goto backout0;
   }
   /* Locate the index entry that points to the dictionary containing
    * the penalty tables for this pair of PANOSE numbers.
    *
    * This routine may flip what lpPanWant and lpPanThis point to so
    * we can guarantee ScriptA/GenreA from the penalty tables is always
    * associated with lpPanWant and ScriptB/GenreB is associated with
    * lpPanThis.
    */
   if (!bELSEGetIndRec(lpMapState, &lpPanWant, &lpPanThis, &panIndRec)) {
      goto backout0;
   }
   /* Sanity check, the dictionary must at least contain penalty
    * tables for the standard 4 digits.
    */
   if ((panIndRec.uiNumAttr < PAN_COUNT_SHARED) ||
      !panIndRec.uiOffsWeights || !panIndRec.uiOffsRngArray) {
      goto backout0;
   }
   /* Lock down the array of mapper weights.  We leave the onus on
    * the opsys or app vendor to implement special support for
    * user-configurable weights.  We pass panIndRec so the routine
    * can look at the script A/B combination.
    */
   if (!(lpjWeights = lpELSEResLockPWeightTbl(&lpMapState->resState,
      &panIndRec, panIndRec.uiOffsWeights,
      panIndRec.uiNumAttr * sizeof(BYTE)))) {
      goto backout0;
   }
   /* Lock down the range array that contains the address of the
    * penalty tables corresponding to each digit.  The first
    * 4 entries in this array are for the standard 4 digits in
    * every PANOSE number.  The remainder are the 'more' attribute
    * digits.
    */
   if (!(lpPRngRec = (LPPANOSE_PRANGE_MEM)lpELSEResLockHdTbl(
      &lpMapState->resState, panIndRec.uiOffsRngArray,
      panIndRec.uiNumAttr * sizeof(PANOSE_PRANGE_MEM)))) {
      goto backout1;
   }
   /* If there is an array that remaps the digits between the two
    * PANOSE numbers (which will happen if the two numbers are from
    * separate scripts) then also lock that table.  A null offset
    * indicates the table is not present.
    *
    * This table only maps the 'more' attribute digits as the standard
    * 4 are expected to be in every PANOSE number, so the number of
    * entries is uiNumAttr - PAN_COUNT_SHARED.
    */
   if (panIndRec.uiOffsAttrAtoB &&
      (panIndRec.uiNumAttr > PAN_COUNT_SHARED)) {
      if (!(lpPA2BRec = (LPPANOSE_ATOB_MEM)lpELSEResLockHdTbl(
         &lpMapState->resState, panIndRec.uiOffsAttrAtoB,
         (panIndRec.uiNumAttr - PAN_COUNT_SHARED) *
         sizeof(PANOSE_ATOB_MEM)))) {
         goto backout2;
      }
   } else {
      lpPA2BRec = 0L;
   }
   /* Optimization for threshold testing:  If we're supposed to
    * return ELSEMATCHERROR if the number is beyond the threshold,
    * then test after every digit match and abort immediatelty
    * once the number is out of range.  If not testing, then just
    * set the threshold to a really large number so the match value
    * never exceeds it.
    */
   if (lpMapState->uiRelaxThresholdCount > 0) {
      uiThreshold = ELSEMAXINT;
   } else {
      uiThreshold = lpMapState->uiThreshold;
   }
   /* First walk the standard 4 digits computing their distances.
    */
   uiMatch = 0;
   if (!bELSEMatchNumber(lpMapState, &uiMatch, &panIndRec, lpPRngRec++,
      *lpjWeights++, lpPanWant->jWeight, lpPanThis->jWeight)) {
      goto backout3;
   }
   if (!bELSEMatchNumber(lpMapState, &uiMatch, &panIndRec, lpPRngRec++,
      *lpjWeights++, lpPanWant->jDistortion, lpPanThis->jDistortion)) {
      goto backout3;
   }
   if (!bELSEMatchNumber(lpMapState, &uiMatch, &panIndRec, lpPRngRec++,
      *lpjWeights++, lpPanWant->jMonospace, lpPanThis->jMonospace)) {
      goto backout3;
   }
   if (!bELSEMatchNumber(lpMapState, &uiMatch, &panIndRec, lpPRngRec++,
      *lpjWeights++, lpPanWant->jContrast, lpPanThis->jContrast)) {
      goto backout3;
   }
   /* Abort now if the number is already beyond the threshold.
    */
   if (uiMatch > uiThreshold) {
      uiMatch = ELSEMATCHERROR;
      goto backout3;
   }
   /* Walk the 'more' attributes computing their distances.
    */
   if (lpPA2BRec) {

      /* Walk via remapping array.
       */
      for (i = 0, uiNumAttrWant = lpPanWant->uiNumAttr - PAN_COUNT_SHARED,
         uiNumAttrThis = lpPanThis->uiNumAttr - PAN_COUNT_SHARED,
         lpjAttrWant = lpPanWant->ajMoreAttr,
         lpjAttrThis = lpPanThis->ajMoreAttr,
         lpPA2BItem = lpPA2BRec->AtoBItem; i < lpPA2BRec->uiNumAtoB;
         ++i, ++lpPRngRec, ++lpjWeights, ++lpPA2BItem) {

         /* Compute the 'no fit' penalty for 'more' attribute
          * digits that are not present.
          *
          * Otherwise, compute the values via the remapping array.
          */
         if ((lpPA2BItem->uiAttrA >= uiNumAttrWant) ||
            (lpPA2BItem->uiAttrB >= uiNumAttrThis)) {

            /* At least one number is out of range so give it the
             * 'no fit' penalty.
             */
            if (!bELSEMatchNumber(lpMapState, &uiMatch, &panIndRec,
               lpPRngRec, *lpjWeights, PANOSE_NOFIT, PANOSE_NOFIT)) {
               goto backout3;
            }
         } else if (!bELSEMatchNumber(lpMapState, &uiMatch, &panIndRec,
            lpPRngRec, *lpjWeights, lpjAttrWant[lpPA2BItem->uiAttrA],
            lpjAttrThis[lpPA2BItem->uiAttrB])) {
            goto backout3;
         }
         /* Abort without testing all the digits if the number
          * is already beyond the threshold.
          */
         if (uiMatch > uiThreshold) {
            uiMatch = ELSEMATCHERROR;
            goto backout3;
         }
      }
   } else {

      /* Walk without a remapping array.  Compute count of values
       * in the two PANOSE numbers.
       */
      uiNum = lpPanWant->uiNumAttr < lpPanThis->uiNumAttr ?
         lpPanWant->uiNumAttr : lpPanThis->uiNumAttr;
      if (panIndRec.uiNumAttr < uiNum)
         uiNum = panIndRec.uiNumAttr;

      /* Walk each 'more' digit (note our count variable 'i' starts
       * at the count of shared digits, so we'll walk the count of
       * 'more' digits).
       */
      for (i = PAN_COUNT_SHARED, lpjAttrWant = lpPanWant->ajMoreAttr,
         lpjAttrThis = lpPanThis->ajMoreAttr; i < uiNum;
         ++i, ++lpjAttrWant, ++lpjAttrThis, ++lpPRngRec, ++lpjWeights) {

         /* Compute the match.
          */
         if (!bELSEMatchNumber(lpMapState, &uiMatch, &panIndRec,
            lpPRngRec, *lpjWeights, *lpjAttrWant, *lpjAttrThis)) {
            goto backout3;
         }
         /* Abort without testing all the digits if the number
          * is already beyond the threshold.
          */
         if (uiMatch > uiThreshold) {
            uiMatch = ELSEMATCHERROR;
            goto backout3;
         }
      }
      /* Any digits not in the numbers get the 'no fit' penalty.
       */
      for (; i < panIndRec.uiNumAttr;
         ++i, ++lpjAttrWant, ++lpjAttrThis, ++lpPRngRec, ++lpjWeights) {

         /* Compute the 'no fit' penalty.
          */
         if (!bELSEMatchNumber(lpMapState, &uiMatch, &panIndRec,
            lpPRngRec, *lpjWeights, PANOSE_NOFIT, PANOSE_NOFIT)) {
            goto backout3;
         }
         /* Abort if the number is already beyond the threshold.
          */
         if (uiMatch > uiThreshold) {
            uiMatch = ELSEMATCHERROR;
            goto backout3;
         }
      }
   }
   /* Unlock all the tables.
    */
backout3:
   if (lpPA2BRec)
      vELSEResUnlockHdTbl(&lpMapState->resState, (LPEBYTE)lpPA2BRec);
backout2:
   vELSEResUnlockHdTbl(&lpMapState->resState, (LPEBYTE)lpPRngRec);
backout1:
   vELSEResUnlockPWeightTbl(&lpMapState->resState, lpjWeights);
backout0:
   /* Return the match value.  If we encountered an error or a match
    * out of range, then it will equal ELSEMATCHERROR.
    */
   return (uiMatch);
}

/***************************************************************************
 * FUNCTION: vELSEMakeExpPANDummy
 *
 * PURPOSE:  Build a dummy Expanded PANOSE number in BYTE format with
 *           all attributes set to PANOSE_NOFIT.
 *
 *           This routine is used internally by the mapper to put
 *           something valid in the PANOSE number in the event it is
 *           going to do an error return.
 *
 * RETURNS:  Nothing.
 ***************************************************************************/
VOID ELSEFARPROC ELSEPASCAL vELSEMakeExpPANDummy (
   LPPANOSE_NUM_MEM lpPanThis,
   UINT uiSize )
{
   if (uiSize < sizeof(UINT))
      return;
   lpPanThis->uiNumAttr = 0;

   if (uiSize < (sizeof(UINT) * 2))
      return;
   lpPanThis->uiScript = PANOSE_NOFIT;

   if (uiSize < (sizeof(UINT) * 3))
      return;
   lpPanThis->uiGenre = PANOSE_NOFIT;

   if (uiSize < (sizeof(UINT) * 4))
      return;
   lpPanThis->uiUniqueID = PANOSE_ANY;

   if (uiSize < ((sizeof(UINT) * 4) + sizeof(BYTE)))
      return;
   lpPanThis->jWeight = PANOSE_NOFIT;

   if (uiSize < ((sizeof(UINT) * 4) + (sizeof(BYTE) * 2)))
      return;
   lpPanThis->jDistortion = PANOSE_NOFIT;

   if (uiSize < ((sizeof(UINT) * 4) + (sizeof(BYTE) * 3)))
      return;
   lpPanThis->jMonospace = PANOSE_NOFIT;

   if (uiSize < ((sizeof(UINT) * 4) + (sizeof(BYTE) * 4)))
      return;
   lpPanThis->jContrast = PANOSE_NOFIT;
}

/***************************************************************************/
/************************** Default Font Routines **************************/
/***************************************************************************/

#ifndef NOELSEDEFAULTROUTINES
/***************************************************************************
 * FUNCTION: iELSEGetMapDefault
 *
 * PURPOSE:  Fill in the passed-in PANOSE number structure with the
 *           default font.
 *
 *           This routine creates a BYTE format Expanded PANOSE number,
 *           the caller must use iELSECvtExpByteToNibble to change it
 *           into NIBBLE format.
 *
 * RETURNS:  Return 0 if the default number was not copied (passed-in
 *           structure too small), a positive count if all the numbers
 *           in the default copied, a negative count if less that the
 *           total count of numbers copied (the inverse of the negative
 *           is the count copied).
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSEGetMapDefault (
   LPELSE_MAP_STATE lpMapState,
   LPPANOSE_NUM_MEM lpPanDef,
   UINT uiSizePanDef )
{
   /* Copy the default font if a PANOSE number is passed in.
    */
   if (lpPanDef)
      return(iELSEPanCpy(lpPanDef, &lpMapState->panDefault, uiSizePanDef));

   return (0);
}

/***************************************************************************
 * FUNCTION: iELSESetMapDefault
 *
 * PURPOSE:  Change the PANOSE(tm) digits for the default font.  This
 *           routine does not enable usage of the font, the routine
 *           bELSEEnableMapDefault must be called to do that.
 *
 * RETURNS:  The return value is the count of digits copied to the
 *           destination.  It is zero if an error occurred, or negative
 *           if fewer digits were copied than were available.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSESetMapDefault (
   LPELSE_MAP_STATE lpMapState,
   LPPANOSE_NUM_MEM lpPanDef )
{
   /* Copy the default font.  If it is in NIBBLE format then convert
    * it to BYTE format for our internal structure.
    */
   if (lpPanDef) {
      if (M_PAN_IS_NIBBLE(lpPanDef->uiNumAttr)) {
         return (iELSECvtExpNibbleToByte(&lpMapState->panDefault,
            sizeof(lpMapState->panDefault) + sizeof(lpMapState->ajDefMore),
            (LPPANOSE_NIB_NUM_MEM)lpPanDef));
      } else {
         return (iELSEPanCpy(&lpMapState->panDefault, lpPanDef,
            sizeof(lpMapState->panDefault) + sizeof(lpMapState->ajDefMore)));
      }
   }
   return (0);
}

/***************************************************************************
 * FUNCTION: bELSEEnableMapDefault
 *
 * PURPOSE:  Enable/disable usage of the default font.
 *
 * RETURNS:  Return the previous usage state.
 ***************************************************************************/
BOOL ELSEFARPROC ELSEPASCAL bELSEEnableMapDefault (
   LPELSE_MAP_STATE lpMapState,
   BOOL bEnable )
{
   BOOL bPrev = lpMapState->bUseDefault;

   lpMapState->bUseDefault = bEnable;
   return (bPrev);
}

/***************************************************************************
 * FUNCTION: bELSEIsDefaultEnabled
 *
 * PURPOSE:  This function gets the state of using the default font.
 *
 * RETURNS:  Return TRUE if usage of the default font is enabled.
 ***************************************************************************/
BOOL ELSEFARPROC ELSEPASCAL bELSEIsDefaultEnabled (
   LPELSE_MAP_STATE lpMapState )
{
   return (lpMapState->bUseDefault);
}
#endif

/***************************************************************************/
/**************************** Threshold Routines ***************************/
/***************************************************************************/

#ifndef NOELSETHRESHOLDROUTINES
/***************************************************************************
 * FUNCTION: uiELSEGetMapThreshold
 *
 * PURPOSE:  This function gets the state of using threshold checking
 *           in the mapper.
 *
 * RETURNS:  Return the match threshold.
 ***************************************************************************/
UINT ELSEFARPROC ELSEPASCAL uiELSEGetMapThreshold (
   LPELSE_MAP_STATE lpMapState )
{
   return (lpMapState->uiThreshold);
}

/***************************************************************************
 * FUNCTION: bELSESetMapThreshold
 *
 * PURPOSE:  Change the match threshold.
 *
 * RETURNS:  Return TRUE if the threshold is changed, FALSE if it is
 *           equal to the match error value and therefore rejected.
 ***************************************************************************/
BOOL ELSEFARPROC ELSEPASCAL bELSESetMapThreshold (
   LPELSE_MAP_STATE lpMapState,
   UINT uiThreshold )
{
   /* Cannot set a threshold equal to the error value.
    */
   if (uiThreshold == ELSEMATCHERROR)
      return (FALSE);

   /* Set new threshold.
    */
   lpMapState->uiThreshold = uiThreshold;
   return (TRUE);
}

/***************************************************************************
 * FUNCTION: bELSEIsThresholdRelaxed
 *
 * PURPOSE:  This function gets the state of using the threshold in
 *           mapping.
 *
 * RETURNS:  Return TRUE if the match threshold is relaxed, FALSE otherwise.
 ***************************************************************************/
BOOL ELSEFARPROC ELSEPASCAL bELSEIsThresholdRelaxed (
   LPELSE_MAP_STATE lpMapState )
{
   return (lpMapState->uiRelaxThresholdCount > 0);
}
#endif

#if (!defined(NOELSETHRESHOLDROUTINES) || !defined(NOELSEPICKFONTROUTINE))
/***************************************************************************
 * FUNCTION: vELSERelaxThreshold
 *
 * PURPOSE:  Temporarily relax the threshold variable so every font
 *           except the erroneous ones will return a match value.
 *
 * RETURNS:  Nothing.
 ***************************************************************************/
VOID ELSEFARPROC ELSEPASCAL vELSERelaxThreshold (
   LPELSE_MAP_STATE lpMapState )
{
   ++lpMapState->uiRelaxThresholdCount;
}

/***************************************************************************
 * FUNCTION: bELSERestoreThreshold
 *
 * PURPOSE:  Restore mapping within a threshold.
 *
 * RETURNS:  Return TRUE if the threshold is back in effect, FALSE if
 *           someone else has relaxed it too so it still is relaxed.
 ***************************************************************************/
BOOL ELSEFARPROC ELSEPASCAL bELSERestoreThreshold (
   LPELSE_MAP_STATE lpMapState )
{
   if (lpMapState->uiRelaxThresholdCount > 0)
      return (--lpMapState->uiRelaxThresholdCount == 0);
   else
      return (TRUE);
}
#endif

/***************************************************************************/
/**************************** Pick Font Routine ****************************/
/***************************************************************************/

#ifndef NOELSEPICKFONTROUTINE
/***************************************************************************
 * FUNCTION: uiELSEPickFonts
 *
 * PURPOSE:  Walk an array of fonts ordering them by the closest to the
 *           requested font.  If no font is within range of the threshold
 *           then look for the closest to the default font.  If still
 *           not font is found then just pick the first font in the list.
 *
 * RETURNS:  Return the number of fonts found to match the requested
 *           font, or zero if uiNumInds == 0 or an error ocurred.  If no
 *           match was found within the range of the threshold (assuming
 *           threshold enabled), then *lpMatchValue > threshold if the
 *           default font was returned (if default font enabled), or
 *           *lpMatchValue == ELSEMATCHERROR if default disabled in which
 *           case the mapper arbitrarily picked the first font in the list.
 ***************************************************************************/
UINT ELSEFARPROC ELSEPASCAL uiELSEPickFonts (
   LPELSE_MAP_STATE lpMapState,
   LPEUINT lpIndsBest,
   LPEUINT lpMatchValues,
   LPPANOSE_NUM_MEM lpPanWant,
   UINT uiNumInds,
   LPPANOSE_NUM_MEM lpPanFirst,
   UINT uiNumEntries,
   INT iRecSize )
{
   UINT uiMatchValue;
   UINT j;
   UINT uiNumFound = 0;
   UINT i;
   UINT k;
   LPEUINT lpInds;
   LPEUINT lpMatches;
   LPEBYTE lpjPNum;

   /* Sanity check.
    */
   if ((uiNumInds == 0) || (uiNumEntries == 0) ||
      (iRecSize < 0 && iRecSize > -(int)M_SIZEOF_MIN_PANOSE_NIB()) ||
      (iRecSize > 0 && iRecSize < M_SIZEOF_MIN_PANOSE_NIB())) {
      return (0);
   }
   /* If the 'want' number is in NIBBLE format then convert
    * it to BYTE format.  This is a performance optimization
    * because it prevents having uiELSEMatchFonts do the
    * conversion on every call.
    */
   if (M_PAN_IS_NIBBLE(lpPanWant->uiNumAttr)) {
      if (iELSECvtExpNibbleToByte(&lpMapState->panTmpWant,
         sizeof(lpMapState->panTmpWant) + sizeof(lpMapState->ajTmpWantMore),
         (LPPANOSE_NIB_NUM_MEM)lpPanWant) == 0) {
         return (0);
      }
      lpPanWant = &lpMapState->panTmpWant;
   }
   /* Walk each of the set of PANOSE(tm) digits ordering them by
    * best to worst match.  Walk the array with a byte pointer,
    * advancing by the passed in record size.
    */
   for (i = 0, lpjPNum = (LPEBYTE)lpPanFirst; i < uiNumEntries;
      ++i, lpjPNum += iRecSize) {

      /* Get the match value.
       */
      if ((uiMatchValue = uiELSEMatchFonts(lpMapState, lpPanWant,
         (LPPANOSE_NUM_MEM)lpjPNum)) != ELSEMATCHERROR) {

         /* Find the slot in the array where this match value
          * should reside.
          */
         for (j = 0, lpMatches = lpMatchValues;
            (j < uiNumFound) && (*lpMatches < uiMatchValue);
            ++j, ++lpMatches)
            ;

         /* If this match value is better than one of the matches
          * already in the array, then insert it.  Notice that
          * until the array is full everything goes in it.  After
          * that, we shuffle less close matches back and insert this
          * one.
          */
         if (j < uiNumInds) {
            if (uiNumFound < uiNumInds)
               ++uiNumFound;
            for (lpInds = &lpIndsBest[k = uiNumFound - 1],
               lpMatches = &lpMatchValues[k];
               k > j;
               lpInds[0] = lpInds[-1], lpMatches[0] = lpMatches[-1],
               --k, --lpInds, --lpMatches)
               ;
            *lpInds = i;
            *lpMatches = uiMatchValue;
         }
      }
   }
   /* If no acceptable match was found, then attempt to find a match
    * for the default font.  We temporarily step off the threshold
    * so we will definitely find something.  At this point, we do
    * not care if the default is not within the threshold, we just
    * want to find it.
    */
   if (!uiNumFound && lpMapState->bUseDefault) {
      vELSERelaxThreshold(lpMapState);

      for (i = 0, lpjPNum = (LPEBYTE)lpPanFirst; i < uiNumEntries;
         ++i, lpjPNum += iRecSize) {

         if ((uiMatchValue = uiELSEMatchFonts(lpMapState,
            &lpMapState->panDefault,
            (LPPANOSE_NUM_MEM)lpjPNum)) != ELSEMATCHERROR) {

            if (uiNumFound == 0) {
               *lpIndsBest = i;
               *lpMatchValues = uiMatchValue;
               ++uiNumFound;
            } else if (uiMatchValue < *lpMatchValues) {
               *lpIndsBest = i;
               *lpMatchValues = uiMatchValue;
            }
         }
      }
      /* Restore true threshold.
       */
      bELSERestoreThreshold(lpMapState);

      /* We flag this match with the error so the caller can
       * determine that the default font was substituted.
       */
      if (uiNumFound > 0)
         *lpMatchValues = ELSEMATCHERROR;
   }
   /* If still no match is found then just pick the first font.
    */
   if (!uiNumFound) {
      *lpIndsBest = 0;
      *lpMatchValues = ELSEMATCHERROR;
      ++uiNumFound;
   }
   /* Return the number of fonts found.  It will be zero if we
    * encountered an error.
    */
   return (uiNumFound);
}
#endif

/***************************************************************************/
/************************ PANOSE Number Copy Routine ***********************/
/***************************************************************************/

#if (!defined(NOELSEPANCPYROUTINE) || !defined(NOELSEDEFAULTROUTINES))
/***************************************************************************
 * FUNCTION: iELSEPanCpy
 *
 * PURPOSE:  Copy a PANOSE number from one place to another.  If the
 *           destination number is not large enough to hold all the
 *           'more attr' digits then copy as many as possible.
 *
 *           This routine keeps the same format of the Expanded PANOSE
 *           number.  That is, it copies BYTE format to BYTE format, or
 *           NIBBLE format to NIBBLE format.  To convert from BYTE to
 *           NIBBLE or vice versa the caller should use
 *           iELSECvtExpNibbleToByte or iELSECvtExpByteToNibble.
 *
 * RETURNS:  Return 0 if the number is not copied, a positive count if
 *           all the digits copied, or a negative count if fewer than
 *           the total were copied (the inverse of the negative shows
 *           the count of digits copied).
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSEPanCpy (
   LPPANOSE_NUM_MEM lpPanDst,
   LPPANOSE_NUM_MEM lpPanSrc,
   UINT uiSizePanDst )
{
   BOOL bTooSmall;
   UINT uiNum;

   /* Handle BYTE or NIBBLE format.
    */
   if (M_PAN_IS_NIBBLE(lpPanSrc->uiNumAttr)) {

      /* NIBBLE format.
       *
       * If we didn't get at least the main part of the PANOSE
       * number then return zero.
       *
       * Notice we don't ever try to access the digits, so we're
       * okay using the BYTE format pointers without transferring
       * them.
       */
      if (!M_PAN_IS_NIBBLE(lpPanSrc->uiNumAttr) ||
         (M_PAN_CLEAR_NIBBLE(lpPanSrc->uiNumAttr) < PAN_COUNT_SHARED) ||
         (uiSizePanDst < M_SIZEOF_MIN_PANOSE_NIB())) {
         vELSEMakeExpPANDummy(lpPanDst, uiSizePanDst);
         return (0);
      }
      /* Set the count of digits we will copy based upon how
       * many we have and what we have room for.
       */
      uiNum = M_COUNT_MORE_PANOSE_NIB(uiSizePanDst) + PAN_COUNT_SHARED;
      if (uiNum >= M_PAN_CLEAR_NIBBLE(lpPanSrc->uiNumAttr)) {
         uiNum = M_PAN_CLEAR_NIBBLE(lpPanSrc->uiNumAttr);
         bTooSmall = FALSE;
      } else {
         bTooSmall = TRUE;
      }
      /* Copy the number and make sure the count variable represents
       * the number of digits we really copied.
       */
      ELSEMEMCPY((LPEBYTE)lpPanDst, (LPEBYTE)lpPanSrc,
         M_SIZEOF_PANOSE_NUM_NIB(uiNum));
      lpPanDst->uiNumAttr = M_PAN_SET_NIBBLE(uiNum);

   } else {

      /* BYTE format.
       *
       * If we didn't get at least the main part of the PANOSE
       * number then return zero.
       */
      if ((lpPanSrc->uiNumAttr < PAN_COUNT_SHARED) ||
         (uiSizePanDst < M_SIZEOF_MIN_PANOSE_BYTE())) {
         vELSEMakeExpPANDummy(lpPanDst, uiSizePanDst);
         return (0);
      }
      /* Set the count of digits we will copy based upon how
       * many we have and what we have room for.
       */
      uiNum = M_COUNT_MORE_PANOSE_BYTE(uiSizePanDst) + PAN_COUNT_SHARED;
      if (uiNum >= lpPanSrc->uiNumAttr) {
         uiNum = lpPanSrc->uiNumAttr;
         bTooSmall = FALSE;
      } else {
         bTooSmall = TRUE;
      }
      /* Copy the number and make sure the count variable represents
       * the number of digits we really copied.
       */
      ELSEMEMCPY((LPEBYTE)lpPanDst, (LPEBYTE)lpPanSrc,
         M_SIZEOF_PANOSE_NUM_BYTE(uiNum));
      lpPanDst->uiNumAttr = uiNum;
   }
   /* Return neg count if less than what's available.
    */
   return (bTooSmall ? -(int)uiNum : uiNum);
}
#endif

/***************************************************************************/
/************************** LOCAL SERVICE ROUTINES *************************/
/***************************************************************************/

/***************************************************************************
 * FUNCTION: bELSEGetIndRec
 *
 * PURPOSE:  Search the header of the database looking for a dictionary
 *           of penalty tables designed for this script/genre pair.
 *
 *           We may actually switch which number lpPanWant points to
 *           versus which number lpPanThis points to so we consistently
 *           associate lpPanWant with scriptA/genreA and lpPanThis with
 *           scriptB/genreB.
 *
 * RETURNS:  Return TRUE if a dictionary is found, and fill in lpPIndRec
 *           with the information from the index.  If not found or an
 *           error occurs, return FALSE.
 ***************************************************************************/
ELSELOCAL BOOL ELSENEARPROC ELSEPASCAL bELSEGetIndRec (
   LPELSE_MAP_STATE lpMapState,
   LPPANOSE_NUM_MEM ELSEFARPTR *lplpPanWant,
   LPPANOSE_NUM_MEM ELSEFARPTR *lplpPanThis,
   LPPANOSE_PINDEX_MEM lpPIndRec )
{
   BOOL bFound = FALSE;
   UINT i;
   UINT uiScriptA = (*lplpPanWant)->uiScript;
   UINT uiScriptB = (*lplpPanThis)->uiScript;
   UINT uiGenreA = (*lplpPanWant)->uiGenre;
   UINT uiGenreB = (*lplpPanThis)->uiGenre;
   LPPANOSE_PDICT_MEM lpPHead;
   LPPANOSE_PINDEX_MEM ind;
   LPPANOSE_NUM_MEM lpPanSwitch;

   /* Lock down the header containing the index to all the penalty
    * table dictionaries.  If the version number of the database
    * does not match what we expect then abort.
    */
   if (!(lpPHead = lpELSEResLockHead(&lpMapState->resState)))
      goto backout0;
   if (lpPHead->uiVersion != PANOSE_PENALTY_VERS)
      goto backout1;

   /* Walk the index looking for a matching script/genre pair.  To
    * be consistent, we want to associate lpPanWant with ScriptA/GenreA
    * in the event order is critical, which would be the case if
    * ScriptA/GenreA != ScriptB/GenreB.
    */
   for (i = lpPHead->uiNumDicts, ind = lpPHead->pindex; i > 0; --i, ++ind) {
      if ((ind->uiScriptA == uiScriptA) && (ind->uiGenreA == uiGenreA)) {
         if ((ind->uiScriptB == uiScriptB) && (ind->uiGenreB == uiGenreB)) {
            bFound = TRUE;
            ELSEMEMCPY((LPEBYTE)lpPIndRec, (LPEBYTE)ind,
               sizeof(PANOSE_PINDEX_MEM));
            break;
         }
      } else if ((ind->uiScriptB == uiScriptA) && (ind->uiGenreB == uiGenreA)) {
         if ((ind->uiScriptA == uiScriptB) && (ind->uiGenreA == uiGenreB)) {
            bFound = TRUE;
            ELSEMEMCPY((LPEBYTE)lpPIndRec, (LPEBYTE)ind,
               sizeof(PANOSE_PINDEX_MEM));
            lpPanSwitch = *lplpPanWant;
            *lplpPanWant = *lplpPanThis;
            *lplpPanThis = lpPanSwitch;
            break;
         }
      }
   }
backout1:
   vELSEResUnlockHead(&lpMapState->resState, lpPHead);
backout0:
   return (bFound);
}

/***************************************************************************
 * FUNCTION: bELSEGetPenaltyC0
 *
 * PURPOSE:  Compute the penalty between two PANOSE digits using 'C0'
 *           compression, where the entire table is provided.
 *
 * RETURNS:  Return TRUE if the computed index is within range, and
 *           *lpuiMatch is filled in with the penalty value, FALSE if
 *           it is out of range.
 ***************************************************************************/
ELSELOCAL BOOL ELSENEARPROC ELSEPASCAL bELSEGetPenaltyC0 (
   LPEBYTE lpPTbl,
   LPEUINT lpuiMatch,
   UINT uiTblSize,
   UINT uiLast,
   UINT uiAttrA,
   UINT uiAttrB )
{
   UINT uiInd;

   if ((uiInd = (uiAttrA * (uiLast + 1)) + uiAttrB) >= uiTblSize)
      return (FALSE);
   *lpuiMatch = lpPTbl[uiInd];
   return (TRUE);
}

/***************************************************************************
 * FUNCTION: uiELSEGetPenaltyC1
 *
 * PURPOSE:  Compute the penalty between two PANOSE digits using 'C1'
 *           compression, which is a perfectly symmetrical table around
 *           the diagonal.  Two digits on the diagonal are an exact match.
 *           A difference of 1 yields a penalty of 1, a difference of 2
 *           yields a penalty of 2, and so on.
 *
 * RETURNS:  Return the penalty from the table, the function cannot fail.
 ***************************************************************************/
ELSELOCAL UINT ELSENEARPROC ELSEPASCAL uiELSEGetPenaltyC1 (
   LPPANOSE_PINDEX_MEM lpPIndRec,
   UINT uiAttrA,
   UINT uiAttrB )
{
   INT iDiff;

   if (uiAttrA == PANOSE_NOFIT || uiAttrB == PANOSE_NOFIT)
      return (lpPIndRec->jDefNoFitPenalty);
   else if (uiAttrA == PANOSE_ANY || uiAttrB == PANOSE_ANY)
      return (lpPIndRec->jDefAnyPenalty);
   else if (uiAttrA == uiAttrB)
      return (lpPIndRec->jDefMatchPenalty);

   if ((iDiff = (INT)uiAttrA - (INT)uiAttrB) < 0)
      iDiff = -iDiff;

   return (iDiff);
}

/***************************************************************************
 * FUNCTION: bELSEGetPenaltyC2
 *
 * PURPOSE:  Compute the penalty between two PANOSE digits using 'C2'
 *           compression, which is a table symmetrical about the
 *           diagonal, but not a smooth range from low to high, so the
 *           lower left corner of the table is provided.  The uiAttrA
 *           digit references the row and uiAttrB references the column.
 *
 * RETURNS:  Return TRUE if the computed index is within range, and
 *           *lpuiMatch is filled in with the penalty value, FALSE if
 *           it is out of range.
 ***************************************************************************/
ELSELOCAL BOOL ELSENEARPROC ELSEPASCAL bELSEGetPenaltyC2 (
   LPPANOSE_PTABLE_C2_MEM lpPC2,
   LPEUINT lpuiMatch,
   UINT uiTblSize,
   UINT uiLast,
   UINT uiAttrA,
   UINT uiAttrB )
{
   UINT uiSwap;
   INT iInd;

   /* Special case no-fit, any, or exact matches.
    */
   if (uiAttrA == PANOSE_NOFIT || uiAttrB == PANOSE_NOFIT) {
      *lpuiMatch = lpPC2->jNoFitPenalty;
      return (TRUE);
   } else if (uiAttrA == PANOSE_ANY || uiAttrB == PANOSE_ANY) {
      *lpuiMatch = lpPC2->jAnyPenalty;
      return (TRUE);
   } else if (uiAttrA == uiAttrB) {
      *lpuiMatch = lpPC2->jMatchPenalty;
      return (TRUE);
   }
   /* The formula we use assumes the lower left half of the
    * penalty table, which means row > column.  The table is
    * symmetric about the diagonal, so if row < column we can
    * just switch their values.
    */
   if (uiAttrA < uiAttrB) {
      uiSwap = uiAttrA;
      uiAttrA = uiAttrB;
      uiAttrB = uiSwap;
   }
   /* The table is missing the any, no-fit, and exact match
    * penalties as those are handled separately.  Since the
    * table is triangular shaped, we use the additive series
    * to compute the row:
    *
    *   n + ... + 3 + 2 + 1 == 1/2 * n * (n + 1)
    *
    * Substituting n for row - 3, the first possible row, and
    * adding the column offset, we get the following formula:
    *
    *   (1/2 * (row - 3) * (row - 2)) + (col - 2)
    *
    * We know that row >= 3 and col >= 2 as we catch the other
    * cases above.
    */
   if ((INT)(iInd = ELSEMULDIV(uiAttrA - 3, uiAttrA - 2, 2) + (INT)uiAttrB - 2) >=
      (INT)((INT)uiTblSize - (sizeof(PANOSE_PTABLE_C2_MEM) - sizeof(BYTE)))) {
      return (FALSE);
   }
   *lpuiMatch = lpPC2->jPenalties[iInd];
   return (TRUE);
}

/***************************************************************************
 * FUNCTION: bELSEMatchNumber
 *
 * PURPOSE:  Compute the match value between two PANOSE digits and add
 *           it to the passed in match total.
 *
 * RETURNS:  Return TRUE if the match value is computed and added to
 *           *lpuiMatchTotal.  If an error occurs, return FALSE and
 *           set *lpuiMatchTotal to the value ELSEMATCHERROR.
 ***************************************************************************/
ELSELOCAL BOOL ELSENEARPROC ELSEPASCAL bELSEMatchNumber (
   LPELSE_MAP_STATE lpMapState,
   UINT ELSEFARPTR *lpuiMatchTotal,
   LPPANOSE_PINDEX_MEM lpPIndRec,
   LPPANOSE_PRANGE_MEM lpPRngRec,
   UINT uiWeight,
   UINT uiAttrA,
   UINT uiAttrB )
{
   BOOL bMatched;
   UINT uiMatch;
   UINT uiLast = lpPRngRec->jRangeLast;
   LPEBYTE lpPTbl;

   /* First make sure the digit values are not out of range, and the
    * compression algorithm is supported.
    */
   if (uiAttrA > uiLast || uiAttrB > uiLast ||
      lpPRngRec->jCompress > PANOSE_COMPRESS__LAST) {
      goto errout;
   }
   /* Compute the penalty depending on the kind of compression
    * used for the table.
    */
   switch (lpPRngRec->jCompress) {

      case PANOSE_COMPRESS_C0:
         if (!lpPRngRec->ulOffsTbl || !lpPRngRec->uiTblSize ||
            !(lpPTbl = lpELSEResLockPTbl(&lpMapState->resState,
            lpPRngRec->ulOffsTbl, lpPRngRec->uiTblSize))) {
            goto errout;
         }
         bMatched = bELSEGetPenaltyC0(lpPTbl, &uiMatch,
            lpPRngRec->uiTblSize, uiLast, uiAttrA, uiAttrB);
         vELSEResUnlockPTbl(&lpMapState->resState, lpPTbl);
         if (!bMatched)
            goto errout;
         *lpuiMatchTotal += uiMatch * uiWeight;
         break;

      case PANOSE_COMPRESS_C1:
         *lpuiMatchTotal +=
            uiELSEGetPenaltyC1(lpPIndRec, uiAttrA, uiAttrB) * uiWeight;
         break;

      case PANOSE_COMPRESS_C2:
         if (!lpPRngRec->ulOffsTbl || !lpPRngRec->uiTblSize ||
            !(lpPTbl = lpELSEResLockPTbl(&lpMapState->resState,
            lpPRngRec->ulOffsTbl, lpPRngRec->uiTblSize))) {
            goto errout;
         }
         bMatched = bELSEGetPenaltyC2((LPPANOSE_PTABLE_C2_MEM)lpPTbl,
            &uiMatch, lpPRngRec->uiTblSize, uiLast, uiAttrA, uiAttrB);
         vELSEResUnlockPTbl(&lpMapState->resState, lpPTbl);
         if (!bMatched)
            goto errout;
         *lpuiMatchTotal += uiMatch * uiWeight;
         break;
   }
   return (TRUE);

errout:
   *lpuiMatchTotal = ELSEMATCHERROR;
   return (FALSE);
}

#ifdef ELSEUSELOCALMEMCPY
/***************************************************************************
 * FUNCTION: uiELSEMemCpy
 *
 * PURPOSE:  Copy from source to destination.
 *
 * RETURNS:  Return the count of bytes copied.
 ***************************************************************************/
ELSELOCAL UINT ELSENEARPROC ELSEPASCAL uiELSEMemCpy (
   LPEBYTE lpjDst,
   LPEBYTE lpjSrc,
   UINT uiLen )
{
   UINT i;

   for (i = uiLen; i > 0; *lpjDst++ = *lpjSrc++, --i)
      ;
   return (uiLen);
}
#endif

/***************************************************************************
 * Revision log:
 ***************************************************************************/
/*
 * $lgb$
 * 1.0     3-Feb-92  msd Version 1.00 mapper placed under version control.
 * $lge$
 */

