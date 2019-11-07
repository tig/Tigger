// XRT_OBJ.H
//
// Header file for XRT_OBJ IDataObject sample application.  This file
// defines the classes for the main window and UI.
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// July 15, 1993
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  July 15, 1993   cek     First implementation.
//
// Implementation Notes:
//       

#ifndef _XRT_OBJ_
#define _XRT_OBJ_

/////////////////////////////////////////////////////////////////////////////
// Application class
//
class CXRTObjectApp : public CWinApp
{       

protected:
    LPCLASSFACTORY  m_pIClassFactory ;
    DWORD           m_dwRegCO ;
    
public:
    ULONG           m_cLock ; 
    DWORD           m_cObj ; 
    void            LockServer() ;
    void            UnlockServer() ;

    virtual BOOL    InitInstance();
    virtual int     ExitInstance(); 

};


/////////////////////////////////////////////////////////////////////////////
// IClassFactory implementation class
//
class __far CImpIClassFactory : public IClassFactory
{
    protected:
        ULONG           m_cRef;         //Reference count on object

    public:
        CImpIClassFactory(void);
        ~CImpIClassFactory(void);

        //IUnknown interface members
        STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IClassFactory members
        STDMETHODIMP CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
        STDMETHODIMP LockServer(BOOL);
};


typedef CImpIClassFactory FAR * LPIMPICLASSFACTORY;


/////////////////////////////////////////////////////////////////////////////
// Dialog class
//
class CPromptDlg : public CDialog
{
public:
    CPromptDlg();
    //{{AFX_DATA(CPromptDlg)
    enum { IDD = IDD_PROMPT };
    //}}AFX_DATA

    void Save();
    virtual void DoDataExchange(CDataExchange* pDX);
    //{{AFX_MSG(CPromptDlg)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif _XRT_OBJ_
