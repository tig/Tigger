// control.cpp : Implementation of the CWinPrintControl OLE control class.

#include "stdafx.h"
#include "winprt20.h"
#include "headerse.h"
#include "control.h"
#include "ctlppg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Defaults used by :OnResetState
// 
const char DEFAULT_CONFIGNAME[]         = "Factory Default" ;
const char DEFAULT_FILETYPEDRIVER[]     = "ANSI Text" ;

const BOOL DEFAULT_USEDEFAULTPRINTER    = FALSE ;
const BOOL DEFAULT_USEPRINTERSETTINGS   = FALSE ; 
                                             
// note: these are ignored, the default printer is queried for
// defaults to use.
//
const long DEFAULT_ORIENTATION          = 0 ;
const long DEFAULT_PAPERSIZE            = 0 ;
const long DEFAULT_PAPERLENGTH          = 0 ;
const long DEFAULT_SCLALE               = 0 ;
const long DEFAULT_COPIES               = 0 ;
const long DEFAULT_DEFAULTSOURCE        = 0 ;
const long DEFAULT_PRINTQUALITY         = 0 ;
const long DEFAULT_COLOR                = 0 ;
const long DEFAULT_DUPLEX               = 0 ;

const float DEFAULT_TOPMARGIN           = 0.5 ;
const float DEFAULT_BOTTOMMARGIN        = 0.5 ;
const float DEFAULT_LEFTMARGIN          = 0.5 ;
const float DEFAULT_RIGHTMARGIN         = 0.5 ;

const BOOL DEFAULT_BORDER               = TRUE ;
const BOOL DEFAULT_SHADE                = FALSE ;
    
const short DEFAULT_ROWS                = 1 ;
const short DEFAULT_COLUMNS             = 1 ;
    
const float DEFAULT_HORZSEPARATION      = 0.125 ;
const float DEFAULT_VERTSEPARATION      = 0.125 ;

IMPLEMENT_DYNCREATE(CWinPrintControl, COleControl)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CWinPrintControl, COleControl)
    //{{AFX_MSG_MAP(CWinPrintControl)
    // NOTE - ClassWizard will add and remove message map entries
    //    DO NOT EDIT what you see in these blocks of generated code !
    ON_OLEVERB(IDS_PROPERTIESVERB, OnProperties)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CWinPrintControl, COleControl)
    //{{AFX_DISPATCH_MAP(CWinPrintControl)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "Printer", m_strPrinter, OnPrinterChanged, VT_BSTR)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "PrinterDriver", m_strPrinterDriver, OnPrinterDriverChanged, VT_BSTR)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "PrinterOutput", m_strPrinterOutput, OnPrinterOutputChanged, VT_BSTR)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "UseDefaultPrinter", m_fDefaultPrinter, OnUseDefaultPrinterChanged, VT_BOOL)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "UsePrinterSettings", m_fUsePrinterSettings, OnUsePrinterSettingsChanged, VT_BOOL)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "Orientation", m_lOrientation, OnOrientationChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "PaperSize", m_lPaperSize, OnPaperSizeChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "PaperLength", m_lPaperLength, OnPaperLengthChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "PaperWidth", m_lPaperWidth, OnPaperWidthChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "Scale", m_lScale, OnScaleChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "Copies", m_lCopies, OnCopiesChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "DefaultSource", m_lDefaultSource, OnDefaultSourceChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "PrintQuality", m_lPrintQuality, OnPrintQualityChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "Color", m_lColor, OnColorChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "Duplex", m_lDuplex, OnDuplexChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "TopMargin", m_flTopMargin, OnTopMarginChanged, VT_R4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "BottomMargin", m_flBottomMargin, OnBottomMarginChanged, VT_R4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "LeftMargin", m_flLeftMargin, OnLeftMarginChanged, VT_R4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "RightMargin", m_flRightMargin, OnRightMarginChanged, VT_R4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "Border", m_fBorder, OnBorderChanged, VT_BOOL)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "Shade", m_fShade, OnShadeChanged, VT_BOOL)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "Rows", m_sRows, OnRowsChanged, VT_I2)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "Columns", m_sColumns, OnColumnsChanged, VT_I2)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "HorzSeparation", m_flHorzSeparation, OnHorzSeparationChanged, VT_R4)
    DISP_PROPERTY_NOTIFY(CWinPrintControl, "VertSeparation", m_flVertSeparation, OnVertSeparationChanged, VT_R4)
    DISP_PROPERTY_EX(CWinPrintControl, "ConfigName", GetConfigName, SetNotSupported, VT_BSTR)
    DISP_PROPERTY_EX(CWinPrintControl, "FileTypeDriverName", GetFileTypeDriverName, SetFileTypeDriverName, VT_BSTR)
    DISP_PROPERTY_EX(CWinPrintControl, "FileTypeDriver", GetFileTypeDriver, SetNotSupported, VT_DISPATCH)
    DISP_PROPERTY_EX(CWinPrintControl, "Header", GetHeader, SetHeader, VT_DISPATCH)
    DISP_PROPERTY_EX(CWinPrintControl, "Footer", GetFooter, SetFooter, VT_DISPATCH)
    DISP_FUNCTION(CWinPrintControl, "LoadConfiguration", LoadConfiguration, VT_BOOL, VTS_BSTR)
    DISP_FUNCTION(CWinPrintControl, "SaveConfiguration", SaveConfiguration, VT_BOOL, VTS_BSTR)
    DISP_FUNCTION(CWinPrintControl, "SetSource", SetSource, VT_EMPTY, VTS_BSTR)
    DISP_FUNCTION(CWinPrintControl, "Print", Print, VT_BOOL, VTS_VARIANT)
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CWinPrintControl, COleControl)
    //{{AFX_EVENT_MAP(CWinPrintControl)
    // NOTE - ClassWizard will add and remove event map entries
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// To add another property page to your control:
//  1. Increase the count in the BEGIN_PROPPAGEIDS macro
//  2. Add a PROPPAGEID entry containing the class ID of the page.

BEGIN_PROPPAGEIDS(CWinPrintControl, 1)
    PROPPAGEID(CControlPropPage::guid)
END_PROPPAGEIDS(CWinPrintControl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CWinPrintControl, "WinPrint20.Control.1",
    0x2a501, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x46)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CWinPrintControl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID IID_IWinPrintControl =
        { 0x0002A503, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0, 0x0, 0x46 } };
const IID IID_IWinPrintControlEvents =
        { 0x0002A504, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0, 0x0, 0x46 } };

/////////////////////////////////////////////////////////////////////////////
// CWinPrintControl::CWinPrintControlFactory::UpdateRegistry -
// Adds or removes system registry entries for CWinPrintControl

void CWinPrintControl::CWinPrintControlFactory::UpdateRegistry(BOOL bRegister)
{
    AfxOleUnregisterClass(m_clsid, m_lpszProgID);

    if (bRegister)
        AfxOleRegisterControlClass(
            AfxGetInstanceHandle(),
            m_clsid,
            m_lpszProgID,
            IDS_WINPRINTCONTROL,
            IDB_WINPRINTCONTROL,
            TRUE,                   // Insertable
            OLEMISC_INVISIBLEATRUNTIME |
            OLEMISC_ACTIVATEWHENVISIBLE |
            OLEMISC_INSIDEOUT |
            OLEMISC_CANTLINKINSIDE |
            OLEMISC_RECOMPOSEONRESIZE);
}


/////////////////////////////////////////////////////////////////////////////
// Licensing strings

static const char BASED_CODE _szLicFileName[] = "WINPRT20.LIC";

static const char BASED_CODE _szLicString[] =
    "Copyright (c) 1994 ";


/////////////////////////////////////////////////////////////////////////////
// CWinPrintControl::CWinPrintControlFactory::VerifyUserLicense -
// Checks for existence of a user license

BOOL CWinPrintControl::CWinPrintControlFactory::VerifyUserLicense()
{
    return AfxVerifyLicFile(AfxGetInstanceHandle(), _szLicFileName,
        (LPBYTE)_szLicString);
}


/////////////////////////////////////////////////////////////////////////////
// CWinPrintControl::CWinPrintControlFactory::GetLicenseKey -
// Returns a runtime licensing key

BOOL CWinPrintControl::CWinPrintControlFactory::GetLicenseKey(DWORD dwReserved,
    BSTR FAR* pbstrKey)
{
    UNUSED dwReserved ;
 
    *pbstrKey = SysAllocString(_szLicString);
    return (*pbstrKey != NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CWinPrintControl::CWinPrintControl - Constructor

CWinPrintControl::CWinPrintControl() 
{
    InitializeIIDs(&IID_IWinPrintControl, &IID_IWinPrintControlEvents);
    
    m_Page.m_pControl = this ;
    m_Input.m_pControl = this ;
    
    m_Header.SetName("Header") ;
    m_Header.m_pControl = this ;
    m_Footer.SetName("Footer") ;
    m_Footer.m_pControl = this ;

    m_pdispFileTypeDriver = NULL ;

    m_fUsePrintOffset = TRUE ;  // ITEM: provide UI/VBA access in future ver.

    SetInitialSize( 150, 150 ) ;
    //OnResetState() ;        // When used as a pure Auto object                
}


/////////////////////////////////////////////////////////////////////////////
// CWinPrintControl::~CWinPrintControl - Destructor

CWinPrintControl::~CWinPrintControl()
{   
    // Clear Stream object map
    //
    COleStreamFile* pStmObj ;
    CString str ;

    for (POSITION pos = m_mapStrToStm.GetStartPosition() ; pos != NULL ;)
    { 
        m_mapStrToStm.GetNextAssoc( pos, str, (CObject*&)pStmObj ) ;
        if (pStmObj)
            delete pStmObj ;
    }
    m_mapStrToStm.RemoveAll() ;        

    if (m_pdispFileTypeDriver != NULL)
       m_pdispFileTypeDriver->Release() ;
       
}


/////////////////////////////////////////////////////////////////////////////
// CWinPrintControl::OnDraw - Drawing function
const int XTRA_SPACE =      5 ;
const int SHADOW_WIDTH =    5 ;

void CWinPrintControl::OnDraw(
            CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{   
    CRect   rcClient = rcBounds ;   // client area of window
    CRect   rcPage ;                // the "page"
    CRect   rc ;                    // utiltiy ;
    
    // OnDraw is called in one of two ways. 1) when the control is
    // active, it is called by the WM_PAINT handler.  In this case 
    // rcBounds starts at (0,0).   2) when the control is inactive
    // and the framework calls it to draw into the metafile.  In
    // this case no assumptions can be made about rcBounds.x & y.
    //
    
    UNUSED rcInvalid ;

    if (pdc->GetDeviceCaps( TECHNOLOGY ) == DT_METAFILE)
    {
        m_Page.Draw( pdc ) ;
        return ;
    }
    
    // The client area of the WinPrint contol contains a preview of the 
    // current configuration.   
    //
    // The ::OnDraw method draws an image of a page on the client area
    // with a grey background (setable property?).  It then sets up the
    // DC so that the viewport and extents match the printer and calls
    // the ::DrawPage method.
    //

    pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));

    pdc->TextOut( 2, 2, m_strFrameTitle ) ;
    if (m_bInPlaceActive)
        pdc->TextOut( 2, 18, "InPlaceActive" ) ;
          
    if (m_bUIActive)
        pdc->TextOut( 2, 34, "UIActive" ) ;
      
    // Provide some space between the edge of the window and the page
    //
    rcClient.InflateRect( -XTRA_SPACE*2, -XTRA_SPACE*2 ) ;
                  
    // Setup ISOTROPIC map mode if physical page won't fit in our window
    //
    if ((m_Page.m_iResolutionX + (2 * m_Page.m_ptPrintOffset.x) >= rcClient.Width()) &&
        (m_Page.m_iResolutionY + (2 * m_Page.m_ptPrintOffset.y) >= rcClient.Height()))
    {
        pdc->SetMapMode( MM_ISOTROPIC ) ;
        pdc->SetWindowOrg(0,0) ;
        pdc->SetViewportOrg(0,0) ;
        pdc->SetWindowExt( m_Page.m_iResolutionX + (2 * m_Page.m_ptPrintOffset.x), 
                           m_Page.m_iResolutionY + (2 * m_Page.m_ptPrintOffset.y) ) ;
        pdc->SetViewportExt( rcClient.Size() ) ;                           
    }

    // Make rcPage be in device units and use to call SetViewportOrg.  This
    // set's things up so drawing to the logical coord (0,0), will result
    // in drawing to the top right corner of our 'page' within the client
    // area.
    //
    rcPage.left = rcPage.top = 0 ;
    rcPage.right = m_Page.m_iResolutionX - 1 ;
    rcPage.bottom = m_Page.m_iResolutionY - 1 ;
    pdc->LPtoDP( rcPage ) ;
    pdc->SetViewportOrg( rcClient.left + (rcClient.Width() - rcPage.Width()) / 2 ,
                         rcClient.top + (rcClient.Height() - rcPage.Height()) / 2  ) ;

    // Draw the border around the page.  Drop shadow on bottom right.
    //
    pdc->SelectStockObject(BLACK_PEN) ;
    rc.SetRect( -1, -1, m_Page.m_iResolutionX, m_Page.m_iResolutionY ) ;
    rc.InflateRect( m_Page.m_ptPrintOffset.x, m_Page.m_ptPrintOffset.y ) ;
    pdc->Rectangle( rc ) ;
    
    // Shadow (bottom)
    // 
    pdc->SelectStockObject( NULL_PEN ) ;
    pdc->SelectStockObject( GRAY_BRUSH ) ;
    pdc->LPtoDP( rc ) ;
    rc.left += SHADOW_WIDTH ;
    rc.right += SHADOW_WIDTH ;
    rc.top  = rc.bottom ;
    rc.bottom += SHADOW_WIDTH ;
    pdc->DPtoLP( rc ) ;
    pdc->Rectangle( rc ) ;

    // Shadow (right)
    // 
    rc.SetRect( -1, -1, (int)m_Page.m_iResolutionX, (int)m_Page.m_iResolutionY ) ;
    rc.InflateRect( m_Page.m_ptPrintOffset.x, m_Page.m_ptPrintOffset.y ) ;
    pdc->LPtoDP( rc ) ;
    rc.left = rc.right ;
    rc.right += SHADOW_WIDTH ;
    rc.top += SHADOW_WIDTH ;
    rc.bottom += SHADOW_WIDTH ;
    pdc->DPtoLP( rc ) ;
    pdc->Rectangle( rc ) ;

    // Now draw
    //
    m_Page.Draw( pdc ) ;

}

void CWinPrintControl::Serialize(CArchive& ar)
{
    FORMATETC   fetc ;
    STGMEDIUM   stgm ;
    DWORD       cb ;
    BYTE FAR*   pbPropset ;
    ULARGE_INTEGER uliStart;
    ULARGE_INTEGER uliEnd;
    LARGE_INTEGER liZero = {0,0};    
    
    memset( &fetc, 0, sizeof(fetc) ) ;
    memset( &stgm, 0, sizeof(stgm) ) ;
    fetc.tymed = TYMED_ISTREAM ;
    stgm.tymed = TYMED_ISTREAM ;
    
    if (ar.IsLoading())
    {
        ar >> cb ;
        pbPropset = new BYTE[cb] ;
        if (pbPropset)
        {
            ar.Read( pbPropset, (UINT)cb ) ;
            stgm.pstm = _AfxCreateMemoryStream() ;
            if (stgm.pstm)
            {
                stgm.pstm->Write( pbPropset, cb, NULL ) ; 
                stgm.pstm->Seek( liZero, STREAM_SEEK_SET, &uliStart ) ;
                SetPropsetData( &fetc, &stgm, CLSID_PersistPropset ) ;
                stgm.pstm->Release() ;
                //StockPropsUpdated() ;
                InvalidateControl() ;
            }
        }
        delete []pbPropset ;
    }
    else
    {
        stgm.pstm = _AfxCreateMemoryStream() ;
        if (stgm.pstm)
        {
            GetPropsetData( &fetc, &stgm, CLSID_PersistPropset ) ;
            stgm.pstm->Seek( liZero, STREAM_SEEK_SET, &uliStart ) ;
            if (SUCCEEDED(stgm.pstm->Seek(liZero, STREAM_SEEK_CUR, &uliStart)) &&
                SUCCEEDED(stgm.pstm->Seek(liZero, STREAM_SEEK_END, &uliEnd)) &&
                SUCCEEDED(stgm.pstm->Seek(*(LARGE_INTEGER*)&uliStart, STREAM_SEEK_SET, NULL)))
            {
                cb = uliEnd.LowPart - uliStart.LowPart;
                pbPropset = new BYTE[cb] ;
                stgm.pstm->Read( pbPropset, cb, NULL ) ;
                stgm.pstm->Release() ;

                ar << cb ;
                ar.Write( pbPropset, (UINT)cb ) ;
                delete [] pbPropset ;
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// CWinPrintControl::DoPropExchange - Persistence support

void CWinPrintControl::DoPropExchange(CPropExchange* pPX)
{
    COleControl::DoPropExchange(pPX);

    PX_String( pPX, "ConfigName", m_strConfigName ) ;
    if (pPX->m_bLoading)
        m_strFrameTitle = m_strConfigName ;
    
    ExchangeFileTypeDriver( pPX ) ;

    ExchangePrinterInfo( pPX ) ;
    
    PX_Float( pPX, "TopMargin", m_flTopMargin );
    PX_Float( pPX, "BottomMargin", m_flBottomMargin );
    PX_Float( pPX, "LeftMargin", m_flLeftMargin );
    PX_Float( pPX, "RightMargin", m_flRightMargin );

    PX_Bool( pPX, "Border", m_fBorder );
    PX_Bool( pPX, "Shade", m_fShade );
    
    // REVIEW:  Need to provide shade color

    PX_Property( pPX, "Header", &m_Header ) ;
    PX_Property( pPX, "Footer", &m_Footer ) ;
    
    PX_Short( pPX, "Rows", m_sRows );
    PX_Short( pPX, "Columns", m_sColumns );
    
    PX_Float( pPX, "HorzSeparation", m_flHorzSeparation );
    PX_Float( pPX, "VertSeparation", m_flVertSeparation );
    
    if (pPX->m_bLoading)
    {
        m_Page.Recalc() ;
    }    
}

void CWinPrintControl::ExchangeFileTypeDriver( CPropExchange* pPX )
{    
    HRESULT hr ;
    
    PX_String( pPX, "FileTypeDriver", m_strFileTypeDriver ) ;
    if (pPX->m_bLoading)
    {
        if (m_pdispFileTypeDriver != NULL) m_pdispFileTypeDriver->Release() ;
        m_pdispFileTypeDriver = NULL ;

        COleStreamFile* pStmObj ;
        CString         str ;
        
        // Clear map
        for (POSITION pos = m_mapStrToStm.GetStartPosition() ; pos != NULL ;)
        { 
            m_mapStrToStm.GetNextAssoc( pos, str, (CObject*&)pStmObj ) ;
            if (pStmObj)
                delete pStmObj ;
        }        
        m_mapStrToStm.RemoveAll() ;        
        
        // For each FTD that is installed try to read it's state property as a
        // BLOB.  Convert this BLOB to a stream and store that stream in
        // the map.
        
        // TODO:  Write code to enum regdb (for now we just put m_strFTD in the
        // map as a hack)
        //
        // REVIEW:  Can we cache the list of installed FTDs somehow for performance?
        // otherwise loading is going to be slow if we have to search the entire
        // regdb for all FTDs!
        //
        void*    pvBlob = NULL ;
        for (int i = 0 ; i < 1 ; i++)       //HACK!!!
        {
            str = "FTD." + m_strFileTypeDriver ;   //HACK!!!
            
//            PX_Blob( pPX, str, pvBlob, NULL ) ;
            if (pvBlob != NULL)
            {   
                pStmObj = new COleStreamFile ;

                ASSERT_VALID(pStmObj) ;
                pStmObj->CreateMemoryStream() ;
                ASSERT_POINTER(pStmObj->m_lpStream, IStream) ;

                pStmObj->m_lpStream->Write( pvBlob, *(ULONG*)pvBlob, NULL ) ;
                m_mapStrToStm.SetAt( m_strFileTypeDriver, pStmObj ) ;
            }
        }
                        
        // For the FTD specified by m_strFileTypeDriver, load the state
        //
        if (m_mapStrToStm.Lookup( m_strFileTypeDriver, (CObject*&)pStmObj ) != 0)
            hr = ftdLoad( m_strFileTypeDriver, pStmObj->m_lpStream, IID_IDispatch, (LPVOID FAR*)&m_pdispFileTypeDriver ) ;        
        else
            hr = ftdLoad( m_strFileTypeDriver, NULL, IID_IDispatch, (LPVOID FAR*)&m_pdispFileTypeDriver ) ;        

        // REVIEW:  Error checking here?
    }
    else
    {
        COleStreamFile* pStmObj ;

        // Tell current ftd to write to stream
        //
        if (m_pdispFileTypeDriver != NULL)
        {
            // Look up the appropriate stream.  If it exists, save to it
            // otherwise create one and save to it.
            if (m_mapStrToStm.Lookup( m_strFileTypeDriver, (CObject*&)pStmObj ) == 0)
            {
                // Does not exist.  Add it.
                pStmObj = new COleStreamFile() ;
                pStmObj->CreateMemoryStream() ;
                m_mapStrToStm.SetAt( m_strFileTypeDriver, pStmObj ) ;
            }
            
            ASSERT_VALID(pStmObj) ;
            ASSERT_POINTER(pStmObj->m_lpStream, IStream) ;
            
            hr = ftdSave( pStmObj->m_lpStream, m_pdispFileTypeDriver ) ;
            if (FAILED(hr))
            {
                TRACE("%s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
                TRACE("Could not save file type driver's persistent state to stream\n" ) ;
                if (m_mapStrToStm.Lookup( m_strFileTypeDriver, (CObject*&)pStmObj ) != 0)
                {
                    m_mapStrToStm.RemoveKey( m_strFileTypeDriver ) ;
                    if (pStmObj)
                        delete pStmObj ;
                }
            }
        }    
        
        // For each stream in the map save
        //
        ULARGE_INTEGER  uliStart;
        ULARGE_INTEGER  uliEnd;
        CString str ;
        for (POSITION pos = m_mapStrToStm.GetStartPosition() ; pos != NULL ;)
        { 
            m_mapStrToStm.GetNextAssoc( pos, str, (CObject*&)pStmObj ) ;

            ASSERT_VALID(pStmObj) ;
            ASSERT_POINTER(pStmObj->m_lpStream, IStream) ;

            LARGE_INTEGER   liZero = {0,0} ;                        
            if (SUCCEEDED(pStmObj->m_lpStream->Seek(liZero, STREAM_SEEK_CUR, &uliStart)) &&
                SUCCEEDED(pStmObj->m_lpStream->Seek(liZero, STREAM_SEEK_END, &uliEnd)) &&
                SUCCEEDED(pStmObj->m_lpStream->Seek(*(LARGE_INTEGER*)&uliStart, STREAM_SEEK_SET, NULL)))
            {
                DWORD cb = uliEnd.LowPart - uliStart.LowPart;
                BYTE FAR* pbData = new BYTE[cb+sizeof(DWORD)];
                *(DWORD*)pbData = cb;
                if (SUCCEEDED(pStmObj->m_lpStream->Read(pbData+sizeof(DWORD), cb, NULL)))
                {
                    str = "FTD." + str ;  

                    PX_Blob(pPX, str, (void *&)pbData, NULL ) ;
                }
                delete [] pbData;
            }
        }        
    }
}
  
void CWinPrintControl::ExchangePrinterInfo( CPropExchange* pPX )
{    
    PX_Bool( pPX, "UseDefaultPrinter", m_fDefaultPrinter ) ;
    PX_Bool( pPX, "UsePrinterSettings", m_fUsePrinterSettings ) ;
    if (pPX->m_bLoading)
    {
        // REVIEW:  What does "Default Printer" mean?  Does it mean you get
        // the default printer, but custom settings, or does it mean you
        // get default printer with it's default settings?    Currently
        // we save settings.
        //
        if (m_fDefaultPrinter)
        {
            // If pPX is an archive exhange we have to make sure we read
            // everything we wrote...
            //
            CString str ;
            PX_String( pPX, "Printer", str ) ;
            PX_String( pPX, "PrinterDriver", str) ;
            PX_String( pPX, "PrinterOutput", str ) ;
        }
        else
        {
            PX_String( pPX, "Printer", m_strPrinter ) ;
            PX_String( pPX, "PrinterDriver", m_strPrinterDriver ) ;
            PX_String( pPX, "PrinterOutput", m_strPrinterOutput ) ;
        }
    }
    else
    {
        PX_String( pPX, "Printer", m_strPrinter ) ;
        PX_String( pPX, "PrinterDriver", m_strPrinterDriver ) ;
        PX_String( pPX, "PrinterOutput", m_strPrinterOutput ) ;
    }

    // The UsePrinterSettings property tells the control to
    // always use the printer's settings for the properties below.
    // CPage::Recalc does the work of setting them during recalc.
    //        
    if (!pPX->m_bLoading || !m_fUsePrinterSettings)
    {                
        PX_Long( pPX, "Orientation", m_lOrientation );
        PX_Long( pPX, "PaperSize", m_lPaperSize );
        PX_Long( pPX, "PaperLength", m_lPaperLength );
        PX_Long( pPX, "PaperWidth", m_lPaperWidth );
        PX_Long( pPX, "Scale", m_lScale );
        PX_Long( pPX, "Copies", m_lCopies );
        PX_Long( pPX, "DefaultSource", m_lDefaultSource );
        PX_Long( pPX, "PrintQuality", m_lPrintQuality );
        PX_Long( pPX, "Color", m_lColor );
        PX_Long( pPX, "Duplex", m_lDuplex );
    }
    else
    {   
        // If pPX is an archive exhange we have to make sure we read
        // everything we wrote...
        //
        LONG    l ;
        PX_Long( pPX, "Orientation", l );
        PX_Long( pPX, "PaperSize", l );
        PX_Long( pPX, "PaperLength", l );
        PX_Long( pPX, "PaperWidth", l );
        PX_Long( pPX, "Scale", l );
        PX_Long( pPX, "Copies", l );
        PX_Long( pPX, "DefaultSource", l );
        PX_Long( pPX, "PrintQuality", l );
        PX_Long( pPX, "Color", l );
        PX_Long( pPX, "Duplex", l );
    }
}        

/////////////////////////////////////////////////////////////////////////////
// CWinPrintControl::OnResetState - Reset control to default state

void CWinPrintControl::OnResetState()
{
    m_strConfigName = DEFAULT_CONFIGNAME ;
    m_strFrameTitle = m_strConfigName ;
    m_strFileTypeDriver = DEFAULT_FILETYPEDRIVER ;

    m_fDefaultPrinter = DEFAULT_USEDEFAULTPRINTER ;
    m_fUsePrinterSettings = DEFAULT_USEPRINTERSETTINGS ;

    // Assume default printer and it's settings
    //
    CPrintDialog   pd( FALSE ) ;
    pd.GetDefaults() ;
    m_strPrinter = pd.GetDeviceName() ;
    m_strPrinterDriver = pd.GetDriverName() ;
    m_strPrinterOutput = pd.GetPortName() ;
    
    ASSERT(pd.m_pd.hDevNames) ;
    if (pd.m_pd.hDevMode)
    {
        m_lOrientation = (long)pd.GetDevMode()->dmOrientation ;
        m_lPaperSize = (long)pd.GetDevMode()->dmPaperSize ;
        m_lPaperLength = (long)pd.GetDevMode()->dmPaperLength ;
        m_lPaperWidth = (long)pd.GetDevMode()->dmPaperWidth ;
        m_lScale = (long)pd.GetDevMode()->dmScale ;
        m_lCopies= (long)pd.GetDevMode()->dmCopies ;
        m_lDefaultSource = (long)pd.GetDevMode()->dmDefaultSource ;
        m_lPrintQuality= (long)pd.GetDevMode()->dmPrintQuality ;
        m_lColor= (long)pd.GetDevMode()->dmColor ;
        m_lDuplex = (long)pd.GetDevMode()->dmDuplex ;
        GlobalFree( pd.m_pd.hDevMode ) ;
    }
    if (pd.m_pd.hDevNames)
        GlobalFree( pd.m_pd.hDevNames ) ;

    // Clear Stream object map
    //
    COleStreamFile* pStmObj ;
    CString str ;
    for (POSITION pos = m_mapStrToStm.GetStartPosition() ; pos != NULL ;)
    { 
        m_mapStrToStm.GetNextAssoc( pos, str, (CObject*&)pStmObj ) ;
        if (pStmObj)
            delete pStmObj ;
    }        
    m_mapStrToStm.RemoveAll() ;        

    if (m_pdispFileTypeDriver != NULL)
       m_pdispFileTypeDriver->Release() ;
    
    m_flTopMargin = DEFAULT_TOPMARGIN ;
    m_flBottomMargin = DEFAULT_BOTTOMMARGIN ;
    m_flLeftMargin = DEFAULT_LEFTMARGIN ;
    m_flRightMargin = DEFAULT_RIGHTMARGIN ;

    m_fBorder = DEFAULT_BORDER ;
    m_fShade = DEFAULT_SHADE ;
    
    // REVIEW:  Need to provide shade color

    m_Header.OnResetState() ;
    m_Footer.OnResetState() ;
    
    m_sRows = DEFAULT_ROWS ;
    m_sColumns = DEFAULT_COLUMNS ;
    
    m_flHorzSeparation = DEFAULT_HORZSEPARATION ;
    m_flVertSeparation = DEFAULT_HORZSEPARATION ;

    m_Page.Recalc() ;

    COleControl::OnResetState();  // Resets defaults found in DoPropExchange
}
  
// CWinPrintControl OLE Automation Handlers
//

// LoadConfiguration loads from the registry (Win32) or from
// the WinPrint config file (Win16).
//
BOOL CWinPrintControl::LoadConfiguration(LPCSTR szConfigName) 
{
    BOOL        bSuccess = FALSE ;
    HRESULT     hr ;
    LPSTORAGE   pstgConfig = NULL ;

    #ifdef WIN32
    // TODO:  In the Win32 case we pull the stream out of the registry.  It is stored
    // as a REG_BINARY key.
    //
    
    #else
    // In the Win16 case we use a file (DocFile) to store
    // all config data.  This file resides in the same dir as
    // the WINPRT20.DLL.
    //
    char szPathName[_MAX_PATH];
    ::GetModuleFileName(AfxGetInstanceHandle(), szPathName, _MAX_PATH);
    LPSTR pszFileName = _fstrrchr(szPathName, '\\') + 1;
    _fstrcpy(pszFileName, g_WinPrint.m_strConfigFileName);
    
    LPSTORAGE pstg = NULL ;
    hr = StgOpenStorage( szPathName, NULL, STGM_DIRECT | STGM_READ | 
                             STGM_SHARE_EXCLUSIVE, NULL, 0, &pstg);
    if (FAILED(hr))
    {
        TRACE("%s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
        TRACE("Could not open configuration file.\n") ;
        return FALSE ;
    }        

    if (NULL != pstg)
    {
        CLSID       clsID ;
        
        // Did we write this file???
        ReadClassStg( pstg, &clsID ) ;
            
        CLSID clsidCtl ; GetClassID( &clsidCtl ) ;
        if (!IsEqualCLSID( clsID, clsidCtl ))
        {
            pstg->Release();
            TRACE( "File is not a WinPrint 2.0 configuration file. Could not open file.\n") ;
            return FALSE ;
        }
            
        // Read from storage
        hr = pstg->OpenStorage( szConfigName,
                                    NULL,
                                    STGM_READ|STGM_DIRECT|STGM_SHARE_EXCLUSIVE,
                                    0L, 0L,
                                    &pstgConfig ) ;
                             
        if (FAILED(hr))
        {
            TRACE("Could not open storage containing configuration. %s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
            pstg->Release();
            AfxThrowOleDispatchException( 0, "Could not open stream containing configuration.") ;
        }
    }
    #endif

    LPPERSISTSTORAGE    lpPersistStorage ;
    hr = (HRESULT)ExternalQueryInterface( (LPVOID)&IID_IPersistStorage,  (LPVOID FAR*)&lpPersistStorage ) ;
    if (FAILED(hr))
    {
        pstg->Release() ;
        pstgConfig->Release() ;
        TRACE("QI for IID_IPersistStorage failed. %s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
        AfxThrowOleDispatchException( 0, "QI for IID_IPersistStorage failed.") ;
    }

    hr = lpPersistStorage->Load( pstgConfig ) ;
    if (FAILED(hr))
    {
        pstg->Release() ;
        pstgConfig->Release() ;
        lpPersistStorage->Release() ;
        TRACE("IPersistStorage::Load failed. %s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
        AfxThrowOleDispatchException( 0, "Load failed.") ;
    }

    lpPersistStorage->Release() ;
    pstgConfig->Release() ;
    pstg->Release() ;
    bSuccess = TRUE ;
  
    SetModifiedFlag( FALSE ) ;     // start off with unmodified
    m_Page.Recalc() ;

    return bSuccess ;
}

// SaveConfiguration saves to the registry (Win32) or to
// the WinPrint config file (Win16).
//
BOOL CWinPrintControl::SaveConfiguration(LPCSTR szConfigName) 
{
    BOOL        bSuccess = FALSE ;
    LPSTREAM    pstm = NULL ;
    LPSTORAGE   pstgConfig = NULL ;
    HRESULT     hr ;
    CString     strConfigName ;
    
    strConfigName = m_strConfigName ;
    m_strConfigName = szConfigName ;
    
    #ifdef WIN32
    // Create a memory stream, ::GetPropsetData it, convert it to a BLOB
    // and write it to the registry
    
    #else 
    char szPathName[_MAX_PATH] ;
    ::GetModuleFileName(AfxGetInstanceHandle(), szPathName, _MAX_PATH);
    LPSTR pszFileName = _fstrrchr(szPathName, '\\') + 1;
    _fstrcpy(pszFileName, g_WinPrint.m_strConfigFileName);

    LPSTORAGE   pstg ;
    // Open the storage (create if needed)
    if (NOERROR != StgIsStorageFile( szPathName ))
    {
        hr = StgCreateDocfile( szPathName, STGM_DIRECT | STGM_READWRITE | 
                              STGM_SHARE_EXCLUSIVE, 0, &pstg );
        if (FAILED(hr))
        {
            m_strConfigName = strConfigName ;
            TRACE("%s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
            AfxThrowOleDispatchException( 0, "Could not create DocFile." ) ;
        }                              
        CLSID clsidCtl ; GetClassID( &clsidCtl ) ;
        WriteClassStg( pstg, clsidCtl ) ;

        // Create the mandatory CONTENTS stream
        //
        hr = pstg->CreateStream( "Contents", STGM_DIRECT | STGM_READWRITE | STGM_CREATE |
                                    STGM_SHARE_EXCLUSIVE, 0L, 0L, &pstm ) ;
        if (FAILED(hr))
        {
            m_strConfigName = strConfigName ;
            TRACE("%s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
            AfxThrowOleDispatchException( 0, "Could not open CONTENTS stream.") ;
        }
        pstm->Release() ;

    }
    else
    {
        hr = StgOpenStorage( szPathName, NULL, STGM_DIRECT | STGM_READWRITE | 
                                 STGM_SHARE_EXCLUSIVE, NULL, 0, &pstg);
        if (FAILED(hr))
        {   
            m_strConfigName = strConfigName ;
            TRACE("%s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
            AfxThrowOleDispatchException( 0, "Could not open configuration file.") ;
        }

        CLSID       clsID ;
        // Did we write this file???
        ReadClassStg( pstg, &clsID ) ;
            
        CLSID clsidCtl ; GetClassID( &clsidCtl ) ;
        if (!IsEqualCLSID( clsID, clsidCtl ))
        {
            m_strConfigName = strConfigName ;
            pstg->Release();
            TRACE( "File is not a WinPrint 2.0 configuration file. Could not open file.\n") ;
            AfxThrowOleDispatchException( 0, "File is not a WinPrint 2.0 configuration file. Could not open file.\n") ;
        }
    }    
    
    hr = pstg->CreateStorage( m_strConfigName, STGM_CREATE|STGM_READWRITE|STGM_TRANSACTED|STGM_SHARE_EXCLUSIVE, 
                            0L, 0L, &pstgConfig ) ;
    if (FAILED(hr))
    {
        m_strConfigName = strConfigName ;
        pstg->Release() ;
        TRACE("Could not open sub-storage for config data. %s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
        AfxThrowOleDispatchException( 0, "Could not open sub-storage for config data.") ;
    }
    #endif

    // Write to the stream
    ASSERT_POINTER(pstgConfig, IStorage) ;
  
    LPPERSISTSTORAGE    lpPersistStorage ;
    hr = (HRESULT)ExternalQueryInterface( (LPVOID)&IID_IPersistStorage,  (LPVOID FAR*)&lpPersistStorage ) ;
    if (FAILED(hr))
    {
        m_strConfigName = strConfigName ;
        pstg->Release() ;
        pstgConfig->Release() ;
        TRACE("QI for IID_IPersistStorage failed. %s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
        AfxThrowOleDispatchException( 0, "QI for IID_IPersistStorage failed.") ;
    }

    hr = ::OleSave( lpPersistStorage, pstgConfig, FALSE );
    if (FAILED(hr))
    {
        m_strConfigName = strConfigName ;
        pstg->Release() ;
        pstgConfig->Release() ;
        lpPersistStorage->Release() ;
        TRACE("::OleSave failed. %s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
        AfxThrowOleDispatchException( 0, "OleSave failed.") ;
    }

    lpPersistStorage->Release() ;

    pstgConfig->Commit( STGC_DEFAULT ) ;
    pstgConfig->Release() ;

    pstg->Commit( STGC_DEFAULT ) ;
    pstg->Release() ;
    
    bSuccess = TRUE ;

    if (bSuccess == FALSE)
        m_strConfigName = strConfigName ;

    m_strFrameTitle = m_strConfigName ;
    return bSuccess ;
}

void CWinPrintControl::SetSource(LPCSTR SourceFile) 
{
    UNUSED SourceFile ;
    
    // TODO: Add your dispatch handler code here

}

BOOL CWinPrintControl::Print(const VARIANT FAR& Source) 
{
    UNUSED Source ;
    // TODO: Add your dispatch handler code here

    return TRUE;
}

BSTR CWinPrintControl::GetConfigName() 
{
    return m_strConfigName.AllocSysString();
}

//void CWinPrintControl::SetConfigName(LPCSTR lpszNewValue) 
//{                                          
//    m_strConfigName = lpszNewValue ;
//    SetModifiedFlag();
//}

BSTR CWinPrintControl::GetFileTypeDriverName() 
{
    return m_strFileTypeDriver.AllocSysString();
}

void CWinPrintControl::SetFileTypeDriverName(LPCSTR lpszNewValue) 
{   
    // This is the same as doing GetFileTypeDriver using a name
    //
    LPDISPATCH lpdisp = NULL ;

    COleStreamFile*  pStmObj ;
    if (m_pdispFileTypeDriver != NULL)
    {
        // Setting a new FTD, must save the old one first
        //
        // Look up the appropriate stream.  If it exists, save to it
        // otherwise create one and save to it.
        if (m_mapStrToStm.Lookup( m_strFileTypeDriver, (CObject*&)pStmObj ) == 0)
        {
            // Does not exist.  Add it.
            pStmObj = new COleStreamFile() ;
            m_mapStrToStm.SetAt( m_strFileTypeDriver, pStmObj ) ;
        }
            
        ASSERT_VALID(pStmObj) ;
        ASSERT_POINTER(pStmObj->m_lpStream, IStream) ;
        HRESULT hr = ftdSave( pStmObj->m_lpStream, m_pdispFileTypeDriver ) ;
        if (FAILED(hr))
        {
            if (m_mapStrToStm.Lookup( m_strFileTypeDriver, (CObject*&)pStmObj ) != 0)
            {
                m_mapStrToStm.RemoveKey( m_strFileTypeDriver ) ;
                if (pStmObj)
                    delete pStmObj ;
            }
            TRACE("%s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
            AfxThrowOleDispatchException(0,"Could not save File Type Driver's persistent state.") ;
        }
    }    
        
    // Load the FTD.  If a stream exists for this FTD then use it, otherwise
    // dont...
    //
    pStmObj = NULL ;
    m_mapStrToStm.Lookup( lpszNewValue, (CObject*&)pStmObj ) ;

    HRESULT hr ;
    if (pStmObj)
    {
        ASSERT_VALID(pStmObj) ;
        ASSERT_POINTER(pStmObj->m_lpStream, IStream) ;
        hr = ftdLoad( lpszNewValue, pStmObj->m_lpStream, IID_IDispatch, (LPVOID FAR *)&lpdisp ) ;
    }            
    else
        hr = ftdLoad( lpszNewValue, NULL, IID_IDispatch, (LPVOID FAR *)&lpdisp ) ;
            
    if (FAILED(hr))
    {
        TRACE("%s\n", (LPCSTR)AfxGetFullScodeString(GetScode(hr))) ;
        AfxThrowOleDispatchException(0,"Could not load File Type Driver's persistent state.") ;
    }
        
    if (m_pdispFileTypeDriver)
        m_pdispFileTypeDriver->Release() ;

    m_pdispFileTypeDriver = lpdisp ;                

    m_strFileTypeDriver = lpszNewValue ;
    SetModifiedFlag();
}

LPDISPATCH CWinPrintControl::GetFileTypeDriver() 
{
    ASSERT_POINTER( m_pdispFileTypeDriver, IDispatch ) ;

    // Extra AddRef because we are returning an IDispatch pointer
    //    
    m_pdispFileTypeDriver->AddRef() ;
    return m_pdispFileTypeDriver ;
}

//void CWinPrintControl::SetFileTypeDriver(LPDISPATCH newValue) 
//{
//    UNUSED newValue ;
//
//    AfxThrowOleDispatchException( 0, "FileTypeDriver is a read only property" ) ;
//}

void CWinPrintControl::OnUseDefaultPrinterChanged() 
{
    // If changed to TRUE then CPage::Recalc will update m_strPrinter etc...
    // If changed to FALSE then just leave as is...
    //
    SetModifiedFlag();
    m_Page.Recalc() ;
}


void CWinPrintControl::OnUsePrinterSettingsChanged() 
{
    // If changed to TRUE then CPages::Recalc will handle
    // else ignore...
    //
    SetModifiedFlag();
    m_Page.Recalc() ;
}

void CWinPrintControl::OnPrinterChanged() 
{
    // Under Win16 two properties define where output goes:  Printer device
    // and printer output (port).  Under Win32 just the printer name (device) is
    // required.
    //
    // Thus, under Win32 PrinterOutput is readonly and is set automatically here
    // when the printer is changed, but under Win16 is is necessary
    // to let the user set it.  
    //
    // This causes a problem in Win16 because the current setting for output may
    // not match one of the device entries in WIN.INI, thus we must set output to
    // the *first* output whenever printer changes.
    //
    
    #ifdef WIN32
    // Use GetPrinter to get driver and output
    //
    HANDLE  h ;
    if (OpenPrinter( m_strPrinter, &h, NULL))
    {
        PRINTER_INFO_2* pPI ;
        DWORD           dw = 0 ;
        
        // Get size of PRINTER_INFO_2
        GetPrinter( h, 2, NULL, 0, &dw ) ;
        
        if (dw)
        {
            pPI = (PRINTER_INFO_2*)new BYTE[dw] ;
            if (GetPrinter( h, 2, pPI, dw, &dw ))
            {
                m_strPrinterPort = pPI->pPortName ;
                m_strPrinterDriver = pPI->pDriverName ;
            }
            else
            {
                TRACE( "GetPrinter Failed.\n") ;
            }
            delete [](BYTE*)pPI ;
        }
        ClosePrinter( h ) ;
    }
    else
    {
        TRACE( "OPenPrinter Failed.\n") ;
    }
    #else
    // For Win16 use WIN.INI
    //
    char sz[1024] ;
    if (::GetProfileString("devices", m_strPrinter, "LPT1:", sz, 1024))
    {
        // sz is of the form "HP LaserJet III = HPPCL5MS,LPT1:,LPT2:
        // We want the "LPT1:"
        //
        LPSTR port = _fstrrchr( sz, ',' ) ;
        if (port) 
        {
            *(port) = '\0' ;
            port++ ;
            m_strPrinterDriver = sz ;
        
            LPSTR end = _fstrrchr( port, ',' ) ;
            if (end)
                *end = '\0' ;
            m_strPrinterOutput = port ;
        }
    }
    #endif
    m_fDefaultPrinter = FALSE ;

    SetModifiedFlag();
    m_Page.Recalc() ;
}

void CWinPrintControl::OnPrinterDriverChanged() 
{
    // TODO: Add notification handler code
    
    // REVIEW:  PrinterDriver should be read only!!!

//    SetModifiedFlag();
//    m_Page.Recalc() ;
}

void CWinPrintControl::OnPrinterOutputChanged() 
{
    // TODO: Add notification handler code

    // REVIEW:  PrinterOutput should be read only!!!
    
//    SetModifiedFlag();
//    m_Page.Recalc() ;
}

LPDISPATCH CWinPrintControl::GetHeader() 
{
    return m_Header.GetIDispatch(TRUE);
}

void CWinPrintControl::SetHeader(LPDISPATCH newValue) 
{   
    UNUSED newValue;
    AfxThrowOleDispatchException( 0, "Header is a read only property" ) ;
}

LPDISPATCH CWinPrintControl::GetFooter() 
{
    return m_Footer.GetIDispatch(TRUE);
}

void CWinPrintControl::SetFooter(LPDISPATCH newValue) 
{
    UNUSED newValue;
    AfxThrowOleDispatchException( 0, "Footer is a read only property" ) ;
}

void CWinPrintControl::OnOrientationChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;
}

void CWinPrintControl::OnPaperSizeChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;
}

void CWinPrintControl::OnPaperLengthChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;
}

void CWinPrintControl::OnPaperWidthChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnScaleChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnCopiesChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnDefaultSourceChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnPrintQualityChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnColorChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnDuplexChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnTopMarginChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnBottomMarginChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnLeftMarginChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnRightMarginChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnBorderChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnShadeChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnRowsChanged() 
{
    // TODO: Add notification handler code
    
    m_sRows = (short)max(1, m_sRows) ;
    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnColumnsChanged() 
{
    // TODO: Add notification handler code

    m_sColumns = (short)max(1, m_sColumns) ;
    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnHorzSeparationChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}

void CWinPrintControl::OnVertSeparationChanged() 
{
    // TODO: Add notification handler code

    SetModifiedFlag();
    m_Page.Recalc() ;

}
