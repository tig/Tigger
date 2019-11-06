/************************************************************************
 *
 *     Project:  WinPrint 1.4
 *
 *      Module:  dragdrop.c
 *
 *     Remarks:  Isolates drag drop api so DLL's don't have to
 *               be present.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "PRECOMP.H"

#include <shellapi.h>

static HINSTANCE hinstShell ;


VOID FAR PASCAL MyDragAcceptFiles( HWND hWnd, BOOL fAccept ) 
{
   #ifdef WIN32
      DragAcceptFiles( hWnd, fAccept ) ;
   #else
   VOID (FAR PASCAL *lpfn)( HWND, BOOL ) ;

   if (!hinstShell)
   {
      SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
      hinstShell = LoadLibrary( "SHELL.DLL" ) ;

      if (hinstShell <= (HINSTANCE)32)
      {
         hinstShell = NULL ;
         return ;
      }
   }
      
   DP( hWDB, "hinstShell = %04X", hinstShell ) ;

   if ((FARPROC)lpfn = GetProcAddress( hinstShell, "DragAcceptFiles" ))
      (*lpfn)( hWnd, fAccept ) ;
   #endif
}

VOID FAR PASCAL MyDragFinish( HANDLE hDrop )
{
   #ifdef WIN32
      DragFinish( hDrop ) ;
   #else
   VOID (FAR PASCAL *lpfn)( HANDLE ) ;

   if (!hinstShell)
     return ;

   if ((FARPROC)lpfn = GetProcAddress( hinstShell, "DragFinish" ))
      (*lpfn)( hDrop ) ;
   #endif
}

WORD FAR PASCAL MyDragQueryFile( HANDLE hDrop, WORD iFile, LPSTR lpszFile, WORD cb )
{
   #ifdef WIN32
      return DragQueryFile( hDrop, iFile, lpszFile, cb ) ;
   #else
   WORD (FAR PASCAL *lpfn)( HANDLE, WORD, LPSTR, WORD ) ;

   if (!hinstShell)
      return NULL ;

   if ((FARPROC)lpfn = GetProcAddress( hinstShell, "DragQueryFile" ))
      return (*lpfn)( hDrop, iFile, lpszFile, cb ) ;

   return NULL ;
   #endif
}

BOOL FAR PASCAL MyDragQueryPoint( HANDLE hDrop, LPPOINT lppt )
{
   #ifdef WIN32
      return DragQueryPoint( hDrop, lppt ) ;
   #else
   BOOL (FAR PASCAL *lpfn)( HANDLE, LPPOINT ) ;

   if (!hinstShell)
      return FALSE ;

   if ((FARPROC)lpfn = GetProcAddress( hinstShell, "DragQueryPoint" ))
      return (*lpfn)( hDrop, lppt ) ;

   return FALSE ;
   #endif
}

VOID FAR PASCAL MyDragUnload( VOID )
{
   #ifndef WIN32
   if (hinstShell)
      FreeLibrary( hinstShell ) ;
   #endif
}


/************************************************************************
 * End of File: dragdrop.c
 ***********************************************************************/

