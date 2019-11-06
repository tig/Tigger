/************************************************************************
 *
 *     Project:  WinPrint 1.4
 *
 *      Module:  dragdrop.h
 *
 ***********************************************************************/

#ifdef WIN32

#define MyDragAcceptFiles  DragAcceptFiles
#define MyDragFinish       DragFinish
#define MyDragQueryFile    DragQueryFile
#define MyDragQueryPoint   DragQueryPoint

#define MyDragUnload()     {;}

#else

VOID FAR PASCAL MyDragAcceptFiles( HWND hWnd, BOOL fAccept )  ;

VOID FAR PASCAL MyDragFinish( HANDLE hDrop ) ;

WORD FAR PASCAL MyDragQueryFile( HANDLE hDrop, WORD iFile, LPSTR lpszFile, WORD cb ) ;

BOOL FAR PASCAL MyDragQueryPoint( HANDLE hDrop, LPPOINT lppt ) ;

VOID FAR PASCAL MyDragUnload( VOID ) ;
#endif

/************************************************************************
 * End of File: dragdrop.h
 ***********************************************************************/

