// propview.cpp : implementation of the CPropView class
//

#include "stdafx.h"
#include <ole2.h>
#include <storage.h>
#include <variant.h>

#include "clsid.h"
#include "prop.h"

#include "propdoc.h"
#include "propview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropView

IMPLEMENT_DYNCREATE(CPropView, CView)

BEGIN_MESSAGE_MAP(CPropView, CView)
    //{{AFX_MSG_MAP(CPropView)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropView construction/destruction

CPropView::CPropView()
{
    // TODO: add construction code here
}

CPropView::~CPropView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPropView drawing

void CPropView::OnDraw(CDC* pDC)
{        
    RECT    rcClient ;
    CString sz ;    
    CPropertySection*   pSect ;
    
    CPropDoc* pDoc = GetDocument();
    
    GetClientRect( &rcClient ) ;
    InflateRect( &rcClient, -5, -5 ) ;                                                                               
    
    if (!(pSect = pDoc->GetSummaryInfo()->GetSection( FormatID_SummaryInformation )))
    {
        sz = "\nThere is no SummaryInformation." ;
    }
    else
    {     
        LPSTR sz2 ;
        char    szBuf[256] ;
        
        sz =  CString("\nTitle:\t\t\t") ;
        sz2 = (LPSTR)pSect->Get( PID_TITLE ) ;
        sz += (sz2 ? sz2 : "<empty>" ) ;        
        
        sz += CString("\nSubject:\t\t\t") ;
        sz2 =  (LPSTR)pSect->Get( PID_SUBJECT ) ;
        sz += (sz2 ? sz2 : "<empty>" ) ;        
        
        sz += CString("\nAuthor:\t\t\t") ;
        sz2 = (LPSTR)pSect->Get( PID_AUTHOR ) ;
        sz += (sz2 ? sz2 : "<empty>" ) ;        
        
        sz += CString("\nKeywords:\t\t") ;
        sz2 = (LPSTR)pSect->Get( PID_KEYWORDS ) ;
        sz += (sz2 ? sz2 : "<empty>" ) ;        
        
        sz += CString("\nComments:\t\t") ;
        sz2 = (LPSTR)pSect->Get( PID_COMMENTS )  ;
        sz += (sz2 ? sz2 : "<empty>" ) ;        
        
        sz += CString("\nTemplate:\t\t") ;
        sz2 = (LPSTR)pSect->Get( PID_TEMPLATE ) ;
        sz += (sz2 ? sz2 : "<empty>" ) ;        
        
        sz += CString("\nLast saved by:\t\t") ;
        sz2 = (LPSTR)pSect->Get( PID_LASTAUTHOR ) ;
        sz += (sz2 ? sz2 : "<empty>" ) ;        
        
        sz += CString("\nRevision Number:\t\t") ;
        sz2 = (LPSTR)pSect->Get( PID_REVNUMBER ) ;
        sz += (sz2 ? sz2 : "<empty>" ) ;        
        
        sz += CString("\nTotal editing time:\t\t<n/a>") ;
        //pSect->Get( PID_EDITTIME_DTM, VT_FILETIME )  ;
        
        sz += CString("\nLast Printed:\t\t<n/a>") ;
        //pSect->Get( PID_LASTPRINTED VT_FILETIME )  ;

        sz += CString("\nCreate Time/Date:\t\t<n/a>") ;
        //pSect->Get( PID_CREATE_DTM, VT_FILETIME )  ;

        sz += CString("\nLast saved Time/Date:\t<n/a>") ;
        //pSect->Get( PID_CREATE_DTM, VT_FILETIME )  ;

        sz += CString("\nNumber of Pages:\t\t") ;
        sz2 = (LPSTR)pSect->Get( PID_PAGECOUNT ) ;
        wsprintf( szBuf, "%lu", (sz2 ? *(LPDWORD)sz2 : 0L) ) ;
        sz += szBuf ;
        
        sz += CString("\nNumber of Words:\t\t") ;
        sz2 = (LPSTR)pSect->Get( PID_WORDCOUNT) ;
        wsprintf( szBuf, "%lu", (sz2 ? *(LPDWORD)sz2 : 0L) ) ;
        sz += szBuf ;
        
        sz += CString("\nNumber of Characters:\t") ;
        sz2 = (LPSTR)pSect->Get( PID_CHARCOUNT ) ;
        wsprintf( szBuf, "%lu", (sz2 ? *(LPDWORD)sz2 : 0L) ) ;
        sz += szBuf ;
        
        sz += CString("\nThumbnail:\t\t<n/a>") ;
        
        sz += CString("\nName of Creating App.:\t") ;
        sz2 = (LPSTR)pSect->Get( PID_APPNAME ) ;
        sz += (sz2 ? sz2 : "<empty>" ) ;             
        
        sz += CString("\nSecurity:\t\t\t") ;
        sz2 = (LPSTR)pSect->Get( PID_SECURITY ) ;
        wsprintf( szBuf, "%#0.8lX", (sz2 ? *(LPDWORD)sz2 : -0L ) ) ;
        sz += szBuf ;
        
    }    
    pDC->DrawText( sz, 
              -1, &rcClient, DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX  | DT_WORDBREAK ) ;
}



/////////////////////////////////////////////////////////////////////////////
// CPropView diagnostics

#ifdef _DEBUG
void CPropView::AssertValid() const
{
    CView::AssertValid();
}

void CPropView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CPropDoc* CPropView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPropDoc)));
    return (CPropDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPropView message handlers
