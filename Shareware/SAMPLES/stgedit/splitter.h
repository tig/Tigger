// splitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplitter frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


class CSplitter : public CSplitterWnd
{
	DECLARE_DYNCREATE(CSplitter)

// Attributes
public:

public:
   	CSplitter();
   	
// Operations
public:
    BOOL ReplaceView(int row, int col,CRuntimeClass * pViewClass,SIZE size);
// Implementation
public:
	virtual ~CSplitter();

	// Generated message map functions
	//{{AFX_MSG(CSplitter)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
