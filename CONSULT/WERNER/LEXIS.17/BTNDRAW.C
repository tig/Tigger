/*
**	btndraw.c - Button Draw Functions
**
**	History
**	-------
**	03/20/89	D. LeFebvre		Changed names to Btn****
**	02/27/89	D. LeFebvre		Separated from Main module
*/
#include <windows.h>
#include <string.h>
#include "btnlib.h"
#include "btnpriv.h"

void
BtnPaint(HWND hWnd, HDC hDC, PRECT rect)
{
	HBRUSH hBrush;
	BOOL DeleteBrush = FALSE;
	RECT TextRect;
	char szText[40];
	int ButtonStatus = BUTTON_STATUS;
	int ButtonStyle = BUTTON_STYLE;
	int ButtonType = (int)(ButtonStyle & BUTTON_TYPE);

  /*
  **  If we have no width then just return and save us some time.
  */
  if (rect->right == 0 || rect->bottom == 0)
    return;


	/*
	**	If inverting the button, change the colors now
	*/
	if (ButtonStatus & BUTTON_INVERTED && !(ButtonStyle & BTS_ANIMATE))
	{
		LOGBRUSH LogBrush;

		SetBkColor(hDC, ~GetBkColor(hDC));
		SetTextColor(hDC, ~GetTextColor(hDC));
		hBrush = SelectObject(hDC, GetStockObject(NULL_BRUSH));

		/*
		**	create new brush
		*/
		GetObject(hBrush, sizeof(LOGBRUSH), (LPSTR)&LogBrush);

		hBrush = CreateSolidBrush(~LogBrush.lbColor);
		SelectObject(hDC, hBrush);
		DeleteBrush = TRUE;
	}

	/*
	**	Draw The Button
	*/
	BtnDraw(hWnd, hDC, ButtonType, rect);

	/*
	**	Draw the text
	*/
	GetWindowText(hWnd, szText, sizeof(szText));

	/*
	**	if disabled, set the system font
	*/
	if (ButtonStatus & BUTTON_DISABLED)
		SelectObject(hDC, GetStockObject(SYSTEM_FONT));

	if (ButtonStyle & BTS_VERT)
		TextRect = BtnDrawVertText(hWnd, hDC, szText, rect);
	else
		TextRect = BtnDrawHorzText(hWnd, hDC, szText, rect);

	/*
	**	if we have the focus, draw line
	*/
	if (ButtonStatus & GOT_FOCUS)
		BtnDrawCaret(hWnd, hDC, rect);

	if (DeleteBrush)
	{
		SelectObject(hDC, GetStockObject(NULL_BRUSH));
		DeleteObject(hBrush);
	}

	return;
}

/*
**	Main Draw function that selects the proper draw function
*/
void
BtnDraw(HWND hWnd, HDC hDC, int ButtonType, PRECT rect)
{
	RECT NewRect;

	switch(ButtonType)
	{
	case BTS_ELLIPSE:
		BtnDrawEllipse(hWnd, hDC, rect);
		break;

	case BTS_PUSHBUTTON:
		BtnDrawPushButton(hWnd, hDC, rect);
		break;

	case BTS_RECT:
		BtnDrawRect(hWnd, hDC, rect);
		break;

	default:
		break;
	}
}

/*
**	Draw the Button Caret
*/
short Dotted[] =
{
	0x55,
	0xAA,
	0x55,
	0xAA,
	0x55,
	0xAA,
	0x55,
	0xAA
};

void
BtnDrawCaret(HWND hWnd, HDC hDC, RECT *rect)
{
	HBITMAP hBitmap = CreateBitmap(8, 8, 1, 1, (LPSTR) Dotted);
	HBRUSH hBrush = CreatePatternBrush(hBitmap);
	HBRUSH hNewBrush;
	RECT TextRect;
	char WinText[40];
	short OldBkMode = SetBkMode(hDC, TRANSPARENT);

	GetWindowText(hWnd, WinText, sizeof(WinText));

	TextRect = BtnGetTextRect(hWnd, hDC, WinText, rect);

	FrameRect(hDC, &TextRect, hBrush);

	SetBkMode(hDC, OldBkMode);
	DeleteObject(hBrush);
	DeleteObject(hBitmap);
}

/*
**	Draw PushButton
*/
void
BtnDrawPushButton(hWnd, hDC, rect)
	HWND hWnd;
	HDC hDC;
	RECT *rect;
{
	int Ellipse = max(rect->right - rect->left, rect->bottom - rect->top)/4;

	RoundRect(hDC, rect->left, rect->top, rect->right, rect->bottom,
		Ellipse, Ellipse * GetDeviceCaps(hDC, ASPECTX)
			/ GetDeviceCaps(hDC, ASPECTY));
}

/*
**	Draw Ellipse Button
*/
void
BtnDrawEllipse(hWnd, hDC, rect)
	HWND hWnd;
	HDC hDC;
	RECT *rect;
{
	Ellipse(hDC, rect->left, rect->top, rect->right, rect->bottom);
}

/*
**	Draw Rectangle Button
*/
void
BtnDrawRect(HWND hWnd, HDC hDC, PRECT rect)
{
	Rectangle(hDC, rect->left, rect->top, rect->right, rect->bottom);
}
