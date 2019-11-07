// view.h
//

#ifndef _VIEW_H_
#define _VIEW_H_

#include "collist.h"

class CListBoxView : public CView
{
public: 
    CListBoxView();
    DECLARE_DYNAMIC(CListBoxView)

// Attributes
public:
    CColumnListBox  m_lb ;
    COleDropTarget  m_dropTarget ;
    
// Overridables
public:
    virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject,
        DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject,
        DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject,
        DROPEFFECT dropEffect, CPoint point);

// Generated message map functions
protected:
    //{{AFX_MSG(CListBoxView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // _VIEW_H_