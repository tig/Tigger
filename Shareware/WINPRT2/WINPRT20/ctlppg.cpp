// ctlppg.cpp : Implementation of the CControlPropPage property page class.

#include "stdafx.h"
#include "winprt20.h"
#include "ctlppg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CControlPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CControlPropPage, COlePropertyPage)
    //{{AFX_MSG_MAP(CControlPropPage)
    ON_BN_CLICKED(IDC_HEADERS, OnHeaders)
    ON_BN_CLICKED(IDC_PRINTER, OnPrinter)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CControlPropPage, "WinPrint20.ControlPropPage.1",
    0x2a505, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x46)


/////////////////////////////////////////////////////////////////////////////
// CControlPropPage::CControlPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CControlPropPage

void CControlPropPage::CControlPropPageFactory::UpdateRegistry(BOOL bRegister)
{
    AfxOleUnregisterClass(m_clsid, NULL);

    if (bRegister)
        AfxOleRegisterPropertyPageClass(
            AfxGetInstanceHandle(),
            m_clsid,
            IDS_WINPRINTCONTROL_PPG);
}


/////////////////////////////////////////////////////////////////////////////
// CControlPropPage::CControlPropPage - Constructor

CControlPropPage::CControlPropPage() :
    COlePropertyPage(AfxGetInstanceHandle(), IDD, IDS_WINPRINTCONTROL_PPG_CAPTION)
{
    //{{AFX_DATA_INIT(CControlPropPage)
    m_strConfigName = "";
    m_strFileTypeDriver = "";
    m_strPrinter = "";
    m_strPrinterDriver = "";
    m_strPrinterOutput = "";
    m_fShade = FALSE;
    m_fBorder = FALSE;
    m_flLeft = 0;
    m_flTop = 0;
    m_flRight = 0;
    m_flBottom = 0;
    m_fDefaultPrinter = FALSE;
    //}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CControlPropPage::DoDataExchange - Moves data between page and properties

void CControlPropPage::DoDataExchange(CDataExchange* pDX)
{
    //{{AFX_DATA_MAP(CControlPropPage)
    DDP_Text(pDX, IDC_CONFIGNAME, m_strConfigName, "ConfigName");
    DDX_Text(pDX, IDC_CONFIGNAME, m_strConfigName);
    DDP_Text(pDX, IDC_FILETYPEDRV_TXT, m_strFileTypeDriver, "FileTypeDriverName");
    DDX_Text(pDX, IDC_FILETYPEDRV_TXT, m_strFileTypeDriver);
    DDP_Text(pDX, IDC_PRINTER_TXT, m_strPrinter, "Printer");
    DDX_Text(pDX, IDC_PRINTER_TXT, m_strPrinter);
    DDP_Text(pDX, IDC_PRINTERDEV_TXT, m_strPrinterDriver, "PrinterDriver");
    DDX_Text(pDX, IDC_PRINTERDEV_TXT, m_strPrinterDriver);
    DDP_Text(pDX, IDC_PRINTEROUT_TXT, m_strPrinterOutput, "PrinterOutput");
    DDX_Text(pDX, IDC_PRINTEROUT_TXT, m_strPrinterOutput);
    DDP_Check(pDX, IDC_SHADE, m_fShade, "Shade");
    DDX_Check(pDX, IDC_SHADE, m_fShade);
    DDP_Check(pDX, IDC_BORDER, m_fBorder, "Border");
    DDX_Check(pDX, IDC_BORDER, m_fBorder);
    DDP_Text(pDX, IDC_LEFT, m_flLeft, "LeftMargin");
    DDX_Text(pDX, IDC_LEFT, m_flLeft);
    DDP_Text(pDX, IDC_TOP, m_flTop, "TopMargin");
    DDX_Text(pDX, IDC_TOP, m_flTop);
    DDP_Text(pDX, IDC_RIGHT, m_flRight, "RightMargin");
    DDX_Text(pDX, IDC_RIGHT, m_flRight);
    DDP_Text(pDX, IDC_BOTTOM, m_flBottom, "BottomMargin");
    DDX_Text(pDX, IDC_BOTTOM, m_flBottom);
    DDP_Check(pDX, IDC_DEFAULTPRN, m_fDefaultPrinter, "DefaultPrinter");
    DDX_Check(pDX, IDC_DEFAULTPRN, m_fDefaultPrinter);
    //}}AFX_DATA_MAP
    DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CControlPropPage message handlers


void CControlPropPage::OnHeaders() 
{
    // TODO: Add your control notification handler code here
    
}


void CControlPropPage::OnPrinter() 
{
    CPrintDialog  pd( FALSE ) ;
    pd.GetDefaults() ;
    if (m_fDefaultPrinter == FALSE)
    {
        // Build our own DEVNAMES
        //
        if (pd.m_pd.hDevNames)
            GlobalFree( pd.m_pd.hDevNames ) ;
        pd.m_pd.hDevNames = wpCreateDevNames( m_strPrinter, m_strPrinterDriver, m_strPrinterOutput ) ;
        ASSERT( pd.m_pd.hDevNames ) ;
    }
}
