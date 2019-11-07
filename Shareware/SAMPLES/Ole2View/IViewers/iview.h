/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 2.00.0102 */
/* at Mon Sep 25 16:02:46 1995
 */
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __iview_h__
#define __iview_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IInterfaceViewer_FWD_DEFINED__
#define __IInterfaceViewer_FWD_DEFINED__
typedef interface IInterfaceViewer IInterfaceViewer;
#endif 	/* __IInterfaceViewer_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IInterfaceViewer_INTERFACE_DEFINED__
#define __IInterfaceViewer_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IInterfaceViewer
 * at Mon Sep 25 16:02:46 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [object][uuid] */ 



EXTERN_C const IID IID_IInterfaceViewer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IInterfaceViewer : public IUnknown
    {
    public:
        virtual HRESULT __stdcall View( 
            /* [in] */ HWND hwndParent,
            /* [in] */ REFIID riid,
            /* [in] */ IUnknown __RPC_FAR *punk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInterfaceViewerVtbl
    {
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IInterfaceViewer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IInterfaceViewer __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IInterfaceViewer __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *View )( 
            IInterfaceViewer __RPC_FAR * This,
            /* [in] */ HWND hwndParent,
            /* [in] */ REFIID riid,
            /* [in] */ IUnknown __RPC_FAR *punk);
        
    } IInterfaceViewerVtbl;

    interface IInterfaceViewer
    {
        CONST_VTBL struct IInterfaceViewerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInterfaceViewer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInterfaceViewer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInterfaceViewer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInterfaceViewer_View(This,hwndParent,riid,punk)	\
    (This)->lpVtbl -> View(This,hwndParent,riid,punk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall IInterfaceViewer_View_Proxy( 
    IInterfaceViewer __RPC_FAR * This,
    /* [in] */ HWND hwndParent,
    /* [in] */ REFIID riid,
    /* [in] */ IUnknown __RPC_FAR *punk);


void __RPC_STUB IInterfaceViewer_View_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInterfaceViewer_INTERFACE_DEFINED__ */


extern RPC_IF_HANDLE __MIDL__intf_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL__intf_0006_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */


void __RPC_USER HWND_to_xmit( HWND __RPC_FAR *, long  __RPC_FAR * __RPC_FAR * );
void __RPC_USER HWND_from_xmit( long  __RPC_FAR *, HWND __RPC_FAR * );
void __RPC_USER HWND_free_inst( HWND __RPC_FAR * );
void __RPC_USER HWND_free_xmit( long  __RPC_FAR * );

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
