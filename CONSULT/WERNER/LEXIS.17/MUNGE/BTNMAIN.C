/*
**	Btnmain.c  --  Used for special buttons
**
**		Window Class "SpecialButton"
**
**		Styles
**		------
**		BTS_PUSHBUTTON		Rounded Rectangle
**		BTS_ELLIPSE		Ellipse
**		BTS_STOPSIGN		Stop Sign
**		BTS_ICON			Displays an Icon
**		BTS_RECT			Rectangle
**		BTS_BITMAP		BitMap
**
**		Options
**		-------
**		BTS_ANIMATE		Animate the push button
**		BTS_SHADOW		Drop Shadow Behind Button
**		BTS_VERT			Vertical text
**		BTS_NOFOCUS		Do not SetFocus on mouse click
**		BTS_DEFAULT		Makes button the Default Push Button
**							Similar to BS_DEFPUSHBUTTON
**
**		Window Text is used as text in the button except for Icons and
**	Bitmaps.  This text is the name of the resource.  Special Resource
**	names used for system Icons: "hand", "question", "exclamation",
**	"asterisk".
**
**		Button handles both mouse and keyboard input. Displays a dotted
**	line around text or icon when it has the input focus.
**
**		Color
**		-----
**		Color of the button can be specified in two ways:
**
**		1. Calling the functions SetButtonColor and SetButtonTextColor
**		2. By handling the WM_CTLCOLOR msg sent to the parent by the
**			button.
**
**		Bitmap
**		------
**		The displayed bitmap can be set using the window text in the
**	resource file or by calling SetButtonBitmap with the handle of the
**	desired bitmap.
**		
**		.DEF file
**		---------
**		You must export the following routines:
**
**			BtnWndProc	- The Window Proc for SpecialButton
**                      GraySubText - Call-back function to GrayString
**			GrayIcon - Call-back function to GrayString for Icons
**			GrayBitmap - Call-back function to GrayString for Bitmaps
**
**		Required Calls
**		--------------
**		RegisterButtons(hInstance) must be called before using the
**	SpecialButton Class.
**
**
**	History
**	-------
**	03/20/89	D. LeFebvre		Changed names to Btn****
**	02/26/89	D. LeFebvre		Added Vertical Buttons
**	02/10/89	D. LeFebvre		Initial Coding.
*/


#include <windows.h>
#include <string.h>
#include "btnlib.h"
#include "btnpriv.h"

/*
**	RegisterButtons
**
**	Must be the first function called
*/
BOOL FAR PASCAL
RegisterButtons(HANDLE hInstance)
{
	WNDCLASS wndClass;

	wndClass.style			= CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc	= BtnWndProc;
	wndClass.cbClsExtra		= 0;
	wndClass.cbWndExtra 	= WND_EXTRA;
	wndClass.hInstance		= hInstance;
	wndClass.hIcon			= NULL;
	wndClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground	= NULL;
	wndClass.lpszMenuName 	= NULL;
	wndClass.lpszClassName	= "SpecialButton";

	return (RegisterClass(&wndClass));
}

/*
**	SetButtonColor for the background
*/
DWORD FAR PASCAL
SetButtonColor(HWND hWnd, DWORD Color)
{
	return(SET_COLOR(Color));
}

/*
**	SetButtonTextColor for the Text
*/
DWORD FAR PASCAL
SetButtonTextColor(HWND hWnd, DWORD Color)
{
	return(SET_TEXT_COLOR(Color));
}

/*
**	Special Button Window Proc
*/
long FAR PASCAL
BtnWndProc(HWND hWnd, unsigned iMessage, WORD wParam, LONG lParam)
{
	HBRUSH hBrush;
	HPEN hPen = NULL;
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;
	DWORD TextSize;
	POINT MousePos;
	int ButtonStatus = BUTTON_STATUS;
	int ButtonStyle = BUTTON_STYLE;
	int ButtonType = (int)(ButtonStyle & BUTTON_TYPE);
	BOOL DeleteBrush = FALSE;

	switch (iMessage)
	{
	case WM_CREATE:
		/*
		**	initilize the button data
		*/
		SET_COLOR(-1L);
		SET_TEXT_COLOR(-1L);
		SET_BUTTON_STATUS(0);
		SET_BUTTON_BACKGROUND(NULL);
		if (ButtonStyle & BTS_ANIMATE)
			SET_BUTTON_STYLE(ButtonStyle |= BTS_SHADOW);

		/*
		**	if a StopSign, then make window square
		*/
		if (ButtonType == BTS_STOPSIGN)
		{
			POINT UpperLeft;
			HDC hDC = GetDC(hWnd);
			int AspectX = GetDeviceCaps(hDC, ASPECTX);
			int AspectY = GetDeviceCaps(hDC, ASPECTY);

			ReleaseDC(hWnd, hDC);

			GetClientRect(hWnd, &rect);
			UpperLeft.x = rect.left;
			UpperLeft.y = rect.top;

			ClientToScreen(hWnd, &UpperLeft);
			ScreenToClient(GetParent(hWnd), &UpperLeft);

			if (rect.right > rect.bottom * AspectX / AspectY)
			{
				MoveWindow(hWnd, UpperLeft.x, UpperLeft.y,
					rect.bottom * AspectY / AspectX,
					rect.bottom,
					FALSE);
			}
			else
			{
				MoveWindow(hWnd, UpperLeft.x, UpperLeft.y,
					rect.right,
					rect.right * AspectX / AspectY,
					FALSE);
			}
		}

		/*
		**	if shadowing, Make room for the shadow
		*/
		if (ButtonStyle & BTS_SHADOW)
		{
			POINT UpperLeft;

			GetClientRect(hWnd, &rect);
			UpperLeft.x = rect.left;
			UpperLeft.y = rect.top;

			ClientToScreen(hWnd, &UpperLeft);
			ScreenToClient(GetParent(hWnd), &UpperLeft);

			MoveWindow(hWnd, UpperLeft.x, UpperLeft.y,
				rect.right + X_SHADOW, rect.bottom + Y_SHADOW, FALSE);
		}
		break;


	case WM_DESTROY:
		if (BUTTON_BACKGROUND != NULL)
		{
			DeleteObject(BUTTON_BACKGROUND);
		}
		break;

	case WM_GETDLGCODE:
		if (ButtonStyle & BTS_DEFAULT)
			return(DLGC_BUTTON | DLGC_DEFPUSHBUTTON);
		else
			return(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);

	case WM_ENABLE:
		/*
		**	Must set flag and gray the string
		*/
		if (wParam)
		{
			SET_BUTTON_STATUS(ButtonStatus & ~BUTTON_DISABLED);
		}
		else
		{
			SET_BUTTON_STATUS(ButtonStatus | BUTTON_DISABLED);
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_SETFOCUS:
		/*
		**	Must set Flag
		*/
		SET_BUTTON_STATUS(ButtonStatus | GOT_FOCUS);

		/*
		**	Draw the caret
		*/
		hDC = GetDC(hWnd);
		rect = BtnGetRect(hWnd);

		if (ButtonStatus & BUTTON_INVERTED && (ButtonStyle & BTS_ANIMATE))
			OffsetRect(&rect, X_SHADOW, Y_SHADOW);

		DeleteBrush = BtnColor(hWnd, hDC, &hBrush);
		BtnDrawCaret(hWnd, hDC, &rect);

		if (DeleteBrush)
		{
			SelectObject(hDC, GetStockObject(NULL_BRUSH));
			DeleteObject(hBrush);
		}

		ReleaseDC(hWnd, hDC);
		break;

	case WM_KILLFOCUS:
		/*
		**	Must set Flag and trigger repaint
		*/
		SET_BUTTON_STATUS(ButtonStatus & ~GOT_FOCUS);

		if (ButtonStatus & BUTTON_DOWN)
		{
			SendMessage(hWnd, WM_LBUTTONUP, 0, 0L);
		}
		else if (ButtonStatus & BUTTON_INVERTED)
		{
			SendMessage(hWnd, WM_KEYUP, VK_SPACE, 0L);
		}

		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);
		break;

	case BM_GETSTATE:
		/*
		**	return Inverted status
		*/
		return(ButtonStatus & BUTTON_INVERTED);

	case BM_SETSTATE:

		/*
		**	Set the proper flag and trigger repaint
		*/
		if ((wParam && !(ButtonStatus & BUTTON_INVERTED))
			||
		    (!wParam && (ButtonStatus & BUTTON_INVERTED)))
		{
			BtnInvert(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
		}
		break;

	case BM_SETSTYLE:
		if (wParam)
			ButtonStyle |= BTS_DEFAULT;
		else
			ButtonStyle &= ~BTS_DEFAULT;
		SET_BUTTON_STYLE(ButtonStyle);
		break;

	case WM_ERASEBKGND:
		/*
		**	Must restore background
		*/
		GetClientRect(hWnd, &rect);

  /*
  **  If we have no width then just return and save us some time.
  */
  if (rect.right == 0 || rect.bottom == 0)
    break;

		if (BUTTON_BACKGROUND == 0)
		{
			HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
			HDC hMemDC = CreateCompatibleDC(hDC);
			HBITMAP hOldBitmap, hBitmap;
			POINT UpperLeft;

			/*
			**	Handle Shadowing
			*/
			if (ButtonStyle & BTS_SHADOW)
			{
				SetBkColor(wParam, RGB(0, 0, 0));
				SetTextColor(wParam, RGB(0, 0, 0));
				SelectObject(wParam, GetStockObject(BLACK_BRUSH));

				/*
				**	shrink the rect
				*/
				rect.left += X_SHADOW;
				rect.top  += Y_SHADOW;

				BtnDraw(hWnd, wParam, ButtonType, &rect);

				rect.left -= X_SHADOW;
				rect.top  -= Y_SHADOW;
			}

			UpperLeft.x = rect.left;
			UpperLeft.y = rect.top;

			hBitmap = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);

			hOldBitmap = SelectObject(hMemDC, hBitmap);

			ClientToScreen(hWnd, &UpperLeft);

			BitBlt(hMemDC, 0, 0, rect.right, rect.bottom,
				hDC, UpperLeft.x, UpperLeft.y, SRCCOPY);


			SET_BUTTON_BACKGROUND(hBitmap);
			SET_BUTTON_STATUS(ButtonStatus & ~BUTTON_HIDDEN);
			
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hDC);
			DeleteDC(hMemDC);
		}
		else	if (BUTTON_BACKGROUND != NULL)
		{
			HDC hMemDC;
			HBITMAP hOldBitmap, hBitmap = BUTTON_BACKGROUND;

			hDC = wParam;
			hMemDC = CreateCompatibleDC(hDC);

			hOldBitmap = SelectObject(hMemDC, hBitmap);

			BitBlt(hDC, 0, 0, rect.right, rect.bottom,
				hMemDC, 0, 0, SRCCOPY);

			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}

		break;

	case WM_PAINT:
		/*
		**	must paint the button
		*/
		hDC = BeginPaint(hWnd, &ps);
		rect = BtnGetRect(hWnd);

  /*
  **  If we have no width then just return and save us some time.
  */
  if (rect.right == 0 || rect.bottom == 0)
    {
    EndPaint(hWnd, &ps);
    break;
    }
		if (ButtonStatus & BUTTON_INVERTED && (ButtonStyle & BTS_ANIMATE))
			OffsetRect(&rect, X_SHADOW, Y_SHADOW);

		DeleteBrush = BtnColor(hWnd, hDC, &hBrush);
		BtnPaint(hWnd, hDC, &rect);
	
		
		if (DeleteBrush)
		{
			SelectObject(hDC, GetStockObject(NULL_BRUSH));
			DeleteObject(hBrush);
		}

		EndPaint(hWnd, &ps);
		break;

	/*
	**	Handle the Button Logic
	*/
	case WM_KEYDOWN:
		if (wParam != VK_SPACE || ButtonStatus & BUTTON_DOWN)
			break;

		lParam = 0L;

		/* fall through */
	case WM_LBUTTONDOWN:
		/*
		**	Process button down: capture mouse and invert button
		*/
		if (iMessage == WM_LBUTTONDOWN)
		{
			SET_BUTTON_STATUS(ButtonStatus | BUTTON_DOWN);
		}

		SetCapture(hWnd);

		if (!(ButtonStyle & BTS_NOFOCUS))
			SetFocus(hWnd);

		MousePos = MAKEPOINT(lParam);
		GetClientRect(hWnd, &rect);

		if (!PtInRect(&rect, MousePos) && ButtonStatus & BUTTON_INVERTED)
		{
			BtnInvert(hWnd);
		}
		else if (PtInRect(&rect, MousePos)
			&& !(ButtonStatus & BUTTON_INVERTED))
		{
			BtnInvert(hWnd);
		}
		break;

	case WM_KEYUP:
		if (wParam != VK_SPACE || ButtonStatus & BUTTON_DOWN)
			break;

		lParam = 0L;

		/* fall through */
	case WM_LBUTTONUP:
		/*
		**	handle button up: invert button, send message, release mouse
		*/
		/*
		**	if we didn't own mouse or cursor not on button, forget it
		*/
		if (iMessage == WM_LBUTTONUP && !(ButtonStatus & BUTTON_DOWN))
			break;

		MousePos = MAKEPOINT(lParam);
		GetClientRect(hWnd, &rect);

		SET_BUTTON_STATUS(ButtonStatus & ~BUTTON_DOWN);
		
		if (ButtonStatus & BUTTON_INVERTED)
			BtnInvert(hWnd);

		if (PtInRect(&rect, MousePos))
		{
			SendMessage(GetParent(hWnd), WM_COMMAND,
				GetWindowWord(hWnd, GWW_ID), (LONG) hWnd);
		}

		ReleaseCapture();
		break;


	case WM_MOUSEMOVE:
		/*
		**	if we don't own the mouse, ignore
		*/
		if (!(ButtonStatus & BUTTON_DOWN))
			break;

		/*
		**	if mouse leaves button, invert button
		*/
		MousePos = MAKEPOINT(lParam);
		GetClientRect(hWnd, &rect);

		if (!PtInRect(&rect, MousePos) && ButtonStatus & BUTTON_INVERTED)
		{
			BtnInvert(hWnd);
		}
		else if (PtInRect(&rect, MousePos)
			&& !(ButtonStatus & BUTTON_INVERTED))
		{
			BtnInvert(hWnd);
		}
		break;

	default:
		return(DefWindowProc(hWnd, iMessage, wParam, lParam));
	}

	return(0L);
}

/*
**	Flips the Inverted status, invalidates the hWnd, and returns the old
**	state
*/
BOOL
BtnInvert(HWND hWnd)
{
	BOOL Inverted = BUTTON_STATUS & BUTTON_INVERTED;

	/*
	**	flip the state
	*/
	SET_BUTTON_STATUS(BUTTON_STATUS ^ BUTTON_INVERTED);

	if (BUTTON_STYLE & BTS_ANIMATE)
	{
		int xShadow = X_SHADOW/MAX_STEPS;
		int yShadow = Y_SHADOW/MAX_STEPS;
		HDC hDC = GetDC(hWnd);
		HDC hMemDC = CreateCompatibleDC(hDC);
		HDC hBkMemDC = CreateCompatibleDC(hDC);
		HBITMAP hBitmap, hOldBitmap, hOldBackBitmap;
		HBRUSH hBrush;
		RECT WndRect, rect;
		BOOL DeleteBrush = FALSE;
		int i;

		GetClientRect(hWnd, &WndRect);
		hBitmap = CreateCompatibleBitmap(hDC, WndRect.right, WndRect.bottom);
		hOldBitmap = SelectObject(hMemDC, hBitmap);
		hOldBackBitmap = SelectObject(hBkMemDC, BUTTON_BACKGROUND);
		
		DeleteBrush = BtnColor(hWnd, hMemDC, &hBrush);

		rect = BtnGetRect(hWnd);

		if (BUTTON_STATUS & BUTTON_INVERTED)
		{
			for (i = 0; i < MAX_STEPS; ++i)
			{
				BitBlt(hMemDC, 0, 0, WndRect.right, WndRect.bottom,
					hBkMemDC, 0, 0, SRCCOPY);
			 	OffsetRect(&rect, xShadow, yShadow);
				BtnPaint(hWnd, hMemDC, &rect);
				BitBlt(hDC, 0, 0, WndRect.right, WndRect.bottom,
					hMemDC, 0, 0, SRCCOPY);
			}
		}
		else
		{
			OffsetRect(&rect, X_SHADOW, Y_SHADOW);

			for (i = 0; i < MAX_STEPS; ++i)
			{
				BitBlt(hMemDC, 0, 0, WndRect.right, WndRect.bottom,
					hBkMemDC, 0, 0, SRCCOPY);
			 	OffsetRect(&rect, -xShadow, -yShadow);
				BtnPaint(hWnd, hMemDC, &rect);
				BitBlt(hDC, 0, 0, WndRect.right, WndRect.bottom,
					hMemDC, 0, 0, SRCCOPY);
			}
		}
		
		if (DeleteBrush)
		{
			SelectObject(hMemDC, GetStockObject(NULL_BRUSH));
			DeleteObject(hBrush);
		}

		SelectObject(hMemDC, hOldBitmap);
		SelectObject(hBkMemDC, hOldBackBitmap);
		DeleteDC(hMemDC);
		DeleteDC(hBkMemDC);
		ReleaseDC(hWnd, hDC);
		
		DeleteObject(hBitmap);
		ValidateRect(hWnd, NULL);
	}
	else
		InvalidateRect(hWnd, NULL, FALSE);

	return(Inverted);
}

/*
**	Set the Button colors
*/
BOOL
BtnColor(HWND hWnd, HDC hDC, HBRUSH *hBrush)
{
	BOOL DeleteBrush = FALSE;
	HBRUSH hOldBrush, hNewBrush;

	/*
	**	if no brush address passed, then only want to
	**	call user color routine
	*/
	if (hBrush == NULL)
	{
		SendMessage(GetParent(hWnd), WM_CTLCOLOR,
			hDC, MAKELONG(hWnd, CTLCOLOR_BTN));

		return(FALSE);
	}

	/*
	**	Set the colors into the hDC
	*/
	*hBrush = (HBRUSH) SendMessage(GetParent(hWnd), WM_CTLCOLOR,
		hDC, MAKELONG(hWnd, CTLCOLOR_BTN));

	if (BUTTON_COLOR != -1L)
	{
		SetBkColor(hDC, BUTTON_COLOR);
		*hBrush = CreateSolidBrush(BUTTON_COLOR);
		DeleteBrush = TRUE;
	}

	if (BUTTON_TEXT_COLOR != -1L)
	{
		SetTextColor(hDC, BUTTON_TEXT_COLOR);
	}

	hOldBrush = SelectObject(hDC, *hBrush);

	return(DeleteBrush);
}
