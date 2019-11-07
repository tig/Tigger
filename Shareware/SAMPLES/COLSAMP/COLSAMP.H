// colsamp.h : main header file for the STOCKS application
//
// CColSampApp, CApplication, CAboutBox definition  
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  December 8, 1993   cek     First implementation.
//

#ifndef _COLSAMP_H_
#define _COLSAMP_H_

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


/////////////////////////////////////////////////////////////////////////////
// CApplication command target

class CApplication : public CCmdTarget
{   
    DECLARE_DYNCREATE(CApplication)
public:
    CApplication();         // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
public:
    virtual ~CApplication();
    virtual void OnFinalRelease();

protected:
    // Generated message map functions
    //{{AFX_MSG(CApplication)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    DECLARE_OLECREATE(CApplication)

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CApplication)
    afx_msg BOOL GetVisible();
    afx_msg void SetVisible(BOOL bNewValue);
    afx_msg void Quit(const VARIANT FAR& save);
    afx_msg void DoAbout();
    afx_msg void Activate();
    afx_msg LPDISPATCH Documents(const VARIANT FAR& index);
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CColSampApp:
// See colsamp.cpp for the implementation of this class
//

class CColSampApp : public CWinApp
{ 
DECLARE_DYNCREATE(CColSampApp)

public:
    CColSampApp();
    ~CColSampApp();
// Overrides
    virtual BOOL InitInstance();
// Implementation
    COleTemplateServer m_server;
        // Server object for document creation

protected:        
    
public:

    //{{AFX_MSG(CColSampApp)
    afx_msg void OnAppAbout();
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:    

};


/////////////////////////////////////////////////////////////////////////////

#endif // _COLSAMP_H_

