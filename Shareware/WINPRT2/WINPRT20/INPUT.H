// input.h : header file
//

#ifndef __INPUT_H__
#define __INPUT_H__

/////////////////////////////////////////////////////////////////////////////
// CInput command target

class CWinPrintControl ;

class CInput : public CObject
{   
public:
    CInput();           // protected constructor used by dynamic creation
    virtual ~CInput();

// Attributes
public:
    CWinPrintControl* m_pControl ;                   

    CString& GetFileName() ;
    ULONG GetFileSize() ;
    void GetFileTime( FILETIME FAR* pFileTime ) ;
                   
                   
// Operations
public:

// Implementation
protected:
    CFile*  m_pFile ;
};

/////////////////////////////////////////////////////////////////////////////
#endif //
    
