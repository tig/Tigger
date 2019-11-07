// cprop.h : header file
//

#ifndef __CPROP_H__
#define __CPROP_H__

LPSTREAM _AfxCreateMemoryStream() ;

#define DECLARE_CONTROLPROP(class_name) \
    static const GUID CDECL BASED_CODE guid; \
    virtual HRESULT GetClassID(LPCLSID pclsid);

#define IMPLEMENT_CONTROLPROP(class_name, \
            l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    const GUID CDECL BASED_CODE class_name::guid = \
        { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; \
    HRESULT class_name::GetClassID(LPCLSID pclsid) \
        { *pclsid = guid; return NOERROR; }

/////////////////////////////////////////////////////////////////////////////
// CControlProperty command target

class CControlProperty : public CCmdTarget
{   
    DECLARE_DYNAMIC(CControlProperty)
protected:
    CControlProperty();        

// Attributes
public:

// Operations
public:
    virtual void OnResetState();
    // Use the DEFINE/IMPLEMENT_CONTROLPROP macro to set
    //
    virtual HRESULT GetClassID(LPCLSID pclsid) = 0;

    BOOL GetPropsetData( LPFORMATETC lpFormatEtc, 
                    LPSTGMEDIUM lpStgMedium, REFCLSID fmtid );
    BOOL SetPropsetData( LPFORMATETC lpFormatEtc, 
                    LPSTGMEDIUM lpStgMedium, REFCLSID fmtid );
    CString& GetName() ;
    void SetName( const CString& strName ) ;
    
// Implementation
protected:
    virtual ~CControlProperty();

    virtual void DoPropExchange(CPropExchange* pPX);

    CString m_strName ;
};

BOOL PX_Property( CPropExchange* pPX, LPCSTR pszPropName, 
                  CControlProperty* pControlProp);

/////////////////////////////////////////////////////////////////////////////
    
#endif //__CPROP_H__


