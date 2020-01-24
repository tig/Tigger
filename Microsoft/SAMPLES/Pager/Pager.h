/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.00.44 */
/* at Mon Oct 14 22:46:54 1996
 */
/* Compiler settings for Pager.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __Pager_h__
#define __Pager_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IPager_FWD_DEFINED__
#define __IPager_FWD_DEFINED__
typedef interface IPager IPager;
#endif 	/* __IPager_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IPager_INTERFACE_DEFINED__
#define __IPager_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IPager
 * at Mon Oct 14 22:46:54 1996
 * using MIDL 3.00.44
 ****************************************/
/* [oleautomation][dual][unique][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IPager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IPager : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Page( 
            /* [in] */ BSTR PhoneNumber,
            /* [in] */ BSTR PIN,
            /* [in] */ BSTR Msg,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPager __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPager __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IPager __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IPager __RPC_FAR * This,
            /* [in] */ UINT itinfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *pptinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IPager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out][in] */ DISPID __RPC_FAR *rgdispid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IPager __RPC_FAR * This,
            /* [in] */ DISPID dispidMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [unique][in] */ DISPPARAMS __RPC_FAR *pdispparams,
            /* [unique][out][in] */ VARIANT __RPC_FAR *pvarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pexcepinfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Page )( 
            IPager __RPC_FAR * This,
            /* [in] */ BSTR PhoneNumber,
            /* [in] */ BSTR PIN,
            /* [in] */ BSTR Msg,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);
        
        END_INTERFACE
    } IPagerVtbl;

    interface IPager
    {
        CONST_VTBL struct IPagerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPager_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPager_GetTypeInfo(This,itinfo,lcid,pptinfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,itinfo,lcid,pptinfo)

#define IPager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgdispid)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgdispid)

#define IPager_Invoke(This,dispidMember,riid,lcid,wFlags,pdispparams,pvarResult,pexcepinfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispidMember,riid,lcid,wFlags,pdispparams,pvarResult,pexcepinfo,puArgErr)


#define IPager_Page(This,PhoneNumber,PIN,Msg,retval)	\
    (This)->lpVtbl -> Page(This,PhoneNumber,PIN,Msg,retval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPager_Page_Proxy( 
    IPager __RPC_FAR * This,
    /* [in] */ BSTR PhoneNumber,
    /* [in] */ BSTR PIN,
    /* [in] */ BSTR Msg,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);


void __RPC_STUB IPager_Page_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPager_INTERFACE_DEFINED__ */



#ifndef __PagerLib_LIBRARY_DEFINED__
#define __PagerLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: PagerLib
 * at Mon Oct 14 22:46:54 1996
 * using MIDL 3.00.44
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_PagerLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_Pager;

class Pager;
#endif
#endif /* __PagerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
