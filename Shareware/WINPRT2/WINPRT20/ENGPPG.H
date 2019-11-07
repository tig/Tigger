// engppg.h : Declaration of the CEnginePropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CEnginePropPage : See engppg.cpp for implementation.

class CEnginePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CEnginePropPage)
	DECLARE_OLECREATE_EX(CEnginePropPage)

// Constructor
public:
	CEnginePropPage();

// Dialog Data
	//{{AFX_DATA(CEnginePropPage)
	enum { IDD = IDD_PROPPAGE_WINPRINTENGINE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CEnginePropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

