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
    DWORD cb = lstrlen( lpsz ) + 1  ;
    LPSTR lp = (LPSTR)_fmalloc( (int)cb + sizeof(DWORD) ) ;
    if (!lp)
        return NULL ;
        
    *(LPDWORD)lp = cb ;
    lstrcpy( lp + sizeof(DWORD), lpsz ) ;
    return lp ;
}


//------------------------------------------------------------------------
//Implementation of the CProperty class
//
CProperty::CProperty( void )
{   
    m_dwPropID = 0 ; 

    m_dwType = VT_EMPTY ;
    m_pValue = NULL ;       // must init to NULL 
} 

CProperty::CProperty( DWORD dwID, const LPVOID pValue, DWORD dwType ) 
{   
    m_dwPropID = dwID ; 
    m_dwType = dwType ;
    m_pValue = NULL ;       // must init to NULL 
    Set( dwID, pValue, dwType ) ;  
}

CProperty::~CProperty()
{
    if (m_pValue != NULL)
        _ffree( m_pValue ) ;
    return ;
} 

BOOL CProperty::Set( DWORD dwID, const LPVOID pValue, DWORD dwType )
{
    m_dwType = dwType ;
    m_dwPropID = dwID ;
    
    return Set( pValue ) ;
}

BOOL CProperty::Set( const LPVOID pValue, DWORD dwType )
{
    m_dwType = dwType ;
    return Set( pValue ) ;
}

BOOL CProperty::Set( const  LPVOID pVal )
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
            
            case VT_BSTR:           /* binary string               */
            case VT_LPSTR:          /* null terminated string      */
            case VT_STREAM:         /* Name of the stream follows  */
            case VT_STORAGE:        /* Name of the storage follows */
            case VT_STREAMED_OBJECT:/* Stream contains an object   */
            case VT_STORED_OBJECT:  /* Storage contains an object  */
            case VT_BLOB_OBJECT:    /* Blob contains an object     */
            case VT_BLOB:           /* Length prefixed bytes       */
            case VT_CF:             /* Clipboard format            */
            case VT_CLSID:          /* A Class ID                  */
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

LPVOID CProperty::Get( void )
{   return Get( (DWORD*)NULL ) ;   }

LPVOID CProperty::Get( DWORD* pcb )
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
            
        case VT_BSTR:           /* binary string               */
        case VT_LPSTR:          /* null terminated string      */
        case VT_BLOB:           /* Length prefixed bytes       */
        case VT_STREAM:         /* Name of the stream follows  */
        case VT_STORAGE:        /* Name of the storage follows */
        case VT_STREAMED_OBJECT:/* Stream contains an object   */
        case VT_STORED_OBJECT:  /* Storage contains an object  */
        case VT_BLOB_OBJECT:    /* Blob contains an object     */
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

DWORD  CProperty::GetType( void )
{   return m_dwType ;  }

void   CProperty::SetType( DWORD dwType ) 
{   m_dwType = dwType ; } 
    
DWORD CProperty::GetID( void )
{   return m_dwPropID ;   }

void CProperty::SetID( DWORD dwPropID ) 
{    m_dwPropID = dwPropID ;   }

LPVOID CProperty::GetRawValue( void ) 
{   return m_pValue ; }

BOOL CProperty::WriteToStream( IStream* pIStream )
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
            
            case VT_BSTR:           /* binary string               */
            case VT_LPSTR:          /* null terminated string      */
            case VT_BLOB:           /* Length prefixed bytes       */
            case VT_STREAM:         /* Name of the stream follows  */
            case VT_STORAGE:        /* Name of the storage follows */
            case VT_STREAMED_OBJECT:/* Stream contains an object   */
            case VT_STORED_OBJECT:  /* Storage contains an object  */
            case VT_BLOB_OBJECT:    /* Blob contains an object     */
            case VT_CF:             /* Clipboard format            */
            case VT_CLSID:          /* A Class ID                  */
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

BOOL CProperty::ReadFromStream( IStream* pIStream )
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
            
            case VT_BSTR:           /* binary string               */
            case VT_LPSTR:          /* null terminated string      */
            case VT_BLOB:           /* Length prefixed bytes       */
            case VT_STREAM:         /* Name of the stream follows  */
            case VT_STORAGE:        /* Name of the storage follows */
            case VT_STREAMED_OBJECT:/* Stream contains an object   */
            case VT_STORED_OBJECT:  /* Storage contains an object  */
            case VT_BLOB_OBJECT:    /* Blob contains an object     */
            case VT_CF:             /* Clipboard format            */
            case VT_CLSID:          /* A Class ID                  */
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
// Implementation of the CPropertySection Class
//
CPropertySection::CPropertySection( void ) 
{
    m_FormatID = GUID_NULL ;
    m_SH.cbSection = 0 ;
    m_SH.cProperties = 0 ;
}      

CPropertySection::CPropertySection( CLSID FormatID ) 
{
    m_FormatID = FormatID ;
    m_SH.cbSection = 0 ;
    m_SH.cProperties = 0 ;
}      

CPropertySection::~CPropertySection( void ) 
{
    RemoveAll() ;
    return ;
}
        
CLSID CPropertySection::GetFormatID( void )
{   return m_FormatID ; }

void CPropertySection::SetFormatID( CLSID FormatID ) 
{   m_FormatID = FormatID ; }

BOOL CPropertySection::Set( DWORD dwPropID, LPVOID pValue, DWORD dwType ) 
{
    CProperty* pProp = GetProperty( dwPropID ) ;
    if (pProp == NULL)
    {
        if (pProp = new CProperty( dwPropID, pValue, dwType ))
            AddProperty( pProp ) ;
        return (pProp != NULL) ;
    }
    pProp->Set( dwPropID, pValue, dwType ) ;
    return TRUE ;
}

BOOL CPropertySection::Set( DWORD dwPropID, LPVOID pValue ) 
{
    // Since no dwType was specified, the property is assumed
    // to exist.   Fail if it does not.
    CProperty* pProp = GetProperty( dwPropID ) ;
    if (pProp != NULL && pProp->m_dwType)
    {
        pProp->Set( dwPropID, pValue, pProp->m_dwType ) ;
        return TRUE ;
    }
    else
        return FALSE ;
}

LPVOID CPropertySection::Get( DWORD dwPropID )
{   return Get( dwPropID, (DWORD*)NULL ) ;  }

LPVOID CPropertySection::Get( DWORD dwPropID, DWORD* pcb )
{
    CProperty* pProp = GetProperty( dwPropID ) ;
    if (pProp)
        return pProp->Get( pcb ) ;   
    else
        return NULL ;
}

void CPropertySection::Remove( DWORD dwID )
{
    POSITION pos ;
    POSITION posRemove = m_PropList.GetHeadPosition();
    CProperty*  pProp ;
    while( posRemove != NULL )
    {
        pProp = (CProperty*)m_PropList.GetNext( pos ) ;
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

void CPropertySection::RemoveAll( )
{                    
    POSITION pos = m_PropList.GetHeadPosition();
    while( pos != NULL )
        delete (CProperty*)m_PropList.GetNext( pos ) ;
    m_PropList.RemoveAll() ;
    m_SH.cProperties = 0 ;
}

 
CProperty* CPropertySection::GetProperty( DWORD dwPropID ) 
{
    POSITION pos = m_PropList.GetHeadPosition() ;
    CProperty* pProp ;
    while (pos != NULL)
    {
        pProp= (CProperty*)m_PropList.GetNext( pos ) ;
        if (pProp->m_dwPropID == dwPropID )
            return pProp ;
    }
    return NULL ;
}

void CPropertySection::AddProperty( CProperty* pProp ) 
{
    m_PropList.AddTail( pProp ) ;
    m_SH.cProperties++ ;
}

DWORD CPropertySection::GetSize( void ) 
{   return m_SH.cbSection ; }

DWORD CPropertySection::GetCount( void ) 
{   return m_PropList.GetCount() ;  }

CObList* CPropertySection::GetList( void ) 
{   return &m_PropList ;  }

BOOL CPropertySection::WriteToStream( IStream* pIStream )
{
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
    CProperty* pProp = NULL;
    POSITION pos = m_PropList.GetHeadPosition();
    while( pos != NULL )
    {
        // Get next element (note cast)
        pProp = (CProperty*)m_PropList.GetNext( pos );
                              
        // Write it 
        if (!pProp->WriteToStream( pIStream ))
        {
            pIStrPIDO->Release() ;
            return FALSE ;
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

BOOL CPropertySection::ReadFromStream( IStream* pIStream )
{   
    ULONG               cb ;         
    PROPERTYIDOFFSET    pido ;
    ULONG               cProperties ;
    LPSTREAM            pIStrPIDO ;      
    ULARGE_INTEGER      ulSectionStart ;
    LARGE_INTEGER       li ;
    CProperty*          pProp ;

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
        pProp = new CProperty( pido.propertyID, NULL, 0 ) ;
        pProp->ReadFromStream( pIStream ) ;
        AddProperty( pProp ) ;
    }                                   
    
    pIStrPIDO->Release() ;
    
    return TRUE ;
}

//------------------------------------------------------------------------
// Implementation of the CPropertySet class
//
CPropertySet::CPropertySet( void ) 
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

CPropertySet::CPropertySet( CLSID clsID ) 
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

CPropertySet::~CPropertySet()
{   RemoveAll() ;  }

BOOL CPropertySet::Set( CLSID FormatID, DWORD dwPropID, LPVOID pValue, DWORD dwType ) 
{
    CPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect == NULL)
    {
        if (pSect = new CPropertySection( FormatID ))
            AddSection( pSect ) ;
    }    
    pSect->Set( dwPropID, pValue, dwType ) ;
    return TRUE ;
}

BOOL CPropertySet::Set( CLSID FormatID, DWORD dwPropID, LPVOID pValue ) 
{
    // Since there is no dwType, we have to assume that the property
    // already exists.  If it doesn't, fail.
    CPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect != NULL)
        return pSect->Set( dwPropID, pValue ) ;
    else
        return FALSE ;
}
    
LPVOID CPropertySet::Get( CLSID FormatID, DWORD dwPropID, DWORD* pcb ) 
{
    CPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect)
        return pSect->Get( dwPropID, pcb ) ;
    else
        return NULL ;
}

LPVOID CPropertySet::Get( CLSID FormatID, DWORD dwPropID ) 
{   return Get( FormatID, dwPropID, (DWORD*)NULL ) ; }

void CPropertySet::Remove( CLSID FormatID, DWORD dwPropID ) 
{
    CPropertySection*  pSect = GetSection( FormatID );
    if (pSect)
        pSect->Remove( dwPropID ) ;
}

void CPropertySet::Remove( CLSID FormatID ) 
{            
    CPropertySection* pSect ;
    POSITION pos ;
    POSITION posRemove = m_SectionList.GetHeadPosition() ;        
    while( posRemove != NULL )
    {
        pSect = (CPropertySection*)m_SectionList.GetNext( pos ) ;
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

void CPropertySet::RemoveAll( ) 
{
    POSITION pos = m_SectionList.GetHeadPosition();
    while( pos != NULL )
    {
        delete (CPropertySection*)m_SectionList.GetNext( pos ) ;  
    }
    m_SectionList.RemoveAll() ;
    m_PH.cSections = 0 ;
}

CPropertySection* CPropertySet::GetSection( CLSID FormatID ) 
{
    POSITION pos = m_SectionList.GetHeadPosition() ;
    CPropertySection* pSect ;
    while (pos != NULL)
    {
        pSect = (CPropertySection*)m_SectionList.GetNext( pos ) ;
        if (IsEqualCLSID( pSect->m_FormatID, FormatID ))
            return pSect ;
    }
    return NULL ;
}

CPropertySection* CPropertySet::AddSection( CLSID FormatID ) 
{
    CPropertySection* pSect = new CPropertySection( FormatID )  ;
    if (pSect)
        AddSection( pSect ) ;
    return pSect ;
}

void CPropertySet::AddSection( CPropertySection* pSect ) 
{
    m_SectionList.AddTail( pSect ) ;
    m_PH.cSections++ ;
}

CProperty* CPropertySet::GetProperty( CLSID FormatID, DWORD dwPropID ) 
{
    CPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect)
        return pSect->GetProperty( dwPropID ) ;
    else
        return NULL ;
}

void CPropertySet::AddProperty( CLSID FormatID, CProperty* pProp ) 
{
    CPropertySection* pSect = GetSection( FormatID ) ;
    if (pSect)
        pSect->AddProperty( pProp ) ;
}

WORD CPropertySet::GetByteOrder( void ) 
{   return m_PH.wByteOrder ;  }
  
WORD CPropertySet::GetFormatVersion( void ) 
{   return m_PH.wFormat ;  }
  
void CPropertySet::SetFormatVersion( WORD wFmtVersion ) 
{   m_PH.wFormat = wFmtVersion ;  }

DWORD CPropertySet::GetOSVersion( void ) 
{   return m_PH.dwOSVer ;  }
  
void CPropertySet::SetOSVersion( DWORD dwOSVer ) 
{   m_PH.dwOSVer = dwOSVer ;  }

CLSID CPropertySet::GetClassID( void ) 
{   return m_PH.clsID;  }
  
void CPropertySet::SetClassID( CLSID clsID ) 
{   m_PH.clsID = clsID ;  }

DWORD CPropertySet::GetCount( void ) 
{   return m_SectionList.GetCount() ;  }

CObList* CPropertySet::GetList( void ) 
{   return &m_SectionList ;  }

  
BOOL CPropertySet::WriteToStream( IStream* pIStream )
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
    CPropertySection*   pSect = NULL;
    POSITION            pos = m_SectionList.GetHeadPosition();
    while( pos != NULL )
    {
        // Get next element (note cast)
        pSect = (CPropertySection*)m_SectionList.GetNext( pos );
                   
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

BOOL CPropertySet::ReadFromStream( IStream* pIStream )
{                
    ULONG               cb ;         
    FORMATIDOFFSET      fido ;
    ULONG               cSections ;
    LPSTREAM            pIStrFIDO ;      
    CPropertySection*   pSect ;
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
        pSect = new CPropertySection ;
        pSect->SetFormatID( fido.formatID ) ;
        pSect->ReadFromStream( pIStream ) ;
        m_SectionList.AddTail( pSect ) ;
    }                                   

    pIStrFIDO->Release() ;
    return TRUE ;
}

