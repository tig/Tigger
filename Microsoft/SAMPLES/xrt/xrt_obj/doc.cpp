// doc.cpp : implementation of the CXRTData class
//

#include "stdafx.h"
#include "xrt_obj.h"

#include "doc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXRTData

IMPLEMENT_DYNCREATE(CXRTData, CDocument)

BEGIN_MESSAGE_MAP(CXRTData, CDocument)
    //{{AFX_MSG_MAP(CXRTData)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXRTData construction/destruction

CXRTData::CXRTData()
{
    CString szSection ; szSection.LoadString( IDS_INI_CONFIG ) ;
    m_uiTimeout = AfxGetApp()->GetProfileInt( szSection, "Timeout", 100 ) ;

    m_uiQuoteProb = AfxGetApp()->GetProfileInt( szSection, "QuoteProb", 10 ) ;
    m_uiElementProb = AfxGetApp()->GetProfileInt( szSection, "ElementProb", 3 ) ;

    lstrcpy( m_sz, "MSFT 77 3/4" ) ;

    m_lpQuotes = NULL ;    
    
    // [Quotes] secton of INI file contains entries like this:
    // MSFT=high,low,ask,volume,pe
    //
    LPSTR   lpbuf = new _far char[16000] ;  
    char szVal[80] ;
    if (GetPrivateProfileString( "Quotes", NULL, "", lpbuf, 16000, AfxGetApp()->m_pszProfileName ))
    {
        int c ;
        LPSTR pszKey ;
        for (pszKey = lpbuf, c = 0;
                *pszKey != '\0'; pszKey += lstrlen(pszKey) + 1) 
            c++ ;

        m_lpQuotes = (LPXRTSTOCKS)GlobalAllocPtr( GHND | GMEM_SHARE, (c * sizeof(XRTSTOCK)) + sizeof(XRTSTOCKS) ) ;
        m_lpQuotes->cStocks = c ;
        
        LPSTR lp ;
        int nConv ;
        for (pszKey = lpbuf, c = 0 ; *pszKey != '\0'; pszKey += lstrlen(pszKey) + 1) 
        {
            GetPrivateProfileString("Quotes", pszKey, "XXX,0,0", szVal, sizeof(szVal), AfxGetApp()->m_pszProfileName ) ;
            lstrcpy( m_lpQuotes->rgStocks[c].szSymbol, pszKey ) ;
            lp = szVal ;
            m_lpQuotes->rgStocks[c].ulLast = ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
            m_lpQuotes->rgStocks[c].ulVolume = ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
            m_lpQuotes->rgStocks[c].uiMembers = XSM_ALL ;
            m_lpQuotes->rgStocks[c].uiIndex = c ;
            c++ ;
        }            
    }
    else
    {
        m_lpQuotes = (LPXRTSTOCKS)GlobalAllocPtr( GHND | GMEM_SHARE, sizeof(XRTSTOCKS) ) ;
        m_lpQuotes->cStocks = 0 ;
    }
    delete lpbuf ;
    srand( (unsigned)time( NULL ) );
}

CXRTData::~CXRTData()
{
    CString szSection ; szSection.LoadString( IDS_INI_CONFIG ) ;
    char sz[16] ; wsprintf( sz, "%u", m_uiTimeout ) ;
    AfxGetApp()->WriteProfileString( szSection, "Timeout", sz ) ;
    
    if (m_lpQuotes != NULL)
    {
        GlobalFreePtr( m_lpQuotes ) ;
    }
}

BOOL CXRTData::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)
    return TRUE;
}


void CXRTData::GetNewData( ) 
{
    int change ;
    LPXRTSTOCK    lpQ ;
    // for each quote, decide if anything should change at all
    //
    for ( UINT n = 0 ; n < m_lpQuotes->cStocks ; n ++ )
    {
        if (rand() < (RAND_MAX / (int)m_uiQuoteProb))
        {
            lpQ = &m_lpQuotes->rgStocks[n] ;
            // Randomize ask and volume
            // 
            change = (rand() < (RAND_MAX / (int)m_uiElementProb)) ? 250 : -125 ;
            if ((LONG)lpQ->ulLast + change > (LONG)0)
                lpQ->ulLast += change ;
            lpQ->uiMembers |= XSM_LAST ;
            
            if (lpQ->ulLast > lpQ->ulHigh)
            {
                lpQ->ulHigh = lpQ->ulLast ;
                lpQ->uiMembers |= XSM_HIGH ;
            }    
            if (lpQ->ulLast < lpQ->ulLow)
            {
                lpQ->ulLow = lpQ->ulLast ;
                lpQ->uiMembers |= XSM_LOW ;
            }
                
            lpQ->ulVolume += (abs(change)/2) ;
            lpQ->uiMembers |= XSM_VOLUME ;
        }
    }

    UpdateAllViews( NULL ) ;
    
    for ( n = 0 ; n < m_lpQuotes->cStocks ; n ++ )
    {
        m_lpQuotes->rgStocks[n].uiMembers = 0 ;
    }    
}


/////////////////////////////////////////////////////////////////////////////
// CXRTData diagnostics

#ifdef _DEBUG
void CXRTData::AssertValid() const
{
    CDocument::AssertValid();
}

void CXRTData::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXRTData commands
