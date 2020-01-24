/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Dynamic Dialog Box Generator Module (DLGBOX.C)                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module is responsible for generating and displaying user   *
*             defined dialogs through use of the script language.  We get     *
*             called from the script interpreter when it encounters the       *
*             'DialogBox' statement.  We then build the dialog based on the   *
*             users parameters and if everything is ok we display it, else    *
*             return an error back to the interpreter.                        *
*                                                                             *
*  Revisions: 																   *
*  1.00 07/17/89 Initial Version.                                  		   *
*  1.02 02/16/90 dca00024 PJL Make sure memory handles passed to the Windows  *
*                             GlobalReAlloc routine get reset after the call. *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "compile.h"
#include "interp.h"
#include "variable.h"
#include "dialogs.h"


/******************************************************************************/
/*									      */
/*  Local Constants							      */
/*									      */
/******************************************************************************/

#define COMM_INPBUF	2048	/* Define the size of our input buffer	    */
#define COMM_OUTBUF	1024	/* Define the size of our output buffer     */
#define COMM_LINE       256     /* Define the size of the line buffers      */

#define OpCode(x)	(*((WORD far *)(&lpObjectCode[x])))


/******************************************************************************/
/*									      */
/*  Local Structures                                                          */
/*									      */
/******************************************************************************/

typedef struct
  {
  long dtStyle;
  BYTE dtItemCount;
  int  dtX;
  int  dtY;
  int  dtCX;
  int  dtCY;
  char dtResourceName[1];
  char dtClassName[1];
  char dtCaptionText[1];
  } DLGHDR;

typedef struct
  {
  int  dtX;
  int  dtY;
  int  dtCX;
  int  dtCY;
  int  dtID;
  long dtStyle;
  BYTE dtClass;
  } DLGITEM;


/******************************************************************************/
/*									      */
/*  Local Variables							      */
/*									      */
/******************************************************************************/

  static HANDLE hMemory = 0;            /* Handle to global memory buffer   */
  static LPSTR  lpMemory;
  static int    iMemory;
  static int    cMemory;
  static int    nIdent;
  static int    nFocus;


/******************************************************************************/
/*									      */
/*  Dialog Prototypes                                                         */
/*									      */
/******************************************************************************/

  BOOL DlgDlgbox(HWND, unsigned, WORD, LONG);


/******************************************************************************/
/*									      */
/*  Local Procedures Prototypes 					      */
/*									      */
/******************************************************************************/

  static SHORT near GenerateDialog();
  static SHORT near DisplayDialog();
  static SHORT near GenerateHeader(int, int, int, int);
  static SHORT near GenerateControl(int, int, int, int, int, PSTR, WORD, int);
  static SHORT near GenerateTrailer(int);


/******************************************************************************/
/*									      */
/*  Global Procedures							      */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT DlgboxInterpret()                                                   */
/*									      */
/*----------------------------------------------------------------------------*/
SHORT DlgboxInterpret()
{
  register i;


  if ((i = GenerateDialog()) == 0)
    i = DisplayDialog();

  if (hMemory)
    {
    GlobalUnlock(hMemory);
    GlobalFree(hMemory);
    hMemory = NULL;
    }

  return (i);
}


/******************************************************************************/
/*									      */
/*  Local Procedures							      */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT GenerateDialog()                                                    */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT near GenerateDialog()
{
  int   nX, nY, nW, nH, nVarNo, nItems;
  WORD  wFlags;
  BYTE  bBuf[256];
  register i;


  if (i = EvalIntIntIntInt(&nX, &nY, &nW, &nH))
    return (i);
  if (i = EvalEol())
    return (i);
  if (i =  GenerateHeader(nX, nY, nW, nH))
    return (i);

  nItems = 0;
  while (1)
    {
    int  nCtl;

    switch (nCtl = OpCode(wAddr))
      {
      case TK_LTEXT:
      case TK_RTEXT:
      case TK_CTEXT:
      case TK_GROUPBOX:
        wAddr += 2;
        if (i = EvalIntIntIntIntStr(&nX, &nY, &nW, &nH, bBuf))
          return (i);
        if (i = EvalEol())
          return (i);
        if (i = GenerateControl(nCtl, nX, nY, nW, nH, bBuf, NULL, NULL))
          return (i);
        nItems++;
        break;

      case TK_PUSHBUTTON:
      case TK_DEFPUSHBUTTON:
        wAddr += 2;
        if (i = EvalIntIntIntIntStr(&nX, &nY, &nW, &nH, bBuf))
          return (i);
        if (i = EvalCtlFlags(&wFlags))
          return (i);
        if (i = GenerateControl(nCtl, nX, nY, nW, nH, bBuf, wFlags, NULL))
          return (i);
        nItems++;
        break;

      case TK_CHECKBOX:
      case TK_RADIOBUTTON:
      case TK_LISTBOX:
        wAddr += 2;
        if (i = EvalIntIntIntIntStr(&nX, &nY, &nW, &nH, bBuf))
          return (i);
        if (i = EvalComma())
          return (i);
        if (i = EvalIntvar(&nVarNo))
          return (i);
        if (i = EvalCtlFlags(&wFlags))
          return (i);
        if (i = GenerateControl(nCtl, nX, nY, nW, nH, bBuf, wFlags, nVarNo))
          return (i);
        nItems++;
        break;

      case TK_EDITTEXT:
        wAddr += 2;
        if (i = EvalIntIntIntIntStr(&nX, &nY, &nW, &nH, bBuf))
          return (i);
        if (i = EvalComma())
          return (i);
        if (i = EvalStrvar(&nVarNo))
          return (i);
        if (i = EvalCtlFlags(&wFlags))
          return (i);
        if (i = GenerateControl(nCtl, nX, nY, nW, nH, bBuf, wFlags, nVarNo))
          return (i);
        nItems++;
        break;

      case TK_ENDDIALOG:
        if (nItems == 0)
          return (ERRINT_NOCONTROL);
        if (i = GenerateTrailer(nItems))
          return (i);
        wAddr += 2;
        return (0);
        break;

      case TK_EOFM:
        return (ERRINT_NOENDDIALOG);
        break;

      default:
        return (ERRINT_BADCONTROL);
        break;
      }
    }
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT GenerateDialog()                                                    */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT near DisplayDialog()
{
  FARPROC lpDlg;


  lpDlg = MakeProcInstance((FARPROC)DlgDlgbox, hWndInst);
  DialogBoxIndirect(hWndInst, hMemory, hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  return (0);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT GenerateDialog()                                                    */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT near GenerateHeader(nX, nY, nW, nH)
  int  nX;
  int  nY;
  int  nW;
  int  nH;
{
  DLGHDR DlgHdr;
  LPSTR  lpStr;
  register i;


  if ((hMemory = GlobalAlloc(GMEM_MOVEABLE, 2048L)) == 0)
    return (ERRINT_MEMORY);

  iMemory  = 0;
  cMemory  = (int)GlobalSize(hMemory);
  lpMemory = GlobalLock(hMemory);
  nIdent   = 16;
  nFocus   = 0;

  DlgHdr.dtStyle = WS_DLGFRAME | WS_POPUP | WS_VISIBLE;
  DlgHdr.dtItemCount = 1;
  DlgHdr.dtX  = nX;
  DlgHdr.dtY  = nY;
  DlgHdr.dtCX = nW;
  DlgHdr.dtCY = nH;
  DlgHdr.dtResourceName[0] = NULL;
  DlgHdr.dtClassName[0] = NULL;
  DlgHdr.dtCaptionText[0] = NULL;

  lpStr = (LPSTR)&DlgHdr;
  for (i = 0; i < sizeof(DlgHdr); i++)
    *lpMemory++ = *lpStr++;

  iMemory += sizeof(DlgHdr);

  return (0);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT GenerateControl()                                                   */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT near GenerateControl(nTk, nX, nY, nW, nH, pText, wFlags, nVarNo)
  int  nTk;
  int  nX;
  int  nY;
  int  nW;
  int  nH;
  PSTR pText;
  WORD wFlags;
  int  nVarNo;
{
  DLGITEM DlgItem;
  PSTR    pStr;
  register i;


  if (strlen(pText) + 32 > cMemory - iMemory)
    {
    cMemory += 1024;
    GlobalUnlock(hMemory);
    if ((hMemory = GlobalReAlloc(hMemory, (DWORD)cMemory, GMEM_MOVEABLE)) == 0)
      return (ERRINT_MEMORY);
    cMemory  = (int)GlobalSize(hMemory);
    lpMemory = GlobalLock(hMemory);
    lpMemory += iMemory;
    }

  DlgItem.dtX  = nX;
  DlgItem.dtY  = nY;
  DlgItem.dtCX = nW;
  DlgItem.dtCY = nH;
  DlgItem.dtID = 0;

  switch (nTk)
    {
    case TK_LTEXT:
      DlgItem.dtClass = 0x82;
      DlgItem.dtStyle = SS_LEFT | WS_GROUP | WS_CHILD | WS_VISIBLE;
      break;

    case TK_RTEXT:
      DlgItem.dtClass = 0x82;
      DlgItem.dtStyle = SS_RIGHT | WS_GROUP | WS_CHILD | WS_VISIBLE;
      break;

    case TK_CTEXT:
      DlgItem.dtClass = 0x82;
      DlgItem.dtStyle = SS_CENTER | WS_GROUP | WS_CHILD | WS_VISIBLE;
      break;

    case TK_GROUPBOX:
      DlgItem.dtClass = 0x80;
      DlgItem.dtStyle = BS_GROUPBOX | WS_GROUP | WS_CHILD | WS_VISIBLE;
      break;

    case TK_CHECKBOX:
      DlgItem.dtClass = 0x80;
      DlgItem.dtStyle = BS_AUTOCHECKBOX | WS_GROUP | WS_CHILD | WS_VISIBLE;
      DlgItem.dtID = (nIdent++) + 1000;
      break;

    case TK_PUSHBUTTON:
      DlgItem.dtClass = 0x80;
      DlgItem.dtStyle = BS_PUSHBUTTON | WS_GROUP | WS_CHILD | WS_VISIBLE;
      DlgItem.dtID = nIdent++;
      break;

    case TK_DEFPUSHBUTTON:
      DlgItem.dtClass = 0x80;
      DlgItem.dtStyle = BS_DEFPUSHBUTTON | WS_GROUP | WS_CHILD | WS_VISIBLE;
      DlgItem.dtID = nIdent++;
      break;

    case TK_LISTBOX:
      DlgItem.dtClass = 0x83;
      DlgItem.dtStyle = WS_GROUP | WS_CHILD | WS_VISIBLE |
                        WS_VSCROLL | WS_BORDER;
      DlgItem.dtID = (nIdent++) + 1000;
      break;

    case TK_RADIOBUTTON:
      DlgItem.dtClass = 0x80;
      DlgItem.dtStyle = BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE;
      DlgItem.dtID = (nIdent++) + 1000;
      break;

    case TK_EDITTEXT:
      DlgItem.dtClass = 0x81;
      DlgItem.dtStyle = ES_LEFT | WS_BORDER | WS_GROUP | WS_CHILD | WS_VISIBLE;
      DlgItem.dtID = (nIdent++) + 1000;
      break;
    }

  if (wFlags & 0x01)
    DlgItem.dtStyle |= WS_GROUP;
  if (wFlags & 0x02)
    DlgItem.dtStyle |= WS_TABSTOP;
  if (wFlags & 0x04)
    DlgItem.dtID = 1;
  else if (wFlags & 0x08)
    DlgItem.dtID = 2;
  if (wFlags & 0x10)
    nFocus = DlgItem.dtID;

  pStr = (PSTR)&DlgItem;
  for (i = 0; i < sizeof(DlgItem); i++)
    *lpMemory++ = *pStr++;

  iMemory += sizeof(DlgItem);

  while (1)
    {
    *lpMemory++ = *pText;
    iMemory++;
    if (*pText == NULL)
      break;
    pText++;
    }

  *lpMemory++ = 2;
  *lpMemory++ = (BYTE)LOBYTE(nVarNo);
  *lpMemory++ = (BYTE)HIBYTE(nVarNo);
  iMemory += 3;

  return (0);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT GenerateDialog()                                                    */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT near GenerateTrailer(nItems)
  int  nItems;
{

  if (nItems == 0)
    return (ERRINT_NOCONTROL);

  GlobalUnlock(hMemory);
  // dca00024 PJL can't assume GlobalReAlloc returns the same handle it
  //              was passed, especially when using the GMEM_MOVEABLE flag.
  // GlobalReAlloc(hMemory, (DWORD)iMemory, GMEM_MOVEABLE);
  if ((hMemory = GlobalReAlloc(hMemory, (DWORD)iMemory, GMEM_MOVEABLE)) == 0)
		return(ERRINT_MEMORY);
  lpMemory = GlobalLock(hMemory);
  lpMemory[4] = (BYTE)nItems;

  return (0);
}


/******************************************************************************/
/*									      */
/*  Dialog Procedures                                                         */
/*									      */
/******************************************************************************/

BOOL DlgDlgbox(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  switch (message)
    {
    case WM_INITDIALOG:
      {
      int   nItem, nID, nClass, nVarNo;
      long  lInteger;
      LPSTR lpStr;
      char  buf[256];
      register i;

      nItem = lpMemory[4];
      i = 16;

      while (nItem--)
        {
        nID    = *(int far *)(&lpMemory[i+8]);
        nClass = (BYTE)lpMemory[i+14];
        i += 15;
        lpStr = &lpMemory[i];
        while (lpMemory[i++]);
        i++;
        nVarNo = *(int far *)(&lpMemory[i]);
        i += 2;

        switch (nClass)
          {
          case 0x80:
            if (nID > 1000)
              {
              GetIntVariable(nVarNo, &lInteger);
              CheckDlgButton(hDlg, nID, lInteger != 0L);
              }
            break;

          case 0x81:
         // GetStrVariable(nVarNo, buf, sizeof(buf));
         // SetDlgItemText(hDlg, nID, buf);
            break;

          case 0x83:
            {
            register j;

            j = 0;
            while (*lpStr)
              {
              if (*lpStr == ',')
                {
                if (j)
                  {
                  buf[j] = NULL;
                  SendDlgItemMessage(hDlg, nID, LB_ADDSTRING,
                                     NULL, (DWORD)(LPSTR)buf);
                  j = 0;
                  }
                }
              else
                buf[j++] = *lpStr;
              lpStr++;
              }
            buf[j] = NULL;
            SendDlgItemMessage(hDlg, nID, LB_ADDSTRING,
                               NULL, (DWORD)(LPSTR)buf);
            GetIntVariable(nVarNo, &lInteger);
            SendDlgItemMessage(hDlg, nID, LB_SETCURSEL, (int)lInteger, 0L);
            }
            break;
          }

        }
      if (nFocus)
        {
        SetFocus(GetDlgItem(hDlg, nFocus));
        return (FALSE);
        }
      }
      break;

    case WM_COMMAND:
      if (wParam > 0 && wParam < 1000)
	{
        int   nItem, nID, nClass, nVarNo;
        int   nPushButton;
        long  lInteger;
        char  buf[256];
        register i;

        nPushButton = 0;
        nWorkChoice = 0;
        nItem = lpMemory[4];
        i = 16;

        while (nItem--)
          {
          nID    = *(int far *)(&lpMemory[i+8]);
          nClass = (BYTE)lpMemory[i+14];
          i += 15;
          while (lpMemory[i++]);
          i++;
          nVarNo = *(int far *)(&lpMemory[i]);
          i += 2;

          switch (nClass)
            {
            case 0x80:
              if (nID > 1000)
                {
                if (wParam != ID_CANCEL)
                  {
                  lInteger = IsDlgButtonChecked(hDlg, nID);
                  SetIntVariable(nVarNo, lInteger);
                  }
                }
              else if (nID)
                {
                nPushButton++;
                if (wParam == nID)
                  nWorkChoice = nPushButton;
                }
              break;

            case 0x81:
              if (wParam != ID_CANCEL)
                {
                GetDlgItemText(hDlg, nID, buf, sizeof(buf));
                SetStrVariable(nVarNo, buf, strlen(buf));
                }
              break;

            case 0x83:
              if (wParam != ID_CANCEL)
                {
                lInteger = SendDlgItemMessage(hDlg, nID, LB_GETCURSEL, 0, 0L);
                SetIntVariable(nVarNo, lInteger+1L);
                }
              break;
            }

          }
        EndDialog(hDlg, FALSE);
        }
      break;

    default:
      return (FALSE);
      break;
    }

  return (TRUE);
}
