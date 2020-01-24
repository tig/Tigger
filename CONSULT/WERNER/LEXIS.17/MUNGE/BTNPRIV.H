/*
**	btnpriv.h  Private header used by button modules
**
**	History
**	-------
**	02/27/89	D. LeFebvre		Initial Coding.
**	
*/

/*
**	Defines
*/
#define X_SHADOW		4
#define Y_SHADOW		4

#define MAX_STEPS		1

#define BUTTON_INVERTED	0x01
#define BUTTON_DOWN		0x02
#define BUTTON_DISABLED	0x04
#define BUTTON_HIDDEN	0x08
#define GOT_FOCUS		0x10
#define DELETE_HBITMAP	0x20

#define WND_EXTRA		14

#define BUTTON_COLOR		GetWindowLong(hWnd, 0)
#define BUTTON_TEXT_COLOR	GetWindowLong(hWnd, 4)
#define BUTTON_STATUS		GetWindowWord(hWnd, 8)
#define BUTTON_BACKGROUND	GetWindowWord(hWnd, 10)
#define BUTTON_HBITMAP		GetWindowWord(hWnd, 12)
#define BUTTON_STYLE		LOWORD(GetWindowLong(hWnd, GWL_STYLE))

#define SET_COLOR(c)		SetWindowLong(hWnd, 0, c)
#define SET_TEXT_COLOR(c)	SetWindowLong(hWnd, 4, c)
#define SET_BUTTON_STATUS(c)	SetWindowWord(hWnd, 8, c)
#define SET_BUTTON_BACKGROUND(c) SetWindowWord(hWnd, 10, c)
#define SET_BUTTON_HBITMAP(c)	SetWindowWord(hWnd, 12, c)
#define SET_BUTTON_STYLE(c)	SetWindowLong(hWnd, GWL_STYLE, MAKELONG(\
				c, HIWORD(GetWindowLong(hWnd, GWL_STYLE))))

/*
**	forward references
*/
void BtnDraw(HWND hWnd, HDC hDC, int ButtonType, PRECT rect);
void BtnDrawStopSign(HWND, HDC, RECT *);
void	BtnDrawPushButton(HWND, HDC, RECT *);
void	BtnDrawEllipse(HWND, HDC, RECT *);
void	BtnDrawRect(HWND, HDC, RECT *);
void BtnDrawIcon(HWND, HDC, RECT *);
HICON BtnLoadIcon(HWND hWnd);
BOOL BtnInvert(HWND);
RECT BtnDrawHorzText(HWND hWnd, HDC hDC, PSTR szText, PRECT rect);
RECT BtnDrawVertText(HWND hWnd, HDC hDC, PSTR szText, PRECT rect);
void BtnDrawCaret(HWND hWnd, HDC hDC, PRECT rect);
RECT BtnGetTextRect(HWND hWnd, HDC hDC, PSTR szText, PRECT rect);
RECT BtnGetRect(HWND hWnd);
BOOL BtnColor(HWND hWnd, HDC hDC, HBRUSH *hBrush);
void BtnPaint(HWND hWnd, HDC hDC, PRECT rect);
void BtnDrawBitmap(HWND hWnd, HDC hDC, RECT *rect);
void BtnDisplayBitmap(HDC hDC, int left, int top, HBITMAP hBitmap);
void BtnSetBitmapSize(HWND hWnd, HBITMAP hBitmap);



