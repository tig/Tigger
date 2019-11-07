// INIedit 2.0 (C++ version)
//
//

#ifndef __INIEDIT_H__
#define __INIEDIT_H__

#include "filename.h"

// CProEntry
// An INI file entry (i.e. 'key=value').
//
class CProEntry : public CObject
{
public:
    // constructors
    void CProEntry()
    {   m_szKey = "" ; m_szValue = "" ; m_pSection = NULL ; }

    void CProEntry( const CProSection &rSection ) 
    {   m_szKey = "" ;
        m_szValue = "" ; 
        m_pSection = &rSection ;
    }

    // member access member functions
    void SetKey( const char *szKey )
    {   m_szKey = szKey ; }

    CString GetKey()
    {   return m_szKey ; }

    void SetValue( const char *szValue )
    {   m_szValue = szValue ; }

    CString GetValue( )
    {   return m_szValue ; }

    BOOL Write() ;

    BOOL Read() ;

    void Paint( const CDC &rDC, const CRect &rRect ) ;

private:
    CString     m_szKey ;
    CString     m_szValue ;
    CProSection *m_pSection ;
} ;

// CProSection
// Class definition for the Profile Section class.  This class
// contains an INI file 'section'
//
class CProSection : public CObject
{
public:
    

private:
    CString         m_Name ;        // Section name
    CProEntryList   m_EntryList ;

} ;

// CProSectionList
// List of ProSection objects
//
class CProSectionList : public CObList 
{
public:

private:
    BOOL            fSort ;         // Sort list of sections?
    CProSection     m_Section ;
} ;


// CProfile
// Class definition for Windows Initialization files
//
class CProfile : public CFile
{
public:
    // Constructors.  CFile has a no parameter 
    // constructor, we inherit this.  We also
    // create our own one parameter constructor
    // that takes a filename.
    CProfile( const char *pszFilename )
    {   ASSERT( pszFilename != NULL ) ;
        m_szName = pszFilename ;
        m_hFile = hFileNull ;
    }

    // member functions
    void SetFilename( const char *psz )
    {   ASSERT( psz != NULL ) ;
        m_szName = psz ;
    }

    
    BOOL Refresh() ; // re-read (if needed).  returns TRUE if someting had changed


private:
    CFileName       m_szFileName ;      // Fully qualified path

    CProSectionList m_SectionList ;

    // This string is built/modified during many of the
    // member functions.  It is used in drawing the status for
    // speed reasons.
    CString         m_szFileStatus ;    // size, date, time, rwahs
    


} ;



#endif // __INIEDIT_H__
