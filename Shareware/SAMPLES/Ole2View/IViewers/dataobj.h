// dataobj.h : header file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1993 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __DATAOBJ_H__
#define __DATAOBJ_H__

#include "iviewer.h"
/////////////////////////////////////////////////////////////////////////////
// CDataObjectViewer command target

class CDataObjectViewer : public CInterfaceViewer
{
	DECLARE_DYNCREATE(CDataObjectViewer)
protected:
	CDataObjectViewer();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataObjectViewer)
	//}}AFX_VIRTUAL
    virtual HRESULT OnView(HWND hwndParent, REFIID riid, LPUNKNOWN punk);

// Implementation
protected:
	virtual ~CDataObjectViewer();

	// Generated message map functions
	//{{AFX_MSG(CDataObjectViewer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CDataObjectViewer)
};

#endif // __DATAOBJ_H__

