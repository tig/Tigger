// obj_vw.h
//
// Interface def for CObjTreeView
//

#ifndef _OBJ_VW_H_
#define _OBJ_VW_H_

/////////////////////////////////////////////////////////////////////////////
// CObjectData
#ifndef CLSCTX_REMOTE_SERVER
#define CLSCTX_REMOTE_SERVER	0x10
typedef struct _ComServerInfo
{
    OLECHAR     *       pszName;  // machine name
} ComServerInfo;
#endif

class CObjectData : public CObject
{
    DECLARE_DYNCREATE(CObjectData)
public:
    CObjectData() ;
    virtual ~CObjectData() ;

    HRESULT CreateInstance( DWORD clsctx, ComServerInfo* pcsi) ;
    BOOL Release() ;
    BOOL Initialize(const TCHAR* szCLSID);

    DWORD           m_nType;

    CLSID           m_clsid ;
    TCHAR           m_szProgID[40] ; // max 39 chars
    WORD            m_wMajorVer, m_wMinorVer ;
    UINT            m_uiBitmap ;
    UINT            m_uiCategoryHint;
    IClassFactory*  m_pcf ;
    IUnknown*       m_punk ;
    CString         m_strPersistFile ; // for BindToFile
    CString         m_strToolBoxBitmap;

	enum {
        typeUnknown,
        typeObject,
        typeObjectInstance,
        typeTypeLib,
        typeInterface,
        typeStaticInterface,
        typeCategories,
        typeCategory
	};

} ;

class CObjTreeView : public CTreeView
{
	DECLARE_DYNCREATE(CObjTreeView)
protected: // create from serialization only
    CObjTreeView();

// Attributes
public:
	BOOL GetActivateAtStorage(REFCLSID rclsid);
	BOOL SetActivateAtStorage(REFCLSID rclsid, BOOL fSet);
	void OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL AddObjectInstance(IUnknown* punk, const CString& strFileNameOrProgID);
	BOOL AddObjectInstance(LPCTSTR szDesc, CObjectData* pOD);
	HRESULT BindToFile(const CString& strFileName, IUnknown** ppunk);

    COle2ViewDoc* GetDocument();
    HTREEITEM     m_hObjectInstances;
    HTREEITEM     m_hObjectClasses;
    HTREEITEM     m_hTypeLibs ;
    HTREEITEM     m_hInterfaces;

    HTREEITEM     m_hInsertable ;
    HTREEITEM     m_hControls ;
    HTREEITEM     m_hInternal ;
#ifdef SHOW_CONTAINERS
    HTREEITEM     m_hContainers ;
#endif
    HTREEITEM     m_hOLE1 ;
    HTREEITEM     m_hUnclassified ;
    HTREEITEM     m_hInformalTypes ;
    BOOL          m_fComCatMgrAvail ;
    CString       m_strServerName;
// Operations
public:
    BOOL IsValidSel() ;
    void ExpandInterfaces( NM_TREEVIEW* pNMTreeView );
    void ExpandTypeLibs( NM_TREEVIEW* pNMTreeView );
    void ExpandClassification( NM_TREEVIEW* pNMTreeView );
    void ExpandObject( NM_TREEVIEW* pNMTreeView );
    void ExpandComponentCategories( NM_TREEVIEW* pnmtv );
	BOOL CallInterfaceViewer(CObjectData* pObjectData, HTREEITEM hItem);

// Implementation
public:

    virtual ~CObjTreeView();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) ;
    virtual void OnDraw( CDC* pdc );
	virtual void OnInitialUpdate();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
    //{{AFX_MSG(CObjTreeView)
    afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnObjectDelete();
	afx_msg void OnObjectVerify();
    afx_msg void OnUseInProcServer();
    afx_msg void OnUpdateUseInProcServer(CCmdUI* pCmdUI);
    afx_msg void OnUseInProcHandler();
    afx_msg void OnUpdateUseInProcHandler(CCmdUI* pCmdUI);
    afx_msg void OnUseLocalServer();
    afx_msg void OnUpdateUseLocalServer(CCmdUI* pCmdUI);
    afx_msg void OnFileBind();
	afx_msg void OnObjectRelease();
	afx_msg void OnUpdateObjectRelease(CCmdUI* pCmdUI);
	afx_msg void OnObjectCreate();
	afx_msg void OnUpdateObjectCreate(CCmdUI* pCmdUI);
	afx_msg void OnObjectView();
	afx_msg void OnUpdateObjectView(CCmdUI* pCmdUI);
	afx_msg void OnViewHiddenComCats();
	afx_msg void OnUpdateViewHiddenComCats(CCmdUI* pCmdUI);
	afx_msg void OnObjectCreateOn();
	afx_msg void OnUpdateObjectCreateOn(CCmdUI* pCmdUI);
	afx_msg void OnObjectAtStorage();
	afx_msg void OnUpdateObjectAtStorage(CCmdUI* pCmdUI);
	//}}AFX_MSG
    afx_msg void OnTreeSelchanged(NMHDR* pNMHDR, LRESULT* pResult) ;
    afx_msg void OnTreeItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) ;
    afx_msg void OnTreeItemExpanded(NMHDR* pNMHDR, LRESULT* pResult) ;
    afx_msg void OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) ;
    afx_msg void OnTreeReturn(NMHDR* pNMHDR, LRESULT* pResult) ;
    afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM lParam) ;
    DECLARE_MESSAGE_MAP()

	friend class CMainFrame;
};

#ifndef _DEBUG  // debug version in vw.cpp
inline COle2ViewDoc* CObjTreeView::GetDocument()
   { return (COle2ViewDoc*) m_pDocument; }
#endif

#endif // _OBJ_VW_H_
