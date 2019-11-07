// control.h : Declaration of the CWinPrintControl OLE control class.

#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "header.h"
#include "ftd.h"
#include "page.h"
#include "input.h"

#define ASSERT_POINTER(p, type) \
    ASSERT(((p) != NULL) && AfxIsValidAddress((p), sizeof(type), FALSE))

#define ASSERT_NULL_OR_POINTER(p, type) \
    ASSERT(((p) == NULL) || AfxIsValidAddress((p), sizeof(type), FALSE))


/////////////////////////////////////////////////////////////////////////////
// CWinPrintControl : See config.cpp for implementation.

class CWinPrintControl : public COleControl
{   
friend class CPage ;
friend class CHeader ;
friend class CHeaderSection ;
friend class CInput ;
    DECLARE_DYNCREATE(CWinPrintControl)

// Constructors
public:
    CWinPrintControl();

// Overrides

    // Drawing function
    virtual void OnDraw(
                CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);

    // Persistence
    virtual void DoPropExchange(CPropExchange* pPX);

    // Reset control state
    virtual void OnResetState();

    virtual void Serialize(CArchive& ar);

// Implementation
protected:
    ~CWinPrintControl();

    void ExchangePrinterInfo( CPropExchange* pPX ) ;
    void ExchangeFileTypeDriver( CPropExchange* pPX ) ;

    BEGIN_OLEFACTORY(CWinPrintControl)      // Class factory and guid
        virtual BOOL VerifyUserLicense();
        virtual BOOL GetLicenseKey(DWORD, BSTR FAR*);
    END_OLEFACTORY(CWinPrintControl)

    DECLARE_OLETYPELIB(CWinPrintControl)        // GetTypeInfo
    DECLARE_PROPPAGEIDS(CWinPrintControl)       // Property page IDs

// Message maps
    //{{AFX_MSG(CWinPrintControl)
        // NOTE - ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

// Dispatch maps
    //{{AFX_DISPATCH(CWinPrintControl)
    BOOL m_fDefaultPrinter;
    afx_msg void OnUseDefaultPrinterChanged();
    BOOL m_fUsePrinterSettings;
    afx_msg void OnUsePrinterSettingsChanged();
    CString m_strPrinter;
    afx_msg void OnPrinterChanged();
    CString m_strPrinterDriver;
    afx_msg void OnPrinterDriverChanged();
    CString m_strPrinterOutput;
    afx_msg void OnPrinterOutputChanged();
    long m_lOrientation;
    afx_msg void OnOrientationChanged();
    long m_lPaperSize;
    afx_msg void OnPaperSizeChanged();
    long m_lPaperLength;
    afx_msg void OnPaperLengthChanged();
    long m_lPaperWidth;
    afx_msg void OnPaperWidthChanged();
    long m_lScale;
    afx_msg void OnScaleChanged();
    long m_lCopies;
    afx_msg void OnCopiesChanged();
    long m_lDefaultSource;
    afx_msg void OnDefaultSourceChanged();
    long m_lPrintQuality;
    afx_msg void OnPrintQualityChanged();
    long m_lColor;
    afx_msg void OnColorChanged();
    long m_lDuplex;
    afx_msg void OnDuplexChanged();
    float m_flTopMargin;
    afx_msg void OnTopMarginChanged();
    float m_flBottomMargin;
    afx_msg void OnBottomMarginChanged();
    float m_flLeftMargin;
    afx_msg void OnLeftMarginChanged();
    float m_flRightMargin;
    afx_msg void OnRightMarginChanged();
    BOOL m_fBorder;
    afx_msg void OnBorderChanged();
    BOOL m_fShade;
    afx_msg void OnShadeChanged();
    short m_sRows;
    afx_msg void OnRowsChanged();
    short m_sColumns;
    afx_msg void OnColumnsChanged();
    float m_flHorzSeparation;
    afx_msg void OnHorzSeparationChanged();
    float m_flVertSeparation;
    afx_msg void OnVertSeparationChanged();
    afx_msg BSTR GetConfigName();
    afx_msg BSTR GetFileTypeDriverName();
    afx_msg void SetFileTypeDriverName(LPCSTR lpszNewValue);
    afx_msg LPDISPATCH GetFileTypeDriver();
    afx_msg LPDISPATCH GetHeader();
    afx_msg void SetHeader(LPDISPATCH newValue);
    afx_msg LPDISPATCH GetFooter();
    afx_msg void SetFooter(LPDISPATCH newValue);
    afx_msg BOOL LoadConfiguration(LPCSTR ConfigName);
    afx_msg BOOL SaveConfiguration(LPCSTR ConfigName);
    afx_msg void SetSource(LPCSTR SourceFile);
    afx_msg BOOL Print(const VARIANT FAR& Source);
    //}}AFX_DISPATCH
//    afx_msg void SetFileTypeDriver(LPDISPATCH newValue);
    DECLARE_DISPATCH_MAP()

// Event maps
    //{{AFX_EVENT(CWinPrintControl)
    //}}AFX_EVENT
    DECLARE_EVENT_MAP()
    
    CString     m_strConfigName ;
    CString     m_strFileTypeDriver ;
    LPDISPATCH  m_pdispFileTypeDriver ;
    //CString     m_strPrinter ;
    // REVIEW:  Need to provide shade color

    CHeader     m_Header ;
    CHeader     m_Footer ;
    
    // REVIEW:  Need a property for units (inches vs. cm).
    
    // Keep a memory stream for each file type driver
    // this control has ever loaded.  The map is of
    // ProgIDs to COleStreamFile*
    //
    CMapStringToOb m_mapStrToStm ;

    // When calcluating margins you can either incorporate the
    // printoffset or not.  If fUsePrinteOffset is True then
    // margin measurements will *include* the print offset (the
    // non-printable area of the physical page).
    //
    BOOL        m_fUsePrintOffset ;
    
    // The page keeps a local copy of the page dimensions
    // and other info.
    //  
    CPage       m_Page ;

    // The input object is responsible for managing the input.
    // 
    CInput      m_Input ;
    
// Dispatch and event IDs
public:
    enum {
    //{{AFX_DISP_ID(CWinPrintControl)
    dispidPrinter = 1L,
    dispidPrinterDriver = 2L,
    dispidPrinterOutput = 3L,
    dispidUseDefaultPrinter = 4L,
    dispidUsePrinterSettings = 5L,
    dispidOrientation = 56,
    dispidPaperSize = 7L,
    dispidPaperLength = 8L,
    dispidPaperWidth = 9L,
    dispidScale = 10L,
    dispidCopies = 11L,
    dispidDefaultSource = 12L,
    dispidPrintQuality = 13L,
    dispidColor = 14L,
    dispidDuplex = 15L,
    dispidTopMargin = 16L,
    dispidBottomMargin = 17L,
    dispidLeftMargin = 18L,
    dispidRightMargin = 19L,
    dispidBorder = 20L,
    dispidShade = 21L,
    dispidRows = 22L,
    dispidColumns = 23L,
    dispidHorzSeparation = 24L,
    dispidVertSeparation = 25L,
    dispidConfigName = 26L,
    dispidFileTypeDriverName = 27L,
    dispidFileTypeDriver = 28L,
    dispidHeader = 29L,
    dispidFooter = 30L,
    dispidLoadConfiguration = 31L,
    dispidSaveConfiguration = 32L,
    dispidSetSource = 33L,
    dispidPrint = 34L,
    //}}AFX_DISP_ID
    };
};

#endif //
