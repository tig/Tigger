// inifile.h : header file for INI file objects
//          
// Classes defined here:
//
//      UINIfile    : FILENAME.INI contains the filename and a list 
//                      of sections plus some house keeping info.
//      USection    : [Section] contains the section name and a list 
//                      of KeyValues plus some house keeping info.
//      UKeyValue   : Key=Value contains the key and value plus
//                      some house keeping info.
//
//  There are also 'list classes' for each of these (UINIfileList etc..)
//                

// Forward declaration of USection so UKeyValue can
// have a pointer to it's section.
//                
class USection ;

/////////////////////////////////////////////////////////////////////
// UKeyValue interface
//
// The UKeyValue class is the lowest member in the INI file 
// hierarchy.  It consists of a Key (i.e. "Foo=") and a 
// value (i.e. "Bar"...where "Key=Bar").
//
// Set operations on a KeyValue object (SetKeyValue) will
// result in the Key=Value being phsically written to
// the section identified by m_pSection.
//        
class UKeyValue : public CObject
{  
// Attributes
public:
    CString GetKey() ;
    BOOL SetKey( const CString& sz ) ;

    CString GetValue() ;
    BOOL SetValue( const CString& sz ) ;
    
    BOOL IsDeleted() ;
    
protected:    
    USection* GetSection() ;
    void SetSection( USection* pSection ) ;
    
// Operations
public:        
    BOOL Update() ;
    BOOL Write() ;
    BOOL Delete() ;                     // Delete this key.  

// Implementation
    UKeyValue() ;
    UKeyValue( USection* pSection ) ; 
    ~UKeyValue() ;

private:
    CString     m_szKey ;           // Key=
    CString     m_szValue ;         // Value
    UINT        m_fQuoted : 1 ;     // Are the supposed to be quotes
                                    // around the value in the file?
    UINT        m_fIsDeleted : 1 ;  // TRUE if this keyvalue has
                                    // been deleted.                                        
    USection*   m_pSection ;        // Pointer back to section
} ;
            
                 
class UKeyValueList : public CObList 
{  
} ;

// forward declaration of UINIfile for USection which needs a 
// pointer back to it's file.
//
class UINIfile ;  

/////////////////////////////////////////////////////////////////////
// USection interface
//
// An USection contains a list of UKeyValue objects.
//     
class USection : public CObject
{  
// Attributes
public:
    CString GetName() ;
    BOOL SetName( const CString& sz ) ;

    UKeyValueList* GetKeyValueList() ;

    BOOL IsDeleted() ;

//protected:    
    UINIfile* GetINIfile() ;
    void SetINIfile( UINIfile* pINIfile ) ;
    
// Operations
public:
    BOOL    Update() ;
    BOOL    Write() ;
    BOOL    Delete() ;

// Implementation
    USection() ;
    USection( UINIfile* pINIfile ) ; 
    ~USection() ;

private:
    CString         m_szName ;          // Name of the section
    UKeyValueList   m_KeyValues ;       // list of key values
    BOOL            m_fIsDeleted : 1 ;  // TRUE if this section has been del'd
    
    UINIfile*       m_pINIfile ;

} ;
   
class USectionList : public CObList
{  
} ;
     
/////////////////////////////////////////////////////////////////////
// UINIfile interface
//
// An UINIfile contains a list of sections.
//     
class UINIfile : public CObject
{  

// Attributes
public:
    CFileStatus GetFileStatus() ;
    BOOL    IsDiskFileChanged() ;
    
    USectionList* GetSectionList() ;

    CString GetOutputString() ;
    void SetOutputString( const CString& szOutput ) ;
        
// Operations
public:
	BOOL Update() ;		// read section list from disk

// Implementation
    UINIfile() ;
    UINIfile( const CString &szName ) ; 
    ~UINIfile() ;

private:
    CFileStatus     m_Status ;
    CString         m_szOutput ;      // temp output buffer to speed drawing

public:    
    USectionList    m_Sections ;
} ;

class UINIfileList : public CObList
{  
} ;
