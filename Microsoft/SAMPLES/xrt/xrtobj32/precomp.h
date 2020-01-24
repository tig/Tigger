
#ifdef WIN32

#define _UNICODE
#define UNICODE
#include <windows.h>
#include <tchar.h>
#include <wchar.h>

#else

#include <windows.h>

#endif

#include <compobj.h>
#include <ole2.h>
#include <stdlib.h>
#include "..\\wosaxrt.h"
