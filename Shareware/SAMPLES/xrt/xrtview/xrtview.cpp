// xrtview.cpp    
//
// Implementation file for XRTVIEW.DLL (LibMain, WEP).
//
// WOSA/XRT XRTView Sample Application Version 1.00.003
//
// Copyright (c) 1994 Microsoft Corp.  All Rights Reserved.
//
// Revisions:
//  August 6, 1993  cek     First implementation.
//  August 7, 1993  cek     Rewrote without using MFC 2.0.
//  Feburary 19, 1994 cek   Derived XRTView from DEFO2V.DLL 
//

#include "precomp.h"

#ifndef WIN32
#include <initguid.h>
#endif

#include "xrtview.h"

HINSTANCE   g_hInst = NULL ;

extern "C"
HANDLE FAR PASCAL LibMain(HINSTANCE hInst, WORD wDataSeg, WORD cbHeapSize, LPSTR lpCmdLine)
{

#ifndef WIN32
    if (0!=cbHeapSize)
        UnlockData(0);
#endif

    g_hInst=hInst;
    return hInst;
}

#pragma code_seg("WEP_SEG") 
extern "C"
void FAR PASCAL WEP(int bSystemExit)
{
    return;
}





