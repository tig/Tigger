// cprop.cpp : implementation file
//
// CControlProperty is a base class that can be used to derive
// sub-property classes.   For example, if you have a 
// Control with an "Address" property, and the Address property
// had it's own properties, this class would allow you to  easily
// save/restore those properties as part of your control's
// DoPropExchange mechanism.
//
// A PX_Property() function is provided (see end of cprop.cpp) which
// can be called like any of the normal PX_ functions to do 
// property exchange.  This function stores the properties of the
// CControlProp derived object into a VT_BLOB_PROPSET type property
// using PX_Blob( pPX ... ).
//

#include "stdafx.h"
#include "winprt20.h"
#include "propset.h"
#include "ctlimpl.h"
#include "cprop.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef OSTYPE
    #ifndef WIN32
        #ifdef _MAC_
            #define OSTYPE  1   // Mac  
        #else
            #define OSTYPE  0   // Win16
        #endif
    #else
        #define OSTYPE  2   // Win32
    #endif
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CControlProperty
IMPLEMENT_DYNAMIC(CControlProperty,CCmdTarget)

CControlProperty::CControlProperty()
{
}

CControlProperty::~CControlProperty()
{
}

BOOL CControlProperty::GetPropsetData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium, REFCLSID fmtid )
{
    ASSERT_VALID(this);
    ASSERT(AfxIsValidAddress(lpFormatEtc, sizeof(FORMATETC), FALSE));
    ASSERT(AfxIsValidAddress(lpStgMedium, sizeof(STGMEDIUM)));

    // Allow IStream as the storage medium.
    if (!(lpFormatEtc->tymed & TYMED_ISTREAM))
    {
        TRACE("Propset only supported for stream or storage.\n");
        return FALSE;
    }

    LPSTREAM lpStream = NULL;

    // Create the property set using our CLSID
    //
    CLSID clsid ;
    GetClassID( &clsid ) ;
    CPropertySet pset( clsid ) ;
    pset.SetOSVersion(MAKELONG( LOWORD(GetVersion()), OSTYPE) ) ;

    // Add a section using our CLSID as the FMTID
    //
    CPropertySection* ppsec = pset.AddSection( fmtid );
    if (ppsec == NULL)
    {
        TRACE("CPropertySet::AddSection failed.\n");
        return FALSE ;
    }

    // Set the name
    //
    ppsec->SetSectionName( m_strName ) ;

    // Create a PropExchange object on the stack and leverage
    // the CDK implementation
    //
    CPropsetPropExchange propx( *ppsec, NULL, FALSE );
    
    // Use propx to save properties
    BOOL bPropExchange = FALSE ;
    TRY
    {
        DoPropExchange(&propx);
        bPropExchange = TRUE;
    }
    END_TRY
    
    if (!bPropExchange)
    {
        TRACE("Property Exchange failed in ::DoPropExchange.\n");
        return FALSE ;
    }

    lpStgMedium->pstm = _AfxCreateMemoryStream();
    if (!lpStgMedium->pstm)  
    {
        TRACE("Failed to allocate memory for stream.\n");
        return FALSE ;
    }
    
    ASSERT_POINTER(lpStgMedium->pstm, IStream);

    // write propx (ppsec) as a BLOB
    if (FAILED(pset.WriteToStream( lpStgMedium->pstm )))
    {
        TRACE("CPropertySet::WriteToStream failed in ::DoPropExchange.\n");
        lpStgMedium->pstm->Release();
        lpStgMedium->pstm = NULL ;
        return FALSE ;
    }

    lpStgMedium->tymed = TYMED_ISTREAM;
    lpStgMedium->pUnkForRelease = NULL;
    
    return TRUE ;
}

BOOL CControlProperty::SetPropsetData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium, REFCLSID fmtid )
{
#ifndef _DEBUG
    UNUSED lpFormatEtc; // unused in retail builds
#endif

    ASSERT_VALID(this);
    ASSERT(AfxIsValidAddress(lpFormatEtc, sizeof(FORMATETC), FALSE));
    ASSERT(AfxIsValidAddress(lpStgMedium, sizeof(STGMEDIUM)));

    // Get the stream that contains the property set.
    LPSTREAM lpStream = NULL;
    switch (lpStgMedium->tymed)
    {
        case TYMED_ISTREAM:
            lpStream = lpStgMedium->pstm;
            break;
            
        default:
            TRACE("CControlProperty only supports streams.\n");
            return FALSE;
    }

    ASSERT_POINTER(lpStream, IStream);

    // Read the property set from the stream.
    //
    CPropertySet pset;
    if (!pset.ReadFromStream(lpStream))
    {
        TRACE("CPropertySet::ReadFromStream failed.\n");
        return FALSE;
    }

    CPropertySection* ppsec = pset.GetSection(fmtid);
    if (ppsec == NULL)
    {
        TRACE("Section not found in property set.\n");
        return FALSE;
    }
            
    // Parse the property set.
    CPropsetPropExchange propx(*ppsec, NULL, TRUE);
    BOOL bPropExchange = FALSE;
    TRY
    {
        DoPropExchange(&propx);
        bPropExchange = TRUE;
    }
    END_TRY
    
    return bPropExchange;
}

CString& CControlProperty::GetName() 
{
    return m_strName ;
}

void CControlProperty::SetName( const CString& strName ) 
{
    m_strName = strName ;
}

// virtual
void CControlProperty::DoPropExchange(CPropExchange* pPX)
{
    UNUSED pPX ;
}                 

// virtual
void CControlProperty::OnResetState() 
{
}
                                 
BOOL PX_Property( CPropExchange* pPX, LPCSTR pszPropName, CControlProperty* pControlProp) 
{
    LPSTREAM lpStream = NULL ;
    BOOL bSuccess = FALSE;
    LARGE_INTEGER liZero = {0,0};    
        
    if (pPX->m_bLoading)
    {
        // We're loading so we need to create an IStream from
        // the blob.
        //
        void*    pvBlob = NULL ;
        PX_Blob( pPX, pszPropName, pvBlob, NULL ) ;
        if (pvBlob != NULL)
        {
            TRACE("PX_Blob got pvBlob (%lu)\n", (DWORD)*(LPDWORD)pvBlob ) ;
            lpStream = _AfxCreateMemoryStream() ;
            if (lpStream != NULL)
            {
                ULONG   cb = *(ULONG*)pvBlob ; 
                ASSERT(cb) ;
                lpStream->Write( (LPBYTE)pvBlob+sizeof(DWORD), cb, NULL ) ;
                ULARGE_INTEGER ul ;
                lpStream->Seek( liZero, STREAM_SEEK_SET, &ul );
                    
                // Call CWinPrintHeader::SetPropsetData to set the 
                // object's properties
                //
                FORMATETC   fetc ;
                memset( &fetc, 0, sizeof(FORMATETC)) ;   // Not used by SetPropsetData
                
                STGMEDIUM   stgm ;
                stgm.tymed = TYMED_ISTREAM ;
                stgm.pstm = lpStream ;
                stgm.pUnkForRelease = NULL ;
    
                bSuccess = pControlProp->SetPropsetData( &fetc, &stgm, CLSID_PersistPropset ) ;
                
                // Free the stream
                lpStream->Release() ;
            }
            delete []pvBlob ;
        }
    }
    else
    {
        // Saving, have the object create a stream and write to it,
        // and then we convert it to a blob and do PX_Blob.
        // 
        FORMATETC   fetc ;
        memset( &fetc, 0, sizeof(FORMATETC)) ;   // Not used by GetPropsetData
        fetc.tymed = TYMED_ISTREAM ;

        STGMEDIUM   stgm ;
        memset( &fetc, 0, sizeof(STGMEDIUM)) ;   // GetPropsetData creates a stream
        stgm.tymed = TYMED_ISTREAM ;
        if (pControlProp->GetPropsetData( &fetc, &stgm, CLSID_PersistPropset ))
        {
            // Turn it into a BLOB
            //
            ASSERT_POINTER(stgm.pstm, IStream) ;
    
            // stgm.pstm now holds our property set
            //
            ULARGE_INTEGER uliStart;
            ULARGE_INTEGER uliEnd;
            LARGE_INTEGER liZero = {0,0};
        
            // REVIEW: We assume the stream length fits in a DWORD.  Is this OK?
            stgm.pstm->Seek( liZero, STREAM_SEEK_SET, NULL) ;
            if (SUCCEEDED(stgm.pstm->Seek(liZero, STREAM_SEEK_CUR, &uliStart)) &&
                SUCCEEDED(stgm.pstm->Seek(liZero, STREAM_SEEK_END, &uliEnd)) &&
                SUCCEEDED(stgm.pstm->Seek(*(LARGE_INTEGER*)&uliStart, STREAM_SEEK_SET, NULL)))
            {
                DWORD cb = uliEnd.LowPart - uliStart.LowPart;

        // HACK BUGWORKAROUND
        // additional size DWORD on the write...
        //
                BYTE FAR* pbData = new BYTE[cb+sizeof(DWORD)  +sizeof(DWORD)  ];
                *(LPDWORD)pbData = cb+sizeof(DWORD);
                *((LPDWORD)pbData+1) = cb ;
                if (SUCCEEDED(stgm.pstm->Read( pbData+sizeof(DWORD)  +sizeof(DWORD)  , cb, &cb)))
                {
                    bSuccess = PX_Blob(pPX, pszPropName, (void *&)pbData, NULL ) ;
                }
                delete [] pbData;
            }
        }
        else
        {
            TRACE("GetPropsetData failed in PX_Property\n" ) ;
        }
    }
    return bSuccess ;
}

