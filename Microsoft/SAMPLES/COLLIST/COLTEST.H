// coltest.h : main header file for the COLTEST application
//

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#ifndef WIN32
#include "wintime.h"
#endif
#include "collist.h"

/////////////////////////////////////////////////////////////////////////////
// CColtestApp:
// See coltest.cpp for the implementation of this class
//

class CColtestApp : public CWinApp
{
public:
    CColtestApp();

// Overrides
    virtual BOOL InitInstance();
    virtual void OnFileOpen() ;

// Implementation

    //{{AFX_MSG(CColtestApp)
    afx_msg void OnAppAbout();
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
