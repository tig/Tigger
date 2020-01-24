// IPersistDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iviewers.h"
#include "iview.h"
#include "iviewer.h"
#include "IPersist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPersistDlg dialog


CIPersistDlg::CIPersistDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPersistDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIPersistDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIPersistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPersistDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIPersistDlg, CDialog)
	//{{AFX_MSG_MAP(CIPersistDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPersistDlg message handlers
