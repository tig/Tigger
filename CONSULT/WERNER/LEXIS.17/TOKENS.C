/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Convert ASCII Data into Tokens Module (TOKENS.C)                 */
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

#include <windows.h>
#include <ctype.h>
#include "lexis.h"
#include "tokens.h"


/******************************************************************************/
/*									      */
/*  Output Global Variables                                                   */
/*									      */
/******************************************************************************/

  USHORT usToken;               /* Value of the last token retrieved        */
  USHORT usTokenLast;           /* Value of the previous to above token     */


/******************************************************************************/
/*									      */
/*  Local Variables							      */
/*									      */
/******************************************************************************/

  static PBYTE  pbPosition;     /* Position to scan for the next token      */
  static PBYTE  pbLastPos;      /* Last start position of the last token    */


/******************************************************************************/
/*									      */
/*  Local Procedure's Prototypes                                              */
/*									      */
/******************************************************************************/

  static USHORT near IdentifyString(PBYTE);


/******************************************************************************/
/*									      */
/*  Global Procedures							      */
/*									      */
/******************************************************************************/


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT TokenAsciiToTokens(szText, pbTokens, wMax)                          */
/*									      */
/*  Gets the next token from the ascii string defined in TokenSetLine.  Each  */
/*  time a new token is retrieved, the point to the ascii string is updated   */
/*  to the next.  At end of line, TokenGetNext keeps returning TKN_EOL until  */
/*  a new line is specified.                                                  */
/*									      */
/*----------------------------------------------------------------------------*/
SHORT TokenAsciiToTokens(szText, pbTokens, wMax)
  PSTR   szText;
  BYTE   *pbTokens;
  WORD   wMax;
{
  USHORT usToken;
  BYTE   byToken[256];
  register i;


  TokenSetLine(szText);

  i = sizeof(int);
  while (i < wMax - 2)
    {
    TokenGetNext(&usToken, byToken);

    pbTokens[i++] = (BYTE)LOBYTE(usToken);
    pbTokens[i++] = (BYTE)HIBYTE(usToken);
    switch (usToken)
      {
      case TKN_NUMCON:
        pbTokens[i++] = byToken[0];
        pbTokens[i++] = byToken[1];
        break;

      case TKN_IDENT:
      case TKN_LABEL:
      case TKN_STRCON:
        {
        register j;

        for (j = 0; byToken[j]; j++)
          pbTokens[i++] = byToken[j];
        pbTokens[i++] = NULL;
        }
        break;
      }

    if (usToken == TKN_EOL || usToken == TKN_ERROR)
      break;
    }

  pbTokens[0] = (BYTE)i;
  pbTokens[1] = (BYTE)(i >> 8);

  return (0);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  PBYTE TokenGetNext(pusToken, pvoidToken)                                  */
/*									      */
/*  Gets the next token from the ascii string defined in TokenSetLine.  Each  */
/*  time a new token is retrieved, the point to the ascii string is updated   */
/*  to the next.  At end of line, TokenGetNext keeps returning TKN_EOL until  */
/*  a new line is specified.                                                  */
/*									      */
/*----------------------------------------------------------------------------*/
PBYTE TokenGetNext(PUSHORT pusToken, PVOID pvoidToken)
{
  while (isspace(*pbPosition))
    pbPosition++;

  pbLastPos   = pbPosition;
  usTokenLast = usToken;


  switch (*pbPosition)
    {
    case NULL:
      usToken = TKN_EOL;
      break;

    case ';':
      usToken = TKN_SEMI;
      pbPosition++;
      break;

    case ',':
      usToken = TKN_COMMA;
      pbPosition++;
      break;

    case '=':
      usToken = TKN_EQUAL;
      pbPosition++;
      break;

    case '+':
      usToken = TKN_PLUS;
      pbPosition++;
      break;

    case ':':
      {
      register int  i;
      char fStr;

      pbPosition++;
      i = 0;
      while (isalnum(*pbPosition) || *pbPosition == '_')
        {
        if (i < 16 && *pbPosition != '_')
          ((PBYTE)pvoidToken)[i++] = toupper(*pbPosition);
        pbPosition++;
        }
      ((PBYTE)pvoidToken)[i] = NULL;

      usToken = TKN_LABEL;
      }
      break;

    case '"':
      {
      register char *pStr;

      pStr = pvoidToken;
      pbPosition++;
      while (*pbPosition != '\n' && *pbPosition != '\r' && *pbPosition != NULL)
	{
        if (*pbPosition == '"')
	  {
          pbPosition++;
          if (*pbPosition != '"')
	    break;
	  }

        if (*pbPosition == '^' && pbPosition[1] >= '@' && pbPosition[1] <= 'Z')
          {
          pbPosition++;
          *pStr++ = *pbPosition++ - '@';
          }
        else
          *pStr++ = *pbPosition++;
        }
      *pStr = NULL;
      }
      usToken = TKN_STRCON;
      break;

    default:
      if (isdigit(*pbPosition))
	{
	register int i;

	i = 0;
        while (isdigit(*pbPosition))
	  {
          i = i * 10 + (*pbPosition - '0');
          pbPosition++;
	  }

        *(PSHORT)pvoidToken = i;
        usToken = TKN_NUMCON;
	}
      else if (isalpha(*pbPosition))
	{
	register int  i;
	char fStr;

	i = 0;
        while (isalnum(*pbPosition) || *pbPosition == '_')
	  {
          if (i < 16 && *pbPosition != '_')
            ((PBYTE)pvoidToken)[i++] = toupper(*pbPosition);
          pbPosition++;
	  }
        ((PBYTE)pvoidToken)[i] = NULL;

        usToken = IdentifyString((PBYTE)pvoidToken);
	}
      else
	{
        usToken = TKN_ERROR;
        pbPosition++;
	}
      break;
    }

  *pusToken = usToken;
  return (pbPosition);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  PBYTE TokenSetLine(pbLine)                                                */
/*									      */
/*  This routine initializes the variable for the TokenGetNext routine.  It   */
/*  much be called before the first time that TokenGetNext and must be called */
/*  to a new line after the TKN_EOL token is received.                        */
/*									      */
/*----------------------------------------------------------------------------*/
PBYTE TokenSetLine(pbLine)
  PBYTE pbLine;
{
  pbPosition = pbLine;
  pbLastPos  = pbLine;

  usToken     = TKN_EOL;
  usTokenLast = TKN_EOL;

  return (pbPosition);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  static USHORT IdentifyString(PBYTE)                                       */
/*									      */
/*  This routine tries to identify the given string, if not it returns the    */
/*  TKN_IDENT token.  The tokens that are in the TokenTable are in the same   */
/*  sequence as they are defined in the TOKENS.H file.  Thus the first token  */
/*  is the list has the value of 0, and so on.                                */
/*									      */
/*----------------------------------------------------------------------------*/
static USHORT near IdentifyString(PBYTE pbText)
  {
  if (strcmp(pbText, "BREAK") == 0)
    return (TKN_BREAK);
  if (strcmp(pbText, "DIAL") == 0)
    return (TKN_DIAL);
  if (strcmp(pbText, "DTR") == 0)
    return (TKN_DTR);
  if (strcmp(pbText, "END") == 0)
    return (TKN_END);
  if (strcmp(pbText, "GOTO") == 0)
    return (TKN_GOTO);
  if (strcmp(pbText, "IF") == 0)
    return (TKN_IF);
  if (strcmp(pbText, "INPUT") == 0)
    return (TKN_INPUT);
  if (strcmp(pbText, "NUMBER") == 0)
    return (TKN_NUMBER);
  if (strcmp(pbText, "NODE") == 0)
    return (TKN_NODE);
  if (strcmp(pbText, "PREFIX") == 0)
    return (TKN_PREFIX);
  if (strcmp(pbText, "PRINT") == 0)
    return (TKN_PRINT);
  if (strcmp(pbText, "SLEEP") == 0)
    return (TKN_SLEEP);
  if (strcmp(pbText, "SPEED") == 0)
    return (TKN_SPEED);
  if (strcmp(pbText, "THEN") == 0)
    return (TKN_THEN);
  if (strcmp(pbText, "TIMEOUT") == 0)
    return (TKN_TIMEOUT);

  return (TKN_IDENT);
  }
