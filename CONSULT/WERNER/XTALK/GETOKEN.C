/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Get Token (GETOKEN.C)                                           *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module converts the next item on the source line to a      *
*             token that is easier to process in the compiler logic.          *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "getoken.h"
#include "provars.h"


  char *pText, *pOldText;
  int  nToken, nOldToken;
  int  nTokenNum;

int near GetNextToken(ptr)
  char *ptr;
{

  while (isspace(*pText))
    pText++;

  pOldText  = pText;
  nOldToken = nToken;

  switch (*pText)
    {
    case NULL:
    case '\'':
      nToken = TOKEN_EOL;
      break;

    case ',':
      nToken = TOKEN_COMMA;
      pText++;
      break;

    case ';':
      nToken = TOKEN_SEMI;
      pText++;
      break;

    case '+':
      nToken = TOKEN_PLUS;
      pText++;
      break;

    case '-':
      nToken = TOKEN_MINUS;
      pText++;
      break;

    case '*':
      nToken = TOKEN_MULTIPLY;
      pText++;
      break;

    case '/':
      nToken = TOKEN_DIVIDE;
      pText++;
      break;

    case '=':
      nToken = TOKEN_EQUAL;
      pText++;
      break;

    case '>':
      pText++;
      if (*pText == '=')
	{
	nToken = TOKEN_GE;
	pText++;
	}
      else
	{
	nToken = TOKEN_GT;
	}
      break;

    case '<':
      pText++;
      if (*pText == '=')
	{
	nToken = TOKEN_LE;
	pText++;
	}
      else if (*pText == '>')
	{
	nToken = TOKEN_NE;
	pText++;
	}
      else
	{
	nToken = TOKEN_LT;
	}
      break;

    case '(':
      nToken = TOKEN_LPAR;
      pText++;
      break;

    case ')':
      nToken = TOKEN_RPAR;
      pText++;
      break;

    case '&':
      pText++;
      if (toupper(*pText) == 'H')
	{
	register unsigned i;

	i = 0;
	pText++;
	while (1)
	  {
          if (*pText >= '0' && *pText <= '9')
            i = i * 16 + (*pText - '0');
          else if (toupper(*pText) >= 'A' && toupper(*pText) <= 'F')
            i = i * 16 + (toupper(*pText) - 'A' + 10);
	  else
	    break;

	  pText++;
	  }

	(unsigned int)nTokenNum = i;
	}
      else
	{
	register unsigned i;

        if (toupper(*pText) == 'O')
	  pText++;

	i = 0;
        while (*pText >= '0' && *pText <= '7')
	  {
          i = i * 8 + (*pText - '0');
	  pText++;
	  }

	(unsigned int)nTokenNum = i;
	}

      nToken = TOKEN_NUMCON;
      break;

    case '"':
      {
      register char *pStr;


      pStr = ptr;
      pText++;
      while (*pText != '\n' && *pText != '\r' && *pText != NULL)
	{
        if (*pText == '"')
	  {
	  pText++;
          if (*pText != '"')
	    {
	    *pStr = NULL;
	    break;
	    }
	  }

	*pStr++ = *pText++;
	}
      }

      StringEncode(ptr, ptr);
      nToken = TOKEN_STRCON;
      break;

    default:
      if (isdigit(*pText))
	{
	register int i;

	i = 0;
	while (isdigit(*pText))
	  {
          i = i * 10 + (*pText - '0');
	  pText++;
	  }

	nTokenNum = i;
	nToken = TOKEN_NUMCON;
	}
      else if (isalpha(*pText))
	{
	register int  i;
	char fStr;

	i = 0;
        while (isalnum(*pText) || *pText == '_')
	  {
          if (i < 16 && *pText != '_')
	    ptr[i++] = toupper(*pText);
	  pText++;
	  }
	ptr[i] = NULL;

        if (*pText == '$')
	  {
	  fStr = 1;
	  pText++;
	  }
	else
	  fStr = 0;

        if (*pText == ':' && !fStr)
	  {
	  nToken = TOKEN_LABEL;
	  pText++;
	  }
	else
	  {
	  if (fStr)
	    {
	    if (nTokenNum = StrVarFind(ptr))
	      {
	      nToken = TOKEN_STRVAR;
	      break;
	      }
	     }
	  else
	    {
	    if (nTokenNum = IntVarFind(ptr))
	      {
	      nToken = TOKEN_NUMVAR;
	      break;
	      }
	    }

	  if (fStr)
	    {
            ptr[i+0] = '$';
	    ptr[i+1] = NULL;
	    }

	  if (i = TranslateToken(IDU_LNGCMDS, ptr))
	    nToken = i;
	  else
	    nToken = TOKEN_IDENT;
	  }
	}
      else
	{
	nToken = TOKEN_ERROR;
	pText++;
	}
      break;
    }

  return (nToken);
}


int near ExpectToken(nToken)
  int  nToken;
{
  char buf[82];


  if (GetNextToken(buf) != nToken)
    return (SYSERR_SYNTAX);

  return (0);
}

