/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Font Support Module (FONT.C)                                     */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <ctype.h>
#include "lexis.h"
#include "library.h"
#include "font.h"
#include "term.h"
#include "data.h"
#include "dialogs.h"


BOOL  DlgFonts(HWND, unsigned, WORD, LONG);
short FontEnumFonts(LPLOGFONT, LPTEXTMETRIC, short, LPSTR);
static void near pascal FormatHeightWidth(PSTR, int, int);


  HANDLE hSysFont;
  short  nSysFontWidth;
  short  nSysFontHeight;

  HANDLE hCurFont;
  short  nCurFontWidth;
  short  nCurFontHeight;
  BYTE	 bCurFontCharSet;


/*****************************************************************************/
/*  InitFont()								     */
/*									     */
/*****************************************************************************/
BOOL InitFont()
{
  TEXTMETRIC sTM;
  LOGFONT    LogFont;
  FARPROC    lpDlg;
  HANDLE     hFile;
  OFSTRUCT   sOfStruct;
  HDC  hDC;
  char szDriver[8], szNull[1], buf[32];


  szDriver[0] = 'D';
  szDriver[1] = 'I';
  szDriver[2] = 'S';
  szDriver[3] = 'P';
  szDriver[4] = 'L';
  szDriver[5] = 'A';
  szDriver[6] = 'Y';
  szDriver[7] = NULL;

  szNull[0] = NULL;

  hDC = CreateDC(szDriver, szNull, szNull, szNull);
  GetTextMetrics(hDC, (LPTEXTMETRIC)&sTM);

  memset(&LogFont, NULL, sizeof(LOGFONT));
  LogFont.lfWidth	   = sTM.tmAveCharWidth;
  LogFont.lfHeight	   = sTM.tmHeight + sTM.tmExternalLeading;
  LogFont.lfCharSet	   = OEM_CHARSET;
  LogFont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;

  hSysFont = hCurFont = CreateFontIndirect((LPLOGFONT)&LogFont);

  nSysFontWidth   = nCurFontWidth   = sTM.tmAveCharWidth;
  nSysFontHeight  = nCurFontHeight  = sTM.tmHeight + sTM.tmExternalLeading;
  bCurFontCharSet = OEM_CHARSET;

  FreeProcInstance(lpDlg);
  DeleteDC(hDC);

  NewFontData();

  return TRUE;
}


/*****************************************************************************/
/*  TermFont()								     */
/*									     */
/*****************************************************************************/
void TermFont()
{
  if (hSysFont != hCurFont)
    DeleteObject(hCurFont);

  DeleteObject(hSysFont);
}


/*****************************************************************************/
/*  NewFontData()							     */
/*									     */
/*****************************************************************************/
void NewFontData()
{
  szDataFace[0] = 'T';
  szDataFace[1] = 'E';
  szDataFace[2] = 'R';
  szDataFace[3] = 'M';
  szDataFace[4] = 'I';
  szDataFace[5] = 'N';
  szDataFace[6] = 'A';
  szDataFace[7] = 'L';
  szDataFace[8] = NULL;

  wDataCharset      = OEM_CHARSET;

  wDataPitchFamily = FIXED_PITCH | FF_MODERN;
  wDataHeight         = (char)nSysFontHeight;
  wDataWidth          = (char)nSysFontWidth;

  if (hChildTerminal)
    UpdateFont();
}


/*****************************************************************************/
/*  UpdateFont()							     */
/*									     */
/*****************************************************************************/
void UpdateFont()
{
  TEXTMETRIC sTM;
  LOGFONT    sLogFont;
  HFONT      hFont;


  if (szDataFace[0] == '\0')
    NewFontData();

  memset(&sLogFont, NULL, sizeof(LOGFONT));
  strcpy(sLogFont.lfFaceName, szDataFace);
  sLogFont.lfPitchAndFamily = (char)wDataPitchFamily;
  sLogFont.lfCharSet	    = (char)wDataCharset;
  sLogFont.lfHeight         = wDataHeight;
  sLogFont.lfWidth          = wDataWidth;
  sLogFont.lfQuality	    = DEFAULT_QUALITY;
  sLogFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
  sLogFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;


  SelectObject(hTerminalDC, hSysFont);
  if (hCurFont != hSysFont)
    DeleteObject(hCurFont);
  hCurFont = CreateFontIndirect((LPLOGFONT)&sLogFont);
  SelectObject(hTerminalDC, hCurFont);

  GetTextMetrics(hTerminalDC, (LPTEXTMETRIC)&sTM);

  nCurFontWidth   = sTM.tmAveCharWidth;
  nCurFontHeight  = sTM.tmHeight + sTM.tmExternalLeading;
  bCurFontCharSet = sTM.tmCharSet;

  UpdateTerminalFont();
}


void CmdFonts()
{
  FARPROC lpDlg;

  lpDlg = MakeProcInstance((FARPROC)DlgFonts, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_FONTS), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
}

BOOL DlgFonts(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static HANDLE hSaveMem;
  FARPROC lpDlg;
  LPSTR   lpStr;
  char	  buf[32];
  register int i;


  switch (message)
    {
    case WM_INITDIALOG:
      hSaveMem = NULL;

      lpDlg = MakeProcInstance((FARPROC)FontEnumFonts, hWndInst);
      EnumFonts(hTerminalDC, NULL, lpDlg, (LPSTR)(MAKELONG(1,hDlg)));
      SendDlgItemMessage(hDlg, ID_FONT_FONTLIST, LB_SELECTSTRING, -1,
                         (DWORD)(LPSTR)szDataFace);
      SetDlgItemText(hDlg, ID_FONT_FONTTEXT, szDataFace);

      EnumFonts(hTerminalDC, szDataFace, lpDlg, (LPSTR)(MAKELONG(2,hDlg)));
      FormatHeightWidth(buf, wDataHeight, wDataWidth);
      SendDlgItemMessage(hDlg, ID_FONT_SIZELIST, LB_SELECTSTRING, -1, (DWORD)(LPSTR)buf);
      SetDlgItemText(hDlg, ID_FONT_SIZETEXT, buf);
      EnumFonts(hTerminalDC, szDataFace, lpDlg, (LPSTR)(MAKELONG(3, hDlg)));
      FreeProcInstance(lpDlg);

      return TRUE;
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_REPLACE:
        //if (hSaveMem == 0)
        //  {
        //  if (hSaveMem = GlobalAlloc(GMEM_FIXED, (DWORD)sizeof(Font)))
        //    {
        //    lpStr = GlobalLock(hSaveMem);
        //    lmemcpy(lpStr, (LPSTR)&Font, sizeof(Font));
        //    GlobalUnlock(hSaveMem);
        //    }
        //  }
	case ID_OK:
          GetDlgItemText(hDlg, ID_FONT_FONTTEXT, szDataFace,
                         sizeof(szDataFace));

	  GetDlgItemText(hDlg, ID_FONT_SIZETEXT, buf, sizeof(buf));
          wDataHeight = wDataWidth = 0;
	  i = 0;
	  while (buf[i] == ' ')
	    i++;
	  while (isdigit(buf[i]))
            wDataHeight = wDataHeight * 10 + buf[i++] - '0';
	  if (buf[i] == 'x' || buf[i] == 'X')
	    {
	    i++;
	    while (isdigit(buf[i]))
              wDataWidth = wDataWidth * 10 + buf[i++] - '0';
	    }

          if (wDataHeight == 0)
            wDataHeight = (char)nCurFontHeight;
	  else if (wDataHeight >= 100)
	    wDataHeight = 99;

          if (wDataWidth == 0)
            wDataWidth = (char)nCurFontWidth;
	  else if (wDataWidth >= 100)
	    wDataWidth = 99;

	  GetDlgItemText(hDlg, ID_FONT_TYPE, buf, sizeof(buf));
	  if (buf[0] == 'A')
            wDataCharset = 0;
	  else if (buf[0] == 'O')
            wDataCharset = 255;
	  else if (buf[0] == 'V')
            wDataCharset = 128;

	  UpdateFont();

	  if (wParam == ID_OK)
	    {
            DataUpdate();
	    EndDialog(hDlg, TRUE);
	    }
	  break;

	case ID_CANCEL:
      //  if (hSaveMem)
      //    {
      //    lpStr = GlobalLock(hSaveMem);
      //    lmemcpy((LPSTR)&Font, lpStr, sizeof(Font));
      //    GlobalUnlock(hSaveMem);
      //    GlobalFree(hSaveMem);
      //    UpdateTerminalFont();
      //    }
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_FONT_FONTLIST:
	  if (HIWORD(lParam) == LBN_SELCHANGE)
	    {
	    i = (int)SendDlgItemMessage(hDlg, ID_FONT_FONTLIST, LB_GETCURSEL, 0, 0L);
	    if (i != LB_ERR)
	      {
	      SendDlgItemMessage(hDlg, ID_FONT_FONTLIST, LB_GETTEXT, i, (DWORD)(LPSTR)buf);
	      SetDlgItemText(hDlg, ID_FONT_FONTTEXT, buf);

	      lpDlg = MakeProcInstance((FARPROC)FontEnumFonts, hWndInst);
	      SendDlgItemMessage(hDlg, ID_FONT_SIZELIST, LB_RESETCONTENT, 0, 0L);
	      SendDlgItemMessage(hDlg, ID_FONT_SIZELIST, WM_SETREDRAW, 0, 0L);
	      EnumFonts(hTerminalDC, buf, lpDlg, (LPSTR)(MAKELONG(2, hDlg)));
	      SendDlgItemMessage(hDlg, ID_FONT_SIZELIST, WM_SETREDRAW, 1, 0L);
	      InvalidateRect(GetDlgItem(hDlg, ID_FONT_SIZELIST), NULL, FALSE);
	      UpdateWindow(GetDlgItem(hDlg, ID_FONT_SIZELIST));
	      EnumFonts(hTerminalDC, buf, lpDlg, (LPSTR)(MAKELONG(3, hDlg)));
	      GetDlgItemText(hDlg, ID_FONT_SIZETEXT, buf, sizeof(buf));
	      SendDlgItemMessage(hDlg, ID_FONT_SIZELIST, LB_SELECTSTRING, -1, (DWORD)(LPSTR)buf);
	      FreeProcInstance(lpDlg);
	      }
	    else
	      {
	      GetDlgItemText(hDlg, ID_FONT_FONTLIST, buf, sizeof(buf));
	      SendDlgItemMessage(hDlg, ID_FONT_FONTTEXT, LB_SELECTSTRING, -1, (DWORD)(LPSTR)buf);
	      }
	    }
	  break;

	case ID_FONT_SIZELIST:
	  if (HIWORD(lParam) == LBN_SELCHANGE)
	    {
	    i = (int)SendDlgItemMessage(hDlg, ID_FONT_SIZELIST, LB_GETCURSEL, 0, 0L);
	    if (i != LB_ERR)
	      {
	      SendDlgItemMessage(hDlg, ID_FONT_SIZELIST, LB_GETTEXT, i, (DWORD)(LPSTR)buf);
	      SetDlgItemText(hDlg, ID_FONT_SIZETEXT, buf);
	      }
	    }
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}


short FontEnumFonts(lpLogFont, lpTextMetric, nFontType, lpData)
  LPLOGFONT    lpLogFont;
  LPTEXTMETRIC lpTextMetric;
  short        nFontType;
  LPSTR        lpData;
{
  char buf[16];

  if (LOWORD(lpData) == 1)
    {
    if ((lpLogFont->lfPitchAndFamily & 0x03) == FIXED_PITCH)
      {
      SendDlgItemMessage(HIWORD(lpData), ID_FONT_FONTLIST, LB_ADDSTRING,
			 0, (DWORD)lpLogFont->lfFaceName);
      }
    }
  else if (LOWORD(lpData) == 2)
    {
    FormatHeightWidth(buf, lpLogFont->lfHeight, lpLogFont->lfWidth);
    SendDlgItemMessage(HIWORD(lpData), ID_FONT_SIZELIST, LB_ADDSTRING, 0, (DWORD)(LPSTR)buf);
    }
  else
    {
    if (lpLogFont->lfCharSet == 0)
      {
      buf[0] = 'A';
      buf[1] = 'N';
      buf[2] = 'S';
      buf[3] = 'I';
      buf[4] = 0;
      }
    else if (lpLogFont->lfCharSet == 255)
      {
      buf[0] = 'O';
      buf[1] = 'E';
      buf[2] = 'M';
      buf[3] = 0;
      }
    else if (lpLogFont->lfCharSet == 128)
      {
      buf[0] = 'V';
      buf[1] = 'T';
      buf[2] = '1';
      buf[3] = '0';
      buf[4] = '2';
      buf[5] = 0;
      }
    else
      {
      buf[0] = 'U';
      buf[1] = 'N';
      buf[2] = 'K';
      buf[3] = 'W';
      buf[4] = 0;
      }

    SetDlgItemText(HIWORD(lpData), ID_FONT_TYPE, buf);
    return (FALSE);
    }

  return (TRUE);
}

static void near pascal FormatHeightWidth(pStr, nHeight, nWidth)
  PSTR pStr;
  int  nHeight;
  int  nWidth;
{
  if (nHeight >= 10)
    *pStr++ = (char)('0' + nHeight / 10);
  else
    *pStr++ = ' ';
  *pStr++ = (char)('0' + nHeight % 10);

  *pStr++ = 'x';

  if (nWidth >= 10)
    *pStr++ = (char)('0' + nWidth / 10);
  *pStr++ = (char)('0' + nWidth % 10);

  *pStr++ = 0;
}
