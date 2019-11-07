// WOSA/XRT Sample Excel 4.0 AddIn DLL
//
#include "precomp.h"
#include "framewrk.h"
#include "debug.h"
#include "clsid.h"
#include "fn.h"
#include "ddesrv.h"
#include "wrapper.h"

ASSERTDATA

HINSTANCE g_hinst ;
HWND      g_hwndXL = NULL ;
HINSTANCE g_hinstXL = NULL ;

//char buffer[20];

// rgFuncs
//
// This is a table of all the functions exported by this module.
// These functions are all registered (in xlAutoOpen) when you
// open the XLL. Before every string, leave a space for the
// byte count. The format of this table is the same as 
// the last seven arguments to the REGISTER function.
// rgFuncsRows define the number of rows in the table. The
// dimension [7] represents the number of columns in the table.
//
static LPSTR rgFuncs[][7] =
{       
    {" XRTCreateObject",       " RR",  " XRTCreateObject",       " Prog ID", " 2",
        " WOSA/XRT Functions", " "},    

    {" XRTDestroyObject",       " A",  " XRTDestroyObject",       " ", " 2",
        " WOSA/XRT Functions", " "},    

    // XRTInfo : Retrieves information about the XRT data object
    // One parameter indicates info to return:
    //
    //  "COUNT" - Number of quotes
    //
    {" XRTInfo",       " RR",  " XRTInfo",       " Item", " 1",
        " WOSA/XRT Functions", " "},    

    // XRTQuote( Ticker, Val ) : Gets quote info:
    //
    //  Ticker = ticker name (i.e. "MSFT") 
    //  Val = "High", "Low", "Ask", "Volume"
    //
    {" XRTQuote",       " RRR!",  " XRTQuote",       " Ticker, Attribute", " 1",
        " WOSA/XRT Functions", " "},    

    {" XRTOnData",       " ",  " XRTOnData",       " ", " 1",
        " WOSA/XRT Functions", " "},    

    {" XRTSetRange",       " RR",  " XRTSetRange",       " Range", " 1",
        " WOSA/XRT Functions", " "},    

    {" XRTItemName",       " RR",  " XRTItemName",       " Item", " 1",
        " WOSA/XRT Functions", " "},    

    {" XRTExit",       " A",   " XRTExit",       " ",    " 2",
        " WOSA/XRT Functions", " "},
};

#define rgFuncsRows (sizeof(rgFuncs)/sizeof(rgFuncs[0]))

//
// rgMenu
//
// This is a table describing the XRTXLL drop-down menu. It is in
// the same format as the Microsoft Excel macro language menu tables.
// The first column contains the name of the menu or command, the
// second column contains the function to be executed, the third
// column contains the (Macintosh only) shortcut key, the fourth
// column contains the help text for the status bar, and
// the fifth column contains the help text index. Leave a space
// before every string so the byte count can be inserted. rgMenuRows
// defines the number of menu items. 5 represents the number of
// columns in the table.
//

#define rgMenuRows 1
static LPSTR rgMenu[rgMenuRows][5] =
{
    {" &XRTXLL",          " ",           " ",
    " The XRTXLL XLL Add-In",         " "},
//    {" E&xit",             " fExit",      " ",
//    " Close the XRTXLL XLL",          " "},
};

// LibMain
//
// This function is called by LibEntry (in LIBENTRY.OBJ), which is called
// by Windows when the DLL is first loaded. LibEntry initializes the
// DLL's heap if a HEAPSIZE is specified in the DLL's .DEF file, and
// then calls LibMain. The following LibMain function satisfies that
// call. The LibMain function should perform additional initialization
// tasks required by the DLL. In this example, we byte-count all the strings
// in the preceding table. LibMain will be called only once, when a DLL is
// first loaded, even if that DLL is used by multiple applications.
//
// LibMain
//
// Arguments:
//
//      HANDLE hInstance    Instance handle
//      WORD wDataSeg       Library's data segment
//      WORD wHeapSize      Heap size
//      LPSTR lpszCmdLine   Command line
//
// Returns:
//
//      int                 1 if initialization is successful.
extern "C"
int FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg,
       WORD wHeapSize, LPSTR lpszCmdLine)
{

#ifdef _DEBUG
    OutputDebugString( "XRTXLL:  LibMain\r\n" ) ;
#endif
    
    // In the following four loops, the strings in rgFuncs[], rgTool[],
    // rgMenu[] and rgDialog[] are byte-counted so that they won't need
    // to be byte-counted later. In addition, the instance handle passed
    // into LibMain is saved in the global variable hInst for later use.
    //
    int i,j;

    g_hinst = hInstance;

    for (i=0; i<rgFuncsRows; i++)
    {
        for (j=0; j<7; j++)
        {
            rgFuncs[i][j][0] = (BYTE) lstrlen (rgFuncs[i][j]+1);
        }
    }

    for (i=0; i<rgMenuRows; i++)
    {
        for (j=0; j<5; j++)
        {
            rgMenu[i][j][0] = (BYTE) lstrlen (rgMenu[i][j]+1);
        }
    }

    return 1;
}


// WEP
//
// This is the DLL's termination function, which is called when the
// DLL terminates. This function should return 1. Microsoft
// QuickC for Windows provides a built-in WEP that eliminates the
// need for this routine.
//
// Arguments:
//
//      int nArgument      WEP_SYSTEMEXIT if Windows is
//                         shutting down, or
//                         WEP_FREE_DLL if this DLL is
//                         being unloaded by Windows.
//
// Returns:
//
//      int                1 (required)
extern "C"
int __export FAR PASCAL WEP (int nArgument)
{
    #ifdef _DEBUG
    OutputDebugString("XRTXLL: WEP\r\n") ;
    #endif
    
    ReallyUninitializeDDE() ;

    return 1;
}

// lpstricmp
//
// Compares two Pascal strings and checks if they are equal,
// without case sensitivity. This routine is useful for
// xlAutoRegister.
//
// Arguments:
//
//      LPSTR s     First string
//      LPSTR t     Second string
//
// Returns:
//
//      int         0 if they are equal
//                  Nonzero otherwise
//
// Note:
//
//      Unlike the usual string functions, lpstricmp
//      doesn't care about collating sequence.
int WINAPI lpstricmp(LPSTR s, LPSTR t)
{
    int i;

    if (*s != *t)
    {
    return 1;
    }

    for (i=1; i<=s[0]; i++)
    {
    if (tolower(s[i]) != tolower(t[i]))
    {
        return 1;
    }
    }

    return 0;
}

// xlAutoOpen
//
// Microsoft Excel uses xlAutoOpen to load XLL files.
// When you open an XLL file, the only action
// Microsoft Excel takes is to call the xlAutoOpen function.
//
// More specifically, xlAutoOpen is called:
//
//  - when you open this XLL file from the File menu,
//  - when this XLL is in the XLSTART directory, and is
//    automatically opened when Microsoft Excel starts,
//  - when Microsoft Excel opens this XLL for any other reason, or
//  - when a macro calls REGISTER(), with only one argument, which is the
//    name of this XLL.
//
// xlAutoOpen is also called by the Add-in Manager when you add this XLL 
// as an add-in. The Add-in Manager first calls xlAutoAdd, then calls
// REGISTER("EXAMPLE.XLL"), which in turn calls xlAutoOpen.
//
// xlAutoOpen should:
//
//  - register all the functions you want to make available while this
//    XLL is open,
//
//  - add any menus or menu items that this XLL supports,
//
//  - perform any other initialization you need, and
//
//  - return 1 if successful, or return 0 if your XLL cannot be opened.
//
extern "C"
int __export FAR PASCAL xlAutoOpen(void)
{

    static XLOPER   xDLL,             // name of this DLL //
                    xMenu,            // xltypeMulti containing the menu //
                    xTest;            // used for menu test //
    LPXLOPER pxMenu;  // Points to first menu item //
    LPXLOPER px;      // Points to the current item //
    int             i, j;             // Loop indices //
    HANDLE          hMenu;            // global memory holding menu //

    Excel4( xlGetHwnd, &xDLL, 0 ) ;
    g_hwndXL = xDLL.val.w ;

    Excel4( xlGetInst, &xDLL, 0 ) ;
    g_hinstXL = xDLL.val.w ;

    Excel(xlGetName, &xDLL, 0);


    for (i=0; i<rgFuncsRows; i++)
    {
    Excel(xlfRegister, NULL, 8,
        (LPXLOPER) &xDLL,
        (LPXLOPER) TempStr(rgFuncs[i][0]),
        (LPXLOPER) TempStr(rgFuncs[i][1]),
        (LPXLOPER) TempStr(rgFuncs[i][2]),
        (LPXLOPER) TempStr(rgFuncs[i][3]),
        (LPXLOPER) TempStr(rgFuncs[i][4]),
        (LPXLOPER) TempStr(rgFuncs[i][5]),
        (LPXLOPER) TempStr(rgFuncs[i][6]));
    }

    // In the following block of code, the XRTXLL drop-down menu is created.
    // Before creation, a check is made to determine if XRTXLL already
    // exists. If not, it is added. If the menu needs to be added, memory is
    // allocated to hold the array of menu items. The rgMenu[] table is then
    // transferred into the newly created array. The array is passed as an
    // argument to xlfAddMenu to actually add the drop-down menu before the
    // help menu. As a last step the memory allocated for the array is
    // released.
    //
    // This block uses TempStr() and TempNum(). Both create a temporary
    // XLOPER. The XLOPER created by TempStr() contains the string passed to
    // it. The XLOPER created by TempNum() contains the number passed to it.
    // The Excel() function frees the allocated temporary memory. Both
    // functions are part of the framework library.
    //
    Excel(xlfGetBar, &xTest, 3, TempInt(1), TempStr(" XRTXLL"), TempInt(0));

    if (xTest.xltype == xltypeErr)
    {
        px = pxMenu = (LPXLOPER) GlobalLock(hMenu = GlobalAlloc(GMEM_MOVEABLE,
        sizeof(XLOPER)*5*rgMenuRows));

        for (i=0; i<rgMenuRows; i++)
        {
            for (j=0; j<5; j++)
            {
            px->xltype = xltypeStr;
            px->val.str = rgMenu[i][j];
            px++;
            }
        }
    
        xMenu.xltype = xltypeMulti;
        xMenu.val.array.lparray = pxMenu;
        xMenu.val.array.rows = rgMenuRows;
        xMenu.val.array.columns = 5;
    
        Excel(xlfAddMenu,0,3,TempNum(1),(LPXLOPER)&xMenu,TempStr(" Help"));
    
        GlobalUnlock(hMenu);
        GlobalFree(hMenu);
    }
    
    // Free the XLL filename //
    Excel(xlFree, 0, 2, (LPXLOPER) &xTest, (LPXLOPER) &xDLL);


    // Now create DDE Server
    //
    if (FALSE == SetupDDEServer())
    {
        AssertSz( 0, "Could not create DDE Server" ) ;
        return 0 ;
    }

    return 1;
}


//
// xlAutoClose
//
// xlAutoClose is called by Microsoft Excel:
//
//  - when you quit Microsoft Excel, or
//  - when a macro sheet calls UNREGISTER(), giving a string argument
//    which is the name of this XLL.
//
// xlAutoClose is called by the Add-in Manager when you remove this XLL from
// the list of loaded add-ins. The Add-in Manager first calls xlAutoRemove,
// then calls UNREGISTER("EXAMPLE.XLL"), which in turn calls xlAutoClose.
//
// xlAutoClose is called by EXAMPLE.XLL by the function fExit. This function
// is called when you exit Example.
//
// xlAutoClose should:
//
//  - Remove any menus or menu items that were added in xlAutoOpen,
//
//  - do any necessary global cleanup, and
//
//  - delete any names that were added (names of exported functions, and 
//    so on). Remember that registering functions may cause names to be created.
//
// xlAutoClose does NOT have to unregister the functions that were registered
// in xlAutoOpen. This is done automatically by Microsoft Excel after
// xlAutoClose returns.
//
// xlAutoClose should return 1.
//
extern "C"
int __export FAR PASCAL xlAutoClose(void)
{
    int i;
    XLOPER xRes;

    UnInitXRTObject() ;

    KillDDEServer() ;

    // This block first deletes all names added by xlAutoOpen or
    // xlAutoRegister. Next, it checks if the drop-down menu XRTXLL still
    // exists. If it does, it is deleted using xlfDeleteMenu. It then checks
    // if the Test toolbar still exists. If it is, xlfDeleteToolbar is
    // used to delete it.
    //

    for (i=0; i<rgFuncsRows; i++)
    {
        Excel(xlfSetName, 0, 1, TempStr(rgFuncs[i][2]));
    }

    Excel(xlfGetBar, &xRes, 3, TempInt(1), TempStr(" XRTXLL"), TempInt(0));

    if (xRes.xltype != xltypeErr)
    {
        Excel(xlfDeleteMenu, 0, 2, TempNum(1), TempStr(" XRTXLL"));
    
        // Free the XLOPER returned by xlfGetBar //
        Excel(xlFree, 0, 1, (LPXLOPER) &xRes);
    }


    return 1;
}


// xlAutoRegister
//
// This function is called by Microsoft Excel if a macro sheet tries to
// register a function without specifying the type_text argument. If that
// happens, Microsoft Excel calls xlAutoRegister, passing the name of the
// function that the user tried to register. xlAutoRegister should use the
// normal REGISTER function to register the function, only this time it must
// specify the type_text argument. If xlAutoRegister does not recognize the
// function name, it should return a #VALUE! error. Otherwise, it should
// return whatever REGISTER returned.
//
// Arguments:
//
//      LPXLOPER pxName     xltypeStr containing the
//                          name of the function
//                          to be registered. This is not
//                          case sensitive.
//
// Returns:
//
//      LPXLOPER            xltypeNum containing the result
//                          of registering the function,
//                          or xltypeErr containing #VALUE!
//                          if the function could not be
//                          registered.
//
extern "C"
LPXLOPER __export FAR PASCAL xlAutoRegister(LPXLOPER pxName)
{
    static XLOPER xDLL, xRegId;
    int i;

    //
    // This block initializes xRegId to a #VALUE! error first. This is done in
    // case a function is not found to register. Next, the code loops through 
    // the functions in rgFuncs[] and uses lpstricmp to determine if the 
    // current row in rgFuncs[] represents the function that needs to be 
    // registered. When it finds the proper row, the function is registered 
    // and the register ID is returned to Microsoft Excel. If no matching 
    // function is found, an xRegId is returned containing a #VALUE! error.
    //

    xRegId.xltype = xltypeErr;
    xRegId.val.err = xlerrValue;

    for (i=0; i<rgFuncsRows; i++)
    {
        if (!lpstricmp(rgFuncs[i][0], pxName->val.str))
        {
            Excel(xlGetName, &xDLL, 0);
    
            Excel(xlfRegister, &xRegId, 8,
            (LPXLOPER) &xDLL,
            (LPXLOPER) TempStr(rgFuncs[i][0]),
            (LPXLOPER) TempStr(rgFuncs[i][1]),
            (LPXLOPER) TempStr(rgFuncs[i][2]),
            (LPXLOPER) TempStr(rgFuncs[i][3]),
            (LPXLOPER) TempStr(rgFuncs[i][4]),
            (LPXLOPER) TempStr(rgFuncs[i][5]),
            (LPXLOPER) TempStr(rgFuncs[i][6]));
    
            //* Free oper returned by xl //
            Excel(xlFree, 0, 1, (LPXLOPER) &xDLL);
    
            return (LPXLOPER) &xRegId;
        }
    }
    return (LPXLOPER) &xRegId;
}

// xlAutoAdd
//
// This function is called by the Add-in Manager only. When you add a
// DLL to the list of active add-ins, the Add-in Manager calls xlAutoAdd()
// and then opens the XLL, which in turn calls xlAutoOpen.
//
//
extern "C"
int __export FAR PASCAL xlAutoAdd(void)
{
    // Display a dialog box indicating that the XLL was successfully added //
    Excel(xlcAlert, 0, 2, TempStr(" Thank you for adding XRTXLL.XLL!"),
      TempInt(2));
    return 1;
}


//
// xlAutoRemove
//
// This function is called by the Add-in Manager only. When you remove
// an XLL from the list of active add-ins, the Add-in Manager calls
// xlAutoRemove() and then UNREGISTER("EXAMPLE.XLL").
//
// You can use this function to perform any special tasks that need to be
// performed when you remove the XLL from the Add-in Manager's list
// of active add-ins. For example, you may want to delete an
// initialization file when the XLL is removed from the list.
//
extern "C"
int __export FAR PASCAL xlAutoRemove(void)
{
    // Show a dialog box indicating that the XLL was successfully removed //
    Excel(xlcAlert, 0, 2, TempStr(" Thank you for removing XRTXLL.XLL!"),
      TempInt(2));
    return 1;
}

//
// xlAddInManagerInfo
//
//
// This function is called by the Add-in Manager to find the long name
// of the add-in. If xAction = 1, this function should return a string
// containing the long name of this XLL, which the Add-in Manager will use
// to describe this XLL. If xAction = 2 or 3, this function should return
// #VALUE!.
//
// Arguments
//
//      LPXLOPER xAction    What information you want. One of:
//                          1 = the long name of the
//                              add-in
//                          2 = reserved
//                          3 = reserved
// Return value
//
//      LPXLOPER            The long name or #VALUE!.
//
//
extern "C"
LPXLOPER __export FAR PASCAL xlAddInManagerInfo(LPXLOPER xAction)
{
    static XLOPER xInfo, xIntAction;

    //
    // This code coerces the passed-in value to an integer. This is how the
    // code determines what is being requested. If it receives a 1, 
    // it returns a string representing the long name. If it receives 
    // anything else, it returns a #VALUE! error.
    //

    Excel(xlCoerce, &xIntAction, 2, xAction, TempInt(xltypeInt));

    if(xIntAction.val.w==1)
    {
        xInfo.xltype = xltypeStr;
        xInfo.val.str = "\026XRTXLL Standalone DLL";
    }
    else
    {
        xInfo.xltype = xltypeErr;
        xInfo.val.err = xlerrValue;
    }

    return (LPXLOPER) &xInfo;
}



// fExit
//
// This is a user-initiated routine to exit XRT_XL4.XLL You may be tempted to
// simply call UNREGISTER("XRT_XL4.XLL") in this function. Don't do it! It
// will have the effect of forcefully unregistering all of the functions in
// this DLL, even if they are registered somewhere else! Instead, unregister
// the functions one at a time.
//
// Arguments:
//
//      None
//
// Returns:
//
//      int     1
//
extern "C"
int __export FAR PASCAL XRTExit(void)
{
    XLOPER  xDLL,                   /* The name of this DLL */
        xFunc,                  /* The name of the function */
        xRegId;                 /* The registration ID */
    int     i;

    /*
    // This code gets the DLL name. It then uses this along with information
    // from rgFuncs[] to obtain a REGISTER.ID() for each function. The
    // register ID is then used to unregister each function. Then the code
    // frees the DLL name and calls xlAutoClose.
    */

    /* Make xFunc a string */
    xFunc.xltype = xltypeStr;

    Excel(xlGetName, &xDLL, 0);

    for (i=0; i<rgFuncsRows; i++)
    {
        xFunc.val.str = (LPSTR) (rgFuncs[i][0]);
        Excel(xlfRegisterId,&xRegId,2,(LPXLOPER)&xDLL,(LPXLOPER)&xFunc);
        Excel(xlfUnregister, 0, 1, (LPXLOPER) &xRegId);
    }
    Excel(xlFree, 0, 1,  (LPXLOPER) &xDLL);

    return xlAutoClose();

}


