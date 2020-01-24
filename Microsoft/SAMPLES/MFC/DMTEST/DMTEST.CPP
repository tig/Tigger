#include "person.h"

// FUnction prototypes
CPersonList *MakeDataBase() ;
CFile *OpenForReading( const CString &rFileName ) ;
CFile *OpenForWriting( const CString &rFileName ) ;
CPersonList *ReadDataBase( CFile *pFile ) ;
BOOL WriteDataBase( CFile *pFile, CPersonList *pDataBase ) ;
void TestFindPerson( CPersonList *pDataBase ) ;
void ListDataBase( CPersonList *db ) ;

void main()
{
    const char szFileName[] = "tutorial.dat" ;

#ifdef _DEBUG
    // Prepare for dispaly...
    const int nDumpChildren = 1 ;
    afxDump.SetDepth( nDumpChildren ) ;
#endif

    printf( "Create a person list and fill it with persons\n" ) ;
    CPersonList *pDataBase = MakeDataBase() ;

    printf( "Serialize the person list\n" ) ;
    CFile *pFile ;

    TRY
    {
        // Could throw a file exception if can't open file
        pFile = OpenForWriting( szFileName ) ;

        // Could throw an archive exception if can't create
        WriteDataBase( pFile, pDataBase ) ;
    }
    CATCH( CFileException, theException )
    {
        printf( "Unable to open file for writing\n" ) ;
        exit( -1 ) ;
    }
    AND_CATCH( CArchiveException, theException )
    {
        printf ("Unable to save the database\n" ) ;
        pFile->Close() ;
        delete pFile ;
        exit( -1 ) ;
    }
    END_CATCH

    // No exceptions, so close up
    pFile->Close() ;
    delete pFile ;

    ListDataBase( pDataBase ) ;

    printf( "Delete the list and all it's elements\n" ) ;
    pDataBase->DeleteAll() ;
    ListDataBase( pDataBase ) ;
    delete pDataBase ;

    printf( "Deserialize the data from disk into a new list\n" ) ;
    CPersonList *pDataBase2 ; 

    TRY
    {
        pFile = OpenForReading( szFileName ) ;

        pDataBase2 = ReadDataBase( pFile ) ;
    }
    CATCH( CFileException, theException )
    {
        printf( "Unable to open file for reading database\n" ) ;
    }
    AND_CATCH( CArchiveException, theException )
    {
        printf( "Unable to read the database\n" ) ;
        pFile->Close() ;
        delete pFile ;
        exit (-1) ;
    }
    END_CATCH

    pFile->Close() ;
    delete pFile ;

    ListDataBase( pDataBase2 ) ;

    printf( "Test the FindPerson functio\n" ) ;
    if (pDataBase2 != NULL)
        TestFindPerson( pDataBase2 ) ;

    printf( "Delete the list and all its elements\n" ) ;
    pDataBase2->DeleteAll() ;
    delete pDataBase ;

    TRACE( "End of program\n" ) ;
}

// MakeDataBase - Create a database and add some persons
//
CPersonList* MakeDataBase()
{
    TRACE( "  Make a new person list on the heap\n" );
    CPersonList* pDataBase = new CPersonList;

    TRACE( "  Add several new persons to the list\n" );
    CPerson* pNewPerson1 = new CPerson( "Smith", "Mary", "435-8159" );
    pDataBase->AddHead( pNewPerson1 );

    CPerson* pNewPerson2 = new CPerson( "Smith", "Al", "435-4505" );
    pDataBase->AddHead( pNewPerson2 );

    CPerson* pNewPerson3 = new CPerson( "Jones", "Steve", "344-9865" );
    pDataBase->AddHead( pNewPerson3 );

    CPerson* pNewPerson4 = new CPerson( "Hart", "Mary", "287-0987" );
    pDataBase->AddHead( pNewPerson4 );

    CPerson* pNewPerson5 = new CPerson( "Meyers", "Brian", "236-1234" );
    pDataBase->AddHead( pNewPerson5 );

    TRACE( "  Return the completed database to main\n" );
    return pDataBase;
}

CFile *OpenForReading( const CString &rFileName ) 
{
    CFile *pFile = new CFile ;

    CFileException *theException = new CFileException ;
    if (!pFile->Open( rFileName, CFile::modeRead, theException ))
    {
        delete pFile ;
        TRACE( "  Threw file exception in OpenForReading\n" ) ;
        THROW( theException ) ;
    }

    return pFile ;
}

CFile *OpenForWriting( const CString &rFileName ) 
{
    CFile *pFile = new CFile ;
    CFileStatus status ;
    UINT nAccess = CFile::modeWrite ;

    // Get Status will return TRUE if file exists
    if (!CFile::GetStatus( rFileName, status ))
        nAccess |= CFile::modeCreate ;


    CFileException *theException = new CFileException ;
    if (!pFile->Open( rFileName, nAccess, theException ))
    {
        delete pFile ;
        TRACE( "  Threw file exception in OpenForWriting\n" ) ;
        THROW( theException ) ;
    }

    TRACE( "  Opened file for writing OK\n" ) ;
    return pFile ;
}

 
// ReadDataBase
CPersonList *ReadDataBase( CFile *pFile )
{
    CPersonList *pNewDataBase = NULL ;
    
    CArchive archive( pFile, CArchive::load ) ;
    
    TRY
    {
       archive >> pNewDataBase ;
    }
    CATCH( CArchiveException, theException )
    {       
        TRACE( "  Caught an archive exception in ReadDataBase\n" ) ;
        #ifdef _DEBUG
        theException->Dump( afxDump ) ;
        #endif

        archive.Close() ;

        if (pNewDataBase != NULL)
        {
            pNewDataBase->DeleteAll() ;
            delete pNewDataBase ;
        }
        THROW_LAST() ;
    }
    END_CATCH

    archive.Close() ;
    return pNewDataBase ;
}

BOOL WriteDataBase( CFile *pFile, CPersonList *pDB ) 
{
    CArchive archive( pFile, CArchive::store ) ;

    TRY
    {
        archive << pDB ;
    }
    CATCH( CArchiveException, theException )
    {
        TRACE( "  Caught an archive exception in WriteDataBase\n" ) ;
        #ifdef _DEBUG
        theException->Dump( afxDump ) ;
        #endif

        archive.Close() ;
        THROW_LAST() ;
    }
    END_CATCH

    archive.Close() ;
    return TRUE ;
}

void TestFindPerson( CPersonList *pDB ) 
{
    printf( "  Looking for the name Banipuli\n" ) ;
    CPersonList *pFound = pDB->FindPerson( "Banipuli" ) ;
    if (pFound->IsEmpty())
        printf( "  No matching persons\n" ) ;
    else
    {
        printf( "  Found matching person(s)\n" ) ;
        #ifdef _DEBUG
        pFound->Dump( afxDump ) ;
        #endif

    }

    delete pFound ;

    printf( "  Look for the name Smith\n" ) ;
    pFound = pDB->FindPerson( "Smith" ) ;
    if (pFound->IsEmpty())
        printf( "  No matching persons\n" ) ;
    else
    {
        printf( "  Found matching person(s)\n" ) ;
        #ifdef _DEBUG
        pFound->Dump( afxDump ) ;
        #endif

    }

    delete pFound ;
}

void ListDataBase( CPersonList *db ) 
{
    CPerson *pCurrent ;
    POSITION pos ;

    if (db->GetCount() == 0)
        printf ("  List is empty" ) ;
    else
    {
        printf( "  List contains:\n" ) ;
        pos = db->GetHeadPosition() ;
        while (pos != NULL )
        {
            pCurrent = (CPerson*)db->GetNext( pos ) ;
            printf( "\t%s, %s\t%s\n", (const char*)pCurrent->GetLastName(),
                    (const char*)pCurrent->GetFirstName(),
                    (const char*)pCurrent->GetPhoneNumber() ) ;
        }
    }
}