#include "person.h"
#include <string.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Call 'IMPLEMENT_SERIAL' macro for all the classes declared in
// person.h

IMPLEMENT_SERIAL( CPerson, CObject, 0 )
IMPLEMENT_SERIAL( CPersonList, CObList, 0 )


// CPerson::CPerson
// Copy constructor for CPerson class
//
CPerson::CPerson( const CPerson &a )
{
    ASSERT_VALID( this ) ;
    ASSERT_VALID( &a ) ;

    m_LastName = a.m_LastName ;
    m_FirstName = a.m_FirstName ;
    m_PhoneNumber = a.m_PhoneNumber ;
    m_modTime = a.m_modTime ;
}

// CPerson::CPerson
// Memberwise Constructor for CPerson Class
//
CPerson::CPerson( const char * pszLastName, const char *pszFirstName, const char *pszPhoneNum )
{
    ASSERT_VALID( this ) ;
    m_LastName = pszLastName ;
    m_FirstName = pszFirstName ;
    m_PhoneNumber = pszPhoneNum ;
    m_modTime = CTime::GetCurrentTime() ;
}

// CPerson::operator=
// OVerloaded operator= to perform assignments
CPerson& CPerson::operator=( const CPerson &b )
{
    ASSERT_VALID( this ) ;
    ASSERT_VALID( &b ) ;
    
    m_LastName = b.m_LastName ;
    m_FirstName = b.m_FirstName ;
    m_PhoneNumber = b.m_PhoneNumber ;
    m_modTime = b.m_modTime ;

    return *this ;
}        

// CPerson::Dump
// Write the contents of the object to a diagnostic context
//
//
#ifdef _DEBUG

void CPerson::Dump( CDumpContext &dc ) const
{
    ASSERT_VALID( this ) ;
    // Call base class function first
    //
    CObject::Dump( dc ) ;

    // Now dump the data
    dc << "\n"
    << "Last Name:  " << m_LastName << "\n"
    << "First Name: " << m_FirstName << "\n"
    << "Phone #:    " << m_PhoneNumber << "\n"  
    << "Mod date:   " << m_modTime << "\n" ;
}

void CPerson::AssertValid() const
{
    CObject::AssertValid() ;
}

#endif 


// CPerson:: Serialize
// Read or write the contents of the object to an archive
//
void CPerson::Serialize( CArchive &archive )
{
    ASSERT_VALID( this ) ;

    //call base class function frist
    CObject::Serialize( archive ) ;

    //Now dump data for this class
    if (archive.IsStoring())
    {
        TRACE( "Serializing a CPerson out.\n" ) ;
        archive << m_LastName << m_FirstName
                << m_PhoneNumber << m_modTime ;
    }
    else
    {
        TRACE( "Serializing a CPerson in.\n" ) ;
        archive >> m_LastName >> m_FirstName 
                >> m_PhoneNumber >> m_modTime ;
    }
}


// CPersonList::FindPerson
// 
CPersonList* CPersonList::FindPerson( const char * szTarget )
{
    ASSERT_VALID( this ) ;

    CPersonList *pNewList = new CPersonList ;
    CPerson     *pNext = NULL ;

    // Start at front
    POSITION    pos = GetHeadPosition() ;

    // Iterate over whole list
    while (pos != NULL)
    {
        // Get next element (note cast)
        pNext = (CPerson*)GetNext( pos ) ;

        // add current element to new list if it matches
        if (_strnicmp( pNext->GetLastName(), szTarget, strlen( szTarget ) ) == 0)
            pNewList->AddTail( pNext ) ;
    }

    if (pNewList->IsEmpty())
    {
        delete pNewList ;
        pNewList = NULL ;
    }

    return pNewList ;
}

// CPersonList::DeleteAll
// 
void CPersonList::DeleteAll()
{
    ASSERT_VALID( this ) ;

    POSITION    pos = GetHeadPosition() ;

    while (pos != NULL)
    {
        delete GetNext( pos ) ;
    }
    
    RemoveAll() ;
}
