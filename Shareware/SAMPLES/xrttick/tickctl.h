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
// tickctl.h : Declaration of the CTickCtrl OLE control class.
#include "wosaxrt.h"

/////////////////////////////////////////////////////////////////////////////
// CTickCtrl : See tickctl.cpp for implementation.
class CTick ;

class CTickCtrl : public COleControl
{
    DECLARE_DYNCREATE(CTickCtrl)

// Constructor
public:
    CTickCtrl();

// Overrides

    // Drawing function
	virtual void OnDrawMetafile(CDC* pDC, const CRect& rcBounds);
    virtual void OnDraw(
                CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);

    // Persistence
    virtual void DoPropExchange(CPropExchange* pPX);

    // Reset control state
    virtual void OnResetState();

	// Change notifications
    virtual void OnBackColorChanged() ;
    virtual void OnEnabledChanged();
    virtual void OnTextChanged() ;
    virtual void OnFontChanged() ;
    virtual void OnForeColorChanged() ;

	// IOleControl notifications
    virtual void OnAmbientPropertyChange(DISPID dispid);

	// IDataObject - see COleDataSource for a description of these overridables
    virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal) ;

	// IOleObject notifications
	virtual BOOL OnSetExtent(LPSIZEL lpSizeL);

    // HACK:  Work Around Excel 5.0 for NT bug
#ifdef _WIN32
	virtual void OnSetClientSite();
#endif

//    virtual void OnChange(OLE_NOTIFICATION nCode, DWORD dwParam);
    virtual void SetInitialDataFormats();
// Implementation
protected:
    ~CTickCtrl();

    DECLARE_OLECREATE_EX(CTickCtrl)    // Class factory and guid
    DECLARE_OLETYPELIB(CTickCtrl)      // GetTypeInfo
    DECLARE_PROPPAGEIDS(CTickCtrl)     // Property page IDs
    DECLARE_OLECTLTYPE(CTickCtrl)       // Type name and misc status

    // Create a sub-object that implements IAdviseSink
    // Use the MFC macros to do it, but hack it so that
    // this IAdviseSink does NOT hang off of the COntrol's
    // IUnknown....it's a separate object.
    //
    BEGIN_INTERFACE_PART(RTAdviseSink, IAdviseSink)
        XRTAdviseSink() ;
        int m_cRef ;
        STDMETHOD_(void,OnDataChange)(LPFORMATETC, LPSTGMEDIUM);
        STDMETHOD_(void,OnViewChange)(DWORD, LONG);
        STDMETHOD_(void,OnRename)(LPMONIKER);
        STDMETHOD_(void,OnSave)();
        STDMETHOD_(void,OnClose)();     
    END_INTERFACE_PART(RTAdviseSink)


// Message maps
    //{{AFX_MSG(CTickCtrl)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

// Dispatch maps
    //{{AFX_DISPATCH(CTickCtrl)
	long m_nScrollSpeed;
	afx_msg void OnScrollSpeedChanged();
	long m_nScrollWidth;
	afx_msg void OnScrollWidthChanged();
	long m_nMaxCacheSize;
	afx_msg void OnMaxCacheSizeChanged();
	CString m_strDataObjectName;
	afx_msg void OnDataObjectNameChanged();
	BOOL m_fActive;
	afx_msg void OnDataObjectActiveChanged();
	BOOL m_fDataObjectVisible;
	afx_msg void OnDataObjectVisibleChanged();
	BOOL m_fAutoStartDataObject;
	afx_msg void OnAutoStartDataObjectChanged();
	BOOL m_fCacheOn;
	afx_msg void OnCacheOnChanged();
	BOOL m_fOffsetValues;
	afx_msg void OnOffsetValuesChanged();
	CString m_strNameProperty;
	afx_msg void OnDataObjectNamePropertyChanged();
	CString m_strValueProperty;
	afx_msg void OnDataObjectValuePropertyChanged();
	CString m_strRequest;
	afx_msg void OnDataObjectRequestChanged();
	afx_msg LPDISPATCH GetDataObject();
	afx_msg void SetDataObject(LPDISPATCH newValue);
    afx_msg void Clear();
    afx_msg BOOL StartDataObject();
    afx_msg BOOL StopDataObject();
    afx_msg BOOL InitializeDataObject();
    afx_msg void Tick(long ID, const VARIANT FAR& Name, const VARIANT FAR& Value);
    afx_msg BOOL DataObjectAdvise();
    afx_msg BOOL DataObjectUnAdvise();
	//}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()

    afx_msg void AboutBox();

// Event maps
    //{{AFX_EVENT(CTickCtrl)
	void FireCacheOverflow()
		{FireEvent(eventidCacheOverflow,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
    DECLARE_EVENT_MAP()

    // Bitmap to paint
    //
    void CreateDisplayBitmap( CDC* pdcIn = NULL) ;
    CDC        m_dcDisplay ;
    CBitmap    m_bmDisplay ;
    CSize      m_sizeDisplay;
    //COleDispatchDriver  m_drvDataObject ;
    IDataObject*    m_pDataObject ; // holds our reference to the dataobject
    DWORD      m_dwAdvise ;
    BOOL       m_fFeedDown ;
    BOOL       m_fWOSAXRT ;     // Are we talking XRT?
    // Tick queue management
    //
    CObList     m_qCache ;      // CTick objects that have already been drawn once
    CObList     m_qNew ;        // CTick objects that have yet to be drawn
    CTick*      m_pOnstage ;    // Currently being drawn
    CTick*      FindInQue( CObList& que, DWORD dwDataItemID ); 
    CString     GetQueText();

    void MoveTicker( long nMove ) ;
    void StartTimer(); 
    void StopTimer();
    void XRTDataChange(LPMARKETDATA pMD);
    void TextDataChange(TCHAR* psz);
    void ErrorMessageBox( const CString& str, HRESULT hr );
    void SyncDataObjectActiveState() ;
    void SyncDataObjectVisibleState() ;
    HRESULT SetupAdviseSink();
    HRESULT KillAdviseSink();

    void ThrowMyDispatchException(CException* pException, const CString& strErrorMsg, HRESULT hr);

// Dispatch and event IDs
public:
    enum {
    //{{AFX_DISP_ID(CTickCtrl)
	dispidScrollSpeed = 1L,
	dispidScrollWidth = 2L,
	dispidMaxCacheSize = 3L,
	dispidDataObjectName = 4L,
	dispidDataObjectActive = 5L,
	dispidDataObjectVisible = 6L,
	dispidAutoStartDataObject = 7L,
	dispidCacheOn = 8L,
	dispidOffsetValues = 9L,
	dispidDataObjectNameProperty = 10L,
	dispidDataObjectValueProperty = 11L,
	dispidDataObjectRequest = 12L,
	dispidDataObject = 13L,
	dispidClear = 14L,
	dispidStartDataObject = 15L,
	dispidStopDataObject = 16L,
	dispidInitializeDataObject = 17L,
	dispidTick = 18L,
	dispidDataObjectAdvise = 19L,
	dispidDataObjectUnAdvise = 20L,
	eventidCacheOverflow = 1L,
	//}}AFX_DISP_ID
    };

    // Implementation of IDataObject
    // CControlDataSource implements OnRender reflections to COleControl
    class CDropSourceDataSource : public COleDataSource
    {
    protected:
        virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal);
        virtual BOOL OnRenderData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium);

        virtual BOOL OnSetData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium,
            BOOL bRelease);
    };
    CDropSourceDataSource m_datasourceDropSource;
    friend class CDropSourceDataSource;

    class CMyOleDropTarget : public COleDropTarget
    {
	    virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		    DWORD dwKeyState, CPoint point);
	    virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		    DWORD dwKeyState, CPoint point);
	    virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		    DROPEFFECT dropEffect, CPoint point);

    };
    CMyOleDropTarget  m_dropTarget ;
    friend class CMyOleDropTarget;

friend class CTick ;
};

#ifndef _RGB_H_
#define _RGB_H_
   #define RGBBLACK     RGB(0,0,0)
   #define RGBRED       RGB(128,0,0)
   #define RGBGREEN     RGB(0,128,0)
   #define RGBBLUE      RGB(0,0,128)
   #define RGBBROWN     RGB(128,128,0)
   #define RGBMAGENTA   RGB(128,0,128)
   #define RGBCYAN      RGB(0,128,128)
   #define RGBLTGRAY    RGB(192,192,192)
   #define RGBGRAY      RGB(128,128,128)
   #define RGBLTRED     RGB(255,0,0)
   #define RGBLTGREEN   RGB(0,255,0)
   #define RGBLTBLUE    RGB(0,0,255)
   #define RGBYELLOW    RGB(255,255,0)
   #define RGBLTMAGENTA RGB(255,0,255)
   #define RGBLTCYAN    RGB(0,255,255)
   #define RGBWHITE     RGB(255,255,255)

   const OLE_COLOR      OLE_COLOR_BLACK     =0x00000000;
   const OLE_COLOR      OLE_COLOR_RED       =0x00000080;
   const OLE_COLOR      OLE_COLOR_GREEN     =0x00008000;
   const OLE_COLOR      OLE_COLOR_BLUE      =0x00800000;
   const OLE_COLOR      OLE_COLOR_BROWN     =0x00008080;
   const OLE_COLOR      OLE_COLOR_MAGENTA   =0x00800080;
   const OLE_COLOR      OLE_COLOR_CYAN      =0x00808000;
   const OLE_COLOR      OLE_COLOR_GRAY      =0x00808080;
   const OLE_COLOR      OLE_COLOR_LTRED     =0x000000FF;
   const OLE_COLOR      OLE_COLOR_LTGREEN   =0x0000FF00;
   const OLE_COLOR      OLE_COLOR_LTBLUE    =0x00FF0000;
   const OLE_COLOR      OLE_COLOR_YELLOW    =0x0000FFFF;
   const OLE_COLOR      OLE_COLOR_LTMAGENTA =0x00FF00FF;
   const OLE_COLOR      OLE_COLOR_LTCYAN    =0x00FFFF00;
   const OLE_COLOR      OLE_COLOR_LTGRAY    =0x00C0C0C0;
   const OLE_COLOR      OLE_COLOR_WHITE     =0x00FFFFFF;
#endif

