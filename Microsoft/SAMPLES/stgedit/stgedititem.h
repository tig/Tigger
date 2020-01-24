
/////////////////////////////////////////////////////////////////////////////
// StgEditItem.h : header file
//

#include "propset.h"

// Map from STATSTG.type to the tree view's image list entries
#define TYPE_TO_ID(n) ((n-1)*2) 
 
// Map from the image ID to a STATSTG.type
#define ID_TO_TYPE(n) ((n / 2) + 1)

/////////////////////////////////////////////////////////////////////////////
// CStgEditItem command target

class CStgEditItem : public CObject
{
	DECLARE_DYNAMIC(CStgEditItem)

	CStgEditItem(CTreeCtrl* pTree);           // protected constructor used by dynamic creation
	virtual ~CStgEditItem();

// Attributes
public:
    CTreeCtrl*      m_pTree ;

// Operations
public:
    virtual BOOL Expand( HTREEITEM hitem ) ;

// Implementation

};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CStgEditStgItem command target

class CStgEditStgItem : public CStgEditItem
{
	DECLARE_DYNAMIC(CStgEditStgItem)
		
	CStgEditStgItem(CTreeCtrl* pTree, IStorage* pStorage);           // protected constructor used by dynamic creation
	virtual ~CStgEditStgItem();

// Attributes
public:
	IStorage* m_pStorage;

// Operations
public:
    virtual BOOL Expand( HTREEITEM hitem ) ;


// Implementation
protected:
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CStgEditStmItem command target

class CStgEditStmItem : public CStgEditItem
{
	DECLARE_DYNAMIC(CStgEditStmItem)
		
	CStgEditStmItem(CTreeCtrl* pTree, IStream* pStream);           // protected constructor used by dynamic creation
	virtual ~CStgEditStmItem();

// Attributes
public:
	IStream* m_pStream;

// Operations
public:
    virtual BOOL Expand( HTREEITEM hitem ) ;


// Implementation
protected:
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CStgEditPsetItem command target

class CStgEditPsetItem : public CStgEditStmItem
{
	DECLARE_DYNAMIC(CStgEditPsetItem)
		
	CStgEditPsetItem(CTreeCtrl* pTree, IStream* pStream);           // protected constructor used by dynamic creation
	virtual ~CStgEditPsetItem();

// Attributes
public:
	CPropertySet	m_propset ;

// Operations
public:
    virtual BOOL Expand( HTREEITEM hitem ) ;


// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CStgEditPropItem command target

class CStgEditPropItem : public CStgEditItem
{
	DECLARE_DYNAMIC(CStgEditPropItem)
		
	CStgEditPropItem(CTreeCtrl* pTree, CPropertySection* pSec, ULONG ulPropID);           // protected constructor used by dynamic creation
	virtual ~CStgEditPropItem();

// Attributes
public:
	CPropertySection* m_pSec ;
	ULONG m_ulPropID ;

// Operations
public:
    virtual BOOL Expand( HTREEITEM hitem ) ;


// Implementation
protected:
};
