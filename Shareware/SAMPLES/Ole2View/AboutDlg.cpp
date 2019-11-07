
#include "stdafx.h"
#include "AboutDlg.h"

#define new DEBUG_NEW

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    DDX_Control(pDX, IDCANCEL, m_Cancel);
    DDX_Control(pDX, IDC_WRITTENBY, m_WrittenBy);
    DDX_Control(pDX, IDC_VERSION, m_Version);
    DDX_Control(pDX, IDC_GROUP, m_Group);
    DDX_Control(pDX, IDC_COPYRIGHT, m_Copyright);
    DDX_Control(pDX, IDC_APPNAME, m_AppName);
    DDX_Control(pDX, IDC_ALLRIGHTSRESERVED, m_AllRightsReserved);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    TCHAR szVersion[64] ;
    BYTE* pdata=NULL;
	TCHAR szFileName[_MAX_PATH] ;
	::GetModuleFileName(NULL, szFileName, _MAX_PATH) ;

    wsprintf( szVersion, _T("Version 2.00 - %s"),(LPTSTR)__DATE__  );
#ifndef _MAC
    DWORD dwDummy ;
    DWORD dw = ::GetFileVersionInfoSize(szFileName, &dwDummy) ;
    if (dw)
    {
        pdata = new BYTE[dw] ;
        if (pdata && ::GetFileVersionInfo(szFileName, NULL, dw, pdata))
        {
			DWORD* pdwBuffer ;
			// Get the translation information.
			BOOL bResult = ::VerQueryValue( pdata,
							  _T("\\VarFileInfo\\Translation"),
							  (void**)&pdwBuffer,
							  (UINT*)&dw);
		    if (!bResult || !dw) goto NastyGoto ;

			// Build the path to the OLESelfRegister key
			// using the translation information.
			TCHAR szName[64] ;
			wsprintf( szName,
					 _T("\\StringFileInfo\\%04hX%04hX\\FileVersion"),
					 LOWORD(*pdwBuffer),
					 HIWORD(*pdwBuffer)) ;

		    // Search for the key.
		    bResult = ::VerQueryValue( pdata, 
									   szName, 
									   (void**)&pdwBuffer, 
									   (UINT*)&dw);
		    if (!bResult || !dw) goto NastyGoto ;

            wsprintf( szVersion, _T("Version %s - %s"),  (LPCTSTR)pdwBuffer, (LPTSTR)__DATE__ ) ;
        }
NastyGoto:
        if (pdata)
            delete []pdata ;
    }
#endif // !_MAC

#ifdef _DEBUG
	m_Version.SetWindowText( CString(szVersion) + _T(" Debug Build") ) ;
#else
	m_Version.SetWindowText( szVersion ) ;
#endif

    CenterWindow();

    return TRUE;  // return TRUE  unless you set the focus to a control
}
