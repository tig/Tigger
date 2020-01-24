/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Convert ASCII Data into Tokens Include (TOKENS.H)                */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: This module is responsible for converting ascii strings into     */
/*           tokens that are more easily digested by the rest of the program. */
/*           To try and save alittle space, the keyword tokens must be first  */
/*           in the number sequence, thus the position of the keywords in the */
/*           internal table is the same value as in the TOKENS.H file.  Be    */
/*           very in keeping these two files in sync.                         */
/*									      */
/******************************************************************************/


/******************************************************************************/
/*									      */
/*  Global Definitions							      */
/*									      */
/******************************************************************************/

#define TKN_BREAK           1
#define TKN_DIAL            2
#define TKN_DTR             3
#define TKN_END             4
#define TKN_GOTO            5
#define TKN_IF              6
#define TKN_INPUT           7
#define TKN_NUMBER          8
#define TKN_NODE            9
#define TKN_PREFIX         10
#define TKN_PRINT          11
#define TKN_SLEEP          12
#define TKN_SPEED          13
#define TKN_THEN           14
#define TKN_TIMEOUT        15

#define TKN_COMMA          16
#define TKN_EQUAL          17
#define TKN_PLUS           18

#define TKN_LABEL          19
#define TKN_IDENT          20
#define TKN_NUMCON         21
#define TKN_STRCON         22
#define TKN_ERROR          23
#define TKN_EOL            24
#define TKN_SEMI           25


/******************************************************************************/
/*									      */
/*  Output Global Variables                                                   */
/*									      */
/******************************************************************************/

  extern USHORT usTokenCurrent;     /* Value of the last token retrieved    */
  extern USHORT usTokenLast;        /* Value of the previous to above token */


/******************************************************************************/
/*									      */
/*  External Procedure's Prototypes                                           */
/*									      */
/******************************************************************************/

  extern SHORT TokenAsciiToTokens(PSTR, BYTE *, WORD);
  extern PBYTE TokenGetNext(PUSHORT, PVOID);
  extern PBYTE TokenSetLine(PBYTE);
