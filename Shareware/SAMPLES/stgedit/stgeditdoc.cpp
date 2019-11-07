// StgEditDoc.cpp : implementation of the CStgEditDoc class
//

#include "stdafx.h"
#include "StgEdit.h"

#include "StgEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStgEditDoc

IMPLEMENT_DYNCREATE(CStgEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CStgEditDoc, CDocument)
	//{{AFX_MSG_MAP(CStgEditDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStgEditDoc construction/destruction

CStgEditDoc::CStgEditDoc()
{
	m_lpRootStg = NULL ;
	
}

CStgEditDoc::~CStgEditDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CStgEditDoc diagnostics

#ifdef _DEBUG
void CStgEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CStgEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStgEditDoc commands

BOOL CStgEditDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	ASSERT(lpszPathName == NULL || AfxIsValidString(lpszPathName));

	// abort changes to current docfile
	if (lpszPathName != NULL)
	{
		DeleteContents();
		if (m_lpRootStg)
		{
			m_lpRootStg->Release() ;
			m_lpRootStg = NULL ;
		}
	}
	SetModifiedFlag();  // dirty during de-serialize

	BOOL bResult = FALSE;
	try
	{
		if (m_lpRootStg == NULL)
		{
			// open new storage file
			LPSTORAGE lpStorage;
			
#ifdef _UNICODE
			WCHAR* rgwszPathName = lpszPathName ;
#else
			WCHAR  rgwszPathName[_MAX_PATH] ;
			MultiByteToWideChar(CP_OEMCP, 0, 
								lpszPathName, 
								lstrlen(lpszPathName)+1,
								rgwszPathName,
								_MAX_PATH) ;
#endif
								
			
			SCODE sc = StgOpenStorage(rgwszPathName, NULL,
				STGM_READ|STGM_SHARE_DENY_WRITE,
				0, 0, &lpStorage);
			if (sc != S_OK)
				AfxThrowOleException(sc);
			ASSERT(lpStorage != NULL);
			m_lpRootStg = lpStorage;
		}

		// use helper to read document from storage
//		LoadFromStorage();

		SetModifiedFlag(FALSE); // start off with unmodified
		bResult = TRUE;
	}
	catch(CException* e)
	{
		DeleteContents();   // removed failed contents
		if (m_lpRootStg)
		{
			m_lpRootStg->Release();
			m_lpRootStg = NULL ;
		}
		
		// if not file-based load, return exceptions to the caller
		if (lpszPathName == NULL)
		{
			THROW_LAST();
			ASSERT(FALSE);  // not reached
		}

		try
		{
			ReportSaveLoadException(lpszPathName, e,
				FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		}
		catch(CException* e)
		{ e->Delete() ;}

		e->Delete();
	}


	return bResult;
}

void CStgEditDoc::OnCloseDocument() 
{

	// release storage since document has been closed
	if (m_lpRootStg)
	{
		m_lpRootStg->Release();
		m_lpRootStg = NULL ;
	}

	CDocument::OnCloseDocument();
}

BOOL CStgEditDoc::OnNewDocument() 
{
	// release storage since document has been closed
	if (m_lpRootStg)
	{
		m_lpRootStg->Release();
		m_lpRootStg = NULL ;
	}

	return CDocument::OnNewDocument();
}
