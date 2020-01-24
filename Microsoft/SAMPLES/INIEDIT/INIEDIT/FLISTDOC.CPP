// flistdoc.cpp : implementation file
//

#include "stdafx.h"
#include <dos.h>              // dos_findfirst/dos_findnext
#include <sys\types.h>
#include "iniedit.h"
#include "flistdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

UINIfileList* GetDirectoryList( const CString& rszDir, const CString& rszSpec ) ;

/////////////////////////////////////////////////////////////////////////////
// CFListDoc
IMPLEMENT_DYNCREATE(CFListDoc, CDocument)


CFListDoc::CFListDoc()
{
    // Get a list of all the files in the WINDOWs directory
    m_pINIfiles = GetDirectoryList( "", "" ) ;
    
    // Get files from the [Other List] section
}

BOOL CFListDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
        
    return TRUE;
}

CFListDoc::~CFListDoc()
{
    // Make sure the list of files has been freed
}


BEGIN_MESSAGE_MAP(CFListDoc, CDocument)
    //{{AFX_MSG_MAP(CFListDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFListDoc commands
UINIfileList*  CFListDoc::GetFileList()
{
    return m_pINIfiles ;
}

/////////////////////////////////////////////////////////////////////////////
// CFListDoc helpers

UINIfileList* GetDirectoryList( const CString& rszDir, const CString& rszSpec )
{
    CString         szDir ;
    CString         szSpec ;
    struct find_t   FT ;
    UINIfile*       pINIfile ;
    UINIfileList*   pINIfileList ;

    szDir = rszDir ;
    szSpec = rszSpec ;
    
    if (szSpec.GetLength() == 0)
        szSpec.LoadString( IDS_INIFILESPEC )  ;

    // If no dir is passed in we use the WINDOWS dir
    if (szDir.GetLength() == 0)
    {                             
        LPSTR lp = szDir.GetBuffer( _MAX_PATH ) ;
        GetWindowsDirectory( lp, _MAX_PATH ) ;
        szDir.ReleaseBuffer( -1 ) ;
    }

    if (szDir[szDir.GetLength() - 1] != '\\')
        szDir += "\\" ;

    szSpec = szDir + szSpec ;
            
    pINIfileList = new UINIfileList ;
                
    // Get all files in the dir that match
    if (!_dos_findfirst( szSpec, _A_NORMAL, &FT ))
        do
        {                     
            pINIfile = new UINIfile( szDir + FT.name ) ;         
            pINIfileList->AddTail( pINIfile ) ;
        } while (!_dos_findnext( &FT )) ;

   return pINIfileList ;

} /* GetDirectoryList()  */

