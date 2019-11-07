// autodude.h : main header file for the AUTODUDE application
//

#ifndef _AUTODUDE_H_
#define _AUTODUDE_H_

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols   

#include <ole2.h>                
#include <dispatch.h>  

#include "dispauto.h"
                                           
/////////////////////////////////////////////////////////////////////////////
// CAutoDudeApp:
// See autodude.cpp for the implementation of this class
//

class CAutoDudeApp : public CWinApp
{
public:
    CAutoDudeApp();

// Overrides
    virtual BOOL InitInstance();
    virtual int ExitInstance() ;
    
// Implementation
                  
    HRESULT InitOle() ;
    void UninitOle() ;

public:
    CFileMgr FAR* m_pFileMgr ;
    DWORD m_dwCFileMgrCF  ;
    DWORD m_dwRegisterCFileMgr ;
                  
    //{{AFX_MSG(CAutoDudeApp)
    afx_msg void OnAppAbout();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // _AUTODUDE_H_
