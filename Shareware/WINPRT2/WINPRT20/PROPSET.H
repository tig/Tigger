// PropSet.h
//
// Definitions and function prototypes for classes that can 
// manipulate OLE 2.0 property sets.
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
// Implementation Notes:
//
//  See PROPSET.TXT for documentation on these classes.
#ifndef __PROPSET_H__
#define __PROPSET_H__

#include <compobj.h>

// "Helper" Structures
typedef struct tagSECTIONHEADER
{
    DWORD       cbSection ;
    DWORD       cProperties ;  // Number of props. 
} SECTIONHEADER, FAR *LPSECTIONHEADER ;

typedef struct tagPROPERTYIDOFFSET
{
    DWORD       propertyID;
    DWORD       dwOffset;
} PROPERTYIDOFFSET, FAR * LPPROPERTYIDOFFSET;

typedef struct tagPROPHEADER
{
    WORD        wByteOrder ;    // Always 0xFFFE
    WORD        wFormat ;       // Always 0
    DWORD       dwOSVer ;       // System version
    CLSID       clsID ;         // Application CLSID
    DWORD       cSections ;     // Number of sections (must be at least 1)
} PROPHEADER, FAR *LPPROPHEADER ;
    
typedef struct tagFORMATIDOFFSET
{
    GUID        formatID;
    DWORD       dwOffset;
} FORMATIDOFFSET, FAR * LPFORMATIDOFFSET;

// CProperty class definition
// 
class CProperty : public CObject
{
    friend class CPropertySet ;
    friend class CPropertySection ;
    
public:
// Construction
    CProperty( void ) ;
    CProperty( DWORD dwID, const LPVOID pValue, DWORD dwType ) ;
    
// Attributes
    BOOL    Set( DWORD dwID, const LPVOID pValue, DWORD dwType ) ;
    BOOL    Set( const LPVOID pValue, DWORD dwType ) ;
    BOOL    Set( const LPVOID pValue ) ;
    LPVOID  Get( DWORD* pcb ) ;     // Returns pointer to actual value
    LPVOID  Get( void ) ;           // Returns pointer to actual value
    DWORD   GetType( void ) ;       // Returns property type
    void    SetType( DWORD dwType ) ;
    DWORD   GetID( void ) ;         
    void    SetID( DWORD dwPropID ) ;   

    LPVOID  GetRawValue( void ) ;   // Returns pointer internal value (may
                                    // include size information)
// Operations                           
    BOOL    WriteToStream( IStream* pIStream ) ;
    BOOL    ReadFromStream( IStream* pIStream ) ;
             
private:         
    DWORD       m_dwPropID ;
    DWORD       m_dwType ;
    LPVOID      m_pValue ;

public:
    ~CProperty() ;
} ;   


// CPropertySection
//
// Property set 'sections'.
//
class CPropertySection : public CObject
{                                          
    friend class CPropertySet ;
    friend class CProperty ;
    
public:
// Construction
    CPropertySection( void ) ;
    CPropertySection( CLSID FormatID ) ;

// Attributes
    CLSID   GetFormatID( void ) ;
    void    SetFormatID( CLSID FormatID ) ;
    
    BOOL    Set( DWORD dwPropID, LPVOID pValue, DWORD dwType ) ;
    BOOL    Set( DWORD dwPropID, LPVOID pValue ) ;
    LPVOID  Get( DWORD dwPropID, DWORD* pcb ) ;
    LPVOID  Get( DWORD dwPropID ) ;
    void    Remove( DWORD dwPropID ) ;
    void    RemoveAll() ;
    
    CProperty* GetProperty( DWORD dwPropID ) ;
    void AddProperty( CProperty* pProp ) ;

    DWORD   GetSize( void ) ;
    DWORD   GetCount( void ) ;
    CObList* GetList( void ) ;

    BOOL    GetID( LPCSTR pszName, DWORD* pdwPropID ) ;
    BOOL    SetName( DWORD dwPropID, LPCSTR pszName ) ;

    BOOL    SetSectionName( LPCSTR pszName );
    LPCSTR  GetSectionName( void );
        
// Operations                           
    BOOL    WriteToStream( IStream* pIStream ) ;
    BOOL    ReadFromStream( IStream* pIStream ) ;
    BOOL    WriteNameDictToStream( IStream* pIStream ) ;
    BOOL    ReadNameDictFromStream( IStream* pIStream ) ;

private:
// Implementation    
    CLSID           m_FormatID ;
    SECTIONHEADER   m_SH ;
    // List of properties (CProperty)               
    CObList         m_PropList ;
    // Dictionary of property names
    CMapStringToPtr m_NameDict ;
    CString         m_strSectionName;
        
public:
    ~CPropertySection();    
} ;


// CPropertySet
//
// This class provides a C++ interface to property sets.
// 
class CPropertySet : public CObject
{       
    friend class CPropertySection ;
    friend class CProperty ;

public:
// Construction
    CPropertySet( void ) ;
    CPropertySet( CLSID clsID )  ;
    
// Attributes
    BOOL    Set( CLSID FormatID, DWORD dwPropID, LPVOID pValue, DWORD dwType ) ;
    BOOL    Set( CLSID FormatID, DWORD dwPropID, LPVOID pValue ) ;
    LPVOID  Get( CLSID FormatID, DWORD dwPropID, DWORD* pcb ) ;
    LPVOID  Get( CLSID FormatID, DWORD dwPropID ) ;
    void    Remove( CLSID FormatID, DWORD dwPropID ) ;
    void    Remove( CLSID FormatID ) ;
    void    RemoveAll( ) ;

    CProperty* GetProperty( CLSID FormatID, DWORD dwPropID ) ;
    void AddProperty( CLSID FormatID, CProperty* pProp ) ;
    CPropertySection* GetSection( CLSID FormatID ) ;
    CPropertySection* AddSection( CLSID FormatID ) ;
    void AddSection( CPropertySection* psect ) ;

    WORD    GetByteOrder( void ) ;
    WORD    GetFormatVersion( void ) ;
    void    SetFormatVersion( WORD wFmtVersion ) ;
    DWORD   GetOSVersion( void ) ;
    void    SetOSVersion( DWORD dwOSVer ) ;
    CLSID   GetClassID( void ) ;
    void    SetClassID( CLSID clsid ) ;
    DWORD   GetCount( void ) ;
    CObList* GetList( void ) ;

// Operations                           
    BOOL    WriteToStream( IStream* pIStream ) ;
    BOOL    ReadFromStream( IStream* pIStream ) ;

// Implementation
private:                  
    PROPHEADER      m_PH ;                                                    
    CObList         m_SectionList ;
    
public:
    ~CPropertySet();    
} ;

#endif // __PROPSET_H__