// idataobj.h 
//
// Header file for the IDataObject interface viewer.
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// August 6, 1993
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  August 6, 1993  cek     First implementation.
//
 
#ifndef _IDATAOBJ_H_
#define _IDATAOBJ_H_ 

extern "C"
HRESULT WINAPI EXPORT DisplayIDataObject( HWND hwndParent, LPDATAOBJECT lpDO, LPIID lpiid, LPSTR lpszName ) ;

class CImpIAdviseSink ;
typedef CImpIAdviseSink FAR * LPIMPIADVISESINK;


/////////////////////////////////////////////////////////////////////////////
// IDataObjDlg dialog
//
extern "C"
BOOL WINAPI EXPORT fnIDataObjDlg( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

class FAR IDataObjDlg 
{
friend BOOL WINAPI EXPORT fnIDataObjDlg( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;
friend class CImpIAdviseSink ;
// Construction                    friend
public:
    IDataObjDlg( HWND hwnd, LPDATAOBJECT lpDO, LPIID lpiid, LPSTR lpszName ) ;
    ~IDataObjDlg() ;
    int DoModal( void ) ;

    LPDATAOBJECT    m_lpDO ;
    LPIID           m_lpiid ;
    LPSTR           m_lpszName ;
    
    FORMATETC       m_fetc ;
    DWORD           m_advf ;    
    BOOL            m_fDoOnGetDataPosted ;
    
    LPXRTSTOCKS     m_lpUpdateList ;
    
    HWND        m_hWndParent ;
    HWND        m_hDlg ;
                             
    HWND        m_btnDoGetData ;                             
    HWND        m_btnSetupAdvise ;
    HWND        m_lbGetData ;
    HWND        m_edtAdvise ;
    HWND        m_chkUpdateDisplay ;
    BOOL        m_fUpdateDisplay ;
    HWND        m_chkPrimeFirst ;

    HWND        m_lbFmtEtc ;    
    
    DWORD       m_dwTime ;
    DWORD       m_cOnDataChanges ;
    DWORD       m_cItemChanges ;

    LPIMPIADVISESINK    m_pSink ; 
    DWORD               m_dwConn ;

// Implementation
protected:
    BOOL DoIDataObject( UINT nLevel, LPDATAOBJECT pI ) ;
    BOOL DoIEnumFormatEtc( UINT nLevel, LPENUMFORMATETC pI ) ;
    
    int AddItem( UINT nLevel, LPSTR sz, LPVOID lpData, UINT uiType, int cColumns, LPCOLUMNSTRUCT  rgCol ) ;
    int AddItem( UINT nLevel, LPSTR sz, LPVOID lpData, UINT uiType ) ;
    int AddItem( UINT nLevel, LPSTR sz, LPVOID lpData ) ;

    BOOL OnInitDialog();
    
    void OnDoGetData() ;
    HRESULT GotData( LPFORMATETC lpfetc, LPSTGMEDIUM lpstm ) ;
    void OnSetupAdvise() ;
    void OnKillAdvise() ;
        
    void OnSize(UINT nType, int cx, int cy);
    void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    void OnDblClkFormatEtc();
    void OnSelChangeFormatEtc();
    void OnDestroy();
};

typedef IDataObjDlg FAR * LPIDATAOBJDLG ;

class FAR CImpIAdviseSink : public IAdviseSink
    {
    protected:
        ULONG               m_cRef;      //Interface reference count.
        LPIDATAOBJDLG       m_pIDOD ;       //Back pointer to the application

    public:
        CImpIAdviseSink(LPIDATAOBJDLG);
        ~CImpIAdviseSink(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //We only implement OnDataChange for now.
        STDMETHODIMP_(void)  OnDataChange(LPFORMATETC, LPSTGMEDIUM);
        STDMETHODIMP_(void)  OnViewChange(DWORD, LONG);
        STDMETHODIMP_(void)  OnRename(LPMONIKER);
        STDMETHODIMP_(void)  OnSave(void);
        STDMETHODIMP_(void)  OnClose(void);
    };


#endif // _IDATAOBJ_H_
