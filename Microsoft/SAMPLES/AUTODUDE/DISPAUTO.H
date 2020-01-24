
#ifndef _DISPAUTO_H_
#define _DISPAUTO_H_

#include "resource.h"       // main symbols

#include <ole2.h>                
#include <dispatch.h>
#include "clsid.h"

#ifndef CLASS
# ifdef __TURBOC__
#  define CLASS class huge
# else
#  define CLASS class FAR
# endif
#endif

#pragma warning(disable:4355)

#define DIM(X) (sizeof(X)/sizeof(X[0]))


CLASS CFileMgr : public IUnknown
{
public:
    static CFileMgr FAR* Create();

    STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppv);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

    // Introduced "AutoDude" interface
    //
    // This nested class implementes core file mgmt functionality
    // (such as it is) *and* is the interface that will be exposed via
    // IDispatch for external programmability.
    //
    CLASS CFileMgmt
    {
    public:
                
        STDMETHOD_(void, put_Text)(BSTR sz);
        STDMETHOD_(BSTR, get_Text)(void);
        STDMETHOD_(void, Clear)(void);                    
        STDMETHOD_(void, About)(void);
        STDMETHOD_(void, Display)(void);
        STDMETHOD_(void, Quit)(void);
        
        CFileMgmt(CFileMgr FAR* pFileMgr)
            { m_pFileMgr  = pFileMgr ; }
        
    private:
        CFileMgr FAR* m_pFileMgr ;
    };  
    
    friend CFileMgmt;
    CFileMgmt m_FileMgmt;
                             
    CFileMgr() : m_FileMgmt(this)
    {
      m_refs = 1;
      m_punkStdDisp = NULL;
    }

private:
    ULONG m_refs;
    IUnknown FAR* m_punkStdDisp;
};


// the following enum defines method indices used by the
// default IDispatch implementation - DispInvoke().
//
// Note: these must match the order of the preceeding declarations
//
enum IMETH_CFileMgmt
{                
    IMETH_PUTTEXT = 0,
    IMETH_GETTEXT,
    IMETH_CLEAR,
    IMETH_ABOUT,
    IMETH_DISPLAY,
    IMETH_QUIT,

    // Define the "property" indices. these are defined to be
    // the first index in a set/get property method pair. These
    // definitions are used to build the METHODDATA that drives
    // our implementation of IDispatch. see cdisp.cpp.
    //           
    IMETH_TEXT    = IMETH_PUTTEXT,
    
};

// the following enum defines the IDs used by IDispatch
//
// Note: these values do *not* depend on order of declaration,
// but are sensitive to the kind of the method - ie, if a get/set
// method pair implements a property, then they need to share
// an ID.
//
enum IDMEMBER_CFileMgmt
{   
    IDMEMBER_TEXT = DISPID_VALUE,	// the default property
    IDMEMBER_CLEAR,
    IDMEMBER_ABOUT,
    IDMEMBER_DISPLAY,
    IDMEMBER_QUIT
};


// the CFileMgr Class Factory
//
CLASS CFileMgrCF : public IClassFactory
{
public:
    static IClassFactory FAR* Create();

    STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppv);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

    STDMETHOD(CreateInstance)(
      IUnknown FAR* punkOuter, REFIID riid, void FAR* FAR* ppv);
    STDMETHOD(LockServer)(BOOL fLock);

    CFileMgrCF() { m_refs = 1; }

private:
    ULONG m_refs;
};
 
#endif //_DISPAUTO_H_
