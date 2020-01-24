/*
**	Buttons.h  --  Used for special buttons
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
**	line around text, bitmap or icon when it has the input focus.
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
*/

/* Special Button Styles */
#define BUTTON_TYPE		0x07L

#define BTS_PUSHBUTTON	0L
#define BTS_ELLIPSE		1L
#define BTS_STOPSIGN	2L
#define BTS_ICON		3L
#define BTS_RECT		4L
#define BTS_BITMAP		5L

#define BTS_ANIMATE		0x10L
#define BTS_SHADOW		0x20L
#define BTS_DEFAULT		0x40L
#define BTS_VERT		0x80L
#define BTS_NOFOCUS		0x08L

BOOL  RegisterButtons(HANDLE);
BOOL  GraySubText(HDC hDC, LPSTR szText, short int nCount);
BOOL  GrayIcon(HDC, DWORD, short int);
BOOL  GrayBitmap(HDC, DWORD, short int);
long  BtnWndProc(HWND, unsigned, WORD, LONG);
DWORD SetButtonColor(HWND, DWORD);
DWORD SetButtonTextColor(HWND, DWORD);
DWORD SetButtonBitmap(HWND, HBITMAP);
