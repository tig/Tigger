// engine.cpp : Implementation of the CWinPrintEngine OLE control class.
//
#include "stdafx.h"
#include "winprt20.h"
#include "engine.h"
//#include "engppg.h"
    

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CWinPrintEngine, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CWinPrintEngine, COleControl)
    //{{AFX_MSG_MAP(CWinPrintEngine)
    // NOTE - ClassWizard will add and remove message map entries
    //    DO NOT EDIT what you see in these blocks of generated code !
    ON_OLEVERB(IDS_PROPERTIESVERB, OnProperties)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CWinPrintEngine, COleControl)
    //{{AFX_DISPATCH_MAP(CWinPrintEngine)
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CWinPrintEngine, COleControl)
    //{{AFX_EVENT_MAP(CWinPrintEngine)
    // NOTE - ClassWizard will add and remove event map entries
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// To add another property page to your control:
//  1. Increase the count in the BEGIN_PROPPAGEIDS macro
//  2. Add a PROPPAGEID entry containing the class ID of the page.
// HACK:  Changed 1 to 0 and commented out entry below
//BEGIN_PROPPAGEIDS(CWinPrintEngine, 0/*1*/)
//    PROPPAGEID(CEnginePropPage::guid)
//END_PROPPAGEIDS(CWinPrintEngine)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CWinPrintEngine, "WinPrint20.Engine.1",
    0x2a506, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x46)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CWinPrintEngine, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID IID_IWinPrintEngine =
        { 0x0002A507, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0, 0x0, 0x46 } };
//const IID IID_IWinPrintEngineEvents =
//        { 0x0002A508, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0, 0x0, 0x46 } };

/////////////////////////////////////////////////////////////////////////////
// CWinPrintEngine::CWinPrintEngineFactory::UpdateRegistry -
// Adds or removes system registry entries for CWinPrintEngine
static char BASED_CODE sz00[] = "%2\0" "%5";
static char BASED_CODE sz01[] = "%2\\CLSID\0" "%1";
static char BASED_CODE sz05[] = "CLSID\\%1\0" "%5";
static char BASED_CODE sz06[] = "CLSID\\%1\\ProgID\0" "%2";
static char BASED_CODE sz08[] = "CLSID\\%1\\InProcServer\0" "%3";
static char BASED_CODE sz14[] = "CLSID\\%1\\DefaultIcon\0" "%3,1";
    
// registration for OAT_DISPATCH_OBJECT
static LPCSTR BASED_CODE rglpszRegister[] =
{
    sz00, sz05, NULL
};
static LPCSTR BASED_CODE rglpszOverwrite[] =
{
    sz01, sz06, sz08, sz14, NULL
};  

void CWinPrintEngine::CWinPrintEngineFactory::UpdateRegistry(BOOL bRegister)
{
    bRegister = bRegister ;
    
    // HACK:  
    #ifdef BE_A_CONTROL
    AfxOleUnregisterClass(m_clsid, m_lpszProgID);

    if (bRegister)
        AfxOleRegisterControlClass(
            AfxGetInstanceHandle(),
            m_clsid,
            m_lpszProgID,
            IDS_WINPRINTENGINE,
            IDB_WINPRINTENGINE,
            FALSE,                      //  Not insertable
            OLEMISC_INVISIBLEATRUNTIME |
            OLEMISC_INSIDEOUT |
            OLEMISC_CANTLINKINSIDE |
            OLEMISC_RECOMPOSEONRESIZE);
    #else 

    AfxOleRegisterServerClass( m_clsid, m_lpszProgID, "WinPrint", "WinPrint 2.0 Engine",
        OAT_DISPATCH_OBJECT, rglpszRegister, rglpszOverwrite) ;
    #endif            
}


/////////////////////////////////////////////////////////////////////////////
// Licensing strings

static const char BASED_CODE _szLicFileName[] = "WINPRT20.LIC";

static const char BASED_CODE _szLicString[] =
    "Copyright (c) 1994 ";


/////////////////////////////////////////////////////////////////////////////
// CWinPrintEngine::CWinPrintEngineFactory::VerifyUserLicense -
// Checks for existence of a user license

BOOL CWinPrintEngine::CWinPrintEngineFactory::VerifyUserLicense()
{
    return AfxVerifyLicFile(AfxGetInstanceHandle(), _szLicFileName,
        (LPBYTE)_szLicString);
}


/////////////////////////////////////////////////////////////////////////////
// CWinPrintEngine::CWinPrintEngineFactory::GetLicenseKey -
// Returns a runtime licensing key

BOOL CWinPrintEngine::CWinPrintEngineFactory::GetLicenseKey(DWORD dwReserved,
    BSTR FAR* pbstrKey)
{
    dwReserved = dwReserved ;
    *pbstrKey = SysAllocString(_szLicString);
    return (*pbstrKey != NULL);
}   

/////////////////////////////////////////////////////////////////////////////
// CWinPrintEngine::CWinPrintEngine - Constructor

CWinPrintEngine::CWinPrintEngine()
{   
    InitializeIIDs(&IID_IWinPrintEngine, NULL /*&IID_IWinPrintEngineEvents*/);

    // TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CWinPrintEngine::~CWinPrintEngine - Destructor

CWinPrintEngine::~CWinPrintEngine()
{
    // TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CWinPrintEngine::OnDraw - Drawing function


/////////////////////////////////////////////////////////////////////////////
// CWinPrintEngine::DoPropExchange - Persistence support

void CWinPrintEngine::DoPropExchange(CPropExchange* pPX)
{
    COleControl::DoPropExchange(pPX);
    
    // TODO: Call PX_ functions for each persistent custom property.
}


/////////////////////////////////////////////////////////////////////////////
// CWinPrintEngine::OnResetState - Reset control to default state

void CWinPrintEngine::OnResetState()
{
    COleControl::OnResetState();  // Resets defaults found in DoPropExchange
    
    // TODO: Reset any other control state here.
}

