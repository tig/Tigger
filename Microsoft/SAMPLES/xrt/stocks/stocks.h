// stocks.h : main header file for the STOCKS application
//

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CStocksApp:
// See stocks.cpp for the implementation of this class
//

class CStocksApp : public CWinApp
{
public:
    CStocksApp();

// Overrides
    virtual BOOL InitInstance();

// Implementation
    COleTemplateServer m_server;
        // Server object for document creation

    //{{AFX_MSG(CStocksApp)
    afx_msg void OnAppAbout();
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
