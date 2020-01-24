// iadvsink.h
#ifndef _IADVSINK_H_
#define _IADVSINK_H_

class __far CImpIAdviseSink : public IAdviseSink
    {
    protected:
        ULONG               m_cRef;      //Interface reference count.

    public:
        CImpIAdviseSink(void);
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

typedef CImpIAdviseSink FAR * LPIMPIADVISESINK;

#endif

