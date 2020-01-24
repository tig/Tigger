/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Macro Child (MACRO.C)                                           *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module processes the macro child window on the main        *
*             display screen.                                                 *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 08/15/89 00018 Flash the macro keys while running a script.*
*             1.01 09/05/89 00022 Make macro flash while executing a script   *
*                                   and if clicked by mouse.                  *
*             1.01 09/08/89 00033 Make for a smoother startup.                *
*	      1.01 11/07/89 00058 Reset info font for :XTALK startup.	      *
*    1.1 03/19/90 dca00052 MKL fixed function keys character set problem      *
*    1.1 03/26/90 dca00056 MKL fixed function keys character base line and    *
*	text extent problem						      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "disp.h"
#include "font.h"
#include "macro.h"
#include "interp.h"
#include "dialogs.h"

  HWND	 hChildMacro = NULL;
  struct stMacro Macro;
  char   cMacroRow[4], cMacroRows;

  static int nWidth;

  static void near DrawMacroRow(HDC, int, int, int);


long WndMacroProc(hWnd, message, wParam, lParam)
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  PAINTSTRUCT ps;
  int i, j, k;


  switch (message)
    {
    case WM_PAINT:
      {
      register row, i;

      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
      // dca000?? MKL fixed function keys background problem
      FillRect(ps.hdc, (LPRECT)&ps.rcPaint, GetStockObject(WHITE_BRUSH));
      MoveTo(ps.hdc, ps.rcPaint.left, 0);
      LineTo(ps.hdc, ps.rcPaint.right+1, 0);

      i = 0;
      for (row = 0; row < MAX_MACRO_ROW; row++)
        {
        if (cMacroRow[row])
          {
          DrawMacroRow(ps.hdc, (nSysFontHeight * 2 * i) + 1,
                       row*12, cMacroRow[row]);
          i++;
          }
        }

      EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
      }
      break;

    case WM_SIZE:
      nWidth = LOWORD(lParam);
      break;

    case WM_LBUTTONDOWN:
      {
      int row, col;
      register i;

      if ((i = HIWORD(lParam) / (nSysFontHeight * 2)) >= cMacroRows)
        i = cMacroRows - 1;
      for (row = 0; row < MAX_MACRO_ROW; row++)
        {
        if (cMacroRow[row])
          {
          if (i == 0)
            break;
          else
            i--;
          }
        }

      col = LOWORD(lParam) / (nWidth / cMacroRow[row]);

      if (Disp.fMacron)
        {
        for (i = 0; i < 12; i++)
          {
          if (Macro.szMacro[row*12+i][0] != '\0')
            {
            if (col == 0)
              break;
            else
              col--;
            }
          }
        }
      else
        i = col;

      if (i < 12)
        ActivateMacro(row*12+i);
      }
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }

  return(0L);
}


/*****************************************************************************\
*  ActivateMacro
*
*  Flash the selected macro on the screen, if shown, and execute the script if
*  the macro string begins with '@'.  Else send the string to the comm port.
\*****************************************************************************/
BOOL ActivateMacro(int macro)
  {
  RECT Rect;
  HDC  hDC;
  long lTimeOut;
  int  row, col;
  register i;


  if (IfSystemStatus(SS_SCRIPT))
    {
    FlashMacro(macro);
    InterpreterKeyboard(0x0411 + macro, TRUE);
    return (FALSE);
    }

  if (cMacroRow[macro/12]==0 || (Disp.fMacron && strlen(Macro.szMacro[macro])==0))
    {
    MessageBeep(NULL);
    return FALSE;
    }

  row = 0;
  for (i = 0; i < MAX_MACRO_ROW; i++)
    {
    if (cMacroRow[i])
      {
      if (i == macro / 12)
        break;
      else
        row++;
      }
    }

  if (Disp.fMacron)
    {
    col = 0;
    for (i = 0; i < macro % 12; i++)
      {
      if (Macro.szMacro[(macro/12)*12+i][0] != '\0')
        col++;
      }
    }
  else
    col = macro % 12;

  if (col < 12)
    {
    UpdateWindow(hChildMacro);

    Rect.top    = row * (nSysFontHeight * 2) + nSysFontHeight / 4 + 2;
    Rect.bottom = nSysFontHeight + nSysFontHeight / 2 + 1;
    Rect.left   = col * (nWidth / cMacroRow[macro/12]) + nSysFontWidth / 4;
    Rect.right  = nWidth / cMacroRow[macro/12] - nSysFontWidth / 2;

    hDC = GetDC(hChildMacro);

    SelectObject(hDC, GetStockObject(BLACK_BRUSH));
    PatBlt(hDC, Rect.left, Rect.top, Rect.right, Rect.bottom, DSTINVERT);

    lTimeOut = GetCurrentTime() + 100;
    while (lTimeOut > GetCurrentTime());

    PatBlt(hDC, Rect.left, Rect.top, Rect.right, Rect.bottom, DSTINVERT);
    ReleaseDC(hChildMacro, hDC);
    }

  if (strlen(Macro.szMacro[macro]))
    {
    if (Macro.szMacro[macro][0] == '@')
      {
      if (i = ExecuteScript(&Macro.szMacro[macro][1]))
	GeneralError(i);
      }
    else
      {
      char buf[96];

      LibDestore(Macro.szMacro[macro], buf, sizeof(buf));
      CommDataOutput(&buf[1], (int)(buf[0]));
      }
    }
  else
    {
    MessageBeep(NULL);
    return FALSE;
    }

  return TRUE;
  }

// 00018 Add FlashMacro() function to flash macro while in script.
/*****************************************************************************\
*  FlashMacro
*
*  Flash the selected macro on the screen, if shown.
\*****************************************************************************/
VOID FlashMacro(short macro)
  {
  RECT Rect;
  HDC  hDC;
  long lTimeOut;
  int  row, col;
  register i;


  if (cMacroRow[macro/12]==0 || (Disp.fMacron && strlen(Macro.szMacro[macro])==0))
    {
    MessageBeep(NULL);
    return;
    }

  row = 0;
  for (i = 0; i < MAX_MACRO_ROW; i++)
    {
    if (cMacroRow[i])
      {
      if (i == macro / 12)
        break;
      else
        row++;
      }
    }

  if (Disp.fMacron)
    {
    col = 0;
    for (i = 0; i < macro % 12; i++)
      {
      if (Macro.szMacro[(macro/12)*12+i][0] != '\0')
        col++;
      }
    }
  else
    col = macro % 12;

  if (col < 12)
    {
    UpdateWindow(hChildMacro);

    Rect.top    = row * (nSysFontHeight * 2) + nSysFontHeight / 4 + 2;
    Rect.bottom = nSysFontHeight + nSysFontHeight / 2 + 1;
    Rect.left   = col * (nWidth / cMacroRow[macro/12]) + nSysFontWidth / 4;
    Rect.right  = nWidth / cMacroRow[macro/12] - nSysFontWidth / 2;

    hDC = GetDC(hChildMacro);

    SelectObject(hDC, GetStockObject(BLACK_BRUSH));
    PatBlt(hDC, Rect.left, Rect.top, Rect.right, Rect.bottom, DSTINVERT);

    lTimeOut = GetCurrentTime() + 100;
    while (lTimeOut > GetCurrentTime());

    PatBlt(hDC, Rect.left, Rect.top, Rect.right, Rect.bottom, DSTINVERT);
    ReleaseDC(hChildMacro, hDC);
    }
  }

static void near DrawMacroRow(hDC, nRow, nOff, nAct)
  HDC hDC;
  int nRow;
  int nOff;
  int nAct;
{
  HBRUSH hBrush, hOldBrush;
  char buf[32], cArrow;
  int  nDelta, nChars, nXPos, j, k;
  register i;
  DWORD TextExt;

  /*  dca00052 MKL fixed function keys character set problem
  cArrow = 0x27;
  if (hMacroFont)
    {
    SelectObject(hDC, hMacroFont);
    cArrow = 0x18;
    }
  */


  nDelta = nWidth / nAct;
  nChars = (nDelta - nSysFontWidth) / nSysFontWidth;
  nRow++;

  /*
  SetBkMode(hDC, OPAQUE);

  nXPos = 0;
  for (i = nOff; i < nOff + nAct; i++)
    {
    SelectObject(hDC, GetStockObject(BLACK_BRUSH));
    Rectangle(hDC, nXPos + nSysFontWidth / 2,
		   nRow + nSysFontHeight / 2,
		   nXPos + nDelta,
		   nRow + nSysFontHeight * 2);

    SelectObject(hDC, GetStockObject(WHITE_BRUSH));
    Rectangle(hDC, nXPos + nSysFontWidth / 4,
		   nRow + nSysFontHeight / 4,
		   nXPos + nDelta - nSysFontWidth / 4,
		   nRow + nSysFontHeight * 2 - nSysFontHeight / 4);

    nXPos += nDelta;
    }
  */

  nXPos = 0;
  for (i = nOff; i < nOff + 12; i++)
    {
    if ((!Disp.fMacron) || Macro.szMacro[i][0] != NULL)
      {
      // dca00056 MKL fixed function keys character base line and text extent problem
      // register j;

      SetBkMode(hDC, OPAQUE);

      SelectObject(hDC, GetStockObject(BLACK_BRUSH));
      Rectangle(hDC, nXPos + nSysFontWidth / 2,
		     nRow + nSysFontHeight / 2,
		     nXPos + nDelta,
		     nRow + nSysFontHeight * 2);

      if (Macro.bColor[i] == 0)
	Macro.bColor[i] = 7;

      switch (Macro.bColor[i] & 0x0F)
	{
	case 0:
	  hBrush = CreateSolidBrush(RGB(0, 0, 0));
	  break;

	case 1:
	  hBrush = CreateSolidBrush(RGB(0, 0, 255));
	  break;

	case 2:
	  hBrush = CreateSolidBrush(RGB(0, 255, 0));
	  break;

	case 3:
	  hBrush = CreateSolidBrush(RGB(255, 0, 0));
	  break;

	case 4:
	  hBrush = CreateSolidBrush(RGB(0, 255, 255));
	  break;

	case 5:
	  hBrush = CreateSolidBrush(RGB(255, 255, 0));
	  break;

	case 6:
	  hBrush = CreateSolidBrush(RGB(255, 0, 255));
	  break;

	case 7:
	  hBrush = CreateSolidBrush(RGB(255, 255, 255));
	  break;
	}

      hOldBrush = SelectObject(hDC, hBrush);
      Rectangle(hDC, nXPos + nSysFontWidth / 4,
		     nRow + nSysFontHeight / 4,
		     nXPos + nDelta - nSysFontWidth / 4,
		     nRow + nSysFontHeight * 2 - nSysFontHeight / 4);
      SelectObject(hDC, hOldBrush);
      DeleteObject(hBrush);

      SetBkMode(hDC, TRANSPARENT);

      switch (Macro.bColor[i] / 16)
	{
	case 0:
	  SetTextColor(hDC, RGB(0, 0, 0));
	  break;

	case 1:
	  SetTextColor(hDC, RGB(0, 0, 255));
	  break;

	case 2:
	  SetTextColor(hDC, RGB(0, 255, 0));
	  break;

	case 3:
	  SetTextColor(hDC, RGB(255, 0, 0));
	  break;

	case 4:
	  SetTextColor(hDC, RGB(0, 255, 255));
	  break;

	case 5:
	  SetTextColor(hDC, RGB(255, 255, 0));
	  break;

	case 6:
	  SetTextColor(hDC, RGB(255, 0, 255));
	  break;

	case 7:
	  SetTextColor(hDC, RGB(255, 255, 255));
	  break;
	}

      memset(buf, 0, sizeof(buf));

      if (Macro.szLabel[i][0])
        {
        int nIn, nOt;

	// dca00052 MKL fixed function keys character set problem
        SelectObject(hDC, GetStockObject(SYSTEM_FONT));

        for (nIn = nOt = 0; Macro.szLabel[i][nIn]; nIn++)
          {
          if (Macro.szLabel[i][nIn] != '&' || Macro.szLabel[i][nIn+1] == '&')
            buf[nOt++] = Macro.szLabel[i][nIn];
          }
        buf[nOt] = NULL;
	}
      else
        {
	// dca00052 MKL fixed function keys character set problem
	SelectObject(hDC, hMacroFont);
	cArrow = 0x18;

        if (i < 24)
          {
          if (i < 12)
	    buf[0] = cArrow;
          else
            buf[0] = '^';

          buf[1] = 'F';
          if ((i % 12) < 9)
            {
            buf[2] = (char)('1' + (i % 12));
            buf[3] = NULL;
            }
          else
            {
            buf[2] = (char)('0' + ((i % 12)+1) / 10);
            buf[3] = (char)('0' + ((i % 12)+1) % 10);
            buf[4] = NULL;
            }
          }
        else
          {
          buf[0] = '^';
          if (i < 40)
            buf[1] = (char)('A' + i - 24);
          else if (i == 40)
            buf[1] = 'R';
          else
            buf[1] = (char)('A' + i - 22);
          buf[2] = NULL;
          }
	}

      for (j = 0; buf[j] != 0; j++);
      while (j > 0 && buf[j-1] == ' ')
	j--;

      // dca00056 MKL fixed function keys character base line and text extent problem
      // if (j > nChars && (buf[0] == '^' || buf[0] == cArrow))
      if (buf[0] == '^' || buf[0] == cArrow) {
	if (j> nChars) TextOut(hDC, nXPos + (nDelta - nChars * nSysFontWidth) / 2,
             nRow + (nSysFontHeight * 2) / 3, &buf[j-nChars], nChars);
	else {
	    if (j > nChars) j = nChars;
            TextOut(hDC, nXPos + (nDelta - j * nSysFontWidth) / 2,
                     nRow + (nSysFontHeight * 2) / 3, buf, j);
	    }
      }
      else {
	if (sVersion < 3) {
           if (j > nChars) j = nChars;
           TextOut(hDC, nXPos + (nDelta - j * nSysFontWidth) / 2,
                     nRow + (nSysFontHeight * 2) / 3-2, buf, j);
	}
	else {
	   if (j < 31) { buf[j] = ' '; buf[j+1] = 0;}
	   for (k = 1; k <= j; ++k) {
	      TextExt = GetTextExtent(hDC, buf, k);
	      if (LOWORD(TextExt) > (nDelta-6)) {
	         -- k;
	         if (k > 0) TextExt = GetTextExtent(hDC, buf, k);
	         break;
	      }
           }
	   if (k > 0) TextOut(hDC, nXPos + (nDelta - LOWORD(TextExt)) / 2,
                     nRow + (nSysFontHeight * 2) / 3 -2 , buf, k);

        }
      }
      nXPos += nDelta;
      }
    }
}


BOOL CreateMacroChild(hParent)
  HWND hParent;
{
  HWND hWnd;
  char szClassName[64];


  LoadString(hWndInst, IDS_WNDCLASS_MACRO, szClassName, sizeof(szClassName));

  hWnd = CreateWindow(szClassName,
		      NULL,
		      WS_CHILD | WS_VISIBLE,
		      0,
		      0,
		      0,
		      0,
		      hParent,
		      (HMENU)NULL,
		      (HANDLE)hWndInst,
		      NULL
		      );

  if (hWnd)
    {
    hChildMacro = hWnd;
    return TRUE;
    }
  else
    {
    hChildMacro = NULL;
    return FALSE;
    }
}


BOOL DestroyMacroChild()
{
  if (hChildMacro)
    DestroyWindow(hChildMacro);

  hChildMacro = NULL;

  return TRUE;
}

BOOL InitMacro()
{
  NewMacroData();
  return (TRUE);
}

void NewMacroData()
{
  memset(&Macro, 0, sizeof(Macro));
  cMacroRow[0] = 0;
  cMacroRow[1] = 0;
  cMacroRow[2] = 0;
  cMacroRow[3] = 0;
  cMacroRows   = 1;
  UpdateMacro();
}

void UpdateMacro()
{
  static char fMenuEmpty = FALSE;
  HMENU hMenu;
  char	buf[32];
  register int i;


  if (Disp.fMacron)
    {
    cMacroRow[0] = 0;
    for (i = 0; i < 12; i++)
      if (Macro.szMacro[i][0] != NULL)
       cMacroRow[0]++;
    }
  else
    {
    cMacroRow[0] = 12;
    }

  cMacroRow[1] = 0;
  for (i = 12; i < 24; i++)
    if (Macro.szMacro[i][0] != NULL)
     cMacroRow[1]++;
  if (!Disp.fMacron && cMacroRow[1])
    cMacroRow[1] = 12;

  cMacroRow[2] = 0;
  for (i = 24; i < 36; i++)
    if (Macro.szMacro[i][0] != NULL)
     cMacroRow[2]++;
  if (!Disp.fMacron && cMacroRow[2])
    cMacroRow[2] = 12;

  cMacroRow[3] = 0;
  for (i = 36; i < 48; i++)
    if (Macro.szMacro[i][0] != NULL)
     cMacroRow[3]++;
  if (!Disp.fMacron && cMacroRow[3])
    cMacroRow[3] = 12;

  i = 0;
  if ((Disp.fMacron && cMacroRow[0]) || !Disp.fMacron)
    i++;
  if (cMacroRow[1])
    i++;
  if (cMacroRow[2])
    i++;
  if (cMacroRow[3])
    i++;

  if (hChildMacro && (cMacroRows != (char)i))
    {
    cMacroRows = (char)i;
    InvalidateRect(hChildMacro, NULL, TRUE);
    ResetScreenDisplay();
    }
  else if (hChildMacro)
    {
    InvalidateRect(hChildMacro, NULL, TRUE);
    }

  if (hMenu = GetSubMenu(hWndMenu, 4))
    {
    register int j;

    i = GetMenuItemCount(hMenu);
    while (i-- > 0)
      ChangeMenu(hMenu, 0, NULL, NULL, MF_DELETE | MF_BYPOSITION);

    for (i = j = 0; i < 48; i++)
      {
      if (Macro.szLabel[i][0] && Macro.szMacro[i][0] && Macro.fMenu[i])
        {
        char buf[32];

        if (strchr(Macro.szLabel[i], '&') == 0)
          {
          buf[0] = '&';
          strcpy(&buf[1], Macro.szLabel[i]);
          }
        else
          strcpy(buf, Macro.szLabel[i]);

        if (j && (j % 12 == 0))
          ChangeMenu(hMenu, NULL, buf, CMD_MENUMACRO1+i, MF_APPEND | MF_MENUBREAK);
        else
          ChangeMenu(hMenu, NULL, buf, CMD_MENUMACRO1+i, MF_APPEND);
	j++;
	}
      }

    if (j == 0)
      {
      if (!fMenuEmpty)
        {
        buf[0] = 'E';
        buf[1] = 'm';
        buf[2] = 'p';
        buf[3] = 't';
        buf[4] = 'y';
        buf[5] = NULL;
        ChangeMenu(hMenu, NULL, buf, CMD_MENUNULL, MF_APPEND);
        EnableMenuItem(hWndMenu, 4, MF_GRAYED | MF_BYPOSITION);
        DrawMenuBar(hWnd);
        fMenuEmpty = TRUE;
        }
      }
    else
      {
      if (fMenuEmpty)
        {
        EnableMenuItem(hWndMenu, 4, MF_ENABLED | MF_BYPOSITION);
        DrawMenuBar(hWnd);
        fMenuEmpty = FALSE;
        }
      }
    }

  InvalidateRect(hChildMacro, NULL, TRUE);
}
