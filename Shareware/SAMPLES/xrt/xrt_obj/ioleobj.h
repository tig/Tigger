// IOLEOBJ.H
//
// Definitions of a template IOleObject interface implementation.
//
// Copyright (c)1993 Microsoft Corporation, All Right Reserved
//
// Kraig Brockschmidt, Software Design Engineer
// Microsoft Systems Developer Relations
//
// Internet  :  kraigb@microsoft.com
// Compuserve:  >INTERNET:kraigb@microsoft.com
//
#ifndef _IOLEOBJ_H_
#define _IOLEOBJ_H_

class __far CImpIOleObject : public IOleObject
{
    private:
        ULONG               m_cRef ;         //Interface reference count.
        LPCXRTDataObject    m_pObj ;         //Back pointer to the object.
        LPUNKNOWN           m_punkOuter ;    //Controlling unknown for delegation

    public:
        CImpIOleObject(LPCXRTDataObject, LPUNKNOWN) ;
        ~CImpIOleObject(void) ;

        //IUnknown members that delegate to m_punkOuter.
        STDMETHODIMP         QueryInterface(REFIID, LPVOID FAR *) ;
        STDMETHODIMP_(ULONG) AddRef(void) ;
        STDMETHODIMP_(ULONG) Release(void) ;

        //IOleObject members
        STDMETHODIMP SetClientSite(LPOLECLIENTSITE) ;
        STDMETHODIMP GetClientSite(LPOLECLIENTSITE FAR *) ;
        STDMETHODIMP SetHostNames(LPCSTR, LPCSTR) ;
        STDMETHODIMP Close(DWORD) ;
        STDMETHODIMP SetMoniker(DWORD, LPMONIKER) ;
        STDMETHODIMP GetMoniker(DWORD, DWORD, LPMONIKER FAR *) ;
        STDMETHODIMP InitFromData(LPDATAOBJECT, BOOL, DWORD) ;
        STDMETHODIMP GetClipboardData(DWORD, LPDATAOBJECT FAR *) ;
        STDMETHODIMP DoVerb(LONG, LPMSG, LPOLECLIENTSITE, LONG, HWND, LPCRECT) ;
        STDMETHODIMP EnumVerbs(LPENUMOLEVERB FAR *) ;
        STDMETHODIMP Update(void) ;
        STDMETHODIMP IsUpToDate(void) ;
        STDMETHODIMP GetUserClassID(CLSID FAR *) ;
        STDMETHODIMP GetUserType(DWORD, LPSTR FAR *) ;
        STDMETHODIMP SetExtent(DWORD, LPSIZEL) ;
        STDMETHODIMP GetExtent(DWORD, LPSIZEL) ;
        STDMETHODIMP Advise(LPADVISESINK, DWORD FAR *) ;
        STDMETHODIMP Unadvise(DWORD) ;
        STDMETHODIMP EnumAdvise(LPENUMSTATDATA FAR *) ;
        STDMETHODIMP GetMiscStatus(DWORD, DWORD FAR *) ;
        STDMETHODIMP SetColorScheme(LPLOGPALETTE) ;
} ;


#endif //_IOLEOBJ_H_
