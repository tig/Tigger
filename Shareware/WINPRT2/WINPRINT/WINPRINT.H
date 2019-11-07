// winprint.h : main header file for the WINPRINT application
//

#ifndef __WINPRINT_H__
#define __WINPRINT_H__

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

const CLSID CLSID_WinPrintControl =
        { 0x0002A501, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0, 0x0, 0x46 } };

#include "resource.h"       // main symbols
#include "util.h"
#include "shadow.h"
#include "aboutbox.h"

#define IDB_FIRST       IDB_NA
#define IDB_LAST        IDB_LANDSCAPE
#define NUM_IDB         (IDB_LAST - IDB_FIRST + 1)
#define BMINDEX(x)      (x - IDB_FIRST)

/////////////////////////////////////////////////////////////////////////////
// CWinPrintApp:
// See winprint.cpp for the implementation of this class
//
class CWinPrintApp : public CWinApp
{
public:
    CWinPrintApp();

    CFont*   m_pFont ;
    CFont*   m_pFontBold ;
    UINT     m_cyFont ;

    CBitmap*    m_rgbmNorm[NUM_IDB] ;
    CBitmap*    m_rgbmHigh[NUM_IDB] ;

    void        LoadBitmaps(BOOL fFree);
    void        FreeBitmaps();

private:
// Overrides
    virtual BOOL InitInstance();
    virtual int ExitInstance();

// Implementation
    virtual BOOL OnIdle(LONG lCount);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    DWORD m_dwSplashTime;
    CSplashWnd m_splash;

    //COleTemplateServer m_server;
        // Server object for document creation

    //{{AFX_MSG(CWinPrintApp)
    afx_msg void OnAppAbout();
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

extern CWinPrintApp theApp;


/////////////////////////////////////////////////////////////////////////////
#endif
