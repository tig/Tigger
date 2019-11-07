/************************************************************************
 *
 *    Copyright (c) 1991 Microsoft Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  wsscanf.c
 *
 *      Author:  baw
 *
 *
 *     Remarks:  sscanf() for Windows' DLLs
 *
 *               This routine uses FAR pointers to ALL arguments.
 *               It does not support floating point.
 *               Based on Microsoft C 6.00a runtime source code.
 *
 *   Revisions:  
 *     01.00.000  2/23/81 baw   Initial version, initial build
 *
 *
 ************************************************************************/
#include <windows.h>

#include <ctype.h>
#include <memory.h>

#include "wstdarg.h"
#include "wsscanf.h"

// global definitons (scope: wsscanf.c)

#ifdef ALLOW_RANGE
   char szSBrackSet[] = " \t-\r]" ;  // use range-style list
#else
   char szSBrackSet[] = "\t\n\v\f\r]" ;  // chars defined by isspace
#endif

char szCBrackSet[] = "]" ;

/************************************************************************
 *  int FAR _cdecl wsscanf( LPSTR lpBuffer, LPSTR lpFormat,
 *                          LPSTR lpParms, ... )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ************************************************************************/

int FAR _cdecl wsscanf( LPSTR lpBuffer, LPSTR lpFormat, LPSTR lpParms, ... )
{
   BYTE          bCoerceShort, bDoneFlag, bLongOne, bMatch, bNegative,
                 bReject, bSuppress ;
   BYTE FAR *    lpPtrScan ;
   DWORD         dwNumber ;
   LPSTR         lpPtrFormat, lpPtrBuffer ;
   LPVOID FAR *  lpArgList;
   LPVOID        lpPointer, lpStart ;
   WORD          wHoldSeg ;
   char          ch, chCom, chLast, chPrev, chRange, szTable[ ASCII ] ;
   int           nCount, nCharCount, nWidth, nWidthSet, nStarted ;


   lpArgList = (LPVOID FAR *) &lpParms ;

   nCount = nCharCount = 0 ;
   bMatch = FALSE ;
   lpPtrFormat = lpFormat ;
   lpPtrBuffer = lpBuffer ;

   while (*lpPtrFormat)
   {
      if (isspace( *lpPtrFormat ))
      {
         UN_INC( EAT_WHITE() ) ;  // put first non-space char back
         while (isspace(*++lpPtrFormat)) ;
      }
      if ('%' == *lpPtrFormat)
      {
         bCoerceShort = bDoneFlag = bLongOne = bNegative =
         bReject = bSuppress = FALSE ;
         nWidth = nWidthSet = nStarted = 0 ;
         dwNumber = 0 ;
         chPrev = 0 ;
   
         while (!bDoneFlag)
         {
            if (isdigit( chCom = *++lpPtrFormat ))
            {
               ++nWidthSet ;
               nWidth = MUL10( nWidth ) + (chCom - '0') ;
            }
            else
               switch (chCom)
               {
                  case 'F':
                  case 'N':
                     // FAR is only type of pointer in DLLs
                     break ;
   
                  case 'h':
                     ++bCoerceShort ;
                     break ;
            
                  case 'l':
                     ++bLongOne ;
                     break ;
   
                  case '*':
                     ++bSuppress ;
                     break ;
   
                  default:
                     ++bDoneFlag ;
                     break ;
               }
         }
   
         if (!bSuppress)
            /* ALL pointers are pushed as FAR */
            lpPointer = (LPVOID) *lpArgList++ ;
   
         bDoneFlag = FALSE ;
   
         if ('n' != (chCom = (char)(*lpPtrFormat | ('a' - 'A'))))
         {
            if ('c' != chCom && LEFT_BRACKET != chCom)
               ch = EAT_WHITE() ;
            else
               ch = INC() ;
         }
   
         if (!nWidthSet || nWidth)
         {
            switch (chCom)
            {
               case 'c':
                  if (!nWidthSet)
                  {
                     ++nWidthSet ;
                     ++nWidth ;
                  }
                  lpPtrScan = (LPSTR) szCBrackSet  ;
                  --bReject ;
                  goto ScanIt2 ;
   
               case 's':
                  lpPtrScan = (LPSTR) szSBrackSet ;
                  --bReject ;
                  goto ScanIt2 ;
   
               case LEFT_BRACKET:
                  lpPtrScan = (LPSTR) (++lpPtrFormat) ;
                  if ('^' == *lpPtrScan)
                  {
                     ++lpPtrScan ;
                     --bReject ;
                  }
ScanIt2:
                  _fmemset( szTable, 0, ASCII ) ;
   
#ifdef ALLOW_RANGE
                  if (LEFT_BRACKET == chCom)
                     if (']' == *lpPtrScan)
                     {
                        chPrev = ']' ;
                        ++lpPtrScan ;
                        szTable[ ']' >> 3 ] = 1 << (']' & 7 ) ;
                     }
                  while (']' != *lpPtrScan)
                  {
                     chRange = *lpPtrScan++ ;
                     if ('-' != chRange || !chPrev || ']' == *lpPtrScan)
                        szTable[(chPrev = chRange) >> 3] |= 1 << (chRange & 7) ;
                     else
                     {
                        // handle a-z type set
                        chRange = *lpPtrScan++ ;
                        if (chPrev < chRange)
                           chLast = chRange ;
                        else
                        {
                           chLast = chPrev ;
                           chPrev = chRange ;
                        }
                        for (chRange = chPrev; chRange <= chLast; ++chRange)
                           szTable[chRange >> 3] |= 1 << (chRange & 7) ;
                        chPrev = 0 ;
                     }
                  }
#else
                  if (LEFT_BRACKET == chCom)
                     if (']'  == *lpPtrScan)
                     {
                        ++lpPtrScan;
                        szTable[(chPrev = ']') >> 3] |= 1 << (']' & 7) ;
                     }
                  while (']' != *lpPtrScan)
                     szTable[ *lpPtrScan >> 3] |= 1 << (*lpPtrScan & 7) ;
#endif
                  if (!*lpPtrScan)
                     goto ErrorReturn ;
         
                  if (LEFT_BRACKET == chCom)
                     lpPtrFormat = lpPtrScan ;
            
                  lpStart = lpPointer ;
                  while ((!nWidthSet || nWidth--) &&
                         ((szTable[ ch >> 3 ] ^ bReject) & (1 << (ch & 7))))
                  {
                     if (!bSuppress)
                     {
                        *(LPSTR)lpPointer = (char) ch ;
                        ++(LPSTR)lpPointer ;
                     }
                     else
                        // just indicate a match
                        ++(LPSTR)lpStart ;
                     ch = INC() ;
                  }
                  UN_INC( ch )  ;

                  // make sure something has been match and, if assignment
                  // is not suppressed, null-terminate output string if
                  // chCom != c

                  if (lpStart != lpPointer)
                  {
                     if (!bSuppress)
                     {
                        nCount++ ;
                        if ('c' != chCom)
                           // NULL terminate strings
                           *(LPSTR) lpPointer = NULL ;
                     }
                  }
                  else
                     goto ErrorReturn ;

                  break ;

               case 'i':
                  chCom = 'd' ;  // use 'd' as default
         
               case 'x':
                  if ('-' == ch)
                  {
                     ++bNegative ;
                     goto XIncWidth ;
                  }
                  else if ('+' == ch)
                  {
XIncWidth:
                     if (!--nWidth && nWidthSet)
                        ++bDoneFlag ;
                     else
                        ch = INC() ;
                  }
                  if ('0' == ch)
                  {
                     if ('x' == ((char) (ch = INC())) || 'X' == (char) ch)
                     {
                        ch = INC() ;
                        chCom = 'x' ;
                     }
                     else
                     {
                        // scanning a hex number that starts with 0
                        // push back the character currently in ch
                        // and restore the 0
   
                        UN_INC( ch ) ;
                        ch = '0' ;
                     }
                  }
                  goto GetNum ;
   
               case 'p':
                  // ALL pointers are FAR
                  if (!bCoerceShort)
                  {
                     ++bLongOne ;
                     chCom = 'F' ;  // indicates FAR
                  }
   
               case 'o':
               case 'u':
               case 'd':
                  if ('-' == ch)
                  {
                     ++bNegative ;
                     goto dIncWidth ;
                  }
                  else if ('+' == ch)
                  {
dIncWidth:
                     if (!--nWidth && nWidthSet)
                        ++bDoneFlag ;
                     else
                        ch = INC() ;
                  }
GetNum:
                  while (!bDoneFlag)
                  {
                     if ('x' == chCom || 'p' == chCom || 'F' == chCom)
                     {
                        if (isxdigit(ch))
                        {
                           dwNumber = (dwNumber << 4) ;
                           ch = HEXTODEC( ch ) ;
                        }
                        else if ('F' == chCom)
                        {
                           if (nStarted)
                           {
                              if (':' == ch)
                              {
                                 wHoldSeg = LOWORD( dwNumber ) ;
                                 dwNumber = 0 ;
                                 nStarted = -1 ;
                                 chCom = 'p' ; // switch to offset
                                 ch = '0' ; // don't add ':'
                              }
                              else
                              {
                                 nStarted = 0 ;
                                 ++bDoneFlag ;
                              }
                           }
                           else
                              ++bDoneFlag ;
                        }
                        else
                           ++bDoneFlag ;
                     }
                     else if (isdigit( ch ))
                     {
                        if ('o' == chCom)
                        {
                           if ('8' > ch)
                              dwNumber = (dwNumber << 3) ;
                           else
                              ++bDoneFlag ;
                        }
                        else
                           // 'd' == chCom
                           dwNumber = MUL10( dwNumber ) ;
                     } 
                     else
                        ++bDoneFlag ;

                     if (!bDoneFlag)
                     {
                        ++nStarted ;
                        dwNumber += ch - '0' ;
                        if (nWidthSet && !--nWidth)
                           ++bDoneFlag ;
                        else
                           ch = INC() ;
                     }
                     else
                        UN_INC( ch ) ;

                  } // end of while
   
                  if ('p' == chCom && bLongOne)
                     dwNumber = (dwNumber & (DWORD) 0x0000FFFF) |
                                 ((DWORD) wHoldSeg) << 16 ;
                  if (bNegative)
                     dwNumber = -dwNumber ;
                  if ('F' == chCom)
                     nStarted = 0 ;
                  if (nStarted)
                  {
                     if (!bSuppress)
                     {
                        ++nCount ;
AssignNum:
                        if (bLongOne)
                           *(DWORD FAR *)lpPointer = dwNumber ;
                        else
                           *(WORD FAR *)lpPointer = LOWORD( dwNumber ) ;
                     }
                  }
                  else
                     goto ErrorReturn ;
   
                  break ;
   
               case 'n':
                  dwNumber = nCharCount ;
                  goto AssignNum ;
   
               default:
                  if ((char)*lpPtrFormat != ch)
                  {
                     UN_INC( ch ) ;
                     goto ErrorReturn ;
                  }
                  else
                     bMatch-- ;
   
                  if (!bSuppress)
                     // ALL pointers are FAR pointers
                     --lpArgList ;
   
            } // end of switch

            bMatch++ ;

         } // end of while
         else
         {
            UN_INC( ch ) ;
            goto ErrorReturn ;
         }
         ++lpPtrFormat ;
      }
      else
      {
         // ('%' != *lpPtrFormat)
         if ((char) *lpPtrFormat++ != (ch = INC()))
         {
            UN_INC( ch ) ;
            goto ErrorReturn ;
         }
      }

   } // end while (*lpPtrFormat)

ErrorReturn:
   return nCount ;

} /* end of wsscanf() */

/************************************************************************
 *  char NEAR HexToDec( char chChar )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ************************************************************************/

char NEAR HexToDec( char chChar )
{
   return (isdigit( chChar ) ? chChar : (char) ((chChar & ~('a' - 'A')) - 'A' + 10 + '0')) ;

} /* end of HexToDec() */

/************************************************************************
 *  char NEAR Inc( LPSTR FAR *lpPtrBuffer )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ************************************************************************/

char NEAR Inc( LPSTR FAR *lpPtrBuffer )
{
   char  ch ;

   ch = *((LPSTR) *lpPtrBuffer) ;
   ((LPSTR) (*lpPtrBuffer))++ ;

   return ( ch ) ;

} /* end of Inc() */

/************************************************************************
 *  VOID NEAR UnInc( char chChar, LPSTR FAR *lpPtrBuffer )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ************************************************************************/

VOID NEAR UnInc( char chChar, LPSTR FAR *lpPtrBuffer )
{
   ((LPSTR) (*lpPtrBuffer))-- ;
   *((LPSTR) *lpPtrBuffer) = chChar ;
   
} /* end of UnInc() */

/************************************************************************
 *  char NEAR WhiteOut( int *pnCounter, LPSTR FAR *lpPtrBuffer )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ************************************************************************/

char NEAR WhiteOut( int FAR *lpnCounter, LPSTR FAR *lpPtrBuffer )
{
   char  ch ;

   while (isspace(ch = (++*lpnCounter, Inc( lpPtrBuffer )))) ;
   return ( ch ) ;

} /* end of WhiteOut() */

/************************************************************************
 * End of File: wsscanf.c
 ************************************************************************/

