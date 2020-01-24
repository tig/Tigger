// UnifomDataTransfer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUniformDataTransfer dialog
//{{AFX_INCLUDES()
//}}AFX_INCLUDES

class CUniformDataTransfer : public CDialog
{
// Construction
public:
	LPTSTR GetNameOfClipboardFormat(CLIPFORMAT cf);
	IDataObject* m_pdo;
	CUniformDataTransfer(CWnd* pParent, IDataObject* pdo, REFIID riid, LPCTSTR lpszName) ;

// Dialog Data
	//{{AFX_DATA(CUniformDataTransfer)
	enum { IDD = IDD_DATAOBJECT };
	CStatic	m_DummyRichText;
	CListCtrl	m_FormatList;
	//}}AFX_DATA

    CRichEditCtrl m_RichText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUniformDataTransfer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUniformDataTransfer)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkDataFormats(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickDataFormats(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClickDataFormats(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnDeleteItemDataFormats(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedDataFormats(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
