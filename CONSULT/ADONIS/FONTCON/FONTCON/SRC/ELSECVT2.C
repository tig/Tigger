/***************************************************************************
 * ELSECVT2.C - Routines for converting between nibbles and bytes.
 *
 * $keywords: elsecvt2.c 1.0  3-Feb-92 4:58:12 PM$
 *
 * Copyright (C) 1991 ElseWare Corporation.  All rights reserved.
 ***************************************************************************/

#define ELSE_MAPPER_CORE
#include "elsemap.h"

/***************************************************************************
 * FUNCTION: iELSECvtExpNibbleToByte
 *
 * PURPOSE:  Convert and Expanded PANOSE number in NIBBLE format to BYTE
 *           format.
 *
 * RETURNS:  The function returns zero if no digits were copied, a
 *           positive count if all the digits in the source were copied
 *           to the destination, or a negative count if fewer than the
 *           total available were copied (the inverse of the negative
 *           is the count copied).
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtExpNibbleToByte (
   LPPANOSE_NUM_MEM lpPanByte,
   UINT uiSizePanByte,
   LPPANOSE_NIB_NUM_MEM lpPanNib )
{
   BOOL bTooSmall;
   UINT i;
   UINT uiNum;
   UINT uiNumMore;
   LPEBYTE lpjMoreByte;
   LPEBYTE lpjMoreNib;

   /* Sanity check, the source record should be in NIBBLE format
    * and should contain atleast the shared 4 digits.  The destination
    * record should be large enough to hold atleast the shared four
    * digits.
    *
    * The function will not construct a valid number if lpPanByte and
    * lpPanNib point to the same memory, so catch that here too.
    */
   if (!M_PAN_IS_NIBBLE(lpPanNib->uiNumAttr) ||
      (M_PAN_CLEAR_NIBBLE(lpPanNib->uiNumAttr) < PAN_COUNT_SHARED) ||
      (uiSizePanByte < M_SIZEOF_MIN_PANOSE_BYTE()) ||
      ((LPEBYTE)lpPanByte == (LPEBYTE)lpPanNib)) {
      vELSEMakeExpPANDummy(lpPanByte, uiSizePanByte);
      return (0);
   }
   /* Compute the count of digits available on the destination
    * and trim it back to the count of digits on the source.
    */
   uiNum = M_COUNT_MORE_PANOSE_BYTE(uiSizePanByte) + PAN_COUNT_SHARED;
   if (uiNum >= (i = M_PAN_CLEAR_NIBBLE(lpPanNib->uiNumAttr))) {
      uiNum = i;
      bTooSmall = FALSE;
   } else {
      bTooSmall = TRUE;
   }
   /* Transfer the structure.
    */
   lpPanByte->uiNumAttr = uiNum;
   lpPanByte->uiScript = lpPanNib->uiScript;
   lpPanByte->uiGenre = lpPanNib->uiGenre;
   lpPanByte->uiUniqueID = lpPanNib->uiUniqueID;
   lpPanByte->jWeight = M_ELSE_GET_MSN(lpPanNib->jWeightDistort);
   lpPanByte->jDistortion = M_ELSE_GET_LSN(lpPanNib->jWeightDistort);
   lpPanByte->jMonospace = M_ELSE_GET_MSN(lpPanNib->jMonospcContrast);
   lpPanByte->jContrast = M_ELSE_GET_LSN(lpPanNib->jMonospcContrast);

   /* Copy over the 'more' attributes digits.
    */
   for (i = 0, uiNumMore = uiNum - PAN_COUNT_SHARED,
      lpjMoreByte = lpPanByte->ajMoreAttr,
      lpjMoreNib = lpPanNib->ajMoreNibAttr; i < uiNumMore;
      ++i, ++lpjMoreByte) {

      if ((i % 2) == 0) {
         *lpjMoreByte = M_ELSE_GET_MSN(*lpjMoreNib);
      } else {
         *lpjMoreByte = M_ELSE_GET_LSN(*lpjMoreNib);
         ++lpjMoreNib;
      }
   }
   /* Return neg count if it is less than what's available.
    */
   return (bTooSmall ? -(int)uiNum : uiNum);
}

/***************************************************************************
 * FUNCTION: iELSECvtExpByteToNibble
 *
 * PURPOSE:  Convert and Expanded PANOSE number in BYTE format to NIBBLE
 *           format.
 *
 * RETURNS:  The function returns zero if no digits were copied, a
 *           positive count if all the digits in the source were copied
 *           to the destination, or a negative count if fewer than the
 *           total available were copied (the inverse of the negative
 *           is the count copied).
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtExpByteToNibble (
   LPPANOSE_NIB_NUM_MEM lpPanNib,
   UINT uiSizePanNib,
   LPPANOSE_NUM_MEM lpPanByte )
{
   BOOL bTooSmall;
   UINT i;
   UINT uiNum;
   UINT uiNumMore;
   LPEBYTE lpjMoreByte;
   LPEBYTE lpjMoreNib;

   /* Make sure the source record is in BYTE format and contains
    * at least the four shared digits.  The destination record
    * should also be at least large enough to hold the four shared
    * digits.
    */
   if (M_PAN_IS_NIBBLE(lpPanByte->uiNumAttr) ||
      (lpPanByte->uiNumAttr < PAN_COUNT_SHARED) ||
      (uiSizePanNib < M_SIZEOF_MIN_PANOSE_NIB())) {
      vELSEMakeExpPANDummy((LPPANOSE_NUM_MEM)lpPanNib, uiSizePanNib);
      return (0);
   }
   /* Compute the count of digits available on the destination
    * and trim it back to the count of digits on the source.
    */
   uiNum = M_COUNT_MORE_PANOSE_NIB(uiSizePanNib) + PAN_COUNT_SHARED;
   if (uiNum >= lpPanByte->uiNumAttr) {
      uiNum = lpPanByte->uiNumAttr;
      bTooSmall = FALSE;
   } else {
      bTooSmall = TRUE;
   }
   /* Transfer the structure.
    */
   lpPanNib->uiNumAttr = M_PAN_SET_NIBBLE(uiNum);
   lpPanNib->uiScript = lpPanByte->uiScript;
   lpPanNib->uiGenre = lpPanByte->uiGenre;
   lpPanNib->uiUniqueID = lpPanByte->uiUniqueID;
   lpPanNib->jWeightDistort =
      M_ELSE_PACK_BYTE(lpPanByte->jWeight, lpPanByte->jDistortion);
   lpPanNib->jMonospcContrast =
      M_ELSE_PACK_BYTE(lpPanByte->jMonospace, lpPanByte->jContrast);

   /* Copy over the 'more' attributes digits.
    */
   for (i = 0, uiNumMore = uiNum - PAN_COUNT_SHARED,
      lpjMoreByte = lpPanByte->ajMoreAttr,
      lpjMoreNib = lpPanNib->ajMoreNibAttr; i < uiNumMore;
      ++lpjMoreNib) {

      if (i < (uiNumMore - 1)) {
         *lpjMoreNib = M_ELSE_PACK_BYTE(lpjMoreByte[0], lpjMoreByte[1]);
         i += 2, lpjMoreByte += 2;
      } else {
         *lpjMoreNib = M_ELSE_PACK_BYTE(lpjMoreByte[0], 0);
         ++i, ++lpjMoreByte;
      }
   }
   /* Return neg count if it is less than what's available.
    */
   return (bTooSmall ? -(int)uiNum : uiNum);
}

#ifndef NOELSEGETSIZEINFO
/***************************************************************************
 * FUNCTION: uiELSECvtExpGetSizeInfo
 *
 * PURPOSE:  Examine the Expanded PANOSE number for size information.
 *
 * RETURNS:  The function returns the number of digits in the number,
 *           including the four shared digits (the caller should subtract
 *           PAN_COUNT_SHARED to get the count of 'more' digits).  The
 *           return value is zero if the number is invalid or does not
 *           contain at least the standard four shared digits.  The
 *           variable *lpbIsByteFormat is TRUE if the number is in BYTE
 *           format, FALSE if it is in NIBBLE format.  The variable
 *           *lpuiSizeBytes contains the size of the structure in BYTE
 *           format and *lpuiSizeNibs contains the size in NIBBLE format.
 ***************************************************************************/
UINT ELSEFARPROC ELSEPASCAL uiELSECvtExpGetSizeInfo (
   LPPANOSE_NUM_MEM lpPanThis,
   LPEBOOL lpbIsByteFormat,
   LPEUINT lpuiSizeBytes,
   LPEUINT lpuiSizeNibs )
{
   UINT uiNum;

   /* Determine NIBBLE or BYTE.
    */
   if (M_PAN_IS_NIBBLE(lpPanThis->uiNumAttr)) {
      *lpbIsByteFormat = FALSE;
      uiNum = M_PAN_CLEAR_NIBBLE(lpPanThis->uiNumAttr);
   } else {
      *lpbIsByteFormat = TRUE;
      uiNum = lpPanThis->uiNumAttr;
   }
   /* Fail if the number does not contain at least the four
    * shared digits.
    */
   if (uiNum < PAN_COUNT_SHARED) {
      *lpuiSizeBytes = 0;
      *lpuiSizeNibs = 0;
      return (0);
   }
   /* Compute the different sizes and return the count of digits.
    */
   *lpuiSizeBytes = M_SIZEOF_PANOSE_NUM_BYTE(uiNum);
   *lpuiSizeNibs = M_SIZEOF_PANOSE_NUM_NIB(uiNum);
   return (uiNum);
}
#endif  /* ifndef NOELSEGETSIZEINFO */

/***************************************************************************
 * Revision log:
 ***************************************************************************/
/*
 * $lgb$
 * 1.0     3-Feb-92  msd Version 1.00 mapper placed under version control.
 * $lge$
 */

