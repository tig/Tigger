// msntickDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsntickDlg dialog
//{{AFX_INCLUDES()
#include "ticker.h"
//}}AFX_INCLUDES

class CMsntickDlg : public CDialog
{
// Construction
public:
	CMsntickDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMsntickDlg)
	enum { IDD = IDD_MSNTICK_DIALOG };
	CTicker	m_Ticker;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsntickDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMsntickDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
