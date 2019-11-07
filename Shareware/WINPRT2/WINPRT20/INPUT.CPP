// input.cpp : implementation file
//

#include "stdafx.h"
#include "winprt20.h"
#include "input.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInput

CInput::CInput()
{
    m_pControl = NULL ;
}

CInput::~CInput()
{
}

CString& CInput::GetFileName() 
{
    return CString("") ;
}

ULONG CInput::GetFileSize() 
{
    return 0L ;
}

void CInput::GetFileTime( FILETIME FAR* pFileTime ) 
{
UNUSED pFileTime ;
}
