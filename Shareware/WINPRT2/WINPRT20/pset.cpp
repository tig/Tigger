// PropSet.cpp
//
// Implementation of a class that can manipulate OLE 2.0 property sets.
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// June 28, 1993
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  June 28, 1993   cek     First implementation.
//  June 30, 1993   cek     Re-wrote.
//

#include "stdafx.h"   
#include <malloc.h>
#include <ole2.h>
#include <variant.h>
#include "propset.h"

#define CHEAT_ON_BSTR

LPSTR CreateVT_BSTR( const LPSTR lpsz )
{
#if defined(NODBCS)
    DWORD cb = lstrlen( lpsz ) + 1  ;
    LPSTR lp = (LPSTR)_fmalloc( (int)cb + sizeof(DWORD) ) ;
    if (lp)
        {
        *(LPDWORD)lp = cb ;
        lstrcpy( lp + sizeof(DWORD), lpsz ) ;
        }
#else
    DWORD cb = (lstrlen( lpsz ) + 1);
    LPSTR lp = (LPSTR)_fmalloc( (int)cb * sizeof(TCHAR) + sizeof(DWORD) ) ;
    if (lp)
        {
        *(LPDWORD)lp = cb ;
        lstrcpy( lp + sizeof(DWORD), lpsz ) ;
        }
#endif
    return lp ;
}


//------------------------------------------------------------------------
//Implementation of the CMyProperty class
//
CMyProperty::CMyProperty( void )
{   
    m_dwPropID = 0 ; 

    m_dwType = VT_EMPTY ;
    m_pValue = NULL ;       // must init to NULL 
} 

CMyProperty::CMyProperty( DWORD dwID, const LPVOID pValue, DWORD dwType ) 
{   
    m_dwPropID = dwID ; 
    m_dwType = dwType ;
    m_pValue = NULL ;       // must init to NULL 
    Set( dwID, pValue, dwType ) ;  
}

CMyProperty::~CMyProperty()
{
    if (m_pValue != NULL)
        _ffree( m_pValue ) ;
    return ;
} 

BOOL CMyProperty::Set( DWORD dwID, const LPVOID pValue, DWORD dwType )
{
    m_dwType = dwType ;
    m_dwPropID = dwID ;
    
    return Set( pValue ) ;
}

BOOL CMyProperty::Set( const LPVOID pValue, DWORD dwType )
{
    m_dwType = dwType ;
    return Set( pValue ) ;
}

BOOL CMyProperty::Set( const  LPVOID pVal )
{    
    ULONG           cb ;     
    ULONG           cbItem ;
    ULONG           cbValue ;
    ULONG           nReps ;
    LPBYTE          pCur ;    
    LPVOID          pValue = pVal ;
    DWORD           dwType = m_dwType ;
#ifdef CHEAT_ON_BSTR
    LPVOID          pValueOrig = NULL ;
#endif  // CHEAT_ON_BSTR

    if (m_pValue != NULL)
    {
        _ffree( m_pValue ) ;
        m_pValue = NULL ;
    }
    
    if (pValue == NULL || m_dwType == 0)
        return TRUE ;

    // Given pValue, determine how big it is
    // Then allocate a new buffer for m_pValue and copy...
    nReps = 1 ;        
    cbValue = 0 ;
    pCur = (LPBYTE)pValue ;
    if (m_dwType & VT_VECTOR)
    {
        // The next DWORD is a count of the elements
        nReps = *(LPDWORD)pValue ;
        cb = sizeof( nReps ) ;
        pCur += cb ;
        cbValue += cb ;
        dwType &= ~VT_VECTOR ;
    }
    else
    {
#ifdef CHEAT_ON_BSTR
        // CHEAT:  If we get any of the BSTR like types,
        // and we are not a vector create the 
        // BSTR for the sap...
        switch (dwType )
        {
            case VT_BSTR:           /* binary string               */
            case VT_LPSTR:          /* null terminated string      */
            case VT_STREAM:         /* Name of the stream follows  */
            case VT_STORAGE:        /* Name of the storage follows */
            case VT_STREAMED_OBJECT:/* Stream contains an object   */
            case VT_STORED_OBJECT:  /* Storage contains an object  */
                pValueOrig = pValue ;
                pValue = (LPVOID)CreateVT_BSTR( (LPSTR)pValueOrig ) ;        
                pCur = (LPBYTE)pValue ;
            break ;
        }
#endif  // CHEAT_ON_BSTR
    }            

    // Since a value can be made up of a vector (VT_VECTOR) of
    // items, we first seek through the value, picking out
    // each item, getting it's size.  
    //
    cbItem = 0 ;        // Size of the current item
    while (nReps--)
    {    
        switch (dwType)
        {
            case VT_EMPTY:          /* nothing                     */
                cbItem = 0 ;
            break ;
            
            case VT_I2:             /* 2 byte signed int           */
            case VT_BOOL:           /* True=-1, False=0            */
                cbItem = 2 ;
            break ;
            
            case VT_I4:             /* 4 byte signed int           */
            case VT_R4:             /* 4 byte real                 */
                cbItem = 4 ;
            break ;
            
            case VT_R8:             /* 8 byte real                 */
            case VT_CY:             /* currency                    */
            case VT_DATE:           /* date                        */
            case VT_I8:             /* signed 64-bit int           */
            case VT_FILETIME:       /* FILETIME                    */
                cbItem = 8 ;
            break ;

// CKINDEL: BUGFIX 5/18
            case VT_CLSID:          /* A Class ID                  */
                cbItem = sizeof(CLSID) ;
            break ;
            
            case VT_BSTR:           /* binary string               */
            case VT_LPSTR:          /* null terminated string      */
            case VT_STREAM:         /* Name of the stream follows  */
            case VT_STORAGE:        /* Name of the storage follows */
            case VT_STREAMED_OBJECT:/* Stream contains an object   */
            case VT_STORED_OBJECT:  /* Storage contains an object  */
            case VT_BLOB_OBJECT:    /* Blob contains an object     */
            case VT_STREAMED_PROPSET:/* Stream contains a propset  */
            case VT_STORED_PROPSET: /* Storage contains a propset  */
            case VT_BLOB_PROPSET:   /* Blob contains a propset     */
            case VT_BLOB:           /* Length prefixed bytes       */
            case VT_CF:             /* Clipboard format            */
                // Get the DWORD that gives us the size, making
                // sure we increment cbValue.
                cbItem = *(LPDWORD)pCur ;
                cb = sizeof(cbItem) ;
                pCur += cb ;
                cbValue += cb ;
            break ;
                        
            case VT_LPWSTR:         /* wide null terminated UINCODE string */
                cbItem = *(LPDWORD)pCur * sizeof(WORD) ;  // Unicode!
                cb = sizeof( cbItem ) ;
                pCur += cb ;
                cbValue += cb ;
            break ;
            
    //        case VT_VARIANT:        /* VARIANT FAR*                */
    //        break ;
            
            default:       
                if (pValueOrig)
                    _ffree( pValue ) ;
                return FALSE ;
        }
         
        // QUESTION:  Can we assume that the data found in a 
        // type VT_VECTOR element has internal padding so that
        // each sub-element is 32 bit aligned?  
        //
        // If so, we need to do the following here:
        //    cb = ((cb + 3) >> 2) << 2 ;
        //
        // Add 'cb' to cbItem before seeking...
        //
        // Seek to the next item
        pCur += cbItem ;
        cbValue += cbItem ;
    }
    
    if (NULL == (m_pValue = _fmalloc( (int)cbValue )))
    {
        TRACE( "_fmalloc failed" ) ;
        return FALSE ;                                  
    }
    _fmemcpy( m_pValue, pValue, (int)cbValue ) ;

#ifdef CHEAT_ON_BSTR
    // This is related to the CHEAT! above
    if (pValueOrig)
        _ffree( pValue ) ;
#endif  // CHEAT_ON_BSTR

    return TRUE ;    
}

LPVOID CMyProperty::Get( void )
{   return Get( (DWORD*)NULL ) ;   }

LPVOID CMyProperty::Get( DWORD* pcb )
{   
    DWORD   cb ;  
    LPBYTE  p = NULL ;
    
    p = (LPBYTE)m_pValue ;
        
    // m_pValue points to a Property "Value" which may
    // have size information included...
    switch( m_dwType )
    {
        case VT_EMPTY:          /* nothing                     */
            cb = 0 ;
        break ;
            
        case VT_I2:             /* 2 byte signed int           */
        case VT_BOOL:           /* True=-1, False=0            */
            cb = sizeof( short ) ;
        break ;
            
        case VT_I4:             /* 4 byte signed int           */
        case VT_R4:             /* 4 byte real                 */
            cb = sizeof( DWORD ) ;
        break ;
            
        case VT_R8:             /* 8 byte real                 */
        case VT_CY:             /* currency                    */
        case VT_DATE:           /* date                        */
        case VT_I8:             /* signed 64-bit int           */
        case VT_FILETIME:       /* FILETIME                    */
            cb = 8 ;
        break ;
          
// CKINDEL: BUGFIX 5/18
        case VT_BLOB:           /* Length prefixed bytes       */
            cb = *(LPDWORD)p ;      
        break ;
            
        case VT_BSTR:           /* binary string               */
        case VT_LPSTR:          /* null terminated string      */
        case VT_STREAM:         /* Name of the stream follows  */
        case VT_STORAGE:        /* Name of the storage follows */
        case VT_STREAMED_OBJECT:/* Stream contains an object   */
        case VT_STORED_OBJECT:  /* Storage contains an object  */
        case VT_BLOB_OBJECT:    /* Blob contains an object     */
        case VT_STREAMED_PROPSET:/* Stream contains a propset  */
        case VT_STORED_PROPSET: /* Storage contains a propset  */
        case VT_BLOB_PROPSET:   /* Blob contains a propset     */
        case VT_CF:             /* Clipboard format            */
        case VT_CLSID:          /* A Class ID                  */
            // Read the DWORD that gives us the size, making
            // sure we increment cbValue.
            cb = *(LPDWORD)p ;      
#ifdef CHEAT_ON_BSTR            
            p += sizeof( DWORD ) ;
#endif // CHEAT_ON_BSTR            
        break ;
                        
        case VT_LPWSTR:         /* wide null terminated UINCODE string */
            cb = *(LPDWORD)p * sizeof( WORD ) ;
#ifdef CHEAT_ON_BSTR            
            p += sizeof( DWORD ) ;
#endif // CHEAT_ON_BSTR            
        break ;
            
//        case VT_VARIANT:        /* VARIANT FAR*                */
//        break ;
            
        default:      
            return NULL ;
    }              
    if (pcb != NULL)
        *pcb = cb ;
    
    return p ;
}

DWORD  CMyProperty::GetType( void )
{   return m_dwType ;  }

void   CMyProperty::SetType( DWORD dwType ) 
{   m_dwType = dwType ; } 
    
DWORD CMyProperty::GetID( void )
{   return m_dwPropID ;   }

void CMyProperty::SetID( DWORD dwPropID ) 
{    m_dwPropID = dwPropID ;   }

LPVOID CMyProperty::GetRawValue( void ) 
{   return m_pValue ; }

BOOL CMyProperty::WriteToStream( IStream* pIStream )
{
    ULONG           cb ;
    ULONG           cbTotal ; // Total size of the whole value
    DWORD           dwType = m_dwType;
    DWORD           nReps ;   
    LPBYTE          pCur ;
                          
    nReps = 1 ;
    pCur = (LPBYTE)m_pValue ;                          
    cbTotal = 0 ;
    if (m_dwType & VT_VECTOR)
    {
        // Value is a DWORD count of elements followed by
        // that many repititions of the value.
        //
        nReps = *(LPDWORD)pCur ;
        cbTotal = sizeof(DWORD) ;
        pCur += cbTotal ;
        dwType &= ~VT_VECTOR ;
    }
                              
    // Figure out how big the data is.
    while (nReps--)
    {    
        switch (dwType)
        {
            case VT_EMPTY:          /* nothing                     */
                cb = 0 ;
            break ;

            case VT_I2:             /* 2 byte signed int           */
            case VT_BOOL:           /* True=-1, False=0            */
                cb = sizeof(short) ;
            break ;
            
            case VT_I4:             /* 4 byte signed int           */
            case VT_R4:             /* 4 byte real                 */
                cb = sizeof(long) ;
            break ;

            case VT_R8:             /* 8 byte real                 */
            case VT_CY:             /* currency                    */
            case VT_DATE:           /* date                        */
            case VT_I8:             /* signed 64-bit int           */
            case VT_FILETIME:       /* FILETIME                    */
                cb = 8 ;
            break ;

// CKINDEL: BUGFIX 5/18
            case VT_CLSID:          /* A Class ID                  */
                cb = sizeof(CLSID) ;
            break ;
            
            case VT_BSTR:           /* binary string               */
            case VT_LPSTR:          /* null terminated string      */
            case VT_BLOB:           /* Length prefixed bytes       */
            case VT_STREAM:         /* Name of the stream follows  */
            case VT_STORAGE:        /* Name of the storage follows */
            case VT_STREAMED_OBJECT:/* Stream contains an object   */
            case VT_STORED_OBJECT:  /* Storage contains an object  */
            case VT_BLOB_OBJECT:    /* Blob contains an object     */
            case VT_STREAMED_PROPSET:/* Stream contains a propset  */
            case VT_STORED_PROPSET: /* Storage contains a propset  */
            case VT_BLOB_PROPSET:   /* Blob contains a propset     */
            case VT_CF:             /* Clipboard format            */
                cb = sizeof(DWORD) + *(LPDWORD)pCur ;
            break ;
    
            case VT_LPWSTR:         /* wide null terminated UINCODE string */
                cb = sizeof(DWORD) + (*(LPDWORD)pCur * sizeof(WORD)) ;
            break ;

    //        case VT_VARIANT:        /* VARIANT FAR*                */
    //        break ;
            
            default:      
                return FALSE ;
        }
         
        // QUESTION:  Can we assume that the data found in a 
        // type VT_VECTOR element has internal padding so that
        // each sub-element is 32 bit aligned?  
        //
        // If so, we need to do the following here:
        //    cb = ((cb + 3) >> 2) << 2 ;
        pCur += cb ;
        cbTotal+= cb ;
    }

    // Write the type
    pIStream->Write((LPVOID)&m_dwType, sizeof(m_dwType), &cb);
    if (cb != sizeof(m_dwType))
        return FALSE ;

    // Write the value 
    pIStream->Write((LPVOID)m_pValue, cbTotal, &cb);
    if (cb != cbTotal)
        return FALSE ;

    // Make sure we are 32 bit aligned
    BYTE    b = 0 ;
    cbTotal = (((cbTotal + 3) >> 2) << 2) - cbTotal ;
    while (cbTotal--)
    {
        pIStream->Write((LPVOID)&b, 1, &cb);
        if (cb != sizeof(BYTE))
            return FALSE ;
    }
    return TRUE ;
}                

BOOL CMyProperty::ReadFromStream( IStream* pIStream )
{                 
    ULONG           cb ;     
    ULONG           cbItem ;
    ULONG           cbValue ;
    DWORD           dwType ;
    ULONG           nReps ;
    LPSTREAM        pIStrItem ;
    LARGE_INTEGER   li ;
    
    // All properties are made up of a type/value pair.
    // The obvious first thing to do is to get the type...
    pIStream->Read( (LPVOID)&m_dwType, sizeof(m_dwType), &cb ) ;
    if (cb != sizeof(m_dwType))
        return FALSE ;
     
    dwType = m_dwType ;
    nReps = 1 ;        
    cbValue = 0 ;
    if (m_dwType & VT_VECTOR)
    {
        // The next DWORD in the stream is a count of the
        // elements
        pIStream->Read( (LPVOID)&nReps, sizeof(nReps), &cb ) ;
        if (cb != sizeof(nReps))
            return FALSE ;
        cbValue += cb ;
        dwType &= ~VT_VECTOR ;
    }

    // Since a value can be made up of a vector (VT_VECTOR) of
    // items, we first seek through the value, picking out
    // each item, getting it's size.  We use a cloned
    // stream for this (pIStrItem).
    // We then use our pIStream to read the entire 'blob' into
    // the allocated buffer.
    //
    cbItem = 0 ;        // Size of the current item
    pIStream->Clone( &pIStrItem ) ;
    ASSERT(pIStrItem != NULL);
    while (nReps--)
    {    
        switch (dwType)
        {
            case VT_EMPTY:          /* nothing                     */
                cbItem = 0 ;
            break ;
            
            case VT_I2:             /* 2 byte signed int           */
            case VT_BOOL:           /* True=-1, False=0            */
                cbItem = sizeof( short ) ;
            break ;
            
            case VT_I4:             /* 4 byte signed int           */
            case VT_R4:             /* 4 byte real                 */
                cbItem = sizeof( DWORD ) ;
            break ;
            
            case VT_R8:             /* 8 byte real                 */
            case VT_CY:             /* currency                    */
            case VT_DATE:           /* date                        */
            case VT_I8:             /* signed 64-bit int           */
            case VT_FILETIME:       /* FILETIME                    */
                cbItem = 8 ;
            break ;

// CKINDEL BUGFIX 5/18/94
            case VT_CLSID:          /* A Class ID                  */
                cbItem = sizeof(CLSID) ;
            break ;
            
            case VT_BSTR:           /* binary string               */
            case VT_LPSTR:          /* null terminated string      */
            case VT_BLOB:           /* Length prefixed bytes       */
            case VT_STREAM:         /* Name of the stream follows  */
            case VT_STORAGE:        /* Name of the storage follows */
            case VT_STREAMED_OBJECT:/* Stream contains an object   */
            case VT_STORED_OBJECT:  /* Storage contains an object  */
            case VT_BLOB_OBJECT:    /* Blob contains an object     */
            case VT_STREAMED_PROPSET:/* Stream contains a propset  */
            case VT_STORED_PROPSET: /* Storage contains a propset  */
            case VT_BLOB_PROPSET:   /* Blob contains a propset     */
            case VT_CF:             /* Clipboard format            */
                // Read the DWORD that gives us the size, making
                // sure we increment cbValue.
                pIStream->Read( (LPVOID)&cbItem, sizeof(cbItem), &cb ) ;
                if (cb != sizeof(cbItem))
                    return FALSE ;
                LISet32( li, -(LONG)cb ) ;
                pIStream->Seek( li, STREAM_SEEK_CUR, NULL ) ;
                cbValue += cb ;
            break ;
                        
            case VT_LPWSTR:         /* wide null terminated UINCODE string */
                pIStream->Read( (LPVOID)&cbItem, sizeof(cbItem), &cb ) ;
                if (cb != sizeof(cbItem))
                    return FALSE ;
                LISet32( li, -(LONG)cb ) ;
                pIStream->Seek( li, STREAM_SEEK_CUR, NULL ) ;
                cbValue += cb ;
                cbItem *= sizeof(WORD) ;  // Unicode!
            break ;
            
    //        case VT_VARIANT:        /* VARIANT FAR*                */
    //        break ;
            
            default:      
                pIStrItem->Release();
                return FALSE ;
        }
         
        // QUESTION:  Can we assume that the data found in a 
        // type VT_VECTOR element has internal padding so that
        // each sub-element is 32 bit aligned?  
        //
        // If so, we need to do the following here:
        //    cb = ((cb + 3) >> 2) << 2 ;
        //
        // Add 'cb' to cbItem before seeking...
        //
        // Seek to the next item
        LISet32( li, cbItem ) ;
        pIStrItem->Seek( li, STREAM_SEEK_CUR, NULL) ;
        cbValue += cbItem ;
    }
               
    pIStrItem->Release();
    
    // Allocate cbValue bytes
    m_pValue = _fmalloc( (int)cbValue ) ;
    if (m_pValue == NULL)
        return FALSE ;                                  
                                     
    // Read the buffer from pIStream
    pIStream->Read( m_pValue, cbValue, &cb ) ;
    if (cb != cbValue)
        return FALSE ;
    
    // Done!
    return TRUE ;
}

  
//------------------------------------------------------------------------
// Implementation of the CMyPropertySection Class
//
CMyPropertySection::CMyPropertySection( void ) 
{
    m_FormatID = GUID_NULL ;
    m_SH.cbSection = 0 ;
    m_SH.cProperties = 0 ;
}      

CMyPropertySection::CMyPropertySection( CLSID FormatID ) 
{
    m_FormatID = FormatID ;
    m_SH.cbSection = 0 ;
    m_SH.cProperties = 0 ;
}      

CMyPropertySection::~CMyPropertySection( void ) 
{
    RemoveAll() ;
    return ;
}
        
CLSID CMyPropertySection::GetFormatID( void )
{   return m_FormatID ; }

void CMyPropertySection::SetFormatID( CLSID FormatID ) 
{   m_FormatID = FormatID ; }

BOOL CMyPropertySection::Set( DWORD dwPropID, LPVOID pValue, DWORD dwType ) 
{
    CMyProperty* pProp = GetProperty( dwPropID ) ;
    if (pProp == NULL)
    {
        if ((pProp = new CMyProperty( dwPropID, pValue, dwType )) != NULL)
            AddProperty( pProp ) ;
        return (pProp != NULL) ;
    }
    pProp->Set( dwPropID, pValue, dwType ) ;
    return TRUE ;
}

BOOL CMyPropertySection::Set( DWORD dwPropID, LPVOID pValue ) 
{
    // Since no dwType was specified, the property is assumed
    // to exist.   Fail if it does not.
    CMyProperty* pProp = GetProperty( dwPropID ) ;
    if (pProp != NULL && pProp->m_dwType)
    {
        pProp->Set( dwPropID, pValue, pProp->m_dwType ) ;
        return TRUE ;
    }
    else
        return FALSE ;
}

LPVOID CMyPropertySection::Get( DWORD dwPropID )
{   return Get( dwPropID, (DWORD*)NULL ) ;  }

LPVOID CMyPropertySection::Get( DWORD dwPropID, DWORD* pcb )
{
    CMyProperty* pProp = GetProperty( dwPropID ) ;
    if (pProp)
        return pProp->Get( pcb ) ;   
    else
        return NULL ;
}

void CMyPropertySection::Remove( DWORD dwID )
{
    POSITION pos ;
    POSITION posRemove = m_PropList.GetHeadPosition();
    CMyProperty*  pProp ;
    while( posRemove != NULL )
    {
        pProp = (CMyProperty*)m_PropList.GetNext( pos ) ;
        if (pProp->m_dwPropID == dwID)
        {
                m_PropList.RemoveAt( posRemove ) ;
            delete pProp ;
            m_SH.cProperties-- ;
            return ;
        }
        posRemove = pos ;
    }        
}

void CMyPropertySection::RemoveAll( )
{                    
    POSITION pos = m_PropList.GetHeadPosition();
    while( pos != NULL )
        delete (CMyProperty*)m_PropList.GetNext( pos ) ;
    m_PropList.RemoveAll() ;
    m_SH.cProperties = 0 ;
}

 
CMyProperty* CMyPropertySection::GetProperty( DWORD dwPropID ) 
{
    POSITION pos = m_PropList.GetHeadPosition() ;
    CMyProperty* pProp ;
    while (pos != NULL)
    {
        pProp= (CMyProperty*)m_PropList.GetNext( pos ) ;
        if (pProp->m_dwPropID == dwPropID )
            return pProp ;
    }
    return NULL ;
}

void CMyPropertySection::AddProperty( CMyProperty* pProp ) 
{
    m_PropList.AddTail( pProp ) ;
    m_SH.cProperties++ ;
}

DWORD CMyPropertySection::GetSize( void ) 
{   return m_SH.cbSection ; }

DWORD CMyPropertySection::GetCount( void ) 
{   return m_PropList.GetCount() ;  }

CObList* CMyPropertySection::GetList( void ) 
{   return &m_PropList ;  }

BOOL CMyPropertySection::WriteToStream( IStream* pIStream )
{
    // Create a dummy property entry for the name dictionary (ID == 0).
    Set(0, NULL, VT_EMPTY);

    ULONG           cb ;
    ULARGE_INTEGER  ulSeekOld ;
    ULARGE_INTEGER  ulSeek ;
    LPSTREAM        pIStrPIDO;
    PROPERTYIDOFFSET  pido ;   
    LARGE_INTEGER   li ;

    // The Section header contains the number of bytes in the
    // section.  Thus we need  to go back to where we should 
    // write the count of bytes
    // after we write all the property sets..
    // We accomplish this by saving the seek pointer to where
    // the size should be written in ulSeekOld
    m_SH.cbSection = 0 ;
    m_SH.cProperties = m_PropList.GetCount() ;
    LISet32( li, 0 ) ;
    pIStream->Seek( li, STREAM_SEEK_CUR, &ulSeekOld);

    pIStream->Write((LPVOID)&m_SH, sizeof(m_SH), &cb);
    if (sizeof(m_SH) != cb)
    {
        TRACE( "Write of section header failed (1).\n" ) ;
        return FALSE ;
    }
                                                     
    if (m_PropList.IsEmpty())
    {
        TRACE( "Warning: Wrote empty property section.\n" ) ;
        return TRUE ;
    }
                                                     
    // After the section header is the list of property ID/Offset pairs
    // Since there is an ID/Offset pair for each property and we
    // need to write the ID/Offset pair as we write each property
    // we clone the stream and use the clone to access the
    // table of ID/offset pairs (PIDO)...
    //
    pIStream->Clone( &pIStrPIDO ) ;

    // Now seek pIStream past the PIDO list
    //
    LISet32( li,  m_SH.cProperties * sizeof( PROPERTYIDOFFSET ) ) ;
    pIStream->Seek( li, STREAM_SEEK_CUR, &ulSeek);

    // Now write each section to pIStream.
    CMyProperty* pProp = NULL;
    POSITION pos = m_PropList.GetHeadPosition();
    while( pos != NULL )
    {
        // Get next element (note cast)
        pProp = (CMyProperty*)m_PropList.GetNext( pos );

        if (pProp->m_dwPropID != 0)
        {                              
            // Write it 
            if (!pProp->WriteToStream( pIStream ))
            {
                pIStrPIDO->Release() ;
                return FALSE ;
            }
        }
        else
        {
            if (!WriteNameDictToStream( pIStream ))
            {
                pIStrPIDO->Release() ;
                return FALSE ;
            }
        }
        
        // Using our cloned stream write the Format ID / Offset pair
        // The offset to this property is the current seek pointer
        // minus the pointer to the beginning of the section                              
        pido.dwOffset = ulSeek.LowPart - ulSeekOld.LowPart ;
        pido.propertyID = pProp->m_dwPropID ;
        pIStrPIDO->Write((LPVOID)&pido, sizeof(pido), &cb) ;
        if (sizeof(pido) != cb)
        {
            TRACE( "Write of 'pido' failed\n" ) ;
            pIStrPIDO->Release() ;
            return FALSE ;
        }

        // Get the seek offset after the write
        LISet32( li, 0 ) ;
        pIStream->Seek( li, STREAM_SEEK_CUR, &ulSeek ) ;
    }

    pIStrPIDO->Release() ;

    // Now go back to ulSeekOld and write the section header.
    // Size of section is current seek point minus old seek point
    //
    m_SH.cbSection = ulSeek.LowPart - ulSeekOld.LowPart ;
    
    // Seek to beginning of this section and write the section header.
    LISet32( li, ulSeekOld.LowPart ) ;
    pIStream->Seek( li, STREAM_SEEK_SET, NULL ) ;
    pIStream->Write((LPVOID)&m_SH, sizeof(m_SH), &cb);
    if (sizeof(m_SH) != cb)
    {
        TRACE( "Write of section header failed (2).\n" ) ;
        return FALSE ;
    }
   
    return TRUE ;
}                

BOOL CMyPropertySection::ReadFromStream( IStream* pIStream )
{   
    ULONG               cb ;         
    PROPERTYIDOFFSET    pido ;
    ULONG               cProperties ;
    LPSTREAM            pIStrPIDO ;      
    ULARGE_INTEGER      ulSectionStart ;
    LARGE_INTEGER       li ;
    CMyProperty*          pProp ;

    if (m_SH.cProperties || !m_PropList.IsEmpty())
        RemoveAll() ;
              
    // pIStream is pointing to the beginning of the section we
    // are to read.  First there is a DWORD that is the count
    // of bytes in this section, then there is a count 
    // of properties, followed by a list of propertyID/offset pairs,
    // followed by type/value pairs.
    //                
    LISet32( li, 0 ) ;
    pIStream->Seek( li, STREAM_SEEK_CUR, &ulSectionStart ) ;
    pIStream->Read( (LPVOID)&m_SH, sizeof(m_SH), &cb ) ;
    if (cb != sizeof(m_SH))
        return FALSE ;
                      
    // Now we're pointing at the first of the PropID/Offset pairs
    // (PIDOs).   To get to each property we use a cloned stream
    // to stay back and point at the PIDOs (pIStrPIDO).  We seek
    // pIStream to each of the Type/Value pairs, creating CProperites
    // and so forth as we go...
    // 
    pIStream->Clone( &pIStrPIDO ) ;
    
    cProperties = m_SH.cProperties ;
    while (cProperties--)
    {
        pIStrPIDO->Read( (LPVOID)&pido, sizeof( pido ), &cb ) ;
        if (cb != sizeof(pido))
        {
            pIStrPIDO->Release() ;
            return FALSE ;
        }

        // Do an absolute seek from the beginning of the stream
        LISet32( li, ulSectionStart.LowPart + pido.dwOffset ) ;
        pIStream->Seek( li, STREAM_SEEK_SET, NULL ) ;

        // Now pIStream is at the type/value pair
        if (pido.propertyID != 0)
        {        
            pProp = new CMyProperty( pido.propertyID, NULL, 0 ) ;
            pProp->ReadFromStream( pIStream ) ;
            AddProperty( pProp ) ;
        }
        else
        {
            ReadNameDictFromStream( pIStream ) ;
        }
    }                                   
    
    pIStrPIDO->Release() ;
    
    return TRUE ;
}

BOOL CMyPropertySection::GetID( LPCSTR pszName, DWORD* pdwPropID )
{
    CString strName(pszName);
    strName.MakeLower();        // Dictionary stores all names in lowercase
    
    void* pvID;
    if (m_NameDict.Lookup(strName, pvID))
    {
        *pdwPropID = (DWORD)pvID;
        return TRUE;
    }

    TRACE( "Failed to find entry in dictionary.\n" ) ;  
    return FALSE;
}

BOOL CMyPropertySection::SetName( DWORD dwPropID, LPCSTR pszName )
{
    BOOL bSuccess = TRUE;
    CString strName(pszName);
    strName.MakeLower();        // Dictionary stores all names in lowercase

    TRY
    {
        m_NameDict.SetAt(strName, (void*)dwPropID);
    }
    CATCH (CException, e)
    {
        TRACE( "Failed to add entry to dictionary.\n" ) ;
        bSuccess = FALSE;
    }
    END_CATCH
    
    return bSuccess;
}

struct DICTENTRYHEADER
{
    DWORD dwPropID;
    DWORD cb;
};

struct DICTENTRY
{
    DICTENTRYHEADER hdr;
    char sz[256];
};

BOOL CMyPropertySection::ReadNameDictFromStream( IStream* pIStream )
{
    ULONG cb;

    // Read dictionary header (count).
    ULONG cProperties = 0;
    pIStream->Read((LPVOID)&cProperties, sizeof(cProperties), &cb);
    if (sizeof(cProperties) != cb)
    {
        TRACE( "Read of dictionary header failed.\n" ) ;
        return FALSE ;
    }

    ULONG iProp;
    DICTENTRY entry;

    for (iProp = 0; iProp < cProperties; iProp++)
    {
        // Read entry header (dwPropID, cb).
        pIStream->Read((LPVOID)&entry, sizeof(DICTENTRYHEADER), &cb);
        if (sizeof(DICTENTRYHEADER) != cb)
        {
            TRACE( "Read of dictionary entry failed.\n" ) ;
            return FALSE;
        }
        
        // Read entry data (name).
        pIStream->Read((LPVOID)&entry.sz, entry.hdr.cb, &cb);
        if (entry.hdr.cb != cb)
        {
            TRACE( "Read of dictionary entry failed.\n" ) ;
            return FALSE;
        }

        // Section's "name" appears first in list and has dwPropID == 0.
        if ((iProp == 0) && (entry.hdr.dwPropID == 0))
            m_strSectionName = entry.sz;            // Section name
        else
            SetName(entry.hdr.dwPropID, entry.sz);  // Some other property
    }

    return TRUE;
}                                                 


BOOL WriteNameDictEntry( IStream* pIStream, DWORD dwPropID, CString& strName)
{
    ULONG cb;
    DICTENTRY entry;
    
    entry.hdr.dwPropID = dwPropID;
    entry.hdr.cb = min(strName.GetLength() + 1, 256) * sizeof(char);
    memcpy(entry.sz, (LPCSTR)strName, (size_t)entry.hdr.cb);
        
    pIStream->Write((LPVOID)&entry, sizeof(DICTENTRYHEADER) + entry.hdr.cb, &cb);
    if (sizeof(DICTENTRYHEADER) + entry.hdr.cb != cb)
    {
        TRACE( "Write of dictionary entry failed.\n" );
        return FALSE;
    }

    return TRUE;
}

BOOL CMyPropertySection::WriteNameDictToStream( IStream* pIStream )
{
    ULONG cb;

    // Write dictionary header (count).
    ULONG cProperties = m_NameDict.GetCount() + 1;
    pIStream->Write((LPVOID)&cProperties, sizeof(cProperties), &cb);
    if (sizeof(cProperties) != cb)
    {
        TRACE( "Write of dictionary header failed.\n" ) ;
        return FALSE ;
    }

    POSITION pos;
    CString strName;
    void* pvID;

    // Write out section's "name" with dwPropID == 0 first
    if (! WriteNameDictEntry(pIStream, 0, m_strSectionName))
        return FALSE;
    
    // Enumerate contents of dictionary and write out (dwPropID, cb, name). 
    pos = m_NameDict.GetStartPosition();
    while (pos != NULL)
    {
        m_NameDict.GetNextAssoc( pos, strName, pvID ) ;
        if (! WriteNameDictEntry(pIStream, (DWORD)pvID, strName))
            return FALSE;
    }

    return TRUE;
}

BOOL CMyPropertySection::SetSectionName( LPCSTR pszName )
{
    m_strSectionName = pszName;
    return TRUE;
}

LPCSTR CMyPropertySection::GetSectionName( void )
{
    return (LPCSTR)m_strSectionName;
}

//------------------------------------------------------------------------
// Implementation of the CMyPropertySet class
//
CMyPropertySet::CMyPropertySet( void ) 
{
    m_PH.wByteOrder = 0xFFFE ;
    m_PH.wFormat = 0 ;
    #ifdef WIN32
    m_PH.dwOSVer = (DWORD)MAKELONG( LOWORD(GetVersion()), 2 ) ;
    #else
    m_PH.dwOSVer = (DWORD)MAKELONG( LOWORD(GetVersion()), 0 ) ;
    #endif
    m_PH.clsID =  GUID_NULL ;
    m_PH.cSections = 0 ;
        
}

CMyPropertySet::CMyPropertySet( CLSID clsID ) 
{
    m_PH.wByteOrder = 0xFFFE ;
    m_PH.wFormat = 0 ;
    #ifdef WIN32
    m_PH.dwOSVer = (DWORD)MAKELONG( LOWORD(GetVersion()), 2 ) ;
    #else
    m_PH.dwOSVer = (DWORD)MAKELONG( LOWORD(GetVersion()), 0 ) ;
    #endif
    m_PH.clsID = clsID ;
    m_PH.cSections = 0 ;
}

CMyPropertySet::~CMyPropertySet()
{   RemoveAll() ;  }

BOOL CMyPropertySet::Set( CLSID FormatID, DWORD dwPropID, LPVOID pValue, DWORD dwType ) 
{
    CMyPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect == NULL)
    {
        if ((pSect = new CMyPropertySection( FormatID )) != NULL)
            AddSection( pSect ) ;
    }    
    pSect->Set( dwPropID, pValue, dwType ) ;
    return TRUE ;
}

BOOL CMyPropertySet::Set( CLSID FormatID, DWORD dwPropID, LPVOID pValue ) 
{
    // Since there is no dwType, we have to assume that the property
    // already exists.  If it doesn't, fail.
    CMyPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect != NULL)
        return pSect->Set( dwPropID, pValue ) ;
    else
        return FALSE ;
}
    
LPVOID CMyPropertySet::Get( CLSID FormatID, DWORD dwPropID, DWORD* pcb ) 
{
    CMyPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect)
        return pSect->Get( dwPropID, pcb ) ;
    else
        return NULL ;
}

LPVOID CMyPropertySet::Get( CLSID FormatID, DWORD dwPropID ) 
{   return Get( FormatID, dwPropID, (DWORD*)NULL ) ; }

void CMyPropertySet::Remove( CLSID FormatID, DWORD dwPropID ) 
{
    CMyPropertySection*  pSect = GetSection( FormatID );
    if (pSect)
        pSect->Remove( dwPropID ) ;
}

void CMyPropertySet::Remove( CLSID FormatID ) 
{            
    CMyPropertySection* pSect ;
    POSITION pos ;
    POSITION posRemove = m_SectionList.GetHeadPosition() ;        
    while( posRemove != NULL )
    {
        pSect = (CMyPropertySection*)m_SectionList.GetNext( pos ) ;
        if (IsEqualCLSID( pSect->m_FormatID, FormatID ))
        {
            m_SectionList.RemoveAt( posRemove ) ;
            delete pSect ;
            m_PH.cSections-- ;
            return ;
        }
        posRemove = pos ;
    }        
}

void CMyPropertySet::RemoveAll( ) 
{
    POSITION pos = m_SectionList.GetHeadPosition();
    while( pos != NULL )
    {
        delete (CMyPropertySection*)m_SectionList.GetNext( pos ) ;  
    }
    m_SectionList.RemoveAll() ;
    m_PH.cSections = 0 ;
}

CMyPropertySection* CMyPropertySet::GetSection( CLSID FormatID ) 
{
    POSITION pos = m_SectionList.GetHeadPosition() ;
    CMyPropertySection* pSect ;
    while (pos != NULL)
    {
        pSect = (CMyPropertySection*)m_SectionList.GetNext( pos ) ;
        if (IsEqualCLSID( pSect->m_FormatID, FormatID ))
            return pSect ;
    }
    return NULL ;
}

CMyPropertySection* CMyPropertySet::AddSection( CLSID FormatID ) 
{                                         
    CMyPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect)
        return pSect ;
        
    pSect = new CMyPropertySection( FormatID )  ;
    if (pSect)
        AddSection( pSect ) ;
    return pSect ;
}

void CMyPropertySet::AddSection( CMyPropertySection* pSect ) 
{
    m_SectionList.AddTail( pSect ) ;
    m_PH.cSections++ ;
}

CMyProperty* CMyPropertySet::GetProperty( CLSID FormatID, DWORD dwPropID ) 
{
    CMyPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect)
        return pSect->GetProperty( dwPropID ) ;
    else
        return NULL ;
}

void CMyPropertySet::AddProperty( CLSID FormatID, CMyProperty* pProp ) 
{
    CMyPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect)
        pSect->AddProperty( pProp ) ;
}

WORD CMyPropertySet::GetByteOrder( void ) 
{   return m_PH.wByteOrder ;  }
  
WORD CMyPropertySet::GetFormatVersion( void ) 
{   return m_PH.wFormat ;  }
  
void CMyPropertySet::SetFormatVersion( WORD wFmtVersion ) 
{   m_PH.wFormat = wFmtVersion ;  }

DWORD CMyPropertySet::GetOSVersion( void ) 
{   return m_PH.dwOSVer ;  }
  
void CMyPropertySet::SetOSVersion( DWORD dwOSVer ) 
{   m_PH.dwOSVer = dwOSVer ;  }

CLSID CMyPropertySet::GetClassID( void ) 
{   return m_PH.clsID;  }
  
void CMyPropertySet::SetClassID( CLSID clsID ) 
{   m_PH.clsID = clsID ;  }

DWORD CMyPropertySet::GetCount( void ) 
{   return m_SectionList.GetCount() ;  }

CObList* CMyPropertySet::GetList( void ) 
{   return &m_SectionList ;  }

  
BOOL CMyPropertySet::WriteToStream( IStream* pIStream )
{
    LPSTREAM        pIStrFIDO;
    FORMATIDOFFSET  fido ;
    ULONG           cb ;
    ULARGE_INTEGER  ulSeek ;
    LARGE_INTEGER   li ;
                               
    // Write the Property List Header    
    m_PH.cSections = m_SectionList.GetCount() ;
    pIStream->Write((LPVOID)&m_PH, sizeof(m_PH), &cb);
    if (sizeof(m_PH) != cb)
    {
        TRACE( "Write of Property Set Header failed.\n" ) ;
        return FALSE ;
    }

    if (m_SectionList.IsEmpty())
    {
        TRACE( "Warning: Wrote empty property set.\n" ) ;
        return TRUE ;
    }
    
    // After the header is the list of Format ID/Offset pairs
    // Since there is an ID/Offset pair for each section and we
    // need to write the ID/Offset pair as we write each section
    // we clone the stream and use the clone to access the
    // table of ID/offset pairs (FIDO)...
    //
    pIStream->Clone( &pIStrFIDO ) ;

    // Now seek pIStream past the FIDO list
    //                
    LISet32( li, m_PH.cSections * sizeof( FORMATIDOFFSET ) ) ;
    pIStream->Seek( li, STREAM_SEEK_CUR, &ulSeek);
    
    // Write each section.
    CMyPropertySection*   pSect = NULL;
    POSITION            pos = m_SectionList.GetHeadPosition();
    while( pos != NULL )
    {
        // Get next element (note cast)
        pSect = (CMyPropertySection*)m_SectionList.GetNext( pos );
                   
        // Write it
        if (!pSect->WriteToStream( pIStream ))
        {
            pIStrFIDO->Release() ;
            return FALSE ;
        }
        
        // Using our cloned stream write the Format ID / Offset pair
        fido.formatID = pSect->m_FormatID ;
        fido.dwOffset = ulSeek.LowPart ;
        pIStrFIDO->Write((LPVOID)&fido, sizeof(fido), &cb) ;
        if (sizeof(fido) != cb)
        {
            TRACE( "Write of 'fido' failed.\n" ) ;
            pIStrFIDO->Release() ;
            return FALSE ;
        }
        
        // Get the seek offset (for pIStream) after the write
        LISet32( li, 0 ) ;
        pIStream->Seek( li, STREAM_SEEK_CUR, &ulSeek ) ;
    }
     
    pIStrFIDO->Release() ;
    
    return TRUE ;
}                

BOOL CMyPropertySet::ReadFromStream( IStream* pIStream )
{                
    ULONG               cb ;         
    FORMATIDOFFSET      fido ;
    ULONG               cSections ;
    LPSTREAM            pIStrFIDO ;      
    CMyPropertySection*   pSect ;
    LARGE_INTEGER       li ;

    if (m_PH.cSections || !m_SectionList.IsEmpty())
         RemoveAll() ;
              
    // The stream starts like this:
    //  wByteOrder   wFmtVer   dwOSVer   clsID  cSections
    // Which is nice, because our PROPHEADER is the same!
    pIStream->Read( (LPVOID)&m_PH, sizeof( m_PH ), &cb ) ;
    if (cb != sizeof(m_PH))
        return FALSE ;

    // Now we're pointing at the first of the FormatID/Offset pairs
    // (FIDOs).   To get to each section we use a cloned stream
    // to stay back and point at the FIDOs (pIStrFIDO).  We seek
    // pIStream to each of the sections, creating CProperitySection
    // and so forth as we go...
    // 
    pIStream->Clone( &pIStrFIDO ) ;
    
    cSections = m_PH.cSections ;
    while (cSections--)
    {
        pIStrFIDO->Read( (LPVOID)&fido, sizeof( fido ), &cb ) ;
        if (cb != sizeof(fido))
        {
            pIStrFIDO->Release() ;
            return FALSE ;
        }

        // Do an absolute seek from the beginning of the stream
        LISet32( li, fido.dwOffset ) ;
        pIStream->Seek( li, STREAM_SEEK_SET, NULL ) ;
        
        // Now pIStream is at the type/value pair
        pSect = new CMyPropertySection ;
        pSect->SetFormatID( fido.formatID ) ;
        pSect->ReadFromStream( pIStream ) ;
        m_SectionList.AddTail( pSect ) ;
    }                                   

    pIStrFIDO->Release() ;
    return TRUE ;
}

