// iniedit.h : main header file for the INIEDIT application
//

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "inifile.h"

/////////////////////////////////////////////////////////////////////////////
// CINIeditApp:
// See iniedit.cpp for the implementation of this class
//

class CINIeditApp : public CWinApp
{
public:
    CINIeditApp();

// Overrides
    virtual BOOL InitInstance();
    void OnFileNew() ;
// Implementation

    //{{AFX_MSG(CINIeditApp)
    afx_msg void OnAppAbout();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
