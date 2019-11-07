// msntickDlg.cpp : implementation file
//

#include "stdafx.h"
#include "msntick.h"
#include "msntickDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsntickDlg dialog

class CPropBag : IPropertyBag
{
public:
    ULONG m_cRefs ;

    CPropBag() 
    { 
        m_cRefs = 0;
    };

    virtual ~CPropBag() {};

    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
    {
        *ppvObject = NULL;
        if (riid == IID_IUnknown || riid == IID_IPropertyBag)
        {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;

    }
    
    virtual ULONG STDMETHODCALLTYPE AddRef( void)
    {
        m_cRefs++;
        return m_cRefs;
    }

    virtual ULONG STDMETHODCALLTYPE Release( void)
    {
        m_cRefs--;
        return m_cRefs;
    }


    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read( 
        /* [in] */ LPCOLESTR pszPropName,
        /* [out][in] */ VARIANT __RPC_FAR *pVar,
        /* [in] */ IErrorLog __RPC_FAR *pErrorLog)
    {
        // <param name="NumLines" value="2">
        if (lstrcmpiW(L"NumLines", pszPropName)==0)
        {
            VARIANT v;
            VariantInit(&v);
            if (pVar->vt == VT_EMPTY)
                pVar->vt = VT_I4;
            v.vt = VT_I4;
            v.lVal = 2;
            VariantChangeType(pVar, &v, 0, pVar->vt);
            return S_OK;
        }

	    // <param name="DataObjectActive" value="1">
        if (lstrcmpiW(L"DataObjectActive", pszPropName)==0)
        {
            VARIANT v;
            VariantInit(&v);
            if (pVar->vt == VT_EMPTY)
                pVar->vt = VT_I4;
            v.vt = VT_I4;
            v.lVal = 1;
            VariantChangeType(pVar, &v, 0, pVar->vt);
            return S_OK;
        }

	    // <param name="ServerRoot" value="http://investor.msn.com">
        if (lstrcmpiW(L"ServerRoot", pszPropName)==0)
        {
            VARIANT v;
            VariantInit(&v);
            if (pVar->vt == VT_EMPTY)
                pVar->vt = VT_BSTR;
            v.vt = VT_BSTR;
            v.bstrVal = SysAllocString(L"http://investor.msn.com");
            VariantChangeType(pVar, &v, 0, pVar->vt);
            return S_OK;
        }

	    // <param name="Keywords" value="http://www.msnbc.com/news/ticker.txt">
        if (lstrcmpiW(L"Keywords", pszPropName)==0)
        {
            VARIANT v;
            VariantInit(&v);
            if (pVar->vt == VT_EMPTY)
                pVar->vt = VT_BSTR;
            v.vt = VT_BSTR;
            v.bstrVal = SysAllocString(L"http://www.msnbc.com/news/ticker.txt");
            VariantChangeType(pVar, &v, 0, pVar->vt);
            return S_OK;
        }
	     
        // <param name="InvestorURL" value="http://investor.msn.com/">
        if (lstrcmpiW(L"InvestorURL", pszPropName)==0)
        {
            VARIANT v;
            VariantInit(&v);
            if (pVar->vt == VT_EMPTY)
                pVar->vt = VT_BSTR;
            v.vt = VT_BSTR;
            v.bstrVal = SysAllocString(L"http://investor.msn.com/");
            VariantChangeType(pVar, &v, 0, pVar->vt);
            return S_OK;
        }

        return E_INVALIDARG;
    }
    
    virtual HRESULT STDMETHODCALLTYPE Write( 
        /* [in] */ LPCOLESTR pszPropName,
        /* [in] */ VARIANT __RPC_FAR *pVar)
    {
        return E_NOTIMPL;
    }

};

CMsntickDlg::CMsntickDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsntickDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMsntickDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMsntickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsntickDlg)
	DDX_Control(pDX, IDC_TICKER, m_Ticker);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMsntickDlg, CDialog)
	//{{AFX_MSG_MAP(CMsntickDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsntickDlg message handlers

BOOL CMsntickDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    IDispatch* pdisp = m_Ticker.GetIDispatch(FALSE);
    ASSERT(pdisp);
    IPersistPropertyBag* pPBag;
    pdisp->QueryInterface(IID_IPersistPropertyBag, (void**)&pPBag);
    ASSERT(pPBag);

    CPropBag* p = new CPropBag;
    IPropertyBag* pBag;
    pBag->QueryInterface(IID_IPropertyBag, (void**)&pBag);
    ASSERT(pPBag);

    ASSERT(pBag);
    pPBag->Load(pBag, NULL);
    pPBag->Release();
    pBag->Release();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMsntickDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMsntickDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
