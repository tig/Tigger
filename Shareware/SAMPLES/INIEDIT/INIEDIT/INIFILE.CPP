// inifile.cpp : implementation of the CINIfile classes
//

#include "stdafx.h"
#include "stdlib.h"
#include "malloc.h"
#include "inifile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

LPSTR NEAR PASCAL FindEndOfLine( LPSTR lpLine, LPSTR lpEOF ) ;

/////////////////////////////////////////////////////////////////////
// class UKeyValue implementation
//
//
// Constructors
//
//UKeyValue::UKeyValue()
//{
//}

UKeyValue::UKeyValue( USection* pSection )
{
    m_pSection = pSection ;
}

// Destructor
UKeyValue::~UKeyValue() 
{
}       
 
CString UKeyValue::GetKey()                     
{
    return m_szKey ;
}                   

BOOL UKeyValue::SetKey( const CString& sz)
{ 
    // When you set the name of a Key 
    // you are renaming that key, unless the key
    // was not already there....in which case it
    // needs to be created.
    //           
    
    // Remove old key from the section
    if (m_szKey.GetLength())
    {
        WritePrivateProfileString( m_pSection->GetName(), m_szKey, NULL, 
                        m_pSection->GetINIfile()->GetFileStatus().m_szFullName ) ;
    
        // Write new section with new Key
        m_szKey = sz ;
        Write() ;
    }
    else
        m_szKey = sz ;
        
    return TRUE ;
}

CString UKeyValue::GetValue()
{
    return m_szValue ;
}

BOOL UKeyValue::SetValue( const CString& sz )
{
    // Any time the value gets set, we write to disk.
    //
    m_szValue = sz ;
    Write() ;
    return TRUE ;
}


BOOL UKeyValue::Update() 
{   
    LPSTR   lp = m_szValue.GetBuffer( m_szValue.GetLength()+1 ) ;
    if (!GetPrivateProfileString( m_pSection->GetName(), m_szKey, "", lp, 
                                  LOWORD(m_pSection->GetINIfile()->GetFileStatus().m_size), 
                                  m_pSection->GetINIfile()->GetFileStatus().m_szFullName ))        
    {      
        m_szValue.ReleaseBuffer() ;
        return FALSE ;
    }
                
    m_szValue.ReleaseBuffer() ;
    return TRUE ;
}

BOOL UKeyValue::Write()
{
    WritePrivateProfileString( m_pSection->GetName(), m_szKey, m_szValue, 
                    m_pSection->GetINIfile()->GetFileStatus().m_szFullName ) ;
    m_pSection->GetINIfile()->GetFileStatus() ;                    
    return TRUE ;                    
}

BOOL UKeyValue::IsDeleted()
{
    return m_fIsDeleted ;
}

BOOL UKeyValue::Delete()
{                        
    // This will now no longer show up in the list of sections
    WritePrivateProfileString( m_pSection->GetName(), m_szKey, NULL, 
                               m_pSection->GetINIfile()->GetFileStatus().m_szFullName ) ;
    m_pSection->GetINIfile()->GetFileStatus() ;                    

    return m_fIsDeleted = TRUE ;
}

USection* UKeyValue::GetSection()
{  
    return m_pSection ;
}

void UKeyValue::SetSection( USection* pSection ) 
{
    m_pSection = pSection ;
}    


/////////////////////////////////////////////////////////////////////
// class USection implementation
//
//
// Constructors
//
USection::USection()
{
}

USection::USection( UINIfile* pINIfile )
{
    m_pINIfile = pINIfile ;
}

// Destructor
USection::~USection() 
{
    // If we have any KeyValues in our list then
    // make sure they get freed too
    //
    POSITION pos = m_KeyValues.GetHeadPosition() ;
    while (pos != NULL)
    {
        delete m_KeyValues.GetNext( pos ) ;
    }
    m_KeyValues.RemoveAll() ;
}       
 
CString USection::GetName()                     
{
    return m_szName ;
}

BOOL USection::SetName( const CString& sz )
{ 
    // When you set the name of a section
    // you are renaming that section, unless the section
    // did not already have a name....in which case it
    // needs to be created.
    //           
    
    // Remove old section from the file
    if (m_szName.GetLength())
    {
        WritePrivateProfileString( m_szName, NULL, NULL, 
                               m_pINIfile->GetFileStatus().m_szFullName ) ;
    
        // Write new section with new name
        m_szName = sz ;
        Write() ;
    }
    else
        m_szName = sz ;                                            
        
    return TRUE ;
}

BOOL USection::Update() 
{
    // Only update if we need to
    //
    if (m_KeyValues.IsEmpty() || m_pINIfile->IsDiskFileChanged())
    {
        // Brute force:  Nuke contents of 
        // current key value list and then re-read.
        // Smarter way would be to read only changed
        // key/values...but how do you detect whether
        // just a key or value has changed?
        //
        POSITION pos = m_KeyValues.GetHeadPosition() ;
        while (pos != NULL)
        {
            delete m_KeyValues.GetNext( pos ) ;
        }
        
        m_KeyValues.RemoveAll() ;
        
        // Now read all KeyValues
        //      
        LPSTR lp ;             
        if (!(lp = (LPSTR)_fmalloc( max( LOWORD(m_pINIfile->GetFileStatus().m_size), 1024 ) )))
            return FALSE ;

        if (!GetPrivateProfileString( m_szName, NULL, "", lp, 
                                      LOWORD(m_pINIfile->GetFileStatus().m_size), 
                                      m_pINIfile->GetFileStatus().m_szFullName ))        
        {
            _ffree( lp ) ;
            return FALSE ;
        }
        
        LPSTR lpszKey ;

        for (lpszKey = lp ; *lpszKey != '\0' ; lpszKey += lstrlen( lpszKey ) + 1)
        {
            UKeyValue* pKeyValue = new UKeyValue( this ) ;
            
            pKeyValue->SetKey( lpszKey ) ;
            pKeyValue->Update() ;
            
            m_KeyValues.AddTail( pKeyValue ) ;
        }
    }
    
    if (m_KeyValues.IsEmpty())
        return FALSE ;
                
    return TRUE ;
}

BOOL USection::Write()
{
    WritePrivateProfileString( m_szName, "", "", 
                               m_pINIfile->GetFileStatus().m_szFullName ) ;
    m_pINIfile->GetFileStatus() ;                    

    POSITION pos = m_KeyValues.GetHeadPosition() ;
    while (pos != NULL)
    {
        ((UKeyValue*)m_KeyValues.GetNext( pos ))->Write() ;
    }

    return TRUE ;                          
}

UKeyValueList* USection::GetKeyValueList()
{
    return &m_KeyValues ;
}

BOOL USection::IsDeleted()
{
    return m_fIsDeleted ;
}

BOOL USection::Delete()
{                        
    // This will now no longer show up in the list of sections
    WritePrivateProfileString( m_szName, NULL, NULL, 
                               m_pINIfile->GetFileStatus().m_szFullName ) ;
    m_pINIfile->GetFileStatus() ;                    

    POSITION pos = m_KeyValues.GetHeadPosition() ;
    while (pos != NULL)
    {
        delete m_KeyValues.GetNext( pos ) ;
    }
    m_KeyValues.RemoveAll() ;

    return m_fIsDeleted = TRUE ;
}

UINIfile* USection::GetINIfile()
{  
    return m_pINIfile ;
}

void USection::SetINIfile( UINIfile* pINIfile ) 
{
    m_pINIfile = pINIfile ;
}    
                     

/////////////////////////////////////////////////////////////////////
// class UINIfile implementation
//
//
// Constructors
//
UINIfile::UINIfile()
{
    ::UINIfile( "" ) ;
}

UINIfile::UINIfile( const CString& szName )
{
    if(szName.GetLength())
        CFile::GetStatus( szName, m_Status ) ;
}

// Destructor
UINIfile::~UINIfile() 
{
}       

CString UINIfile::GetName()
{
    return m_Status.m_szFullName ;
}

// Get the time stamp of a file
//
CFileStatus UINIfile::GetFileStatus()
{
    if(lstrlen(m_Status.m_szFullName) && CFile::GetStatus( m_Status.m_szFullName, m_Status )) 
    {
        // 
    }
    else
    {
        m_Status.m_size = 0 ;
        m_Status.m_mtime = 0 ;
    }
    return m_Status ;
}

BOOL UINIfile::IsDiskFileChanged()
{
    CFileStatus status ;
    
    CFile::GetStatus( m_Status.m_szFullName, status ) ;
    if (m_Status.m_mtime != status.m_mtime)
        return TRUE ;
        
    // I doubt that the size will ever be different without the
    // time also being different, so this could possibly be
    // taken out...
    //
    if (m_Status.m_size != status.m_size)
        return TRUE ;
        
    return FALSE ;    
}          

USectionList* UINIfile::GetSectionList()
{
    return &m_Sections ;
}

CString UINIfile::GetOutputString()
{
    return m_szOutput ;
}

void UINIfile::SetOutputString( const CString &sz )
{
    m_szOutput = sz ;
}     
                     
BOOL UINIfile::Update()
{   
    // Only update if we need to
    //
    if (m_Sections.IsEmpty() || IsDiskFileChanged())
    {
        // Brute force:  Nuke contents of 
        // current section list and then re-read.
        // Smarter way would be to read only changed
        // sections...but how do you detect whether
        // just a section has changed?
        //
        POSITION pos = m_Sections.GetHeadPosition() ;
        while (pos != NULL)
        {
            delete m_Sections.GetNext( pos ) ;
        }
        
        m_Sections.RemoveAll() ;
        
        // Now read all sections
        //                   
        // Open the file.
        //
        CFile file ;
        CFileException e ;
        
        if (!file.Open( m_Status.m_szFullName, 
                    CFile::modeRead | CFile::shareDenyWrite, &e ))
        {
            // Bummer!
            // File could not be opened.
            //
            #ifdef _DEBUG
                afxDump << m_Status.m_szFullName << " could not be opened!\n" ;
            #endif
            
            return FALSE ;
        }           
        
        // Get the latest status
        //
        file.GetStatus( m_Status ) ;
        
        // Allocate a buffer large enough to hold all the section
        // strings.  Just use the file size.
        //
        LPSTR lpFileBuf ;                                 
        
        if (!(lpFileBuf = (LPSTR)_fmalloc( max( LOWORD(m_Status.m_size), 1024 ) )))
        {
            file.Close() ;
            return FALSE ;
        }
            
        // Read the file.
        //  
        UINT uiBytes ;
        if (3 > (uiBytes = file.Read( lpFileBuf, LOWORD(m_Status.m_size) )))
        {
            // Looks like an empty file...reject it
            //
            file.Close() ;
            _ffree( lpFileBuf ) ;
            return FALSE ;
        }
         
        // Go through file with lpLine always pointing to the
        // beginning of a line and lpEOL always pointing to the
        // last character of a line.
        //
        // lpCur is our current pointer within the line.
        //
        LPSTR   lpLine = lpFileBuf ;        
        LPSTR   lpEOF = lpFileBuf + uiBytes ;
        LPSTR   lpEOL, lpCur ;
        CString szTemp ;
        BOOL    fInSec ;
    
        while (lpEOL = FindEndOfLine( lpLine, lpEOF ))
        {
            // Scan the current line.  If we find a ';' the line is
            // a comment.  If we find a '[' then we are beginning
            // a section.
            //
            for (lpCur = lpLine, fInSec = FALSE, szTemp = "" ; 
                 lpCur < lpEOL ; 
                 lpCur++)
            {
                if (fInSec)
                {
                    if (*lpCur == ']')
                    {
                        // We have found one!
                        //
                        TRY
                        {
                            USection* pSec = new USection( this ) ;
                            
                            pSec->SetName( szTemp ) ;
                            
                            m_Sections.AddTail( pSec ) ;
                            break ;
                        }
                        CATCH( CMemoryException, theException )
                        {
                            #ifdef _DEBUG
                                afxDump << "MemoryException in UpdateSectionList\n" ;
                            #endif
                            
                            file.Close() ;
                            _ffree( lpFileBuf ) ;
                            return FALSE ;
                        }
                        END_CATCH
                    }
                                                 
                    szTemp += *lpCur ;                                             
                }
                else
                {
                    if (*lpCur == '[')
                        fInSec = TRUE ;
                        
                    if (*lpCur == ';')
                        break ;
                        
                    if (*lpCur == '=')
                        break ;
                }
            }
    
            lpLine = lpEOL ;
        }
            
        _ffree( lpFileBuf ) ;
        file.Close() ;
    }
    
    if (m_Sections.IsEmpty())
        return FALSE ;
        
    return TRUE ;
}


/****************************************************************
 *  LPSTR NEAR PASCAL FindEndOfLine( LPSTR lpLine, LPSTR lpEOF )
 *
 *  Description: 
 *
 *    This function scans the line lpLine returning a pointer to
 *    the last character on the line.
 *
 *    It returns NULL when lpLine == lpEOF.
 *
 *  Comments:
 *
 *
 ****************************************************************/
LPSTR NEAR PASCAL FindEndOfLine( LPSTR lpLine, LPSTR lpEOF )
{
   register char  c ;

   while (lpLine <= lpEOF)
   {
      c = *lpLine ;

      if (c == '\n' || c == '\r' || c == '\f' || c == '\x1A')
      {
         /*
          * Scan until you get to a non '\n', '\r', '\f', or '\x1A'
          * character and return that pointer.
          */
         while( lpLine <= lpEOF )
         {
            if (c == '\n' || c == '\r' || c == '\f' || c == '\x1A')
               lpLine++ ;
            else
               return lpLine ;

            c = *lpLine ;
             
         }

       }
      lpLine++ ;
   }

   if (lpLine >= lpEOF)
      return NULL ;

   return lpLine ;

} /* FindEndOfLine()  */
