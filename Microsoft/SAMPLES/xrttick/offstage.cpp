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
// offstage.cpp : implementation file
//

#include "stdafx.h"
#include "xrttick.h"
#include "tickctl.h"
#include "offstage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTick

CTick::CTick( CTickCtrl* pctl )
{
    m_dwDataItemID = (DWORD)-1L ;
    m_x = 0 ;
    m_pControl = pctl ;
    m_bCache= FALSE ;
    m_bDelete = FALSE ;
}

CTick::~CTick()
{
    if (m_bm.GetSafeHandle())
    {
        m_dc.DeleteDC() ;
        m_bm.DeleteObject() ;
        m_bm.m_hObject = NULL ;
    }
}

//  Function:   Create
//
//  Synopsis:   Given a text string and a DC, creates a bitmap with that
//              text rendered in it.  The bitmap will always be at least
//              cxMin wide.   The resulting size of the bitmap is 
//              returned in sizeTicker
//
void CTick::CreateImage( CDC* pdcIn ) 
{
    CRect       rc ;
    ASSERT_VALID(pdcIn) ;    
    EraseImage() ;

    // Create a bitmap at least as large as the display, but not larger
    // than the max.
    //
    m_dc.CreateCompatibleDC( pdcIn ) ;
    ASSERT_VALID(&m_dc) ;

    // Select in our font
    //
    CFont* pfontOld = m_dc.SelectObject( CFont::FromHandle(m_pControl->InternalGetFont().GetFontHandle() )) ; 
    CSize sizeName ;
    CSize sizeValue(0,0) ;

    CString str = m_strName + " " ;
    if (!m_strValue.IsEmpty())
        str += " " ;
    sizeName = m_dc.GetTextExtent( str, str.GetLength()) ;

    if (!m_strValue.IsEmpty())
    {
        str = m_strValue + "  " ;
        sizeValue = m_dc.GetTextExtent( str, str.GetLength()) ;
    }

    if (m_pControl->m_fOffsetValues)
        sizeValue.cy = sizeName.cy + (sizeName.cy / 3) ;

    m_size.cy = m_pControl->m_sizeDisplay.cy ;
    m_size.cx = sizeName.cx + sizeValue.cx ;
    m_bm.CreateCompatibleBitmap( pdcIn, m_size.cx, m_size.cy ) ;

    // Draw the text into the bitmap
    //
    // TODO:  Make this do a cool LED thing
    //
    m_dc.SelectObject( &m_bm ) ;
    rc.SetRect( 0, 0, m_size.cx, m_size.cy ) ;
    m_dc.SetBkColor( m_pControl->TranslateColor( m_pControl->GetBackColor() ) ) ;
    m_dc.ExtTextOut( 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;
    
    m_dc.SetBkMode( TRANSPARENT ) ;

    // Draw text
    //    
    #if 0
    if (m_bCache)
        // TODO:  Create a "CacheForeColor" property for ticks that are displayed
        // out of the cache
        //
        m_dc.SetTextColor( RGBLTGRAY ) ;
    else
    #endif
        m_dc.SetTextColor( m_pControl->TranslateColor( m_pControl->GetForeColor() ) ) ;

    str = m_strName + " " ;
    if (!m_strValue.IsEmpty())
        str += " " ; 
    rc.top += (m_size.cy - sizeValue.cy) / 2 ;
    rc.bottom = rc.top + sizeName.cy ;
    rc.right = sizeName.cx ;
    m_dc.ExtTextOut( rc.left, rc.top, 0, &rc, str, str.GetLength(), NULL ) ;

    if (!m_strValue.IsEmpty())
    {
        str = m_strValue + "  " ;
        if (m_pControl->m_fOffsetValues)
        {
            rc.top += (sizeName.cy / 3) ;
            rc.bottom += (sizeName.cy / 3) ;
        }
        rc.left = sizeName.cx ;
        rc.right = m_size.cy ;
        m_dc.ExtTextOut( rc.left, rc.top, 0, &rc, str, str.GetLength(), NULL ) ;
    }
            
    m_dc.SelectObject( pfontOld) ; 
    m_x = 0 ;
    return ;    
}

void CTick::EraseImage() 
{
    if (m_bm.GetSafeHandle())
    {
        m_dc.DeleteDC() ;
        m_dc.m_hDC = NULL ;
        m_dc.m_hAttribDC = NULL ;
        m_bm.DeleteObject() ;         
        m_bm.m_hObject = NULL ;
    }
}
