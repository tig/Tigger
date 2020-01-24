// IPERFILE.H
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// July 15, 1993
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  July 15, 1993   cek     First implementation.
//
// Implementation Notes:
//
//  Stolen directly out of KraigB's Chapter 6 code.
//
#include "precomp.h"
#include "xrtobj32.h"
#include "clsid.h"
#include "iperfile.h"

// CImpIPersistFile:CImpIPersistFile
// CImpIPersistFile::~CImpIPersistFile
//
// Constructor Parameters:
//  pObj            LPVOID pointing to the object we live in.
//  punkOuter       LPUNKNOWN of the controlling unknown.
//
CImpIPersistFile::CImpIPersistFile(LPVOID pObj, LPUNKNOWN punkOuter)
{
    m_cRef=0 ;
    m_pObj=pObj ;
    m_punkOuter=punkOuter ;
    return ;
}

CImpIPersistFile::~CImpIPersistFile(void)
{
    return ;
}

// CImpIPersistFile::QueryInterface
// CImpIPersistFile::AddRef
// CImpIPersistFile::Release
//
// Purpose:
//  Standard set of IUnknown members for this interface
//
STDMETHODIMP CImpIPersistFile::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    return m_punkOuter->QueryInterface(riid, ppv) ;
}
STDMETHODIMP_(ULONG) CImpIPersistFile::AddRef(void)
{
    ++m_cRef ;
    return m_punkOuter->AddRef() ;
}
STDMETHODIMP_(ULONG) CImpIPersistFile::Release(void)
{
    --m_cRef ;
    return m_punkOuter->Release() ;
}

// CImpIPersistFile::GetClassID
//
// Purpose:
//  Returns the CLSID of the file represented by this interface.
//
// Parameters:
//  pClsID          LPCLSID in which to store our CLSID.
//
// Return Value:
//  HRESULT         Standard
//
STDMETHODIMP CImpIPersistFile::GetClassID(LPCLSID pClsID)
{
    *pClsID = CLSID_StocksRUsStockQuotes ;    
    return NOERROR ;
}

// CImpIPersistFile::IsDirty
//
// Purpose:
//  Tells the caller if we have made changes to this object since
//  it was loaded or initialized new.
//
// Parameters:
//  None
//
// Return Value:
//  HRESULT         Contains S_OK if we ARE dirty, S_FALSE if NOT dirty,
//                  that is, "Yes I AM dirty, or NO, I'm clean."
//
STDMETHODIMP CImpIPersistFile::IsDirty(void)
{
    return ResultFromScode(S_FALSE) ;
}

// CImpIPersistFile::Load
//
// Purpose:
//  Asks the server to load the document for the given filename.
//
// Parameters:
//  pszFile         LPCTSTR of the filename to load.
//  grfMode         DWORD flags to use when opening the file.
//
// Return Value:
//  HRESULT         Standard
//
STDMETHODIMP CImpIPersistFile::Load(LPCTSTR pszFile, DWORD grfMode)
{
    #ifdef _DEBUG
    OutputDebugString( TEXT("IPersistFile::Load: ") ) ;
    OutputDebugString( pszFile ) ;
    OutputDebugString( TEXT("\r\n") ) ;

    #endif
    return NOERROR ;
}

// CImpIPersistFile::Save
//
// Purpose:
//  Instructs the server to write the current file into a new filename,
//  possibly then using that filename as the current one.
//
// Parameters:
//  pszFile         LPCTSTR of the file into which we save.  If NULL,
//                  this means save the current file.
//  fRemember       BOOL indicating if we're to use this filename as
//                  the current file now (Save As instead of Save Copy As).
//
// Return Value:
//  HRESULT         Standard
//
STDMETHODIMP CImpIPersistFile::Save(LPCTSTR pszFile, BOOL fRemember)
{
    return NOERROR ;
}

// CImpIPersistFile::SaveCompleted
//
// Purpose:
//  Informs us that the operation that called Save is now finished and
//  we can access the file again.
//
// Parameters:
//  pszFile         LPCTSTR of the file in which we can start writing again.
//
// Return Value:
//  HRESULT         Standard
//
STDMETHODIMP CImpIPersistFile::SaveCompleted(LPCTSTR pszFile)
{
    return NOERROR ;
}

// CImpIPersistFile::GetCurFile
//
// Purpose:
//  Returns the current filename.
//
// Parameters:
//  ppszFile        LPTSTR FAR// into which we store a pointer to the
//                  filename that should be allocated with the shared
//                  IMalloc.
//
// Return Value:
//  HRESULT         Standard
//
STDMETHODIMP CImpIPersistFile::GetCurFile(LPTSTR FAR *ppszFile)
{
    return ResultFromScode(E_NOTIMPL) ;
}
