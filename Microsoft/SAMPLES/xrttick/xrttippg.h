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
// xrttippg.h : Declaration of the CTickPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CTickPropPage : See xrttippg.cpp for implementation.

class CTickPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CTickPropPage)
	DECLARE_OLECREATE_EX(CTickPropPage)

// Constructor
public:
	CTickPropPage();

// Dialog Data
	//{{AFX_DATA(CTickPropPage)
	enum { IDD = IDD_PROPPAGE_XRTTICKER };
	CString	m_strText;
	long	m_lSpeed;
	long	m_lTickWidth;
	BOOL	m_fCacheOn;
	BOOL	m_fOffsetValues;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CTickPropPage)
	afx_msg void OnAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CDataObjectPropPage : Property page dialog

class CDataObjectPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CDataObjectPropPage)
	DECLARE_OLECREATE_EX(CDataObjectPropPage)

// Constructors
public:
	CDataObjectPropPage();

// Dialog Data
	//{{AFX_DATA(CDataObjectPropPage)
	enum { IDD = IDD_PROPPAGE_DATAOBJECT };
	CButton	m_btnStartStop;
	BOOL	m_fActive;
	CString	m_strDataObjectName;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CDataObjectPropPage)
	afx_msg void OnStartstop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CXRTPropPage : Property page dialog

class CXRTPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CXRTPropPage)
	DECLARE_OLECREATE_EX(CXRTPropPage)

// Constructors
public:
	CXRTPropPage();

// Dialog Data
	//{{AFX_DATA(CXRTPropPage)
	enum { IDD = IDD_PROPPAGE_WOSAXRT };
	BOOL	m_fDataObjectVisible;
	CString	m_strRequest;
	CString	m_strNameProperty;
	CString	m_strValueProperty;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CXRTPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// C_DXRTTicker wrapper class

class C_DXRTTicker : public COleDispatchDriver
{
public:
	C_DXRTTicker() {}		// Calls COleDispatchDriver default constructor
	C_DXRTTicker(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	C_DXRTTicker(const C_DXRTTicker& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	BOOL GetEnabled();
	void SetEnabled(BOOL);
	long GetScrollSpeed();
	void SetScrollSpeed(long);
	long GetScrollWidth();
	void SetScrollWidth(long);
	long GetMaxCacheSize();
	void SetMaxCacheSize(long);
	CString GetDataObjectName();
	void SetDataObjectName(LPCTSTR);
	BOOL GetDataObjectActive();
	void SetDataObjectActive(BOOL);
	BOOL GetDataObjectVisible();
	void SetDataObjectVisible(BOOL);
	BOOL GetAutoStartDataObject();
	void SetAutoStartDataObject(BOOL);
	BOOL GetCacheOn();
	void SetCacheOn(BOOL);
	BOOL GetOffsetValues();
	void SetOffsetValues(BOOL);
	CString GetDataObjectNameProperty();
	void SetDataObjectNameProperty(LPCTSTR);
	CString GetDataObjectValueProperty();
	void SetDataObjectValueProperty(LPCTSTR);
	CString GetDataObjectRequest();
	void SetDataObjectRequest(LPCTSTR);
	LPUNKNOWN GetDataObject();
	void SetDataObject(LPUNKNOWN);
	LPFONTDISP GetFont();
	void SetFont(LPFONTDISP);
	OLE_COLOR GetForeColor();
	void SetForeColor(OLE_COLOR);
	OLE_COLOR GetBackColor();
	void SetBackColor(OLE_COLOR);
	CString GetText();
	void SetText(LPCTSTR);

// Operations
public:
	void Clear();
	BOOL StartDataObject();
	BOOL StopDataObject();
	BOOL InitializeDataObject();
	void Tick(long ID, const VARIANT& Name, const VARIANT& Value);
	BOOL DataObjectAdvise();
	BOOL DataObjectUnAdvise();
	void AboutBox();
};
