// PropView.cpp : implementation file
//

#include "stdafx.h"
#include "oleprop.h"
#include "propset.h"
#include "StgEdit.h"

#include "StgEditItem.h"
#include "ContentsView.h"
#include "PropView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef struct tagVT 
{
	VARTYPE	vt ;
	TCHAR*	psz ;
} VT;

static VT grgVT[] = 
{
    VT_EMPTY,   _T("VT_EMPTY"),
	VT_NULL,	_T("VT_NULL"),
	VT_I2,		_T("VT_I2"),
	VT_I4,		_T("VT_I4"),
	VT_R4,		_T("VT_R4"),
	VT_R8,		_T("VT_R8"),
	VT_CY,		_T("VT_CY"),
	VT_DATE,	_T("VT_DATE"),
	VT_BSTR,	_T("VT_BSTR"),
	VT_ERROR,	_T("VT_ERROR"),
	VT_BOOL,	_T("VT_BOOL"),
	VT_VARIANT,	_T("VT_VARIANT"),
	VT_UI1,		_T("VT_UI1"),
	VT_UI2,		_T("VT_UI2"),
	VT_UI4,		_T("VT_UI4"),
	VT_I8,		_T("VT_I8"),
	VT_UI8,		_T("VT_UI8"),
	VT_LPSTR,	_T("VT_LPSTR"),
	VT_LPWSTR,	_T("VT_LPWSTR"),
	VT_FILETIME,_T("VT_FILETIME"),
	VT_BLOB,	_T("VT_BLOB"),
	VT_STREAM,	_T("VT_STREAM"),
	VT_STORAGE,	_T("VT_STORAGE"),
	VT_STREAMED_OBJECT,	_T("VT_STREAMED_OBJECT"),
	VT_STORED_OBJECT,	_T("VT_STORED_OBJECT"),
	VT_BLOB_OBJECT,	_T("VT_BLOB_OBJECT"),
	VT_CF,		_T("VT_CF"),
	VT_CLSID,	_T("VT_CLSID"),
	VT_VECTOR,	_T("VT_VECTOR"),
	VT_ILLEGAL,	_T("VT_ILLEGAL")
};

#define NUM_VT (sizeof(grgVT)/sizeof(VT))

/////////////////////////////////////////////////////////////////////////////
// CPropertyView

IMPLEMENT_DYNCREATE(CPropertyView, CContentsView)

CPropertyView::CPropertyView()
{
}

CPropertyView::~CPropertyView()
{
}


BEGIN_MESSAGE_MAP(CPropertyView, CContentsView)
	//{{AFX_MSG_MAP(CPropertyView)
    ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyView drawing

void CPropertyView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyView diagnostics

#ifdef _DEBUG
void CPropertyView::AssertValid() const
{
	CContentsView::AssertValid();
}

void CPropertyView::Dump(CDumpContext& dc) const
{
	CContentsView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPropertyView message handlers

BOOL CPropertyView::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CContentsView::PreCreateWindow(cs);
}

int CPropertyView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CContentsView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    CRect rect ;
    GetClientRect( &rect ) ;

    DWORD dw = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
								LBS_EXTENDEDSEL |
                                LBS_HASSTRINGS |
                                LBS_DISABLENOSCROLL | 
                                LBS_NOINTEGRALHEIGHT ;
                          
    if (!m_lb.Create( dw, rect, this, 42 ))
    {
        TRACE( "Listbox failed to create!" ) ;
        return -1 ;
    }
    
    CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject( ANSI_FIXED_FONT ) ) ;
    m_lb.SetFont( pFont ) ;
	
	return 0;
}

void CPropertyView::OnInitialUpdate()
{
	TRACE0("CPropertyView::OnInitialUpdate()\n") ;
	
}

void CPropertyView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
    m_lb.SetRedraw( FALSE ) ;            
	m_lb.ResetContent() ;
    TCHAR szLine[BYTESPERLINE * 6 + 12];
	ULONG	cbRead = 0 ;
	ULONG	ulPos = 0 ;

	if (pHint && pHint->IsKindOf(RUNTIME_CLASS(CStgEditPropItem)))
	{
		CStgEditPropItem* pItem = (CStgEditPropItem*)pHint ;
		CProperty* pProp = pItem->m_pSec->GetProperty(pItem->m_ulPropID) ;
		ASSERT(pProp) ;
		
		DWORD cb ;
		CString strLine ;
		
		LPVOID  pv = pProp->Get( &cb ) ;
		
		strLine.Format(_T("Property ID: 0x%08x"), pItem->m_ulPropID ) ;
		m_lb.AddString(strLine) ;
		
		LPCTSTR psz = NULL ;
		for (int i = 0 ; i < NUM_VT ; i++)
		{
			if (pProp->GetType() == grgVT[i].vt)
			{	
				psz = grgVT[i].psz ;
				break ;
			}
		}	
		if (psz == NULL)
		{
			psz = _T("<invalid type!>" );
		}

		strLine.Format(_T("Type:        %s"), psz ) ;
		m_lb.AddString(strLine) ;
		
		CString str ;
		if (pItem->m_pSec->GetName( pProp->GetID(), str ))
		{			
			strLine.Format(_T("Name:        %s"), str ) ;
			m_lb.AddString(strLine) ;
		}

		PROPVARIANT propvar ;
		VariantInit((VARIANT*)&propvar) ;
		
		propvar.vt = (VARTYPE)pProp->GetType() ;
		
		switch(propvar.vt)
		{
			case VT_EMPTY:          // nothing                     
			case VT_NULL:
			case VT_ILLEGAL:
			break;

			case VT_I2:             // 2 byte signed int           
			case VT_BOOL:           // True=-1, False=0            
			case VT_I4:             // 4 byte signed int           
			case VT_R4:             // 4 byte real                 
			case VT_R8:             // 8 byte real                 
			case VT_CY:             // currency                    
			case VT_DATE:           // date                        
			case VT_I8:             // signed 64-bit int           
			case VT_FILETIME:       // FILETIME                    
				if (pv && cb)
					memcpy(&propvar.bVal, pv, cb) ;
			break;

			case VT_LPSTR:          // null terminated 8-bit string 
			{
				char* psz = (char*)pv ;
				propvar.vt = VT_BSTR ;
				propvar.bstrVal = SysAllocStringLen(NULL,cb) ;
				ASSERT(propvar.bstrVal) ;
				MultiByteToWideChar(CP_ACP, 0, psz, cb, propvar.bstrVal, cb) ;
			}
			break ;

			case VT_STREAM:         // Name of the stream follows  
			case VT_STORAGE:        // Name of the storage follows 
			case VT_STREAMED_OBJECT:// Stream contains an object   
			case VT_STORED_OBJECT:  // Storage contains an object  
			case VT_STREAMED_PROPSET:// Stream contains a propset  
			case VT_STORED_PROPSET: // Storage contains a propset  
			case VT_BSTR:           // binary string               
			case VT_LPWSTR:         // UNICODE string 
				propvar.vt = VT_BSTR ;
				propvar.bstrVal = SysAllocStringLen((WCHAR*)pv,cb) ;
				ASSERT(propvar.bstrVal) ;
			break;
				
			case VT_BLOB:           // Length prefixed bytes       
			case VT_BLOB_OBJECT:    // Blob contains an object     
			case VT_BLOB_PROPSET:   // Blob contains a propset     
			case VT_CF:             // Clipboard format            
	        case VT_VARIANT:        // VARIANT*
				propvar.vt = VT_BSTR ;
				propvar.bstrVal = SysAllocString(L"<contains binary data>") ;
				ASSERT(propvar.bstrVal) ;
			break;

			case VT_CLSID:          // A Class ID                  
			{
				WCHAR* pwsz = NULL;
				StringFromCLSID(*(CLSID*)pv,&pwsz) ;
				propvar.vt = VT_BSTR ;
				propvar.bstrVal = SysAllocString(pwsz) ;
				CoTaskMemFree(pwsz) ;
				ASSERT(propvar.bstrVal) ;
			}
			break;

		default:
			TRACE0("Invalid property type!\n");
			break ;
		}
		
		if (propvar.vt != VT_BSTR && FAILED(VariantChangeType((VARIANT*)&propvar, (VARIANT*)&propvar, 0, VT_BSTR)))
		{
			strLine.Format(_T("Value:       <VariantChangeType Failed>"));
		}
		else
#ifdef _UNICODE
			strLine.Format(L"Value:       %s", propvar.bstrVal ) ;
#else
			strLine.Format("Value:       %S", propvar.bstrVal ) ;
#endif
		
		m_lb.AddString(strLine) ;
		VariantClear((VARIANT*)&propvar) ;
		
		//strLine.Format(_T("Raw data (%d bytes):"), cb) ;
		m_lb.AddString(_T("")) ;

		ulPos = 0 ;
		for (BYTE* p2 = (BYTE*)pv ; p2 < ((BYTE*)pv) + cb ;)
		{
			BYTE* p3 = p2 ;
			for (cbRead = 0 ; (cbRead < BYTESPERLINE) && (p2 < ((BYTE*)pv) + cb) ; cbRead++)
				p2++ ;	
			
			if (cbRead > 0)
			{
				FormatHexEditLine( ulPos, cbRead, p3, szLine ) ;
				ulPos += cbRead ;
				m_lb.AddString(szLine) ;
			}
		}
		while(cbRead >= BYTESPERLINE) ;
	
	}
    
    m_lb.Invalidate( FALSE ) ;   
    m_lb.SetRedraw( TRUE ) ;
}

void CPropertyView::OnSize(UINT nType, int cx, int cy)
{
    CContentsView::OnSize(nType, cx, cy);
    
    m_lb.SetWindowPos( NULL, -1, -1, cx+2, cy+2, SWP_NOZORDER ) ;        
}

void CPropertyView::OnEditCopy() 
{
	MessageBeep(0);
}

void CPropertyView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((BOOL)m_lb.GetCount());	
}
