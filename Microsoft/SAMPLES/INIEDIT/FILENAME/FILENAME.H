// CFileName class
//
// CFileName provides
// the ability to manipulate the distinct parts of a filename
// such as drive letter, path, name, extention, etc... It supports
// UNC filenames.
//
#ifndef __FILENAME_H__
#define __FILENAME_H__


class CFilename : public CString
{
public:
    CString GetDrive( ) ;
    CString GetDir( ) ;
    CString GetPath( ) ;
    CString GetFname( ) ;       // Get the filename part (i.e. 'foobar')
    CString GetExt( ) ;
    CString GetFilename( ) ;    // Get the filename and extension (i.e. 'foobar.ext')

} ;




#if XXX
class CFilename : public CObject
{
public:
    CFilename()
    {   m_szDrive = "" ; m_szDir = "" ; m_szFname = "" ; m_szExt = "" ; }

    CFilename( const char *sz )
    {   CFilename::Set( sz ) ; } 

    CString Get( ) ;
    CString GetDrive( ) ;
    CString GetDir( ) ;
    CString GetPath( ) ;
    CString GetFname( ) ;       // Get the filename part (i.e. 'foobar')
    CString GetExt( ) ;
    CString GetFilename( ) ;    // Get the filename and extension (i.e. 'foobar.ext')

    BOOL Set( const char *sz ) ;
    BOOL SetDrive( const char *szDrive ) ;
    BOOL SetDir( const char *szDir ) ;
    BOOL SetPath( const char *szPath ) ;
    BOOL SetFname( const char *szFname ) ;
    BOOL SetExt( const char *szExt ) ;
    BOOL SetFilename( const char *szFilename ) ;

private:
    CString     m_szDrive ;         // Could be '', 'C:'or '\\SEVER\SHARE'
    CString     m_szDir ;           // Could be '', '\', 'AB\XY\', or '\AB\XY\'
    CString     m_szFname ;         // Could be '', 'ABC'
    CString     m_szExt ;           // Could be '', '.', 'EXT', or '.EXT'
} ;
#endif

#endif // __FILENAME_H__
