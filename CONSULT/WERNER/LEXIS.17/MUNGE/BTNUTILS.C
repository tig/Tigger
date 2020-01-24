/*
**	BtnUtils.c  --  Utility routines for buttons
**
**	History
**	-------
**	03/20/89	D. LeFebvre		Changed names to Btn****
**	02/28/89	D. LeFebvre		Initial Coding.
*/


#include <windows.h>
#include <string.h>
#include "btnlib.h"
#include "btnpriv.h"


/*
**	GetTextRect calculates the surrounding box of the text
*/
RECT
BtnGetTextRect(HWND hWnd, HDC hDC, PSTR szText, PRECT rect)
{
	RECT TextRect;
	int ButtonStyle = BUTTON_STYLE;
	int ButtonType = ButtonStyle & BUTTON_TYPE;
	DWORD TextSize = GetTextExtent(hDC, szText, (short) strlen(szText));
	int StrLength = strchr(szText, '&') ? strlen(szText) - 1
		: strlen(szText);


	if (ButtonType == BTS_ICON)
	{
		TextRect.left   = rect->left;
		TextRect.top    = rect->top;
		TextRect.right  = TextRect.left + GetSystemMetrics(SM_CXICON);
		TextRect.bottom = TextRect.top + GetSystemMetrics(SM_CYICON);
	}
	else if (ButtonType == BTS_BITMAP)
	{
		TextRect = *rect;
	}
	else	if (ButtonStyle & BTS_VERT)
	{
		int TextHeight = HIWORD(TextSize) + 1;
		int TextWidth  = LOWORD(GetTextExtent(hDC, "0", 1));

		TextHeight *= StrLength;
	
		TextHeight = min(TextHeight, rect->bottom - rect->top - 2);

		TextRect.left   = (rect->left + rect->right + 1 - TextWidth)/2;
		TextRect.top    = (rect->top + rect->bottom + 1 - TextHeight)/2;
		TextRect.right  = TextRect.left + TextWidth;
		TextRect.bottom = TextRect.top + TextHeight;
		InflateRect(&TextRect, 1, 1);
	}
	else
	{
		int TextHeight = HIWORD(TextSize);
		int TextWidth  = LOWORD(TextSize);

		/*
		**	subtract size of '&' which will disappear
		*/
		if (strchr(szText, '&'))
		{
			DWORD AmpSize = GetTextExtent(hDC, "&", 1);

			TextWidth -= LOWORD(AmpSize);
		}

		TextRect.left   = (rect->left + rect->right + 1 - TextWidth)/2;
		TextRect.top    = (rect->top + rect->bottom + 1 - TextHeight)/2;
		TextRect.right  = TextRect.left + TextWidth;
		TextRect.bottom = TextRect.top + TextHeight;
		InflateRect(&TextRect, 1, 1);
	}

	return(TextRect);
}

/*
**	GetButtonRect returns Client Area for the Button
*/
RECT
BtnGetRect(HWND hWnd)
{
	RECT rect;

	GetClientRect(hWnd, &rect);

	/*
	**	compensate for the shadow
	*/
	if (BUTTON_STYLE & BTS_SHADOW)
	{
		/*
		**	shrink the rect
		*/
		rect.right  -= X_SHADOW;
		rect.bottom -= Y_SHADOW;
	}

	return(rect);
}

/*
**	Draw Text Horizontally with possible Graying
**
**	returns a RECT
*/
RECT
BtnDrawHorzText(HWND hWnd, HDC hDC, PSTR szText, PRECT rect)
  {
  DWORD TextSize;
  RECT  TextRect;
  short TextWidth, TextHeight;


	TextRect = BtnGetTextRect(hWnd, hDC, szText, rect);
	
	/*
	**	Draw The Text
	*/
	if (BUTTON_STATUS & BUTTON_DISABLED)
	{
          //    FARPROC lpfnGraySubText = MakeProcInstance(GraySubText,
          //            GetWindowWord(hWnd, GWW_HINSTANCE));

          TextSize = GetTextExtent(hDC, szText, strlen(szText));
          TextWidth = LOWORD(TextSize);
          TextHeight = HIWORD(TextSize);

          //    GrayString(hDC, GetStockObject(BLACK_BRUSH), lpfnGraySubText,
          //            (DWORD) szText, strlen(szText), TextRect.left,
          //            TextRect.top, rect->right - rect->left,
          //            rect->bottom - rect->top);

                GrayString(hDC, GetStockObject(BLACK_BRUSH), 0L,
			(DWORD)((LPSTR)szText), 0, TextRect.left,
                        TextRect.top, TextWidth, TextHeight);

          //    FreeProcInstance(lpfnGraySubText);
	}
	else
	{
		DrawText(hDC, szText, -1, rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	return(TextRect);
}


/*
**	Draw Text Vertically with possible Graying
**
**	returns a RECT
*/
RECT
BtnDrawVertText(HWND hWnd, HDC hDC, register PSTR szText, PRECT rect)
{
	RECT TextRect;
	RECT TempRect; 
	DWORD TextSize = GetTextExtent(hDC, szText, strlen(szText));
	register int CurrentTop;
	register int TextHeight = HIWORD(TextSize);
	register int TextWidth = LOWORD(TextSize);
	int StrLength = strchr(szText, '&') ? strlen(szText) - 1
		: strlen(szText);
        FARPROC lpfnGraySubText;
	
	if (TextHeight * StrLength > rect->bottom - rect->top - 2)
	{
		TextHeight = (rect->bottom - rect->top - 2) / StrLength;
		SetBkMode(hDC, TRANSPARENT);
	}
	else
		++TextHeight;

	TextRect = BtnGetTextRect(hWnd, hDC, szText, rect);

	/*
	**	prepare call back function if graying is needed
	*/
	if (BUTTON_STATUS & BUTTON_DISABLED)
	{
                lpfnGraySubText = MakeProcInstance(GraySubText,
			GetWindowWord(hWnd, GWW_HINSTANCE));
	}
	
	/*
	**	Draw The Text
	*/
	CurrentTop = TextRect.top;
	TempRect = *rect;

	for (; *szText != '\0'; ++szText)
	{
		if (BUTTON_STATUS & BUTTON_DISABLED)
		{
                        GrayString(hDC, GetStockObject(BLACK_BRUSH), lpfnGraySubText,
				(DWORD) szText, *szText == '&' ? 2 : 1, TextRect.left,
				CurrentTop, TextWidth, TextHeight);
		}
		else
		{
			if (*szText == '&')
			{
				TempRect.top = CurrentTop;
				DrawText(hDC, szText, 2, &TempRect, DT_CENTER);
			}
			else
				TextOut(hDC, TextRect.left, CurrentTop, szText, 1);
		}

		CurrentTop += TextHeight;
		if (*szText == '&')
			++szText;
	}


	if (BUTTON_STATUS & BUTTON_DISABLED)
	{
                FreeProcInstance(lpfnGraySubText);
	}

	return(TextRect);
}

/*
**	Call Back Function used by GrayString
*/
BOOL FAR PASCAL
GraySubText(HDC hDC, LPSTR szText, short int nCount)
{
	DWORD TextSize = GetTextExtent(hDC, szText, nCount);
	int TextWidth = LOWORD(TextSize);
	int TextHeight = HIWORD(TextSize);
	RECT rect;

	SetRect(&rect, 0, 0, TextWidth, TextHeight);

	SetBkMode(hDC, TRANSPARENT);
	DrawText(hDC, szText, nCount, &rect, 0);

	return(TRUE);
}
