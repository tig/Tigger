/*
**  Microsoft Excel Software Development Kit
**  Version 4.0
**
**  File:           SAMPLE\FRAMEWRK\FRAMEWRK.C
**  Description:    Framework library for Microsoft Excel
**  Platform:       Microsoft Windows
**
**  This library provides some basic functions
**  that help you write Excel DLLs. It includes
**  simple functions for managing memory with XLOPERs,
**  creating temporary XLOPERs, robustly calling
**  Excel4(), and printing debugging strings on
**  a terminal attached to COM1.
**
**  The main purpose of this library is to help
**  you to write cleaner C code for calling Excel.
**  For example, using the framework library you
**  can write
**
**      Excel(xlcDisplay, 0, 2, TempMissing(), TempBool(0));
**
**  instead of the more verbose
**
**      XLOPER xMissing, xBool;
**      xMissing.xltype = xltypeMissing;
**      xBool.xltype = xltypeBool;
**      xBool.val.bool = 0;
**      Excel4(xlcDisplay, 0, 2, (LPXLOPER) &xMissing, (LPXLOPER) &xBool);
**
**
**  The library is non-reentrant: it assumes that
**  if there are multiple copies of Excel using the
**  DLL, they will not be preempted. This is
**  acceptable under Windows/DOS, but may present
**  some problems under Windows/NT. In particular,
**  the function Excel() frees all the temporary
**  memory, which means that this DLL must not be
**  reentered while temporary memory is in use.
**
**  Define DEBUG to use the debugging functions.
**
**  Source code is provided so that you may
**  enhance this library or optimize it for your
**  own application.
*/


#include <windows.h>
#include <xlcall.h>
#include "framewrk.h"


/*
** Globals (see the comment about reentrancy 
** earlier in this file)
*/

char vMemBlock[MEMORYSIZE]; /* Memory for temporary XLOPERs */
int vOffsetMemBlock=0;      /* Offset of next memory block to allocate */


#ifdef DEBUG

/*
** debugPrintf
**
** Prints a debugging string on the auxiliary port.
** Uses the Windows function wvsprintf() to print the
** debugging string. This works a lot like the normal
** C "printf" family of functions, only it doesn't
** know how to print floating point numbers.
**
** Arguments:
**
**      LPSTR lpFormat  The format definition string
**      ...             The values to print
**
** Returns:
**
**      Nothing
**
** Note:
**
**      To use a debugging terminal, connect a serial
**      terminal to the COM1 port of your PC. Then, set
**      it up with the following MS-DOS version 5.0 command:
**
**          MODE COM1: baud=9600 parity=n data=8 stop=1
**
**      See your MS-DOS version 5.0 manual for more
**      information on the MODE command. Next, set the
**      terminal to 9600 baud, parity none, 8 data bits,
**      1 stop bit, half duplex, translate incoming CR->CR/LF.
**      See the manual for your terminal for more information
**      on this. You can use another PC running terminal
**      emulation software as the debugging terminal.
**
**      If you don't have a debugging terminal, you could
**      rewrite debugPrintf to use a Windows MessageBox(),
**      or even put the debugging information on the Excel
**      status bar (using xlcMessage).
**
**      debugPrintf provides very rudimentary debugging.
**      You will find that Microsoft Codeview for Windows
**      (included with the Windows SDK) works quite well
**      with Excel DLLs, too.
*/

void far cdecl debugPrintf(LPSTR lpFormat, ...)
{
    char rgch[256];

    _lwrite(3,rgch,wvsprintf(rgch,lpFormat,(LPSTR)((&lpFormat)+1)));
}

#endif /* DEBUG */


/*
** GetTempMemory
**
** Allocates temporary memory. Temporary memory
** can only be freed in one chunk, by calling
** FreeAllTempMemory(). This is done by Excel().
**
** Arguments:
**
**      int cBytes      How many bytes to allocate
**
** Returns:
**
**      LPSTR           A pointer to the allocated memory,
**                      or 0 if more memory cannot be
**                      allocated. If this fails,
**                      check that you are initializing
**                      vOffsetMemBlock to 0, and check that
**                      MEMORYSIZE is big enough.
**
** Algorithm:
**
**      The memory allocation algorithm is extremely
**      simple: on each call, allocate the next cBytes
**      bytes of a static memory buffer. If the buffer
**      becomes too full, simply fail. To free memory,
**      simply reset the pointer (vOffsetMemBlock)
**      back to zero. This memory scheme is very fast
**      and is optimized for the assumption that the
**      only thing you are using temporary memory
**      for is to hold arguments while you call Excel().
**      We rely on the fact that you will free all the
**      temporary memory at the same time. We also
**      assume you will not need more memory than
**      the amount required to hold a few arguments
**      to Excel().
*/

LPSTR GetTempMemory(int cBytes)
{
    LPSTR lpMemory;

    if (vOffsetMemBlock + cBytes > MEMORYSIZE)
    {
        return 0;
    }
    else
    {
        lpMemory = (LPSTR) &vMemBlock + vOffsetMemBlock;
        vOffsetMemBlock += cBytes;

		/* Prevent odd pointers */
		if (vOffsetMemBlock & 1) vOffsetMemBlock++;
        return lpMemory;
    }
}


/*
** FreeAllTempMemory
**
** Frees all temporary memory that has been allocated.
**
** Arguments:
**
**      None.
**
** Return value:
**
**      None.
*/

void FreeAllTempMemory(void)
{
    vOffsetMemBlock = 0;
}


/*
** Excel
**
** A fancy wrapper for the Excel4() function. It also
** does the following:
**
**  (1) Checks that none of the LPXLOPER arguments are 0,
**      which would indicate that creating a temporary XLOPER
**      has failed. In this case, it doesn't call Excel
**      but it does print a debug message.
**  (2) If an error occurs while calling Excel,
**      print a useful debug message.
**  (3) When done, free all temporary memory.
**
**  #1 and #2 require DEBUG to be defined.
**
** Arguments (same as Excel4()):
**
**      int xlfn            Function number (xl...) to call
**      LPXLOPER pxResult   Pointer to a place to stuff the result,
**                          or 0 if you don't care about the result.
**      int count           Number of arguments
**      ...                 (all LPXLOPERs) - the arguments.
**
** Return value:
**
**      A return code (Some of the xlret... values, as defined
**      in XLCALL.H, OR'ed together).
**
** Note:
**
**      Be sure to cast all the arguments after the third
**      to LPXLOPERs. If you accidentally pass a near pointer
**      instead of a far pointer, you will probably crash Excel.
*/

int cdecl Excel(int xlfn, LPXLOPER pxResult, int count, ...)
{
    int xlret;

#ifdef DEBUG
    int i;
    LPXLOPER FAR *ppx;
#endif

#ifdef DEBUG

    ppx = (LPXLOPER FAR *) (&count + 1);
    for (i=0; i<count; i++)
    {

        if (!*ppx)
        {
            debugPrintf("Error! A zero parameter is being passed "
                "to Excel. Check memory...\r");
            FreeAllTempMemory();
            return xlretFailed;
        }

        ppx++;
    }

#endif /* DEBUG */

    xlret = Excel4v(xlfn,pxResult,count,(LPXLOPER FAR *)(&count+1));

#ifdef DEBUG

    if (xlret != xlretSuccess)
    {
        debugPrintf("Error! Excel4(");

        if (xlfn & xlCommand)
            debugPrintf("xlCommand | ");
        if (xlfn & xlSpecial)
            debugPrintf("xlSpecial | ");
        if (xlfn & xlIntl)
            debugPrintf("xlIntl | ");
        if (xlfn & xlPrompt)
            debugPrintf("xlPrompt | ");

        debugPrintf("%u) callback failed:",xlfn & 0x0FFF);

        /* More than one error bit may be on */

        if (xlret & xlretAbort)
        {
            debugPrintf(" Macro Halted\r");
        }

        if (xlret & xlretInvXlfn)
        {
            debugPrintf(" Invalid Function Number\r");
        }

        if (xlret & xlretInvCount)
        {
            debugPrintf(" Invalid Number of Arguments\r");
        }

        if (xlret & xlretInvXloper)
        {
            debugPrintf(" Invalid XLOPER\r");
        }

        if (xlret & xlretStackOvfl)
        {
            debugPrintf(" Stack Overflow\r");
        }

        if (xlret & xlretFailed)
        {
            debugPrintf(" Command failed\r");
        }

        if (xlret & xlretUncalced)
        {
            debugPrintf(" Uncalced cell\r");
        }

    }

#endif /* DEBUG */

    FreeAllTempMemory();

    return xlret;
}


/*
** TempNum
**
** Creates a temporary numeric (IEEE floating point) XLOPER.
**
** Arguments:
**
**      double d        The value
**
** Returns:
**
**      LPXLOPER        The temporary XLOPER, or 0
**                      if GetTempMemory() failed.
**
*/

LPXLOPER TempNum(double d)
{
    LPXLOPER lpx;

    lpx = (LPXLOPER) GetTempMemory(sizeof(XLOPER));

    if (!lpx)
    {
        return 0;
    }

    lpx->xltype = xltypeNum;
    lpx->val.num = d;

    return lpx;
}


/*
** TempStr
**
** Creates a temporary string XLOPER.
**
** Arguments:
**
**      LPSTR lpstr     The string, as a null-terminated
**                      C string, with the first byte
**                      undefined. This function will
**                      count the bytes of the string
**                      and insert that count in the
**                      first byte of lpstr. Excel cannot
**                      handle strings longer than 255
**                      characters.
**
** Returns:
**
**      LPXLOPER        The temporary XLOPER, or 0
**                      if GetTempMemory() failed.
**
** Notes:
**
**      (1) This function has the side effect of inserting
**          the byte count as the first character of
**          the created string.
**
**      (2) For highest speed, with constant strings,
**          you may want to manually count the length of
**          the string before compiling, and then avoid
**          using this function.
**
**      (3) Behavior is undefined for non-null terminated
**          input or strings longer than 255 characters.
**
*/

LPXLOPER TempStr(LPSTR lpstr)
{
    LPXLOPER lpx;

    lpx = (LPXLOPER) GetTempMemory(sizeof(XLOPER));

    if (!lpx)
    {
        return 0;
    }

    lpstr[0] = (BYTE) lstrlen (lpstr+1);
    lpx->xltype = xltypeStr;
    lpx->val.str = lpstr;

    return lpx;
}


/*
** TempBool
**
** Creates a temporary logical (true/false) XLOPER.
**
** Arguments:
**
**      int b           0 - for a FALSE XLOPER
**                      Anything else - for a TRUE XLOPER
**
** Returns:
**
**      LPXLOPER        The temporary XLOPER, or 0
**                      if GetTempMemory() failed.
**
*/

LPXLOPER TempBool(int b)
{
    LPXLOPER lpx;

    lpx = (LPXLOPER) GetTempMemory(sizeof(XLOPER));

    if (!lpx)
    {
        return 0;
    }

    lpx->xltype = xltypeBool;
    lpx->val.bool = b?1:0;

    return lpx;
}


/*
** TempInt
**
** Creates a temporary integer XLOPER.
**
** Arguments:
**
**      short int i          The integer
**
** Returns:
**
**      LPXLOPER        The temporary XLOPER, or 0
**                      if GetTempMemory() failed.
**
*/

LPXLOPER TempInt(short int i)
{
    LPXLOPER lpx;

    lpx = (LPXLOPER) GetTempMemory(sizeof(XLOPER));

    if (!lpx)
    {
        return 0;
    }

    lpx->xltype = xltypeInt;
    lpx->val.w = i;

    return lpx;
}


/*
** TempErr
**
** Creates a temporary error XLOPER.
**
** Arguments:
**
**      WORD err        The error code. One of the xlerr...
**                      constants, as defined in XLCALL.H.
**                      See the Excel user manual for
**                      descriptions about the interpretation
**                      of various error codes.
**
** Returns:
**
**      LPXLOPER        The temporary XLOPER, or 0
**                      if GetTempMemory() failed.
**
*/

LPXLOPER TempErr(WORD err)
{
    LPXLOPER lpx;

    lpx = (LPXLOPER) GetTempMemory(sizeof(XLOPER));

    if (!lpx)
    {
        return 0;
    }

    lpx->xltype = xltypeErr;
    lpx->val.err = err;

    return lpx;
}


/*
** TempActiveRef
**
** Creates a temporary rectangular reference to the active
** sheet. Remember that the active sheet is the sheet that
** the user sees in front, not the sheet that is currently
** being calculated.
**
** Arguments:
**
**      WORD rwFirst    (0 based) The first row in the rectangle.
**      WORD rwLast     (0 based) The last row in the rectangle.
**      BYTE colFirst   (0 based) The first column in the rectangle.
**      BYTE colLast    (0 based) The last column in the rectangle.
**
** Returns:
**
**      LPXLOPER        The temporary XLOPER, or 0
**                      if GetTempMemory() failed.
**
*/

LPXLOPER TempActiveRef(WORD rwFirst, WORD rwLast, BYTE colFirst, BYTE colLast)
{
    LPXLOPER lpx;
    LPXLMREF lpmref;
    int wRet;

    lpx = (LPXLOPER) GetTempMemory(sizeof(XLOPER));
    lpmref = (LPXLMREF) GetTempMemory(sizeof(XLMREF));


    /* calling Excel() instead of Excel4() would free all temp memory! */
    wRet = Excel4(xlSheetId, lpx, 0);

    if (wRet != xlretSuccess)
    {
        return 0;
    }
    else
    {
        lpx->xltype = xltypeRef;
        lpx->val.mref.lpmref = lpmref;
        lpmref->count = 1;
        lpmref->reftbl[0].rwFirst = rwFirst;
        lpmref->reftbl[0].rwLast = rwLast;
        lpmref->reftbl[0].colFirst = colFirst;
        lpmref->reftbl[0].colLast = colLast;

        return lpx;
    }
}


/*
** TempActiveCell
**
** Creates a temporary reference to a single cell on the active
** sheet. Remember that the active sheet is the sheet that
** the user sees in front, not the sheet that is currently
** being calculated.
**
** Arguments:
**
**      WORD rw         (0 based) The row of the cell.
**      BYTE col        (0 based) The column of the cell.
**
** Returns:
**
**      LPXLOPER        The temporary XLOPER, or 0
**                      if GetTempMemory() failed.
**
*/

LPXLOPER TempActiveCell(WORD rw, BYTE col)
{
    return TempActiveRef(rw, rw, col, col);
}


/*
** TempActiveRow
**
** Creates a temporary reference to an entire row on the active
** sheet. Remember that the active sheet is the sheet that
** the user sees in front, not the sheet that is currently
** being calculated.
**
** Arguments:
**
**      WORD rw         (0 based) The row.
**
** Returns:
**
**      LPXLOPER        The temporary XLOPER, or 0
**                      if GetTempMemory() failed.
**
*/

LPXLOPER TempActiveRow(WORD rw)
{
    return TempActiveRef(rw, rw, 0, 0xFF);
}


/*
** TempActiveColumn
**
** Creates a temporary reference to an entire column on the active
** sheet. Remember that the active sheet is the sheet that
** the user sees in front, not the sheet that is currently
** being calculated.
**
** Arguments:
**
**      BYTE col        (0 based) The column.
**
** Returns:
**
**      LPXLOPER        The temporary XLOPER, or 0
**                      if GetTempMemory() failed.
**
*/

LPXLOPER TempActiveColumn(BYTE col)
{
    return TempActiveRef(0, 0x3FFF, col, col);
}


/*
** TempMissing
**
** This is used to simulate a missing argument when
** calling Excel(). It creates a temporary
** "missing" XLOPER.
**
** Arguments:
**
**      none.
**
** Returns:
**
**      LPXLOPER        The temporary XLOPER, or 0
**                      if GetTempMemory() failed.
**
*/

LPXLOPER TempMissing(void)
{
    LPXLOPER lpx;

    lpx = (LPXLOPER) GetTempMemory(sizeof(XLOPER));

    if (!lpx)
    {
        return 0;
    }

    lpx->xltype = xltypeMissing;

    return lpx;
}


/*
** InitFramework
**
** Initializes all the framework functions.
**
** Arguments:
**
**      None.
**
** Return value:
**
**      None.
*/

void InitFramework(void)
{
    vOffsetMemBlock = 0;
}


