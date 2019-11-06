// ID3COM.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f ID3COMps.mk in the project directory.

#include "stdafx.h"

#include "resource.h"
#include <initguid.h>
#include "ID3COM.h"

#include "ID3COM_i.c"
#include "ID3Frames.h"
#include "ID3Fields.h"
#include "ID3Tag.h"
#include "ID3Frame.h"
#include "ID3Field.h"
#include "ID3VersInfo.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_ID3Tag, CID3Tag)
OBJECT_ENTRY(CLSID_ID3Frame, CID3Frame)
//OBJECT_ENTRY(CLSID_ID3Field, CID3Field)
//OBJECT_ENTRY(CLSID_ID3VersInfo, CID3VersInfo)
//OBJECT_ENTRY(CLSID_ID3Frames, CID3Frames)
//OBJECT_ENTRY(CLSID_ID3Fields, CID3Fields)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point


extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_ID3COM);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}


