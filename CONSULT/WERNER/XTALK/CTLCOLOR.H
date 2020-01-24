/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Color Bar Control Window (CTLCOLOR.C)			      *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module supports a color bar control window that is used    *
*	      within the application.					      *
*                                                                             *
*  Revisions: 1.00 10/17/89 Initial Version.				      *
*                                                                             *
\*****************************************************************************/

//
//  External Prototypes
//
BOOL RegisterColorBarControl(HANDLE hWndInst);
VOID SetColorBar(HWND hCtl, USHORT usSelect);
VOID GetColorBar(HWND hCtl, PUSHORT pusSelect);
