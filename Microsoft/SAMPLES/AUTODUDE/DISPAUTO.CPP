/*** 
*dispauto.cpp
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  This module implements the basic user interface and functionality
*  functionality of the IDispatch file manager. 
*
*  The implementation of IDispatch it via aggregation with an
*  instance of the "standard" IDispatch implementation, which is
*  initialized with a DispTypeInfo constructed from an INTERFACEDATA
*  description.
*
*Implementation Notes:
*
*****************************************************************************/
#include "stdafx.h"
                
#include <ole2.h>
#if !defined(WIN32)
#include <olenls.h>
#endif
#include <dispatch.h>

#include "resource.h" 
#include "autodude.h" 
#include "autoddoc.h"
#include "dispauto.h"


/***
*CFileMgr *CFileMgr::Create(void)
*Purpose:
*  Create an instance of the IDispatch filemgr, build a
*  TypeInfo that describes the exposed functionality and
*  aggregate with an instance of CStdDispatch that has been
*  initialized with this TypeInfo.
*
*Entry:
*  None
*
*Exit:
*  return value = CFileMgr*, NULL if the creation failed.
*
***********************************************************************/
CFileMgr FAR*
CFileMgr::Create()
{
    HRESULT hresult;
    CFileMgr FAR* pfilemgr;
    ITypeInfo FAR* ptinfo;
    IUnknown FAR* punkStdDisp;
extern INTERFACEDATA NEAR g_idataCFileMgr;

    if((pfilemgr = new FAR CFileMgr()) == NULL)
        return NULL;

    // Build a TypeInfo for the functionality on this object that
    // is being exposing for external programmability.
    //
    hresult = CreateDispTypeInfo(
        &g_idataCFileMgr, LOCALE_SYSTEM_DEFAULT, &ptinfo);
      
    if(hresult != NOERROR)
        goto LError0;

    // Create and aggregate with an instance of the default
    // implementation of IDispatch that is initialized with our
    // TypeInfo.
    //
    hresult = CreateStdDispatch(
        pfilemgr,                // controlling unknown
        &pfilemgr->m_FileMgmt,          // vtable* to dispatch on
        ptinfo,
        &punkStdDisp);

    ptinfo->Release();

    if(hresult != NOERROR)
        goto LError0;

    pfilemgr->m_punkStdDisp = punkStdDisp;

    return pfilemgr;

LError0:;
    pfilemgr->Release();

    return NULL;
}


//---------------------------------------------------------------------
//                        IUnknown methods
//---------------------------------------------------------------------


STDMETHODIMP
CFileMgr::QueryInterface(REFIID riid, void FAR* FAR* ppv)
{
    if(riid == IID_IUnknown)
    {
      *ppv = this;
    }
    else
        if(riid == IID_IDispatch)
        {
            return m_punkStdDisp->QueryInterface(riid, ppv);
        }
        else
        {                                         
            TRACE( "CFileMgr::QueryInterface returning E_NOITERFACE!\n" ) ;
            return ResultFromScode(E_NOINTERFACE);
        }

    AddRef();
    return NOERROR;
}


STDMETHODIMP_(ULONG)
CFileMgr::AddRef()
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG)
CFileMgr::Release()
{
    if(--m_refs == 0)
    {
        if(m_punkStdDisp != NULL)
            m_punkStdDisp->Release();
            
        PostQuitMessage(0);
        
        delete this;
        return 0;
    }
    return m_refs;
}


//---------------------------------------------------------------------
//                      File Management features
//---------------------------------------------------------------------




//---------------------------------------------------------------------
//                       User Interface features
//---------------------------------------------------------------------

STDMETHODIMP_(void)
CFileMgr::CFileMgmt::put_Text(BSTR sz)
{
    ((CAutoDudeDoc*)((CFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveDocument())->SetText(sz) ;
}


STDMETHODIMP_(BSTR)
CFileMgr::CFileMgmt::get_Text()
{       
    BSTR    bsz ;
    
    CString sz = ((CAutoDudeDoc*)((CFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveDocument())->GetText() ;
                 
    bsz = SysAllocString( sz.GetBuffer(255) ) ; 
                        
    sz.ReleaseBuffer(-1) ;    
    
    return bsz ;
}

STDMETHODIMP_(void)
CFileMgr::CFileMgmt::Clear()
{
    TRACE( "CFileMgr::CFileMgt::Clear()\n" ) ;  
    ((CAutoDudeDoc*)((CFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveDocument())->SetText("") ;
}

STDMETHODIMP_(void)
CFileMgr::CFileMgmt::About()
{
    TRACE( "CFileMgr::CFileMgt::About()\n" ) ;  
    ((CAutoDudeApp*)AfxGetApp())->OnAppAbout() ;
}

STDMETHODIMP_(void)
CFileMgr::CFileMgmt::Display()
{
    TRACE( "CFileMgr::CFileMgt::Display()\n" ) ;
    AfxGetApp()->m_pMainWnd->MessageBox( "The 'Display' IDispatch interface was called!" ) ;
}

STDMETHODIMP_(void)
CFileMgr::CFileMgmt::Quit()
{                  
    TRACE( "CFileMgr::CFileMgt::Quit()\n" ) ;

    AfxGetApp()->m_pMainWnd->SendMessage( WM_CLOSE ) ;
}


//---------------------------------------------------------------------
//                      The CFileMgr Class Factory
//---------------------------------------------------------------------


IClassFactory FAR*
CFileMgrCF::Create()
{
    return new FAR CFileMgrCF();
}


STDMETHODIMP
CFileMgrCF::QueryInterface(REFIID riid, void FAR* FAR* ppv)
{
    if(riid == IID_IUnknown || riid == IID_IClassFactory){
      AddRef();
      *ppv = this;
      return NOERROR;
    }
    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG)
CFileMgrCF::AddRef()
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG)
CFileMgrCF::Release()
{
    if(--m_refs == 0){
      delete this;
      return 0;
    }
    return m_refs;
}


STDMETHODIMP
CFileMgrCF::CreateInstance(
    IUnknown FAR* punkOuter,
    REFIID riid,
    void FAR* FAR* ppv)
{
    return ((CAutoDudeApp*)AfxGetApp())->m_pFileMgr->QueryInterface(riid, ppv);
}


STDMETHODIMP
CFileMgrCF::LockServer(BOOL fLock)
{
    return NOERROR;
}          

