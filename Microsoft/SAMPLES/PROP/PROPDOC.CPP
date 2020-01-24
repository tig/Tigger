// propdoc.cpp : implementation of the CPropDoc class
//

#include "stdafx.h"
#include <ole2.h>
#include <storage.h>
#include <variant.h>

#include "clsid.h"

#include "prop.h"
#include "propdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropDoc

IMPLEMENT_DYNCREATE(CPropDoc, CDocument)

BEGIN_MESSAGE_MAP(CPropDoc, CDocument)
    //{{AFX_MSG_MAP(CPropDoc)
    ON_COMMAND(ID_FILE_SUMMARYINFORMATION, OnFileSumInfo)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

static CPropDoc*  g_pDoc ;

/////////////////////////////////////////////////////////////////////////////
// CPropDoc construction/destruction

CPropDoc::CPropDoc()
{
    // TODO: add one-time construction code here
    g_pDoc = this ;
}

CPropDoc::~CPropDoc()
{
}

BOOL CPropDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
        
    // put info in m_SummaryInfo
    m_SummaryInfo.RemoveAll() ;

    m_SummaryInfo.SetClassID( CLSID_PropSetDoc ) ;
    CPropertySection* pSect = m_SummaryInfo.AddSection( FormatID_SummaryInformation ) ;
    if (pSect)
    {                                                  
        ULONG   ul ;
    
        char    szBuf[256] ;
        GetProfileString( "MS Word User", "Name", "", szBuf,  256 ) ;
        pSect->Set( PID_AUTHOR, szBuf, VT_LPSTR ) ;

        pSect->Set( PID_LASTAUTHOR, szBuf, VT_LPSTR ) ;
        pSect->Set( PID_REVNUMBER, "0", VT_LPSTR ) ;
        
        // Set PID_CREATE_DTM
        
        // Set PID_EDITTIME to 0
        
        // Set PID_LASTSAVE_DTM to 0                              

        // Set PID_LASTPRINTED_DTM to 0                              
                              
        pSect->Set( PID_APPNAME, (LPSTR)AfxGetAppName(), VT_LPSTR ) ;
        ul = 0 ;        
        pSect->Set( PID_SECURITY, (LPVOID)&ul, VT_I4 ) ;
    }
        
    return TRUE;
}  

BOOL CPropDoc::OnOpenDocument(const char* pszPathName)
{
    HRESULT     hr ;
    LPSTORAGE   pIStorage ;
    
    /*
     * If this is not a Compound File, open the file using STGM_CONVERT
     * in TRANSACTED mode to effectively see old files as a storage with
     * one stream called "CONTENTS" (which is conveniently the name we use
     * in the new files).  We must use STGM_TRANSACTED here or else
     * the old file will be immediately converted on disk:  we only want
     * a converted image in memory from which to read.  In addition,
     * note that we need STGM_READWRITE as well since conversion is
     * inherently a write operation.
     */

    pIStorage = NULL ;

    if (NOERROR != StgIsStorageFile( pszPathName ))
    {
        hr = StgCreateDocfile( pszPathName, STGM_TRANSACTED | STGM_READWRITE | 
                              STGM_CONVERT | STGM_SHARE_EXCLUSIVE, 0, &pIStorage );
        
        if (FAILED(hr))
        {
            // we were denied write access
            MessageBox(NULL, "File is not an IStorage file.  Could not convert to open.", NULL,
                         MB_ICONINFORMATION | MB_OK);
            return FALSE;
        }
    }
    else
    {
        hr = StgOpenStorage( pszPathName, NULL, STGM_DIRECT | STGM_READ | 
                             STGM_SHARE_EXCLUSIVE, NULL, 0, &pIStorage);

        if (FAILED(hr))
        {
            MessageBox(NULL, "Could not open file.", NULL,
                         MB_ICONINFORMATION | MB_OK);
            return FALSE;
        }
    }

    if (NULL != pIStorage)
    {
        CLSID       clsID ;
        LPSTREAM    pIStream ;
        
        // Did we write this file???
        ReadClassStg( pIStorage, &clsID ) ;
            
        if (!IsEqualCLSID( clsID, CLSID_PropSetDoc ))
        {
            MessageBox(NULL, "File is not a Property Set Document.  Could not open file.", NULL,
                         MB_ICONINFORMATION | MB_OK);
            pIStorage->Release();
            return FALSE;
        }
            
        BeginWaitCursor();
        
        // Read from storage
        hr = pIStorage->OpenStream( SZSUMMARYINFOSTREAM,
                                    NULL,
                                    STGM_DIRECT | STGM_READ |
                                    STGM_SHARE_EXCLUSIVE,
                                    0L, 
                                    &pIStream ) ;
                             
        if (!m_SummaryInfo.ReadFromStream( pIStream ))
        {
            EndWaitCursor();
            MessageBox(NULL, "Could not read file.  ReadFromStream failed.", NULL,
                             MB_ICONINFORMATION | MB_OK);
            pIStream->Release() ;                                    
            pIStorage->Release();
            return FALSE;
        }
        
        // Set PID_EDITTIME to 0
        

                                    
        pIStream->Release() ;                                    
        pIStorage->Release();

        SetPathName( pszPathName ) ;
        SetModifiedFlag( FALSE ) ;     // start off with unmodified
        EndWaitCursor();
    }

    return TRUE ;

}


BOOL CPropDoc::OnSaveDocument(const char* pszPathName)
{
    HRESULT     hr;
    LPSTORAGE   pIStorage ;
    LPSTREAM    pIStream ;

    hr = StgCreateDocfile(pszPathName, STGM_DIRECT | STGM_READWRITE
            | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, &pIStorage);

    if (FAILED(hr))
    {
        MessageBox(NULL, "Could not save file.  StgCreateDocFile failed.", NULL,
                         MB_ICONINFORMATION | MB_OK);
        return FALSE;
    }

    BeginWaitCursor();
    
    // Mark this as one of our class
    WriteClassStg( pIStorage, CLSID_PropSetDoc ) ;

    // Write user-readable class information
    WriteFmtUserTypeStg( pIStorage, 0, "Property Set Document" );

    hr = pIStorage->CreateStream( SZSUMMARYINFOSTREAM, 
                                  STGM_DIRECT | STGM_CREATE |
                                  STGM_WRITE | STGM_SHARE_EXCLUSIVE, 
                                  0, 0, &pIStream ) ;

    if (FAILED(hr))
    {
        MessageBox(NULL, "Could not save file.   ::CreateStream failed.", NULL,
                         MB_ICONINFORMATION | MB_OK);
        pIStorage->Release();
        EndWaitCursor();
        return FALSE;
    }    
    
    char    szBuf[256] ;
    LPSTR lp = (LPSTR)m_SummaryInfo.Get( FormatID_SummaryInformation, PID_REVNUMBER ) ; 
    lstrcpy( szBuf, lp ) ;
    DWORD dw = atol( szBuf ) ;
    wsprintf( szBuf, "%lu", dw + 1 ) ;
    m_SummaryInfo.Set( FormatID_SummaryInformation, PID_REVNUMBER, szBuf, VT_LPSTR ) ;
    
    GetProfileString( "MS Word User", "Name", "", szBuf,  256 ) ;
    m_SummaryInfo.Set( FormatID_SummaryInformation, PID_LASTAUTHOR, szBuf, VT_LPSTR ) ;

    OnFileSumInfo() ;
    
    // Update PID_EDITTIME
    
    // Update PID_LASTSAVE_DTM
    
    if (!m_SummaryInfo.WriteToStream( pIStream ))
    {
        MessageBox(NULL, "Could not save file.  ::WriteToStream failed.", NULL,
                         MB_ICONINFORMATION | MB_OK);
        pIStream->Release() ;                         
        pIStorage->Release();
        EndWaitCursor();
        return FALSE;
    }    

    pIStream->Release() ;                         
    pIStorage->Release();

    EndWaitCursor();
    SetModifiedFlag(FALSE);     // back to unmodified

    return TRUE ;
}

CPropertySet* CPropDoc::GetSummaryInfo( void )
{
    return &m_SummaryInfo ;
}

/////////////////////////////////////////////////////////////////////////////
// CPropDoc diagnostics

#ifdef _DEBUG
void CPropDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CPropDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPropDoc commands
/////////////////////////////////////////////////////////////////////////////
// CSumInfoDlg dialog

CSumInfoDlg::CSumInfoDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSumInfoDlg::IDD, pParent)
{   
    //{{AFX_DATA_INIT(CSumInfoDlg)
    m_Author = "";
    m_Comments = "";
    m_Keywords = "";
    m_LastAuthor = "";
    m_RevNumber = "";
    m_Subject = "";
    m_Template = "";
    m_Title = "" ;
    //}}AFX_DATA_INIT
}

void CSumInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSumInfoDlg)
    DDX_Control(pDX, IDC_DIRECTORY, m_Directory);
    DDX_Control(pDX, IDC_FILENAME, m_Filename);
    DDX_Control(pDX, IDC_DETAILS, m_Details);
    DDX_Text(pDX, IDC_COMMENTS, m_Comments);
    DDX_Text(pDX, IDC_KEYWORDS, m_Keywords);
    DDX_Text(pDX, IDC_LASTAUTHOR, m_LastAuthor);
    DDX_Text(pDX, IDC_REVNUMBER, m_RevNumber);
    DDX_Text(pDX, IDC_SUBJECT, m_Subject);
    DDX_Text(pDX, IDC_TEMPLATE, m_Template);
    DDX_Text(pDX, IDC_TITLE, m_Title);
    DDX_Text(pDX, IDC_AUTHOR, m_Author);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSumInfoDlg, CDialog)
    //{{AFX_MSG_MAP(CSumInfoDlg)
    ON_BN_CLICKED(IDC_DETAILS, OnClickedDetails)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSumInfoDlg message handlers
void CSumInfoDlg::OnClickedDetails()
{
    // TODO: Add your control notification handler code here
    
}

void CPropDoc::OnFileSumInfo()
{
    // TODO: Add your command handler code here
    CSumInfoDlg suminfoDlg;

    CPropertySet* pSum = g_pDoc->GetSummaryInfo() ;
    
    LPSTR sz ;
    sz = (LPSTR)pSum->Get( FormatID_SummaryInformation, PID_AUTHOR ) ;
    if (sz) suminfoDlg.m_Author = sz;         
    
    sz = (LPSTR)pSum->Get( FormatID_SummaryInformation, PID_COMMENTS) ;
    if (sz) suminfoDlg.m_Comments = sz;

    sz = (LPSTR)pSum->Get( FormatID_SummaryInformation, PID_KEYWORDS ) ;
    if (sz) suminfoDlg.m_Keywords = sz;

    sz = (LPSTR)pSum->Get( FormatID_SummaryInformation, PID_LASTAUTHOR ) ;
    if (sz) suminfoDlg.m_LastAuthor = sz;

    sz = (LPSTR)pSum->Get( FormatID_SummaryInformation, PID_REVNUMBER ) ;
    if (sz) suminfoDlg.m_RevNumber = sz;

    sz = (LPSTR)pSum->Get( FormatID_SummaryInformation, PID_SUBJECT ) ;
    if (sz) suminfoDlg.m_Subject = sz;

    sz = (LPSTR)pSum->Get( FormatID_SummaryInformation, PID_TEMPLATE ) ;
    if (sz) suminfoDlg.m_Template = sz;

    sz = (LPSTR)pSum->Get( FormatID_SummaryInformation, PID_TITLE ) ;
    if (sz) suminfoDlg.m_Title = sz ;
    
    if ( IDOK == suminfoDlg.DoModal())
    {
        g_pDoc->m_SummaryInfo.Set( FormatID_SummaryInformation, PID_AUTHOR, (LPSTR)suminfoDlg.m_Author.GetBuffer(0), VT_LPSTR ) ;
        g_pDoc->m_SummaryInfo.Set( FormatID_SummaryInformation, PID_COMMENTS, (LPSTR)suminfoDlg.m_Comments.GetBuffer(0), VT_LPSTR);
        g_pDoc->m_SummaryInfo.Set( FormatID_SummaryInformation, PID_KEYWORDS, (LPSTR)suminfoDlg.m_Keywords.GetBuffer(0), VT_LPSTR);
        g_pDoc->m_SummaryInfo.Set( FormatID_SummaryInformation, PID_LASTAUTHOR, (LPSTR)suminfoDlg.m_LastAuthor.GetBuffer(0), VT_LPSTR);
        g_pDoc->m_SummaryInfo.Set( FormatID_SummaryInformation, PID_REVNUMBER, (LPSTR)suminfoDlg.m_RevNumber.GetBuffer(0), VT_LPSTR);
        g_pDoc->m_SummaryInfo.Set( FormatID_SummaryInformation, PID_SUBJECT, (LPSTR)suminfoDlg.m_Subject.GetBuffer(0), VT_LPSTR);
        g_pDoc->m_SummaryInfo.Set( FormatID_SummaryInformation, PID_TEMPLATE, (LPSTR)suminfoDlg.m_Template.GetBuffer(0), VT_LPSTR);
        g_pDoc->m_SummaryInfo.Set( FormatID_SummaryInformation, PID_TITLE, (LPSTR)suminfoDlg.m_Title.GetBuffer(0), VT_LPSTR);
        g_pDoc->SetModifiedFlag(TRUE);
    }
}

BOOL CSumInfoDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    
    // TODO: Add extra initialization here
    m_Filename.SetWindowText( g_pDoc->GetPathName() ) ;
    m_Directory.SetWindowText( g_pDoc->GetPathName() ) ;
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}
