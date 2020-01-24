/***************************************************************************
 * ELSECVT3.C - Routines for converting Expanded PANOSE numbers from
 *              memory to disk format and vice versa.
 *
 * $keywords: elsecvt3.c 1.0  3-Feb-92 4:58:14 PM$
 *
 * Copyright (C) 1991 ElseWare Corporation.  All rights reserved.
 ***************************************************************************/

#define ELSE_MAPPER_CORE
#include "elsemap.h"

#ifndef NOELSEDISKROUTINES

/***************************************************************************
 * FUNCTION: iELSECvtExpPANDiskToMem
 *
 * PURPOSE:  Convert an Expanded PANOSE number from disk format to
 *           memory format.
 *
 *           If ELSE_CVT_TO_BYTE is specified, the routine creates a
 *           BYTE format number regardless of the format of the disk
 *           structure.  If ELSE_CVT_TO_NIBBLE is specified, the
 *           routine creates a NIBBLE format number.  If no flag or
 *           ELSE_CVT_TO_SAME is specified then the routine uses the
 *           same format as the disk structure.
 *
 *           It is acceptable for lpPanDst and lpPanSrc to point to
 *           the same memory provided ELSE_CVT_TO_SAME or no flag is
 *           specified.
 *
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function fails
 *           if there is not enough space for the 'more' digits.  If
 *           not specified, the function converts only up to the size
 *           of PANOSE_NUM_DISK which does not include the more digits
 *           (notice that since the more digits are bytes anyway the
 *           only thing the routine does with them is copy from source
 *           to destination, which may be necessary if switching from
 *           BYTE to NIBBLE or vice versa).
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required (with ELSE_CVT_TO_SAME the size of both
 *           structures is identical).
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtExpPANDiskToMem (
   LPPANOSE_NUM_MEM lpPanDst,
   UINT uiSizeDst,
   LPEBYTE lpPanSrc,
   UINT uiSizeSrc,
   UINT uiFlags )
{
   BOOL bFromNibble;
   BOOL bToNibble;
   INT iRetVal;
   UINT i;
   UINT uiNum;
   UINT uiNumAttr;
   UINT uiSizeNeedSrc;
   UINT uiSizeNeedDst;
   LPEBYTE lpjSrc;
   LPEBYTE lpjDst;
   LPPANOSE_NIB_NUM_MEM lpPanNib;

   /* Get the count of attributes, which is the first thing
    * in the Expanded PANOSE record.
    */
   if (uiSizeSrc < sizeof(UINT)) {
      iRetVal = 0;
      goto errout;
   }
   uiNumAttr = M_ELSE_PACK_WORD(lpPanSrc[PAN_DISK_IND_ATTR_MSB],
      lpPanSrc[PAN_DISK_IND_ATTR_LSB]);

   /* Compute the size we need in the source struct.
    */
   if (M_PAN_IS_NIBBLE(uiNumAttr)) {

      /* NIBBLE format.
       */
      bFromNibble = TRUE;
      uiNumAttr = M_PAN_CLEAR_NIBBLE(uiNumAttr);

      if (uiFlags & ELSE_CVT_APPEND_ARRAY)
         uiSizeNeedSrc = M_SIZEOF_PANOSE_NUM_NIB(uiNumAttr);
      else
         uiSizeNeedSrc = sizeof(PANOSE_NIB_NUM_DISK);

   } else {

      /* BYTE format.
       */
      bFromNibble = FALSE;

      if (uiFlags & ELSE_CVT_APPEND_ARRAY)
         uiSizeNeedSrc = M_SIZEOF_PANOSE_NUM_BYTE(uiNumAttr);
      else
         uiSizeNeedSrc = sizeof(PANOSE_NUM_DISK);
   }
   /* Compute the size we need in the destination struct.
    */
   if ((uiFlags & ELSE_CVT_TO_NIBBLE) ||
      (!(uiFlags & ELSE_CVT_TO_BYTE) && bFromNibble)) {

      /* NIBBLE format.
       */
      bToNibble = TRUE;

      if (uiFlags & ELSE_CVT_APPEND_ARRAY)
         uiSizeNeedDst = M_SIZEOF_PANOSE_NUM_NIB(uiNumAttr);
      else
         uiSizeNeedDst = M_SIZEOF_MIN_PANOSE_NIB();

   } else {

      /* BYTE format.
       */
      bToNibble = FALSE;

      if (uiFlags & ELSE_CVT_APPEND_ARRAY)
         uiSizeNeedDst = M_SIZEOF_PANOSE_NUM_BYTE(uiNumAttr);
      else
         uiSizeNeedDst = M_SIZEOF_MIN_PANOSE_BYTE();
   }
   /* If the source number does not have at least the four
    * shared digits in it then abort now.
    */
   if (uiNumAttr < PAN_COUNT_SHARED) {
      iRetVal = 0;
      goto errout;
   }
   /* If one of the sizes is not big enough then return failure.
    * Since only one number may be returned, return the largest
    * one.
    */
   if ((uiSizeNeedSrc > uiSizeSrc) || (uiSizeNeedDst > uiSizeDst)) {
      iRetVal = (uiSizeNeedSrc > uiSizeNeedDst) ?
         -(int)uiSizeNeedSrc : -(int)uiSizeNeedDst;
      goto errout;
   }
   /* Convert the part of the struct common to both BYTE
    * and NIBBLE formats.
    */
   lpPanDst->uiNumAttr = bToNibble ?
      M_PAN_SET_NIBBLE(uiNumAttr) : uiNumAttr;
   lpPanDst->uiScript = M_ELSE_PACK_WORD(lpPanSrc[PAN_DISK_IND_SCRIPT_MSB],
      lpPanSrc[PAN_DISK_IND_SCRIPT_LSB]);
   lpPanDst->uiGenre = M_ELSE_PACK_WORD(lpPanSrc[PAN_DISK_IND_GENRE_MSB],
      lpPanSrc[PAN_DISK_IND_GENRE_LSB]);
   lpPanDst->uiUniqueID = M_ELSE_PACK_WORD(lpPanSrc[PAN_DISK_IND_UNIQUE_MSB],
      lpPanSrc[PAN_DISK_IND_UNIQUE_LSB]);

   if (bToNibble) {

      /* Converting to NIBBLE format.
       */
      lpPanNib = (LPPANOSE_NIB_NUM_MEM)lpPanDst;

      if (bFromNibble) {

         /* Converting from NIBBLE format.
          */
         lpPanNib->jWeightDistort =
            lpPanSrc[PAN_NIB_DISK_IND_WEIGHT_DISTORT];
         lpPanNib->jMonospcContrast =
            lpPanSrc[PAN_NIB_DISK_IND_MONOSPC_CNTRST];

         /* Transfer 'more' digits if the caller wants us to
          * do so.
          */
         if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
            for (i = 0, uiNum = (uiNumAttr - PAN_COUNT_SHARED + 1) / 2,
               lpjDst = lpPanNib->ajMoreNibAttr,
               lpjSrc = &lpPanSrc[PAN_NIB_DISK_COUNT]; i < uiNum;
               ++i, *lpjDst++ = *lpjSrc++)
               ;
         }
      } else {

         /* Converting from BYTE format.
          */
         lpPanNib->jWeightDistort =
            M_ELSE_PACK_BYTE(lpPanSrc[PAN_DISK_IND_WEIGHT],
               lpPanSrc[PAN_DISK_IND_DISTORTION]);
         lpPanNib->jMonospcContrast =
            M_ELSE_PACK_BYTE(lpPanSrc[PAN_DISK_IND_MONOSPACE],
               lpPanSrc[PAN_DISK_IND_CONTRAST]);

         /* Transfer 'more' digits if the caller wants us to
          * do so.
          */
         if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
            for (i = 0, uiNum = uiNumAttr - PAN_COUNT_SHARED,
               lpjDst = lpPanNib->ajMoreNibAttr,
               lpjSrc = &lpPanSrc[PAN_DISK_COUNT]; i < uiNum;
               ++lpjDst) {

               if (i < (uiNum - 1)) {
                  *lpjDst = M_ELSE_PACK_BYTE(lpjSrc[0], lpjSrc[1]);
                  i += 2, lpjSrc += 2;
               } else {
                  *lpjDst = M_ELSE_PACK_BYTE(lpjSrc[0], 0);
                  ++i, ++lpjSrc;
               }
            }
         }
      }

   } else {

      /* Converting to BYTE format.
       */
      if (bFromNibble) {

         /* Converting from NIBBLE format.
          */
         lpPanDst->jWeight =
            M_ELSE_GET_MSN(lpPanSrc[PAN_NIB_DISK_IND_WEIGHT_DISTORT]);
         lpPanDst->jDistortion =
            M_ELSE_GET_LSN(lpPanSrc[PAN_NIB_DISK_IND_WEIGHT_DISTORT]);
         lpPanDst->jMonospace =
            M_ELSE_GET_MSN(lpPanSrc[PAN_NIB_DISK_IND_MONOSPC_CNTRST]);
         lpPanDst->jContrast =
            M_ELSE_GET_LSN(lpPanSrc[PAN_NIB_DISK_IND_MONOSPC_CNTRST]);

         /* Transfer 'more' digits if the caller wants us to
          * do so.
          */
         if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
            for (i = 0, uiNum = uiNumAttr - PAN_COUNT_SHARED,
               lpjDst = lpPanDst->ajMoreAttr,
               lpjSrc = &lpPanSrc[PAN_NIB_DISK_COUNT]; i < uiNum;
               ++i, ++lpjDst) {

               if ((i % 2) == 0) {
                  *lpjDst = M_ELSE_GET_MSN(*lpjSrc);
               } else {
                  *lpjDst = M_ELSE_GET_LSN(*lpjSrc);
                  ++lpjSrc;
               }
            }
         }
      } else {

         /* Converting from BYTE format.
          */
         lpPanDst->jWeight = lpPanSrc[PAN_DISK_IND_WEIGHT];
         lpPanDst->jDistortion = lpPanSrc[PAN_DISK_IND_DISTORTION];
         lpPanDst->jMonospace = lpPanSrc[PAN_DISK_IND_MONOSPACE];
         lpPanDst->jContrast = lpPanSrc[PAN_DISK_IND_CONTRAST];

         /* Transfer 'more' digits if the caller wants us to
          * do so.
          */
         if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
            for (i = 0, uiNum = uiNumAttr - PAN_COUNT_SHARED,
               lpjDst = lpPanDst->ajMoreAttr,
               lpjSrc = &lpPanSrc[PAN_DISK_COUNT]; i < uiNum;
               ++i, *lpjDst++ = *lpjSrc++)
               ;
         }
      }
   }
   /* Return the size of the struct we filled in.
    */
   return (uiSizeNeedDst);

   /* Error return.  As we do in all the other routines that
    * can return an Expanded PANOSE number, if there is room
    * to put something sensible in the first few fields of
    * the destination structure, we do so.
    */
errout:
   vELSEMakeExpPANDummy(lpPanDst, uiSizeDst);
   return (iRetVal);
}

/***************************************************************************
 * FUNCTION: iELSECvtExpPANMemToDisk
 *
 * PURPOSE:  Convert an Expanded PANOSE number from memory format to
 *           disk format.
 *
 *           It is acceptable for lpPanDst and lpPanSrc to point to
 *           the same memory provided ELSE_CVT_TO_SAME or no flag is
 *           specified.
 *
 *           See the description of iELSECvtExpPANDiskToMem for details
 *           on the passed in flags.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtExpPANMemToDisk (
   LPEBYTE lpPanDst,
   UINT uiSizeDst,
   LPPANOSE_NUM_MEM lpPanSrc,
   UINT uiSizeSrc,
   UINT uiFlags )
{
   BOOL bFromNibble;
   BOOL bToNibble;
   INT iRetVal;
   UINT i;
   UINT uiVal;
   UINT uiNum;
   UINT uiNumAttr;
   UINT uiSizeNeedSrc;
   UINT uiSizeNeedDst;
   LPEBYTE lpjSrc;
   LPEBYTE lpjDst;
   LPPANOSE_NIB_NUM_MEM lpPanNib;

   /* Compute the size we need in the source struct.
    */
   if (M_PAN_IS_NIBBLE(lpPanSrc->uiNumAttr)) {

      /* NIBBLE format.
       */
      bFromNibble = TRUE;
      uiNumAttr = M_PAN_CLEAR_NIBBLE(lpPanSrc->uiNumAttr);

      if (uiFlags & ELSE_CVT_APPEND_ARRAY)
         uiSizeNeedSrc = M_SIZEOF_PANOSE_NUM_NIB(uiNumAttr);
      else
         uiSizeNeedSrc = M_SIZEOF_MIN_PANOSE_NIB();

   } else {

      /* BYTE format.
       */
      bFromNibble = FALSE;
      uiNumAttr = lpPanSrc->uiNumAttr;

      if (uiFlags & ELSE_CVT_APPEND_ARRAY)
         uiSizeNeedSrc = M_SIZEOF_PANOSE_NUM_BYTE(uiNumAttr);
      else
         uiSizeNeedSrc = M_SIZEOF_MIN_PANOSE_BYTE();
   }
   /* Compute the size we need in the destination struct.
    */
   if ((uiFlags & ELSE_CVT_TO_NIBBLE) ||
      (!(uiFlags & ELSE_CVT_TO_BYTE) && bFromNibble)) {

      /* NIBBLE format.
       */
      bToNibble = TRUE;

      if (uiFlags & ELSE_CVT_APPEND_ARRAY)
         uiSizeNeedDst = M_SIZEOF_PANOSE_NUM_NIB(uiNumAttr);
      else
         uiSizeNeedDst = sizeof(PANOSE_NIB_NUM_DISK);

   } else {

      /* BYTE format.
       */
      bToNibble = FALSE;

      if (uiFlags & ELSE_CVT_APPEND_ARRAY)
         uiSizeNeedDst = M_SIZEOF_PANOSE_NUM_BYTE(uiNumAttr);
      else
         uiSizeNeedDst = sizeof(PANOSE_NUM_DISK);
   }
   /* If the source number does not have at least the four
    * shared digits in it then abort now.
    */
   if (uiNumAttr < PAN_COUNT_SHARED) {
      iRetVal = 0;
      goto errout;
   }
   /* If one of the sizes is not big enough then return failure.
    * Since only one number may be returned, return the largest
    * one.
    */
   if ((uiSizeNeedSrc > uiSizeSrc) || (uiSizeNeedDst > uiSizeDst)) {
      iRetVal = (uiSizeNeedSrc > uiSizeNeedDst) ?
         -(int)uiSizeNeedSrc : -(int)uiSizeNeedDst;
      goto errout;
   }
   /* Convert the part of the struct common to both BYTE
    * and NIBBLE formats.
    */
   uiVal = bToNibble ? M_PAN_SET_NIBBLE(uiNumAttr) : uiNumAttr;
   lpPanDst[PAN_DISK_IND_ATTR_MSB] = M_ELSE_GET_MSB(uiVal);
   lpPanDst[PAN_DISK_IND_ATTR_LSB] = M_ELSE_GET_LSB(uiVal);
   uiVal = lpPanSrc->uiScript;
   lpPanDst[PAN_DISK_IND_SCRIPT_MSB] = M_ELSE_GET_MSB(uiVal);
   lpPanDst[PAN_DISK_IND_SCRIPT_LSB] = M_ELSE_GET_LSB(uiVal);
   uiVal = lpPanSrc->uiGenre;
   lpPanDst[PAN_DISK_IND_GENRE_MSB] = M_ELSE_GET_MSB(uiVal);
   lpPanDst[PAN_DISK_IND_GENRE_LSB] = M_ELSE_GET_LSB(uiVal);
   uiVal = lpPanSrc->uiUniqueID;
   lpPanDst[PAN_DISK_IND_UNIQUE_MSB] = M_ELSE_GET_MSB(uiVal);
   lpPanDst[PAN_DISK_IND_UNIQUE_LSB] = M_ELSE_GET_LSB(uiVal);

   if (bToNibble) {

      /* Converting to NIBBLE format.
       */
      if (bFromNibble) {

         /* Converting from NIBBLE format.
          */
         lpPanNib = (LPPANOSE_NIB_NUM_MEM)lpPanSrc;

         lpPanDst[PAN_NIB_DISK_IND_WEIGHT_DISTORT] =
            lpPanNib->jWeightDistort;
         lpPanDst[PAN_NIB_DISK_IND_MONOSPC_CNTRST] =
            lpPanNib->jMonospcContrast;

         /* Transfer 'more' digits if the caller wants us to
          * do so.
          */
         if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
            for (i = 0, uiNum = (uiNumAttr - PAN_COUNT_SHARED + 1) / 2,
               lpjDst = &lpPanDst[PAN_NIB_DISK_COUNT],
               lpjSrc = lpPanNib->ajMoreNibAttr; i < uiNum;
               ++i, *lpjDst++ = *lpjSrc++)
               ;
         }
      } else {

         /* Converting from BYTE format.
          */
         lpPanDst[PAN_NIB_DISK_IND_WEIGHT_DISTORT] =
            M_ELSE_PACK_BYTE(lpPanSrc->jWeight, lpPanSrc->jDistortion);
         lpPanDst[PAN_NIB_DISK_IND_MONOSPC_CNTRST] =
            M_ELSE_PACK_BYTE(lpPanSrc->jMonospace, lpPanSrc->jContrast);

         /* Transfer 'more' digits if the caller wants us to
          * do so.
          */
         if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
            for (i = 0, uiNum = uiNumAttr - PAN_COUNT_SHARED,
               lpjDst = &lpPanDst[PAN_NIB_DISK_COUNT],
               lpjSrc = lpPanSrc->ajMoreAttr; i < uiNum;
               ++lpjDst) {

               if (i < (uiNum - 1)) {
                  *lpjDst = M_ELSE_PACK_BYTE(lpjSrc[0], lpjSrc[1]);
                  i += 2, lpjSrc += 2;
               } else {
                  *lpjDst = M_ELSE_PACK_BYTE(lpjSrc[0], 0);
                  ++i, ++lpjSrc;
               }
            }
         }
      }

   } else {

      /* Converting to BYTE format.
       */
      if (bFromNibble) {

         /* Converting from NIBBLE format.
          */
         lpPanNib = (LPPANOSE_NIB_NUM_MEM)lpPanSrc;

         lpPanDst[PAN_DISK_IND_WEIGHT] =
            M_ELSE_GET_MSN(lpPanNib->jWeightDistort);
         lpPanDst[PAN_DISK_IND_DISTORTION] =
            M_ELSE_GET_LSN(lpPanNib->jWeightDistort);
         lpPanDst[PAN_DISK_IND_MONOSPACE] =
            M_ELSE_GET_MSN(lpPanNib->jMonospcContrast);
         lpPanDst[PAN_DISK_IND_CONTRAST] =
            M_ELSE_GET_LSN(lpPanNib->jMonospcContrast);

         /* Transfer 'more' digits if the caller wants us to
          * do so.
          */
         if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
            for (i = 0, uiNum = uiNumAttr - PAN_COUNT_SHARED,
               lpjDst = &lpPanDst[PAN_DISK_COUNT],
               lpjSrc = lpPanNib->ajMoreNibAttr; i < uiNum;
               ++i, ++lpjDst) {

               if ((i % 2) == 0) {
                  *lpjDst = M_ELSE_GET_MSN(*lpjSrc);
               } else {
                  *lpjDst = M_ELSE_GET_LSN(*lpjSrc);
                  ++lpjSrc;
               }
            }
         }
      } else {

         /* Converting from BYTE format.
          */
         lpPanDst[PAN_DISK_IND_WEIGHT] = lpPanSrc->jWeight;
         lpPanDst[PAN_DISK_IND_DISTORTION] = lpPanSrc->jDistortion;
         lpPanDst[PAN_DISK_IND_MONOSPACE] = lpPanSrc->jMonospace;
         lpPanDst[PAN_DISK_IND_CONTRAST] = lpPanSrc->jContrast;

         /* Transfer 'more' digits if the caller wants us to
          * do so.
          */
         if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
            for (i = 0, uiNum = uiNumAttr - PAN_COUNT_SHARED,
               lpjDst = &lpPanDst[PAN_DISK_COUNT],
               lpjSrc = lpPanSrc->ajMoreAttr; i < uiNum;
               ++i, *lpjDst++ = *lpjSrc++)
               ;
         }
      }
   }
   /* Return the size of the struct we filled in.
    */
   return (uiSizeNeedDst);

   /* Error return.  As we do in all the other routines that
    * can return an Expanded PANOSE number, if there is room
    * to put something sensible in the first few fields of
    * the destination structure, we do so.
    */
errout:
   if (uiSizeDst >= sizeof(UINT)) {
      lpPanDst[PAN_DISK_IND_ATTR_MSB] = M_ELSE_GET_MSB(0);
      lpPanDst[PAN_DISK_IND_ATTR_LSB] = M_ELSE_GET_LSB(0);
   }
   if (uiSizeDst >= (sizeof(UINT) * 2)) {
      lpPanDst[PAN_DISK_IND_SCRIPT_MSB] = M_ELSE_GET_MSB(PANOSE_NOFIT);
      lpPanDst[PAN_DISK_IND_SCRIPT_LSB] = M_ELSE_GET_LSB(PANOSE_NOFIT);
   }
   if (uiSizeDst >= (sizeof(UINT) * 3)) {
      lpPanDst[PAN_DISK_IND_GENRE_MSB] = M_ELSE_GET_MSB(PANOSE_NOFIT);
      lpPanDst[PAN_DISK_IND_GENRE_LSB] = M_ELSE_GET_LSB(PANOSE_NOFIT);
   }
   if (uiSizeDst >= (sizeof(UINT) * 4)) {
      lpPanDst[PAN_DISK_IND_UNIQUE_MSB] = M_ELSE_GET_MSB(PANOSE_ANY);
      lpPanDst[PAN_DISK_IND_UNIQUE_LSB] = M_ELSE_GET_LSB(PANOSE_ANY);
   }
   return (iRetVal);
}

#endif  /* ifndef NOELSEDISKROUTINES */

/***************************************************************************
 * Revision log:
 ***************************************************************************/
/*
 * $lgb$
 * 1.0     3-Feb-92  msd Version 1.00 mapper placed under version control.
 * $lge$
 */

