#include <ole2.h>
#include <dispatch.h>

// Can be compiled as an application or DLL
// #define _APP_ to make it an application (LOCAL_SERVER)
// otherwise it will be compiled as a DLL (INPROC_SERVER)
//
// #define _APP_
             

// List control window ID
#define IDC_COLLECTIONLIST 100

// MAX len of string table entries
#define STR_LEN   50

// String table constants
#define IDS_PROGNAME        1

// Function prototypes
int PASCAL WinMain (HINSTANCE, HINSTANCE, LPSTR, int);
BOOL InitApplication (HINSTANCE);
BOOL InitInstance (HINSTANCE, int);
BOOL InitOle(LPSTR lpCmdLine, LPDWORD pdwRegisterCF);
void UninitOle(DWORD dwRegisterCF);
extern "C" LRESULT CALLBACK MainWndProc (HWND, UINT, WPARAM, LPARAM);    
HRESULT CreateDispatchInterface(LPUNKNOWN  punkController, LPVOID pProgInterface,
                                IUnknown FAR* FAR* ppunkStdDispatch, REFCLSID clsid); 
HRESULT LoadTypeInfo(ITypeInfo FAR* FAR* pptinfo, REFCLSID clsid);                                    


// GUIDs. IID_I* is the interface id. IID_D* is the dispinterface id.
DEFINE_GUID(CLSID_App,       0x2328ED20L, 0xB5E5, 0x101A, 0xB8, 0x26, 0x00, 0xDD, 0x01, 0x10, 0x3D, 0xE1);
DEFINE_GUID(LIBID_App,       0x2328ED21L, 0xB5E5, 0x101A, 0xB8, 0x26, 0x00, 0xDD, 0x01, 0x10, 0x3D, 0xE1);
DEFINE_GUID(IID_IApp,        0x2328ED22L, 0xB5E5, 0x101A, 0xB8, 0x26, 0x00, 0xDD, 0x01, 0x10, 0x3D, 0xE1);
DEFINE_GUID(IID_DApp,        0x2328ED23L, 0xB5E5, 0x101A, 0xB8, 0x26, 0x00, 0xDD, 0x01, 0x10, 0x3D, 0xE1);
DEFINE_GUID(IID_ICollection, 0x2328ED24L, 0xB5E5, 0x101A, 0xB8, 0x26, 0x00, 0xDD, 0x01, 0x10, 0x3D, 0xE1);
DEFINE_GUID(IID_DCollection, 0x2328ED25L, 0xB5E5, 0x101A, 0xB8, 0x26, 0x00, 0xDD, 0x01, 0x10, 0x3D, 0xE1);
DEFINE_GUID(IID_IItem,       0x2328ED26L, 0xB5E5, 0x101A, 0xB8, 0x26, 0x00, 0xDD, 0x01, 0x10, 0x3D, 0xE1);
DEFINE_GUID(IID_DItem,       0x2328ED27L, 0xB5E5, 0x101A, 0xB8, 0x26, 0x00, 0xDD, 0x01, 0x10, 0x3D, 0xE1);

#include "tlb.h"

class FAR CEnumVariant : public IEnumVARIANT
{
public:
    // IUnknown methods 
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    // IEnumVARIANT methods 
    STDMETHOD(Next)(ULONG cElements, VARIANT FAR* pvar, ULONG FAR* pcElementFetched);
    STDMETHOD(Skip)(ULONG cElements);
    STDMETHOD(Reset)();
    STDMETHOD(Clone)(IEnumVARIANT FAR* FAR* ppenum);
    
    static HRESULT Create(SAFEARRAY FAR*, ULONG, CEnumVariant FAR* FAR*); // Creates and intializes Enumerator
    CEnumVariant();
    ~CEnumVariant();
    
private:
    ULONG m_cRef;           // Reference count                                 
    ULONG m_cElements;       // Number of elements in enumerator. 
    long m_lLBound;         // Lower bound of index.
    long m_lCurrent;        // Current index.
    SAFEARRAY FAR* m_psa;   // Safe array that holds elements.
};

    
    
class FAR CCollection : public _ICollection
{
public:
    // IUnknown methods 
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)(); 
    
    // IDispatch methods 
    STDMETHOD(GetTypeInfoCount)(UINT FAR* pctinfo);
    STDMETHOD(GetTypeInfo)(
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo);
    STDMETHOD(GetIDsOfNames)(
      REFIID riid,
      char FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid);
    STDMETHOD(Invoke)(
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr);

    // _ICollection methods 
    STDMETHOD_(void, Add)(LPDISPATCH pdisp);
    STDMETHOD_(long, get_Count)();
    STDMETHOD_(LPDISPATCH, get_Item)(VARIANT vIndex);
    STDMETHOD_(LPUNKNOWN, get__NewEnum)();
    STDMETHOD_(void, Remove)(VARIANT vIndex);
    
    static HRESULT Create(ULONG lMaxSize, long lLBound, CCollection FAR* FAR*, HWND hwnd) ; // Creates and intializes Collection   
    CCollection();
    ~CCollection();
    
private:
    SAFEARRAY FAR *m_psa;        // Safe array that holds collection items.
    ULONG m_cElements;           // Number of items in collection.
    ULONG m_cMax;                // Maximum number of items collection can hold.
    long m_lLBound;              // Lower bound of index of collection.
    LPTYPEINFO m_ptinfo;         // Type information of collection interface.
    HWND m_hwndList;             // List box control to display collection items.
    EXCEPINFO m_excepinfo;       // Information to raise an exception on error.
    BOOL m_bRaiseException;      // Properties and methods use this to signal an exception to be raised.
    ULONG m_cRef;                // Reference count.
};


class FAR CItem : public _IItem
{
public:
    // IUnknown methods 
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    // _IItem methods 
    STDMETHOD_(void, put_Text)(BSTR bstr);
    STDMETHOD_(BSTR, get_Text)();
    
    static HRESULT Create(BSTR bstr, CItem FAR* FAR*); // Creates and intializes Item.
    CItem();
    ~CItem();
    
private:
    BSTR m_bstrText;                // Text of item.
    LPUNKNOWN m_punkStdDispatch;    // Standard IDispatch implementation.
    ULONG m_cRef;                   // Reference count.
};

class FAR CApp : public _IApp
{
public:
    // IUnknown methods 
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);
    
    // _IApp methods
    STDMETHOD_(LPDISPATCH, get_Collection)();
    STDMETHOD_(LPDISPATCH, NewItem)(BSTR bstr);
    
    STDMETHODIMP Init(); // intializes App object.    
    CApp();
    ~CApp();
     
private:
    LPUNKNOWN m_punkStdDispatch;   // Standard IDispatch implementation.
    LPDISPATCH m_pdispCollection;  // Pointer to collection.
    ULONG m_cRef;                  // Reference count.
    HWND  m_hwnd ;
};

class FAR CAppCF : public IClassFactory
{
public:
    // IUnknown methods 
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void); 
    
    // IClassFactory methods
    STDMETHOD(CreateInstance)(IUnknown FAR* punkOuter, REFIID riid, 
                              void FAR* FAR* ppv);
    STDMETHOD(LockServer)(BOOL fLock);    
    
    CAppCF();

    BOOL AppObjectCreated( BOOL fCreated ) ;
    
private:
    ULONG m_cRef;                   // Reference count
};     


extern CAppCF FAR* g_pAppCF ;
     
                                             
