#include <ctype.h>
#include <windows.h>
#include <ole2.h>
#include <compobj.h>

#include "clsid.h"
#include "wosaxrt.h"

#include <xlcall.h>
#include "framewrk.h"
#include "generic.h"   
#include "debug.h"

ASSERTDATA

HWND hInst, hWndMain;
char buffer[20];

/*
** rgFuncs
**
** This is a table of all the functions exported by this module.
** These functions are all registered (in xlAutoOpen) when you
** open the XLL. Before every string, leave a space for the
** byte count. The format of this table is the same as 
** the last seven arguments to the REGISTER function.
** rgFuncsRows define the number of rows in the table. The
** dimension [7] represents the number of columns in the table.
*/
#define rgFuncsRows 6

static LPSTR rgFuncs[rgFuncsRows][7] =
{
    {" Func1",       " RR",  " Func1",       " Arg", " 1",
        " XRT_XL4 Add-In", " "},
    {" FuncSum",     " RRRRRRRRRRRRRRRRRRRRRRRRRRRRRR", /* up to 29 args*/
        " FuncSum", " number1,number2,...", " 1", " XRT_XL4 Add-In", " "},
    {" fDialog",     " A",   " fDialog",     " ",    " 2",
        " XRT_XL4 Add-In", " l"},
    {" fDance",      " A",   " fDance",      " ",    " 2",
        " XRT_XL4 Add-In", " m"},
    {" fShowDialog", " A",   " fShowDialog", " ",    " 2",
        " XRT_XL4 Add-In", " n"},
    {" fExit",       " A",   " fExit",       " ",    " 2",
        " XRT_XL4 Add-In", " o"},
};


/*
** rgMenu
**
** This is a table describing the XRT_XL4 drop-down menu. It is in
** the same format as the Microsoft Excel macro language menu tables.
** The first column contains the name of the menu or command, the
** second column contains the function to be executed, the third
** column contains the (Macintosh only) shortcut key, the fourth
** column contains the help text for the status bar, and
** the fifth column contains the help text index. Leave a space
** before every string so the byte count can be inserted. rgMenuRows
** defines the number of menu items. 5 represents the number of
** columns in the table.
*/

#define rgMenuRows 5


static LPSTR rgMenu[rgMenuRows][5] =
{
    {" &XRT_XL4",          " ",           " ",
    " The XRT_XL4 XLL Add-In",         " "},
    {" &Dialog...",        " fDialog",    " ",
    " Run a sample XRT_XL4 dialog",    " "},
    {" D&ance",            " fDance",     " ",
    " Make the selection dance around"," "},
    {" &Native Dialog...", " fShowDialog"," ",
    " Run a sample native dialog",     " "},
    {" E&xit",             " fExit",      " ",
    " Close the XRT_XL4 XLL",          " "},
};

/*
** rgTool
**
** This is a table describing the toolbar. It is in the same format
** as the Microsoft Excel macro language toolbar tables. The first column
** contains the ID of the tool, the second column contains the function
** to be executed, the third column contains a logical value specifying
** the default image of the tool, the fourth column contains a logical
** value specifying whether the tool can be used, the fifth column contains
** a face for the tool, the sixth column contains the help_text that
** is displayed in the status bar, the seventh column contains the Balloon
** text (Macintosh Only), and the eighth column contains the help topics
** as quoted text. Leave a space before every string so the byte count
** can be inserted. rgToolRows defines the number of tools on the toolbar.
** 8 represents the number of columns in the table.
*/


#define rgToolRows 3


static LPSTR rgTool[rgToolRows][8] =
{
    {" 211", " fDance", " ", " TRUE", " ", " Dance the Selection", " ", " "},
    {" 0",   " ",       " ", " ",     " ", " ",                    " ", " "},
    {" 212", " fExit",  " ", " TRUE", " ", " Exit this example",   " ", " "},
};


/*
** rgDialog
**
** This is a table describing the sample dialog box used in the fDialog()
** function. Admittedly, it would be more efficient to use ints for
** the first 5 columns, but that makes the code much more complicated.
** Storing the text in string tables is another method that could be used.
** Leave a space before every string so the byte count can be inserted.
** rgDialogRows determines the number of rows in the dialog box.
** 7 represents the number of columns in the table.
*/


#define rgDialogRows 16

static LPSTR rgDialog[rgDialogRows][7] =
{
    {" ",   " ",    " ",    " 494", " 210", " XRT_XL4 Sample Dialog", " "},
    {" 1",  " 330", " 174", " 88",  " ",    " OK",                    " "},
    {" 2",  " 225", " 174", " 88",  " ",    " Cancel",                " "},
    {" 5",  " 19",  " 11",  " ",    " ",    " &Name:",                " "},
    {" 6",  " 19",  " 29",  " 251", " ",    " ",                      " "},
    {" 14", " 305", " 15",  " 154", " 73",  " &College",              " "},
    {" 11", " ",    " ",    " ",    " ",    " ",                      " 1"},
    {" 12", " ",    " ",    " ",    " ",    " &Harvard",              " 1"},
    {" 12", " ",    " ",    " ",    " ",    " &Other",                " "},
    {" 5",  " 19",  " 50",  " ",    " ",    " &Reference:",           " "},
    {" 10", " 19",  " 67",  " 253", " ",    " ",                      " "},
    {" 14", " 209", " 93",  " 250", " 63",  " &Qualifications",       " "},
    {" 13", " ",    " ",    " ",    " ",    " &BA / BS",              " 1"},
    {" 13", " ",    " ",    " ",    " ",    " &MA / MS",              " 1"},
    {" 13", " ",    " ",    " ",    " ",    " &PhD / Other Grad",     " 0"},
    {" 15", " 19",  " 99",  " 160", " 96",  " XRT_XL4_List1",         " 1"},
};

/*
/*
** LibMain
**
** This function is called by LibEntry (in LIBENTRY.OBJ), which is called
** by Windows when the DLL is first loaded. LibEntry initializes the
** DLL's heap if a HEAPSIZE is specified in the DLL's .DEF file, and
** then calls LibMain. The following LibMain function satisfies that
** call. The LibMain function should perform additional initialization
** tasks required by the DLL. In this example, we byte-count all the strings
** in the preceding table. LibMain will be called only once, when a DLL is
** first loaded, even if that DLL is used by multiple applications.
**
** LibMain
**
** Arguments:
**
**      HANDLE hInstance    Instance handle
**      WORD wDataSeg       Library's data segment
**      WORD wHeapSize      Heap size
**      LPSTR lpszCmdLine   Command line
**
** Returns:
**
**      int                 1 if initialization is successful.
*/

int FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg,
       WORD wHeapSize, LPSTR lpszCmdLine)
{

    /*
    ** In the following four loops, the strings in rgFuncs[], rgTool[],
    ** rgMenu[] and rgDialog[] are byte-counted so that they won't need
    ** to be byte-counted later. In addition, the instance handle passed
    ** into LibMain is saved in the global variable hInst for later use.
    */

    int i,j;

    hInst = hInstance;

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

    for (i=0; i<rgDialogRows; i++)
    {
        for (j=0; j<7; j++)
        {
            rgDialog[i][j][0] = (BYTE) lstrlen (rgDialog[i][j]+1);
        }
    }

    for (i=0; i<rgToolRows; i++)
    {
        for (j=0; j<8; j++)
        {   
            rgTool[i][j][0] = (BYTE) lstrlen (rgTool[i][j]+1);
        }
    }

    return 1;
}


/*
** WEP
**
** This is the DLL's termination function, which is called when the
** DLL terminates. This function should return 1. Microsoft
** QuickC for Windows provides a built-in WEP that eliminates the
** need for this routine.
**
** Arguments:
**
**      int nArgument      WEP_SYSTEMEXIT if Windows is
**                         shutting down, or
**                         WEP_FREE_DLL if this DLL is
**                         being unloaded by Windows.
**
** Returns:
**
**      int                1 (required)
*/

int __export FAR PASCAL WEP (int nArgument)
{
    return 1;
}


/*
** lpstricmp
**
** Compares two Pascal strings and checks if they are equal,
** without case sensitivity. This routine is useful for
** xlAutoRegister.
**
** Arguments:
**
**      LPSTR s     First string
**      LPSTR t     Second string
**
** Returns:
**
**      int         0 if they are equal
**                  Nonzero otherwise
**
** Note:
**
**      Unlike the usual string functions, lpstricmp
**      doesn't care about collating sequence.
*/

int PASCAL lpstricmp(LPSTR s, LPSTR t)
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


/*
** xlAutoOpen
**
** Microsoft Excel uses xlAutoOpen to load XLL files.
** When you open an XLL file, the only action
** Microsoft Excel takes is to call the xlAutoOpen function.
**
** More specifically, xlAutoOpen is called:
**
**  - when you open this XLL file from the File menu,
**  - when this XLL is in the XLSTART directory, and is
**    automatically opened when Microsoft Excel starts,
**  - when Microsoft Excel opens this XLL for any other reason, or
**  - when a macro calls REGISTER(), with only one argument, which is the
**    name of this XLL.
**
** xlAutoOpen is also called by the Add-in Manager when you add this XLL 
** as an add-in. The Add-in Manager first calls xlAutoAdd, then calls
** REGISTER("EXAMPLE.XLL"), which in turn calls xlAutoOpen.
**
** xlAutoOpen should:
**
**  - register all the functions you want to make available while this
**    XLL is open,
**
**  - add any menus or menu items that this XLL supports,
**
**  - perform any other initialization you need, and
**
**  - return 1 if successful, or return 0 if your XLL cannot be opened.
*/

int __export FAR PASCAL xlAutoOpen(void)
{

    static XLOPER   xDLL,             /* name of this DLL */
            xMenu,            /* xltypeMulti containing the menu */
            xTool,            /* xltypeMulti containing the toolbar */
            xTest;            /* used for menu test */
            LPXLOPER pxMenu;  /* Points to first menu item */
            LPXLOPER px;      /* Points to the current item */
            LPXLOPER pxTool;  /* Points to first toolbar item */
    int             i, j;             /* Loop indices */
    HANDLE          hMenu;            /* global memory holding menu */
    HANDLE          hTool;            /* global memory holding toolbar */

    /*
    ** In the following block of code the name of the XLL is obtained by
    ** calling xlGetName. This name is used as the first argument to the
    ** REGISTER function to specify the name of the XLL. Next, the XLL loops
    ** through the rgFuncs[] table, registering each function in the table
    ** using xlfRegister. Functions must be registered before you can add 
    ** a menu item.
    */

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

    /*
    ** In the following block of code, the XRT_XL4 drop-down menu is created.
    ** Before creation, a check is made to determine if XRT_XL4 already
    ** exists. If not, it is added. If the menu needs to be added, memory is
    ** allocated to hold the array of menu items. The rgMenu[] table is then
    ** transferred into the newly created array. The array is passed as an
    ** argument to xlfAddMenu to actually add the drop-down menu before the
    ** help menu. As a last step the memory allocated for the array is
    ** released.
    **
    ** This block uses TempStr() and TempNum(). Both create a temporary
    ** XLOPER. The XLOPER created by TempStr() contains the string passed to
    ** it. The XLOPER created by TempNum() contains the number passed to it.
    ** The Excel() function frees the allocated temporary memory. Both
    ** functions are part of the framework library.
    */

    Excel(xlfGetBar, &xTest, 3, TempInt(1), TempStr(" XRT_XL4"), TempInt(0));

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

    /*
    ** In this block of code, the Test toolbar is created. Before
    ** creation, a check is made to ensure that Test doesn't already
    ** exist. If it does not, it is created. Memory is allocated to hold
    ** the array containing the toolbar. The information from the rgTool[]
    ** table is then transferred into this array. The toolbar is added with
    ** xlfAddToolbar and subsequently displayed with xlcShowToolbar. Finally,
    ** the memory allocated for the toolbar and the XLL filename is released.
    **
    ** This block uses TempInt(), TempBool(), and TempMissing(). All three
    ** create a temporary XLOPER. The XLOPER created by TempInt() contains
    ** the integer passed to it. TempBool() creates an XLOPER containing the
    ** boolean value passed to it. TempMissing() creates an XLOPER that
    ** simulates a missing argument. The Excel() function frees the temporary
    ** memory associated with these functions. All three are part of the
    ** framework library.
    */

    Excel(xlfGetToolbar, &xTest, 2, TempInt(1), TempStr(" Test"));

    if (xTest.xltype == xltypeErr)
    {

    px = pxTool = (LPXLOPER) GlobalLock(hTool = GlobalAlloc(GMEM_MOVEABLE,
        sizeof(XLOPER)*8*rgToolRows));

    for (i=0; i<rgToolRows; i++)
    {
        for (j=0; j<8; j++)
        {
        px->xltype = xltypeStr;
        px->val.str = rgTool[i][j];
        px++;
        }
    }

    xTool.xltype = xltypeMulti;
    xTool.val.array.lparray = pxTool;
    xTool.val.array.rows = rgToolRows;
    xTool.val.array.columns = 8;

    Excel(xlfAddToolbar,0,2,TempStr(" Test"),(LPXLOPER)&xTool);

    Excel(xlcShowToolbar, 0, 6, TempStr(" Test"), TempBool(1),
        TempInt(5), TempMissing(), TempMissing(), TempInt(999));

    GlobalUnlock(hTool);
    GlobalFree(hTool);
    }

    /* Free the XLL filename */
    Excel(xlFree, 0, 2, (LPXLOPER) &xTest, (LPXLOPER) &xDLL);

    return 1;
}


/*
** xlAutoClose
**
** xlAutoClose is called by Microsoft Excel:
**
**  - when you quit Microsoft Excel, or
**  - when a macro sheet calls UNREGISTER(), giving a string argument
**    which is the name of this XLL.
**
** xlAutoClose is called by the Add-in Manager when you remove this XLL from
** the list of loaded add-ins. The Add-in Manager first calls xlAutoRemove,
** then calls UNREGISTER("EXAMPLE.XLL"), which in turn calls xlAutoClose.
**
** xlAutoClose is called by EXAMPLE.XLL by the function fExit. This function
** is called when you exit Example.
**
** xlAutoClose should:
**
**  - Remove any menus or menu items that were added in xlAutoOpen,
**
**  - do any necessary global cleanup, and
**
**  - delete any names that were added (names of exported functions, and 
**    so on). Remember that registering functions may cause names to be created.
**
** xlAutoClose does NOT have to unregister the functions that were registered
** in xlAutoOpen. This is done automatically by Microsoft Excel after
** xlAutoClose returns.
**
** xlAutoClose should return 1.
*/

int __export FAR PASCAL xlAutoClose(void)
{
    int i;
    XLOPER xRes;

    /*
    ** This block first deletes all names added by xlAutoOpen or
    ** xlAutoRegister. Next, it checks if the drop-down menu XRT_XL4 still
    ** exists. If it does, it is deleted using xlfDeleteMenu. It then checks
    ** if the Test toolbar still exists. If it is, xlfDeleteToolbar is
    ** used to delete it.
    */

    for (i=0; i<rgFuncsRows; i++)
    {
    Excel(xlfSetName, 0, 1, TempStr(rgFuncs[i][2]));
    }

    Excel(xlfGetBar, &xRes, 3, TempInt(1), TempStr(" XRT_XL4"), TempInt(0));

    if (xRes.xltype != xltypeErr)
    {
    Excel(xlfDeleteMenu, 0, 2, TempNum(1), TempStr(" XRT_XL4"));

    /* Free the XLOPER returned by xlfGetBar */
    Excel(xlFree, 0, 1, (LPXLOPER) &xRes);
    }

    Excel(xlfGetToolbar, &xRes, 2, TempInt(1), TempStr(" Test"));

    if (xRes.xltype != xltypeErr)
    {
    Excel(xlfDeleteToolbar, 0, 1, TempStr(" Test"));
    
    /* Free the XLOPER returned by xlfGetToolbar */
    Excel(xlFree, 0, 1, (LPXLOPER) &xRes);
    }

    return 1;
}



/*
** xlAutoRegister
**
** This function is called by Microsoft Excel if a macro sheet tries to
** register a function without specifying the type_text argument. If that
** happens, Microsoft Excel calls xlAutoRegister, passing the name of the
** function that the user tried to register. xlAutoRegister should use the
** normal REGISTER function to register the function, only this time it must
** specify the type_text argument. If xlAutoRegister does not recognize the
** function name, it should return a #VALUE! error. Otherwise, it should
** return whatever REGISTER returned.
**
** Arguments:
**
**      LPXLOPER pxName     xltypeStr containing the
**                          name of the function
**                          to be registered. This is not
**                          case sensitive.
**
** Returns:
**
**      LPXLOPER            xltypeNum containing the result
**                          of registering the function,
**                          or xltypeErr containing #VALUE!
**                          if the function could not be
**                          registered.
*/

LPXLOPER __export FAR PASCAL xlAutoRegister(LPXLOPER pxName)
{
    static XLOPER xDLL, xRegId;
    int i;

    /*
    ** This block initializes xRegId to a #VALUE! error first. This is done in
    ** case a function is not found to register. Next, the code loops through 
    ** the functions in rgFuncs[] and uses lpstricmp to determine if the 
    ** current row in rgFuncs[] represents the function that needs to be 
    ** registered. When it finds the proper row, the function is registered 
    ** and the register ID is returned to Microsoft Excel. If no matching 
    ** function is found, an xRegId is returned containing a #VALUE! error.
    */

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

        /** Free oper returned by xl */
        Excel(xlFree, 0, 1, (LPXLOPER) &xDLL);

        return (LPXLOPER) &xRegId;
    }
    }


    return (LPXLOPER) &xRegId;
}


/*
** xlAutoAdd
**
** This function is called by the Add-in Manager only. When you add a
** DLL to the list of active add-ins, the Add-in Manager calls xlAutoAdd()
** and then opens the XLL, which in turn calls xlAutoOpen.
**
*/

int __export FAR PASCAL xlAutoAdd(void)
{
    /* Display a dialog box indicating that the XLL was successfully added */
    Excel(xlcAlert, 0, 2, TempStr(" Thank you for adding XRT_XL4.XLL!"),
      TempInt(2));
    return 1;
}


/*
** xlAutoRemove
**
** This function is called by the Add-in Manager only. When you remove
** an XLL from the list of active add-ins, the Add-in Manager calls
** xlAutoRemove() and then UNREGISTER("EXAMPLE.XLL").
**
** You can use this function to perform any special tasks that need to be
** performed when you remove the XLL from the Add-in Manager's list
** of active add-ins. For example, you may want to delete an
** initialization file when the XLL is removed from the list.
*/

int __export FAR PASCAL xlAutoRemove(void)
{
    /* Show a dialog box indicating that the XLL was successfully removed */
    Excel(xlcAlert, 0, 2, TempStr(" Thank you for removing XRT_XL4.XLL!"),
      TempInt(2));
    return 1;
}


/*
/* xlAddInManagerInfo
**
**
** This function is called by the Add-in Manager to find the long name
** of the add-in. If xAction = 1, this function should return a string
** containing the long name of this XLL, which the Add-in Manager will use
** to describe this XLL. If xAction = 2 or 3, this function should return
** #VALUE!.
**
** Arguments
**
**      LPXLOPER xAction    What information you want. One of:
**                          1 = the long name of the
**                              add-in
**                          2 = reserved
**                          3 = reserved
** Return value
**
**      LPXLOPER            The long name or #VALUE!.
**
*/

LPXLOPER __export FAR PASCAL xlAddInManagerInfo(LPXLOPER xAction)
{
    static XLOPER xInfo, xIntAction;

    /*
    ** This code coerces the passed-in value to an integer. This is how the
    ** code determines what is being requested. If it receives a 1, 
    ** it returns a string representing the long name. If it receives 
    ** anything else, it returns a #VALUE! error.
    */

    Excel(xlCoerce, &xIntAction, 2, xAction, TempInt(xltypeInt));

    if(xIntAction.val.w==1)
    {
    xInfo.xltype = xltypeStr;
    xInfo.val.str = "\026XRT_XL4 Standalone DLL";
    }
    else
    {
    xInfo.xltype = xltypeErr;
    xInfo.val.err = xlerrValue;
    }

    return (LPXLOPER) &xInfo;
}


/*
** DIALOGMsgProc
**
** This procedure is associated with the native Windows dialog box that
** fShowDialog displays. It provides the service routines for the events
** (messages) that occur when the user operates one of the dialog
** box's buttons, entry fields, or controls.
**
** Arguments
**
**      HWND hWndDlg        Contains the HWND of the dialog box
**      WORD message        The message to respond to
**      WORD wParam         Arguments passed by Windows
**      LONG lParam
**
** Returns
**
**      BOOL                TRUE if message processed, FALSE if not.
**
*/

BOOL __export FAR PASCAL DIALOGMsgProc(HWND hWndDlg, WORD message, WORD wParam,
          LONG lParam)
{

 /*
 ** This block is a very simple message loop for a dialog box. It checks for
 ** only three messages. When it receives WM_INITDIALOG, it uses a buffer to
 ** set a static text item to the amount of free space on the stack. When it
 ** receives WM_CLOSE it posts a CANCEL message to the dialog box. When it
 ** receives WM_COMMAND it checks if the OK button was pressed. If it was,
 ** the dialog box is closed and control returned to fShowDialog.
 */

    switch(message)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hWndDlg, FREE_SPACE, (LPSTR)buffer);
        break;


    case WM_CLOSE:
        PostMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0L);
        break;

    case WM_COMMAND:
        switch(wParam)
        {
            case IDOK:
            EndDialog(hWndDlg, FALSE);
            break;
        }
        break;

    default:
        return FALSE;
    }
    
    return TRUE;
}





/*
**  ExcelCursorProc
**
**  When a modal dialog box is displayed over Microsoft Excel's window, the
**  cursor is a busy cursor over Microsoft Excel's window. This WndProc traps
**  WM_SETCURSORs and changes the cursor back to a normal arrow.
**
**  Arguments
**      HWND hWnd       hWnd of Window
**      WORD wMsg       Message to respond to
**      WORD wParam     Arguments to message
**      LONG lParam
**
**  Returns
**      LONG            0 if message handled, otherwise the result of the
**                      default WndProc
**
*/

/* Create a place to store Microsoft Excel's WndProc address */
static FARPROC lpfnExcelWndProc = NULL;

extern LONG __export FAR PASCAL ExcelCursorProc(HWND hwnd,   WORD wMsg,
               WORD wParam, LONG lParam)
{
  /*
  ** This block checks to see if the message that was passed in is a
  ** WM_SETCURSOR message. If so, the cursor is set to an arrow; if not,
  ** the default WndProc is called.
  */

    if (wMsg == WM_SETCURSOR)
    {
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    return 0L;
    }
    else
    {
    return CallWindowProc(lpfnExcelWndProc, hwnd, wMsg, wParam, lParam);
    }
}

/*
** HookExcelWindow
**
** This is the function that installs ExcelCursorProc so that it is
** called before Microsoft Excel's main WndProc.
**
** Arguments
**
**      HANDLE hWndExcel        This is a handle to Microsoft Excel's hWnd
**
** Returns
**      void
**
*/
extern void FAR PASCAL HookExcelWindow(HANDLE hWndExcel)
{
    /*
    ** This block obtains the address of Microsoft Excel's WndProc through the
    ** use of GetWindowLong(). It stores this value in a global that can be
    ** used to call the default WndProc and also to restore it. Finally, it
    ** replaces this address with the address of ExcelCursorProc using
    ** SetWindowLong().
    */

    lpfnExcelWndProc = (FARPROC) GetWindowLong(hWndExcel, GWL_WNDPROC);
    SetWindowLong(hWndExcel, GWL_WNDPROC, (LONG)(FARPROC) ExcelCursorProc);
}

/*
** UnhookExcelWindow
**
** This is the function that removes the ExcelCursorProc that was
** called before Microsoft Excel's main WndProc.
**
** Arguments
**
**      HANDLE hWndExcel        This is a handle to Microsoft Excel's hWnd
**
** Returns
**      void
**
*/

extern void FAR PASCAL UnhookExcelWindow(HANDLE hWndExcel)
{
    /*
    ** This function restores Microsoft Excel's default WndProc using
    ** SetWindowLong to restore the address that was saved into
    ** lpfnExcelWndProc by HookExcelWindow(). It then sets lpfnExcelWndProc
    ** to NULL.
    */

    SetWindowLong(hWndExcel, GWL_WNDPROC, (LONG) lpfnExcelWndProc);
    lpfnExcelWndProc = NULL;
}



/* fShowDialog
**
** This function loads and shows the native Windows dialog box.
**
** Arguments
**
**      void
**
** Returns
**
**      int 0                   To indicate successful completion
**
*/
int __export FAR PASCAL fShowDialog(void)

{
    static  XLOPER xRes, xStr;


    int     i,nRc=0;              /* return code */

    /*
    ** This block first gets the amount of space left on the stack, coerces it
    ** to a string, and then copies it into a buffer. It obtains this
    ** information so that it can be displayed in the dialog box. The coerced
    ** string is then released with xlFree. The buffer is then converted into
    ** a null-terminated string. The hWnd of Microsoft Excel is then obtained
    ** using xlGetHwnd. Messages are enabled using xlEnableXLMsgs in
    ** preparation for showing the dialog box. hWndMain is passed to
    ** HookExcelWindow so that an arrow cursor is displayed over Microsoft
    ** Excel's window. The dialog box is then displayed. The hWnd of Microsoft
    ** Excel is then obtained. It is obtained a second time in case there
    ** are multiple instances of Microsoft Excel using this XLL. hWndMain may
    ** have changed while the dialog box was displayed. If the wrong hWnd is
    ** passed to UnhookExcelWindow(), Microsoft Excel will most likely crash.
    ** The obtained hWnd is then passed to UnhookExcelWindow(). Messages are
    ** then disabled using xlDisableXLMsgs.
    */

    Excel(xlStack, (LPXLOPER) &xRes, 0);

    Excel(xlCoerce, (LPXLOPER) &xStr, 2, (LPXLOPER) &xRes,
      (LPXLOPER) TempInt(xltypeStr));

    lstrcpy(buffer, xStr.val.str);

    Excel(xlFree, 0, 1, (LPXLOPER) &xStr);

    nRc = buffer[0];
    for(i=0;i<=nRc;i++)
    buffer[i] = buffer[i+1];
    buffer[nRc] = '\0';

    Excel(xlGetHwnd, (LPXLOPER) &xRes, 0);
    hWndMain = xRes.val.w;

    Excel(xlEnableXLMsgs, 0, 0);
    HookExcelWindow(hWndMain);

    nRc = DialogBox(hInst, "TestStack", hWndMain, DIALOGMsgProc);

    UnhookExcelWindow(hWndMain);
    Excel(xlDisableXLMsgs, 0, 0);

    return(0);
}


/*
** Func1
**
** This is a typical user-defined function provided by an XLL.
**
** Arguments:
**
**      LPXLOPER x      (Ignored)
**
** Returns:
**
**      LPXLOPER        Always the string "Func1"
*/

LPXLOPER __export FAR PASCAL Func1 (LPXLOPER x)
{
    static XLOPER xResult; 
    static char szBuf[256] ;
    HRESULT hr ;
    LPCLASSFACTORY  pClassFactory ; 
    LPUNKNOWN       pUnknown ;
    LPDATAOBJECT    pDataObject ;

    if (x->xltype != xltypeStr)
    {    
        xResult.xltype = xltypeErr ;
        xResult.val.err = xlerrValue ;
        return (LPXLOPER) &xResult ;
    }

    if (FAILED(CoInitialize(NULL)))
    {
        Assert(0) ;
        return FALSE;
    }
  
    xResult.xltype = xltypeErr ;  
    xResult.val.err = xlerrNum ;

    hr = CoGetClassObject( &CLSID_XRTDataObject, CLSCTX_LOCAL_SERVER, NULL, &IID_IClassFactory,
                           (LPVOID FAR *)&pClassFactory ) ;
                 
    if (SUCCEEDED(hr))         
    { 
        // Create an instance of the object (Instance) and store
        // it in m_pIUnknown
        //
        hr = pClassFactory->CreateInstance( NULL, &IID_IUnknown, 
                            (LPVOID FAR *)&pUnknown ) ;
        if (SUCCEEDED(hr))         
        {
            hr = pIUnknown->QueryInterface( &IID_DataObject, (LPVOID FAR*)&pDataObject ) ;
            if (SUCCEEDED(hr)
            {
                FORMATETC   fetc ;
                STGMEDIUM   stm ;

                UINT xrtCF_XRTSTOCKS = RegisterClipboardFormat( "xrtCF_XRTSTOCKS" ) ;
                m_fetc.cfFormat = xrtCF_XRTSTOCKS ;
                m_fetc.dwAspect = DVASPECT_CONTENT ;
                m_fetc.ptd = NULL ;
                m_fetc.tymed = TYMED_HGLOBAL ;
                m_fetc.lindex = - 1 ;
                
                stm.tymed = TYMED_HGLOBAL ;
                hr = pDataObject->GetData( &fetc, &stm ) ;
                if (SUCCEEDED(hr))
                {
                    if (m_fetc.cfFormat == g_xrtCF_XRTSTOCKS && stm.tymed & TYMED_HGLOBAL)
                    {         
                        UINT n ;
                        
                        LPXRTSTOCKS lpStocks = (LPXRTSTOCKS)GlobalLock( stm.hGlobal ) ;
                        LPXRTSTOCK lpQ ;

                        AssertSz( lpStocks, "lpStocks is NULL" ) ;                        

                        if (lpstricmp( x->str, "\005COUNT" ) == 0)
                        {
                            xResult.xltype = xltypeInt ;
                            xResult.val.w = lpStocks.cStocks ;
                        }
                        else 
                        {       
                            BOOL fFound = FALSE ;
                            for (n = 0 ; n < lpStocks->cStocks ; n++)                        
                            {
                                wsprintf( szBuf, "%d%s", lstrlen( lpStocks->rgStocks[n].szSymbol ), 
                                            (LPSTR)lpStocks->rgStocks[n].szSymbol ) ;                                
                                if (lstrcmpi( x->str, szBuf ) == 0)
                                {
                                    double d = lpStocks->rgStocks[n].uiAsk ;
                                    fFound = TRUE ;
                                    xResult.xltype = xltypeNum ;
                                    xResult.val.num = d / 1000 ;
                                    break ;
                                }
                            }
                            if (fFound == FALSE)
                            {
                                xResult.xltype = xltypeErr ;
                                xResult.val.err = xlerrNull ;
                            }
                        }
                        GlobalUnlock( stm.hGlobal ) ;
                        ReleaseStgMedium( &stm );
                    }
                    
                    pDataObject->Release() ;
                }
                else
                {
                    AssertSz( 0, "pDataObject->GetData Failed" ) ;
                }
                pDataObject->Release() ;
            }
            else
            {
                AssertSz( 0, "pUnknown->QueryInterface Failed" ) ;
            }        
            pUnknown->Release() ;
        }
        else
        {
            AssertSz( 0, "pClassFactory->CreateInstance Failed" ) ;
        }
        pClassFactory->Release() ;
    }
    else
    {
        AssertSz( 0, "CoGetClassObject Failed" ) ;
    }
    
    CoUninitialize() ;      
    
    return (LPXLOPER) &xResult;
}


/*
** FuncSum
**
** This is a typical user-defined function provided by an XLL. This
** function takes 1-29 arguments and computes their sum. Each argument
** can be a single number, a range, or an array.
**
** Arguments:
**
**      LPXLOPER ...    1 to 29 arguments
**                                              (can be references
**                                              or values)
**
** Returns:
**
**      LPXLOPER        The sum of the arguments
**                                              or #VALUE! if there are
**                                              nonnumerics in the
arguments
**
*/

LPXLOPER __export FAR PASCAL FuncSum (x,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,
           x15,x16,x17,x18,x19,x20,x21,x22,x23,x24,x25,x26,x27,x28,x29)
    LPXLOPER x,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,
        x15,x16,x17,x18,x19,x20,x21,x22,x23,x24,x25,x26,x27,x28,x29;
{
    static XLOPER xResult;  /* Return value */
    double d=0;             /* Accumulate result */
    int iArg;               /* The argument being processed */
    LPXLOPER FAR *ppxArg;   /* Pointer to the argument being processed */
    XLOPER xMulti;          /* Argument coerced to xltypeMulti */
    unsigned i;             /* Row and column counters for arrays */
    LPXLOPER px;            /* Pointer into array */
    int error = -1;         /* -1 if no error; error code otherwise */


    /*
    ** This block accumulates the arguments passed in. Because FuncSum is a
    ** Pascal function, the arguments will be evaluated right to left. For
    ** each argument, this code checks the type of the argument and then does
    ** things necessary to accumulate that argument type. Unless the caller
    ** actually specified all 29 arguments, there will be some missing
    ** arguments. The first case handles this. The second case handles
    ** arguments that are numbers. This case just adds the number to the
    ** accumulator. The third case handles references or arrays. It coerces
    ** references to an array. It then loops through the array, switching on
    ** XLOPER types and adding each number to the accumulator. The fourth
    ** case handles being passed an error. If this happens, the error is
    ** stored in error. The fifth and default case handles being passed
    ** something odd, in which case an error is set. Finally, a check is made
    ** to see if error was ever set. If so, an error of the same type is
    ** returned; if not, the accumulator value is returned.
    */

    for (iArg = 0; iArg < 29; iArg++)
    {
    ppxArg = &x - iArg;

    switch ((*ppxArg)->xltype)
    {
        case xltypeMissing:
        break;

        case xltypeNum:
        d += (*ppxArg)->val.num;
        break;

        case xltypeRef:
        case xltypeSRef:
        case xltypeMulti:

                if (xlretUncalced == Excel(xlCoerce, &xMulti, 2,
            (LPXLOPER) *ppxArg, TempInt(xltypeMulti)))
                {
                    /*
                    ** That coerce might have failed due to an 
                    ** uncalced cell, in which case, we need to 
                    ** return immediately. Microsoft Excel will
                    ** call us again in a moment after that cell
                    ** has been calced.
                    */

                    return 0;
                }
                
        for (i = 0;
             i < (xMulti.val.array.rows * xMulti.val.array.columns);
             i++)
        {

            /* obtain a pointer to the current item */
            px = xMulti.val.array.lparray + i;

            /* switch on XLOPER type */
            switch (px->xltype)
            {

            /* if a num accumulate it */
            case xltypeNum:
                d += px->val.num;
                break;

            /* if an error store in error */
            case xltypeErr:
                error = px->val.err;
                break;

            /* if missing do nothing */
            case xltypeNil:
                break;

            /* if anything else set error */
            default:
                error = xlerrValue;
                break;
            }

        }

        /* free the returned array */
        Excel(xlFree, 0, 1, (LPXLOPER) &xMulti);
        break;

        case xltypeErr:
        error = (*ppxArg)->val.err;
        break;

        default:
        error = xlerrValue;
        break;
    }

    }
    if (error != -1)
    {
    xResult.xltype = xltypeErr;
    xResult.val.err = error;
    }
    else
    {
    xResult.xltype = xltypeNum;
    xResult.val.num = d;
    }

    return (LPXLOPER) &xResult;
}


/*
** fDance
**
** This is an example of a lengthy operation. It calls the function xlAbort
** occasionally. This yields the processor (allowing cooperative 
** multitasking), and checks if the user has pressed ESC to abort this
** operation. If so, it offers the user a chance to cancel the abort.
**
** Arguments:
**
**      None.
**
** Returns:
**
**      int         1
*/

int __export FAR PASCAL fDance(void)
{
    DWORD dtickStart;
    XLOPER xAbort, xConfirm;
    int boolSheet;
    int col=0;
    char rgch[32];


    /*
    ** Check what kind of sheet is active. If it is a worksheet or macro
    ** sheet, this function will move the selection in a loop to show
    ** activity. In any case, it will update the status bar with a countdown.
    **
    ** Call xlSheetId; if that fails the current sheet is not a macro sheet or
    ** worksheet. Next, get the time at which to start. Then start a while
    ** loop that will run for one minute. During the while loop, check if the
    ** user has pressed ESC. If true, confirm the abort. If the abort is
    ** confirmed, clear the message bar and return; if the abort is not
    ** confirmed, clear the abort state and continue. After checking for an
    ** abort, move the active cell if on a worksheet or macro. Then
    ** update the status bar with the time remaining.
    **
    ** This block uses TempActiveCell(), which creates a temporary XLOPER.
    ** The XLOPER contains a reference to a single cell on the active sheet.
    ** This function is part of the framework library.
    */


    boolSheet = (Excel4(xlSheetId, 0, 0) == xlretSuccess);

    dtickStart = GetTickCount();

    while (GetTickCount() < dtickStart + 60000L)
    {
    Excel(xlAbort, &xAbort, 0);
    if (xAbort.val.bool)
    {
        Excel(xlcAlert, &xConfirm, 2,
        TempStr(" Are you sure you want to cancel this operation?"),
        TempNum(1));

        if (xConfirm.val.bool)
        {
        Excel(xlcMessage, 0, 1, TempBool(0));
        return 1;
        }
        else
        {
        Excel(xlAbort, 0, 1, TempBool(0));
        }
    }
    
    if (boolSheet)
    {
        Excel(xlcSelect, 0, 1, TempActiveCell(0,(BYTE)col));
        col = (col + 1) & 3;
    }
    
    wsprintf(rgch," 0:%lu",
      (60000 + dtickStart - GetTickCount()) / 1000L);
    
    Excel(xlcMessage, 0, 2, TempBool(1), TempStr(rgch));
    
    }
    
    Excel(xlcMessage, 0, 1, TempBool(0));

    return 1;
}

/*
** fDialog
**
** An example of how to create a Microsoft Excel
** UDD (User Defined Dialog) from a DLL.
**
** Arguments:
**
**              None.
**
** Returns:
**
**              int                     1
*/

int __export FAR PASCAL fDialog(void)
{
    int i, j;
    HANDLE hrgrgx;
    LPXLOPER prgrgx, px;
    XLOPER xDialog;
    XLOPER rgxList[5];
    XLOPER xList;
    XLOPER xDialogReturned;

    /*
    ** This block first allocates memory to hold the dialog box array. It then
    ** fills this array with information from rgDialog[]. It replaces any
    ** empty entries with NIL XLOPERs while filling the array. It then
    ** creates the name "XRT_XL4_List1" to refer to an array containing the
    ** list box values. The dialog box is then displayed. The dialog box is
    ** redisplayed using the results from the last dialog box. Then the arrays
    ** are freed and the name "XRT_XL4_List1" is deleted.
    */

    px = prgrgx = (LPXLOPER) GlobalLock(hrgrgx = GlobalAlloc(GMEM_MOVEABLE,
    sizeof(XLOPER)*7*rgDialogRows));

    for (i=0; i<rgDialogRows; i++)
    {
    for (j=0; j<7; j++)
    {
        if (rgDialog[i][j][0] == 0)
        {
        px->xltype = xltypeNil;
        }
        else
        {
        px->xltype = xltypeStr;
        px->val.str = rgDialog[i][j];
        }
        px++;
    }
    }

    xDialog.xltype = xltypeMulti;
    xDialog.val.array.lparray = prgrgx;
    xDialog.val.array.rows = rgDialogRows;
    xDialog.val.array.columns = 7;

    rgxList[0].val.str = (LPSTR) " Bake";
    rgxList[1].val.str = (LPSTR) " Broil";
    rgxList[2].val.str = (LPSTR) " Sizzle";
    rgxList[3].val.str = (LPSTR) " Fry";
    rgxList[4].val.str = (LPSTR) " Saute";

    for (i=0; i<5; i++)
    {
    rgxList[i].xltype = xltypeStr;
    rgxList[i].val.str[0]=(BYTE)lstrlen(rgxList[i].val.str + 1);
    }

    xList.xltype = xltypeMulti;
    xList.val.array.lparray = (LPXLOPER) & rgxList;
    xList.val.array.rows = 5;
    xList.val.array.columns = 1;

    Excel(xlfSetName, 0, 2, TempStr(" XRT_XL4_List1"), (LPXLOPER) &xList);

    Excel(xlfDialogBox, &xDialogReturned, 1, (LPXLOPER) &xDialog);

    Excel(xlfDialogBox, 0, 1, (LPXLOPER) &xDialogReturned);

    Excel(xlFree, 0, 1, (LPXLOPER) &xDialogReturned);

    GlobalUnlock(hrgrgx);
    GlobalFree(hrgrgx);

    Excel(xlfSetName, 0, 1, TempStr(" XRT_XL4_List1"));

    return 1;
}


/*
** fExit
**
** This is a user-initiated routine to exit XRT_XL4.XLL You may be tempted to
** simply call UNREGISTER("XRT_XL4.XLL") in this function. Don't do it! It
** will have the effect of forcefully unregistering all of the functions in
** this DLL, even if they are registered somewhere else! Instead, unregister
** the functions one at a time.
**
** Arguments:
**
**      None
**
** Returns:
**
**      int     1
**
*/


int __export FAR PASCAL fExit(void)
{
    XLOPER  xDLL,                   /* The name of this DLL */
        xFunc,                  /* The name of the function */
        xRegId;                 /* The registration ID */
    int     i;

    /*
    ** This code gets the DLL name. It then uses this along with information
    ** from rgFuncs[] to obtain a REGISTER.ID() for each function. The
    ** register ID is then used to unregister each function. Then the code
    ** frees the DLL name and calls xlAutoClose.
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

