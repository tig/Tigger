// WOSA/XRT XRTFrame Sample Application Version 1.01
//
// Copyright (c) 1993-94 Microsoft Corporation, All Rights Reserved.
//
//      This is a part of the Microsoft Source Code Samples. 
//      This source code is only intended as a supplement to 
//      Microsoft Development Tools and/or WinHelp documentation.
//      See these sources for detailed information regarding the 
//      Microsoft samples programs.
//
// xrtframe.h : main header file for the STOCKS application
//
// CXRTFrameApp, CApplication, CAboutBox definition  
//
// Revisions:
//  December 8, 1993   cek     First implementation.
//

#ifndef _XRTFRAME_H_
#define _XRTFRAME_H_

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

// When destroying automation objects internally, we can
// either use 'delete' or ::Release().   The following #define controls which
// is used.  The only reason to not use release is as a double check against
// memory leaks.  
//
#define USERELEASETODELETE

#include "resource.h"       // main symbols

extern const CLSID g_clsidDataObject ;

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
	afx_msg LPDISPATCH GetApplication();
	afx_msg BSTR GetFullName();
	afx_msg BSTR GetName();
    afx_msg void Quit(const VARIANT FAR& save);
    afx_msg void DoAbout();
    afx_msg void Activate();
	//}}AFX_DISPATCH
    afx_msg VARIANT DataObjects(const VARIANT FAR& index);
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CXRTFrameApp:
// See xrtframe.cpp for the implementation of this class
//

class CXRTFrameApp : public CWinApp
{ 
DECLARE_DYNCREATE(CXRTFrameApp)

public:
    CXRTFrameApp();
    ~CXRTFrameApp();
// Overrides
    virtual BOOL InitInstance();
// Implementation
    COleTemplateServer m_server;
        // Server object for document creation

protected:        
    
public:

    //{{AFX_MSG(CXRTFrameApp)
    afx_msg void OnAppAbout();
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:    

};


/////////////////////////////////////////////////////////////////////////////

#endif // _XRTFRAME_H_

