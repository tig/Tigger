// WOSA/XRT XRTView Sample Application Version 1.00.003
//
// Copyright (c) 1994 Microsoft Corp.  All Rights Reserved.
//
// xrtview.h
//
// Header file for OLE 2.0 Object Viewer auxillary DLL (DEFO2V.O2V).
//
// Revisions:
//  August 6, 1993  cek     First implementation.
//  August 7, 1993  cek     Rewrote without using MFC 2.0.
//
// An O2V module is a DLL that exports the following function
// at:
//
//      HRESULT WINAPI DisplayInterface
//      (
//          HWND        hwndParent, // Ole2View's main frame window handle
//          LPUNKNOWN   lpunk,      // Pointer to interface
//          LPIID       lpiid,      // Pointer to IDD for this interface
//          LPSTR       lpszName    // Name of this interface (i.e. IUnknown)
//      ) ;
//
// Ole2View finds O2V modules by lookin in the [Interface Viewers] section in the
// OLE2VIEW.INI file:
//
//  [Interface Viewers]
//  <iid>=filename.o2v,<fn name>
//
// Examples:
//
//  [Interface Viewers]
//  {0000011B-0000-0000-C000-000000000046}=contain.o2v,DisplayIOleContainer
//
// By default, the following entries are assumed (entires are not actually
// in the INI file):
//
//  [Interface Viewers]
//  {0000010E-0000-0000-C000-000000000046}=xrtview.o2v,DisplayIDataObject
//  {00020400-0000-0000-C000-000000000046}=xrtview.o2v,DisplayIDispatch
//
// These 'default' implementations can be overriden by placing entries in
// the ini file.
//


#ifndef _DEFO2V_H_
#define _DEFO2V_H_

#if WIN32!=300
#include <oleguid.h>
#include <coguid.h>
#endif

#include "dispatch.h"
#include "resource.h"
#include "util.h"

#ifdef WIN32
int _lwriteT( int, TCHAR *, unsigned int );
HFILE  WINAPI OpenFileT( LPTSTR tszFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle );
#endif

extern HINSTANCE g_hInst ;

// Ownerdraw listbox stuff
#define I_NORMAL        0x0000
#define I_LABEL         0x0001
#define I_COLUMNHEAD    0x0002
#define I_COLUMNENTRY   0x0003

typedef struct FAR tagITEMDATA
{                           
    UINT            nLevel ;            
    UINT            uiType ;
    int             cColumns ;
    LPCOLUMNSTRUCT  rgCol ;
    LPVOID          lpData ;
    
} ITEMDATA, *PITEMDATA, FAR*LPITEMDATA ;    
    


#endif // _DEFO2V_H_




