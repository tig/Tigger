// engine.h : Declaration of the CWinPrintEngine OLE control class.

/////////////////////////////////////////////////////////////////////////////
// CWinPrintEngine : See engine.cpp for implementation.

class CWinPrintEngine : public COleControl
{
    DECLARE_DYNCREATE(CWinPrintEngine)

// Constructors
public:
    CWinPrintEngine();

// Overrides

    // Persistence
    virtual void DoPropExchange(CPropExchange* pPX);

    // Reset control state
    virtual void OnResetState();

// Implementation
protected:
    ~CWinPrintEngine();

    BEGIN_OLEFACTORY(CWinPrintEngine)       // Class factory and guid
        virtual BOOL VerifyUserLicense();
        virtual BOOL GetLicenseKey(DWORD, BSTR FAR*);
    END_OLEFACTORY(CWinPrintEngine)

    DECLARE_OLETYPELIB(CWinPrintEngine)     // GetTypeInfo
//    DECLARE_PROPPAGEIDS(CWinPrintEngine)        // Property page IDs

// Message maps
    //{{AFX_MSG(CWinPrintEngine)
        // NOTE - ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

// Dispatch maps
    //{{AFX_DISPATCH(CWinPrintEngine)
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()

// Event maps
    //{{AFX_EVENT(CWinPrintEngine)
    //}}AFX_EVENT
    DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
    enum {
    //{{AFX_DISP_ID(CWinPrintEngine)
    //}}AFX_DISP_ID
    };
};
