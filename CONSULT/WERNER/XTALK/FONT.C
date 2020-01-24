/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Font Logic and Dialog (FONT.C)                                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   XTALK.FON is our special font file that we look for.            *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.											   *
*  1.01 10/31/89 00052 Add context help to the menus and dialogs.			   *
*  1.01 11/07/89 00058 Reset info font for :XTALK startup.					   *
*  1.1  04/01/90 dca00064 PJL Set appropriate font when loading terminals.    *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "font.h"
#include "term.h"
#include "prodata.h"
#include "dialogs.h"


BOOL  DlgFonts(HWND, unsigned, WORD, LONG);
short FontEnumFonts(LPLOGFONT, LPTEXTMETRIC, short, LPSTR);
static void near FormatHeightWidth(PSTR, int, int);

  struct stFont Font;

  HANDLE hSysFont;
  short  nSysFontWidth;
  short  nSysFontHeight;

  HANDLE hCurFont;
  short  nCurFontWidth;
  short  nCurFontHeight;
  BYTE	 bCurFontCharSet;

  HANDLE hInfoFont  = 0;
  HANDLE hMacroFont = 0;

  char	 fVT102Font;


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

	fVT102Font = FALSE;
	lpDlg = MakeProcInstance((FARPROC)FontEnumFonts, hWndInst);
	EnumFonts(hDC, NULL, lpDlg, 0L);

	if (!fVT102Font)
	{
		buf[0] = 'V';
		buf[1] = 'T';
		buf[2] = '1';
		buf[3] = '0';
		buf[4] = '2';
		buf[5] = '.';
		buf[6] = 'F';
		buf[7] = 'O';
		buf[8] = 'N';
		buf[9] = NULL;
		if ((hFile = OpenFile(buf, &sOfStruct, OF_READ)) != -1)
		{
			DosClose(hFile);
			AddFontResource(buf);
			fVT102Font = FALSE;
			EnumFonts(hDC, NULL, lpDlg, 0L);
		}
    }

	FreeProcInstance(lpDlg);

	memset(&LogFont, NULL, sizeof(LOGFONT));
	LogFont.lfWidth	   = sTM.tmAveCharWidth;
	LogFont.lfHeight	   = sTM.tmHeight + sTM.tmExternalLeading;
	LogFont.lfCharSet	   = OEM_CHARSET;
	LogFont.lfQuality        = DEFAULT_QUALITY;
	LogFont.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;

	hSysFont = hCurFont = CreateFontIndirect((LPLOGFONT)&LogFont);

	nSysFontWidth   = nCurFontWidth   = sTM.tmAveCharWidth;
	nSysFontHeight  = nCurFontHeight  = sTM.tmHeight + sTM.tmExternalLeading;
	bCurFontCharSet = OEM_CHARSET;

	fVT102Font = FALSE;
	lpDlg = MakeProcInstance((FARPROC)FontEnumFonts, hWndInst);
	EnumFonts(hDC, NULL, lpDlg, 0L);

	if (!fVT102Font)
	{
		buf[0] = 'V';
		buf[1] = 'T';
		buf[2] = '1';
		buf[3] = '0';
		buf[4] = '2';
		buf[5] = '.';
		buf[6] = 'F';
		buf[7] = 'O';
		buf[8] = 'N';
		buf[9] = NULL;
		if ((hFile = OpenFile(buf, &sOfStruct, OF_READ)) != -1)
		{
			DosClose(hFile);
			AddFontResource(buf);
			fVT102Font = FALSE;
			EnumFonts(hDC, NULL, lpDlg, 0L);
		}
	}

	FreeProcInstance(lpDlg);
	DeleteDC(hDC);

	NewFontData(TRUE);

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


// dca00064 PJL Set appropriate font when loading a new terminal.
ResetFont(BOOL use_vt102)
{

	if (use_vt102)
	{
		Font.szFaceName[ 0] = 'V';
		Font.szFaceName[ 1] = 'T';
		Font.szFaceName[ 2] = '1';
		Font.szFaceName[ 3] = '0';
		Font.szFaceName[ 4] = '2';
		Font.szFaceName[ 5] = '/';
		Font.szFaceName[ 6] = 'V';
		Font.szFaceName[ 7] = 'T';
		Font.szFaceName[ 8] = '5';
		Font.szFaceName[ 9] = '2';
		Font.szFaceName[10] = NULL;
		Font.cCharSet		= 128;
	}
	else
	{
		Font.szFaceName[0]	= 'T';
		Font.szFaceName[1]	= 'E';
		Font.szFaceName[2]	= 'R';
		Font.szFaceName[3]	= 'M';
		Font.szFaceName[4]	= 'I';
		Font.szFaceName[5]	= 'N';
		Font.szFaceName[6]	= 'A';
		Font.szFaceName[7]	= 'L';
		Font.szFaceName[8]	= NULL;
		Font.cCharSet		= OEM_CHARSET;
	}

	Font.cPitchAndFamily	= FIXED_PITCH | FF_MODERN;
	Font.cHeight			= (char)nSysFontHeight;
	Font.cWidth				= (char)nSysFontWidth;
}


/*****************************************************************************/
/*  NewFontData()							     */
/*									     */
/*****************************************************************************/
void NewFontData()
{

	ResetFont(fVT102Font);
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

	memset(&sLogFont, NULL, sizeof(LOGFONT));
	strcpy(sLogFont.lfFaceName, Font.szFaceName);
	sLogFont.lfPitchAndFamily = Font.cPitchAndFamily;
	sLogFont.lfCharSet	    = Font.cCharSet;
	sLogFont.lfHeight	    = Font.cHeight;
	sLogFont.lfWidth	    = Font.cWidth;
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

	InfoMessage(MSGINF_HELP);
	wHelpDialog = HELP_ITEM_FONTS;

	lpDlg = MakeProcInstance((FARPROC)DlgFonts, hWndInst);
	DialogBox(hWndInst, MAKEINTRESOURCE(IDD_FONTS), hWnd, lpDlg);
	FreeProcInstance(lpDlg);

	wHelpDialog = NULL;
	InfoMessage(NULL);
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
							(DWORD)(LPSTR)Font.szFaceName);
		SetDlgItemText(hDlg, ID_FONT_FONTTEXT, Font.szFaceName);

		EnumFonts(hTerminalDC, Font.szFaceName, lpDlg, (LPSTR)(MAKELONG(2,hDlg)));
		FormatHeightWidth(buf, Font.cHeight, Font.cWidth);
		SendDlgItemMessage(hDlg, ID_FONT_SIZELIST, LB_SELECTSTRING, -1, (DWORD)(LPSTR)buf);
		SetDlgItemText(hDlg, ID_FONT_SIZETEXT, buf);
		EnumFonts(hTerminalDC, Font.szFaceName, lpDlg, (LPSTR)(MAKELONG(3, hDlg)));
		FreeProcInstance(lpDlg);

		return TRUE;
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_REPLACE:
			if (hSaveMem == 0)
			{
				if (hSaveMem = GlobalAlloc(GMEM_FIXED, (DWORD)sizeof(Font)))
				{
					lpStr = GlobalLock(hSaveMem);
					lmemcpy(lpStr, (LPSTR)&Font, sizeof(Font));
					GlobalUnlock(hSaveMem);
				}
			}

		case ID_OK:
			GetDlgItemText(hDlg, ID_FONT_FONTTEXT, Font.szFaceName,
							sizeof(Font.szFaceName));

			GetDlgItemText(hDlg, ID_FONT_SIZETEXT, buf, sizeof(buf));
			Font.cHeight = Font.cWidth = 0;

			i = 0;
			while (buf[i] == ' ')
				i++;

			while (isdigit(buf[i]))
				Font.cHeight = Font.cHeight * 10 + buf[i++] - '0';

			if (buf[i] == 'x' || buf[i] == 'X')
			{
				i++;
				while (isdigit(buf[i]))
					Font.cWidth = Font.cWidth * 10 + buf[i++] - '0';
			}

			if (Font.cHeight == 0)
				Font.cHeight = (char)nCurFontHeight;
			if (Font.cWidth == 0)
				Font.cWidth = (char)nCurFontWidth;

			GetDlgItemText(hDlg, ID_FONT_TYPE, buf, sizeof(buf));
			if (buf[0] == 'A')
				Font.cCharSet = 0;
			else if (buf[0] == 'O')
				Font.cCharSet = 255;
			else if (buf[0] == 'V')
				Font.cCharSet = 128;

			UpdateFont();

			if (wParam == ID_OK)
			{
				fDataChanged = TRUE;
				EndDialog(hDlg, TRUE);
			}
			break;

		case ID_CANCEL:
			if (hSaveMem)
			{
				lpStr = GlobalLock(hSaveMem);
				lmemcpy((LPSTR)&Font, lpStr, sizeof(Font));
				GlobalUnlock(hSaveMem);
				GlobalFree(hSaveMem);
				UpdateTerminalFont();
			}
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

	if (LOWORD(lpData) == 0)
	{
		if (lpLogFont->lfFaceName[0] == 'V' && lpLogFont->lfFaceName[1] == 'T' &&
			lpLogFont->lfFaceName[2] == '1' && lpLogFont->lfFaceName[3] == '0' &&
		lpLogFont->lfFaceName[4] == '2' && lpLogFont->lfFaceName[5] == '/' &&
		lpLogFont->lfFaceName[6] == 'V' && lpLogFont->lfFaceName[7] == 'T' &&
		lpLogFont->lfFaceName[8] == '5' && lpLogFont->lfFaceName[9] == '2')
		fVT102Font = TRUE;
	}
	else if (LOWORD(lpData) == 1)
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


static void near FormatHeightWidth(pStr, nHeight, nWidth)
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
