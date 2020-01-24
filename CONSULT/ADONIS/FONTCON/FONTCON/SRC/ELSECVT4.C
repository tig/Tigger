/***************************************************************************
 * ELSECVT4.C - Routines for converting the penalty dictionary from
 *              memory to disk format and vice versa.
 *
 * $keywords: elsecvt4.c 1.0  3-Feb-92 4:58:15 PM$
 *
 * Copyright (C) 1991 ElseWare Corporation.  All rights reserved.
 ***************************************************************************/

#define ELSE_MAPPER_CORE
#include "elsemap.h"

#ifndef NOELSEDISKROUTINES

/***************************************************************************
 * FUNCTION: iELSECvtPHeadDiskToMem
 *
 * PURPOSE:  Convert the penalty dictionary header from disk format to
 *           memory format.
 *
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function also
 *           converts the index array immediately following the header.
 *
 *           It is acceptable for lpPHeadDst and lpPHeadSrc to point
 *           to the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtPHeadDiskToMem (
   LPPANOSE_PDICT_MEM lpPHeadDst,
   LPEBYTE lpPHeadSrc,
   UINT uiSize,
   UINT uiFlags )
{
   INT iRetVal;
   UINT uiVersion;

   /* Get the version number from the header structure.
    */
   if (uiSize < sizeof(UINT)) {
      return (0);
   }
   uiVersion = M_ELSE_PACK_WORD(lpPHeadSrc[PAN_DISK_DICT_VERSION_MSB],
      lpPHeadSrc[PAN_DISK_DICT_VERSION_LSB]);

   /* Fail if the version number is not what we expect.
    */
   if (uiVersion != PANOSE_PENALTY_VERS) {
      return (0);
   }
   /* If the structure is not large enough to hold the
    * main part of the header then abort now.
    */
   if (uiSize < sizeof(PANOSE_PDICT_DISK)) {
      return (-(int)sizeof(PANOSE_PDICT_DISK));
   }
   /* Convert the main body of the header.
    */
   lpPHeadDst->uiVersion = uiVersion;
   lpPHeadDst->uiNumDicts =
      M_ELSE_PACK_WORD(lpPHeadSrc[PAN_DISK_DICT_NUMDICTS_MSB],
      lpPHeadSrc[PAN_DISK_DICT_NUMDICTS_LSB]);
   lpPHeadDst->uiSizeIndex =
      M_ELSE_PACK_WORD(lpPHeadSrc[PAN_DISK_DICT_SIZE_IND_MSB],
      lpPHeadSrc[PAN_DISK_DICT_SIZE_IND_LSB]);
   lpPHeadDst->uiSizeFullHd =
      M_ELSE_PACK_WORD(lpPHeadSrc[PAN_DISK_DICT_SIZE_HEAD_MSB],
      lpPHeadSrc[PAN_DISK_DICT_SIZE_HEAD_LSB]);
   lpPHeadDst->ulSizeFullDB =
      M_ELSE_PACK_DWORD(lpPHeadSrc[PAN_DISK_DICT_SIZE_DB_MSW_MSB],
      lpPHeadSrc[PAN_DISK_DICT_SIZE_DB_MSW_LSB],
      lpPHeadSrc[PAN_DISK_DICT_SIZE_DB_LSW_MSB],
      lpPHeadSrc[PAN_DISK_DICT_SIZE_DB_LSW_LSB]);

   /* Stop here if we are not supposed to convert the index
    * array immediately following the header.
    */
   if (!(uiFlags & ELSE_CVT_APPEND_ARRAY)) {
      return (sizeof(PANOSE_PDICT_DISK));
   }
   /* If the size of the structure is not large enough
    * then return an error.  Notice we are expecting
    * the data structure to contain a valid value.
    */
   if (lpPHeadDst->uiSizeIndex > uiSize) {
      return (-(int)lpPHeadDst->uiSizeIndex);
   }
   /* Walk the index array.
    */
   iRetVal = iELSECvtPIndDiskToMem(lpPHeadDst->pindex,
      &lpPHeadSrc[PAN_DISK_DICT_COUNT],
      uiSize - sizeof(PANOSE_PDICT_DISK),
      uiFlags, lpPHeadDst->uiNumDicts);

   /* Return error or success depending upon how the index
    * convert routine worked.
    */
   if (iRetVal == 0) {
      return (0);
   } else if (iRetVal < 0) {
      return (iRetVal - sizeof(PANOSE_PDICT_DISK));
   } else {
      return (iRetVal + sizeof(PANOSE_PDICT_DISK));
   }
}

/***************************************************************************
 * FUNCTION: iELSECvtPIndDiskToMem
 *
 * PURPOSE:  Convert the penalty dictionary index (the variable length
 *           array of index records immediately following the header)
 *           from disk format to memory format.
 *
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function
 *           converts an array of index records.  It is assumed
 *           lpPIndDst and lpPIndSrc point to the first element in
 *           the array and uiNumRecs contains the count of records.
 *
 *           It is acceptable for lpPIndDst and lpPIndSrc to point
 *           to the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtPIndDiskToMem (
   LPPANOSE_PINDEX_MEM lpPIndDst,
   LPEBYTE lpPIndSrc,
   UINT uiSize,
   UINT uiFlags,
   UINT uiNumRecs )
{
   UINT i;
   UINT uiSizeNeed;

   /* Compute the size we need.
    */
   if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
      uiSizeNeed = uiNumRecs * sizeof(PANOSE_PINDEX_DISK);
   } else {
      uiSizeNeed = sizeof(PANOSE_PINDEX_DISK);
      uiNumRecs = 1;
   }
   /* If the structure is not large enough then
    * return an error.
    */
   if (uiSizeNeed > uiSize) {
      return (-(int)uiSizeNeed);
   }
   /* Walk the array converting it.  Notice if we are only
    * converting one record we fixed uiNumRecs to 1 above.
    */
   for (i = 0; i < uiNumRecs;
      ++i, ++lpPIndDst, lpPIndSrc += sizeof(PANOSE_PINDEX_DISK)) {

      lpPIndDst->uiNumAttr =
         M_ELSE_PACK_WORD(lpPIndSrc[PAN_DISK_PIND_NUMATTR_MSB],
         lpPIndSrc[PAN_DISK_PIND_NUMATTR_LSB]);
      lpPIndDst->uiScriptA =
         M_ELSE_PACK_WORD(lpPIndSrc[PAN_DISK_PIND_SCRIPTA_MSB],
         lpPIndSrc[PAN_DISK_PIND_SCRIPTA_LSB]);
      lpPIndDst->uiScriptB =
         M_ELSE_PACK_WORD(lpPIndSrc[PAN_DISK_PIND_SCRIPTB_MSB],
         lpPIndSrc[PAN_DISK_PIND_SCRIPTB_LSB]);
      lpPIndDst->uiGenreA =
         M_ELSE_PACK_WORD(lpPIndSrc[PAN_DISK_PIND_GENREA_MSB],
         lpPIndSrc[PAN_DISK_PIND_GENREA_LSB]);
      lpPIndDst->uiGenreB =
         M_ELSE_PACK_WORD(lpPIndSrc[PAN_DISK_PIND_GENREB_MSB],
         lpPIndSrc[PAN_DISK_PIND_GENREB_LSB]);
      lpPIndDst->jDefAnyPenalty = lpPIndSrc[PAN_DISK_PIND_DEF_ANY];
      lpPIndDst->jDefNoFitPenalty = lpPIndSrc[PAN_DISK_PIND_DEF_NOFIT];
      lpPIndDst->jDefMatchPenalty = lpPIndSrc[PAN_DISK_PIND_DEF_MATCH];
      lpPIndDst->jReserved = lpPIndSrc[PAN_DISK_PIND_RESERVED_0];
      lpPIndDst->uiOffsRngArray =
         M_ELSE_PACK_WORD(lpPIndSrc[PAN_DISK_PIND_OFFSRNG_MSB],
         lpPIndSrc[PAN_DISK_PIND_OFFSRNG_LSB]);
      lpPIndDst->uiOffsWeights =
         M_ELSE_PACK_WORD(lpPIndSrc[PAN_DISK_PIND_OFFSWT_MSB],
         lpPIndSrc[PAN_DISK_PIND_OFFSWT_LSB]);
      lpPIndDst->uiOffsAttrAtoB =
         M_ELSE_PACK_WORD(lpPIndSrc[PAN_DISK_PIND_OFFSATOB_MSB],
         lpPIndSrc[PAN_DISK_PIND_OFFSATOB_LSB]);
   }
   /* Return success.
    */
   return (uiSizeNeed);
}

/***************************************************************************
 * FUNCTION: iELSECvtPA2BDiskToMem
 *
 * PURPOSE:  Convert the penalty A-to-B remapping table from disk
 *           format to memory format.
 * 
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function
 *           converts the variable length array of A-to-B remap
 *           records.  Without it, the function just converts the
 *           header which contains the count of remap records.
 *
 *           No 'count' variable is passed in because the function
 *           obtains that from the A-to-B header.
 *
 *           It is acceptable for lpPA2BDst and lpPA2BSrc to point
 *           to the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtPA2BDiskToMem (
   LPPANOSE_ATOB_MEM lpPA2BDst,
   LPEBYTE lpPA2BSrc,
   UINT uiSize,
   UINT uiFlags )
{
   UINT i;
   UINT uiSizeNeed;
   LPPANOSE_ATOB_ITEM_MEM lpPA2BItem;

   /* First attempt to convert the header.
    */
   if (uiSize < sizeof(PANOSE_ATOB_DISK)) {
      return (-(int)sizeof(PANOSE_ATOB_DISK));
   }
   lpPA2BDst->uiNumAtoB =
      M_ELSE_PACK_WORD(lpPA2BSrc[PAN_DISK_ATOB_NUM_MSB],
      lpPA2BSrc[PAN_DISK_ATOB_NUM_LSB]);

   /* If we are not supposed to get the rest of the array
    * then we are done.
    */
   if (!(uiFlags & ELSE_CVT_APPEND_ARRAY)) {
      return (sizeof(PANOSE_ATOB_DISK));
   }
   /* Compute the size we need in order to walk the array.
    */
   uiSizeNeed = sizeof(PANOSE_ATOB_DISK) +
      (lpPA2BDst->uiNumAtoB * sizeof(PANOSE_ATOB_ITEM_DISK));

   /* Abort if the structure is not large enough.
    */
   if (uiSizeNeed > uiSize) {
      return (-(int)uiSizeNeed);
   }
   /* Walk the array converting the records.
    */
   for (i = 0, lpPA2BItem = lpPA2BDst->AtoBItem,
      lpPA2BSrc += sizeof(PANOSE_ATOB_DISK); i < lpPA2BDst->uiNumAtoB;
      ++i, ++lpPA2BItem, lpPA2BSrc += sizeof(PANOSE_ATOB_ITEM_DISK)) {

      lpPA2BItem->uiAttrA =
         M_ELSE_PACK_WORD(lpPA2BSrc[PAN_DISK_ATOB_ITEM_A_MSB],
         lpPA2BSrc[PAN_DISK_ATOB_ITEM_A_LSB]);
      lpPA2BItem->uiAttrB =
         M_ELSE_PACK_WORD(lpPA2BSrc[PAN_DISK_ATOB_ITEM_B_MSB],
         lpPA2BSrc[PAN_DISK_ATOB_ITEM_B_LSB]);
   }
   /* Return success.
    */
   return (uiSizeNeed);
}

/***************************************************************************
 * FUNCTION: iELSECvtPRngDiskToMem
 *
 * PURPOSE:  Convert the penalty range record array from disk format
 *           to memory format.
 *
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function
 *           converts an array of range records.  It is assumed
 *           lpPRngDst and lpPRngSrc point to the first element in
 *           the array and uiNumRecs contains the count of records.
 *
 *           It is acceptable for lpPRngDst and lpPRngSrc to point
 *           to the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtPRngDiskToMem (
   LPPANOSE_PRANGE_MEM lpPRngDst,
   LPEBYTE lpPRngSrc,
   UINT uiSize,
   UINT uiFlags,
   UINT uiNumRecs )
{
   UINT i;
   UINT uiSizeNeed;

   /* Compute the size we need.
    */
   if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
      uiSizeNeed = uiNumRecs * sizeof(PANOSE_PRANGE_DISK);
   } else {
      uiSizeNeed = sizeof(PANOSE_PRANGE_DISK);
      uiNumRecs = 1;
   }
   /* If the structure is not large enough then
    * return an error.
    */
   if (uiSizeNeed > uiSize) {
      return (-(int)uiSizeNeed);
   }
   /* Walk the array converting it.  Notice if we are only
    * converting one record we fixed uiNumRecs to 1 above.
    */
   for (i = 0; i < uiNumRecs;
      ++i, ++lpPRngDst, lpPRngSrc += sizeof(PANOSE_PRANGE_DISK)) {

      lpPRngDst->jRangeLast = lpPRngSrc[PAN_DISK_PRNG_LAST];
      lpPRngDst->jCompress = lpPRngSrc[PAN_DISK_PRNG_COMPRESS];
      lpPRngDst->ulOffsTbl =
         M_ELSE_PACK_DWORD(lpPRngSrc[PAN_DISK_PRNG_OFFSTBL_MSW_MSB],
         lpPRngSrc[PAN_DISK_PRNG_OFFSTBL_MSW_LSB],
         lpPRngSrc[PAN_DISK_PRNG_OFFSTBL_LSW_MSB],
         lpPRngSrc[PAN_DISK_PRNG_OFFSTBL_LSW_LSB]);
      lpPRngDst->uiTblSize =
         M_ELSE_PACK_WORD(lpPRngSrc[PAN_DISK_PRNG_TBLSIZE_MSB],
         lpPRngSrc[PAN_DISK_PRNG_TBLSIZE_LSB]);
   }
   /* Return success.
    */
   return (uiSizeNeed);
}

/***************************************************************************
 * FUNCTION: iELSECvtPC2DiskToMem
 *
 * PURPOSE:  Convert the penalty table with C2 compression from disk
 *           to memory format.
 *
 *           The routine only converts the first few fields in the
 *           table.  The end of the table contains a variable length
 *           array of bytes which is assumed to already be in correct
 *           order.
 *
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function
 *           copies over the variable length array of penalties out
 *           to uiSize.
 *
 *           It is acceptable for lpPC2Dst and lpPC2Src to point to
 *           the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtPC2DiskToMem (
   LPPANOSE_PTABLE_C2_MEM lpPC2Dst,
   LPEBYTE lpPC2Src,
   UINT uiSize,
   UINT uiFlags )
{
   UINT i;
   LPEBYTE lpjDst;

   /* Make sure the size is large enough.
    */
   if (uiSize < sizeof(PANOSE_PTABLE_C2_DISK)) {
      return (-(int)sizeof(PANOSE_PTABLE_C2_DISK));
   }
   /* Convert the structure.
    */
   lpPC2Dst->jAnyPenalty = lpPC2Src[PAN_DISK_C2_ANY];
   lpPC2Dst->jNoFitPenalty = lpPC2Src[PAN_DISK_C2_NOFIT];
   lpPC2Dst->jMatchPenalty = lpPC2Src[PAN_DISK_C2_MATCH];

   /* Stop now if we have not been asked to copy over penalty
    * values.
    */
   if (!(uiFlags & ELSE_CVT_APPEND_ARRAY)) {
      return (sizeof(PANOSE_PTABLE_C2_DISK));
   }
   for (i = sizeof(PANOSE_PTABLE_C2_DISK),
      lpjDst = lpPC2Dst->jPenalties,
      lpPC2Src += sizeof(PANOSE_PTABLE_C2_DISK); i < uiSize;
      ++i, *lpjDst++ = *lpPC2Src++)
      ;
   /* Return success.
    */
   return (uiSize);
}

/***************************************************************************
 * FUNCTION: iELSECvtPHeadMemToDisk
 *
 * PURPOSE:  Convert the penalty dictionary header from memory format
 *           to disk format.
 *
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function also
 *           converts the index array immediately following the header.
 *
 *           It is acceptable for lpPHeadDst and lpPHeadSrc to point
 *           to the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtPHeadMemToDisk (
   LPEBYTE lpPHeadDst,
   LPPANOSE_PDICT_MEM lpPHeadSrc,
   UINT uiSize,
   UINT uiFlags )
{
   UINT uiVal;
   UINT uiSizeIndex = lpPHeadSrc->uiSizeIndex;
   UINT uiNumDicts = lpPHeadSrc->uiNumDicts;
   ULONG ulVal;
   INT iRetVal;

   /* If neither source or destination are large enough to hold
    * the main part of the header then abort now.
    */
   if (uiSize < sizeof(PANOSE_PDICT_DISK)) {
      return (-(int)sizeof(PANOSE_PDICT_DISK));
   }
   /* Convert the main body of the header.
    */
   uiVal = lpPHeadSrc->uiVersion;
   lpPHeadDst[PAN_DISK_DICT_VERSION_MSB] = M_ELSE_GET_MSB(uiVal);
   lpPHeadDst[PAN_DISK_DICT_VERSION_LSB] = M_ELSE_GET_LSB(uiVal);
   uiVal = lpPHeadSrc->uiNumDicts;
   lpPHeadDst[PAN_DISK_DICT_NUMDICTS_MSB] = M_ELSE_GET_MSB(uiVal);
   lpPHeadDst[PAN_DISK_DICT_NUMDICTS_LSB] = M_ELSE_GET_LSB(uiVal);
   uiVal = lpPHeadSrc->uiSizeIndex;
   lpPHeadDst[PAN_DISK_DICT_SIZE_IND_MSB] = M_ELSE_GET_MSB(uiVal);
   lpPHeadDst[PAN_DISK_DICT_SIZE_IND_LSB] = M_ELSE_GET_LSB(uiVal);
   uiVal = lpPHeadSrc->uiSizeFullHd;
   lpPHeadDst[PAN_DISK_DICT_SIZE_HEAD_MSB] = M_ELSE_GET_MSB(uiVal);
   lpPHeadDst[PAN_DISK_DICT_SIZE_HEAD_LSB] = M_ELSE_GET_LSB(uiVal);
   ulVal = lpPHeadSrc->ulSizeFullDB;
   lpPHeadDst[PAN_DISK_DICT_SIZE_DB_MSW_MSB] =
      M_ELSE_GET_MSB(M_ELSE_GET_MSW(ulVal));
   lpPHeadDst[PAN_DISK_DICT_SIZE_DB_MSW_LSB] =
      M_ELSE_GET_LSB(M_ELSE_GET_MSW(ulVal));
   lpPHeadDst[PAN_DISK_DICT_SIZE_DB_LSW_MSB] =
      M_ELSE_GET_MSB(M_ELSE_GET_LSW(ulVal));
   lpPHeadDst[PAN_DISK_DICT_SIZE_DB_LSW_LSB] =
      M_ELSE_GET_LSB(M_ELSE_GET_LSW(ulVal));

   /* Stop here if we are not supposed to convert the index
    * array immediately following the header.
    */
   if (!(uiFlags & ELSE_CVT_APPEND_ARRAY)) {
      return (sizeof(PANOSE_PDICT_DISK));
   }
   /* If the size of the structure is not large enough then
    * return an error.
    */
   if (uiSizeIndex > uiSize) {
      return (-(int)uiSizeIndex);
   }
   /* Walk the index array.
    */
   iRetVal = iELSECvtPIndMemToDisk(&lpPHeadDst[PAN_DISK_DICT_COUNT],
      lpPHeadSrc->pindex, uiSize - sizeof(PANOSE_PDICT_DISK),
      uiFlags, uiNumDicts);

   /* Return error or success depending upon how the index
    * convert routine worked.
    */
   if (iRetVal == 0) {
      return (0);
   } else if (iRetVal < 0) {
      return (iRetVal - sizeof(PANOSE_PDICT_DISK));
   } else {
      return (iRetVal + sizeof(PANOSE_PDICT_DISK));
   }
}

/***************************************************************************
 * FUNCTION: iELSECvtPIndMemToDisk
 *
 * PURPOSE:  Convert the penalty dictionary index (the variable length
 *           array of index records immediately following the header)
 *           from memory format to disk format.
 *
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function
 *           converts an array of index records.  It is assumed
 *           lpPIndDst and lpPIndSrc point to the first element in
 *           the array and uiNumRecs contains the count of records.
 *
 *           It is acceptable for lpPIndDst and lpPIndSrc to point
 *           to the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtPIndMemToDisk (
   LPEBYTE lpPIndDst,
   LPPANOSE_PINDEX_MEM lpPIndSrc,
   UINT uiSize,
   UINT uiFlags,
   UINT uiNumRecs )
{
   UINT i;
   UINT uiVal;
   UINT uiSizeNeed;

   /* Compute the size we need.
    */
   if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
      uiSizeNeed = uiNumRecs * sizeof(PANOSE_PINDEX_DISK);
   } else {
      uiSizeNeed = sizeof(PANOSE_PINDEX_DISK);
      uiNumRecs = 1;
   }
   /* If the structure is not large enough then
    * return an error.
    */
   if (uiSizeNeed > uiSize) {
      return (-(int)uiSizeNeed);
   }
   /* Walk the array converting it.  Notice if we are only
    * converting one record we fixed uiNumRecs to 1 above.
    */
   for (i = 0; i < uiNumRecs;
      ++i, lpPIndDst += sizeof(PANOSE_PINDEX_DISK), ++lpPIndSrc) {

      uiVal = lpPIndSrc->uiNumAttr;
      lpPIndDst[PAN_DISK_PIND_NUMATTR_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPIndDst[PAN_DISK_PIND_NUMATTR_LSB] = M_ELSE_GET_LSB(uiVal);
      uiVal = lpPIndSrc->uiScriptA;
      lpPIndDst[PAN_DISK_PIND_SCRIPTA_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPIndDst[PAN_DISK_PIND_SCRIPTA_LSB] = M_ELSE_GET_LSB(uiVal);
      uiVal = lpPIndSrc->uiScriptB;
      lpPIndDst[PAN_DISK_PIND_SCRIPTB_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPIndDst[PAN_DISK_PIND_SCRIPTB_LSB] = M_ELSE_GET_LSB(uiVal);
      uiVal = lpPIndSrc->uiGenreA;
      lpPIndDst[PAN_DISK_PIND_GENREA_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPIndDst[PAN_DISK_PIND_GENREA_LSB] = M_ELSE_GET_LSB(uiVal);
      uiVal = lpPIndSrc->uiGenreB;
      lpPIndDst[PAN_DISK_PIND_GENREB_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPIndDst[PAN_DISK_PIND_GENREB_LSB] = M_ELSE_GET_LSB(uiVal);
      lpPIndDst[PAN_DISK_PIND_DEF_ANY] = lpPIndSrc->jDefAnyPenalty;
      lpPIndDst[PAN_DISK_PIND_DEF_NOFIT] = lpPIndSrc->jDefNoFitPenalty;
      lpPIndDst[PAN_DISK_PIND_DEF_MATCH] = lpPIndSrc->jDefMatchPenalty;
      lpPIndDst[PAN_DISK_PIND_RESERVED_0] = lpPIndSrc->jReserved;
      uiVal = lpPIndSrc->uiOffsRngArray;
      lpPIndDst[PAN_DISK_PIND_OFFSRNG_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPIndDst[PAN_DISK_PIND_OFFSRNG_LSB] = M_ELSE_GET_LSB(uiVal);
      uiVal = lpPIndSrc->uiOffsWeights;
      lpPIndDst[PAN_DISK_PIND_OFFSWT_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPIndDst[PAN_DISK_PIND_OFFSWT_LSB] = M_ELSE_GET_LSB(uiVal);
      uiVal = lpPIndSrc->uiOffsAttrAtoB;
      lpPIndDst[PAN_DISK_PIND_OFFSATOB_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPIndDst[PAN_DISK_PIND_OFFSATOB_LSB] = M_ELSE_GET_LSB(uiVal);
   }
   /* Return success.
    */
   return (uiSizeNeed);
}

/***************************************************************************
 * FUNCTION: iELSECvtPA2BMemToDisk
 *
 * PURPOSE:  Convert the penalty A-to-B remapping table from memory
 *           format to Disk format.
 * 
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function
 *           converts the variable length array of A-to-B remap
 *           records.  Without it, the function just converts the
 *           header which contains the count of remap records.
 *
 *           No 'count' variable is passed in because the function
 *           obtains that from the A-to-B header.
 *
 *           It is acceptable for lpPA2BDst and lpPA2BSrc to point
 *           to the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtPA2BMemToDisk (
   LPEBYTE lpPA2BDst,
   LPPANOSE_ATOB_MEM lpPA2BSrc,
   UINT uiSize,
   UINT uiFlags )
{
   UINT i;
   UINT uiVal;
   UINT uiNumAtoB = lpPA2BSrc->uiNumAtoB;
   UINT uiSizeNeed;
   LPPANOSE_ATOB_ITEM_MEM lpPA2BItem;

   /* First attempt to convert the header.
    */
   if (uiSize < sizeof(PANOSE_ATOB_DISK)) {
      return (-(int)sizeof(PANOSE_ATOB_DISK));
   }
   lpPA2BDst[PAN_DISK_ATOB_NUM_MSB] = M_ELSE_GET_MSB(uiNumAtoB);
   lpPA2BDst[PAN_DISK_ATOB_NUM_LSB] = M_ELSE_GET_LSB(uiNumAtoB);

   /* If we are not supposed to get the rest of the array
    * then we are done.
    */
   if (!(uiFlags & ELSE_CVT_APPEND_ARRAY)) {
      return (sizeof(PANOSE_ATOB_DISK));
   }
   /* Compute the size we need in order to walk the array.
    */
   uiSizeNeed = sizeof(PANOSE_ATOB_DISK) +
      (uiNumAtoB * sizeof(PANOSE_ATOB_ITEM_DISK));

   /* Abort if the structure is not large enough.
    */
   if (uiSizeNeed > uiSize) {
      return (-(int)uiSizeNeed);
   }
   /* Walk the array converting the records.
    */
   for (i = 0, lpPA2BDst += sizeof(PANOSE_ATOB_DISK),
      lpPA2BItem = lpPA2BSrc->AtoBItem; i < uiNumAtoB;
      ++i, lpPA2BDst += sizeof(PANOSE_ATOB_ITEM_DISK), ++lpPA2BItem) {

      uiVal = lpPA2BItem->uiAttrA;
      lpPA2BDst[PAN_DISK_ATOB_ITEM_A_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPA2BDst[PAN_DISK_ATOB_ITEM_A_LSB] = M_ELSE_GET_LSB(uiVal);
      uiVal = lpPA2BItem->uiAttrB;
      lpPA2BDst[PAN_DISK_ATOB_ITEM_B_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPA2BDst[PAN_DISK_ATOB_ITEM_B_LSB] = M_ELSE_GET_LSB(uiVal);
   }
   /* Return success.
    */
   return (uiSizeNeed);
}

/***************************************************************************
 * FUNCTION: iELSECvtPRngMemToDisk
 *
 * PURPOSE:  Convert the penalty range record array from memory
 *           format to disk format.
 *
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function
 *           converts an array of range records.  It is assumed
 *           lpPRngDst and lpPRngSrc point to the first element in
 *           the array and uiNumRecs contains the count of records.
 *
 *           It is acceptable for lpPRngDst and lpPRngSrc to point
 *           to the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtPRngMemToDisk (
   LPEBYTE lpPRngDst,
   LPPANOSE_PRANGE_MEM lpPRngSrc,
   UINT uiSize,
   UINT uiFlags,
   UINT uiNumRecs )
{
   UINT i;
   UINT uiVal;
   UINT uiSizeNeed;
   ULONG ulVal;

   /* Compute the size we need.
    */
   if (uiFlags & ELSE_CVT_APPEND_ARRAY) {
      uiSizeNeed = uiNumRecs * sizeof(PANOSE_PRANGE_DISK);
   } else {
      uiSizeNeed = sizeof(PANOSE_PRANGE_DISK);
      uiNumRecs = 1;
   }
   /* If the structure is not large enough then
    * return an error.
    */
   if (uiSizeNeed > uiSize) {
      return (-(int)uiSizeNeed);
   }
   /* Walk the array converting it.  Notice if we are only
    * converting one record we fixed uiNumRecs to 1 above.
    */
   for (i = 0; i < uiNumRecs;
      ++i, lpPRngDst += sizeof(PANOSE_PRANGE_DISK), ++lpPRngSrc) {

      lpPRngDst[PAN_DISK_PRNG_LAST] = lpPRngSrc->jRangeLast;
      lpPRngDst[PAN_DISK_PRNG_COMPRESS] = lpPRngSrc->jCompress;
      ulVal = lpPRngSrc->ulOffsTbl;
      lpPRngDst[PAN_DISK_PRNG_OFFSTBL_MSW_MSB] =
         M_ELSE_GET_MSB(M_ELSE_GET_MSW(ulVal));
      lpPRngDst[PAN_DISK_PRNG_OFFSTBL_MSW_LSB] =
         M_ELSE_GET_LSB(M_ELSE_GET_MSW(ulVal));
      lpPRngDst[PAN_DISK_PRNG_OFFSTBL_LSW_MSB] =
         M_ELSE_GET_MSB(M_ELSE_GET_LSW(ulVal));
      lpPRngDst[PAN_DISK_PRNG_OFFSTBL_LSW_LSB] =
         M_ELSE_GET_LSB(M_ELSE_GET_LSW(ulVal));
      uiVal = lpPRngSrc->uiTblSize;
      lpPRngDst[PAN_DISK_PRNG_TBLSIZE_MSB] = M_ELSE_GET_MSB(uiVal);
      lpPRngDst[PAN_DISK_PRNG_TBLSIZE_LSB] = M_ELSE_GET_LSB(uiVal);
   }
   /* Return success.
    */
   return (uiSizeNeed);
}

/***************************************************************************
 * FUNCTION: iELSECvtPC2MemToDisk
 *
 * PURPOSE:  Convert the penalty table with C2 compression from memory
 *           to disk format.
 *
 *           The routine only converts the first few fields in the
 *           table.  The end of the table contains a variable length
 *           array of bytes which is assumed to already be in correct
 *           order.
 *
 *           If ELSE_CVT_APPEND_ARRAY is specified, the function
 *           copies over the variable length array of penalties out
 *           to uiSize.
 *
 *           It is acceptable for lpPC2Dst and lpPC2Src to point to
 *           the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtPC2MemToDisk (
   LPEBYTE lpPC2Dst,
   LPPANOSE_PTABLE_C2_MEM lpPC2Src,
   UINT uiSize,
   UINT uiFlags )
{
   UINT i;
   LPEBYTE lpjSrc;

   /* Make sure the size is large enough.
    */
   if (uiSize < sizeof(PANOSE_PTABLE_C2_DISK)) {
      return (-(int)sizeof(PANOSE_PTABLE_C2_DISK));
   }
   /* Convert the structure.
    */
   lpPC2Dst[PAN_DISK_C2_ANY] = lpPC2Src->jAnyPenalty;
   lpPC2Dst[PAN_DISK_C2_NOFIT] = lpPC2Src->jNoFitPenalty;
   lpPC2Dst[PAN_DISK_C2_MATCH] = lpPC2Src->jMatchPenalty;

   /* Stop now if we have not been asked to copy over penalty
    * values.
    */
   if (!(uiFlags & ELSE_CVT_APPEND_ARRAY)) {
      return (sizeof(PANOSE_PTABLE_C2_DISK));
   }
   for (i = sizeof(PANOSE_PTABLE_C2_DISK),
      lpPC2Dst += sizeof(PANOSE_PTABLE_C2_DISK),
      lpjSrc = lpPC2Src->jPenalties; i < uiSize;
      ++i, *lpPC2Dst++ = *lpjSrc++)
      ;
   /* Return success.
    */
   return (uiSize);
}

/***************************************************************************
 ******************** FULL PENALTY DICTIONARY CONVERSION *******************
 ***************************************************************************/

/***************************************************************************
 * FUNCTION: lELSECvtFullPDictDiskToMem
 *
 * PURPOSE:  Convert the entire penalty dictionary from disk format
 *           to memory format.
 *
 *           The routine maintains the same offsets in the structures,
 *           and the size does not change -- i.e., it just swaps bytes
 *           in the appropriate places.
 *
 *           It is acceptable for lpPDictDst and lpPDictSrc to point
 *           to the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
LONG ELSEFARPROC ELSEPASCAL lELSECvtFullPDictDiskToMem (
   LPEBYTE lpPDictDst,
   LPEBYTE lpPDictSrc,
   UINT uiSize )
{
   UINT i;
   UINT j;
   UINT k;
   UINT uiOffsTbl;
   INT iRetVal;
   LPPANOSE_PDICT_MEM lpPHeadDst = (LPPANOSE_PDICT_MEM)lpPDictDst;
   LPPANOSE_PINDEX_MEM lpPIndRec;
   LPPANOSE_PRANGE_MEM lpPRngRec;
   LPPANOSE_ATOB_MEM lpPA2BRec;
   LPEBYTE lpjDst;
   LPEBYTE lpjSrc;

   /* First get the header including the index.
    */
   if ((iRetVal = iELSECvtPHeadDiskToMem(lpPHeadDst,
      lpPDictSrc, uiSize, ELSE_CVT_APPEND_ARRAY)) <= 0) {
      return (iRetVal);
   }
   /* Make sure we can handle this version of the structure.
    */
   if (lpPHeadDst->uiVersion != PANOSE_PENALTY_VERS) {
      return (0);
   }
   /* Walk the index copying/converting each of the
    * penalty dictionaries.
    */
   for (i = 0, lpPIndRec = lpPHeadDst->pindex; i < lpPHeadDst->uiNumDicts;
      ++i, ++lpPIndRec) {

      /* Abort if the range array offset is invalid.
       */
      if (lpPIndRec->uiOffsRngArray == 0) {
         return (0);
      } else if ((lpPIndRec->uiOffsRngArray +
         (lpPIndRec->uiNumAttr * sizeof(PANOSE_PRANGE_DISK))) > uiSize) {
         return (-(int)lpPIndRec->uiOffsRngArray -
            ((INT)lpPIndRec->uiNumAttr * sizeof(PANOSE_PRANGE_DISK)));
      }
      /* Convert the range array.
       */
      lpPRngRec = (LPPANOSE_PRANGE_MEM)&lpPDictDst[lpPIndRec->uiOffsRngArray];
      if ((iRetVal = iELSECvtPRngDiskToMem(lpPRngRec,
         &lpPDictSrc[lpPIndRec->uiOffsRngArray],
         uiSize - lpPIndRec->uiOffsRngArray,
         ELSE_CVT_APPEND_ARRAY, lpPIndRec->uiNumAttr)) <= 0) {
             
         return ((iRetVal == 0) ?
            0 : (iRetVal - (INT)lpPIndRec->uiOffsRngArray));
      }
      /* Abort if the weights offset is invalid.
       */
      if (lpPIndRec->uiOffsWeights == 0) {
         return (0);
      } else if ((lpPIndRec->uiOffsWeights +
         (lpPIndRec->uiNumAttr * sizeof(BYTE))) > uiSize) {
         return (-(int)lpPIndRec->uiOffsWeights -
            ((INT)lpPIndRec->uiNumAttr * sizeof(BYTE)));
      }
      /* Transfer the weights array.
       */
      for (j = 0, lpjDst = &lpPDictDst[lpPIndRec->uiOffsWeights],
         lpjSrc = &lpPDictSrc[lpPIndRec->uiOffsWeights];
         j < lpPIndRec->uiNumAttr;
         ++j, *lpjDst++ = *lpjSrc++)
         ;
      /* It is valid for the offset to the A-to-B remapping
       * array to be zero -- that means one is not present.
       */
      if (lpPIndRec->uiOffsAttrAtoB != 0) {

         /* Make sure the offset is valid.
          */
         if ((lpPIndRec->uiOffsAttrAtoB +
            sizeof(PANOSE_ATOB_DISK)) > uiSize) {
            return (-(int)lpPIndRec->uiOffsAttrAtoB - sizeof(PANOSE_ATOB_DISK));
         }
         /* Convert the A-to-B remapping array.
         */
         lpPA2BRec = (LPPANOSE_ATOB_MEM)&lpPDictDst[lpPIndRec->uiOffsAttrAtoB];
         if ((iRetVal = iELSECvtPA2BDiskToMem(lpPA2BRec,
            &lpPDictSrc[lpPIndRec->uiOffsAttrAtoB],
            uiSize - lpPIndRec->uiOffsAttrAtoB,
            ELSE_CVT_APPEND_ARRAY)) <= 0) {
             
            return ((iRetVal == 0) ?
               0 : (iRetVal - (INT)lpPIndRec->uiOffsAttrAtoB));
         }
      }
      /* Now walk each of the range array records, converting
       * the penalty tables.
       */
      for (j = 0; j < lpPIndRec->uiNumAttr; ++j, ++lpPRngRec) {

        /* One caveat in the way we handle these records is that
         * the offset to the penalty table is a LONG, which cannot
         * just be indexed into an array in systems that use
         * segment:offset addressing.  We cast the value to UINT
         * which may cause it to be truncated in some environments.
         */
         uiOffsTbl = (UINT)lpPRngRec->ulOffsTbl;
         if ((ULONG)uiOffsTbl != lpPRngRec->ulOffsTbl) {
            return (0);
         }
         /* If there is a table then make sure there is room.
          */
         if ((uiOffsTbl != 0) && (lpPRngRec->uiTblSize != 0) &&
            ((uiOffsTbl + (lpPRngRec->uiTblSize * sizeof(BYTE))) > uiSize)) {
            return (-(INT)uiOffsTbl -
               ((INT)lpPRngRec->uiTblSize * sizeof(BYTE)));
         }

         switch (lpPRngRec->jCompress) {
            case PANOSE_COMPRESS_C0:
               /* Flat n x n table, copy over.
                */
               if (!uiOffsTbl || !lpPRngRec->uiTblSize) {
                  return (0);
               }
               for (k = 0, lpjDst = &lpPDictDst[uiOffsTbl],
                  lpjSrc = &lpPDictSrc[uiOffsTbl]; k < lpPRngRec->uiTblSize;
                  ++k, *lpjDst++ = *lpjSrc++)
                  ;
               break;

            case PANOSE_COMPRESS_C1:
               /* No extra table, do nothing.
                */
               break;

            case PANOSE_COMPRESS_C2:
               /* Partial table provided.  Call convert routine.
                */
               if (!uiOffsTbl || !lpPRngRec->uiTblSize) {
                  return (0);
               }
               if ((iRetVal = iELSECvtPC2DiskToMem(
                  (LPPANOSE_PTABLE_C2_MEM)&lpPDictDst[uiOffsTbl],
                  &lpPDictSrc[uiOffsTbl], lpPRngRec->uiTblSize,
                  ELSE_CVT_APPEND_ARRAY)) <= 0) {
                  return ((iRetVal == 0) ?
                     0 : (iRetVal - (INT)uiOffsTbl));
               }
               break;

            default:
               /* Error! Compression format we are not
                * familiar with.
                */
               return (0);
         }
      }
   }
   /* Return success.
    */
   return (lpPHeadDst->ulSizeFullDB);
}

/***************************************************************************
 * FUNCTION: lELSECvtFullPDictMemToDisk
 *
 * PURPOSE:  Convert the entire penalty dictionary from memory format
 *           to disk format.
 *
 *           The routine maintains the same offsets in the structures,
 *           and the size does not change -- i.e., it just swaps bytes
 *           in the appropriate places.
 *
 *           It is acceptable for lpPDictDst and lpPDictSrc to point
 *           to the same memory location.
 *
 * RETURNS:  The function returns the size of the destination structure
 *           if successful, zero if an error occurs, or a negative
 *           count if the destination or source structure is not large
 *           enough.  The inverse of the count indicates the minimum
 *           size required.
 ***************************************************************************/
LONG ELSEFARPROC ELSEPASCAL lELSECvtFullPDictMemToDisk (
   LPEBYTE lpPDictDst,
   LPEBYTE lpPDictSrc,
   UINT uiSize )
{
   UINT i;
   UINT j;
   UINT k;
   UINT uiOffsTbl;
   UINT uiNumDicts;
   ULONG ulSizeFullDB;
   INT iRetVal;
   LPPANOSE_PDICT_MEM lpPHeadSrc = (LPPANOSE_PDICT_MEM)lpPDictSrc;
   LPPANOSE_PINDEX_MEM lpPIndSrc;
   LPPANOSE_PRANGE_MEM lpPRngSrc;
   LPEBYTE lpPIndDst;
   LPEBYTE lpPRngDst;
   LPEBYTE lpjDst;
   LPEBYTE lpjSrc;
   PANOSE_PINDEX_MEM panIndRec;
   PANOSE_PRANGE_MEM panRngRec;

   /* Make a local copy of header variables before conversion.
    */
   uiNumDicts = lpPHeadSrc->uiNumDicts;
   ulSizeFullDB = lpPHeadSrc->ulSizeFullDB;

   /* Make sure we at least have enough room to convert
    * the entire header.  Notice we are expecting the
    * header to contain valid values.
    */
   if (lpPHeadSrc->uiSizeIndex > uiSize) {
      return (-(int)lpPHeadSrc->uiSizeIndex);
   }
   /* First convert the header without the index.
    */
   if ((iRetVal = iELSECvtPHeadMemToDisk(lpPDictDst,
      lpPHeadSrc, uiSize, 0)) <= 0) {
      return (iRetVal);
   }
   /* Walk the index array copying/converting each of the
    * penalty dictionaries.
    */
   for (i = 0, lpPIndDst = &lpPDictDst[PAN_DISK_DICT_COUNT],
      lpPIndSrc = lpPHeadSrc->pindex;
      i < uiNumDicts;
      ++i, ++lpPIndSrc, lpPIndDst += sizeof(PANOSE_PINDEX_DISK)) {

      /* Make a local copy of the index record before
       * conversion.
       */
      panIndRec = *lpPIndSrc;

      /* Convert the index record.
       */
      if ((iRetVal = iELSECvtPIndMemToDisk(lpPIndDst, lpPIndSrc,
         sizeof(PANOSE_PINDEX_DISK), 0, 1)) <= 0) {
         return ((iRetVal == 0) ?
            0 : (iRetVal - ((INT)i * sizeof(PANOSE_PINDEX_DISK))));
      }
      /* Abort if the range array offset is invalid.
       */
      if (panIndRec.uiOffsRngArray == 0) {
         return (0);
      } else if ((panIndRec.uiOffsRngArray +
         (panIndRec.uiNumAttr * sizeof(PANOSE_PRANGE_DISK))) > uiSize) {
         return (-(int)panIndRec.uiOffsRngArray -
            ((INT)panIndRec.uiNumAttr * sizeof(PANOSE_PRANGE_DISK)));
      }
      /* Abort if the weights offset is invalid.
       */
      if (panIndRec.uiOffsWeights == 0) {
         return (0);
      } else if ((panIndRec.uiOffsWeights +
         (panIndRec.uiNumAttr * sizeof(BYTE))) > uiSize) {
         return (-(int)panIndRec.uiOffsWeights -
            ((INT)panIndRec.uiNumAttr * sizeof(BYTE)));
      }
      /* Transfer the weights array.
       */
      for (j = 0, lpjDst = &lpPDictDst[panIndRec.uiOffsWeights],
         lpjSrc = &lpPDictSrc[panIndRec.uiOffsWeights];
         j < panIndRec.uiNumAttr;
         ++j, *lpjDst++ = *lpjSrc++)
         ;
      /* It is valid for the offset to the A-to-B remapping
       * array to be zero -- that means one is not present.
       */
      if (panIndRec.uiOffsAttrAtoB != 0) {

         /* Make sure the offset is valid.
          */
         if ((panIndRec.uiOffsAttrAtoB +
            sizeof(PANOSE_ATOB_DISK)) > uiSize) {
            return (-(int)panIndRec.uiOffsAttrAtoB - sizeof(PANOSE_ATOB_DISK));
         }
         /* Convert the A-to-B remapping array.
         */
         if ((iRetVal = iELSECvtPA2BMemToDisk(
            &lpPDictDst[panIndRec.uiOffsAttrAtoB],
            (LPPANOSE_ATOB_MEM)&lpPDictSrc[panIndRec.uiOffsAttrAtoB],
            uiSize - panIndRec.uiOffsAttrAtoB,
            ELSE_CVT_APPEND_ARRAY)) <= 0) {
             
            return ((iRetVal == 0) ?
               0 : (iRetVal - (INT)panIndRec.uiOffsAttrAtoB));
         }
      }
      /* Now walk each of the range array records, converting
       * the penalty tables.
       */
      for (j = 0, lpPRngDst = &lpPDictDst[panIndRec.uiOffsRngArray],
         lpPRngSrc = (LPPANOSE_PRANGE_MEM)&lpPDictSrc[panIndRec.uiOffsRngArray];
         j < panIndRec.uiNumAttr;
         ++j, ++lpPRngSrc, lpPRngDst += sizeof(PANOSE_PRANGE_DISK)) {

         /* Make a local copy of the range record before conversion.
          */
         panRngRec = *lpPRngSrc;

         /* Convert the range record.
          */
         if ((iRetVal = iELSECvtPRngMemToDisk(lpPRngDst, lpPRngSrc,
            sizeof(PANOSE_PRANGE_DISK), 0, 1)) <= 0) {
            return ((iRetVal == 0) ?
               0 : (iRetVal - (INT)panIndRec.uiOffsRngArray -
               ((INT)j * sizeof(PANOSE_PRANGE_DISK))));
         }
        /* One caveat in the way we handle these records is that
         * the offset to the penalty table is a LONG, which cannot
         * just be indexed into an array in systems that use
         * segment:offset addressing.  We cast the value to UINT
         * which may cause it to be truncated in some environments.
         *
         * In those cases we just fail -- we expect those environments
         * will not attempt to convert a whole database > 64k in one
         * operation.
         */
         uiOffsTbl = (UINT)panRngRec.ulOffsTbl;
         if ((ULONG)uiOffsTbl != panRngRec.ulOffsTbl) {
            return (0);
         }
         /* If there is a table then make sure there is room.
          */
         if ((uiOffsTbl != 0) && (panRngRec.uiTblSize != 0) &&
            ((uiOffsTbl + (panRngRec.uiTblSize * sizeof(BYTE))) > uiSize)) {
            return (-(INT)uiOffsTbl -
               ((INT)panRngRec.uiTblSize * sizeof(BYTE)));
         }
         /* Convert penalty table.
          */
         switch (panRngRec.jCompress) {
            case PANOSE_COMPRESS_C0:
               /* Flat n x n table, copy over.
                */
               if (!uiOffsTbl || !panRngRec.uiTblSize) {
                  return (0);
               }
               for (k = 0, lpjDst = &lpPDictDst[uiOffsTbl],
                  lpjSrc = &lpPDictSrc[uiOffsTbl]; k < panRngRec.uiTblSize;
                  ++k, *lpjDst++ = *lpjSrc++)
                  ;
               break;

            case PANOSE_COMPRESS_C1:
               /* No extra table, do nothing.
                */
               break;

            case PANOSE_COMPRESS_C2:
               /* Partial table provided.  Call convert routine.
                */
               if (!uiOffsTbl || !panRngRec.uiTblSize) {
                  return (0);
               }
               if ((iRetVal = iELSECvtPC2MemToDisk(&lpPDictDst[uiOffsTbl],
                  (LPPANOSE_PTABLE_C2_MEM)&lpPDictSrc[uiOffsTbl],
                  panRngRec.uiTblSize, ELSE_CVT_APPEND_ARRAY)) <= 0) {
                  return ((iRetVal == 0) ?
                     0 : (iRetVal - (INT)uiOffsTbl));
               }
               break;

            default:
               /* Error! Compression format we are not
                * familiar with.
                */
               return (0);
         }
      }
   }
   /* Return success.
    */
   return (ulSizeFullDB);
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

