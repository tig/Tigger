// job.h : header file
//

#ifndef __PAGE_H__
#define __PAGE_H__

class CWinPrintControl ;
  
/////////////////////////////////////////////////////////////////////////////
// CPage command target
//
class CPage : public CObject
{   
public:
    CPage();         // protected constructor used by dynamic creation
    virtual ~CPage();

// Attributes
public:   
    CWinPrintControl* m_pControl ;

    // Page resolution in device units (from HORZRES/VERTRES)
    // Note that this 
    //
    int    m_iResolutionX ;
    int    m_iResolutionY ;

    // Logical pixels per inch (from LOGPIXELSX/Y)
    //
    int    m_iLogPixelsX ;
    int    m_iLogPixelsY ;
    
    // Offset from physical page edge to 0,0.  IOW painting to pixel 0,0
    // actually draws at m_ptPrintOffset.  From Escape(GETPRINTINGOFFSET)
    //
    CPoint  m_ptPrintOffset ;

    // Workspace.  The Workspace is defined by the margins and header/footers.
    // IOW, the logical pages are drawing within this rect.  Note that
    // borders 'encroach' on the workspace.
    //
    CRect   m_rcWorkspace ;
    
    // Logical page.  The logical pages are measured by a horz and vert 
    // dimension.  The CWinPrintControl defines how many rows/columns there
    // are but, the CPage is responsible for figuring out how many device
    // units there are across and down each logical page.
    //
    int    m_iLogicalPageX ;
    int    m_iLogicalPageY ;
    
    // Current physical page #
    int     m_iPhysicalPage ;
    int     m_iNumPhysicalPages ;
    
// Operations
public:
    BOOL Recalc( CDC* pdc = NULL ) ;
    void Draw( CDC *pdc ) ;
    void DrawFrame( CDC *pdc ) ;

    int GetPhysicalPageNum() ;
    int GetNumPhysicalPages() ;
    
// Implementation
protected:

};

/////////////////////////////////////////////////////////////////////////////
    
#endif // __PAGE_H__
