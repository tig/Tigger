/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.00.08 */
/* at Wed Nov 22 13:35:26 1995
 */
/* Compiler settings for comcat.idl:
    Os, W1, Zp8, env=Win32, ms_ext, app_config, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __comcat_h__
#define __comcat_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IEnumGUID_FWD_DEFINED__
#define __IEnumGUID_FWD_DEFINED__
typedef interface IEnumGUID IEnumGUID;
#endif 	/* __IEnumGUID_FWD_DEFINED__ */


#ifndef __IEnumCATEGORYINFO_FWD_DEFINED__
#define __IEnumCATEGORYINFO_FWD_DEFINED__
typedef interface IEnumCATEGORYINFO IEnumCATEGORYINFO;
#endif 	/* __IEnumCATEGORYINFO_FWD_DEFINED__ */


#ifndef __ICatRegister_FWD_DEFINED__
#define __ICatRegister_FWD_DEFINED__
typedef interface ICatRegister ICatRegister;
#endif 	/* __ICatRegister_FWD_DEFINED__ */


#ifndef __ICatInformation_FWD_DEFINED__
#define __ICatInformation_FWD_DEFINED__
typedef interface ICatInformation ICatInformation;
#endif 	/* __ICatInformation_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL__intf_0000
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [local] */ 


			/* size is 0 */



extern RPC_IF_HANDLE __MIDL__intf_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL__intf_0000_v0_0_s_ifspec;

#ifndef __IEnumGUID_INTERFACE_DEFINED__
#define __IEnumGUID_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumGUID
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [unique][uuid][object] */ 


			/* size is 4 */
typedef /* [unique] */ IEnumGUID __RPC_FAR *LPENUMGUID;

			/* size is 4 */
typedef /* [unique] */ IEnumGUID __RPC_FAR *PENUMGUID;


EXTERN_C const IID IID_IEnumGUID;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IEnumGUID : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT __stdcall Next( 
            /* [in] */ ULONG celt,
            /* [out] */ LPGUID __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT __stdcall Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT __stdcall Reset( void) = 0;
        
        virtual HRESULT __stdcall Clone( 
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumGUIDVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IEnumGUID __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IEnumGUID __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IEnumGUID __RPC_FAR * This);
        
        /* [local] */ HRESULT ( __stdcall __RPC_FAR *Next )( 
            IEnumGUID __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ LPGUID __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( __stdcall __RPC_FAR *Skip )( 
            IEnumGUID __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( __stdcall __RPC_FAR *Reset )( 
            IEnumGUID __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *Clone )( 
            IEnumGUID __RPC_FAR * This,
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumGUIDVtbl;

    interface IEnumGUID
    {
        CONST_VTBL struct IEnumGUIDVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumGUID_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumGUID_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumGUID_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumGUID_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumGUID_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumGUID_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumGUID_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT __stdcall IEnumGUID_RemoteNext_Proxy( 
    IEnumGUID __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPGUID __RPC_FAR *rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumGUID_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumGUID_Skip_Proxy( 
    IEnumGUID __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumGUID_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumGUID_Reset_Proxy( 
    IEnumGUID __RPC_FAR * This);


void __RPC_STUB IEnumGUID_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumGUID_Clone_Proxy( 
    IEnumGUID __RPC_FAR * This,
    /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumGUID_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumGUID_INTERFACE_DEFINED__ */


/****************************************
 * Generated header for interface: __MIDL__intf_0006
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [local] */ 


#define IID_IEnumCLSID		IID_IEnumGUID
#define IEnumCLSID			IEnumGUID
			/* size is 16 */
typedef /* [public] */ GUID CATID;

			/* size is 4 */
typedef /* [public] */ REFGUID REFCATID;

#define CATID_NULL			GUID_NULL
#define IsEqualCATID(rcatid1, rcatid2)	IsEqualGUID(rcatid1, rcatid2)
#define IID_IEnumCATID		IID_IEnumGUID
#define IEnumCATID			IEnumGUID


extern RPC_IF_HANDLE __MIDL__intf_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL__intf_0006_v0_0_s_ifspec;

#ifndef __IEnumCATEGORYINFO_INTERFACE_DEFINED__
#define __IEnumCATEGORYINFO_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumCATEGORYINFO
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [unique][uuid][object] */ 


			/* size is 4 */
typedef /* [unique] */ IEnumCATEGORYINFO __RPC_FAR *LPENUNCATEGORYINFO;

			/* size is 4 */
typedef /* [unique] */ IEnumCATEGORYINFO __RPC_FAR *PENUNCATEGORYINFO;

			/* size is 24 */
typedef struct  tagCATEGORYINFO
    {
    CATID catid;
    LCID lcid;
    OLECHAR __RPC_FAR *szDescription;
    }	CATEGORYINFO;


EXTERN_C const IID IID_IEnumCATEGORYINFO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IEnumCATEGORYINFO : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT __stdcall Next( 
            /* [in] */ ULONG celt,
            /* [out] */ PENUNCATEGORYINFO __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT __stdcall Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT __stdcall Reset( void) = 0;
        
        virtual HRESULT __stdcall Clone( 
            /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCATEGORYINFOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IEnumCATEGORYINFO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IEnumCATEGORYINFO __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IEnumCATEGORYINFO __RPC_FAR * This);
        
        /* [local] */ HRESULT ( __stdcall __RPC_FAR *Next )( 
            IEnumCATEGORYINFO __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ PENUNCATEGORYINFO __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( __stdcall __RPC_FAR *Skip )( 
            IEnumCATEGORYINFO __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( __stdcall __RPC_FAR *Reset )( 
            IEnumCATEGORYINFO __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *Clone )( 
            IEnumCATEGORYINFO __RPC_FAR * This,
            /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumCATEGORYINFOVtbl;

    interface IEnumCATEGORYINFO
    {
        CONST_VTBL struct IEnumCATEGORYINFOVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCATEGORYINFO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCATEGORYINFO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCATEGORYINFO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCATEGORYINFO_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumCATEGORYINFO_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumCATEGORYINFO_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCATEGORYINFO_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT __stdcall IEnumCATEGORYINFO_RemoteNext_Proxy( 
    IEnumCATEGORYINFO __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ PENUNCATEGORYINFO __RPC_FAR *rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumCATEGORYINFO_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumCATEGORYINFO_Skip_Proxy( 
    IEnumCATEGORYINFO __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumCATEGORYINFO_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumCATEGORYINFO_Reset_Proxy( 
    IEnumCATEGORYINFO __RPC_FAR * This);


void __RPC_STUB IEnumCATEGORYINFO_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumCATEGORYINFO_Clone_Proxy( 
    IEnumCATEGORYINFO __RPC_FAR * This,
    /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumCATEGORYINFO_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCATEGORYINFO_INTERFACE_DEFINED__ */


#ifndef __ICatRegister_INTERFACE_DEFINED__
#define __ICatRegister_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICatRegister
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_ICatRegister;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface ICatRegister : public IUnknown
    {
    public:
        virtual HRESULT __stdcall RegisterCategories( 
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATEGORYINFO __RPC_FAR rgCategoryInfo[  ]) = 0;
        
        virtual HRESULT __stdcall UnRegisterCategories( 
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]) = 0;
        
        virtual HRESULT __stdcall RegisterClassCategories( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]) = 0;
        
        virtual HRESULT __stdcall UnRegisterClassCategories( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICatRegisterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            ICatRegister __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            ICatRegister __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            ICatRegister __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *RegisterCategories )( 
            ICatRegister __RPC_FAR * This,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATEGORYINFO __RPC_FAR rgCategoryInfo[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *UnRegisterCategories )( 
            ICatRegister __RPC_FAR * This,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *RegisterClassCategories )( 
            ICatRegister __RPC_FAR * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *UnRegisterClassCategories )( 
            ICatRegister __RPC_FAR * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);
        
        END_INTERFACE
    } ICatRegisterVtbl;

    interface ICatRegister
    {
        CONST_VTBL struct ICatRegisterVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICatRegister_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICatRegister_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICatRegister_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICatRegister_RegisterCategories(This,cCategories,rgCategoryInfo)	\
    (This)->lpVtbl -> RegisterCategories(This,cCategories,rgCategoryInfo)

#define ICatRegister_UnRegisterCategories(This,cCategories,rgcatid)	\
    (This)->lpVtbl -> UnRegisterCategories(This,cCategories,rgcatid)

#define ICatRegister_RegisterClassCategories(This,rclsid,cCategories,rgcatid)	\
    (This)->lpVtbl -> RegisterClassCategories(This,rclsid,cCategories,rgcatid)

#define ICatRegister_UnRegisterClassCategories(This,rclsid,cCategories,rgcatid)	\
    (This)->lpVtbl -> UnRegisterClassCategories(This,rclsid,cCategories,rgcatid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall ICatRegister_RegisterCategories_Proxy( 
    ICatRegister __RPC_FAR * This,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATEGORYINFO __RPC_FAR rgCategoryInfo[  ]);


void __RPC_STUB ICatRegister_RegisterCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatRegister_UnRegisterCategories_Proxy( 
    ICatRegister __RPC_FAR * This,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);


void __RPC_STUB ICatRegister_UnRegisterCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatRegister_RegisterClassCategories_Proxy( 
    ICatRegister __RPC_FAR * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);


void __RPC_STUB ICatRegister_RegisterClassCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatRegister_UnRegisterClassCategories_Proxy( 
    ICatRegister __RPC_FAR * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);


void __RPC_STUB ICatRegister_UnRegisterClassCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICatRegister_INTERFACE_DEFINED__ */


#ifndef __ICatInformation_INTERFACE_DEFINED__
#define __ICatInformation_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICatInformation
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [unique][uuid][object] */ 


    // Category Information
    // Class/Category mapping

EXTERN_C const IID IID_ICatInformation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface ICatInformation : public IUnknown
    {
    public:
        virtual HRESULT __stdcall EnumCategories( 
            /* [in] */ LCID lcid,
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenumCategoryInfo) = 0;
        
        virtual HRESULT __stdcall GetCategoryDesc( 
            /* [in] */ REFCATID rcatid,
            /* [in] */ LCID lcid,
            /* [out] */ OLECHAR __RPC_FAR *ppszDesc) = 0;
        
        virtual HRESULT __stdcall EnumClassesOfCategory( 
            /* [in] */ REFCATID rcatid,
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumClsid) = 0;
        
        virtual HRESULT __stdcall IsClassOfCategory( 
            /* [in] */ REFCLSID rclsid,
            /* [out] */ REFCATID rcatid) = 0;
        
        virtual HRESULT __stdcall EnumCategoriesOfClass( 
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumCatid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICatInformationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            ICatInformation __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            ICatInformation __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *EnumCategories )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ LCID lcid,
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenumCategoryInfo);
        
        HRESULT ( __stdcall __RPC_FAR *GetCategoryDesc )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ REFCATID rcatid,
            /* [in] */ LCID lcid,
            /* [out] */ OLECHAR __RPC_FAR *ppszDesc);
        
        HRESULT ( __stdcall __RPC_FAR *EnumClassesOfCategory )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ REFCATID rcatid,
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumClsid);
        
        HRESULT ( __stdcall __RPC_FAR *IsClassOfCategory )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ REFCLSID rclsid,
            /* [out] */ REFCATID rcatid);
        
        HRESULT ( __stdcall __RPC_FAR *EnumCategoriesOfClass )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumCatid);
        
        END_INTERFACE
    } ICatInformationVtbl;

    interface ICatInformation
    {
        CONST_VTBL struct ICatInformationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICatInformation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICatInformation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICatInformation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICatInformation_EnumCategories(This,lcid,rclsid,ppenumCategoryInfo)	\
    (This)->lpVtbl -> EnumCategories(This,lcid,rclsid,ppenumCategoryInfo)

#define ICatInformation_GetCategoryDesc(This,rcatid,lcid,ppszDesc)	\
    (This)->lpVtbl -> GetCategoryDesc(This,rcatid,lcid,ppszDesc)

#define ICatInformation_EnumClassesOfCategory(This,rcatid,ppenumClsid)	\
    (This)->lpVtbl -> EnumClassesOfCategory(This,rcatid,ppenumClsid)

#define ICatInformation_IsClassOfCategory(This,rclsid,rcatid)	\
    (This)->lpVtbl -> IsClassOfCategory(This,rclsid,rcatid)

#define ICatInformation_EnumCategoriesOfClass(This,rclsid,ppenumCatid)	\
    (This)->lpVtbl -> EnumCategoriesOfClass(This,rclsid,ppenumCatid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall ICatInformation_EnumCategories_Proxy( 
    ICatInformation __RPC_FAR * This,
    /* [in] */ LCID lcid,
    /* [in] */ REFCLSID rclsid,
    /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenumCategoryInfo);


void __RPC_STUB ICatInformation_EnumCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatInformation_GetCategoryDesc_Proxy( 
    ICatInformation __RPC_FAR * This,
    /* [in] */ REFCATID rcatid,
    /* [in] */ LCID lcid,
    /* [out] */ OLECHAR __RPC_FAR *ppszDesc);


void __RPC_STUB ICatInformation_GetCategoryDesc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatInformation_EnumClassesOfCategory_Proxy( 
    ICatInformation __RPC_FAR * This,
    /* [in] */ REFCATID rcatid,
    /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumClsid);


void __RPC_STUB ICatInformation_EnumClassesOfCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatInformation_IsClassOfCategory_Proxy( 
    ICatInformation __RPC_FAR * This,
    /* [in] */ REFCLSID rclsid,
    /* [out] */ REFCATID rcatid);


void __RPC_STUB ICatInformation_IsClassOfCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatInformation_EnumCategoriesOfClass_Proxy( 
    ICatInformation __RPC_FAR * This,
    /* [in] */ REFCLSID rclsid,
    /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumCatid);


void __RPC_STUB ICatInformation_EnumCategoriesOfClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICatInformation_INTERFACE_DEFINED__ */



#ifndef __ComponentCategoryLib_LIBRARY_DEFINED__
#define __ComponentCategoryLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: ComponentCategoryLib
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [helpstring][version][uuid] */ 


			/* size is 0 */

			/* size is 0 */

			/* size is 0 */

			/* size is 0 */


EXTERN_C const IID LIBID_ComponentCategoryLib;

#ifndef __IEnumGUID_INTERFACE_DEFINED__
#define __IEnumGUID_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumGUID
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [unique][uuid][object] */ 


			/* size is 4 */
typedef /* [unique] */ IEnumGUID __RPC_FAR *LPENUMGUID;

			/* size is 4 */
typedef /* [unique] */ IEnumGUID __RPC_FAR *PENUMGUID;


EXTERN_C const IID IID_IEnumGUID;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IEnumGUID : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT __stdcall Next( 
            /* [in] */ ULONG celt,
            /* [out] */ LPGUID __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT __stdcall Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT __stdcall Reset( void) = 0;
        
        virtual HRESULT __stdcall Clone( 
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumGUIDVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IEnumGUID __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IEnumGUID __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IEnumGUID __RPC_FAR * This);
        
        /* [local] */ HRESULT ( __stdcall __RPC_FAR *Next )( 
            IEnumGUID __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ LPGUID __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( __stdcall __RPC_FAR *Skip )( 
            IEnumGUID __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( __stdcall __RPC_FAR *Reset )( 
            IEnumGUID __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *Clone )( 
            IEnumGUID __RPC_FAR * This,
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumGUIDVtbl;

    interface IEnumGUID
    {
        CONST_VTBL struct IEnumGUIDVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumGUID_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumGUID_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumGUID_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumGUID_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumGUID_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumGUID_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumGUID_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT __stdcall IEnumGUID_RemoteNext_Proxy( 
    IEnumGUID __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPGUID __RPC_FAR *rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumGUID_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumGUID_Skip_Proxy( 
    IEnumGUID __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumGUID_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumGUID_Reset_Proxy( 
    IEnumGUID __RPC_FAR * This);


void __RPC_STUB IEnumGUID_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumGUID_Clone_Proxy( 
    IEnumGUID __RPC_FAR * This,
    /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumGUID_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumGUID_INTERFACE_DEFINED__ */


#ifndef __IEnumCATEGORYINFO_INTERFACE_DEFINED__
#define __IEnumCATEGORYINFO_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumCATEGORYINFO
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [unique][uuid][object] */ 


			/* size is 4 */
typedef /* [unique] */ IEnumCATEGORYINFO __RPC_FAR *LPENUNCATEGORYINFO;

			/* size is 4 */
typedef /* [unique] */ IEnumCATEGORYINFO __RPC_FAR *PENUNCATEGORYINFO;

			/* size is 24 */
typedef struct  tagCATEGORYINFO
    {
    CATID catid;
    LCID lcid;
    OLECHAR __RPC_FAR *szDescription;
    }	CATEGORYINFO;


EXTERN_C const IID IID_IEnumCATEGORYINFO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IEnumCATEGORYINFO : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT __stdcall Next( 
            /* [in] */ ULONG celt,
            /* [out] */ PENUNCATEGORYINFO __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT __stdcall Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT __stdcall Reset( void) = 0;
        
        virtual HRESULT __stdcall Clone( 
            /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCATEGORYINFOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IEnumCATEGORYINFO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IEnumCATEGORYINFO __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IEnumCATEGORYINFO __RPC_FAR * This);
        
        /* [local] */ HRESULT ( __stdcall __RPC_FAR *Next )( 
            IEnumCATEGORYINFO __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ PENUNCATEGORYINFO __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( __stdcall __RPC_FAR *Skip )( 
            IEnumCATEGORYINFO __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( __stdcall __RPC_FAR *Reset )( 
            IEnumCATEGORYINFO __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *Clone )( 
            IEnumCATEGORYINFO __RPC_FAR * This,
            /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumCATEGORYINFOVtbl;

    interface IEnumCATEGORYINFO
    {
        CONST_VTBL struct IEnumCATEGORYINFOVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCATEGORYINFO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCATEGORYINFO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCATEGORYINFO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCATEGORYINFO_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumCATEGORYINFO_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumCATEGORYINFO_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCATEGORYINFO_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT __stdcall IEnumCATEGORYINFO_RemoteNext_Proxy( 
    IEnumCATEGORYINFO __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ PENUNCATEGORYINFO __RPC_FAR *rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumCATEGORYINFO_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumCATEGORYINFO_Skip_Proxy( 
    IEnumCATEGORYINFO __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumCATEGORYINFO_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumCATEGORYINFO_Reset_Proxy( 
    IEnumCATEGORYINFO __RPC_FAR * This);


void __RPC_STUB IEnumCATEGORYINFO_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumCATEGORYINFO_Clone_Proxy( 
    IEnumCATEGORYINFO __RPC_FAR * This,
    /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumCATEGORYINFO_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCATEGORYINFO_INTERFACE_DEFINED__ */


#ifndef __ICatRegister_INTERFACE_DEFINED__
#define __ICatRegister_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICatRegister
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_ICatRegister;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface ICatRegister : public IUnknown
    {
    public:
        virtual HRESULT __stdcall RegisterCategories( 
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATEGORYINFO __RPC_FAR rgCategoryInfo[  ]) = 0;
        
        virtual HRESULT __stdcall UnRegisterCategories( 
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]) = 0;
        
        virtual HRESULT __stdcall RegisterClassCategories( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]) = 0;
        
        virtual HRESULT __stdcall UnRegisterClassCategories( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICatRegisterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            ICatRegister __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            ICatRegister __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            ICatRegister __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *RegisterCategories )( 
            ICatRegister __RPC_FAR * This,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATEGORYINFO __RPC_FAR rgCategoryInfo[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *UnRegisterCategories )( 
            ICatRegister __RPC_FAR * This,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *RegisterClassCategories )( 
            ICatRegister __RPC_FAR * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *UnRegisterClassCategories )( 
            ICatRegister __RPC_FAR * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);
        
        END_INTERFACE
    } ICatRegisterVtbl;

    interface ICatRegister
    {
        CONST_VTBL struct ICatRegisterVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICatRegister_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICatRegister_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICatRegister_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICatRegister_RegisterCategories(This,cCategories,rgCategoryInfo)	\
    (This)->lpVtbl -> RegisterCategories(This,cCategories,rgCategoryInfo)

#define ICatRegister_UnRegisterCategories(This,cCategories,rgcatid)	\
    (This)->lpVtbl -> UnRegisterCategories(This,cCategories,rgcatid)

#define ICatRegister_RegisterClassCategories(This,rclsid,cCategories,rgcatid)	\
    (This)->lpVtbl -> RegisterClassCategories(This,rclsid,cCategories,rgcatid)

#define ICatRegister_UnRegisterClassCategories(This,rclsid,cCategories,rgcatid)	\
    (This)->lpVtbl -> UnRegisterClassCategories(This,rclsid,cCategories,rgcatid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall ICatRegister_RegisterCategories_Proxy( 
    ICatRegister __RPC_FAR * This,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATEGORYINFO __RPC_FAR rgCategoryInfo[  ]);


void __RPC_STUB ICatRegister_RegisterCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatRegister_UnRegisterCategories_Proxy( 
    ICatRegister __RPC_FAR * This,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);


void __RPC_STUB ICatRegister_UnRegisterCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatRegister_RegisterClassCategories_Proxy( 
    ICatRegister __RPC_FAR * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);


void __RPC_STUB ICatRegister_RegisterClassCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatRegister_UnRegisterClassCategories_Proxy( 
    ICatRegister __RPC_FAR * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID __RPC_FAR rgcatid[  ]);


void __RPC_STUB ICatRegister_UnRegisterClassCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICatRegister_INTERFACE_DEFINED__ */


#ifndef __ICatInformation_INTERFACE_DEFINED__
#define __ICatInformation_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICatInformation
 * at Wed Nov 22 13:35:26 1995
 * using MIDL 3.00.08
 ****************************************/
/* [unique][uuid][object] */ 


    // Category Information
    // Class/Category mapping

EXTERN_C const IID IID_ICatInformation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface ICatInformation : public IUnknown
    {
    public:
        virtual HRESULT __stdcall EnumCategories( 
            /* [in] */ LCID lcid,
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenumCategoryInfo) = 0;
        
        virtual HRESULT __stdcall GetCategoryDesc( 
            /* [in] */ REFCATID rcatid,
            /* [in] */ LCID lcid,
            /* [out] */ OLECHAR __RPC_FAR *ppszDesc) = 0;
        
        virtual HRESULT __stdcall EnumClassesOfCategory( 
            /* [in] */ REFCATID rcatid,
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumClsid) = 0;
        
        virtual HRESULT __stdcall IsClassOfCategory( 
            /* [in] */ REFCLSID rclsid,
            /* [out] */ REFCATID rcatid) = 0;
        
        virtual HRESULT __stdcall EnumCategoriesOfClass( 
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumCatid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICatInformationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            ICatInformation __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            ICatInformation __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *EnumCategories )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ LCID lcid,
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenumCategoryInfo);
        
        HRESULT ( __stdcall __RPC_FAR *GetCategoryDesc )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ REFCATID rcatid,
            /* [in] */ LCID lcid,
            /* [out] */ OLECHAR __RPC_FAR *ppszDesc);
        
        HRESULT ( __stdcall __RPC_FAR *EnumClassesOfCategory )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ REFCATID rcatid,
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumClsid);
        
        HRESULT ( __stdcall __RPC_FAR *IsClassOfCategory )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ REFCLSID rclsid,
            /* [out] */ REFCATID rcatid);
        
        HRESULT ( __stdcall __RPC_FAR *EnumCategoriesOfClass )( 
            ICatInformation __RPC_FAR * This,
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumCatid);
        
        END_INTERFACE
    } ICatInformationVtbl;

    interface ICatInformation
    {
        CONST_VTBL struct ICatInformationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICatInformation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICatInformation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICatInformation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICatInformation_EnumCategories(This,lcid,rclsid,ppenumCategoryInfo)	\
    (This)->lpVtbl -> EnumCategories(This,lcid,rclsid,ppenumCategoryInfo)

#define ICatInformation_GetCategoryDesc(This,rcatid,lcid,ppszDesc)	\
    (This)->lpVtbl -> GetCategoryDesc(This,rcatid,lcid,ppszDesc)

#define ICatInformation_EnumClassesOfCategory(This,rcatid,ppenumClsid)	\
    (This)->lpVtbl -> EnumClassesOfCategory(This,rcatid,ppenumClsid)

#define ICatInformation_IsClassOfCategory(This,rclsid,rcatid)	\
    (This)->lpVtbl -> IsClassOfCategory(This,rclsid,rcatid)

#define ICatInformation_EnumCategoriesOfClass(This,rclsid,ppenumCatid)	\
    (This)->lpVtbl -> EnumCategoriesOfClass(This,rclsid,ppenumCatid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall ICatInformation_EnumCategories_Proxy( 
    ICatInformation __RPC_FAR * This,
    /* [in] */ LCID lcid,
    /* [in] */ REFCLSID rclsid,
    /* [out] */ IEnumCATEGORYINFO __RPC_FAR *__RPC_FAR *ppenumCategoryInfo);


void __RPC_STUB ICatInformation_EnumCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatInformation_GetCategoryDesc_Proxy( 
    ICatInformation __RPC_FAR * This,
    /* [in] */ REFCATID rcatid,
    /* [in] */ LCID lcid,
    /* [out] */ OLECHAR __RPC_FAR *ppszDesc);


void __RPC_STUB ICatInformation_GetCategoryDesc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatInformation_EnumClassesOfCategory_Proxy( 
    ICatInformation __RPC_FAR * This,
    /* [in] */ REFCATID rcatid,
    /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumClsid);


void __RPC_STUB ICatInformation_EnumClassesOfCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatInformation_IsClassOfCategory_Proxy( 
    ICatInformation __RPC_FAR * This,
    /* [in] */ REFCLSID rclsid,
    /* [out] */ REFCATID rcatid);


void __RPC_STUB ICatInformation_IsClassOfCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICatInformation_EnumCategoriesOfClass_Proxy( 
    ICatInformation __RPC_FAR * This,
    /* [in] */ REFCLSID rclsid,
    /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumCatid);


void __RPC_STUB ICatInformation_EnumCategoriesOfClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICatInformation_INTERFACE_DEFINED__ */

#endif /* __ComponentCategoryLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* [local] */ HRESULT __stdcall IEnumGUID_Next_Proxy( 
    IEnumGUID __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [out] */ LPGUID __RPC_FAR *rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


/* [call_as] */ HRESULT __stdcall IEnumGUID_Next_Stub( 
    IEnumGUID __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPGUID __RPC_FAR *rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);

/* [local] */ HRESULT __stdcall IEnumCATEGORYINFO_Next_Proxy( 
    IEnumCATEGORYINFO __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [out] */ PENUNCATEGORYINFO __RPC_FAR *rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


/* [call_as] */ HRESULT __stdcall IEnumCATEGORYINFO_Next_Stub( 
    IEnumCATEGORYINFO __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ PENUNCATEGORYINFO __RPC_FAR *rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
