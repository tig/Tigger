// propdoc.h : interface of the CPropDoc class
//
/////////////////////////////////////////////////////////////////////////////
   
#include "propset.h"

#define SZSUMMARYINFOSTREAM "\005SummaryInformation"
#define PID_TITLE               0x00000002
#define PID_SUBJECT             0x00000003
#define PID_AUTHOR              0x00000004
#define PID_KEYWORDS            0x00000005
#define PID_COMMENTS            0x00000006
#define PID_TEMPLATE            0x00000007
#define PID_LASTAUTHOR          0x00000008
#define PID_REVNUMBER           0x00000009
#define PID_EDITTIME            0x0000000A
#define PID_LASTPRINTED         0x0000000B
#define PID_CREATE_DTM          0x0000000C
#define PID_LASTSAVE_DTM        0x0000000D
#define PID_PAGECOUNT           0x0000000E
#define PID_WORDCOUNT           0x0000000F
#define PID_CHARCOUNT           0x00000010
#define PID_THUMBNAIL           0x00000011
#define PID_APPNAME             0x00000012
#define PID_SECURITY            0x00000013


class CPropDoc : public CDocument
{
protected: // create from serialization only
    CPropDoc();
    DECLARE_DYNCREATE(CPropDoc)

// Attributes
public:
    CPropertySet* GetSummaryInfo( void ) ;
    

// Operations
public:

// Implementation
public:
    virtual ~CPropDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
protected:
    virtual BOOL    OnNewDocument();
    virtual BOOL OnSaveDocument(const char* pszPathName);
    virtual BOOL OnOpenDocument(const char* pszPathName);

    CPropertySet    m_SummaryInfo ;

// Generated message map functions
protected:
    //{{AFX_MSG(CPropDoc)
	afx_msg void OnFileSumInfo();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSumInfoDlg dialog

class CSumInfoDlg : public CDialog
{
// Construction
public:
	CSumInfoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSumInfoDlg)
	enum { IDD = IDD_SUMINFO };
	CStatic	m_Directory;
	CStatic	m_Filename;
	CButton	m_Details;
	CString	m_Comments;
	CString	m_Keywords;
	CString	m_LastAuthor;
	CString	m_RevNumber;
	CString	m_Subject;
	CString	m_Template;
	CString	m_Title;
	CString	m_Author;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CSumInfoDlg)
	afx_msg void OnClickedDetails();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
