// StgEdit.h : main header file for the STGVIEW application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CStgEditApp:
// See StgEdit.cpp for the implementation of this class
//

class CStgEditApp : public CWinApp
{
public:
	CStgEditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStgEditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStgEditApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#define BYTESPERLINE 16
#define DIVISIONPOINTS 8
void FormatHexEditLine( ULONG ulAddress, ULONG cb, BYTE* rgb, PTSTR pszLine ) ;

/////////////////////////////////////////////////////////////////////////////
