/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Macro Processor (MACRO.C)                                        */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <stdio.h>
#include "lexis.h"
#include "data.h"
#include "session.h"
#include "term.h"
#include "dialogs.h"
#include "btnlib.h"


#define ID_FIRST         1
#define ID_TRANSMIT      1
#define ID_LIBRARY       2
#define ID_NEWSEARCH     3
#define ID_CITATOR       4
#define ID_KWIC          5
#define ID_FIRST_PAGE    6
#define ID_FIRST_DOC     7
#define ID_PRINTALL      8
#define ID_FILE          9
#define ID_STOP         10
#define ID_LEXSTAT      11
#define ID_FULL         12
#define ID_NEXT_PAGE    13
#define ID_NEXT_DOC     14
#define ID_PRINT_DOC    15
#define ID_SIGNON       16
#define ID_RESUME       17
#define ID_LEXSEE       18
#define ID_CITE         19
#define ID_PREV_PAGE    20
#define ID_PREV_DOC     21
#define ID_LAST         21

#define BTN_TOTAL        (ID_LAST - ID_FIRST + 1)
#define BTN_ROWS         3
#define BTN_COLUMNS      7
#define BTN_DELTAS      27

#define WND_EXTRA       42
#define UWW_BUTTONS      0

  HWND  hChildMacro = NULL;
  short bActive = 0;
  short sActiveMacro  = 0;
  short sActiveButton = 0;
  short fOnlineSave = TRUE;


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
long far pascal MacroWndProc(hWnd, message, wParam, lParam)
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static HBRUSH hForeBrush, hBackBrush, hGreenBrush, hRedBrush;
  static int nGray1, nGray2;


  switch (message)
    {
    case UM_MANUAL:
      SetWindowText(GetWindowWord(hWnd, (ID_SIGNON-1) * 2), "Sign Off");
      InvalidateRect(GetWindowWord(hWnd, (ID_SIGNON-1) * 2), NULL, TRUE);
      break;

    case UM_ONLINE:
      SetWindowText(GetWindowWord(hWnd, (ID_SIGNON-1) * 2), "Sign On");
      InvalidateRect(GetWindowWord(hWnd, (ID_SIGNON-1) * 2), NULL, TRUE);

      if (wParam && (!fOnlineSave))
        {
        register i;

        fOnlineSave = TRUE;
        for (i = ID_FIRST; i <= ID_LAST; i++)
          {
          if (i != ID_SIGNON)
            EnableWindow(GetWindowWord(hWnd, (i-1) * 2), TRUE);
          else
            SetWindowText(GetWindowWord(hWnd, (ID_SIGNON-1) * 2), "Sign Off");
          InvalidateRect(GetWindowWord(hWnd, (i-1) * 2), NULL, TRUE);
          }
        }
      else if ((!wParam) && fOnlineSave)
        {
        register i;

        fOnlineSave = FALSE;
        for (i = ID_FIRST; i <= ID_LAST; i++)
          {
          if (i != ID_SIGNON)
            EnableWindow(GetWindowWord(hWnd, (i-1) * 2), FALSE);
          else
            SetWindowText(GetWindowWord(hWnd, (ID_SIGNON-1) * 2), "Sign On");
          InvalidateRect(GetWindowWord(hWnd, (i-1) * 2), NULL, TRUE);
          }
        }
      break;

    case UM_BUTTON:
      if (nWorkVersion < 4)
        {
        if (bActive = wParam)
          {
          SetButtonTextColor(GetWindowWord(hWnd, sActiveButton*2), RGB(255,255,255));
          SetButtonColor(GetWindowWord(hWnd, sActiveButton*2), RGB(0,0,0));
          }
        else
          {
          SetButtonTextColor(GetWindowWord(hWnd, sActiveButton*2), RGB(0,0,0));
          SetButtonColor(GetWindowWord(hWnd, sActiveButton*2), RGB(255,255,255));
          if ((sActiveButton == ID_STOP - 1) && !fMonoDisplay)
            SetButtonTextColor(GetWindowWord(hWnd, sActiveButton*2), RGB(255,0,0));
          if ((sActiveButton == ID_TRANSMIT - 1) && !fMonoDisplay)
            SetButtonColor(GetWindowWord(hWnd, sActiveButton*2), RGB(0,255,0));
          }
        }
      InvalidateRect(GetWindowWord(hWnd, sActiveButton * 2), NULL, TRUE);
      break;

    case UM_EXECUTE:
      wParam = sActiveButton + 1;

    case WM_COMMAND:
      switch (wParam)
        {
        case ID_TRANSMIT:
          TermWriteKeyboard(WM_CHAR, ASC_CR);
          break;

        case ID_LIBRARY:
          SessCanCommand(COMMAND_CHANGELIBRARY);
          break;

        case ID_NEWSEARCH:
          SessCanCommand(COMMAND_NEWSEARCH);
          break;

        case ID_CITATOR:
          CmdCitator();
          break;

        case ID_KWIC:
          SessCanCommand(COMMAND_KWIC);
          break;

        case ID_FIRST_PAGE:
          SessCanCommand(COMMAND_FIRSTPAGE);
          break;

        case ID_FIRST_DOC:
          SessCanCommand(COMMAND_FIRSTDOC);
          break;

        case ID_PRINTALL:
          CmdPrintAll();
          break;

        case ID_FILE:
          SessCanCommand(COMMAND_CHANGEFILE);
          break;

        case ID_STOP:
          SessCanCommand(COMMAND_STOP);
          break;

        case ID_LEXSTAT:
          CmdLexstat();
          break;

        case ID_FULL:
          SessCanCommand(COMMAND_FULL);
          break;

        case ID_NEXT_PAGE:
          CmdNextPage(FALSE);
          break;

        case ID_NEXT_DOC:
          CmdNextDoc(FALSE);
          break;

        case ID_PRINT_DOC:
          if (fDataFastPrint)
            {
            if (bWorkFastPrint)
              SessCanCommand(COMMAND_FASTPRINT2);
            else
              SessCanCommand(COMMAND_FASTPRINT1);
            bWorkFastPrint = TRUE;
            }
          else
            SessCanCommand(COMMAND_PRINTDOC);
          break;

        case ID_SIGNON:
          CmdSignon();
          break;

        case ID_RESUME:
          CmdResume();
          break;

        case ID_LEXSEE:
          CmdLexsee();
          break;

        case ID_CITE:
          SessCanCommand(COMMAND_CITE);
          break;

        case ID_PREV_PAGE:
          CmdPrevPage(FALSE);
          break;

        case ID_PREV_DOC:
          CmdPrevDoc(FALSE);
          break;
        }

      SetFocus(hChildTerminal);
      break;

    case WM_CREATE:
      {
      HWND    hChild;
      HBITMAP hBitmap;
      char    szClass[32], szTitle[32];
      register i;


      if (nWorkVersion > 3)
        {
        hForeBrush  = CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT));
        hBackBrush  = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
        hRedBrush   = CreateSolidBrush(RGB(255, 0, 0));

        szClass[ 0] = 'b';
        szClass[ 1] = 'u';
        szClass[ 2] = 't';
        szClass[ 3] = 't';
        szClass[ 4] = 'o';
        szClass[ 5] = 'n';
        szClass[ 6] = NULL;
        }
      else
        {
        szClass[ 0] = 'S';
        szClass[ 1] = 'p';
        szClass[ 2] = 'e';
        szClass[ 3] = 'c';
        szClass[ 4] = 'i';
        szClass[ 5] = 'a';
        szClass[ 6] = 'l';
        szClass[ 7] = 'B';
        szClass[ 8] = 'u';
        szClass[ 9] = 't';
        szClass[10] = 't';
        szClass[11] = 'o';
        szClass[12] = 'n';
        szClass[13] = NULL;
        }

      for (i = 0; i < BTN_TOTAL; i++)
        {
        LoadString(hWndInst, IDS_MACRO_FIRST + i, szTitle, sizeof(szTitle));

        hChild = CreateWindow(szClass, szTitle,
                              BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
                              0, 0, 0, 0, hWnd, ID_FIRST + i, hWndInst, NULL);

        if (hChild == 0)
          break;

        if (nWorkVersion < 4)
          {
          if ((i == ID_STOP - 1) && !fMonoDisplay)
            SetButtonTextColor(hChild, RGB(255,0,0));
          if ((i == ID_TRANSMIT - 1) && !fMonoDisplay)
            SetButtonColor(hChild, RGB(0,255,0));
          }

        SetWindowWord(hWnd, UWW_BUTTONS + i*2, hChild);
        }
      }
      break;

    case WM_DESTROY:
      if (nWorkVersion > 3)
        {
        DeleteObject(hForeBrush);
        DeleteObject(hBackBrush);
        DeleteObject(hGreenBrush);
        DeleteObject(hRedBrush);
        }
      PostQuitMessage(0);
      break;

    case WM_SYSCOLORCHANGE:
      if (nWorkVersion > 3)
        {
        DeleteObject(hForeBrush);
        DeleteObject(hBackBrush);
        hForeBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT));
        hBackBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        }
      break;

    case WM_PAINT:
      {
      PAINTSTRUCT ps;
      HBRUSH hBrush;
      RECT Rect;

      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

      Rect.top    = ps.rcPaint.top - 2;
      Rect.bottom = ps.rcPaint.bottom;
      Rect.left   = nGray1;
      Rect.right  = nGray2;
      hBrush = CreateSolidBrush(RGB(0, 0, 255));
      FillRect(ps.hdc, &Rect, hBrush);
      DeleteObject(hBrush);

      Rect.top    = ps.rcPaint.top - 2;
      Rect.bottom = ps.rcPaint.bottom;
      Rect.left   = nGray2;
      Rect.right  = ps.rcPaint.right;
      hBrush = CreateSolidBrush(RGB(255, 255, 0));
      FillRect(ps.hdc, &Rect, hBrush);
      DeleteObject(hBrush);

      MoveTo(ps.hdc, ps.rcPaint.left, 0);
      LineTo(ps.hdc, ps.rcPaint.right+1, 0);
      MoveTo(ps.hdc, ps.rcPaint.left, 1);
      LineTo(ps.hdc, ps.rcPaint.right+1, 1);

      EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
      }
      break;

    case WM_SIZE:
      {
      HWND hChild;
      int  xPos;
      int  xDelta, yDelta, nExtra;
      int  nChild;
      register row, col;

      xDelta = LOWORD(lParam) / BTN_DELTAS;
      yDelta = (HIWORD(lParam) - nSysFontHeight / 2) / BTN_ROWS;
      nExtra = (LOWORD(lParam) - xDelta * BTN_DELTAS) / (BTN_COLUMNS - 1);

      for (row = 0; row < BTN_ROWS; row++)
        {
        xPos = 2;
        for (col = 0; col < BTN_COLUMNS; col++)
          {
          if (col == 4)
            {
            MoveWindow(GetWindowWord(hWnd, (row * BTN_COLUMNS + col) * 2),
                       xPos, row * yDelta + 3 + nSysFontHeight / 4,
                       xDelta * 3 - 2, yDelta-1, TRUE);
            xPos += xDelta * 3 + nExtra;
            }
          else
            {
            MoveWindow(GetWindowWord(hWnd, (row * BTN_COLUMNS + col) * 2),
                       xPos, row * yDelta + 3 + nSysFontHeight / 4,
                       xDelta * 4 - 2, yDelta-1, TRUE);
            xPos += xDelta * 4 + nExtra;
            }

          if (col == 3)
            nGray1 = xPos - nExtra / 2 - 2;
          if (col == 4)
            nGray2 = xPos - nExtra / 2 - 2;
          }
        }
      }
      break;

    case WM_CTLCOLOR:
      if (nWorkVersion > 3)
        {
        if (bActive && LOWORD(lParam) == GetDlgItem(hWnd, sActiveMacro+1))
          {
          SetBkColor(wParam, GetSysColor(COLOR_WINDOWTEXT));
          SetTextColor(wParam, GetSysColor(COLOR_WINDOW));
          UnrealizeObject(hForeBrush);
          return ((DWORD)hForeBrush);
          }
        else if (LOWORD(lParam) == GetDlgItem(hWnd, ID_SIGNON))
          {
          SetBkColor(wParam, GetSysColor(COLOR_WINDOW));
          if (fMonoDisplay)
            SetTextColor(wParam, GetSysColor(COLOR_WINDOWTEXT));
          else
            SetTextColor(wParam, RGB(255, 0, 0));
          UnrealizeObject(hBackBrush);
          return ((DWORD)hBackBrush);
          }
        else if (LOWORD(lParam) == GetDlgItem(hWnd, ID_TRANSMIT))
          {
          SetTextColor(wParam, GetSysColor(COLOR_WINDOWTEXT));
          if (fMonoDisplay)
            SetBkColor(wParam, GetSysColor(COLOR_WINDOW));
          else
            SetBkColor(wParam, RGB(0, 255, 0));
          UnrealizeObject(hBackBrush);
          return ((DWORD)hBackBrush);
          }
        else
          return DefWindowProc(hWnd, message, wParam, lParam);
        }
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }

  return(0L);
}

/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
HWND far pascal CreateMacroChild(HWND hParent)
{
  static char fRegistered = FALSE;
  char	szClass[32];


  LoadString(hWndInst, IDS_WNDCLASS_MACRO, szClass, sizeof(szClass));

  if (!fRegistered)
    {
    WNDCLASS WndClass;


    memset(&WndClass, NULL, sizeof(WNDCLASS));

    WndClass.style         = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc   = MacroWndProc;
    WndClass.hInstance     = hWndInst;
    WndClass.hbrBackground = COLOR_MENU + 1;
    WndClass.lpszClassName = (LPSTR)szClass;
    WndClass.cbWndExtra    = WND_EXTRA;

    RegisterClass(&WndClass);  /* We just ignore the error here */
    fRegistered = TRUE;
    }

  hChildMacro = CreateWindow(szClass, NULL, WS_CHILD | WS_VISIBLE,
                             0, 0, 0, 0,
                             hParent, NULL, hWndInst, NULL);

  return (hChildMacro);
}

void far pascal DestroyMacroChild()
{
  if (hChildMacro)
    {
    DestroyWindow(hChildMacro);
    hChildMacro = NULL;
    }
}
