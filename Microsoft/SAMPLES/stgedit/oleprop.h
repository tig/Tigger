/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 2.00.0102 */
/* at Fri Jul 21 20:06:14 1995
 */
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __oleext_h__
#define __oleext_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IPropertyStorage_FWD_DEFINED__
#define __IPropertyStorage_FWD_DEFINED__
typedef interface IPropertyStorage IPropertyStorage;
#endif 	/* __IPropertyStorage_FWD_DEFINED__ */


#ifndef __IPropertySetStorage_FWD_DEFINED__
#define __IPropertySetStorage_FWD_DEFINED__
typedef interface IPropertySetStorage IPropertySetStorage;
#endif 	/* __IPropertySetStorage_FWD_DEFINED__ */


#ifndef __IEnumSTATPROPSTG_FWD_DEFINED__
#define __IEnumSTATPROPSTG_FWD_DEFINED__
typedef interface IEnumSTATPROPSTG IEnumSTATPROPSTG;
#endif 	/* __IEnumSTATPROPSTG_FWD_DEFINED__ */


#ifndef __IEnumSTATPROPSETSTG_FWD_DEFINED__
#define __IEnumSTATPROPSETSTG_FWD_DEFINED__
typedef interface IEnumSTATPROPSETSTG IEnumSTATPROPSETSTG;
#endif 	/* __IEnumSTATPROPSETSTG_FWD_DEFINED__ */


#ifndef __IPropertySetContainer_FWD_DEFINED__
#define __IPropertySetContainer_FWD_DEFINED__
typedef interface IPropertySetContainer IPropertySetContainer;
#endif 	/* __IPropertySetContainer_FWD_DEFINED__ */


#ifndef __INotifyReplica_FWD_DEFINED__
#define __INotifyReplica_FWD_DEFINED__
typedef interface INotifyReplica INotifyReplica;
#endif 	/* __INotifyReplica_FWD_DEFINED__ */


#ifndef __IReconcilableObject_FWD_DEFINED__
#define __IReconcilableObject_FWD_DEFINED__
typedef interface IReconcilableObject IReconcilableObject;
#endif 	/* __IReconcilableObject_FWD_DEFINED__ */


#ifndef __IReconcileInitiator_FWD_DEFINED__
#define __IReconcileInitiator_FWD_DEFINED__
typedef interface IReconcileInitiator IReconcileInitiator;
#endif 	/* __IReconcileInitiator_FWD_DEFINED__ */


#ifndef __IDifferencing_FWD_DEFINED__
#define __IDifferencing_FWD_DEFINED__
typedef interface IDifferencing IDifferencing;
#endif 	/* __IDifferencing_FWD_DEFINED__ */


#ifndef __IAccessControl_FWD_DEFINED__
#define __IAccessControl_FWD_DEFINED__
typedef interface IAccessControl IAccessControl;
#endif 	/* __IAccessControl_FWD_DEFINED__ */


#ifndef __IAuditControl_FWD_DEFINED__
#define __IAuditControl_FWD_DEFINED__
typedef interface IAuditControl IAuditControl;
#endif 	/* __IAuditControl_FWD_DEFINED__ */


#ifndef __IDirectory_FWD_DEFINED__
#define __IDirectory_FWD_DEFINED__
typedef interface IDirectory IDirectory;
#endif 	/* __IDirectory_FWD_DEFINED__ */


#ifndef __IEnumSTATDIR_FWD_DEFINED__
#define __IEnumSTATDIR_FWD_DEFINED__
typedef interface IEnumSTATDIR IEnumSTATDIR;
#endif 	/* __IEnumSTATDIR_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
//#include "accctrl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL__intf_0000
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [local] */ 


# define tagSTGVARIANT tagPROPVARIANT // until STGVARIANTs are renamed
# define STGVARIANT PROPVARIANT // until STGVARIANTs are renamed
			/* size is 0 */

			/* size is 0 */



extern RPC_IF_HANDLE __MIDL__intf_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL__intf_0000_v0_0_s_ifspec;

#ifndef __IPropertyStorage_INTERFACE_DEFINED__
#define __IPropertyStorage_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IPropertyStorage
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [unique][uuid][object] */ 


			/* size is 16 */
typedef GUID FMTID;

#ifdef __cplusplus
#define REFFMTID const FMTID &
#else
#define REFFMTID const FMTID * const
#endif
#if 0 // for MIDL processing only!!
			/* size is 4 */
typedef const GUID __RPC_FAR *REFFMTID;

#endif //if 0: for MIDL processing only!!
			/* size is 4 */
#define	PROPSETFLAG_DEFAULT	( 0 )

			/* size is 4 */
#define	PROPSETFLAG_NONSIMPLE	( 1 )

			/* size is 4 */
#define	PROPSETFLAG_ANSI	( 2 )

#define VT_ILLEGAL  (enum VARENUM) 0xffff
#define VT_TYPEMASK (enum VARENUM) 0x0fff
			/* size is 4 */
typedef /* [unique] */ IPropertyStorage __RPC_FAR *LPPROPERTYSTORAGE;

			/* size is 0 */
typedef struct tagPROPVARIANT PROPVARIANT;

#ifndef _tagCAUI1_DEFINED
#define _tagCAUI1_DEFINED
			/* size is 8 */
typedef struct  tagCAUI1
    {
    ULONG cElems;
    /* [size_is] */ unsigned char __RPC_FAR *pElems;
    }	CAUI1;

#endif
#ifndef _tagCAI2_DEFINED
#define _tagCAI2_DEFINED
			/* size is 8 */
typedef struct  tagCAI2
    {
    ULONG cElems;
    /* [size_is] */ short __RPC_FAR *pElems;
    }	CAI2;

#endif
#ifndef _tagCAUI2_DEFINED
#define _tagCAUI2_DEFINED
			/* size is 8 */
typedef struct  tagCAUI2
    {
    ULONG cElems;
    /* [size_is] */ USHORT __RPC_FAR *pElems;
    }	CAUI2;

#endif
#ifndef _tagCAI4_DEFINED
#define _tagCAI4_DEFINED
			/* size is 8 */
typedef struct  tagCAI4
    {
    ULONG cElems;
    /* [size_is] */ long __RPC_FAR *pElems;
    }	CAI4;

#endif
#ifndef _tagCAUI4_DEFINED
#define _tagCAUI4_DEFINED
			/* size is 8 */
typedef struct  tagCAUI4
    {
    ULONG cElems;
    /* [size_is] */ ULONG __RPC_FAR *pElems;
    }	CAUI4;

#endif
#ifndef _tagCAR4_DEFINED
#define _tagCAR4_DEFINED
			/* size is 8 */
typedef struct  tagCAR4
    {
    ULONG cElems;
    /* [size_is] */ float __RPC_FAR *pElems;
    }	CAR4;

#endif
#ifndef _tagCAR8_DEFINED
#define _tagCAR8_DEFINED
			/* size is 8 */
typedef struct  tagCAR8
    {
    ULONG cElems;
    /* [size_is] */ double __RPC_FAR *pElems;
    }	CAR8;

#endif
#ifndef _tagCACY_DEFINED
#define _tagCACY_DEFINED
			/* size is 8 */
typedef struct  tagCACY
    {
    ULONG cElems;
    /* [size_is] */ CY __RPC_FAR *pElems;
    }	CACY;

#endif
#ifndef _tagCADATE_DEFINED
#define _tagCADATE_DEFINED
			/* size is 8 */
typedef struct  tagCADATE
    {
    ULONG cElems;
    /* [size_is] */ DATE __RPC_FAR *pElems;
    }	CADATE;

#endif
#ifndef _tagCABSTR_DEFINED
#define _tagCABSTR_DEFINED
			/* size is 8 */
typedef struct  tagCABSTR
    {
    ULONG cElems;
    /* [size_is] */ BSTR __RPC_FAR *pElems;
    }	CABSTR;

#endif
#ifndef _tagCABOOL_DEFINED
#define _tagCABOOL_DEFINED
			/* size is 8 */
typedef struct  tagCABOOL
    {
    ULONG cElems;
    /* [size_is] */ VARIANT_BOOL __RPC_FAR *pElems;
    }	CABOOL;

#endif
#ifndef _tagCAERROR_DEFINED
#define _tagCAERROR_DEFINED
			/* size is 8 */
typedef struct  tagCAERROR
    {
    ULONG cElems;
    /* [size_is] */ SCODE __RPC_FAR *pElems;
    }	CAERROR;

#endif
#ifndef _tagCAPROPVARIANT_DEFINED
#define _tagCAPROPVARIANT_DEFINED
			/* size is 8 */
typedef struct  tagCAPROPVARIANT
    {
    ULONG cElems;
    /* [size_is] */ PROPVARIANT __RPC_FAR *pElems;
    }	CAPROPVARIANT;

#endif
#ifndef _tagCAI8_DEFINED
#define _tagCAI8_DEFINED
			/* size is 8 */
typedef struct  tagCAI8
    {
    ULONG cElems;
    /* [size_is] */ LARGE_INTEGER __RPC_FAR *pElems;
    }	CAI8;

#endif
#ifndef _tagCAUI8_DEFINED
#define _tagCAUI8_DEFINED
			/* size is 8 */
typedef struct  tagCAUI8
    {
    ULONG cElems;
    /* [size_is] */ ULARGE_INTEGER __RPC_FAR *pElems;
    }	CAUI8;

#endif
#ifndef _tagCALPSTR_DEFINED
#define _tagCALPSTR_DEFINED
			/* size is 8 */
typedef struct  tagCALPSTR
    {
    ULONG cElems;
    /* [size_is] */ LPSTR __RPC_FAR *pElems;
    }	CALPSTR;

#endif
#ifndef _tagCALPWSTR_DEFINED
#define _tagCALPWSTR_DEFINED
			/* size is 8 */
typedef struct  tagCALPWSTR
    {
    ULONG cElems;
    /* [size_is] */ LPWSTR __RPC_FAR *pElems;
    }	CALPWSTR;

#endif
#ifndef _tagCAFILETIME_DEFINED
#define _tagCAFILETIME_DEFINED
			/* size is 8 */
typedef struct  tagCAFILETIME
    {
    ULONG cElems;
    /* [size_is] */ FILETIME __RPC_FAR *pElems;
    }	CAFILETIME;

#endif
#ifndef _tagCACF_DEFINED
#define _tagCACF_DEFINED
			/* size is 8 */
typedef struct  tagCACF
    {
    ULONG cElems;
    /* [size_is] */ CLIPDATA __RPC_FAR *pElems;
    }	CACF;

#endif
#ifndef _tagCACLSID_DEFINED
#define _tagCACLSID_DEFINED
			/* size is 8 */
typedef struct  tagCACLSID
    {
    ULONG cElems;
    /* [size_is] */ CLSID __RPC_FAR *pElems;
    }	CACLSID;

#endif
#ifndef _tagPROPVARIANT_DEFINED
#define _tagPROPVARIANT_DEFINED
			/* size is 16 */
struct  tagPROPVARIANT
    {
    VARTYPE vt;
    WORD wReserved1;
    WORD wReserved2;
    WORD wReserved3;
    /* [switch_is] */ /* [switch_type] */ union 
        {
        /* [case] */  /* Empty union arm */ 
        /* [case] */ UCHAR bVal;
        /* [case] */ short iVal;
        /* [case] */ USHORT uiVal;
        /* [case] */ VARIANT_BOOL bool;
        /* [case] */ long lVal;
        /* [case] */ ULONG ulVal;
        /* [case] */ float fltVal;
        /* [case] */ SCODE scode;
        /* [case] */ LARGE_INTEGER hVal;
        /* [case] */ ULARGE_INTEGER uhVal;
        /* [case] */ double dblVal;
        /* [case] */ CY cyVal;
        /* [case] */ DATE date;
        /* [case] */ FILETIME filetime;
        /* [case] */ CLSID __RPC_FAR *puuid;
        /* [case] */ BLOB blob;
        /* [case] */ CLIPDATA __RPC_FAR *pclipdata;
        /* [case] */ IStream __RPC_FAR *pStream;
        /* [case] */ IStorage __RPC_FAR *pStorage;
        /* [case] */ BSTR bstrVal;
        /* [case] */ LPSTR pszVal;
        /* [case] */ LPWSTR pwszVal;
        /* [case] */ CAUI1 cab;
        /* [case] */ CAI2 cai;
        /* [case] */ CAUI2 caus;
        /* [case] */ CABOOL cabool;
        /* [case] */ CAI4 cal;
        /* [case] */ CAUI4 caul;
        /* [case] */ CAR4 caflt;
        /* [case] */ CAERROR casc;
        /* [case] */ CAI8 cali;
        /* [case] */ CAUI8 cauli;
        /* [case] */ CAR8 cadbl;
        /* [case] */ CACY cacy;
        /* [case] */ CADATE cadate;
        /* [case] */ CAFILETIME cafiletime;
        /* [case] */ CACLSID cauuid;
        /* [case] */ CACF caclipdata;
        /* [case] */ CABSTR cabstr;
        /* [case] */ CALPSTR calpstr;
        /* [case] */ CALPWSTR calpwstr;
        /* [case] */ CAPROPVARIANT capropvar;
        }	;
    };
#endif
#ifndef _PROPVARIANT_DEFINED
#define _PROPVARIANT_DEFINED
#endif
#ifndef _LPPROPVARIANT_DEFINED
#define _LPPROPVARIANT_DEFINED
			/* size is 4 */
typedef struct tagPROPVARIANT __RPC_FAR *LPPROPVARIANT;

#endif
			/* size is 4 */
#define	PROPID_ILLEGAL	( 0xffffffff )

			/* size is 4 */
#define	PROPID_FIRST_NAME_DEFAULT	( 4095 )

			/* size is 4 */
#define	PROPID_DICTIONARY	( 0 )

			/* size is 4 */
#define	PROPID_CODEPAGE	( 1 )

			/* size is 4 */
#define	PRSPEC_INVALID	( 0xffffffff )

			/* size is 4 */
#define	PRSPEC_LPWSTR	( 0 )

			/* size is 4 */
#define	PRSPEC_PROPID	( 1 )

			/* size is 8 */
typedef struct  tagPROPSPEC
    {
    ULONG ulKind;
    /* [switch_is] */ /* [switch_type] */ union 
        {
        /* [case] */ PROPID propid;
        /* [case] */ LPOLESTR lpwstr;
        /* [default] */  /* Empty union arm */ 
        }	;
    }	PROPSPEC;

			/* size is 24 */
typedef struct  tagFULLPROPSPEC
    {
    GUID guidPropSet;
    PROPSPEC psProperty;
    }	FULLPROPSPEC;

			/* size is 10 */
typedef struct  tagSTATPROPSTG
    {
    LPOLESTR lpwstrName;
    PROPID propid;
    VARTYPE vt;
    }	STATPROPSTG;

			/* size is 60 */
typedef struct  tagSTATPROPSETSTG
    {
    FMTID fmtid;
    CLSID clsid;
    DWORD grfFlags;
    FILETIME mtime;
    FILETIME ctime;
    FILETIME atime;
    }	STATPROPSETSTG;


EXTERN_C const IID IID_IPropertyStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IPropertyStorage : public IUnknown
    {
    public:
        virtual HRESULT __stdcall ReadMultiple( 
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ PROPSPEC __RPC_FAR rgpspec[  ],
            /* [size_is][out] */ PROPVARIANT __RPC_FAR rgpropvar[  ]) = 0;
        
        virtual HRESULT __stdcall WriteMultiple( 
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ PROPSPEC __RPC_FAR rgpspec[  ],
            /* [size_is][in] */ PROPVARIANT __RPC_FAR rgpropvar[  ],
            /* [in] */ PROPID propidNameFirst) = 0;
        
        virtual HRESULT __stdcall DeleteMultiple( 
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ PROPSPEC __RPC_FAR rgpspec[  ]) = 0;
        
        virtual HRESULT __stdcall ReadPropertyNames( 
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ PROPID __RPC_FAR rgpropid[  ],
            /* [size_is][out] */ LPOLESTR __RPC_FAR rglpwstrName[  ]) = 0;
        
        virtual HRESULT __stdcall WritePropertyNames( 
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ PROPID __RPC_FAR rgpropid[  ],
            /* [size_is][in] */ LPOLESTR __RPC_FAR rglpwstrName[  ]) = 0;
        
        virtual HRESULT __stdcall DeletePropertyNames( 
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ PROPID __RPC_FAR rgpropid[  ]) = 0;
        
        virtual HRESULT __stdcall Commit( 
            /* [in] */ DWORD grfCommitFlags) = 0;
        
        virtual HRESULT __stdcall Revert( void) = 0;
        
        virtual HRESULT __stdcall Enum( 
            /* [out] */ IEnumSTATPROPSTG __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
        virtual HRESULT __stdcall SetTimes( 
            /* [in] */ const FILETIME __RPC_FAR *pctime,
            /* [in] */ const FILETIME __RPC_FAR *patime,
            /* [in] */ const FILETIME __RPC_FAR *pmtime) = 0;
        
        virtual HRESULT __stdcall SetClass( 
            /* [in] */ REFCLSID clsid) = 0;
        
        virtual HRESULT __stdcall Stat( 
            /* [out] */ STATPROPSETSTG __RPC_FAR *pstatpsstg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPropertyStorageVtbl
    {
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IPropertyStorage __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IPropertyStorage __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IPropertyStorage __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *ReadMultiple )( 
            IPropertyStorage __RPC_FAR * This,
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ PROPSPEC __RPC_FAR rgpspec[  ],
            /* [size_is][out] */ PROPVARIANT __RPC_FAR rgpropvar[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *WriteMultiple )( 
            IPropertyStorage __RPC_FAR * This,
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ PROPSPEC __RPC_FAR rgpspec[  ],
            /* [size_is][in] */ PROPVARIANT __RPC_FAR rgpropvar[  ],
            /* [in] */ PROPID propidNameFirst);
        
        HRESULT ( __stdcall __RPC_FAR *DeleteMultiple )( 
            IPropertyStorage __RPC_FAR * This,
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ PROPSPEC __RPC_FAR rgpspec[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *ReadPropertyNames )( 
            IPropertyStorage __RPC_FAR * This,
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ PROPID __RPC_FAR rgpropid[  ],
            /* [size_is][out] */ LPOLESTR __RPC_FAR rglpwstrName[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *WritePropertyNames )( 
            IPropertyStorage __RPC_FAR * This,
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ PROPID __RPC_FAR rgpropid[  ],
            /* [size_is][in] */ LPOLESTR __RPC_FAR rglpwstrName[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *DeletePropertyNames )( 
            IPropertyStorage __RPC_FAR * This,
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ PROPID __RPC_FAR rgpropid[  ]);
        
        HRESULT ( __stdcall __RPC_FAR *Commit )( 
            IPropertyStorage __RPC_FAR * This,
            /* [in] */ DWORD grfCommitFlags);
        
        HRESULT ( __stdcall __RPC_FAR *Revert )( 
            IPropertyStorage __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *Enum )( 
            IPropertyStorage __RPC_FAR * This,
            /* [out] */ IEnumSTATPROPSTG __RPC_FAR *__RPC_FAR *ppenum);
        
        HRESULT ( __stdcall __RPC_FAR *SetTimes )( 
            IPropertyStorage __RPC_FAR * This,
            /* [in] */ const FILETIME __RPC_FAR *pctime,
            /* [in] */ const FILETIME __RPC_FAR *patime,
            /* [in] */ const FILETIME __RPC_FAR *pmtime);
        
        HRESULT ( __stdcall __RPC_FAR *SetClass )( 
            IPropertyStorage __RPC_FAR * This,
            /* [in] */ REFCLSID clsid);
        
        HRESULT ( __stdcall __RPC_FAR *Stat )( 
            IPropertyStorage __RPC_FAR * This,
            /* [out] */ STATPROPSETSTG __RPC_FAR *pstatpsstg);
        
    } IPropertyStorageVtbl;

    interface IPropertyStorage
    {
        CONST_VTBL struct IPropertyStorageVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertyStorage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPropertyStorage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPropertyStorage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPropertyStorage_ReadMultiple(This,cpspec,rgpspec,rgpropvar)	\
    (This)->lpVtbl -> ReadMultiple(This,cpspec,rgpspec,rgpropvar)

#define IPropertyStorage_WriteMultiple(This,cpspec,rgpspec,rgpropvar,propidNameFirst)	\
    (This)->lpVtbl -> WriteMultiple(This,cpspec,rgpspec,rgpropvar,propidNameFirst)

#define IPropertyStorage_DeleteMultiple(This,cpspec,rgpspec)	\
    (This)->lpVtbl -> DeleteMultiple(This,cpspec,rgpspec)

#define IPropertyStorage_ReadPropertyNames(This,cpropid,rgpropid,rglpwstrName)	\
    (This)->lpVtbl -> ReadPropertyNames(This,cpropid,rgpropid,rglpwstrName)

#define IPropertyStorage_WritePropertyNames(This,cpropid,rgpropid,rglpwstrName)	\
    (This)->lpVtbl -> WritePropertyNames(This,cpropid,rgpropid,rglpwstrName)

#define IPropertyStorage_DeletePropertyNames(This,cpropid,rgpropid)	\
    (This)->lpVtbl -> DeletePropertyNames(This,cpropid,rgpropid)

#define IPropertyStorage_Commit(This,grfCommitFlags)	\
    (This)->lpVtbl -> Commit(This,grfCommitFlags)

#define IPropertyStorage_Revert(This)	\
    (This)->lpVtbl -> Revert(This)

#define IPropertyStorage_Enum(This,ppenum)	\
    (This)->lpVtbl -> Enum(This,ppenum)

#define IPropertyStorage_SetTimes(This,pctime,patime,pmtime)	\
    (This)->lpVtbl -> SetTimes(This,pctime,patime,pmtime)

#define IPropertyStorage_SetClass(This,clsid)	\
    (This)->lpVtbl -> SetClass(This,clsid)

#define IPropertyStorage_Stat(This,pstatpsstg)	\
    (This)->lpVtbl -> Stat(This,pstatpsstg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall IPropertyStorage_ReadMultiple_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [in] */ ULONG cpspec,
    /* [size_is][in] */ PROPSPEC __RPC_FAR rgpspec[  ],
    /* [size_is][out] */ PROPVARIANT __RPC_FAR rgpropvar[  ]);


void __RPC_STUB IPropertyStorage_ReadMultiple_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_WriteMultiple_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [in] */ ULONG cpspec,
    /* [size_is][in] */ PROPSPEC __RPC_FAR rgpspec[  ],
    /* [size_is][in] */ PROPVARIANT __RPC_FAR rgpropvar[  ],
    /* [in] */ PROPID propidNameFirst);


void __RPC_STUB IPropertyStorage_WriteMultiple_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_DeleteMultiple_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [in] */ ULONG cpspec,
    /* [size_is][in] */ PROPSPEC __RPC_FAR rgpspec[  ]);


void __RPC_STUB IPropertyStorage_DeleteMultiple_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_ReadPropertyNames_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [in] */ ULONG cpropid,
    /* [size_is][in] */ PROPID __RPC_FAR rgpropid[  ],
    /* [size_is][out] */ LPOLESTR __RPC_FAR rglpwstrName[  ]);


void __RPC_STUB IPropertyStorage_ReadPropertyNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_WritePropertyNames_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [in] */ ULONG cpropid,
    /* [size_is][in] */ PROPID __RPC_FAR rgpropid[  ],
    /* [size_is][in] */ LPOLESTR __RPC_FAR rglpwstrName[  ]);


void __RPC_STUB IPropertyStorage_WritePropertyNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_DeletePropertyNames_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [in] */ ULONG cpropid,
    /* [size_is][in] */ PROPID __RPC_FAR rgpropid[  ]);


void __RPC_STUB IPropertyStorage_DeletePropertyNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_Commit_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [in] */ DWORD grfCommitFlags);


void __RPC_STUB IPropertyStorage_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_Revert_Proxy( 
    IPropertyStorage __RPC_FAR * This);


void __RPC_STUB IPropertyStorage_Revert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_Enum_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [out] */ IEnumSTATPROPSTG __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IPropertyStorage_Enum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_SetTimes_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [in] */ const FILETIME __RPC_FAR *pctime,
    /* [in] */ const FILETIME __RPC_FAR *patime,
    /* [in] */ const FILETIME __RPC_FAR *pmtime);


void __RPC_STUB IPropertyStorage_SetTimes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_SetClass_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [in] */ REFCLSID clsid);


void __RPC_STUB IPropertyStorage_SetClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertyStorage_Stat_Proxy( 
    IPropertyStorage __RPC_FAR * This,
    /* [out] */ STATPROPSETSTG __RPC_FAR *pstatpsstg);


void __RPC_STUB IPropertyStorage_Stat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPropertyStorage_INTERFACE_DEFINED__ */


#ifndef __IPropertySetStorage_INTERFACE_DEFINED__
#define __IPropertySetStorage_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IPropertySetStorage
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [unique][uuid][object] */ 


			/* size is 4 */
typedef /* [unique] */ IPropertySetStorage __RPC_FAR *LPPROPERTYSETSTORAGE;


EXTERN_C const IID IID_IPropertySetStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IPropertySetStorage : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Create( 
            /* [in] */ REFFMTID rfmtid,
            /* [unique][in] */ CLSID __RPC_FAR *pclsid,
            /* [in] */ DWORD grfFlags,
            /* [in] */ DWORD grfMode,
            /* [out] */ IPropertyStorage __RPC_FAR *__RPC_FAR *ppprstg) = 0;
        
        virtual HRESULT __stdcall Open( 
            /* [in] */ REFFMTID rfmtid,
            /* [in] */ DWORD grfMode,
            /* [out] */ IPropertyStorage __RPC_FAR *__RPC_FAR *ppprstg) = 0;
        
        virtual HRESULT __stdcall Delete( 
            /* [in] */ REFFMTID rfmtid) = 0;
        
        virtual HRESULT __stdcall Enum( 
            /* [out] */ IEnumSTATPROPSETSTG __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPropertySetStorageVtbl
    {
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IPropertySetStorage __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IPropertySetStorage __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IPropertySetStorage __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *Create )( 
            IPropertySetStorage __RPC_FAR * This,
            /* [in] */ REFFMTID rfmtid,
            /* [unique][in] */ CLSID __RPC_FAR *pclsid,
            /* [in] */ DWORD grfFlags,
            /* [in] */ DWORD grfMode,
            /* [out] */ IPropertyStorage __RPC_FAR *__RPC_FAR *ppprstg);
        
        HRESULT ( __stdcall __RPC_FAR *Open )( 
            IPropertySetStorage __RPC_FAR * This,
            /* [in] */ REFFMTID rfmtid,
            /* [in] */ DWORD grfMode,
            /* [out] */ IPropertyStorage __RPC_FAR *__RPC_FAR *ppprstg);
        
        HRESULT ( __stdcall __RPC_FAR *Delete )( 
            IPropertySetStorage __RPC_FAR * This,
            /* [in] */ REFFMTID rfmtid);
        
        HRESULT ( __stdcall __RPC_FAR *Enum )( 
            IPropertySetStorage __RPC_FAR * This,
            /* [out] */ IEnumSTATPROPSETSTG __RPC_FAR *__RPC_FAR *ppenum);
        
    } IPropertySetStorageVtbl;

    interface IPropertySetStorage
    {
        CONST_VTBL struct IPropertySetStorageVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertySetStorage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPropertySetStorage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPropertySetStorage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPropertySetStorage_Create(This,rfmtid,pclsid,grfFlags,grfMode,ppprstg)	\
    (This)->lpVtbl -> Create(This,rfmtid,pclsid,grfFlags,grfMode,ppprstg)

#define IPropertySetStorage_Open(This,rfmtid,grfMode,ppprstg)	\
    (This)->lpVtbl -> Open(This,rfmtid,grfMode,ppprstg)

#define IPropertySetStorage_Delete(This,rfmtid)	\
    (This)->lpVtbl -> Delete(This,rfmtid)

#define IPropertySetStorage_Enum(This,ppenum)	\
    (This)->lpVtbl -> Enum(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall IPropertySetStorage_Create_Proxy( 
    IPropertySetStorage __RPC_FAR * This,
    /* [in] */ REFFMTID rfmtid,
    /* [unique][in] */ CLSID __RPC_FAR *pclsid,
    /* [in] */ DWORD grfFlags,
    /* [in] */ DWORD grfMode,
    /* [out] */ IPropertyStorage __RPC_FAR *__RPC_FAR *ppprstg);


void __RPC_STUB IPropertySetStorage_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertySetStorage_Open_Proxy( 
    IPropertySetStorage __RPC_FAR * This,
    /* [in] */ REFFMTID rfmtid,
    /* [in] */ DWORD grfMode,
    /* [out] */ IPropertyStorage __RPC_FAR *__RPC_FAR *ppprstg);


void __RPC_STUB IPropertySetStorage_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertySetStorage_Delete_Proxy( 
    IPropertySetStorage __RPC_FAR * This,
    /* [in] */ REFFMTID rfmtid);


void __RPC_STUB IPropertySetStorage_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertySetStorage_Enum_Proxy( 
    IPropertySetStorage __RPC_FAR * This,
    /* [out] */ IEnumSTATPROPSETSTG __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IPropertySetStorage_Enum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPropertySetStorage_INTERFACE_DEFINED__ */


#ifndef __IEnumSTATPROPSTG_INTERFACE_DEFINED__
#define __IEnumSTATPROPSTG_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumSTATPROPSTG
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [unique][uuid][object] */ 


			/* size is 4 */
typedef /* [unique] */ IEnumSTATPROPSTG __RPC_FAR *LPENUMSTATPROPSTG;


EXTERN_C const IID IID_IEnumSTATPROPSTG;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IEnumSTATPROPSTG : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT __stdcall Next( 
            /* [in] */ ULONG celt,
            /* [in] */ STATPROPSTG __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT __stdcall Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT __stdcall Reset( void) = 0;
        
        virtual HRESULT __stdcall Clone( 
            /* [out] */ IEnumSTATPROPSTG __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumSTATPROPSTGVtbl
    {
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IEnumSTATPROPSTG __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IEnumSTATPROPSTG __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IEnumSTATPROPSTG __RPC_FAR * This);
        
        /* [local] */ HRESULT ( __stdcall __RPC_FAR *Next )( 
            IEnumSTATPROPSTG __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [in] */ STATPROPSTG __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( __stdcall __RPC_FAR *Skip )( 
            IEnumSTATPROPSTG __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( __stdcall __RPC_FAR *Reset )( 
            IEnumSTATPROPSTG __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *Clone )( 
            IEnumSTATPROPSTG __RPC_FAR * This,
            /* [out] */ IEnumSTATPROPSTG __RPC_FAR *__RPC_FAR *ppenum);
        
    } IEnumSTATPROPSTGVtbl;

    interface IEnumSTATPROPSTG
    {
        CONST_VTBL struct IEnumSTATPROPSTGVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumSTATPROPSTG_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumSTATPROPSTG_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumSTATPROPSTG_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumSTATPROPSTG_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumSTATPROPSTG_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumSTATPROPSTG_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumSTATPROPSTG_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT __stdcall IEnumSTATPROPSTG_RemoteNext_Proxy( 
    IEnumSTATPROPSTG __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSTG __RPC_FAR *rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumSTATPROPSTG_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumSTATPROPSTG_Skip_Proxy( 
    IEnumSTATPROPSTG __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumSTATPROPSTG_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumSTATPROPSTG_Reset_Proxy( 
    IEnumSTATPROPSTG __RPC_FAR * This);


void __RPC_STUB IEnumSTATPROPSTG_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumSTATPROPSTG_Clone_Proxy( 
    IEnumSTATPROPSTG __RPC_FAR * This,
    /* [out] */ IEnumSTATPROPSTG __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumSTATPROPSTG_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumSTATPROPSTG_INTERFACE_DEFINED__ */


#ifndef __IEnumSTATPROPSETSTG_INTERFACE_DEFINED__
#define __IEnumSTATPROPSETSTG_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumSTATPROPSETSTG
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [unique][uuid][object] */ 


			/* size is 4 */
typedef /* [unique] */ IEnumSTATPROPSETSTG __RPC_FAR *LPENUMSTATPROPSETSTG;


EXTERN_C const IID IID_IEnumSTATPROPSETSTG;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IEnumSTATPROPSETSTG : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT __stdcall Next( 
            /* [in] */ ULONG celt,
            /* [in] */ STATPROPSETSTG __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT __stdcall Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT __stdcall Reset( void) = 0;
        
        virtual HRESULT __stdcall Clone( 
            /* [out] */ IEnumSTATPROPSETSTG __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumSTATPROPSETSTGVtbl
    {
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IEnumSTATPROPSETSTG __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IEnumSTATPROPSETSTG __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IEnumSTATPROPSETSTG __RPC_FAR * This);
        
        /* [local] */ HRESULT ( __stdcall __RPC_FAR *Next )( 
            IEnumSTATPROPSETSTG __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [in] */ STATPROPSETSTG __RPC_FAR *rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( __stdcall __RPC_FAR *Skip )( 
            IEnumSTATPROPSETSTG __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( __stdcall __RPC_FAR *Reset )( 
            IEnumSTATPROPSETSTG __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *Clone )( 
            IEnumSTATPROPSETSTG __RPC_FAR * This,
            /* [out] */ IEnumSTATPROPSETSTG __RPC_FAR *__RPC_FAR *ppenum);
        
    } IEnumSTATPROPSETSTGVtbl;

    interface IEnumSTATPROPSETSTG
    {
        CONST_VTBL struct IEnumSTATPROPSETSTGVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumSTATPROPSETSTG_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumSTATPROPSETSTG_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumSTATPROPSETSTG_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumSTATPROPSETSTG_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumSTATPROPSETSTG_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumSTATPROPSETSTG_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumSTATPROPSETSTG_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT __stdcall IEnumSTATPROPSETSTG_RemoteNext_Proxy( 
    IEnumSTATPROPSETSTG __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSETSTG __RPC_FAR *rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumSTATPROPSETSTG_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumSTATPROPSETSTG_Skip_Proxy( 
    IEnumSTATPROPSETSTG __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumSTATPROPSETSTG_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumSTATPROPSETSTG_Reset_Proxy( 
    IEnumSTATPROPSETSTG __RPC_FAR * This);


void __RPC_STUB IEnumSTATPROPSETSTG_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumSTATPROPSETSTG_Clone_Proxy( 
    IEnumSTATPROPSETSTG __RPC_FAR * This,
    /* [out] */ IEnumSTATPROPSETSTG __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumSTATPROPSETSTG_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumSTATPROPSETSTG_INTERFACE_DEFINED__ */


#ifndef __IPropertySetContainer_INTERFACE_DEFINED__
#define __IPropertySetContainer_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IPropertySetContainer
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IPropertySetContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IPropertySetContainer : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetPropset( 
            /* [in] */ REFGUID rguidName,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObj) = 0;
        
        virtual HRESULT __stdcall AddPropset( 
            /* [in] */ IPersist __RPC_FAR *pPropset) = 0;
        
        virtual HRESULT __stdcall DeletePropset( 
            /* [in] */ REFGUID rguidName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPropertySetContainerVtbl
    {
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IPropertySetContainer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IPropertySetContainer __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IPropertySetContainer __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *GetPropset )( 
            IPropertySetContainer __RPC_FAR * This,
            /* [in] */ REFGUID rguidName,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObj);
        
        HRESULT ( __stdcall __RPC_FAR *AddPropset )( 
            IPropertySetContainer __RPC_FAR * This,
            /* [in] */ IPersist __RPC_FAR *pPropset);
        
        HRESULT ( __stdcall __RPC_FAR *DeletePropset )( 
            IPropertySetContainer __RPC_FAR * This,
            /* [in] */ REFGUID rguidName);
        
    } IPropertySetContainerVtbl;

    interface IPropertySetContainer
    {
        CONST_VTBL struct IPropertySetContainerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertySetContainer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPropertySetContainer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPropertySetContainer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPropertySetContainer_GetPropset(This,rguidName,riid,ppvObj)	\
    (This)->lpVtbl -> GetPropset(This,rguidName,riid,ppvObj)

#define IPropertySetContainer_AddPropset(This,pPropset)	\
    (This)->lpVtbl -> AddPropset(This,pPropset)

#define IPropertySetContainer_DeletePropset(This,rguidName)	\
    (This)->lpVtbl -> DeletePropset(This,rguidName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall IPropertySetContainer_GetPropset_Proxy( 
    IPropertySetContainer __RPC_FAR * This,
    /* [in] */ REFGUID rguidName,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObj);


void __RPC_STUB IPropertySetContainer_GetPropset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertySetContainer_AddPropset_Proxy( 
    IPropertySetContainer __RPC_FAR * This,
    /* [in] */ IPersist __RPC_FAR *pPropset);


void __RPC_STUB IPropertySetContainer_AddPropset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IPropertySetContainer_DeletePropset_Proxy( 
    IPropertySetContainer __RPC_FAR * This,
    /* [in] */ REFGUID rguidName);


void __RPC_STUB IPropertySetContainer_DeletePropset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPropertySetContainer_INTERFACE_DEFINED__ */


#ifndef __INotifyReplica_INTERFACE_DEFINED__
#define __INotifyReplica_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INotifyReplica
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_INotifyReplica;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface INotifyReplica : public IUnknown
    {
    public:
        virtual HRESULT __stdcall YouAreAReplica( 
            /* [in] */ ULONG cOtherReplicas,
            /* [unique][in][size_is][size_is] */ IMoniker __RPC_FAR *__RPC_FAR *rgpOtherReplicas) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INotifyReplicaVtbl
    {
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            INotifyReplica __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            INotifyReplica __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            INotifyReplica __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *YouAreAReplica )( 
            INotifyReplica __RPC_FAR * This,
            /* [in] */ ULONG cOtherReplicas,
            /* [unique][in][size_is][size_is] */ IMoniker __RPC_FAR *__RPC_FAR *rgpOtherReplicas);
        
    } INotifyReplicaVtbl;

    interface INotifyReplica
    {
        CONST_VTBL struct INotifyReplicaVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INotifyReplica_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INotifyReplica_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INotifyReplica_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INotifyReplica_YouAreAReplica(This,cOtherReplicas,rgpOtherReplicas)	\
    (This)->lpVtbl -> YouAreAReplica(This,cOtherReplicas,rgpOtherReplicas)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall INotifyReplica_YouAreAReplica_Proxy( 
    INotifyReplica __RPC_FAR * This,
    /* [in] */ ULONG cOtherReplicas,
    /* [unique][in][size_is][size_is] */ IMoniker __RPC_FAR *__RPC_FAR *rgpOtherReplicas);


void __RPC_STUB INotifyReplica_YouAreAReplica_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INotifyReplica_INTERFACE_DEFINED__ */


/****************************************
 * Generated header for interface: __MIDL__intf_0058
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [local] */ 


			/* size is 0 */



extern RPC_IF_HANDLE __MIDL__intf_0058_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL__intf_0058_v0_0_s_ifspec;

#ifndef __IReconcilableObject_INTERFACE_DEFINED__
#define __IReconcilableObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReconcilableObject
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [unique][uuid][object] */ 


			/* size is 2 */
typedef 
enum _reconcilef
    {	RECONCILEF_MAYBOTHERUSER	= 0x1,
	RECONCILEF_FEEDBACKWINDOWVALID	= 0x2,
	RECONCILEF_NORESIDUESOK	= 0x4,
	RECONCILEF_OMITSELFRESIDUE	= 0x8,
	RECONCILEF_RESUMERECONCILIATION	= 0x10,
	RECONCILEF_YOUMAYDOTHEUPDATES	= 0x20,
	RECONCILEF_ONLYYOUWERECHANGED	= 0x40,
	ALL_RECONCILE_FLAGS	= RECONCILEF_MAYBOTHERUSER | RECONCILEF_FEEDBACKWINDOWVALID | RECONCILEF_NORESIDUESOK | RECONCILEF_OMITSELFRESIDUE | RECONCILEF_RESUMERECONCILIATION | RECONCILEF_YOUMAYDOTHEUPDATES | RECONCILEF_ONLYYOUWERECHANGED
    }	RECONCILEF;


EXTERN_C const IID IID_IReconcilableObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IReconcilableObject : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Reconcile( 
            /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
            /* [in] */ DWORD dwFlags,
            /* [in] */ HWND hwndOwner,
            /* [in] */ HWND hwndProgressFeedback,
            /* [in] */ ULONG cInput,
            /* [size_is][size_is][unique][in] */ LPMONIKER __RPC_FAR *rgpmkOtherInput,
            /* [out] */ LONG __RPC_FAR *plOutIndex,
            /* [unique][in] */ IStorage __RPC_FAR *pstgNewResidues,
            /* [unique][in] */ ULONG __RPC_FAR *pvReserved) = 0;
        
        virtual HRESULT __stdcall GetProgressFeedbackMaxEstimate( 
            /* [out] */ ULONG __RPC_FAR *pulProgressMax) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReconcilableObjectVtbl
    {
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IReconcilableObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IReconcilableObject __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IReconcilableObject __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *Reconcile )( 
            IReconcilableObject __RPC_FAR * This,
            /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
            /* [in] */ DWORD dwFlags,
            /* [in] */ HWND hwndOwner,
            /* [in] */ HWND hwndProgressFeedback,
            /* [in] */ ULONG cInput,
            /* [size_is][size_is][unique][in] */ LPMONIKER __RPC_FAR *rgpmkOtherInput,
            /* [out] */ LONG __RPC_FAR *plOutIndex,
            /* [unique][in] */ IStorage __RPC_FAR *pstgNewResidues,
            /* [unique][in] */ ULONG __RPC_FAR *pvReserved);
        
        HRESULT ( __stdcall __RPC_FAR *GetProgressFeedbackMaxEstimate )( 
            IReconcilableObject __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pulProgressMax);
        
    } IReconcilableObjectVtbl;

    interface IReconcilableObject
    {
        CONST_VTBL struct IReconcilableObjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReconcilableObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReconcilableObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReconcilableObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReconcilableObject_Reconcile(This,pInitiator,dwFlags,hwndOwner,hwndProgressFeedback,cInput,rgpmkOtherInput,plOutIndex,pstgNewResidues,pvReserved)	\
    (This)->lpVtbl -> Reconcile(This,pInitiator,dwFlags,hwndOwner,hwndProgressFeedback,cInput,rgpmkOtherInput,plOutIndex,pstgNewResidues,pvReserved)

#define IReconcilableObject_GetProgressFeedbackMaxEstimate(This,pulProgressMax)	\
    (This)->lpVtbl -> GetProgressFeedbackMaxEstimate(This,pulProgressMax)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall IReconcilableObject_Reconcile_Proxy( 
    IReconcilableObject __RPC_FAR * This,
    /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
    /* [in] */ DWORD dwFlags,
    /* [in] */ HWND hwndOwner,
    /* [in] */ HWND hwndProgressFeedback,
    /* [in] */ ULONG cInput,
    /* [size_is][size_is][unique][in] */ LPMONIKER __RPC_FAR *rgpmkOtherInput,
    /* [out] */ LONG __RPC_FAR *plOutIndex,
    /* [unique][in] */ IStorage __RPC_FAR *pstgNewResidues,
    /* [unique][in] */ ULONG __RPC_FAR *pvReserved);


void __RPC_STUB IReconcilableObject_Reconcile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IReconcilableObject_GetProgressFeedbackMaxEstimate_Proxy( 
    IReconcilableObject __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pulProgressMax);


void __RPC_STUB IReconcilableObject_GetProgressFeedbackMaxEstimate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReconcilableObject_INTERFACE_DEFINED__ */


#ifndef __Versioning_INTERFACE_DEFINED__
#define __Versioning_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: Versioning
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [auto_handle][unique][uuid] */ 



#pragma pack(4)
			/* size is 16 */
typedef GUID VERID;

			/* size is 4 */
typedef struct  tagVERIDARRAY
    {
    DWORD cVerid;
    /* [size_is] */ GUID verid[ 1 ];
    }	VERIDARRAY;

			/* size is 16 */
typedef struct  tagVERBLOCK
    {
    ULONG iveridFirst;
    ULONG iveridMax;
    ULONG cblockPrev;
    /* [size_is] */ ULONG __RPC_FAR *rgiblockPrev;
    }	VERBLOCK;

			/* size is 8 */
typedef struct  tagVERCONNECTIONINFO
    {
    DWORD cBlock;
    /* [size_is] */ VERBLOCK __RPC_FAR *rgblock;
    }	VERCONNECTIONINFO;

			/* size is 12 */
typedef struct  tagVERGRAPH
    {
    VERCONNECTIONINFO blocks;
    VERIDARRAY nodes;
    }	VERGRAPH;


#pragma pack()


extern RPC_IF_HANDLE Versioning_v0_0_c_ifspec;
extern RPC_IF_HANDLE Versioning_v0_0_s_ifspec;
#endif /* __Versioning_INTERFACE_DEFINED__ */

#ifndef __IReconcileInitiator_INTERFACE_DEFINED__
#define __IReconcileInitiator_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReconcileInitiator
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IReconcileInitiator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IReconcileInitiator : public IUnknown
    {
    public:
        virtual HRESULT __stdcall SetAbortCallback( 
            /* [unique][in] */ IUnknown __RPC_FAR *pUnkForAbort) = 0;
        
        virtual HRESULT __stdcall SetProgressFeedback( 
            /* [in] */ ULONG ulProgress,
            /* [in] */ ULONG ulProgressMax) = 0;
        
        virtual HRESULT __stdcall FindVersion( 
            /* [in] */ VERID __RPC_FAR *pverid,
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmk) = 0;
        
        virtual HRESULT __stdcall FindVersionFromGraph( 
            /* [in] */ VERGRAPH __RPC_FAR *pvergraph,
            /* [out] */ VERID __RPC_FAR *pverid,
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReconcileInitiatorVtbl
    {
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IReconcileInitiator __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IReconcileInitiator __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IReconcileInitiator __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *SetAbortCallback )( 
            IReconcileInitiator __RPC_FAR * This,
            /* [unique][in] */ IUnknown __RPC_FAR *pUnkForAbort);
        
        HRESULT ( __stdcall __RPC_FAR *SetProgressFeedback )( 
            IReconcileInitiator __RPC_FAR * This,
            /* [in] */ ULONG ulProgress,
            /* [in] */ ULONG ulProgressMax);
        
        HRESULT ( __stdcall __RPC_FAR *FindVersion )( 
            IReconcileInitiator __RPC_FAR * This,
            /* [in] */ VERID __RPC_FAR *pverid,
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmk);
        
        HRESULT ( __stdcall __RPC_FAR *FindVersionFromGraph )( 
            IReconcileInitiator __RPC_FAR * This,
            /* [in] */ VERGRAPH __RPC_FAR *pvergraph,
            /* [out] */ VERID __RPC_FAR *pverid,
            /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmk);
        
    } IReconcileInitiatorVtbl;

    interface IReconcileInitiator
    {
        CONST_VTBL struct IReconcileInitiatorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReconcileInitiator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReconcileInitiator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReconcileInitiator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReconcileInitiator_SetAbortCallback(This,pUnkForAbort)	\
    (This)->lpVtbl -> SetAbortCallback(This,pUnkForAbort)

#define IReconcileInitiator_SetProgressFeedback(This,ulProgress,ulProgressMax)	\
    (This)->lpVtbl -> SetProgressFeedback(This,ulProgress,ulProgressMax)

#define IReconcileInitiator_FindVersion(This,pverid,ppmk)	\
    (This)->lpVtbl -> FindVersion(This,pverid,ppmk)

#define IReconcileInitiator_FindVersionFromGraph(This,pvergraph,pverid,ppmk)	\
    (This)->lpVtbl -> FindVersionFromGraph(This,pvergraph,pverid,ppmk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall IReconcileInitiator_SetAbortCallback_Proxy( 
    IReconcileInitiator __RPC_FAR * This,
    /* [unique][in] */ IUnknown __RPC_FAR *pUnkForAbort);


void __RPC_STUB IReconcileInitiator_SetAbortCallback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IReconcileInitiator_SetProgressFeedback_Proxy( 
    IReconcileInitiator __RPC_FAR * This,
    /* [in] */ ULONG ulProgress,
    /* [in] */ ULONG ulProgressMax);


void __RPC_STUB IReconcileInitiator_SetProgressFeedback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IReconcileInitiator_FindVersion_Proxy( 
    IReconcileInitiator __RPC_FAR * This,
    /* [in] */ VERID __RPC_FAR *pverid,
    /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmk);


void __RPC_STUB IReconcileInitiator_FindVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IReconcileInitiator_FindVersionFromGraph_Proxy( 
    IReconcileInitiator __RPC_FAR * This,
    /* [in] */ VERGRAPH __RPC_FAR *pvergraph,
    /* [out] */ VERID __RPC_FAR *pverid,
    /* [out] */ IMoniker __RPC_FAR *__RPC_FAR *ppmk);


void __RPC_STUB IReconcileInitiator_FindVersionFromGraph_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReconcileInitiator_INTERFACE_DEFINED__ */


#ifndef __IDifferencing_INTERFACE_DEFINED__
#define __IDifferencing_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IDifferencing
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [unique][uuid][object] */ 


			/* size is 2 */
typedef 
enum __MIDL_IDifferencing_0001
    {	DIFF_TYPE_Ordinary	= 0,
	DIFF_TYPE_Urgent	= DIFF_TYPE_Ordinary + 1
    }	DifferenceType;


EXTERN_C const IID IID_IDifferencing;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IDifferencing : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT __stdcall SubtractMoniker( 
            /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
            /* [in] */ IMoniker __RPC_FAR *pOtherStg,
            /* [in] */ DifferenceType diffType,
            /* [out][in] */ STGMEDIUM __RPC_FAR *pStgMedium,
            /* [in] */ DWORD reserved) = 0;
        
        virtual /* [local] */ HRESULT __stdcall SubtractVerid( 
            /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
            /* [in] */ VERID __RPC_FAR *pVerid,
            /* [in] */ DifferenceType diffType,
            /* [out][in] */ STGMEDIUM __RPC_FAR *pStgMedium,
            /* [in] */ DWORD reserved) = 0;
        
        virtual /* [local] */ HRESULT __stdcall SubtractTimeStamp( 
            /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
            /* [in] */ FILETIME __RPC_FAR *pTimeStamp,
            /* [in] */ DifferenceType diffType,
            /* [out][in] */ STGMEDIUM __RPC_FAR *pStgMedium,
            /* [in] */ DWORD reserved) = 0;
        
        virtual /* [local] */ HRESULT __stdcall Add( 
            /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
            /* [in] */ STGMEDIUM __RPC_FAR *pStgMedium) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDifferencingVtbl
    {
        
        HRESULT ( __stdcall __RPC_FAR *QueryInterface )( 
            IDifferencing __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( __stdcall __RPC_FAR *AddRef )( 
            IDifferencing __RPC_FAR * This);
        
        ULONG ( __stdcall __RPC_FAR *Release )( 
            IDifferencing __RPC_FAR * This);
        
        /* [local] */ HRESULT ( __stdcall __RPC_FAR *SubtractMoniker )( 
            IDifferencing __RPC_FAR * This,
            /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
            /* [in] */ IMoniker __RPC_FAR *pOtherStg,
            /* [in] */ DifferenceType diffType,
            /* [out][in] */ STGMEDIUM __RPC_FAR *pStgMedium,
            /* [in] */ DWORD reserved);
        
        /* [local] */ HRESULT ( __stdcall __RPC_FAR *SubtractVerid )( 
            IDifferencing __RPC_FAR * This,
            /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
            /* [in] */ VERID __RPC_FAR *pVerid,
            /* [in] */ DifferenceType diffType,
            /* [out][in] */ STGMEDIUM __RPC_FAR *pStgMedium,
            /* [in] */ DWORD reserved);
        
        /* [local] */ HRESULT ( __stdcall __RPC_FAR *SubtractTimeStamp )( 
            IDifferencing __RPC_FAR * This,
            /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
            /* [in] */ FILETIME __RPC_FAR *pTimeStamp,
            /* [in] */ DifferenceType diffType,
            /* [out][in] */ STGMEDIUM __RPC_FAR *pStgMedium,
            /* [in] */ DWORD reserved);
        
        /* [local] */ HRESULT ( __stdcall __RPC_FAR *Add )( 
            IDifferencing __RPC_FAR * This,
            /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
            /* [in] */ STGMEDIUM __RPC_FAR *pStgMedium);
        
    } IDifferencingVtbl;

    interface IDifferencing
    {
        CONST_VTBL struct IDifferencingVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDifferencing_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDifferencing_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDifferencing_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDifferencing_SubtractMoniker(This,pInitiator,pOtherStg,diffType,pStgMedium,reserved)	\
    (This)->lpVtbl -> SubtractMoniker(This,pInitiator,pOtherStg,diffType,pStgMedium,reserved)

#define IDifferencing_SubtractVerid(This,pInitiator,pVerid,diffType,pStgMedium,reserved)	\
    (This)->lpVtbl -> SubtractVerid(This,pInitiator,pVerid,diffType,pStgMedium,reserved)

#define IDifferencing_SubtractTimeStamp(This,pInitiator,pTimeStamp,diffType,pStgMedium,reserved)	\
    (This)->lpVtbl -> SubtractTimeStamp(This,pInitiator,pTimeStamp,diffType,pStgMedium,reserved)

#define IDifferencing_Add(This,pInitiator,pStgMedium)	\
    (This)->lpVtbl -> Add(This,pInitiator,pStgMedium)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT __stdcall IDifferencing_RemoteSubtractMoniker_Proxy( 
    IDifferencing __RPC_FAR * This,
    /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
    /* [in] */ IMoniker __RPC_FAR *pOtherStg,
    /* [in] */ DifferenceType diffType,
    /* [out][in] */ RemSTGMEDIUM __RPC_FAR *__RPC_FAR *ppRemoteMedium,
    /* [in] */ DWORD reserved);


void __RPC_STUB IDifferencing_RemoteSubtractMoniker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT __stdcall IDifferencing_RemoteSubtractVerid_Proxy( 
    IDifferencing __RPC_FAR * This,
    /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
    /* [in] */ VERID __RPC_FAR *pVerid,
    /* [in] */ DifferenceType diffType,
    /* [out][in] */ RemSTGMEDIUM __RPC_FAR *__RPC_FAR *ppRemoteMedium,
    /* [in] */ DWORD reserved);


void __RPC_STUB IDifferencing_RemoteSubtractVerid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT __stdcall IDifferencing_RemoteSubtractTimeStamp_Proxy( 
    IDifferencing __RPC_FAR * This,
    /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
    /* [in] */ FILETIME __RPC_FAR *pTimeStamp,
    /* [in] */ DifferenceType diffType,
    /* [out][in] */ RemSTGMEDIUM __RPC_FAR *__RPC_FAR *ppRemoteMedium,
    /* [in] */ DWORD reserved);


void __RPC_STUB IDifferencing_RemoteSubtractTimeStamp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT __stdcall IDifferencing_RemoteAdd_Proxy( 
    IDifferencing __RPC_FAR * This,
    /* [in] */ IReconcileInitiator __RPC_FAR *pInitiator,
    /* [in] */ RemSTGMEDIUM __RPC_FAR *pRemoteMedium);


void __RPC_STUB IDifferencing_RemoteAdd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDifferencing_INTERFACE_DEFINED__ */


/****************************************
 * Generated header for interface: __MIDL__intf_0062
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [local] */ 


			/* size is 0 */



extern RPC_IF_HANDLE __MIDL__intf_0062_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL__intf_0062_v0_0_s_ifspec;

/****************************************
 * Generated header for interface: IAccessControl
 * at Fri Jul 21 20:06:14 1995
 * using MIDL 2.00.0102
 ****************************************/
/* [unique][uuid][object] */ 




#ifdef __cplusplus
}
#endif

#endif
