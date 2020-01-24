/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Term Emulator Module (TERM.C)                                    */
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
#include "term.h"
#include "font.h"
#include "record.h"
#include "report.h"
#include "comm.h"
#include "data.h"
#include "session.h"
#include "dialogs.h"


static void near pascal CreateFonts();
static void near pascal DeleteFonts();
static void near pascal UpdateFonts();
static void near pascal CharInput(unsigned char *, int);
static BOOL near pascal CharOut(int);
static int  near pascal EscapeSeq(unsigned char *, int);

static BOOL near pascal RefreshLine(int, int, int);
static void near pascal RefreshStr(int, int, PSTR, int, int);
static void near pascal SetCurColor(char);
static void near ClrInsertMode();
static void near SetInsertMode();
static void near LineWrap(SHORT sLine);

         BOOL fTermInsert;
         BOOL fTermNoDisplay;
         BOOL fTermPrinting;
         BOOL fTermCancel;

  HFONT hTermFonts[4];


void TermInit()
  {
  char szBuf[16];

  nTermChrX   = 1;
  nTermChrY   = 0;
  cTermAttr   = 0;

  nSaveChrX   = 1;
  nSaveChrY   = 0;
  nSaveAttr   = 0;

  nTermOffX   = 0;
  nTermOffY   = 0;
  nTermExtX   = TERM_MAXCOL;
  nTermExtY   = TERM_MAXROW;

  fTermInsert    = FALSE;
  fTermNoDisplay = FALSE;
  fTermPrinting  = FALSE;

  CreateFonts();

  SendMessage(hChildTerminal, UM_RESET, 0, 0L);
  }


void TermTerm()
  {
  DeleteFonts();
  }


static void near pascal CreateFonts()
{
  int i;

  for (i = 0; i < sizeof(hTermFonts) / sizeof(HFONT); i++)
    hTermFonts[i] = NULL;

  UpdateFonts();
}

static void near pascal DeleteFonts()
{
  int i;

  SelectObject(hTerminalDC, hSysFont);
  cTermFontAttr = -1;


  for (i = 0; i < sizeof(hTermFonts) / sizeof(HFONT); i++)
    {
    if (hTermFonts[i] != NULL)
      {
      DeleteObject(hTermFonts[i]);
      hTermFonts[i] = NULL;
      }
    }
}

static void near pascal UpdateFonts()
{
  LOGFONT LogFont;
  char buf[64];

  DeleteFonts();

  GetObject(hCurFont, sizeof(LOGFONT), (LPSTR)&LogFont);

  LogFont.lfHeight    = nCurFontHeight;
  LogFont.lfWidth     = nCurFontWidth;
  LogFont.lfCharSet   = bCurFontCharSet;

  LogFont.lfWeight    = 400;
  LogFont.lfUnderline = FALSE;
  hTermFonts[0] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 700;
  hTermFonts[1] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 400;
  LogFont.lfUnderline = TRUE;
  hTermFonts[2] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 700;
  hTermFonts[3] = CreateFontIndirect((LPLOGFONT)&LogFont);
}


long TermWndProc(HWND hWnd, unsigned message, WORD wParam, LONG lParam )
  {
  PAINTSTRUCT ps;
  HBRUSH hBrush;
  int	 i, x, y;
  int	 nX1, nX2, nY1, nY2;
  char	 szBuf[1];


  switch (message)
    {
    case UM_RESET:
      nTermChrX = 1;
      nTermChrY = 0;
      nSaveChrX = 1;
      nSaveChrY = 0;
      bTermEscape = FALSE;

      SelectObject(hTerminalDC, hTermFonts[0]);
      cTermAttr = 0x70;
      nSaveAttr = cTermAttr;
      cTermFontAttr = -1;
      cTermFontColor = -1;
      bTermRevr = FALSE;

      if (wParam)
        {
        if (bWorkLogonSent)
          VidClearArea(1, 21, TERM_MAXCOL, TERM_MAXROW-1);
        }
      else
        VidClearPage();

      VidSetCaretPos();
      break;

    case UM_NEWFONT:
      UpdateFonts();
      break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
      if (TermWriteFunction(wParam, lParam) == 0)
        return DefWindowProc(hWnd, message, wParam, lParam);
      break;

    case WM_CHAR:
      TermWriteKeyboard(WM_CHAR, wParam);
      break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
      if (wSessKeyboard == KEYBD_LOCAL)
        MouseSelectMsg(hWnd, message, wParam, lParam);
      break;

    case WM_LBUTTONDBLCLK:
      if (wSessKeyboard != KEYBD_LOCAL || !fCommConnected)
        break;

      if (fSelectedArea)
        MouseSelectClr();

      y = HIWORD(lParam) / nCurFontHeight + nTermOffY;
      x = LOWORD(lParam) / nCurFontWidth + nTermOffX + 1;

      if (y >= TERM_MAXROW)
	break;

      VidClickAndPick(x, y, TRUE);
      break;

    case WM_RBUTTONDOWN:
      if (wSessKeyboard != KEYBD_LOCAL)
        break;

      nTermChrY = HIWORD(lParam) / nCurFontHeight + nTermOffY;
      nTermChrX = LOWORD(lParam) / nCurFontWidth + nTermOffX + 1;
      if (nTermChrY >= TERM_MAXROW)
        nTermChrY = TERM_MAXROW - 1;
      if (nTermChrX > TERM_MAXCOL)
        nTermChrX = TERM_MAXCOL;
      VidSetCaretPos();
      break;

    case WM_SETFOCUS:
      fTermFocusActive = TRUE;
      VidCreateCaret(hWnd);
      VidSetCaretPos();
      ShowCaret(hWnd);
      fTermCaretActive = TRUE;
      break;

    case WM_KILLFOCUS:
      HideCaret(hWnd);
      fTermCaretActive = FALSE;
      VidDeleteCaret(hWnd);
      fTermFocusActive = FALSE;
      break;

    case WM_PAINT:
      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

      hBrush = CreateSolidBrush(ChrAttrColor[TERM_ATTR_WHITE]);
      FillRect(ps.hdc, &ps.rcPaint, hBrush);
      DeleteObject(hBrush);

      /*------------------------------------------------------------------------*/
      /*  Compute the client screen area in characters and set limits.		*/
      /*------------------------------------------------------------------------*/

      nX1 = ps.rcPaint.left / nCurFontWidth + nTermOffX + 1;
      nX2 = (ps.rcPaint.right - 1) / nCurFontWidth + nTermOffX + 1;
      if (nX1 < 1) nX1 = 1;
      if (nX1 > TERM_MAXCOL) nX1 = TERM_MAXCOL;
      if (nX2 < 1) nX2 = 1;
      if (nX2 > TERM_MAXCOL) nX2 = TERM_MAXCOL;

      nY1 = ps.rcPaint.top / nCurFontHeight + nTermOffY;
      if (ps.rcPaint.top == ps.rcPaint.bottom)
	nY2 = nY1;
      else
	nY2 = (ps.rcPaint.bottom - 1) / nCurFontHeight + nTermOffY;

      for (i = nY1; i <= nY2; i++)
	{
	if (!RefreshLine(i, nX1, nX2))
	  break;
	}

      if (fSelectedArea)
	VidSelectArea(nSelectX1, nSelectY1, nSelectX2, nSelectY2);

      EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
      break;

    case WM_HSCROLL:
    case WM_VSCROLL:
      VidScroll(message, wParam, lParam);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }

  return(0L);
  }


static BOOL near pascal RefreshLine(int nRow, int nX1, int nX2)
  {
  PINT	pLine;
  char	cCurAttr, cCurBuff[VID_MAXCOL+1];
  int	nCol, nInx, fBlk, nBlk;


  if (nRow > TERM_MAXROW - 1)
    return (NULL);

  pLine = GetVidLine(nRow);

  for (nCol = nX1, nInx = 0; nCol <= nX2; nCol++)
    {
    if (' ' == LOBYTE(pLine[nCol]))
      {
      if (' ' == LOBYTE(pLine[nCol+1]) && nCol+1 <= nX2)
	{
        if (' ' == LOBYTE(pLine[nCol+2]) && nCol+2 <= nX2)
	  fBlk = TRUE;
	else
	  fBlk = FALSE;
	}
      else
	fBlk = FALSE;
      }
    else
      fBlk = FALSE;

    if (cCurAttr == (char)HIBYTE(pLine[nCol]) && !fBlk && nInx)
      {
      cCurBuff[nInx++] = LOBYTE(pLine[nCol]);
      }
    else
      {
      if (nInx)
	RefreshStr(nRow, nCol - nInx, cCurBuff, cCurAttr, nInx);

      nInx = 0;
      if ((pLine[nCol] & ~(TERM_ATTR_BOLD << 8)) != ((TERM_ATTR_WHITE << 12) | ' '))
	{
	cCurAttr = (char)HIBYTE(pLine[nCol]);
	cCurBuff[nInx++] = LOBYTE(pLine[nCol]);
	}
      }
    }

  if (nInx)
    RefreshStr(nRow, nCol - nInx, cCurBuff, cCurAttr, nInx);

  return TRUE;
  }


static void near pascal RefreshStr(nRow, nCol, pText, nAttr, nLen)
  int  nRow, nCol;
  PSTR pText;
  int  nAttr;
  int  nLen;
{
  RECT Rect;
  int  nXPos, nYPos;
  int  nDelta[VID_MAXCOL+1];
  int  i;


  /*
  nType = *GetVidLine(nRow);
  */

  i = 0;
  if (nAttr & TERM_ATTR_BOLD)
    i += 1;
  if (nAttr & TERM_ATTR_UNDERLINE)
    i += 2;


  if (i != cTermFontAttr)
    {
    cTermFontAttr = (char)i;
    if (SelectObject(hTerminalDC, hTermFonts[cTermFontAttr]) == NULL)
      SelectObject(hTerminalDC, hCurFont);
    }


  if (((i = nAttr) & 0x77) != cTermFontColor)
    SetCurColor((char)i);

  nXPos = (nCol - nTermOffX - 1) * nCurFontWidth;
  nYPos = (nRow - nTermOffY) * nCurFontHeight;

  for (i = 0; i < nLen; i++)
    nDelta[i] = nCurFontWidth;

  Rect.top    = nYPos;
  Rect.left   = nXPos;
  Rect.bottom = nYPos + nCurFontHeight;
  Rect.right  = nXPos + nCurFontWidth * nLen;

  ExtTextOut(hTerminalDC, nXPos, nYPos, 0x04, (LPRECT)&Rect, pText, nLen, (LPINT)nDelta);
}


static BOOL near pascal CharOut(nChar)
  int  nChar;
{
  PINT	pLine;
  PINT	pChar;
  RECT	Rect;
  int	nXPos, nYPos, i;


  nChar &= 0x7F;

  pLine = pPriVidLine[nTermChrY];
  pChar = pLine + nTermChrX;

  if (nChar != NULL)
    {
    nChar = (nChar & 0x00FF) | cTermAttr << 8;

    if (*pChar == nChar)
      return TRUE;

    *pChar = nChar;
    }
  else if (*pChar == 0x7020)
    {
    return TRUE;
    }
  else
    nChar = *pChar;

  i = 0;
  if (HIBYTE(nChar) & TERM_ATTR_BOLD)
    i += 1;
  if (HIBYTE(nChar) & TERM_ATTR_UNDERLINE)
    i += 2;

  if (i != cTermFontAttr)
    {
    cTermFontAttr = (char)i;
    SelectObject(hTerminalDC, hTermFonts[cTermFontAttr]);
    }

  if ((i = (HIBYTE(nChar) & 0x77)) != cTermFontColor)
    SetCurColor((char)i);


  nXPos = (nTermChrX - nTermOffX - 1) * nCurFontWidth;
  nYPos = (nTermChrY - nTermOffY) * nCurFontHeight;

  TextOut(hTerminalDC, nXPos, nYPos, (LPSTR)pChar, 1);

  return TRUE;
}


static void near pascal SetCurColor(cColor)
  char cColor;
{
  DWORD dwForeground;
  DWORD dwBackground;


  cTermFontColor = cColor;

  switch ((cColor >> 4) & 0x07)
    {
    case TERM_ATTR_BLACK:
      dwForeground = ChrAttrColor[TERM_ATTR_WHITE];
      break;

    case TERM_ATTR_RED:
      dwForeground = RGB(255,0,0);
      break;

    case TERM_ATTR_GREEN:
      dwForeground = RGB(0,255,0);
      break;

    case TERM_ATTR_YELLOW:
      dwForeground = RGB(255,255,0);
      break;

    case TERM_ATTR_BLUE:
      dwForeground = RGB(0,0,255);
      break;

    case TERM_ATTR_MAGENTA:
      dwForeground = RGB(255,0,255);
      break;

    case TERM_ATTR_CYAN:
      dwForeground = RGB(0,255,255);
      break;

    case TERM_ATTR_WHITE:
      dwForeground = ChrAttrColor[TERM_ATTR_BLACK];
      break;
    }

  switch (cColor & 0x07)
    {
    case TERM_ATTR_BLACK:
      dwBackground = ChrAttrColor[TERM_ATTR_WHITE];
      break;

    case TERM_ATTR_RED:
      dwBackground = RGB(255,0,0);
      break;

    case TERM_ATTR_GREEN:
      dwBackground = RGB(0,255,0);
      break;

    case TERM_ATTR_YELLOW:
      dwBackground = RGB(255,255,0);
      break;

    case TERM_ATTR_BLUE:
      dwBackground = RGB(0,0,255);
      break;

    case TERM_ATTR_MAGENTA:
      dwBackground = RGB(255,0,255);
      break;

    case TERM_ATTR_CYAN:
      dwBackground = RGB(0,255,255);
      break;

    case TERM_ATTR_WHITE:
      dwBackground = ChrAttrColor[TERM_ATTR_BLACK];
      break;
    }

  if (!bTermRevr)
    {
    SetTextColor(hTerminalDC, dwForeground);
    SetBkColor(hTerminalDC, dwBackground);
    }
  else
    {
    SetTextColor(hTerminalDC, dwBackground);
    SetBkColor(hTerminalDC, dwForeground);
    }
}

extern cdecl sprintf();

short TermWriteFunction(WORD wKey, DWORD dwKey)
  {
  /*
  if (fDataLexis2000)
    {
    switch (wKey)
      {
      case VK_F1:
	if (GetKeyState(VK_SHIFT) >= 0)
	  SessCanCommand(COMMAND_GOTOSTARPAGE);
	break;

      case VK_F2:
	if (GetKeyState(VK_SHIFT) >= 0)
	  SessCanCommand(COMMAND_STAR);
	else
	  SessCanCommand(COMMAND_XSTAR);
	break;

      case VK_F3:
	if (GetKeyState(VK_SHIFT) >= 0)
	  SessCanCommand(COMMAND_PAGES);
	break;

      case VK_F4:
	if (GetKeyState(VK_SHIFT) >= 0)
	  SessCanCommand(COMMAND_BROWSE);
	break;

      case VK_F5:
	if (GetKeyState(VK_SHIFT) >= 0)
	  SessCanCommand(COMMAND_AUTOCITE);
	else
	  SessCanCommand(COMMAND_RESUMEAUTOCITE);
	break;

      case VK_F6:
	if (GetKeyState(VK_SHIFT) >= 0)
	  SessCanCommand(COMMAND_SHEPARD);
	else
	  SessCanCommand(COMMAND_RESUMESHEPARD);
	break;

      case VK_F7:
	if (GetKeyState(VK_SHIFT) >= 0)
	  SessCanCommand(COMMAND_LEXSEE);
	else
	  SessCanCommand(COMMAND_RESUMELEXSEE);
	break;

      case VK_F8:
	if (GetKeyState(VK_SHIFT) >= 0)
	  SessCanCommand(COMMAND_LXSTAT);
	else
	  SessCanCommand(COMMAND_RESUMELXSTAT);
	break;

      case VK_F9:
	if (GetKeyState(VK_SHIFT) >= 0)
	  SessCanCommand(COMMAND_RESUMELEXIS);
	break;

      case VK_F10:
	if (GetKeyState(VK_SHIFT) >= 0)
	  SessCanCommand(COMMAND_SELECTSERVICE);
	else
	  SessCanCommand(COMMAND_EXITSERVICE);
	break;
      }
    }
  else
  */
    {
    if (dwKey & 0x20000000L)
      return (0);

    switch (wKey)
      {
      case VK_CANCEL:
        CmdBreak(500);
        break;

      case VK_PRIOR:
	if (GetKeyState(VK_SHIFT) < 0)
          SessIntCommand(COMMAND_PREVDOC, -1);
	else
          SessIntCommand(COMMAND_PREVPAGE, -1);
	break;

      case VK_NEXT:
	if (GetKeyState(VK_SHIFT) < 0)
          SessIntCommand(COMMAND_NEXTDOC, -1);
	else
          SessIntCommand(COMMAND_NEXTPAGE, -1);
	break;

      case VK_F1:
	if (GetKeyState(VK_SHIFT) < 0)
	  SessCanCommand(COMMAND_SELECTSERVICE);
        else if (GetKeyState(VK_CONTROL) < 0)
          MessageBeep(NULL);
	else
	  SessIntCommand(COMMAND_NEXTPAGE, -1);
	break;

      case VK_F2:
        if (GetKeyState(VK_SHIFT) < 0)
	  SessCanCommand(COMMAND_EXITSERVICE);
        else if (GetKeyState(VK_CONTROL) < 0)
          CmdRecord();
	else
	  SessIntCommand(COMMAND_PREVPAGE, -1);
	break;

      case VK_F3:
	if (GetKeyState(VK_SHIFT) < 0)
	  {
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
	  }
        else if (GetKeyState(VK_CONTROL) < 0)
          MessageBeep(NULL);
	else
	  SessIntCommand(COMMAND_NEXTDOC, -1);
	break;

      case VK_F4:
	if (GetKeyState(VK_SHIFT) < 0)
	  SessCanCommand(COMMAND_PRINTALL);
        else if (GetKeyState(VK_CONTROL) < 0)
          MessageBeep(NULL);
	else
	  SessIntCommand(COMMAND_PREVDOC, -1);
	break;

      case VK_F5:
	if (GetKeyState(VK_SHIFT) < 0)
	  SessCanCommand(COMMAND_PRINTSCREEN);
        else if (GetKeyState(VK_CONTROL) < 0)
          MessageBeep(NULL);
	else
	  SessCanCommand(COMMAND_KWIC);
	break;

      case VK_F6:
	if (GetKeyState(VK_SHIFT) < 0)
	  SessCanCommand(COMMAND_FIRSTPAGE);
        else if (GetKeyState(VK_CONTROL) < 0)
          MessageBeep(NULL);
	else
	  SessCanCommand(COMMAND_FULL);
	break;

      case VK_F7:
	if (GetKeyState(VK_SHIFT) < 0)
	  SessCanCommand(COMMAND_FIRSTDOC);
        else if (GetKeyState(VK_CONTROL) < 0)
          MessageBeep(NULL);
	else
	  SessCanCommand(COMMAND_CITE);
	break;

      case VK_F8:
	if (GetKeyState(VK_SHIFT) < 0)
	  SessIntCommand(COMMAND_DISPDIF, -1);
        else if (GetKeyState(VK_CONTROL) < 0)
          MessageBeep(NULL);
	else
	  SessCanCommand(COMMAND_NEWSEARCH);
	break;

      case VK_F9:
	if (GetKeyState(VK_SHIFT) < 0)
	  SessIntCommand(COMMAND_VARKWIC, -1);
        else if (GetKeyState(VK_CONTROL) < 0)
          CmdSignon();
	else
	  SessCanCommand(COMMAND_CHANGEFILE);
	break;

      case VK_F10:
        if (GetKeyState(VK_SHIFT) < 0)
	  SessCanCommand(COMMAND_SEGMENTS);
        else if (GetKeyState(VK_CONTROL) < 0)
	  SessCanCommand(COMMAND_STOP);
	else
	  SessCanCommand(COMMAND_CHANGELIBRARY);
	return (1);
	break;
      }
    }

  return (0);
}

void TermWriteKeyboard(wMsg, wKey)
  WORD wMsg;
  WORD wKey;
{

  switch (wSessKeyboard)
    {
    case KEYBD_SESSION:
      SessTellKeyboard(wMsg, wKey);
      break;

    case KEYBD_DIRECT:
      {
      if (wMsg == WM_CHAR)
        {
        if (wKey == 3)
          CmdSignon();
        else if (wKey > 0 && wKey <= 127)
          CommWriteChar((BYTE)wKey);
        }
      }
      break;

    case KEYBD_LOCAL:
      {
      if (wMsg == WM_KEYDOWN)
        {
        switch (wKey)
          {
          case VK_ESCAPE:
            InvalidateRect(hChildTerminal, NULL, FALSE);
            break;

          case VK_HOME:
            ClrInsertMode();
            if (fSelectedArea)
              {
              nTermChrX = nSelectX2;
              nTermChrY = nSelectY2;
              MouseSelectClr();
              }
            nTermChrX = 1;
            nTermChrY = 0;
            VidSeeMe(nTermChrY);
            VidSetCaretPos();
            break;

          case VK_END:
            {
            register i;

            ClrInsertMode();
            if (fSelectedArea)
              {
              nTermChrX = nSelectX2;
              nTermChrY = nSelectY2;
              MouseSelectClr();
              }
            nTermChrX = 1;
            nTermChrY = 0;
            for (i = nTermChrY; i < nTermChrY+4; i++)
              {
              if ((nTermChrX = TermQueryLength(i) + 1) > 1)
                {
                nTermChrY = i;
                break;
                }
              }
            if (nTermChrX > 80)
              nTermChrX = 80;

            VidSeeMe(nTermChrY);
            VidSetCaretPos();
            }
            break;

          case VK_UP:
            ClrInsertMode();
            if (nTermChrY > 0)
              {
              nTermChrY--;
              VidSeeMe(nTermChrY);
              VidSetCaretPos();
              }
            break;

          case VK_DOWN:
            ClrInsertMode();
            if (nTermChrY < TERM_MAXROW - 1)
              {
              nTermChrY++;
              VidSeeMe(nTermChrY);
              VidSetCaretPos();
              }
            break;

          case VK_LEFT:
          //ClrInsertMode();
            if (nTermChrX > 1)
              {
              nTermChrX--;
              VidSetCaretPos();
              }
            break;

          case VK_RIGHT:
          //ClrInsertMode();
            if (nTermChrX < TERM_MAXCOL)
              {
              nTermChrX++;
              VidSetCaretPos();
              }
            break;

          case VK_DELETE:
            VidDeleteChars();
            break;

          case VK_INSERT:
            if (fTermInsert)
              ClrInsertMode();
            else
              SetInsertMode();
            break;
          }
        }
      else if (wMsg == WM_CHAR)
        {
        if (wKey < ' ' && wKey > 127)
          return;

        if (wKey == ASC_CR)
          {
          int  nXPos, nYPos;
          BYTE buf[2000], bChar;
          register i;

          if (!fSessOnline)
            {
            MessageBeep(0);
            return;
            }

          if (GetKeyState(VK_SHIFT) < 0)
            {
            buf[0] = NULL;
            SessTellCommand(buf, 0);
            }
          else
            {
            if ((i = TermQuerySelection(buf)) == 0)
              {
              for (i = 0, nXPos = 1, nYPos = 0; i < sizeof(buf)-1;)
                {
                if (nTermChrX == nXPos && nTermChrY == nYPos)
                  break;

                bChar = (BYTE)(*(pPriVidLine[nYPos] + nXPos));

                if (bChar != ' ')
                  buf[i++] = bChar;
                else
                  {
                  if (i)
                    {
                    if (buf[i-1] != ' ')
                      buf[i++] = bChar;
                    }
                  }

                if (nXPos++ >= TERM_MAXCOL)
                  {
                  nXPos = 1;
                  nYPos++;
                  }
                }
              }

            buf[i] = NULL;
            SessTellCommand(buf, i);
            }
          }
        else
          {
          BYTE buf[1];

          if (fSelectedArea)
            MouseSelectClr();

          if (fTermInsert && wKey != ASC_BS)
            {
            HideCaret(hChildTerminal);
            VidInsertChars(1);
            ShowCaret(hChildTerminal);
            }

          if (fTermInsert && wKey == ASC_BS && nTermChrX > 1)
            {
            HideCaret(hChildTerminal);
            nTermChrX--;
            VidDeleteChars();
            ShowCaret(hChildTerminal);
            VidSetCaretPos();
            }
          else
            {
            buf[0] = (BYTE)wKey;
            TermWriteData(buf, 1, TRUE);
            }
          }
        }
      }
      break;

    case KEYBD_LOCKED:
      if (wMsg != WM_KEYDOWN && wKey != VK_F10)
	MessageBeep(0);
      break;
    }
}

void TermWriteData(PBYTE pbBuffer, SHORT cbBuffer, BOOL fKey)
  {
  int  i;


  if (cbBuffer <= 0)
    return;

  if (fTermCaretActive)
    HideCaret(hChildTerminal);

  while (cbBuffer > 0)
    {
    if (bTermEscape)
      {
      i = EscapeSeq(pbBuffer, cbBuffer);

      pbBuffer += i;
      cbBuffer -= i;
      continue;
      }

    switch (*pbBuffer)
      {
      case ASC_NUL:
        break;

      case ASC_STX:
        if (wSessKeyboard != KEYBD_DIRECT && wSessKeyboard != KEYBD_LOCKED)
          fTermNoDisplay = TRUE;
	break;

      case ASC_ETX:
        if (!fTermPrinting && fCommCarrier)
          {
//#ifdef HYPERROM
//	    CommWriteData(".PSM\r", 5);
//#else
	  CommWriteData(".WIN160\r", 8);
//#endif
          fTermNoDisplay = FALSE;
          SessTellOnLexis();
          }
	break;

      case ASC_ENQ:
	break;

      case ASC_BEL:
	MessageBeep(NULL);
	break;

      case ASC_BS:
      case ASC_DEL:
	if (nTermChrX > 1)
          {
	  nTermChrX--;
          CharOut(' ');
          }
	break;

      case ASC_HT:
        if (fTermNoDisplay || fTermPrinting)
          break;

	nTermChrX = (((nTermChrX - 1) / 8) + 1) * 8 + 1;
        if (nTermChrX > TERM_MAXCOL)
          nTermChrX = TERM_MAXCOL;
	break;

      case ASC_CR:
        if (fTermNoDisplay || fTermPrinting)
          break;

        nTermChrX = 1;
        break;

      case ASC_LF:
        if (fTermNoDisplay || fTermPrinting)
          break;

        if (nTermChrY == TERM_MAXROW - 1)
          VidScrollUp(0, TERM_MAXROW - 1);
	else
	  nTermChrY++;

	if (Term.fNewLine)
	  nTermChrX = 1;
	break;

      case ASC_ESC:
        if (fSelectedArea)
          MouseSelectClr();

        ClrInsertMode();

        bTermEscape    = TRUE;
        nTermInx       = 0;
        fTermNoDisplay = FALSE;
	break;

      default:
        if (fTermNoDisplay || fTermPrinting)
          break;

        if (nTermChrX > TERM_MAXCOL)
	  {
	  nTermChrX = 1;
          if (nTermChrY == TERM_MAXROW - 1)
            VidScrollUp(0, TERM_MAXROW - 1);
	  else
	    nTermChrY++;
	  }

        CharOut(*pbBuffer);
	nTermChrX++;

        /*
        ** Add in magical word wrap feature.
        */
        if (fKey && nTermChrX == 81 && *pbBuffer != ' ')
          LineWrap(nTermChrY);
        else
          {
          if (wSessKeyboard == KEYBD_LOCAL && nTermChrX > TERM_MAXCOL)
            {
            nTermChrX = 1;
            if (nTermChrY == TERM_MAXROW - 1)
              VidScrollUp(0, TERM_MAXROW - 1);
            else
              nTermChrY++;
            }
          }
	break;
      }

    pbBuffer++;
    cbBuffer--;
    }

  if (fTermCaretActive)
    {
    VidSetCaretPos();
    ShowCaret(hChildTerminal);
    }

  VidSeeMe(nTermChrY);
}

static int near pascal EscapeSeq(szBuf, nChrCnt)
  unsigned char *szBuf;
  int  nChrCnt;
{
  register nCount;


  nCount = 0;
  while (nChrCnt-- > 0 && bTermEscape)
    {
    nCount++;
    cTermBuf[nTermInx++] = *szBuf++;

    switch (cTermBuf[0])
      {
      case 'H':
        fTermNoDisplay = FALSE;

        nTermChrX = 1;
        nTermChrY = 0;
        bTermEscape = FALSE;
        break;

      case 'C':
        if (fTermPrinting)
          SessTellEndDocument();
	else
	  {
	  if (bRecording)
	    CmdCopyRecording();
	  if (bPrinterOn)
	    CmdCopyPrinter(TRUE);
	  }

        fTermNoDisplay = FALSE;

        nTermChrX = 1;
        nTermChrY = 0;
        VidClearPage();

        bTermEscape = FALSE;
        break;

      case 'A':
        if (nTermInx < 2)
          break;

        switch (cTermBuf[1])
          {
          case '0':
            cTermAttr = (cTermAttr & 0x8F) | (0x70);
            break;

          case '1':
            cTermAttr = (cTermAttr & 0x8F) | (0x40);
            break;

          case '2':
            cTermAttr |= 0x80;
            break;

          case '3':
            cTermAttr &= 0x7F;
            break;

          default:
       //   write(3, "\x07A Error\r\n", 10);
            break;
          }

        bTermEscape = FALSE;
        break;

      case 'L':
        SessTellKeyboardLocked();
	bTermEscape = FALSE;
        break;

      case 'P':
        if (!fTermPrinting)
          SessTellStartDocument();
	bTermEscape = FALSE;
        break;

      case 'U':
        if (fTermPrinting && fTermCancel)
          {
          // The user has requested the document download to be cancelled so
          //   wait until we are at end of packet and abort it.
          ReportEnd();
          wSessLineState = SESSION_CONNECT;
          fSessProcess   = FALSE;
          fTermNoDisplay = FALSE;
          SessCanCommand(COMMAND_STOP);
          }
        else
          SessTellKeyboardUnlocked();

        bTermEscape = FALSE;
        break;

      case 'D':
	SessTellDisconnect(FALSE);  // Was True
	bTermEscape = FALSE;
        break;

      default:
    //  write(3, "B Error\r\n", 9);
	bTermEscape = FALSE;
      }
    }

  return (nCount);
}


int TermQueryArgument()
  {
  int nXPos, nYPos, nAcc;
  register i;


  if (nTermChrY == 0 && nTermChrX == 1)
    return (-1);

  nAcc = 0;
  for (nYPos = 0; nYPos <= nTermChrY; nYPos++)
    {
    for (nXPos = 1; nXPos <= TERM_MAXCOL; nXPos++)
      {
      i = (BYTE)(*(pPriVidLine[nYPos] + nXPos));
      if (isdigit(i))
        nAcc = nAcc * 10 + (i - '0');
      else if (i != ' ')
        break;

      if (nYPos == nTermChrY && nXPos < nTermChrX)
        break;
      }
    }

  return (nAcc);
  }


static void near ClrInsertMode()
{
  if (fTermInsert == FALSE)
    return;

  fTermInsert = FALSE;
  VidCreateCaret(hChildTerminal);
  VidSetCaretPos();
}

static void near SetInsertMode()
{
  if (fTermInsert == TRUE)
    return;

  fTermInsert = TRUE;
  VidCreateCaret(hChildTerminal);
  VidSetCaretPos();
}


static void near LineWrap(SHORT sLine)
  {
  int  *pInt;
  char buf[80];
  register i, j;


  /*
  ** Load in the image of the previous line.
  */
  pInt = pPriVidLine[sLine];
  for (i = 0; i < 80; i++)
    buf[i] = (char)(pInt[i+1]);

  /*
  ** If the last character is blank then just return.
  */
  if (buf[79] == ' ')
    return;

  /*
  ** Scan back and determine the length of the word.
  */
  for (i = 79; i >= 0 && buf[i] != ' '; i--);

  /*
  ** If the word is too big then abort the word wrap.
  */
  if (i < 40)
    return;

  /*
  ** Clear the old text.
  */
  j = fTermInsert;
  fTermInsert = FALSE;
  for (nTermChrX = i+1; nTermChrX <= 80; nTermChrX++)
    CharOut(' ');
  fTermInsert = j;

  /*
  ** Insert the new text to the next line.
  */
  nTermChrX = 80;
  TermWriteData(&buf[i], 80 - i, FALSE);
  }
