// WOSA/XRT XRTTick Sample OLE Control Version 1.00.001
//
// Copyright (c) 1993-94 Microsoft Corporation, All Rights Reserved.
//
//      This is a part of the Microsoft Source Code Samples. 
//      This source code is only intended as a supplement to 
//      Microsoft Development Tools and/or WinHelp documentation.
//      See these sources for detailed information regarding the 
//      Microsoft samples programs.
//
// offstage.h : header file
//

class CTickCtrl ;

/////////////////////////////////////////////////////////////////////////////
// CTick window

class CTick : public CObject
{
// Construction
public:
	CTick();
    CTick( CTickCtrl* pControl ) ;
    virtual ~CTick() ;

    BOOL        IsClear()
    { return m_bm.m_hObject == NULL ; }

    CString     m_strName ;
    CString     m_strValue ;
    CBitmap     m_bm ;
    CSize       m_size ;
    CDC         m_dc ;
    int         m_x ;
    BOOL        m_bCache ;
    BOOL        m_bDelete ;
    DWORD       m_dwDataItemID ;

    void CreateImage( CDC* pdcIn ) ;
    void EraseImage() ;

protected:
    CTickCtrl* m_pControl ;
};

/////////////////////////////////////////////////////////////////////////////
