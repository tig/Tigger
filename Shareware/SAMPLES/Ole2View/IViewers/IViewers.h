// IViewers.h : main header file for the IVIEWERS DLL
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __IVIEWERS_H__
#define __IVIEWERS_H__
#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIViewersApp
// See IViewers.cpp for the implementation of this class
//

class CIViewersApp : public CWinApp
{
public:
	CIViewersApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIViewersApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CIViewersApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#include "iview.h"

// CLSIDs of viewers implemented in IVIEWER.DLL
//
DEFINE_GUID(CLSID_IUnknownViewer,    0x7ce551ea, 0xf85c, 0x11ce, 0x90, 0x59, 0x8,  0x0,  0x36, 0xf1, 0x25, 0x2) ;
DEFINE_GUID(CLSID_IPersistViewer,    0x7ce551eb, 0xf85c, 0x11ce, 0x90, 0x59, 0x8, 0x0, 0x36, 0xf1, 0x25, 0x2) ;
DEFINE_GUID(CLSID_ITypeLibViewer,    0x57efbf49, 0x4a8b, 0x11ce, 0x87, 0xb,  0x8,  0x0,  0x36, 0x8d, 0x23, 0x2 ) ;
DEFINE_GUID(CLSID_IDataObjectViewer, 0x28d8aba0, 0x4b78, 0x11ce, 0xb2, 0x7d, 0x0,  0xaa, 0x0,  0x1f, 0x73, 0xc1);
DEFINE_GUID(CLSID_IDispatchViewer,   0xd2af7a60, 0x4c42, 0x11ce, 0xb2, 0x7d, 0x00, 0xaa, 0x00, 0x1f, 0x73, 0xc1) ;
DEFINE_GUID(IID_IInterfaceViewer,0xfc37e5ba,0x4a8e,0x11ce, 0x87,0x0b,0x08,0x00,0x36,0x8d,0x23,0x02);
// {64454F82-F827-11ce-9059-080036F12502}
DEFINE_GUID(TYPEID_Ole2ViewInterfaceViewers, 0x64454f82, 0xf827, 0x11ce, 0x90, 0x59, 0x8, 0x0, 0x36, 0xf1, 0x25, 0x2);

// Helper function exported from iviewers.dll.  Make sure OLE is initialized
// before calling.   rclsid is the clsid of the interface viewer and riid
// is the iid of the interface.
//
STDAPI DllRegisterInterfaceViewer( REFCLSID rclsid, REFIID riid );
STDAPI DllRegisterDefaultInterfaceViewer( REFCLSID rclsid, REFIID riid );

/////////////////////////////////////////////////////////////////////////////
#endif // __IVIEWERS_H__
