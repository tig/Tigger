// ftd.cpp
//
// File Type Driver helpers and base class
//

#include "stdafx.h"
#include "winprt20.h"
#include "control.h"

// Routine to load an FTD given a ProgID or human name.
//
HRESULT ftdLoad( LPCSTR szName, LPSTREAM lpStreamInit, REFIID riid, LPVOID FAR * ppvUnk )
{
    HRESULT     hr = ResultFromScode(S_OK) ;
    LPUNKNOWN   lpUnk = NULL ;
    *ppvUnk = NULL ;
    
    // First assume szName is a ProgID
    //
    CLSID       clsid ;
    hr = CLSIDFromProgID( szName, &clsid ) ;
    if (FAILED(hr))
    {
        // BUGBUG: The file type driver name is the human readable name.  Get
        // the ProgID.
        TRACE( "File type driver name must be a ProgID for now\n" ) ;
        return hr ;
    }

    hr = CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, riid, (LPVOID FAR*)&lpUnk ) ;
    if (FAILED(hr))
    {
        TRACE( "Could not create file type driver object.\n" ) ;
        return hr ;
    }
    
    if (lpStreamInit != NULL)
    {
        // If the CLSID in the stream matches the CLSID of the FTD then
        // pass the stream to the FTD to init.  Otherwise the FTD will
        // assume it's default state.
        //
        CLSID   clsidStream ;
        hr = ReadClassStm( lpStreamInit, &clsidStream ) ;
        if (IsEqualCLSID(clsidStream, clsid ))
        {
            LPPERSISTSTREAM lpPS ;
            hr = lpUnk->QueryInterface( IID_IPersistStream, (LPVOID FAR*)&lpPS ) ;
            if (FAILED(hr))
            {
                TRACE( "File type driver does not support IPersistStream!\n" ) ;
                lpUnk->Release() ;
                return hr ;
            }
            hr = lpPS->Load( lpStreamInit ) ;
            lpPS->Release() ;
            if (FAILED(hr))
            {
                TRACE( "IPersistStream::Load() failed on FTD\n");
                lpUnk->Release() ;
                return hr ;
            }
        }
    }
    *ppvUnk = lpUnk ;
    return hr ;
}   


HRESULT ftdSave( LPSTREAM lpStreamSave, LPUNKNOWN pUnk ) 
{
    HRESULT hr = ResultFromScode(S_OK) ;
    
    LPPERSISTSTREAM lpPS ;
    hr = pUnk->QueryInterface( IID_IPersistStream, (LPVOID FAR*)&lpPS ) ;
    if (FAILED(hr))
    {
        TRACE( "File type driver does not support IPersistStream!\n" ) ;
        return hr ;
    }
    
    // Caller of IPersistStream::Save must write the clsid
    //
    hr = OleSaveToStream( lpPS, lpStreamSave ) ;
    lpPS->Release() ;
    if (FAILED(hr))
    {
        TRACE( "OleSaveToStream failed on FTD\n");
        return hr ;
    }
    
    return hr ;
}
