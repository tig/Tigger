//*************************************************************
//  File name: dirlist.c
//
//  Description: 
//      Routines for manipulating the DIR list object
//
//  History:    Date       Author     Comment
//               4/30/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"

#include "intFCDB.h"

struct _find_t gt_bdl;
char           gszFindBuff[144];

//*************************************************************
//
//  BuildDirList
//
//  Purpose:
//      Build a directory list and return it
//
//
//  Parameters:
//      LPSTR lpPath
//      LPINT lpLen
//      
//
//  Return: (LPSTR WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/30/92   MSM        Created
//
//*************************************************************

LPSTR WINAPI BuildDirList( LPSTR lpPath, LPINT lpLen )
{
    HLIST hlstDirs = fsmmListCreate( NULL );
    int   fDone, nLen=0;
    LPSTR lpList, lpC, lpN;

    if (!hlstDirs)
    {
        return NULL;
    }

    wsprintf( gszFindBuff, "%s*.*", (LPSTR)lpPath );

    fDone = _dos_findfirst( gszFindBuff, _A_NORMAL, &gt_bdl );

    DP1( hWDB, "%d == _dos_findfirst( %s, ... )", fDone, (LPSTR)gszFindBuff ) ;
    
    DASSERT( hWDB, fDone == 0 ) ;

    while (!fDone)
    {
        int cbCur;

        _fstrupr( gt_bdl.name );

        cbCur = lstrlen( gt_bdl.name );

        lpN = (LPSTR)fsmmListAllocNode( hlstDirs, gt_bdl.name, cbCur+1 );

        if (!lpN)
        {
            fsmmListDestroy( hlstDirs );
            return NULL;
        }

        lpC = (LPSTR)fsmmListFirstNode( hlstDirs );

        while (lpC)
        {
            //*** Find Insert point
            if (_fstricmp(lpN, lpC)<0)
            {
                lpC = (LPSTR)fsmmListPrevNode( hlstDirs, lpC );

                if (!lpC)
                {
                    fsmmListInsertNode( hlstDirs, lpN, LIST_INSERTHEAD );
                    lpC = lpN;  // So we pass the if below
                }
                else
                    fsmmListInsertNode( hlstDirs, lpN, lpC );
                break;
            }
            lpC = (LPSTR)fsmmListNextNode( hlstDirs, lpC );
        }

        if (!lpC)
            fsmmListInsertNode( hlstDirs, lpN, LIST_INSERTTAIL );

        nLen += cbCur + 1;
        fDone = _dos_findnext( &gt_bdl );
    }

    if (nLen==0)
    {
        DP1( hWDB, "BuildDirList failing because nLen is 0!" ) ;
        return NULL;
    }

    lpList = GlobalAllocPtr( GHND, nLen+1 );

    if (!lpList)
    {
        fsmmListDestroy( hlstDirs );
        DP1( hWDB, "BuildDirList failing because GlobalAllocPtr failed!" ) ;
        return NULL;
    }

    lpN = lpList;
    lpC = (LPSTR)fsmmListFirstNode( hlstDirs );

    while (lpC)
    {
        lstrcpy( lpN, lpC );
        lpN += lstrlen( lpC ) + 1;

        lpC = (LPSTR)fsmmListNextNode( hlstDirs, lpC );
    }
    *lpN = 0;

    fsmmListDestroy( hlstDirs );

    if (lpLen)
        *lpLen = nLen+1;

    return lpList;
    
} //*** BuildDirList


//*************************************************************
//
//  DestroyDirList
//
//  Purpose:
//      Destroys a list created with BuildDirList
//
//
//  Parameters:
//      LPSTR lpList
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/30/92   MSM        Created
//
//*************************************************************

VOID  WINAPI DestroyDirList( LPSTR lpList )
{
    if (lpList)
        GlobalFreePtr( lpList );

} //*** DestroyDirList


//*************************************************************
//
//  GetNextDirListItem
//
//  Purpose:
//      Retrieves the next item in the list
//
//
//  Parameters:
//      LPSTR lpCurr
//      
//
//  Return: (LPSTR WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/30/92   MSM        Created
//
//*************************************************************

LPSTR WINAPI GetNextDirListItem( LPSTR lpCurr )
{
    lpCurr += lstrlen( lpCurr ) + 1;
    if (*lpCurr)
        return lpCurr;

    return NULL;

} //*** GetNextDirListItem


//*************************************************************
//
//  IsItemInDirList
//
//  Purpose:
//      Checks to see if an i
//
//
//  Parameters:
//      LPSTR lpList
//      LPSTR lpItem
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/30/92   MSM        Created
//
//*************************************************************

LPSTR WINAPI IsItemInDirList( LPSTR lpList, LPSTR lpItem )
{
    while (lpList)
    {
        if (!_fstricmp(lpList, lpItem))
            return lpList;
        lpList = GetNextDirListItem( lpList );
    }
    return NULL;

} //*** IsItemInDirList

//*** EOF: dirlist.c
