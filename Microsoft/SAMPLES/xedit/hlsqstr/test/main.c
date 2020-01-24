/** tm.c
 *
 *  DESCRIPTION: 
 *      TestMaker Application
 *
 *  HISTORY:
 *
 ** MSM! */

#include "global.h"

HANDLE  ghInst      = NULL;
HWND    ghWndMain   = NULL;

char    szMainMenu[]    = "MainMenu";
char    szMainClass[]   = "MainWClass";

#include <hlsqstr.h>
VOID HeapTest(VOID);
VOID ListTest(VOID);
VOID StackTest(VOID);
VOID QueueTest(VOID);
LPSTR FAR PASCAL DumpProc(LPNODE lpn);


/** int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int)
 *
 *  DESCRIPTION: 
 *      Entry point for all windows application
 *
 *  ARGUMENTS:
 *      (HANDLE, HANDLE, LPSTR, int)
 *
 *  RETURN (int PASCAL):
 *
 *
 *  NOTES:
 *
 ** MSM! */

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    HeapTest();
    ListTest();
    StackTest();
    QueueTest();

    if (!hPrevInstance && !InitApplication(hInstance))
            return (FALSE);       

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);      
        DispatchMessage(&msg);       
    }
    return (msg.wParam);      

} /* WinMain() */

/** long FAR PASCAL MainWndProc(HWND hWnd, unsigned msg, WORD wParam, LONG lParam)
 *
 *  DESCRIPTION: 
 *      Window procedure for main overlapped window
 *
 *  ARGUMENTS:
 *      (HWND hWnd, unsigned msg, WORD wParam, LONG lParam)
 *
 *  RETURN (long FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** MSM! */

long FAR PASCAL MainWndProc(HWND hWnd, unsigned msg, WORD wParam, LONG lParam)
{
    FARPROC lpProc;

    switch (msg) 
    {
        case WM_COMMAND: 
            switch ( wParam )
            {
                case IDM_ABOUT:
                    lpProc = MakeProcInstance(About, ghInst);
                    DialogBox(ghInst, "AboutBox", hWnd, lpProc);    
                    FreeProcInstance(lpProc);
                break;
            }
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
        break;
    }
    return (DefWindowProc(hWnd, msg, wParam, lParam));

} /* MainWndProc() */

/** BOOL FAR PASCAL About(HWND hDlg, unsigned msg, WORD wParam, LONG lParam)
 *
 *  DESCRIPTION: 
 *      The About box window procedure
 *
 *  ARGUMENTS:
 *      (HWND hDlg, unsigned msg, WORD wParam, LONG lParam)
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** MSM! */

BOOL FAR PASCAL About(HWND hDlg, unsigned msg, WORD wParam, LONG lParam)
{
    switch (msg) 
    {
        case WM_INITDIALOG: 
            return (TRUE);

        case WM_COMMAND:
            if (wParam == IDOK || wParam == IDCANCEL) 
            {
                EndDialog(hDlg, TRUE);         
                return (TRUE);
            }
        break;
    }
    return (FALSE);                  /* Didn't process a message    */

} /* About() */

VOID HeapTest()
{
    HANDLE hMem;
    LPSTR lp;
    WORD  wSize;

    HHEAP hhHeap = hlsqHeapCreate( 2048, HT_SHARED|HT_MULTIPLE );

    if (hhHeap)
    {
        hMem = hlsqHeapAlloc(hhHeap, LHND, 16384);
        wSize = hlsqHeapFlags(hhHeap, hMem);
        lp   = hlsqHeapLock(hhHeap, hMem);
        hlsqHeapHandle(hhHeap, lp);
        hlsqHeapUnlock(hhHeap, hMem);
        wSize = hlsqHeapSize(hhHeap, hMem);
        hlsqHeapReAlloc(hhHeap, hMem, 17408, LHND);
        wSize = hlsqHeapSize(hhHeap, hMem);
        wSize = hlsqHeapFlags(hhHeap, hMem);

        hMem = hlsqHeapAlloc(hhHeap, LHND, 16384);
        lp   = hlsqHeapLock(hhHeap, hMem);
        hlsqHeapHandle(hhHeap, lp);
        hlsqHeapUnlock(hhHeap, hMem);
        wSize = hlsqHeapSize(hhHeap, hMem);
        hlsqHeapReAlloc(hhHeap, hMem, 17408, LHND);
        wSize = hlsqHeapSize(hhHeap, hMem);
        wSize = hlsqHeapFlags(hhHeap, hMem);

        hMem = hlsqHeapAlloc(hhHeap, LHND, 16384);
        lp   = hlsqHeapLock(hhHeap, hMem);
        hlsqHeapHandle(hhHeap, lp);
        hlsqHeapUnlock(hhHeap, hMem);
        wSize = hlsqHeapSize(hhHeap, hMem);
        hlsqHeapReAlloc(hhHeap, hMem, 17408, LHND);
        wSize = hlsqHeapSize(hhHeap, hMem);
        wSize = hlsqHeapFlags(hhHeap, hMem);

        hMem = hlsqHeapAlloc(hhHeap, LHND, 16384);
        lp   = hlsqHeapLock(hhHeap, hMem);
        hlsqHeapHandle(hhHeap, lp);
        hlsqHeapUnlock(hhHeap, hMem);
        wSize = hlsqHeapSize(hhHeap, hMem);
        hlsqHeapReAlloc(hhHeap, hMem, 17408, LHND);
        wSize = hlsqHeapSize(hhHeap, hMem);
        wSize = hlsqHeapFlags(hhHeap, hMem);

        hMem = hlsqHeapAlloc(hhHeap, LHND, 16384);
        lp   = hlsqHeapLock(hhHeap, hMem);
        hlsqHeapHandle(hhHeap, lp);
        hlsqHeapUnlock(hhHeap, hMem);
        wSize = hlsqHeapSize(hhHeap, hMem);
        hlsqHeapReAlloc(hhHeap, hMem, 17408, LHND);
        wSize = hlsqHeapSize(hhHeap, hMem);
        wSize = hlsqHeapFlags(hhHeap, hMem);

        hMem = hlsqHeapAlloc(hhHeap, LHND, 16384);
        lp   = hlsqHeapLock(hhHeap, hMem);
        hlsqHeapHandle(hhHeap, lp);
        hlsqHeapUnlock(hhHeap, hMem);
        wSize = hlsqHeapSize(hhHeap, hMem);
        hlsqHeapReAlloc(hhHeap, hMem, 17408, LHND);
        wSize = hlsqHeapSize(hhHeap, hMem);
        wSize = hlsqHeapFlags(hhHeap, hMem);

        hMem = hlsqHeapAlloc(hhHeap, LHND, 16384);
        lp   = hlsqHeapLock(hhHeap, hMem);
        hlsqHeapHandle(hhHeap, lp);
        hlsqHeapUnlock(hhHeap, hMem);
        wSize = hlsqHeapSize(hhHeap, hMem);
        hlsqHeapReAlloc(hhHeap, hMem, 17408, LHND);
        wSize = hlsqHeapSize(hhHeap, hMem);
        wSize = hlsqHeapFlags(hhHeap, hMem);

        hlsqHeapItems(hhHeap);
        hlsqHeapFree( hhHeap, hMem );
        hlsqHeapDumpDebug(hhHeap);
        hlsqHeapDestroy( hhHeap );
    }

    hhHeap = hlsqHeapCreate( 2048, HT_SHARED|HT_MULTIPLE );

    if (hhHeap)
    {
        char temp[80];
        int  i;
        LONG lTime = GetTickCount();

        for (i=0; i<1000; i++)
        {
            if (!hlsqHeapAllocLock( hhHeap, 128 ))
                break;
        }
        lTime = GetTickCount() - lTime;
        wsprintf( temp, "%d AllocLocks in: %ld / %ld", i, lTime, lTime/1000);
        OutputDebugString(temp);
    }
}

VOID ListTest()
{
    HLIST hlstList = hlsqListCreate(NULL);
    DUMPPROC lpDP = (DUMPPROC)MakeProcInstance( (FARPROC)DumpProc, ghInst );

    if (hlstList)
    {
        LPNODE lpn;
        LONG lData=0;

        for (lData=0; lData<20; lData++)
            hlsqListAllocInsertNode(hlstList,(LPSTR)&lData,4,LIST_INSERTTAIL);

        lpn = hlsqListFirstNode(hlstList);
        while (lpn)
        {
            lData = *( (LPLONG)lpn );
            lpn = hlsqListNextNode( hlstList, lpn );
        }
        hlsqListDumpDebug(hlstList, lpDP);
        hlsqListFreeList(hlstList);

        if (hlstList)
        {
            char temp[80];
            int  i;
            LONG lTime = GetTickCount();

            for (i=0; i<1000; i++)
            {
                if (!hlsqListAllocInsertNode(hlstList,(LPSTR)temp,75,LIST_INSERTTAIL))
                    break;
            }
            lTime = GetTickCount() - lTime;
            wsprintf( temp, "%d ListInserts(75) in: %ld / %ld", i, lTime, lTime/1000);
            OutputDebugString(temp);
        }
        hlsqListDestroy( hlstList );
    }
    FreeProcInstance( (FARPROC)lpDP );
}

VOID StackTest()
{
    HSTACK hstkStack = hlsqStackCreate(NULL);
    DUMPPROC lpDP = (DUMPPROC)MakeProcInstance( (FARPROC)DumpProc, ghInst );

    if (hstkStack)
    {
        LPNODE lpn;
        LONG lData=0;

        for (lData=0; lData<20; lData++)
            hlsqStackAllocPush(hstkStack,(LPSTR)&lData,4);

        hlsqStackDumpDebug(hstkStack, lpDP);
        lpn = hlsqStackPop(hstkStack);
        while (lpn)
        {
            lData = *( (LPLONG)lpn );
            hlsqStackFreeNode( hstkStack, lpn );
            lpn = hlsqStackPop(hstkStack);
        }
        hlsqStackDumpDebug(hstkStack, lpDP);

        hlsqStackFreeStack(hstkStack);
        if (hstkStack)
        {
            char temp[80];
            int  i;
            LONG lTime = GetTickCount();

            for (i=0; i<1000; i++)
            {
                if (!hlsqStackAllocPush(hstkStack,(LPSTR)temp,75))
                    break;
            }
            lTime = GetTickCount() - lTime;
            wsprintf( temp, "%d StackPushs(75) in: %ld / %ld", i, lTime, lTime/1000);
            OutputDebugString(temp);
        }
        hlsqStackDestroy( hstkStack );
    }
    FreeProcInstance( (FARPROC)lpDP );
}

VOID QueueTest()
{
    HQUEUE hqQueue = hlsqQueueCreate(NULL);
    DUMPPROC lpDP = (DUMPPROC)MakeProcInstance( (FARPROC)DumpProc, ghInst );

    if (hqQueue)
    {
        LPNODE lpn;
        LONG lData=0;

        for (lData=0; lData<20; lData++)
            hlsqQueueAllocInQ(hqQueue,(LPSTR)&lData,4);

        hlsqQueueDumpDebug(hqQueue, lpDP);
        lpn = hlsqQueueDeQ(hqQueue);
        while (lpn)
        {
            lData = *( (LPLONG)lpn );
            hlsqQueueFreeNode( hqQueue, lpn );
            lpn = hlsqQueueDeQ(hqQueue);
        }
        hlsqQueueDumpDebug(hqQueue, lpDP);
        hlsqQueueFreeQueue(hqQueue);
        if (hqQueue)
        {
            char temp[80];
            int  i;
            LONG lTime = GetTickCount();

            for (i=0; i<1000; i++)
            {
                if (!hlsqQueueAllocInQ(hqQueue,(LPSTR)temp,75))
                    break;
            }
            lTime = GetTickCount() - lTime;
            wsprintf( temp, "%d QueueIns(75) in: %ld / %ld", i, lTime, lTime/1000);
            OutputDebugString(temp);
        }
        hlsqQueueDestroy( hqQueue );
    }
    FreeProcInstance( (FARPROC)lpDP );
}


LPSTR FAR PASCAL DumpProc(LPNODE lpn)
{
    static char temp[40];
    long lData = *( (LPLONG)lpn );

    wsprintf( (LPSTR)temp, (LPSTR)"Number: %ld", lData );
    return (LPSTR)temp;
}        

/** EOF: tm.c **/

