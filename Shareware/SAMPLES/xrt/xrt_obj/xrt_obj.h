// xrt_obj.h : main header file for the XRT_OBJ application
//

#ifndef _XRT_OBJ_H_
#define _XRT_OBJ_H_

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include <oleguid.h>
#include <coguid.h>
#include <dispatch.h>

#define USE_HGLOBAL

#include "clsid.h"
                      
#include "..\\wosaxrt.h"
                      
#include "resource.h"       // main symbols
#include "util.h"
#include "mainfrm.h"
#include "doc.h"
#include "collist.h"
#include "object.h"

/////////////////////////////////////////////////////////////////////////////
// CXRTApp:
// See xrt_obj.cpp for the implementation of this class
//

class CXRTApp : public CWinApp
{
public:
    CXRTApp();

    CFont*   m_pFont ;
    CFont*   m_pFontBold ;
    UINT     m_cyFont ; 

    BOOL m_fEmbedding ;
    BOOL m_fAutomation ;    

    CMultiDocTemplate*  m_pObjectViewTemplate ;
    CMultiDocTemplate*  m_pConfigViewTemplate ;
    CXRTData*           m_pXRTDataDoc ;    

    CXRTObjectView*     CreateXRTObjectView() ;
    
    void            LockServer( BOOL ) ;
    void            ObjectCreated( BOOL ) ;

    LPCLASSFACTORY  m_pIClassFactory ;
private:
    DWORD           m_dwRegCO ;
    ULONG           m_cLock ; 
    DWORD           m_cObj ; 

public:    
// Overrides
    virtual BOOL InitInstance();
    virtual int ExitInstance() ;
    virtual CDocument* OpenDocumentFile(LPCSTR lpszFileName); // open named file
    
// Implementation

    //{{AFX_MSG(CXRTApp)
    afx_msg void OnAppAbout();
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    afx_msg void OnFileNew();
    DECLARE_MESSAGE_MAP()
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
#endif // _XRT_OBJ_H_

