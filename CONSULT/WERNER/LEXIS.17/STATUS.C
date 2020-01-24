/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Status Child Window (STATUS.C)                                   */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: The status child window displays the current client, library,    */
/*           file and service.                                                */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <string.h>
#include <time.h>
#include "lexis.h"
#include "library.h"
#include "data.h"
#include "diary.h"
#include "dialogs.h"
#include "term.h"    /* debug */


  HWND hChildStatus = NULL;


BOOL CreateStatusChild(hParent)
  HWND hParent;
{
  HWND hWnd;
  char szClassName[64];


  LoadString(hWndInst, IDS_WNDCLASS_STATUS, szClassName, sizeof(szClassName));

  hWnd = CreateWindow(szClassName,
		      NULL,
                      WS_CHILD | WS_VISIBLE | CS_HREDRAW,
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
    hChildStatus = hWnd;
    return TRUE;
    }
  else
    {
    hChildStatus = NULL;
    return FALSE;
    }
}


BOOL DestroyStatusChild()
{
  if (hChildStatus)
    DestroyWindow(hChildStatus);

  hChildStatus = NULL;

  return TRUE;
}


long StatusWndProc(hWnd, message, wParam, lParam)
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  switch (message)
    {
    case WM_CREATE:
      {
      memset(szWorkClient, NULL, sizeof(szWorkClient));
      memset(szWorkLibrary, NULL, sizeof(szWorkLibrary));
      memset(szWorkService, NULL, sizeof(szWorkService));
      }
      break;

    case UM_CLIENT:
      {
      if (wParam)
	{
	if (stricmp(szWorkClient, (PSTR)wParam) == 0)
	  break;
	strncpy(szWorkClient, (PSTR)wParam, sizeof(szWorkClient)-1);
	strupr(szWorkClient);
	if (*(PSTR)wParam)
	  DiaryUpdate();
	}
      else
	szWorkClient[0] = '\0';
      InvalidateRect(hWnd, NULL, TRUE);
      }
      break;

    case UM_LIB:
      {
      if (wParam)
	{
	if (stricmp(szWorkLibrary, (PSTR)wParam) == 0)
	  break;
	strncpy(szWorkLibrary, (PSTR)wParam, sizeof(szWorkLibrary)-1);
	strupr(szWorkLibrary);
	if (*(PSTR)wParam)
	  DiaryUpdate();
	}
      else
	szWorkLibrary[0] = '\0';
      InvalidateRect(hWnd, NULL, TRUE);
      }
      break;

    case UM_FILE:
      {
      PSTR pStr;
      register i;

      if (pStr = strchr(szWorkLibrary, '/'))
        *pStr = NULL;
      if (wParam)
        {
        i = strlen(szWorkLibrary);
        szWorkLibrary[i++] = '/';
        strncpy(&szWorkLibrary[i], (PSTR)wParam, sizeof(szWorkLibrary)-i-1);
        strupr(szWorkLibrary);
        if (*(PSTR)wParam)
          DiaryUpdate();
        }
      InvalidateRect(hWnd, NULL, TRUE);
      }
      break;

    case UM_SERVICE:
      {
      strncpy(szWorkService, (PSTR)wParam, sizeof(szWorkService)-1);
      strupr(szWorkService);
      InvalidateRect(hWnd, NULL, TRUE);
      }
      break;

    case UM_DISCONNECT:
      szWorkClient[0]  = NULL;
      szWorkLibrary[0] = NULL;
      szWorkService[0] = NULL;
      InvalidateRect(hWnd, NULL, TRUE);
      break;

    case WM_PAINT:
      {
      PAINTSTRUCT ps;
      RECT  Rect, Area;
      DWORD rgbColor;
      short cClient, cService;
      short cxClient, cxService;


      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

      cClient  = strlen(szWorkClient);
      cService = strlen(szWorkService);
      cxClient  = LOWORD(GetTextExtent(ps.hdc, "Client: ", 10));
      cxClient += LOWORD(GetTextExtent(ps.hdc, szWorkClient, cClient));
      cxService = LOWORD(GetTextExtent(ps.hdc, "XXXXXXXX", 8));

      GetClientRect(hWnd, &Rect);
      GetClientRect(hWnd, &Area);

      Area.right = Rect.left = Rect.right - cxService - nSysFontWidth;
      MoveTo(ps.hdc, Rect.left, 0);
      LineTo(ps.hdc, Rect.left, Rect.bottom);
      if (cService)
        DrawText(ps.hdc, szWorkService, cService, &Rect,
                 DT_SINGLELINE | DT_CENTER | DT_VCENTER);

      GetClientRect(hWnd, &Rect);
      Area.right = Rect.left = cxClient + nSysFontWidth * 2;
      Area.left  = nSysFontWidth / 2;
      DrawText(ps.hdc, "Client: ", 8, &Area, DT_SINGLELINE | DT_VCENTER);
      Area.left += LOWORD(GetTextExtent(ps.hdc, "Client: ", 8));
      rgbColor = SetTextColor(ps.hdc, RGB(0, 0, 255));
      DrawText(ps.hdc, szWorkClient, cClient, &Area, DT_SINGLELINE | DT_VCENTER);
      SetTextColor(ps.hdc, rgbColor);

      if (cClient == 0)
        Rect.left += nSysFontWidth * 8;

      MoveTo(ps.hdc, Rect.left, 0);
      LineTo(ps.hdc, Rect.left, Rect.bottom);

      Rect.left += nSysFontWidth / 2;
      if (fWorkHyperRomMode)
	{
        DrawText(ps.hdc, "HyperRom/Lib/File: ", 19, &Rect, DT_SINGLELINE | DT_VCENTER);
	Rect.left += LOWORD(GetTextExtent(ps.hdc, "Lib/File: ", 19));
	}
      else
	{
	DrawText(ps.hdc, "Lib/File: ", 10, &Rect, DT_SINGLELINE | DT_VCENTER);
	Rect.left += LOWORD(GetTextExtent(ps.hdc, "Lib/File: ", 10));
	}
      rgbColor = SetTextColor(ps.hdc, RGB(0, 0, 255));
      DrawText(ps.hdc, szWorkLibrary, strlen(szWorkLibrary), &Rect,
               DT_SINGLELINE | DT_VCENTER);
      SetTextColor(ps.hdc, rgbColor);

      MoveTo(ps.hdc, ps.rcPaint.left, Rect.bottom-2);
      LineTo(ps.hdc, ps.rcPaint.right, Rect.bottom-2);
      MoveTo(ps.hdc, ps.rcPaint.left, Rect.bottom-1);
      LineTo(ps.hdc, ps.rcPaint.right, Rect.bottom-1);

      EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
      }
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }

  return(0L);
}
