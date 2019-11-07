// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1993 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#define UNUSED		// usage: UNUSED formal_arg

/////////////////////////////////////////////////////////////////////////////
// Codes for COleControl::SendAdvise
//......Code.........................Method called
#define OBJECTCODE_SAVED          0  //IOleAdviseHolder::SendOnSave
#define OBJECTCODE_CLOSED         1  //IOleAdviseHolder::SendOnClose
#define OBJECTCODE_RENAMED        2  //IOleAdviseHolder::SendOnRename
#define OBJECTCODE_SAVEOBJECT     3  //IOleClientSite::SaveObject
#define OBJECTCODE_DATACHANGED    4  //IDataAdviseHolder::SendOnDataChange
#define OBJECTCODE_SHOWWINDOW     5  //IOleClientSite::OnShowWindow(TRUE)
#define OBJECTCODE_HIDEWINDOW     6  //IOleClientSite::OnShowWindow(FALSE)
#define OBJECTCODE_SHOWOBJECT     7  //IOleClientSite::ShowObject
#define OBJECTCODE_VIEWCHANGED    8  //IOleAdviseHolder::SendOnViewChange


/////////////////////////////////////////////////////////////////////////////
// Typedefs
typedef LPVOID FAR* LPLPVOID;


/////////////////////////////////////////////////////////////////////////////
// Macros
#define RELEASE(p)  if ((p) != NULL) { (p)->Release(); (p) = NULL; };

#define ASSERT_POINTER(p, type) \
	ASSERT(((p) != NULL) && AfxIsValidAddress((p), sizeof(type), FALSE))

#define ASSERT_NULL_OR_POINTER(p, type) \
	ASSERT(((p) == NULL) || AfxIsValidAddress((p), sizeof(type), FALSE))

/////////////////////////////////////////////////////////////////////////////
// Functions
void CopyPropValue(VARTYPE vtProp, void* pvDest, const void * pvSrc);


/////////////////////////////////////////////////////////////////////////////
// CControlFrameWnd - used for a control's "open" (non-in-place) state.

class CControlFrameWnd : public CWnd
{
public:
	CControlFrameWnd(COleControl* pCtrl);
	virtual BOOL Create(LPCSTR pszTitle);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual void PostNcDestroy();

	COleControl* m_pCtrl;
	
	//{{AFX_MSG(CControlFrameWnd)
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CReflectorWnd - reflects window messages to a subclassed control.

class CReflectorWnd : public CWnd
{
public:
	CReflectorWnd(void) : m_hWndCtrl(NULL) {}
	BOOL Create(const CRect& rect, HWND hWndParent);
	LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void PostNcDestroy();
	void SetControlHwnd(HWND hWndCtrl);
	
protected:
	HWND m_hWndCtrl;
};


/////////////////////////////////////////////////////////////////////////////
// CArchivePropExchange - for persistence in an archive.

class CArchivePropExchange : public CPropExchange
{
// Constructors
public:
	CArchivePropExchange(CArchive& ar);
	
// Operations
	virtual BOOL ExchangeProp(LPCSTR pszPropName, VARTYPE vtProp,
				void* pvProp, const void* pvDefault = NULL);
	virtual BOOL ExchangeBlobProp(LPCSTR pszPropName, void** ppvBlob,
				const void* pvBlobDefault = NULL);
	virtual BOOL ExchangeFontProp(LPCSTR pszPropName, CFontHolder& font,
				const FONTDESC FAR* pFontDesc, LPFONTDISP pFontDispAmbient);

// Implementation
protected:
	CArchive& m_ar;
};


/////////////////////////////////////////////////////////////////////////////
// CResetPropExchange - for resetting property state to defaults.

class CResetPropExchange : public CPropExchange
{
// Constructors
public:
	CResetPropExchange(void);

// Operations
	virtual BOOL ExchangeProp(LPCSTR pszPropName, VARTYPE vtProp,
				void* pvProp, const void* pvDefault = NULL);
	virtual BOOL ExchangeBlobProp(LPCSTR pszPropName, void** ppvBlob,
				const void* pvBlobDefault = NULL);
	virtual BOOL ExchangeFontProp(LPCSTR pszPropName, CFontHolder& font,
				const FONTDESC FAR* pFontDesc, LPFONTDISP pFontDispAmbient);
};


/////////////////////////////////////////////////////////////////////////////
// CPropsetPropExchange - for persistence in a property set.

class CPropsetPropExchange : public CPropExchange
{
// Constructors
public:
	CPropsetPropExchange(CPropertySection& psec, LPSTORAGE lpStorage,
		BOOL bLoading);
	
// Operations
	virtual BOOL ExchangeProp(LPCSTR pszPropName, VARTYPE vtProp,
				void* pvProp, const void* pvDefault = NULL);
	virtual BOOL ExchangeBlobProp(LPCSTR pszPropName, void** ppvBlob,
				const void* pvBlobDefault = NULL);
	virtual BOOL ExchangeFontProp(LPCSTR pszPropName, CFontHolder& font,
				const FONTDESC FAR* pFontDesc, LPFONTDISP pFontDispAmbient);

// Implementation
	CPropertySection& m_psec;
	LPSTORAGE m_lpStorage;
	DWORD m_dwPropID;
};
