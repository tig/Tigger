// engppg.cpp : Implementation of the CEnginePropPage property page class.

#include "stdafx.h"
#include "winprt20.h"
#include "engppg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CEnginePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CEnginePropPage, COlePropertyPage)
    //{{AFX_MSG_MAP(CEnginePropPage)
    // NOTE - ClassWizard will add and remove message map entries
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CEnginePropPage, "WinPrint20.EnginePropPage.1",
    0x2a509, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x46)


/////////////////////////////////////////////////////////////////////////////
// CEnginePropPage::CEnginePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CEnginePropPage

void CEnginePropPage::CEnginePropPageFactory::UpdateRegistry(BOOL bRegister)
{
    AfxOleUnregisterClass(m_clsid, NULL);

    if (bRegister)
        AfxOleRegisterPropertyPageClass(
            AfxGetInstanceHandle(),
            m_clsid,
            IDS_WINPRINTENGINE_PPG);
}


/////////////////////////////////////////////////////////////////////////////
// CEnginePropPage::CEnginePropPage - Constructor

CEnginePropPage::CEnginePropPage() :
    COlePropertyPage(AfxGetInstanceHandle(), IDD, IDS_WINPRINTENGINE_PPG_CAPTION)
{
    //{{AFX_DATA_INIT(CEnginePropPage)
    // NOTE: ClassWizard will add member initialization here
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CEnginePropPage::DoDataExchange - Moves data between page and properties

void CEnginePropPage::DoDataExchange(CDataExchange* pDX)
{
    //{{AFX_DATA_MAP(CEnginePropPage)
    // NOTE: ClassWizard will add DDP, DDX, and DDV calls here
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DATA_MAP
    DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CEnginePropPage message handlers
