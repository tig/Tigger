// CFilename class implementation
//
#include <afx.h>
#include "filename.h"

CString Drive( const char *sz ) ;
CString Dir( const char *sz ) ;
CString Fname( const char *sz ) ;
CString Ext( const char *sz ) ;
CString Filename( const char *sz ) ;


// CFilename::GetDrive
// Returns a string containing the drive letter and colon,
// or a UNC path indicating the server and share.  Returns
// an empty string if drive can't be found in string.
//
CString CFilename::GetDrive( ) 
{
    ASSERT_VALID( this ) ;

    CString s = this ;

    return Drive( s ) ;
}

CString CFilename::GetDir( )  
{
    ASSERT_VALID( this ) ;

    return Dir( this ) ;
}


CString CFilename::GetPath( ) 
{
    ASSERT_VALID( this ) ;

    return Drive( this ) + Dir( this ) ;
}

// CFilename::GetFname
// Get the filename part (i.e. 'foobar')
//
CString CFilename::GetFname( ) 
{
    ASSERT_VALID( this ) ;

    return Fname( this ) ;
}

CString CFilename::GetExt( )  
{
    ASSERT_VALID( this ) ;

    return Ext( this ) ;
}

// Get the filename and extension (i.e. 'foobar.ext')
CString CFilename::GetFilename( ) 
{
    ASSERT_VALID( this ) ;
    CString s = Ext( this ) ;
    
    if (s.GetAt(0) != '.')
        return Fname() + "." + s ;
    else
        return Fname() + s ;
}


// Helper functions
//
// Drive
// Gets the drive component of a string
//
CString Drive( const char *sz )
{
    CString     s = sz ;
    int         n, m ;
    
    // In order for the drive to be valid, the entire filename must
    // be at least 2 characters long (i.e. for the letter and colon)
    if (s.GetLength() < 2)
        return "" ;

    // The drive is either the first character, if the
    // second character is a colon, or the server\share
    // name if the first two characters are '\'
    if (s.GetAt(1) == ':')
    {
        return s.Left(2) ;
    }

    // Is it a UNC path?
    //
    // If it is, we return a string containing it.  Note that
    // the UNC path does NOT include the trailing '\'
    //
    if (s.GetAt(0) == '\\' && s.GetAt(1) == '\\')
    {
        // Need to move on to second '\' after
        // the two initial '\'s.
        //
        // Find the first '\'
        CString     szTemp = s.Right( s.GetLength() - 2 ) ;
        if (-1 == (n = szTemp.Find( '\\' )))
            return "" ;
        
        if (-1 == (m = szTemp.Right(szTemp.GetLength() - (n+1)).Find( '\\' )))
            return "" ;

        // so the position in this of the '\' is
        // 2+n+m +1
        return s.Left( 2+n+m+1 ) ;
    }
    else
        return "" ;
           
}

// Dir
// Get the dir component
CString Dir( const char *sz )
{
    CString     s = sz ;
    CString     d = Drive( sz ) ;
    CString     f = Filename( sz ) ;

    // Remove the drive part
    //
    s = s.Right( s.GetLength( ) - d.GetLength( ) ) ;

    // Remove the filename part
    //
    s = s.Left( s.GetLength( ) - f.GetLength( ) ) ;

    // Scan backwards for the last '\' or ':'
    int n = s.ReverseFind('\\') ;
    
    if (n == -1)
        n = s.ReverseFind( ':' ) ;

    if (n == -1)
        return "" ;

    return s.Left( n + 1 ) ;
}

CString Fname( const char *sz )
{
    CString s = sz ;
    int n ;

    n = s.ReverseFind( '\\' ) ;
    if (n != -1)
    {
        // Just get everything past here
        s = s.Right( s.GetLength( ) - (n+1) ) ;
    }
    else
    {
        // There's no dir specified, so check for a drive
        //
        if (-1 != (n = s.ReverseFind( ':' )))
        {
            s = s.Right( s.GetLength( ) - (n+1) ) ;
        }
    }
                
    // Is there an extension?
    n = s.Find( '.' ) ;
    if (n == -1)
        return s ;
    else
        return s.Left( s.GetLength( ) - n ) ;

}

CString Ext( const char *sz )
{
    CString s = sz ;
    int n ;

    n = s.ReverseFind( '\\' ) ;
    if (n != -1)
    {
        // Just get everything past here
        s = s.Right( s.GetLength( ) - (n+1) ) ;
    }
    else
    {
        // There's no dir specified, so check for a drive
        //
        if (-1 != (n = s.ReverseFind( ':' )))
        {
            s = s.Right( s.GetLength( ) - (n+1) ) ;
        }
    }
                
    // Is there an extension?
    n = s.Find( '.' ) ;
    if (n == -1)
        return "" ;
    else
        return s.Right( s.GetLength( ) - n ) ;

}


CString Filename( const char *sz )
{
    CString s = sz ;
    int n ;

    n = s.ReverseFind( '\\' ) ;
    if (n != -1)
    {
        // Just get everything past here
        s = s.Right( s.GetLength( ) - (n+1) ) ;
    }
    else
    {
        // There's no dir specified, so check for a drive
        //
        if (-1 != (n = s.ReverseFind( ':' )))
        {
            s = s.Right( s.GetLength( ) - (n+1) ) ;
        }
    }

    return s ;                

}

#ifdef XXX
CString CFilename::Get( )
{
    ASSERT_VALID( this ) ;
    return GetPath() + GetFilename() ;
}

BOOL CFilename::Set( const char *szPath )
{
    ASSERT_VALID( this ) ;

    CString     s = szPath ;

    m_szDrive = Drive( szPath ) ;
    m_szDir   = Dir( szPath ) ;    
    m_szFname = Fname( szPath ) ;
    m_szExt   = Ext( szPath ) ;

    return TRUE ;
    
}

// CFilename::GetDrive
// Returns a string containing the drive letter and colon,
// or a UNC path indicating the server and share.  Returns
// an empty string if drive can't be found in string.
//
CString CFilename::GetDrive( ) 
{
    ASSERT_VALID( this ) ;

    return m_szDrive ;
}

CString CFilename::GetDir( )  
{
    ASSERT_VALID( this ) ;
    return m_szDir ;
}


CString CFilename::GetPath( ) 
{
    ASSERT_VALID( this ) ;

    return m_szDrive + m_szDir ;
}

BOOL CFilename::SetPath( const char *szPath )
{
    ASSERT_VALID( this ) ;

    CString     s = szPath ;

    m_szDrive = Drive( szPath ) ;
    m_szDir = Dir( szPath ) ;    

    return TRUE ;
}

// CFilename::GetFname
// Get the filename part (i.e. 'foobar')
//
CString CFilename::GetFname( ) 
{
    ASSERT_VALID( this ) ;

    return m_szFname ;
}

CString CFilename::GetExt( )  
{
    ASSERT_VALID( this ) ;

    return m_szExt ;
}

// Get the filename and extension (i.e. 'foobar.ext')
CString CFilename::GetFilename( ) 
{
    ASSERT_VALID( this ) ;
    
    if (m_szExt.GetAt(0) != '.')
        return m_szFname + "." + m_szExt ;
    else
        return m_szFname + m_szExt ;
}


// CFilename::SetDrive
// Adds or changes the drive.  The drive can be set to a UNC path.
// 
BOOL CFilename::SetDrive( const char *szDrive )
{
    ASSERT_VALID( this ) ;

    m_szDrive = szDrive ;
    return TRUE ;
}


BOOL CFilename::SetDir( const char *szDir )  
{
    ASSERT_VALID( this ) ;

    m_szDir = szDir ;

    if (m_szDir.GetAt( m_szDir.GetLength() - 1 ) != '\\')
        m_szDir = m_szDir + "\\" ;

    return TRUE ;
}


BOOL CFilename::SetFilename( const char *szFilename ) 
{
    CString s = szFilename ;

    if (s.IsEmpty())
        return TRUE ;

    // Now search forward 
    int n = s.Find( '.' ) ;
    if ( n == -1)
    {
        m_szFname = s ;
        m_szExt = "." ;
    }
    else
    {
        // Strip off the extension
        m_szFname = s.Left( n + 1 ) ;
        m_szExt = s.Right( s.GetLength() - n ) ;
    }

    return TRUE ;
}


// CFilename::SetFname
// Set's the filename part (i.e. 'foobar')
//
BOOL CFilename::SetFname( const char *szFname )
{
    ASSERT_VALID( this ) ;

    m_szFname = szFname ;
    return TRUE ;
}   

BOOL CFilename::SetExt( const char *szExt )  
{
    ASSERT_VALID( this ) ;

    m_szExt = szExt ;
    return TRUE ;  
}

#endif