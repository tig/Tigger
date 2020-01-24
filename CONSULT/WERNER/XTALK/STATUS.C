/*****************************************************************************\
*                                                                             *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Status Child Module (STATUS.C)                                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module supports the status child module and displays it on *
*             the main screen.                                                *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.                                             *
*  1.01 08/28/89 00020 Mostly re-written for Windows 2/3 support.			   *
*	1.01 12/28/89 00076 Change from VT102* to VT52*.						   *
*  1.01 02/03/90 00101 Load modem on comm port change.						   *
*  1.01 02/09/90 dca00022 PJL/MKL Fix Status Bar display for App/Num mode.	   *
*  1.1  04/24/90 dca00093 MKL changed xtalk to dispaly VT102 VT52 without dash*
*	1.1b 05/15/90 dca00096 PJL Fix 'ARQ' to read 'REL' for reliable connects.  *
*  1.1b 05/21/90 dca00100 MKL fixed status bar double-click problem when the 
	bar is short.
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <time.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "prodata.h"
#include "term.h"
#include "properf.h"
#include "dialogs.h"
#include "variable.h"


struct StatData
  {
  int   nEmulator;
  int   nEmulatorPos;
  RECT  rcEmulator;
  char  szEmulator[14];
  int   nConfig;
  int   nConfigPos;
  RECT  rcConfig;
  char  szConfig[16];
  short ixSpeed;
  short ixMode;
  short ixPort;
  int   nKeyPad;
  int   nKeyPadPos;
  RECT  rcKeyPad;
  char  szKeyPad[10];
  int   nCapture;
  int   nCapturePos;
  RECT  rcCapture;
  char  szCapture[8];
  int   nPrinter;
  int   nPrinterPos;
  RECT  rcPrinter;
  char  szPrinter[8];
  int   nComms;
  int   nCommsPos;
  RECT  rcComms;
  char  szComms[6];
  int   nClock;
  int   nClockPos;
  RECT  rcClock;
  char  szClock[9];
  };

  HWND hChildStatus = NULL;

  static struct StatData Data;
  static WORD wElapse = 0;
  static void PaintCapture(HDC hDC);
  static void PaintPrinter(HDC hDC);
  static char cOutput;
  static void PaintComms(HDC hDC);
  static char cComms;
  static BOOL fDisableKeyPad, fDisableConfig, fDisableComms;
  static BOOL fDisableCapture, fDisablePrinter, fDisableEmulator;
  static int Delta; /* width of extra space of status bar fields */



BOOL CreateStatusChild(HWND hParent)
  {
  HWND hWnd;
  char szClassName[64];


  LoadString(hWndInst, IDS_WNDCLASS_STATUS, szClassName, sizeof(szClassName));

  hWnd = CreateWindow(szClassName, NULL, WS_CHILD | WS_VISIBLE|WS_CLIPSIBLINGS,
	0, 0, 0, 0, hParent, (HMENU)NULL, (HANDLE)hWndInst, NULL);

  if (hWnd)
    {
    hChildStatus = hWnd;
    SendMessage(hChildStatus, UM_CONFIG, 0, 0L);
    return (TRUE);
    }
  else
    {
    hChildStatus = NULL;
    return (FALSE);
    }
  }


BOOL DestroyStatusChild()
  {
  if (hChildStatus)
    DestroyWindow(hChildStatus);

  hChildStatus = NULL;

  return (TRUE);
  }


long WndStatProc(HWND hWnd,unsigned message,WORD wParam,LONG lParam)
{
	RECT	 Rect;
	HDC	 hDC;
	PSTR	 pStr, pTime;
	char	 szBuf[64];
	long	 lTime;

	switch (message)
	{
	case UM_EMULATOR:
		//
		//  Display the current terminal emulator on the status line.
		//
		// dca00093 MKL
		if (wParam)
		{
			strcpy(szBuf, (PSTR)wParam);
			if (strcmp(szBuf, "VT-102 ")==0) strcpy(szBuf, "VT102  ");
			else if (strcmp(szBuf, "VT-52  ")==0) strcpy(szBuf, " VT52  ");
		}
		else
			strcpy(szBuf, Data.szEmulator);

		//
		//  The VT102 emulator has a sub-mode of VT52 emulation mode.  When we
		//    are in the VT102 emulator, but in VT52 mode then an asterisk is
		//    placed after the VT102 (VT102*).  We still have a separate VT52
		//    mode.
		//
		// 00076 change the description of VT102* to VT52*
		if (lParam == 1L)
			strcpy(szBuf, "VT102  ");
		else if (lParam == 2L)
			strcpy(szBuf, "VT52*  ");

		//
		//  If we are already displaying the correct emulator then why waste our
		//    time making slow windows calls.
		//
		if (strcmp(Data.szEmulator, szBuf))
		{
			strcpy(Data.szEmulator, szBuf);

			hDC = GetDC(hWnd);

			//  Clear the background before we paint the new configuration.
			CopyRect(&Rect, &Data.rcEmulator);
			Rect.right  -= 2;
			Rect.bottom -= 2;
			FillRect(hDC, &Rect, GetStockObject(WHITE_BRUSH));

			DrawText(hDC, Data.szEmulator, 7, &Data.rcEmulator,
					DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOCLIP);
			ReleaseDC(hWnd, hDC);
		}
		break;

	case UM_CONFIG:
		//
		//  Display the current device configuration on the status line.
		//
		sprintf(szBuf, "%5d/%dN1/COM%d", Comm.nSpeed, Comm.cDataBits,
				cCommPort + 1);

		//
		//  Set the parity type into the configuration description.
		//
		if (Comm.cParity == NOPARITY)
			szBuf[7] = 'N';
		else if (Comm.cParity == ODDPARITY)
			szBuf[7] = 'O';
		else if (Comm.cParity == EVENPARITY)
			szBuf[7] = 'E';
		else if (Comm.cParity == MARKPARITY)
			szBuf[7] = 'M';
		else
			szBuf[7] = 'S';

		//
		//  Set the stop bits into the configuration description.
		//
		if (Comm.cStopBits == ONESTOPBIT)
			szBuf[8] = '1';
		else
			szBuf[8] = '2';

		//
		//  If we have an ARQ/REL connection then let the user know.
		//
		// dca00096 PJL Fix 'ARQ' to read 'REL' for reliable connects.
		if (fConnectARQ)
		{
			char *ptr;

			if (ptr = strrchr(Modem.szConnectARQ, '%'))
				strncpy(szBuf+10, ptr+2, 3);
			else
			{
				szBuf[10] = 'A';
				szBuf[11] = 'R';
				szBuf[12] = 'Q';
			}
		}

		//
		//  If we are already displaying the correct configuration or the width
		//    is too small to have room to display it (fDisableConfig == TRUE)
		//    then don't waste our time.
		//
		if (!fDisableConfig && strcmp(Data.szConfig, szBuf))
		{
			HDC  hDC;
			RECT Rect;

			hDC = GetDC(hWnd);

			//  Clear the background before we paint the new configuration.
			CopyRect(&Rect, &Data.rcConfig);
			Rect.right  -= 2;
			Rect.bottom -= 2;
			FillRect(hDC, &Rect, GetStockObject(WHITE_BRUSH));

			DrawText(hDC, szBuf, 14, &Data.rcConfig,
					DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);
			ReleaseDC(hWnd, hDC);
		}

		//  Make sure that we update Data.szConfig even if we are disabled.
		strcpy(Data.szConfig, szBuf);
		break;

	case UM_KEYPAD:
		// dca00022 PJL/MKL Fix Status Bar display for App/Num mode.
		if (wParam == 0)
			strcpy(szBuf, " Num");
		else if (wParam == 1)
			strcpy(szBuf, "Appl");
		else if (wParam == 2)
			strcpy(szBuf, " Cur");
		else if (wParam == 3)
			strcpy(szBuf, "Lock");
		else if (wParam == 4)
			strcpy(szBuf, "ACur");
		else
		{
			strcpy(szBuf, Data.szKeyPad);
			szBuf[4] = NULL;
		}

		if (LOWORD(lParam) < 100)
			sprintf(&szBuf[4], "/%2d ", LOWORD(lParam));
		else
			sprintf(&szBuf[4], "/%3d", LOWORD(lParam));

		if (!fDisableKeyPad && strcmp(Data.szKeyPad, szBuf))
		{
			hDC = GetDC(hWnd);
			DrawText(hDC, szBuf, 8, &Data.rcKeyPad,
					DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOCLIP);
			ReleaseDC(hWnd, hDC);
		}

		strcpy(Data.szKeyPad, szBuf);
		break;

	case UM_CONNECT:
		wElapse = 0;
		break;

	case WM_LBUTTONDBLCLK:
	{
		register i;

		i = LOWORD(lParam);

		if (i >= Data.rcClock.left) VssScrollToggle();
		else if (i > Data.rcCapture.left-(Delta/2) && i < Data.rcCapture.right)
		    CmdCapture(0, 0);
		else if (i > Data.rcPrinter.left-(Delta/2) && i < Data.rcPrinter.right)
		    TogglePrinter();
		else if (i >= Data.rcEmulator.right && i < Data.rcKeyPad.left)
		{
			if (IfSystemStatus(SS_XFER))
			{
				MessageBeep(0);
				break;
			}
			if (i <= Data.ixSpeed)
			{
				if (Comm.nSpeed == 110)
				{
					Comm.nSpeed = 300;
					Comm.cStopBits = ONESTOPBIT;
				}
				else if (Comm.nSpeed == 300)
					Comm.nSpeed = 600;
				else if (Comm.nSpeed == 600)
					Comm.nSpeed = 1200;
				else if (Comm.nSpeed == 1200)
					Comm.nSpeed = 2400;
				else if (Comm.nSpeed == 2400)
					Comm.nSpeed = 4800;
				else if (Comm.nSpeed == 4800)
					Comm.nSpeed = 9600;
				else if (Comm.nSpeed == 9600)
					Comm.nSpeed = 19200;
				else
				{
					Comm.nSpeed = 110;
					Comm.cStopBits = TWOSTOPBITS;
				}
				CommPortUpdate();
			}
			else if (i <= Data.ixMode) {
				if (Comm.cParity == NOPARITY) {
					Comm.cParity = ODDPARITY;
					Comm.cDataBits =  7;
				}
				else if (Comm.cParity == ODDPARITY) {
					Comm.cParity = EVENPARITY;
					Comm.cDataBits =  7;
				}
				else {
				    Comm.cParity = NOPARITY;
				    Comm.cDataBits = 8;
				}

				CommPortUpdate();
			}
			else if (i <= Data.ixPort) {
			    if (!bConnected) {
				if (sVersion < 3) {
				    // 00101 Force load of new modem.
				    if (cCommPort == 0) Comm.cPort = 1;
				    else Comm.cPort = 0;
				}
				else {
				    // 00101 Force load of new modem.
				    if (cCommPort == 0) Comm.cPort = 1;
				    else if (cCommPort == 1) Comm.cPort = 2;
				    else if (cCommPort == 2) Comm.cPort = 3;
				    else Comm.cPort = 0;
				}
				CommPortUpdate();
			    }
			    else MessageBeep(0);
			}
		    }
		else if (i >= Data.rcEmulator.left && i<Data.rcEmulator.right)
		{
			if (Term.cTerminal == TERM_VT102)
				i = TERM_VT52;
			else if (Term.cTerminal == TERM_VT52)
				i = TERM_IBMPC;
			else if (Term.cTerminal == TERM_IBMPC)
				i = TERM_IBM3101;
			else if (Term.cTerminal == TERM_IBM3101)
				i = TERM_VIDTEX;
			else
				i = TERM_VT102;
			SelectTerminal((char)i);
		}
		else if (i > Data.rcComms.left-(Delta/2) && i < Data.rcComms.right) {
		    if (bConnected) {
			EscapeCommFunction(nCommID, SETXON);
			EscapeCommFunction(nCommID, RESETDEV);
		    }
		    else MessageBeep(0);
		}
	    }
	break;

    case WM_CREATE:
      memset(&Data, '\0', sizeof(Data));
      cOutput = 0;
      cComms  = 0;

      LoadString(hWndInst, IDS_STAT_EMULATOR, Data.szEmulator,
		 sizeof(Data.szEmulator));
      LoadString(hWndInst, IDS_STAT_CONFIG, Data.szConfig,
		 sizeof(Data.szConfig));
      LoadString(hWndInst, IDS_STAT_KEYPAD, Data.szKeyPad,
                 sizeof(Data.szKeyPad));
      LoadString(hWndInst, IDS_STAT_CLOCK, Data.szClock,
		 sizeof(Data.szClock));

      Data.szCapture[0] = 'C';
      Data.szCapture[1] = 'a';
      Data.szCapture[2] = 'p';
      Data.szCapture[3] = 't';
      Data.szCapture[4] = 'u';
      Data.szCapture[5] = 'r';
      Data.szCapture[6] = 'e';
      Data.szCapture[7] = '\0';

      Data.szPrinter[0] = 'P';
      Data.szPrinter[1] = 'r';
      Data.szPrinter[2] = 'i';
      Data.szPrinter[3] = 'n';
      Data.szPrinter[4] = 't';
      Data.szPrinter[5] = 'e';
      Data.szPrinter[6] = 'r';
      Data.szPrinter[7] = '\0';

      Data.szComms[0] = 'I';
      Data.szComms[1] = 'n';
      Data.szComms[2] = ' ';
      Data.szComms[3] = 'O';
      Data.szComms[4] = 'u';
      Data.szComms[5] = 't';
      Data.szComms[6] = '\0';

      lTimeConnected = 0L;
      wElapse = 0;
      break;

    case UM_TIMER:
      wElapse += wParam;
      lTimeConnected = (long)wElapse * 10L;

      /*
      **  Display the current and elapse time in "HH:MM:SS"
      **  format (8 characters).   We get the current time using standard MS
      **  C functions and build the elapse time from the connected time.  If
      **  we are not connected then the elaspe is 00:00:00.  We then compare
      **  the TEXT string that we generated to the current status line (using
      **  UWW_BUFFER).	If the character is different we display it and update
      ** the buffer, else do nothing (already correct on screen).
      */

      sprintf(szBuf, "%02d:%02d:%02d", wElapse / 3600,
	      (wElapse % 3600) / 60, wElapse % 60);

      if (strcmp(szBuf, Data.szClock))
        {
        strcpy(Data.szClock, szBuf);

        hDC = GetDC(hWnd);
        DrawText(hDC, Data.szClock, 8, &Data.rcClock,
                 DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOCLIP);
        ReleaseDC(hWnd, hDC);
        }
      break;

    case WM_TIMER:
      {
      register i;

      i = 0;
      if (bPrinterOn)
	i += 2;
      if (bCaptureOn)
	i += 1;
      if (cOutput != (char)i)
	{
	cOutput = (char)i;
	hDC = GetDC(hWnd);
        if (!fDisableCapture)
          PaintCapture(hDC);
        if (!fDisablePrinter)
          PaintPrinter(hDC);
	ReleaseDC(hWnd, hDC);
	}

      i = 0;
      if (bConnected && !ComStat.fXoffSent)
	i += 2;
      if (bConnected && !(ComStat.fCtsHold||ComStat.fDsrHold||ComStat.fXoffHold))
	i += 1;
      if (!fDisableComms && cComms != (char)i)
	{
	cComms = (char)i;
	hDC = GetDC(hWnd);
        PaintComms(hDC);
	ReleaseDC(hWnd, hDC);
	}
      }
      break;

    case WM_PAINT:
      /*
          Anytimes the width of the main window changes we will re-paint the
          status window and compute the size of the various fields at the
          same time.  Normally a good window programmer would put this in a
          WM_SIZE routine but if one of the fields change size then we have
          to re-size everyone so putting it in the WM_PRINT is just as good
          as anywhere else.
      */
      {
      PAINTSTRUCT ps;
      short cxNeeded, sDelta;
      short cxEmulator, cxConfig, cxKeyPad, cxCapture;
      short cxPrinter, cxComms, cxClock;
      short cyHeight;
      register i;


      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

      //  Compute the size (in pixels) of the various fields.
      i = nSysFontWidth * 2;
      cxEmulator = LOWORD(GetTextExtent(ps.hdc, Data.szEmulator, 7)) + i;
      cxConfig   = LOWORD(GetTextExtent(ps.hdc, Data.szConfig, 14)) + i;
      cxKeyPad   = LOWORD(GetTextExtent(ps.hdc, Data.szKeyPad, 8)) + i;
      cxCapture  = LOWORD(GetTextExtent(ps.hdc, Data.szCapture, 7)) + i;
      cxPrinter  = LOWORD(GetTextExtent(ps.hdc, Data.szPrinter, 7)) + i;
      cxComms    = LOWORD(GetTextExtent(ps.hdc, Data.szComms, 6)) + i;
      cxClock    = LOWORD(GetTextExtent(ps.hdc, Data.szClock, 8)) + i;
      cyHeight   = HIWORD(GetTextExtent(ps.hdc, Data.szEmulator, 1));


      //  Compute the size of the needed area to display all the fields.
      cxNeeded = cxEmulator+cxConfig+cxKeyPad+cxCapture+cxPrinter+cxComms;

      //  Get the client rect size for later.
      GetClientRect(hWnd, &Rect);

      //
      //  We want the elapse timer to always to be in the right edge with no
      //    strange gaps.  So we compute it's size first and always allocate
      //    the same space for it.
      //
      CopyRect(&Data.rcClock, &Rect);
      Data.rcClock.left = Data.rcClock.right - cxClock;
      /*
      if (Rect.right <= cxClock)
	 DrawText(ps.hdc, Data.szClock,
	 8, &Data.rcClock, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOCLIP);
      else {
	 EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
	 break;
      };
      */
      //
      //  Compute which fields we can fit into the status line and the
      //    extra (sDelta) space between each field.
      //
      sDelta = (Data.rcClock.left - cxNeeded) / 6;

      if (sDelta < 0) {
        /* don't have enough width to put all fields in, get rid of
	   IN / OUT stuff first */
        fDisableComms = TRUE;
        cxNeeded -= cxComms;
        sDelta = (Data.rcClock.left - cxNeeded) / 5;
      }
      else fDisableComms = FALSE;

      if (sDelta < 0) {
        fDisableKeyPad = TRUE;
        cxNeeded -= cxKeyPad;
        sDelta = (Data.rcClock.left - cxNeeded) / 4;
      }
      else fDisableKeyPad = FALSE;

      if (sDelta < 0) {
        fDisableEmulator = TRUE;
        cxNeeded -= cxEmulator;
        sDelta = (Data.rcClock.left - cxNeeded) / 3;
      }
      else fDisableEmulator = FALSE;

      if (sDelta < 0) {
        fDisableConfig = TRUE;
        cxNeeded -= cxConfig;
        sDelta = (Data.rcClock.left - cxNeeded) / 2;
      }
      else fDisableConfig = FALSE;

      if (sDelta < 0) {
        fDisablePrinter = TRUE;
        cxNeeded -= cxPrinter;
        sDelta = Data.rcClock.left - cxNeeded;
      }
      else fDisablePrinter = FALSE;

      if (sDelta < 0) {
        fDisableCapture = TRUE;
        sDelta = 0;
      }
      else fDisableCapture = FALSE;

      Delta = sDelta;
      i = 0;

      //
      //  Compute the position of the Emulator field and display it.
      //
      if (!fDisableEmulator) {
          CopyRect(&Data.rcEmulator, &Rect);
          Data.rcEmulator.left = i;
          i += cxEmulator + sDelta;
          Data.rcEmulator.right = i;
          DrawText(ps.hdc, Data.szEmulator, 7, &Data.rcEmulator,
               DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOCLIP);
          MoveTo(ps.hdc, i, ps.rcPaint.top);
          LineTo(ps.hdc, i, ps.rcPaint.bottom);
      }
      else  Data.rcEmulator.right = 0;

      //
      //  If the Config field is showable (enabled) then compute the location
      //    of the config field and display it.  Because we permit the user to
      //    double click within the field to change the configuration we
      //    center the field our self so we can determine where the sub-fields
      //    are if doubled clicked (a real pain).
      //
      if (!fDisableConfig)
        {
        //  While we are here let's compute the indexes into the Config field so
        //    the user can double-click the sub-fields.

        CopyRect(&Data.rcConfig, &Rect);
        Data.rcConfig.left = i + sDelta / 2 + nSysFontWidth;
        Data.rcConfig.right = i += cxConfig + sDelta;
        DrawText(ps.hdc, Data.szConfig, 14, &Data.rcConfig,
                 DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);

        Data.ixSpeed  = Data.rcConfig.left;
        Data.ixSpeed += LOWORD(GetTextExtent(ps.hdc, Data.szConfig, 5));
        Data.ixMode   = Data.rcConfig.left;
        Data.ixMode  += LOWORD(GetTextExtent(ps.hdc, Data.szConfig, 9));
        // Data.ixPort   = Data.rcConfig.left + cxConfig;
        Data.ixPort   = i - 1;

        MoveTo(ps.hdc, i, ps.rcPaint.top);
        LineTo(ps.hdc, i, ps.rcPaint.bottom);
        }
      else Data.rcConfig.right = 0;

      //
      //  If the KeyPad field is showable (enabled) then compute the location
      //    of the KeyPad field and display it.
      //
      if (!fDisableKeyPad) {
        CopyRect(&Data.rcKeyPad, &Rect);
        Data.rcKeyPad.left = i+1;
        Data.rcKeyPad.right = i += cxKeyPad + sDelta;
        DrawText(ps.hdc, Data.szKeyPad, 8, &Data.rcKeyPad,
                 DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOCLIP);

        MoveTo(ps.hdc, i, ps.rcPaint.top);
        LineTo(ps.hdc, i, ps.rcPaint.bottom);
      }
      else Data.rcKeyPad.right = 0;

      //
      //  If the Capture field is showable (enabled) then compute the location
      //    of the Capture field and display it.
      //
      if (!fDisableCapture) {
        CopyRect(&Data.rcCapture, &Rect);
        Data.rcCapture.left   = i + sDelta / 2 + nSysFontWidth;
        //MKL Data.rcCapture.right  = cxCapture - nSysFontWidth * 2;
        Data.rcCapture.right  = i + cxCapture + sDelta;
        Data.rcCapture.top    = (Rect.bottom - cyHeight) / 2;
        Data.rcCapture.bottom = cyHeight;
        PaintCapture(ps.hdc);

        i += cxCapture + sDelta;
        MoveTo(ps.hdc, i, ps.rcPaint.top);
        LineTo(ps.hdc, i, ps.rcPaint.bottom);
      }
      else Data.rcCapture.right = 0;

      //
      //  If the Printer field is showable (enabled) then compute the location
      //    of the Printer field and display it.
      //
      if (!fDisablePrinter) {
        CopyRect(&Data.rcPrinter, &Rect);
        Data.rcPrinter.left   = i + sDelta / 2 + nSysFontWidth;
        Data.rcPrinter.right  = i + cxPrinter + sDelta;
        //MKL Data.rcPrinter.right  = cxPrinter - nSysFontWidth * 2;
        Data.rcPrinter.top    = (Rect.bottom - cyHeight) / 2;
        Data.rcPrinter.bottom = cyHeight;
        PaintPrinter(ps.hdc);

        i += cxPrinter + sDelta;
        MoveTo(ps.hdc, i, ps.rcPaint.top);
        LineTo(ps.hdc, i, ps.rcPaint.bottom);
      }
      else Data.rcPrinter.right = 0;

      //
      //  If the Comms field is showable (enabled) then compute the location
      //    of the Comms field and display it.
      //
      if (!fDisableComms) {
        CopyRect(&Data.rcComms, &Rect);
        Data.rcComms.left   = i + sDelta / 2 + nSysFontWidth;
        Data.rcComms.right  = i + cxComms + sDelta;
        //MKL Data.rcComms.right  = cxComms - nSysFontWidth * 2;
        Data.rcComms.top    = (Rect.bottom - cyHeight) / 2;
        Data.rcComms.bottom = cyHeight;
        PaintComms(ps.hdc);

        i += cxComms + sDelta;
        MoveTo(ps.hdc, Data.rcClock.left, 0);
        LineTo(ps.hdc, Data.rcClock.left, ps.rcPaint.bottom);
      }
      else Data.rcComms.right = 0;

      DrawText(ps.hdc, Data.szClock,
	 8, &Data.rcClock, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOCLIP);

      //  Draw the bottom lines.
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


//
//  PaintCapture
//
//    Paint the capture status on the status line.  The string 'Capture' is
//      displayed normally if we are currently capturing, else it is grayed.
//
static void PaintCapture(HDC hDC)
  {
  if (bCaptureOn)
    {
    DWORD rgbText;

    //  If we have a color screen then paint 'Capture' in red.
    if (!fMonoDisplay)
      rgbText = SetTextColor(hDC, RGB(255, 0, 0));
    TextOut(hDC, Data.rcCapture.left, Data.rcCapture.top, Data.szCapture, 7);
    if (!fMonoDisplay)
      SetTextColor(hDC, rgbText);
    }
  else
    {
    RECT Rect;

    //  Clear the background before we gray the string.
    CopyRect(&Rect, &Data.rcCapture);
    //MKL  Rect.right  += Data.rcCapture.left;
    Rect.bottom += Data.rcCapture.top;
    FillRect(hDC, &Rect, GetStockObject(WHITE_BRUSH));

    //  Draw out 'Capture' and gray it.
    GrayString(hDC, GetStockObject(BLACK_BRUSH), (FARPROC)0L,
               (DWORD)((LPSTR)Data.szCapture), 7, Data.rcCapture.left,
               Data.rcCapture.top, 0, 0);
    }
  }


//
//  PaintPrinter
//
//    Paint the printer status on the status line.  The string 'Printer' is
//      displayed normally if the printer is openned, else it is grayed.
//
static void PaintPrinter(HDC hDC)
  {
  if (bPrinterOn)
    {
    DWORD rgbText;

    //  If we have a color screen then paint 'Capture' in red.
    if (!fMonoDisplay)
      rgbText = SetTextColor(hDC, RGB(255, 0, 0));
    TextOut(hDC, Data.rcPrinter.left, Data.rcPrinter.top, Data.szPrinter, 7);
    if (!fMonoDisplay)
      SetTextColor(hDC, rgbText);
    }
  else
    {
    RECT Rect;

    //  Clear the background before we gray the string.
    CopyRect(&Rect, &Data.rcPrinter);
    // MKL Rect.right  += Data.rcPrinter.left;
    Rect.bottom += Data.rcPrinter.top;
    FillRect(hDC, &Rect, GetStockObject(WHITE_BRUSH));

    //  Draw out 'Printer' and gray it.
    GrayString(hDC, GetStockObject(BLACK_BRUSH), (FARPROC)0L,
               (DWORD)((LPSTR)Data.szPrinter), 7, Data.rcPrinter.left,
               Data.rcPrinter.top, 0, 0);
    }
  }


//
//  PaintComms
//
//    Paint the communication In/Out status display on the status line.  We
//      check the current status of the communication device and either gray
//      or display normally the In/Out status.
//
static void PaintComms(HDC hDC)
  {
  RECT  Rect;
  SHORT ixOut;


  //  Blank out the area so the graying will work.
  CopyRect(&Rect, &Data.rcComms);
  // MKL Rect.right += Data.rcComms.left;
  FillRect(hDC, &Rect, GetStockObject(WHITE_BRUSH));

  //  Compute the offset of the 'Out' string.
  ixOut = LOWORD(GetTextExtent(hDC, Data.szComms, 3));

  //  Display the 'In' string and optionally gray it.
  if (cComms & 0x02)
    {
    TextOut(hDC, Data.rcComms.left, Data.rcComms.top, Data.szComms, 2);
    }
  else
    {
    GrayString(hDC, GetStockObject(BLACK_BRUSH), (FARPROC)0L,
               (DWORD)(LPSTR)Data.szComms, 2, Data.rcComms.left,
               Data.rcComms.top, 0, 0);
    }

  //  Display the 'Out' string and optionally gray it.
  if (cComms & 0x01)
    {
    TextOut(hDC, Data.rcComms.left+ixOut, Data.rcComms.top, &Data.szComms[3],
            3);
    }
  else
    {
    GrayString(hDC, GetStockObject(BLACK_BRUSH), (FARPROC)0L,
               (DWORD)(LPSTR)&Data.szComms[3], 3, Data.rcComms.left+ixOut,
               Data.rcComms.top, 0, 0);
    }
  }
                                                                                                                                                                                                                                                                                              
