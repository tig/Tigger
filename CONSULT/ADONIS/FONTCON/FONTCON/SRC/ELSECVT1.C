/***************************************************************************
 * ELSECVT1.C - Routines for converting between 10-digit PANOSE(tm) and
 *              Expanded PANOSE(tm).
 *
 * $keywords: elsecvt1.c 1.0  3-Feb-92 4:58:20 PM$
 *
 * Copyright (C) 1991 ElseWare Corporation.  All rights reserved.
 ***************************************************************************/

#define ELSE_MAPPER_CORE
#include "elsemap.h"

ELSELOCAL INT ELSENEARPROC ELSEPASCAL iELSECvtLatTD10ToExp
   ELSEARGS (( LPPANOSE_NUM_MEM lpPanThis, UINT uiSizePanThis,
   UINT uiUniqueID, BYTE jFamily, BYTE jSerif, BYTE jWeight,
   BYTE jProportion, BYTE jContrast, BYTE jStroke, BYTE jArmStyle,
   BYTE jLetterform, BYTE jMidline, BYTE jXHeight ));

ELSELOCAL INT ELSENEARPROC ELSEPASCAL iELSECvtLatTDExpTo10
   ELSEARGS (( LPEBYTE lpjPAN10, LPPANOSE_NUM_MEM lpPanThis ));

/***************************************************************************
 * FUNCTION: iELSECvt10DigitToExpanded
 *
 * PURPOSE:  Form an Expanded PANOSE number structure from the passed-in
 *           10-digit PANOSE numbers.
 *
 *           This routine creates a BYTE format Expanded PANOSE number,
 *           the caller must use iELSECvtExpByteToNibble to change it
 *           into NIBBLE format.
 *
 * RETURNS:  The function returns the number of expanded digits filled
 *           in, zero if there was an error.  The count is positive if
 *           all digits that could be converted were converted.  It is
 *           negative if fewer digits were converted than possible.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvt10DigitToExpanded (
   LPPANOSE_NUM_MEM lpPanThis,
   UINT uiSizePanThis,
   UINT uiUniqueID,
   BYTE jFamily,
   BYTE jSerif,
   BYTE jWeight,
   BYTE jProportion,
   BYTE jContrast,
   BYTE jStroke,
   BYTE jArmStyle,
   BYTE jLetterform,
   BYTE jMidline,
   BYTE jXHeight )
{
   /* As of this writing, only Latin script is supported in the
    * 10-digit system.
    */
   switch (jFamily) {
      case FAMILY_LATTEXTANDDISP:
         return ( iELSECvtLatTD10ToExp(lpPanThis, uiSizePanThis, uiUniqueID,
            jFamily, jSerif, jWeight, jProportion, jContrast, jStroke,
            jArmStyle, jLetterform, jMidline, jXHeight) );
      case FAMILY_LATSCRIPT:
      case FAMILY_LATDECORATIVE:
      case FAMILY_LATPICTORIAL:
      case PANOSE_ANY:
      case PANOSE_NOFIT:
      default:
         vELSEMakeExpPANDummy(lpPanThis, uiSizePanThis);
         return (0);
   }
}

/***************************************************************************
 * FUNCTION: iELSECvt10DigitToExpIndirect
 *
 * PURPOSE:  Form an Expanded PANOSE number structure from the passed-in
 *           array of 10-digit PANOSE numbers.
 *
 *           This routine creates a BYTE format Expanded PANOSE number,
 *           the caller must use iELSECvtExpByteToNibble to change it
 *           into NIBBLE format.
 *
 * RETURNS:  The function returns the number of expanded digits filled
 *           in, zero if there was an error.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvt10DigitToExpIndirect (
   LPPANOSE_NUM_MEM lpPanThis,
   UINT uiSizePanThis,
   UINT uiUniqueID,
   LPEBYTE lpjPAN10 )
{
   return ( iELSECvt10DigitToExpanded(lpPanThis, uiSizePanThis, uiUniqueID,
      lpjPAN10[PANOSE_IND_FAMILY],
      lpjPAN10[PANOSE_IND_SERIF],
      lpjPAN10[PANOSE_IND_WEIGHT],
      lpjPAN10[PANOSE_IND_PROPORTION],
      lpjPAN10[PANOSE_IND_CONTRAST],
      lpjPAN10[PANOSE_IND_STROKE],
      lpjPAN10[PANOSE_IND_ARMSTYLE],
      lpjPAN10[PANOSE_IND_LTRFORM],
      lpjPAN10[PANOSE_IND_MIDLINE],
      lpjPAN10[PANOSE_IND_XHEIGHT]) );
}

/***************************************************************************
 * FUNCTION: iELSECvtExpTo10DigitIndirect
 *
 * PURPOSE:  Derive a 10-digit PANOSE number from an Expanded PANOSE
 *           number.
 *
 * RETURNS:  The function returns the count of PANOSE digits filled in,
 *           zero if there is an error.  The count is negative if less
 *           than the standard 10 digits were converted.
 ***************************************************************************/
INT ELSEFARPROC ELSEPASCAL iELSECvtExpTo10DigitIndirect (
   LPEBYTE lpjPAN10,
   LPPANOSE_NUM_MEM lpPanThis )
{
   UINT i;
   PANOSE_NUM_MEM panByte;

   /* Initialize 10-digit PANOSE numbers to 'no fit.'
    */
   for (i = 0; i <= PANOSE_IND__LAST; ++i) {
      lpjPAN10[i] = PANOSE_NOFIT;
   }
   /* If the expanded number is in NIBBLE format then convert
    * it to BYTE format.
    */
   if (M_PAN_IS_NIBBLE(lpPanThis->uiNumAttr)) {
      if (iELSECvtExpNibbleToByte(&panByte, sizeof(panByte),
         (LPPANOSE_NIB_NUM_MEM)lpPanThis) <= 0) {
         return (0);
      }
      lpPanThis = &panByte;
   }
   /* As of this writing, we only know how to convert Latin
    * Text and Display PANOSE numbers.
    */
   if (lpPanThis->uiScript == FAM_SCRIPT_LATIN &&
      lpPanThis->uiGenre == FAM_GENRE_TEXTANDDISP) {
      return ( iELSECvtLatTDExpTo10(lpjPAN10, lpPanThis) );
   } else
      return (0);
}

/***************************************************************************/
/************************** LOCAL SERVICE ROUTINES *************************/
/***************************************************************************/

/***************************************************************************
 * FUNCTION: iELSECvtLatTD10ToExp
 *
 * PURPOSE:  Form a Latin Text and Display Expanded PANOSE number
 *           structure from the passed-in 10-digit PANOSE digits.
 *
 *           This routine creates a BYTE format Expanded PANOSE number,
 *           the caller must use iELSECvtExpByteToNibble to change it
 *           into NIBBLE format.
 *
 * RETURNS:  The function returns the number of expanded digits filled
 *           in, zero if there was an error.  The count is positive if
 *           all digits that could be converted were converted.  It is
 *           negative if fewer digits were converted than possible.
 ***************************************************************************/
ELSELOCAL INT ELSENEARPROC ELSEPASCAL iELSECvtLatTD10ToExp (
   LPPANOSE_NUM_MEM lpPanThis,
   UINT uiSizePanThis,
   UINT uiUniqueID,
   BYTE jFamily,
   BYTE jSerif,
   BYTE jWeight,
   BYTE jProportion,
   BYTE jContrast,
   BYTE jStroke,
   BYTE jArmStyle,
   BYTE jLetterform,
   BYTE jMidline,
   BYTE jXHeight )
{
   UINT uiMaxMore;
   BYTE ELSEFARPTR *lpjMoreAttr;

   /* Compute the count of digits we can put in the variable
    * length attribute array at the end of the PANOSE number rec.
    */
   if (uiSizePanThis < M_SIZEOF_MIN_PANOSE_BYTE())
      return (0);
   uiMaxMore = M_COUNT_MORE_PANOSE_BYTE(uiSizePanThis);
   if (uiMaxMore > (PAN_IND_LATIN__LAST + 1))
      uiMaxMore = PAN_IND_LATIN__LAST + 1;

   /* The count of digits includes the four stored in all
    * PANOSE numbers.
    */
   lpPanThis->uiNumAttr = uiMaxMore + PAN_COUNT_SHARED;

   /* Latin script.
    */
   lpPanThis->uiScript = FAM_SCRIPT_LATIN;

   /* Text and Display genre.
    */
   lpPanThis->uiGenre = FAM_GENRE_TEXTANDDISP;

   /* Unique ID is provided by the caller as there is no equivalent
    * in 10-digit PANOSE.
    */
   lpPanThis->uiUniqueID = uiUniqueID;

   /* Transfer weight.
    */
   lpPanThis->jWeight = jWeight > WEIGHT__LAST ?
      (BYTE)PANOSE_NOFIT : jWeight;

   /* Derive distortion from proportion.
    */
   switch (jProportion) {
      case PANOSE_ANY:
         lpPanThis->jDistortion = PANOSE_ANY;
         break;
      case PROPORTION_OLDSTYLE:
      case PROPORTION_MODERN:
      case PROPORTION_EVENWIDTH:
      case PROPORTION_MONOSPACE:
         lpPanThis->jDistortion = PROP_DISTORT_NORMAL;
         break;
      case PROPORTION_EXPANDED:
         lpPanThis->jDistortion = PROP_DISTORT_EXPANDED;
         break;
      case PROPORTION_CONDENSED:
         lpPanThis->jDistortion = PROP_DISTORT_CONDENSED;
         break;
      case PROPORTION_VERYEXP:
         lpPanThis->jDistortion = PROP_DISTORT_VERYEXP;
         break;
      case PROPORTION_VERYCOND:
         lpPanThis->jDistortion = PROP_DISTORT_VERYCOND;
         break;
      case PANOSE_NOFIT:
      default:
         lpPanThis->jDistortion = PANOSE_NOFIT;
         break;
   }

   /* Set monospace flag.
    */
   switch (jProportion) {
      case PANOSE_ANY:
      case PROPORTION_OLDSTYLE:
      case PROPORTION_MODERN:
      case PROPORTION_EVENWIDTH:
      case PROPORTION_EXPANDED:
      case PROPORTION_CONDENSED:
      case PROPORTION_VERYEXP:
      case PROPORTION_VERYCOND:
         lpPanThis->jMonospace = PROP_MONOSPACE_NO;
         break;
      case PROPORTION_MONOSPACE:
         lpPanThis->jMonospace = PROP_MONOSPACE_YES;
         break;
      case PANOSE_NOFIT:
      default:
         lpPanThis->jMonospace = PANOSE_NOFIT;
         break;
   }

   /* Copy contrast.
    */
   lpPanThis->jContrast = jContrast > CONTRAST__LAST ?
      (BYTE)PANOSE_NOFIT : jContrast;

   /* For the rest of the attributes, test before each one to
    * see if there is enough room.
    */
   if (uiMaxMore <= PAN_IND_LATIN_SERIF)
      return (-(int)lpPanThis->uiNumAttr);

   /* Use direct pointer.
    */
   lpjMoreAttr = lpPanThis->ajMoreAttr;

   /* Copy serif.
    */
   lpjMoreAttr[PAN_IND_LATIN_SERIF] = jSerif > SERIF__LAST ?
      (BYTE)PANOSE_NOFIT : jSerif;

   if (uiMaxMore <= PAN_IND_LATIN_RATIO)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive ratio from proportion.
    */
   switch (jProportion) {
      case PROPORTION_OLDSTYLE:
         lpjMoreAttr[PAN_IND_LATIN_RATIO] = PROP_RATIO_OLDSTYLE;
         break;
      case PANOSE_ANY:
      case PROPORTION_EXPANDED:
      case PROPORTION_CONDENSED:
      case PROPORTION_VERYEXP:
      case PROPORTION_VERYCOND:
      case PROPORTION_MONOSPACE:
      case PROPORTION_MODERN:
         lpjMoreAttr[PAN_IND_LATIN_RATIO] = PROP_RATIO_MODERN;
         break;
      case PROPORTION_EVENWIDTH:
         lpjMoreAttr[PAN_IND_LATIN_RATIO] = PROP_RATIO_EVENWIDTH;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_RATIO] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_SPEED)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive speed from stroke variation.
    */
   switch (jStroke) {
      case STROKE_NOVARIATION:
         lpjMoreAttr[PAN_IND_LATIN_SPEED] = STROKE_SPEED_NONE;
         break;
      case PANOSE_ANY:
      case STROKE_GRADDIAG:
      case STROKE_GRADTRANS:
      case STROKE_GRADVERT:
      case STROKE_GRADHORZ:
         lpjMoreAttr[PAN_IND_LATIN_SPEED] = STROKE_SPEED_GRADUAL;
         break;
      case STROKE_RAPIDVERT:
      case STROKE_RAPIDHORZ:
         lpjMoreAttr[PAN_IND_LATIN_SPEED] = STROKE_SPEED_RAPID;
         break;
      case STROKE_INSTANTVERT:
         lpjMoreAttr[PAN_IND_LATIN_SPEED] = STROKE_SPEED_INSTANT;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_SPEED] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_STRESS)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive stress from stroke variation.
    */
   switch (jStroke) {
      case PANOSE_ANY:
         lpjMoreAttr[PAN_IND_LATIN_STRESS] = PANOSE_ANY;
         break;
      case STROKE_NOVARIATION:
         lpjMoreAttr[PAN_IND_LATIN_STRESS] = STROKE_STRESS_NONE;
         break;
      case STROKE_GRADDIAG:
         lpjMoreAttr[PAN_IND_LATIN_STRESS] = STROKE_STRESS_DIAG;
         break;
      case STROKE_GRADTRANS:
         lpjMoreAttr[PAN_IND_LATIN_STRESS] = STROKE_STRESS_TRANS;
         break;
      case STROKE_GRADVERT:
      case STROKE_RAPIDVERT:
      case STROKE_INSTANTVERT:
         lpjMoreAttr[PAN_IND_LATIN_STRESS] = STROKE_STRESS_VERT;
         break;
      case STROKE_GRADHORZ:
      case STROKE_RAPIDHORZ:
         lpjMoreAttr[PAN_IND_LATIN_STRESS] = STROKE_STRESS_HORZ;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_STRESS] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_ARMTYPE)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive arm type from arm style.
    */
   switch (jArmStyle) {
      case PANOSE_ANY:
         lpjMoreAttr[PAN_IND_LATIN_ARMTYPE] = PANOSE_ANY;
         break;
      case ARM_STRAIGHTHORZ:
      case ARM_STRAIGHTWEDGE:
      case ARM_STRAIGHTVERT:
      case ARM_STRAIGHTSGLSERIF:
      case ARM_STRAIGHTDBLSERIF:
         lpjMoreAttr[PAN_IND_LATIN_ARMTYPE] = ARM_TYPE_STRAIGHT;
         break;
      case ARM_NONSTRTHORZ:
      case ARM_NONSTRTWEDGE:
      case ARM_NONSTRTVERT:
      case ARM_NONSTRTSGLSERIF:
      case ARM_NONSTRTDBLSERIF:
         lpjMoreAttr[PAN_IND_LATIN_ARMTYPE] = ARM_TYPE_NONSTRT;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_ARMTYPE] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_CTERM)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive curve termination from arm style.
    */
   switch (jArmStyle) {
      case ARM_STRAIGHTHORZ:
      case ARM_NONSTRTHORZ:
         lpjMoreAttr[PAN_IND_LATIN_CTERM] = ARM_CTERM_HORZ;
         break;
      case ARM_STRAIGHTWEDGE:
      case ARM_NONSTRTWEDGE:
         lpjMoreAttr[PAN_IND_LATIN_CTERM] = ARM_CTERM_WEDGE;
         break;
      case ARM_STRAIGHTVERT:
      case ARM_NONSTRTVERT:
         lpjMoreAttr[PAN_IND_LATIN_CTERM] = ARM_CTERM_VERT;
         break;
      case ARM_STRAIGHTSGLSERIF:
      case ARM_NONSTRTSGLSERIF:
         lpjMoreAttr[PAN_IND_LATIN_CTERM] = ARM_CTERM_SGLSERIF;
         break;
      case ARM_STRAIGHTDBLSERIF:
      case ARM_NONSTRTDBLSERIF:
         lpjMoreAttr[PAN_IND_LATIN_CTERM] = ARM_CTERM_DBLSERIF;
         break;
      case PANOSE_ANY:
         switch (lpjMoreAttr[PAN_IND_LATIN_SERIF]) {
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            case SERIF_COVE:
            case SERIF_OBTUSECOVE:
            case SERIF_SQUARECOVE:
            case SERIF_OBTSQUARECOVE:
            case SERIF_SQUARE:
            case SERIF_THIN:
            case SERIF_BONE:
            case SERIF_EXAGGERATED:
            case SERIF_TRIANGLE:
            default:
               lpjMoreAttr[PAN_IND_LATIN_CTERM] = ARM_CTERM_SGLSERIF;
               break;
            case SERIF_NORMALSANS:
            case SERIF_OBTUSESANS:
            case SERIF_PERPSANS:
            case SERIF_FLARED:
            case SERIF_ROUNDED:
               lpjMoreAttr[PAN_IND_LATIN_CTERM] = ARM_CTERM_HORZ;
               break;
         }
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_CTERM] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_SLANT)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive slant from letterform.
    */
   switch (jLetterform) {
      case PANOSE_ANY:
         lpjMoreAttr[PAN_IND_LATIN_SLANT] = PANOSE_ANY;
         break;
      case LTRFORM_NORMCONTACT:
      case LTRFORM_NORMWEIGHTED:
      case LTRFORM_NORMBOXED:
      case LTRFORM_NORMFLATTENED:
      case LTRFORM_NORMROUNDED:
      case LTRFORM_NORMOFFCENTER:
      case LTRFORM_NORMSQUARE:
         lpjMoreAttr[PAN_IND_LATIN_SLANT] = LTRFM_SLANT_NORMAL;
         break;
      case LTRFORM_OBLQCONTACT:
      case LTRFORM_OBLQWEIGHTED:
      case LTRFORM_OBLQBOXED:
      case LTRFORM_OBLQFLATTENED:
      case LTRFORM_OBLQROUNDED:
      case LTRFORM_OBLQOFFCENTER:
      case LTRFORM_OBLQSQUARE:
         lpjMoreAttr[PAN_IND_LATIN_SLANT] = LTRFM_SLANT_OBLIQUE;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_SLANT] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_ROUND)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive roundness from letterform.
    */
   switch (jLetterform) {
      case PANOSE_ANY:
      case LTRFORM_NORMOFFCENTER:
      case LTRFORM_OBLQOFFCENTER:
      case LTRFORM_NORMCONTACT:
      case LTRFORM_OBLQCONTACT:
         lpjMoreAttr[PAN_IND_LATIN_ROUND] = LTRFM_ROUND_CONTACT;
         break;
      case LTRFORM_NORMWEIGHTED:
      case LTRFORM_OBLQWEIGHTED:
         lpjMoreAttr[PAN_IND_LATIN_ROUND] = LTRFM_ROUND_WEIGHTED;
         break;
      case LTRFORM_NORMBOXED:
      case LTRFORM_OBLQBOXED:
         lpjMoreAttr[PAN_IND_LATIN_ROUND] = LTRFM_ROUND_BOXED;
         break;
      case LTRFORM_NORMFLATTENED:
      case LTRFORM_OBLQFLATTENED:
         lpjMoreAttr[PAN_IND_LATIN_ROUND] = LTRFM_ROUND_FLATTENED;
         break;
      case LTRFORM_NORMROUNDED:
      case LTRFORM_OBLQROUNDED:
         lpjMoreAttr[PAN_IND_LATIN_ROUND] = LTRFM_ROUND_ROUNDED;
         break;
      case LTRFORM_NORMSQUARE:
      case LTRFORM_OBLQSQUARE:
         lpjMoreAttr[PAN_IND_LATIN_ROUND] = LTRFM_ROUND_SQUARE;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_ROUND] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_SYMMETRY)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive symmetry from letterform.
    */
   switch (jLetterform) {
      case PANOSE_ANY:
      case LTRFORM_NORMCONTACT:
      case LTRFORM_NORMWEIGHTED:
      case LTRFORM_NORMBOXED:
      case LTRFORM_NORMFLATTENED:
      case LTRFORM_NORMROUNDED:
      case LTRFORM_NORMSQUARE:
      case LTRFORM_OBLQCONTACT:
      case LTRFORM_OBLQWEIGHTED:
      case LTRFORM_OBLQBOXED:
      case LTRFORM_OBLQFLATTENED:
      case LTRFORM_OBLQROUNDED:
      case LTRFORM_OBLQSQUARE:
         lpjMoreAttr[PAN_IND_LATIN_SYMMETRY] = LTRFM_SYMMET_ONCENTER;
         break;
      case LTRFORM_NORMOFFCENTER:
      case LTRFORM_OBLQOFFCENTER:
         lpjMoreAttr[PAN_IND_LATIN_SYMMETRY] = LTRFM_SYMMET_OFFCENTER;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_SYMMETRY] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_MIDLNPOS)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive midline position from midline.
    */
   switch (jMidline) {
      case PANOSE_ANY:
         lpjMoreAttr[PAN_IND_LATIN_MIDLNPOS] = PANOSE_ANY;
         break;
      case MIDLINE_STDTRIMMED:
      case MIDLINE_STDPOINTED:
      case MIDLINE_STDSERIFED:
         lpjMoreAttr[PAN_IND_LATIN_MIDLNPOS] = MIDLN_POS_STANDARD;
         break;
      case MIDLINE_HIGHTRIMMED:
      case MIDLINE_HIGHPOINTED:
      case MIDLINE_HIGHSERIFED:
         lpjMoreAttr[PAN_IND_LATIN_MIDLNPOS] = MIDLN_POS_HIGH;
         break;
      case MIDLINE_CONSTTRIMMED:
      case MIDLINE_CONSTPOINTED:
      case MIDLINE_CONSTSERIFED:
         lpjMoreAttr[PAN_IND_LATIN_MIDLNPOS] = MIDLN_POS_CONSTANT;
         break;
      case MIDLINE_LOWTRIMMED:
      case MIDLINE_LOWPOINTED:
      case MIDLINE_LOWSERIFED:
         lpjMoreAttr[PAN_IND_LATIN_MIDLNPOS] = MIDLN_POS_LOW;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_MIDLNPOS] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_APEX)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive apex treatment from midline.
    */
   switch (jMidline) {
      case PANOSE_ANY:
      case MIDLINE_STDTRIMMED:
      case MIDLINE_HIGHTRIMMED:
      case MIDLINE_CONSTTRIMMED:
      case MIDLINE_LOWTRIMMED:
         lpjMoreAttr[PAN_IND_LATIN_APEX] = MIDLN_APEX_TRIMMED;
         break;
      case MIDLINE_STDPOINTED:
      case MIDLINE_HIGHPOINTED:
      case MIDLINE_CONSTPOINTED:
      case MIDLINE_LOWPOINTED:
         lpjMoreAttr[PAN_IND_LATIN_APEX] = MIDLN_APEX_POINTED;
         break;
      case MIDLINE_STDSERIFED:
      case MIDLINE_HIGHSERIFED:
      case MIDLINE_CONSTSERIFED:
      case MIDLINE_LOWSERIFED:
         lpjMoreAttr[PAN_IND_LATIN_APEX] = MIDLN_APEX_SERIFED;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_APEX] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_XHEIGHT)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive x-height height from x-height.
    */
   switch (jXHeight) {
      case PANOSE_ANY:
         lpjMoreAttr[PAN_IND_LATIN_XHEIGHT] = PANOSE_ANY;
      case XHEIGHT_CONSTSMALL:
      case XHEIGHT_DUCKSMALL:
         lpjMoreAttr[PAN_IND_LATIN_XHEIGHT] = XHT_HEIGHT_SMALL;
         break;
      case XHEIGHT_CONSTSTD:
      case XHEIGHT_DUCKSTD:
         lpjMoreAttr[PAN_IND_LATIN_XHEIGHT] = XHT_HEIGHT_STANDARD;
         break;
      case XHEIGHT_CONSTLARGE:
      case XHEIGHT_DUCKLARGE:
         lpjMoreAttr[PAN_IND_LATIN_XHEIGHT] = XHT_HEIGHT_LARGE;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_XHEIGHT] = PANOSE_NOFIT;
         break;
   }

   if (uiMaxMore <= PAN_IND_LATIN_DIACRIT)
      return (-(int)lpPanThis->uiNumAttr);

   /* Derive diacritical adjustment from x-height.
    */
   switch (jXHeight) {
      case PANOSE_ANY:
      case XHEIGHT_CONSTSMALL:
      case XHEIGHT_CONSTSTD:
      case XHEIGHT_CONSTLARGE:
         lpjMoreAttr[PAN_IND_LATIN_DIACRIT] = XHT_DIACRIT_CONSTANT;
         break;
      case XHEIGHT_DUCKSMALL:
      case XHEIGHT_DUCKSTD:
      case XHEIGHT_DUCKLARGE:
         lpjMoreAttr[PAN_IND_LATIN_DIACRIT] = XHT_DIACRIT_DUCKING;
         break;
      case PANOSE_NOFIT:
      default:
         lpjMoreAttr[PAN_IND_LATIN_DIACRIT] = PANOSE_NOFIT;
         break;
   }
   return (lpPanThis->uiNumAttr);
}

/***************************************************************************
 * FUNCTION: iELSECvtLatTDExpTo10
 *
 * PURPOSE:  Derive a Latin Text and Display 10-digit PANOSE number from
 *           an Expanded PANOSE number.
 *
 * RETURNS:  The function returns the count of PANOSE digits filled in,
 *           zero if there is an error.  The count is negative if less
 *           than the standard 10 digits were converted.
 *
 *           To be precise, the following PANOSE digits have been filled
 *           in for the following return values:
 *
 *             0 = none
 *            -4 = family, weight, proportion, and contrast
 *            -5 = serif
 *            -6 = stroke variation
 *            -7 = arm style
 *            -8 = letterform
 *            -9 = midline
 *            10 = x-height
 *
 *           When the return value is between -4 and -9, the digits that
 *           have not recieved a value will contain PANOSE_NOFIT as the
 *           caller already initialized lpuiPAN10 with that.
 ***************************************************************************/
ELSELOCAL INT ELSENEARPROC ELSEPASCAL iELSECvtLatTDExpTo10 (
   LPEBYTE lpjPAN10,
   LPPANOSE_NUM_MEM lpPanThis )
{
   UINT uiNumFilled = 0;
   UINT uiNumAttr = lpPanThis->uiNumAttr;
   BYTE ELSEFARPTR *lpjMoreAttr;

   /* Latin Text and Display.
    */
   lpjPAN10[PANOSE_IND_FAMILY] = FAMILY_LATTEXTANDDISP;

   /* Transfer weight.
    */
   if (lpPanThis->jWeight <= WEIGHT__LAST)
      lpjPAN10[PANOSE_IND_WEIGHT] = lpPanThis->jWeight;

   /* Combine distortion, monospace, and ratio into proportion.
    */
   if (lpPanThis->jMonospace != PROP_MONOSPACE_YES) {
      switch (lpPanThis->jDistortion) {
         case PANOSE_ANY:
            lpjPAN10[PANOSE_IND_PROPORTION] = PANOSE_ANY;
            break;
         case PROP_DISTORT_VERYCOND:
            lpjPAN10[PANOSE_IND_PROPORTION] = PROPORTION_VERYCOND;
            break;
         case PROP_DISTORT_CONDENSED:
            lpjPAN10[PANOSE_IND_PROPORTION] = PROPORTION_CONDENSED;
            break;
         case PROP_DISTORT_NORMAL:
            if (uiNumAttr > PAN_IND_LATIN_RATIO) {
               switch (lpPanThis->ajMoreAttr[PAN_IND_LATIN_RATIO]) {
                  case PROP_RATIO_OLDSTYLE:
                     lpjPAN10[PANOSE_IND_PROPORTION] = PROPORTION_OLDSTYLE;
                     break;
                  case PROP_RATIO_MODERN:
                     lpjPAN10[PANOSE_IND_PROPORTION] = PROPORTION_MODERN;
                     break;
                  case PROP_RATIO_EVENWIDTH:
                     lpjPAN10[PANOSE_IND_PROPORTION] = PROPORTION_EVENWIDTH;
                     break;
                  case PANOSE_ANY:
                  case PANOSE_NOFIT:
                  default:
                     lpjPAN10[PANOSE_IND_PROPORTION] = PANOSE_NOFIT;
                     break;
               }
            } else {
               lpjPAN10[PANOSE_IND_PROPORTION] = PANOSE_NOFIT;
            }
            break;
         case PROP_DISTORT_EXPANDED:
            lpjPAN10[PANOSE_IND_PROPORTION] = PROPORTION_EXPANDED;
            break;
         case PROP_DISTORT_VERYEXP:
            lpjPAN10[PANOSE_IND_PROPORTION] = PROPORTION_VERYEXP;
            break;
         case PANOSE_NOFIT:
         default:
            lpjPAN10[PANOSE_IND_PROPORTION] = PANOSE_NOFIT;
            break;
      }
   } else {
      lpjPAN10[PANOSE_IND_PROPORTION] = PROPORTION_MONOSPACE;
   }

   /* Transfer contrast.
    */
   if (lpPanThis->jContrast <= CONTRAST__LAST)
      lpjPAN10[PANOSE_IND_CONTRAST] = lpPanThis->jContrast;

   /* At this point, four of the 10 digits have been filled in,
    * so set the count of initialized digits to 4.
    */
   uiNumFilled = PAN_COUNT_SHARED;

   /* For the rest of the digits, make sure that the variables
    * exist in the 'more' attributes array at the end of the
    * PANOSE number rec.
    *
    * We expect the caller has pre-initialized all the digits
    * to PANOSE_NOFIT, which will be their value if we stop
    * early.
    */
   if (uiNumAttr <= PAN_IND_LATIN_SERIF)
      return (-(int)uiNumFilled);
   ++uiNumFilled;

   /* Use direct pointer.
    */
   lpjMoreAttr = lpPanThis->ajMoreAttr;

   /* Transfer serif.
    */
   if (lpjMoreAttr[PAN_IND_LATIN_SERIF] <= SERIF__LAST)
      lpjPAN10[PANOSE_IND_SERIF] = lpjMoreAttr[PAN_IND_LATIN_SERIF];

   if (uiNumAttr <= PAN_IND_LATIN_SPEED || uiNumAttr <= PAN_IND_LATIN_STRESS)
      return (-(int)uiNumFilled);
   ++uiNumFilled;

   /* Combine speed and stress into stroke variation.
    */
   switch (lpjMoreAttr[PAN_IND_LATIN_STRESS]) {
      case PANOSE_ANY:
         lpjPAN10[PANOSE_IND_STROKE] = PANOSE_ANY;
         break;
      case STROKE_STRESS_NONE:
         lpjPAN10[PANOSE_IND_STROKE] = STROKE_NOVARIATION;
         break;
      case STROKE_STRESS_DIAG:
         switch (lpjMoreAttr[PAN_IND_LATIN_SPEED]) {
            case STROKE_SPEED_GRADUAL:
               lpjPAN10[PANOSE_IND_STROKE] = STROKE_GRADDIAG;
               break;
            case STROKE_SPEED_NONE:
            case STROKE_SPEED_RAPID:
            case STROKE_SPEED_INSTANT:
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_STROKE] = PANOSE_NOFIT;
               break;
         }
         break;
      case STROKE_STRESS_TRANS:
         switch (lpjMoreAttr[PAN_IND_LATIN_SPEED]) {
            case STROKE_SPEED_GRADUAL:
               lpjPAN10[PANOSE_IND_STROKE] = STROKE_GRADTRANS;
               break;
            case STROKE_SPEED_NONE:
            case STROKE_SPEED_RAPID:
            case STROKE_SPEED_INSTANT:
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_STROKE] = PANOSE_NOFIT;
               break;
         }
         break;
      case STROKE_STRESS_VERT:
         switch (lpjMoreAttr[PAN_IND_LATIN_SPEED]) {
            case STROKE_SPEED_GRADUAL:
               lpjPAN10[PANOSE_IND_STROKE] = STROKE_GRADVERT;
               break;
            case STROKE_SPEED_RAPID:
               lpjPAN10[PANOSE_IND_STROKE] = STROKE_RAPIDVERT;
               break;
            case STROKE_SPEED_INSTANT:
               lpjPAN10[PANOSE_IND_STROKE] = STROKE_INSTANTVERT;
               break;
            case STROKE_SPEED_NONE:
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_STROKE] = PANOSE_NOFIT;
               break;
         }
         break;
      case STROKE_STRESS_HORZ:
         switch (lpjMoreAttr[PAN_IND_LATIN_SPEED]) {
            case STROKE_SPEED_GRADUAL:
               lpjPAN10[PANOSE_IND_STROKE] = STROKE_GRADHORZ;
               break;
            case STROKE_SPEED_RAPID:
               lpjPAN10[PANOSE_IND_STROKE] = STROKE_RAPIDHORZ;
               break;
            case STROKE_SPEED_NONE:
            case STROKE_SPEED_INSTANT:
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_STROKE] = PANOSE_NOFIT;
               break;
            }
         break;
      case PANOSE_NOFIT:
      default:
         lpjPAN10[PANOSE_IND_STROKE] = PANOSE_NOFIT;
         break;
   }

   if (uiNumAttr <= PAN_IND_LATIN_ARMTYPE || uiNumAttr <= PAN_IND_LATIN_CTERM)
      return (-(int)uiNumFilled);
   ++uiNumFilled;

   /* Combine arm type and curve termination into arm style.
    */
   switch (lpjMoreAttr[PAN_IND_LATIN_ARMTYPE]) {
      case PANOSE_ANY:
         lpjPAN10[PANOSE_IND_ARMSTYLE] = PANOSE_ANY;
         break;
      case ARM_TYPE_NONSTRT:
         switch (lpjMoreAttr[PAN_IND_LATIN_CTERM]) {
            case ARM_CTERM_HORZ:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = ARM_NONSTRTHORZ;
               break;
            case ARM_CTERM_WEDGE:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = ARM_NONSTRTWEDGE;
               break;
            case ARM_CTERM_VERT:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = ARM_NONSTRTVERT;
               break;
            case ARM_CTERM_SGLSERIF:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = ARM_NONSTRTSGLSERIF;
               break;
            case ARM_CTERM_DBLSERIF:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = ARM_NONSTRTDBLSERIF;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = PANOSE_NOFIT;
               break;
         }
         break;
      case ARM_TYPE_STRAIGHT:
         switch (lpjMoreAttr[PAN_IND_LATIN_CTERM]) {
            case ARM_CTERM_HORZ:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = ARM_STRAIGHTHORZ;
               break;
            case ARM_CTERM_WEDGE:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = ARM_STRAIGHTWEDGE;
               break;
            case ARM_CTERM_VERT:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = ARM_STRAIGHTVERT;
               break;
            case ARM_CTERM_SGLSERIF:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = ARM_STRAIGHTSGLSERIF;
               break;
            case ARM_CTERM_DBLSERIF:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = ARM_STRAIGHTDBLSERIF;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_ARMSTYLE] = PANOSE_NOFIT;
               break;
         }
         break;
      case PANOSE_NOFIT:
      default:
         lpjPAN10[PANOSE_IND_ARMSTYLE] = PANOSE_NOFIT;
         break;
   }

   if (uiNumAttr <= PAN_IND_LATIN_SLANT || uiNumAttr <= PAN_IND_LATIN_ROUND)
      return (-(int)uiNumFilled);
   ++uiNumFilled;

   /* Combine slant, roundness, and symmetry into letterform.
    */
   switch (lpjMoreAttr[PAN_IND_LATIN_SLANT]) {
      case PANOSE_ANY:
         lpjPAN10[PANOSE_IND_LTRFORM] = PANOSE_ANY;
         break;
      case LTRFM_SLANT_NORMAL:
         if (uiNumAttr > PAN_IND_LATIN_SYMMETRY &&
            lpjMoreAttr[PAN_IND_LATIN_SYMMETRY] == LTRFM_SYMMET_OFFCENTER) {
            lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_NORMOFFCENTER;
         } else switch (lpjMoreAttr[PAN_IND_LATIN_ROUND]) {
            case LTRFM_ROUND_CONTACT:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_NORMCONTACT;
               break;
            case LTRFM_ROUND_WEIGHTED:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_NORMWEIGHTED;
               break;
            case LTRFM_ROUND_BOXED:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_NORMBOXED;
               break;
            case LTRFM_ROUND_FLATTENED:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_NORMFLATTENED;
               break;
            case LTRFM_ROUND_ROUNDED:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_NORMROUNDED;
               break;
            case LTRFM_ROUND_SQUARE:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_NORMSQUARE;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_LTRFORM] = PANOSE_NOFIT;
               break;
         }
         break;
      case LTRFM_SLANT_OBLIQUE:
         if (uiNumAttr > PAN_IND_LATIN_SYMMETRY &&
            lpjMoreAttr[PAN_IND_LATIN_SYMMETRY] == LTRFM_SYMMET_OFFCENTER) {
            lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_OBLQOFFCENTER;
         } else switch (lpjMoreAttr[PAN_IND_LATIN_ROUND]) {
            case LTRFM_ROUND_CONTACT:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_OBLQCONTACT;
               break;
            case LTRFM_ROUND_WEIGHTED:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_OBLQWEIGHTED;
               break;
            case LTRFM_ROUND_BOXED:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_OBLQBOXED;
               break;
            case LTRFM_ROUND_FLATTENED:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_OBLQFLATTENED;
               break;
            case LTRFM_ROUND_ROUNDED:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_OBLQROUNDED;
               break;
            case LTRFM_ROUND_SQUARE:
               lpjPAN10[PANOSE_IND_LTRFORM] = LTRFORM_OBLQSQUARE;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_LTRFORM] = PANOSE_NOFIT;
               break;
         }
         break;
      case PANOSE_NOFIT:
      default:
         lpjPAN10[PANOSE_IND_LTRFORM] = PANOSE_NOFIT;
         break;
   }

   if (uiNumAttr <= PAN_IND_LATIN_MIDLNPOS || uiNumAttr <= PAN_IND_LATIN_APEX)
      return (-(int)uiNumFilled);
   ++uiNumFilled;

   /* Combine midline position and apex treatment into midline.
    */
   switch (lpjMoreAttr[PAN_IND_LATIN_MIDLNPOS]) {
      case PANOSE_ANY:
         lpjPAN10[PANOSE_IND_MIDLINE] = PANOSE_ANY;
         break;
      case MIDLN_POS_STANDARD:
         switch (lpjMoreAttr[PAN_IND_LATIN_APEX]) {
            case MIDLN_APEX_TRIMMED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_STDTRIMMED;
               break;
            case MIDLN_APEX_POINTED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_STDPOINTED;
               break;
            case MIDLN_APEX_SERIFED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_STDSERIFED;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_MIDLINE] = PANOSE_NOFIT;
               break;
         }
         break;
      case MIDLN_POS_HIGH:
         switch (lpjMoreAttr[PAN_IND_LATIN_APEX]) {
            case MIDLN_APEX_TRIMMED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_HIGHTRIMMED;
               break;
            case MIDLN_APEX_POINTED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_HIGHPOINTED;
               break;
            case MIDLN_APEX_SERIFED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_HIGHSERIFED;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_MIDLINE] = PANOSE_NOFIT;
               break;
         }
         break;
      case MIDLN_POS_CONSTANT:
         switch (lpjMoreAttr[PAN_IND_LATIN_APEX]) {
            case MIDLN_APEX_TRIMMED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_CONSTTRIMMED;
               break;
            case MIDLN_APEX_POINTED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_CONSTPOINTED;
               break;
            case MIDLN_APEX_SERIFED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_CONSTSERIFED;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_MIDLINE] = PANOSE_NOFIT;
               break;
         }
         break;
      case MIDLN_POS_LOW:
         switch (lpjMoreAttr[PAN_IND_LATIN_APEX]) {
            case MIDLN_APEX_TRIMMED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_LOWTRIMMED;
               break;
            case MIDLN_APEX_POINTED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_LOWPOINTED;
               break;
            case MIDLN_APEX_SERIFED:
               lpjPAN10[PANOSE_IND_MIDLINE] = MIDLINE_LOWSERIFED;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_MIDLINE] = PANOSE_NOFIT;
               break;
         }
         break;
      case PANOSE_NOFIT:
      default:
         lpjPAN10[PANOSE_IND_MIDLINE] = PANOSE_NOFIT;
         break;
   }

   if (uiNumAttr <= PAN_IND_LATIN_XHEIGHT || uiNumAttr <= PAN_IND_LATIN_DIACRIT)
      return (-(int)uiNumFilled);
   ++uiNumFilled;

   /* Combine x-height height and diacritical adjustment into x-height.
    */
   switch (lpjMoreAttr[PAN_IND_LATIN_XHEIGHT]) {
      case PANOSE_ANY:
         lpjPAN10[PANOSE_IND_XHEIGHT] = PANOSE_ANY;
         break;
      case XHT_HEIGHT_SMALL:
         switch (lpjMoreAttr[PAN_IND_LATIN_DIACRIT]) {
            case XHT_DIACRIT_CONSTANT:
               lpjPAN10[PANOSE_IND_XHEIGHT] = XHEIGHT_CONSTSMALL;
               break;
            case XHT_DIACRIT_DUCKING:
               lpjPAN10[PANOSE_IND_XHEIGHT] = XHEIGHT_DUCKSMALL;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_XHEIGHT] = PANOSE_NOFIT;
               break;
         }
         break;
      case XHT_HEIGHT_STANDARD:
         switch (lpjMoreAttr[PAN_IND_LATIN_DIACRIT]) {
            case XHT_DIACRIT_CONSTANT:
               lpjPAN10[PANOSE_IND_XHEIGHT] = XHEIGHT_CONSTSTD;
               break;
            case XHT_DIACRIT_DUCKING:
               lpjPAN10[PANOSE_IND_XHEIGHT] = XHEIGHT_DUCKSTD;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_XHEIGHT] = PANOSE_NOFIT;
               break;
         }
         break;
      case XHT_HEIGHT_LARGE:
         switch (lpjMoreAttr[PAN_IND_LATIN_DIACRIT]) {
            case XHT_DIACRIT_CONSTANT:
               lpjPAN10[PANOSE_IND_XHEIGHT] = XHEIGHT_CONSTLARGE;
               break;
            case XHT_DIACRIT_DUCKING:
               lpjPAN10[PANOSE_IND_XHEIGHT] = XHEIGHT_DUCKLARGE;
               break;
            case PANOSE_ANY:
            case PANOSE_NOFIT:
            default:
               lpjPAN10[PANOSE_IND_XHEIGHT] = PANOSE_NOFIT;
               break;
         }
         break;
      case PANOSE_NOFIT:
      default:
         lpjPAN10[PANOSE_IND_XHEIGHT] = PANOSE_NOFIT;
         break;
   }

   /* Return 10 to indicate all PANOSE numbers were filled in.
    */
   return (uiNumFilled);
}

/***************************************************************************
 * Revision log:
 ***************************************************************************/
/*
 * $lgb$
 * 1.0     3-Feb-92  msd Version 1.00 mapper placed under version control.
 * $lge$
 */

