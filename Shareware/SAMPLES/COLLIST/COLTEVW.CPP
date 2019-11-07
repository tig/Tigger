// coltevw.cpp : implementation of the CColtestView class
//

#include "stdafx.h"

#include "coltest.h"

#include "coltedoc.h"
#include "coltevw.h"

#ifndef WIN32
#include "wintime.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColtestView

IMPLEMENT_DYNCREATE(CColtestView, CView)

BEGIN_MESSAGE_MAP(CColtestView, CView)
    //{{AFX_MSG_MAP(CColtestView)
    ON_WM_CREATE()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColtestView construction/destruction

CColtestView::CColtestView()
{
    // TODO: add construction code here
}

CColtestView::~CColtestView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CColtestView drawing

void CColtestView::OnDraw(CDC* pDC)
{
    CColtestDoc* pDoc = GetDocument();

    // TODO: add draw code here
}



/////////////////////////////////////////////////////////////////////////////
// CColtestView diagnostics

#ifdef _DEBUG
void CColtestView::AssertValid() const
{
    CView::AssertValid();
}

void CColtestView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CColtestDoc* CColtestView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CColtestDoc)));
    return (CColtestDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CColtestView message handlers

int CColtestView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    RECT rect ;
    rect.top = lpCreateStruct->y ;
    rect.left = lpCreateStruct->x ;
    rect.bottom = lpCreateStruct->cy ;
    rect.right = lpCreateStruct->cx ;
    if (!m_lb.Create( WS_CHILD | WS_VISIBLE | 
                                WS_BORDER |
                                WS_VSCROLL |
                                #if 1
                                LBS_OWNERDRAWFIXED | 
                                LBS_HASSTRINGS |
                                #else
                                LBS_USETABSTOPS |
                                #endif
                                
                                CLBS_LIKEBUTTON |
                                
                                LBS_NOTIFY |
                                //LBS_DISABLENOSCROLL | 
                                LBS_SORT |
                                LBS_NOINTEGRALHEIGHT,
                                rect, this, 42 ))
    {
        TRACE( "Listbox failed to create!" ) ;
        return -1 ;
    }
    
    CDC *pDC = GetDC() ;
    m_Font.CreateFont( -MulDiv(8,pDC->GetDeviceCaps(LOGPIXELSY),72),
                        0,0,0,FW_NORMAL,FALSE,FALSE,0,
                        ANSI_CHARSET,0,0,0,FF_SWISS,"MS Sans Serif");
    ReleaseDC(pDC) ;                        
    
    m_lb.SetFont( &m_Font ) ;
    
    COLINFO rgColInfo[] =
    {
        //  uiWidth,      uiIndent,     uiFormatFlags
                 130,             0,           DT_LEFT, // Filename
                 110,             0,          DT_RIGHT, // Size
                 100,             0,          DT_RIGHT, // Date
                 100,             0,          DT_RIGHT, // Time
                 100,             0,          DT_RIGHT, // Attrib
                 0,               0,            DT_LEFT, // Dummy
    }  ;

    m_lb.SetColumnsEx( sizeof(rgColInfo) / sizeof(rgColInfo[0]), rgColInfo) ;                     
    
    m_lb.SetWindowText( "Filename\tSize\tDate\tTime\tAttrib\t" ) ;
    
    return 0;
}

LPSTR GetAttribStr( char c ) 
{
    static char sz[32] ;
    char*  p ;
     
    p = sz ;
    *p = '\0' ;
    
    if (c & _A_RDONLY)
    {
        *p++ = 'r' ;
        *p = '\0' ;
    }
        
    if (c & _A_HIDDEN)
    {
        *p++ = 'h' ;
        *p = '\0' ;
    }
    
    if (c & _A_SYSTEM)
    {
        *p++ = 's' ;
        *p = '\0' ;
    }
    
    if (c & _A_ARCH)
    {
        *p++ = 'a' ;
        *p = '\0' ;
    }

    if (c & _A_SUBDIR)
    {
        *p++ = 'd' ;
        *p = '\0' ;
    }

	#ifndef WIN32
    if (c & _A_VOLID)
    {
        *p++ = 'v' ;
        *p = '\0' ;
    }
	#endif
    
    return sz ;

}

void CColtestView::OnInitialUpdate()
{
    CColtestDoc* pDoc = GetDocument();

    BeginWaitCursor();
    
	#ifndef WIN32
    _find_t         FT ;
	#else
    _finddata_t         FT ;
	LONG			hFile ;
	#endif

    m_lb.ResetContent() ;
                                  
    char sz[512] ;
	#ifndef WIN32
    char    szTime[128], szDate[128] ;                              
    tm tmTime ;
    memset( &tmTime, 0, sizeof(tm) ) ;
	#else
	tm *ptmTime ;
	#endif
    
    // Get all files in the dir that match
	#ifndef WIN32
    if (!_dos_findfirst( pDoc->m_strFileSpec, 0xFFFF, &FT ))
	#else
    if (hFile = _findfirst( (char*)(const char*)pDoc->m_strFileSpec, &FT ))
	#endif
        do
        { 
        	#ifndef WIN32                    
            tmTime.tm_mday = FT.wr_date & 0x1f;
            tmTime.tm_mon = ((FT.wr_date >> 5) & 0x0f) - 1;
            tmTime.tm_year = (FT.wr_date >> 9) + 80;        
            
            tmTime.tm_sec = (FT.wr_time & 0x1f) * 2 ;
            tmTime.tm_min = (FT.wr_time >> 5) & 0x3f ;
            tmTime.tm_hour = (FT.wr_time >> 11) ;
			#else
			ptmTime = localtime( &FT.time_write ) ;
			#endif

            AnsiLower( FT.name ) ;                                  

			#ifndef WIN32                                  
            wsprintf( sz, "%s\t%lu\t%s\t%s\t%s", (LPSTR)FT.name,  FT.size, 
                (LPSTR)TimeFormatDate( szDate, &tmTime, DATE_SHORTDATE), 
                (LPSTR)TimeFormatTime( szTime, &tmTime, TIME_12HOUR),  
                    (LPSTR)GetAttribStr( FT.attrib ) ) ;
			#else
			GetDateFormatW
            wsprintf( sz, "%s\t%lu\t%s\t%s\t%s", (LPSTR)FT.name,  FT.size, 
                (LPSTR)"n/a", 
                (LPSTR)"n/a",  
                    (LPSTR)GetAttribStr( FT.attrib ) ) ;
			#endif
                    
            m_lb.AddString( sz ) ;
        #ifndef WIN32    
        } while (!_dos_findnext( &FT )) ;
		#else
        } while (0 == _findnext( hFile, &FT )) ;
		_findclose( hFile ) ;
		#endif

    EndWaitCursor();

}

void CColtestView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    m_lb.DrawItem(lpDrawItemStruct);
}

void CColtestView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    m_lb.MeasureItem(lpMeasureItemStruct);
}

void CColtestView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    m_lb.SetWindowPos( NULL, -1, -1, cx+2, cy+2, SWP_NOZORDER ) ;        
}

