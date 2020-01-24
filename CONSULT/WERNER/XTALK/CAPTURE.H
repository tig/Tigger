/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Capture Include File (CAPTURE.H)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

extern void  WriteRawToCapture(char *, short);
extern void  WriteStrToCapture(char *);
extern ULONG CaptureChars(void);
extern void  FlushCaptureBuf();

extern char szCaptureName[16];
extern char szCaptureBuf[136];
extern int  nCaptureInx;
extern char fCapturePause;

#define WriteCharToCapture(x) (nCaptureInx < sizeof(szCaptureBuf) ?	 \
			       (void)(szCaptureBuf[nCaptureInx++] = x) : \
			       FlushCaptureBuf());
