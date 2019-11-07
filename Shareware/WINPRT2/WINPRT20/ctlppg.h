// cfgppg.h : Declaration of the CControlPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CControlPropPage : See cfgppg.cpp for implementation.

class CControlPropPage : public COlePropertyPage
{
    DECLARE_DYNCREATE(CControlPropPage)
    DECLARE_OLECREATE_EX(CControlPropPage)

// Constructor
public:
    CControlPropPage();

// Dialog Data
    //{{AFX_DATA(CControlPropPage)
    enum { IDD = IDD_PROPPAGE_WINPRINTCONTROL };
    CString m_strConfigName;
    CString m_strFileTypeDriver;
    CString m_strPrinter;
    CString m_strPrinterDriver;
    CString m_strPrinterOutput;
    BOOL    m_fShade;
    BOOL    m_fBorder;
    float   m_flLeft;
    float   m_flTop;
    float   m_flRight;
    float   m_flBottom;
    BOOL    m_fDefaultPrinter;
    //}}AFX_DATA
    
// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
    //{{AFX_MSG(CControlPropPage)
    afx_msg void OnHeaders();
    afx_msg void OnPrinter();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

