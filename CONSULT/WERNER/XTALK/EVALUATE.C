/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Expression Evaluation (EVALUATE.C)                              *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module performs the expression evaluation.                 *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <ctype.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "compile.h"
#include "interp.h"
#include "variable.h"
#include "comm.h"
#include "xfer.h"


#define OpCode(x)	(*((WORD far *)(&lpObjectCode[x])))
  extern WORD wAddr;

  static int near EvalExpression(BYTE *, BYTE *, long *);
  static int near PutOperator(WORD, WORD *, WORD *, long *, WORD *);


/*---------------------------------------------------------------------------*/
/*  Evaluate Boolean							     */
/*									     */
/*---------------------------------------------------------------------------*/
int EvalBoolean(pFlag)
  char *pFlag;
{
  BYTE *byBuf, byLen;
  long lInteger;
  register i;


  if (nStackAddr + 512 > nStackHigh)
    return (ERRINT_TOOCOMPLEX);
  byBuf = &filexfer.stack.data[nStackAddr];
  nStackAddr += 256;

  switch (i = EvalExpression(byBuf, &byLen, &lInteger))
    {
    case 0:
      if (byLen)
	*pFlag = TRUE;
      else
	*pFlag = FALSE;
      i = 0;
      break;

    case 1:
      if (lInteger)
	*pFlag = TRUE;
      else
	*pFlag = FALSE;
      i = 0;
      break;

    case 2:
      i = ERRINT_WANTINTEGER;
      break;
    }

  nStackAddr -= 256;
  return (i);
}

/*---------------------------------------------------------------------------*/
/*  Evaluate Channel							     */
/*									     */
/*---------------------------------------------------------------------------*/
int EvalChannel(nChannel, fFollows)
  int  *nChannel;
  char fFollows;
{
  long lInteger;
  register i;


  if (OpCode(wAddr) == TK_POUND)
    wAddr += 2;

  if (i = EvalInteger(&lInteger))
    return (i);

  *nChannel = (int)lInteger;
  if (*nChannel < 1 || *nChannel > 8)
    return (ERRIOE_MISCHANNEL);
  (*nChannel)--;

  if (fFollows == FW_COMMA)
    {
    if (OpCode(wAddr) == TK_COMMA)
      wAddr += 2;
    else
      return (ERRINT_MISCOMMA);
    }
  else if (fFollows == FW_EOL)
    {
    if (!IfEndOfLine())
      return (ERRINT_EXTRATEXT);
    }

  return (0);
}

/*---------------------------------------------------------------------------*/
/*  Evaluate Integer							     */
/*									     */
/*---------------------------------------------------------------------------*/
int EvalInteger(pLong)
  long *pLong;
{
  BYTE *byBuf, byLen;
  long lInteger;
  register i;


  if (nStackAddr + 512 > nStackHigh)
    return (ERRINT_TOOCOMPLEX);
  byBuf = &filexfer.stack.data[nStackAddr];
  nStackAddr += 256;

  switch (i = EvalExpression(byBuf, &byLen, &lInteger))
    {
    case 0:
      i = ERRINT_MIXTYPES;
      break;

    case 1:
      *pLong = lInteger;
      i = 0;
      break;

    case 2:
      i = ERRINT_WANTINTEGER;
      break;
    }

  nStackAddr -= 256;
  return (i);
}

/*---------------------------------------------------------------------------*/
/*  Evaluate EvalIntVariable						     */
/*									     */
/*---------------------------------------------------------------------------*/
int EvalIntVariable(nVarNo, fFollows)
  int  *nVarNo;
  char fFollows;
{
  char buf[64];
  register i;


  if (OpCode(wAddr) != TK_VARIABLE)
    return (ERRINT_MISVARIABLE);

  wAddr += 2;
  lstrcpy(buf, &lpObjectCode[wAddr]);
  while (lpObjectCode[wAddr++]);

  if ((*nVarNo = GetVariableHandle(buf)) == -1)
    return (ERRINT_NOVARDEF);

  if (IsStrVariable(*nVarNo))
    return (ERRINT_BADVARTYPE);

  if (fFollows == FW_COMMA)
    {
    if (OpCode(wAddr) == TK_COMMA)
      wAddr += 2;
    else
      return (ERRINT_MISCOMMA);
    }
  else if (fFollows == FW_EOL)
    {
    if (!IfEndOfLine())
      return (ERRINT_EXTRATEXT);
    }

  return (0);
}

/*---------------------------------------------------------------------------*/
/*  Evaluate Range							     */
/*									     */
/*---------------------------------------------------------------------------*/
int EvalRange(nNum, nLow, nHigh, fFollows)
  int  *nNum;
  int  nLow;
  int  nHigh;
  char fFollows;
{
  long lInteger;
  register i;


  if (i = EvalInteger(&lInteger))
    return (i);

  *nNum = (int)lInteger;
  if (*nNum < nLow || *nNum > nHigh)
    return (ERRATH_OUTOFRANGE);

  if (fFollows == FW_COMMA)
    {
    if (OpCode(wAddr) == TK_COMMA)
      wAddr += 2;
    else
      return (ERRINT_MISCOMMA);
    }
  else if (fFollows == FW_EOL)
    {
    if (!IfEndOfLine())
      return (ERRINT_EXTRATEXT);
    }

  return (0);
}

/*---------------------------------------------------------------------------*/
/*  Evaluate String							     */
/*									     */
/*---------------------------------------------------------------------------*/
int EvalString(pBuf, pLen)
  BYTE *pBuf;
  BYTE *pLen;
{
  long lInteger;
  char buf[32];
  BYTE len;
  int  nPCnt;
  register int	i;


  nPCnt = 0;
  *pLen = 0;
  while (1)
    {
    /* Get a string argument */

    while (OpCode(wAddr) == TK_LPAR)
      {
      nPCnt++;
      wAddr += 2;
      }

    if (OpCode(wAddr) == TK_NEXTLINE)
      {
      return (ERRINT_BADEXPRESSION);
      }
    else if (OpCode(wAddr) == TK_STRING)
      {
      len = lpObjectCode[wAddr+2];
      if ((int)*pLen + (int)len > 255)
	return (ERRINT_LONGSTRING);
      lmemcpy(pBuf, &lpObjectCode[wAddr+3], len);
      wAddr += len + 3;
      pBuf  += len;
      *pLen += len;
      }
    else if (OpCode(wAddr) == TK_VARIABLE)
      {
      lstrcpy(buf, &lpObjectCode[wAddr+2]);
      i = GetVariableHandle(buf);
      if (i < 0)
	return (ERRINT_NOVARDEF);
      if (IsIntVariable(i))
	return (ERRINT_MIXTYPES);
      len = GetStrLength(i);
      if ((int)(*pLen) + (int)len > 255)
	return (ERRINT_LONGSTRING);
      GetStrVariable(i, pBuf, 255);
      pBuf  += len;
      *pLen += len;
      wAddr += 2;
      while (lpObjectCode[wAddr])
	wAddr++;
      wAddr++;
      }
    else if (OpCode(wAddr) == TK_INTEGER)
      {
      return (ERRINT_MIXTYPES);
      }
    else if ((OpCode(wAddr) & CL_FUNCT) == CL_FUNCT)
      {
      if ((OpCode(wAddr) & CL_STRFN) != CL_STRFN)
	return (ERRINT_MIXTYPES);

      len = *pLen;
      if (i = EvalStrFunct(pBuf, pLen))
	return (i);
      pBuf  += *pLen;
      *pLen += len;
      }
    else if (OpCode(wAddr) & CL_SYVAR)
      {
      i = OpCode(wAddr);
      i = GetSysVariableAny(i, pBuf, &len, &lInteger);
      if (i == 0)
	{
	if ((int)(*pLen) + (int)len > 255)
	  return (ERRINT_LONGSTRING);
	wAddr += 2;
	pBuf  += len;
	*pLen += len;
	}
      else
	{
	return (ERRINT_MIXTYPES);
	}
      }
    else if (OpCode(wAddr) == TK_EOVL || OpCode(wAddr) == TK_EOLL)
      {
      wAddr += 2;
      wLine++;
      }
    else
      {
      return (ERRINT_MIXTYPES);
      }

    /* Get Operand, only plus supported for strings */

    while (OpCode(wAddr) == TK_RPAR && nPCnt)
      {
      nPCnt--;
      wAddr += 2;
      }

    if (OpCode(wAddr) != TK_PLUS)
      {
      if (nPCnt)
	return (ERRINT_BADEXPRESSION);

      *pBuf = NULL;
      return (0);
      }
    wAddr += 2;
    if (OpCode(wAddr) == TK_EOVL)
      {
      wAddr += 2;
      wLine++;
      }
    }
}

/*---------------------------------------------------------------------------*/
/*  Evaluate EvalStrVariable						     */
/*									     */
/*---------------------------------------------------------------------------*/
int EvalStrVariable(nVarNo, fFollows)
  int  *nVarNo;
  char fFollows;
{
  char buf[64];
  register i;


  if (OpCode(wAddr) != TK_VARIABLE)
    return (ERRINT_MISVARIABLE);

  wAddr += 2;
  lstrcpy(buf, &lpObjectCode[wAddr]);
  while (lpObjectCode[wAddr++]);

  if ((*nVarNo = GetVariableHandle(buf)) == -1)
    return (ERRINT_NOVARDEF);

  if (!IsStrVariable(*nVarNo))
    return (ERRINT_BADVARTYPE);

  if (fFollows == FW_COMMA)
    {
    if (OpCode(wAddr) == TK_COMMA)
      wAddr += 2;
    else
      return (ERRINT_MISCOMMA);
    }
  else if (fFollows == FW_EOL)
    {
    if (!IfEndOfLine())
      return (ERRINT_EXTRATEXT);
    }

  return (0);
}

/*---------------------------------------------------------------------------*/
/*  Evaluate Time							     */
/*									     */
/*---------------------------------------------------------------------------*/
int EvalTime(pLong)
  long *pLong;
{
  long lInteger;
  register i;


  if (i = EvalInteger(&lInteger))
    return (i);

  switch (OpCode(wAddr))
    {
    case TK_TICK:
      *pLong = lInteger * 100L;
      wAddr += 2;
      break;

    case TK_MILLISEC:
      *pLong = lInteger;
      wAddr += 2;
      break;

    case TK_SECOND:
      *pLong = lInteger * 1000L;
      wAddr += 2;
      break;

    case TK_MINUTE:
      *pLong = lInteger * 60000L;
      wAddr += 2;
      break;

    case TK_HOUR:
      *pLong = lInteger * 3600000L;
      wAddr += 2;
      break;

    default:
      *pLong = lInteger * 1000L;
      break;
    }

  return (0);
}

int EvalIntIntIntInt(pInt1, pInt2, pInt3, pInt4)
  int  *pInt1;
  int  *pInt2;
  int  *pInt3;
  int  *pInt4;
{
  long lInteger;
  register i;


  if (i = EvalInteger(&lInteger))
    return (i);
  *pInt1 = (int)lInteger;
  if (i = EvalComma())
    return (i);

  if (i = EvalInteger(&lInteger))
    return (i);
  *pInt2 = (int)lInteger;
  if (i = EvalComma())
    return (i);

  if (i = EvalInteger(&lInteger))
    return (i);
  *pInt3 = (int)lInteger;
  if (i = EvalComma())
    return (i);

  if (i = EvalInteger(&lInteger))
    return (i);
  *pInt4 = (int)lInteger;

  return (0);
}


int EvalIntIntIntIntStr(pInt1, pInt2, pInt3, pInt4, pStr)
  int  *pInt1;
  int  *pInt2;
  int  *pInt3;
  int  *pInt4;
  char *pStr;
{
  BYTE cStr;
  register i;


  if (i = EvalIntIntIntInt(pInt1, pInt2, pInt3, pInt4))
    return (i);

  if (i = EvalComma())
    return (i);

  if (i = EvalString(pStr, &cStr))
    return (i);

  return (0);
}

int EvalIntvar(pVarNo)
  int *pVarNo;
{
  char buf[64];


  if (OpCode(wAddr) != TK_VARIABLE)
    return (ERRINT_MISVARIABLE);

  wAddr += 2;
  lstrcpy(buf, &lpObjectCode[wAddr]);
  while (lpObjectCode[wAddr++]);

  if ((*pVarNo = GetVariableHandle(buf)) == -1)
    return (ERRINT_NOVARDEF);

  if (IsStrVariable(*pVarNo))
    return (ERRINT_BADVARTYPE);

  return (0);
}

int EvalStrvar(pVarNo)
  int *pVarNo;
{
  char buf[64];


  if (OpCode(wAddr) != TK_VARIABLE)
    return (ERRINT_MISVARIABLE);

  wAddr += 2;
  lstrcpy(buf, &lpObjectCode[wAddr]);
  while (lpObjectCode[wAddr++]);

  if ((*pVarNo = GetVariableHandle(buf)) == -1)
    return (ERRINT_NOVARDEF);

  if (!IsStrVariable(*pVarNo))
    return (ERRINT_BADVARTYPE);

  return (0);
}

int EvalCtlFlags(pFlags)
  WORD *pFlags;
{
  register i;


  *pFlags = NULL;

  if (i = EvalEol())
    {
    if (i = EvalComma())
      return (i);

    while (OpCode(wAddr) == TK_GROUP || OpCode(wAddr) == TK_TABSTOP ||
           OpCode(wAddr) == TK_OK    || OpCode(wAddr) == TK_CANCEL  ||
           OpCode(wAddr) == TK_FOCUS)
      {
      if (OpCode(wAddr) == TK_GROUP)
        *pFlags |= 0x01;
      else if (OpCode(wAddr) == TK_TABSTOP)
        *pFlags |= 0x02;
      else if (OpCode(wAddr) == TK_OK)
        *pFlags |= 0x04;
      else if (OpCode(wAddr) == TK_CANCEL)
        *pFlags |= 0x08;
      else if (OpCode(wAddr) == TK_FOCUS)
        *pFlags |= 0x10;
      wAddr += 2;
      if (OpCode(wAddr) == TK_COMMA)
        wAddr += 2;
      }

    i = EvalEol();
    }

  return (i);
}

int EvalComma()
{
  if (OpCode(wAddr) == TK_COMMA)
    wAddr += 2;
  else
    return (ERRINT_MISCOMMA);

  return (0);
}

int EvalEol()
{
  if (OpCode(wAddr) == TK_EOL)
    {
    wAddr += 2;
    }
  else if (OpCode(wAddr) == TK_EOPL || OpCode(wAddr) == TK_EOVL)
    {
    wAddr += 2;
    wLine++;
    }
  else
    return (ERRINT_EXTRATEXT);

  return (0);
}


/*---------------------------------------------------------------------------*/
/*  Evaluate Expression 						     */
/*									     */
/*---------------------------------------------------------------------------*/
static int near EvalExpression(pBuf, pLen, plInteger)
  BYTE *pBuf;
  BYTE *pLen;
  long *plInteger;
{
  int  nStackSave;
  char buf[32];
  BYTE *pBuf1, bLen1, *pPreStr, bPreLen, fPreStr;
  long lInteger;
  WORD wOperators[32], wOps, wInt;
  long lIntegers[32];
  register i;


  if (nStackAddr + 786 > nStackHigh)
    return (ERRINT_TOOCOMPLEX);
  nStackSave = nStackAddr;

  pPreStr = &filexfer.stack.data[nStackAddr];
  nStackAddr += 256;
  pBuf1 = &filexfer.stack.data[nStackAddr];
  nStackAddr += 256;

  fPreStr = FALSE;
  wOps = wInt = 0;
  while (1)
    {
    i = OpCode(wAddr);
    while (i == TK_NOT || i == TK_NEG || i == TK_SUB || i == TK_BITNOT)
      {
      if (i == TK_SUB)
	i = TK_NEG;
      if (i = PutOperator(i, wOperators, &wOps, lIntegers, &wInt))
	break;
      wAddr += 2;
      i = OpCode(wAddr);
      }

    if (OpCode(wAddr) == TK_LPAR)
      {
      wAddr += 2;
      i = EvalExpression(pBuf1, &bLen1, &lInteger);
      }
    else if (OpCode(wAddr) == TK_VARIABLE)
      {
      wAddr += 2;
      lstrcpy(buf, &lpObjectCode[wAddr]);
      i = GetVariableHandle(buf);
      if (i < 0)
	{
	i = ERRINT_NOVARDEF;
	break;
	}
      if (IsStrVariable(i))
	{
	bLen1 = GetStrVariable(i, pBuf1, 255);
	i = 0;
	}
      else
	{
	GetIntVariable(i, &lInteger);
	i = 1;
	}
      while (lpObjectCode[wAddr])
	wAddr++;
      wAddr++;
      }
    else if (OpCode(wAddr) == TK_INTEGER)
      {
      lInteger = *(long far *)(&lpObjectCode[wAddr+2]);
      wAddr += 6;
      i = 1;
      }
    else if (OpCode(wAddr) == TK_STRING)
      {
      if (i = EvalString(pBuf1, &bLen1))
	break;
      i = 0;
      }
    else if ((OpCode(wAddr) & CL_STRFN) == CL_STRFN)
      {
      if (i = EvalString(pBuf1, &bLen1))
	break;
      i = 0;
      }
    else if ((OpCode(wAddr) & CL_FUNCT) == CL_FUNCT)
      {
      if (i = EvalIntFunct(&lInteger))
	break;
      i = 1;
      }
    else if (OpCode(wAddr) & CL_SYVAR)
      {
      i = GetSysVariableAny(OpCode(wAddr), pBuf1, &bLen1, &lInteger);
      wAddr += 2;
      }
    else if (OpCode(wAddr) == TK_EOVL)
      {
      wAddr += 2;
      wLine++;
      }
    else
      {
      i = ERRINT_EXTRATEXT;
      break;
      }

    if (wOps >= 30 || wInt >= 30)
      {
      i = ERRINT_TOOCOMPLEX;
      break;
      }

    if (i == 0)
      {
      if (fPreStr)
	{
	fPreStr = FALSE;

	if (bPreLen > bLen1)
	  i = bLen1;
	else
	  i = bPreLen;
	if ((i = memcmp(pPreStr, pBuf1, i)) == 0)
	  {
	  if (bPreLen > bLen1)
	    i = 1;
	  else if (bPreLen < bLen1)
            i = 1;
	  }

	lIntegers[wInt++] = 0;
	switch (wOperators[--wOps])
	  {
	  case TK_GT:
	    if (i > 0)
              lIntegers[wInt-1] = 1;
	    i = 0;
	    break;

	  case TK_GE:
	    if (i >= 0)
              lIntegers[wInt-1] = 1;
	    i = 0;
	    break;

	  case TK_EQ:
	    if (i == 0)
              lIntegers[wInt-1] = 1;
	    i = 0;
	    break;

	  case TK_INEQ:
	    if (i != 0)
              lIntegers[wInt-1] = 1;
	    i = 0;
	    break;

	  case TK_LE:
	    if (i <= 0)
              lIntegers[wInt-1] = 1;
	    i = 0;
	    break;

	  case TK_LT:
	    if (i < 0)
              lIntegers[wInt-1] = 1;
	    i = 0;
	    break;

	  default:
	    i = ERRINT_BADSTROP;
	    break;
	  }

	if (i)
	  break;
	}
      else
	{
	fPreStr = TRUE;
	memcpy(pPreStr, pBuf1, bLen1);
	bPreLen = bLen1;
	}
      }
    else if (i == 1)
      {
      lIntegers[wInt++] = lInteger;
      }
    else
      break;

    if (OpCode(wAddr) & CL_OPERN)
      {
      if (i = PutOperator(OpCode(wAddr), wOperators, &wOps, lIntegers, &wInt))
	break;

      wAddr += 2;
      }
    else
      {
      if (fPreStr)
	{
	if (wOps || wInt)
	  i = ERRINT_BADEXPRESSION;
	else
	  {
	  memcpy(pBuf, pPreStr, bPreLen);
	  *pLen = bPreLen;
	  i = 0;
	  }
	}
      else
	{
	if (i = PutOperator(0, wOperators, &wOps, lIntegers, &wInt))
	  break;
	if (wOps || wInt != 1)
	  i = ERRINT_BADEXPRESSION;
	else
	  {
	  *plInteger = lIntegers[0];
	  i = 1;
	  }
	}
      break;
      }
    }

  if (OpCode(wAddr) == TK_RPAR)
    wAddr += 2;

  nStackAddr = nStackSave;
  return (i);
}

static int near PutOperator(wOp, wOperators, wOps, lIntegers, wInt)
  WORD wOp;
  WORD *wOperators;
  WORD *wOps;
  long *lIntegers;
  WORD *wInt;
{
  if (*wOps > 30)
    return (ERRINT_TOOCOMPLEX);

  while (*wOps && (wOp & 0xF0) < (wOperators[(*wOps)-1] & 0xF0))
    {
    (*wOps)--;
    switch (wOperators[*wOps])
      {
      case TK_OR:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	if (lIntegers[*wInt-1] || lIntegers[*wInt-2])
          lIntegers[*wInt-2] = 1L;
	else
	  lIntegers[*wInt-2] = 0L;
	(*wInt)--;
	break;

      case TK_AND:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	if (lIntegers[*wInt-1] && lIntegers[*wInt-2])
          lIntegers[*wInt-2] = 1L;
	else
	  lIntegers[*wInt-2] = 0L;
	(*wInt)--;
	break;

      case TK_NOT:
	if (lIntegers[*wInt-1])
	  lIntegers[*wInt-1] = 0L;
	else
          lIntegers[*wInt-1] = 1L;
	break;

      case TK_EQ:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	if (lIntegers[*wInt-2] == lIntegers[*wInt-1])
          lIntegers[*wInt-2] = 1L;
	else
	  lIntegers[*wInt-2] = 0L;
	(*wInt)--;
	break;

      case TK_GE:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	if (lIntegers[*wInt-2] >= lIntegers[*wInt-1])
          lIntegers[*wInt-2] = 1L;
	else
	  lIntegers[*wInt-2] = 0L;
	(*wInt)--;
	break;

      case TK_GT:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	if (lIntegers[*wInt-2] > lIntegers[*wInt-1])
          lIntegers[*wInt-2] = 1L;
	else
	  lIntegers[*wInt-2] = 0L;
	(*wInt)--;
	break;

      case TK_INEQ:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	if (lIntegers[*wInt-2] != lIntegers[*wInt-1])
          lIntegers[*wInt-2] = 1L;
	else
	  lIntegers[*wInt-2] = 0L;
	(*wInt)--;
	break;

      case TK_LE:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	if (lIntegers[*wInt-2] <= lIntegers[*wInt-1])
          lIntegers[*wInt-2] = 1L;
	else
	  lIntegers[*wInt-2] = 0L;
	(*wInt)--;
	break;

      case TK_LT:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	if (lIntegers[*wInt-2] < lIntegers[*wInt-1])
          lIntegers[*wInt-2] = 1L;
	else
	  lIntegers[*wInt-2] = 0L;
	(*wInt)--;
	break;

      case TK_MIN:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
        if (lIntegers[*wInt-2] < lIntegers[*wInt-1])
	  lIntegers[*wInt-2] = lIntegers[*wInt-1];
	(*wInt)--;
	break;

      case TK_MAX:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
        if (lIntegers[*wInt-2] > lIntegers[*wInt-1])
	  lIntegers[*wInt-2] = lIntegers[*wInt-1];
	(*wInt)--;
	break;

      case TK_PLUS:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	lIntegers[*wInt-2] += lIntegers[*wInt-1];
       (*wInt)--;
       break;

      case TK_BITOR:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	lIntegers[*wInt-2] |= lIntegers[*wInt-1];
       (*wInt)--;
       break;

      case TK_SUB:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	lIntegers[*wInt-2] -= lIntegers[*wInt-1];
       (*wInt)--;
       break;

      case TK_BITAND:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	lIntegers[*wInt-2] &= lIntegers[*wInt-1];
       (*wInt)--;
       break;

      case TK_BITXOR:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	lIntegers[*wInt-2] ^= lIntegers[*wInt-1];
       (*wInt)--;
       break;

      case TK_DIV:
      case TK_IDIV:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	if (lIntegers[*wInt-1] == 0L)
	  return(ERRATH_DIVBYZERO);
	lIntegers[*wInt-2] /= lIntegers[*wInt-1];
       (*wInt)--;
       break;

      case TK_MOD:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	lIntegers[*wInt-2] %= lIntegers[*wInt-1];
       (*wInt)--;
       break;

      case TK_MUL:
	if (*wInt < 2)
	  return (ERRINT_BADEXPRESSION);
	lIntegers[*wInt-2] *= lIntegers[*wInt-1];
       (*wInt)--;
       break;

      case TK_BITNOT:
	lIntegers[*wInt-1] = ~lIntegers[*wInt-1];
	break;

      case TK_NEG:
	lIntegers[*wInt-1] = -lIntegers[*wInt-1];
	break;

      default:
	return (ERRINT_INTERNAL);
      }

    if (*wInt < 1)
      return (ERRINT_INTERNAL);
    }

  if (wOp)
    wOperators[(*wOps)++] = wOp;

  return (0);
}
