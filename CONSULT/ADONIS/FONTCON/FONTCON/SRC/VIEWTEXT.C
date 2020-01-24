//*************************************************************
//  File name: viewtext.c
//
//  Description: 
//      handles the ViewText API
//
//  History:    Date       Author     Comment
//               4/14/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "global.h"
#include <shellapi.h>
#include "viewtext.h"

typedef struct
{
    LPSTR lpText;
    LPSTR lpTitle;
    UINT  uiFlags;
} VIEWTEXTSTRUCT;

//*************************************************************
//
//  ViewText
//
//  Purpose:
//      Views a text buffer
//
//
//  Parameters:
//      HWND  hParent
//      LPSTR lpText
//      LSPTR lpTtile
//      UINT uiFlags
//
//  Return: (WORD WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/14/92   MSM        Created
//
//*************************************************************

WORD WINAPI ViewText(HWND hParent, LPSTR lpText, LPSTR lpTitle, UINT uiFlags)
{
    VIEWTEXTSTRUCT  vts;
    WORD            wRet;
    HINSTANCE       hInst = (HINSTANCE)GetWindowWord( hParent, GWW_HINSTANCE );
    DLGPROC         lpProc;

    vts.lpTitle = lpTitle;
    vts.uiFlags = uiFlags;

    if (uiFlags & VT_LOADFILE )
    {
        OFSTRUCT of ;
        HFILE    hFile ;
        LPSTR    lp;

        if ((hFile = OpenFile( lpText, &of, OF_READ | OF_SHARE_DENY_NONE ))>0)
        {
            UINT cb = (UINT)_filelength( hFile );

            //*** Allocate memory + some
            if (lp = (LPSTR)GlobalAllocPtr(GHND, cb+25))
            {
                UINT ui;

                //*** Error loading file        
                if((ui=_lread( hFile, lp, cb ))!=cb)
                    lstrcpy( lp+ui, "<<ERROR LOADING FILE>>" );
                lpText = lp;
            }
            else
            {
                lpText = NULL;
                uiFlags &= ~VT_LOADFILE;
            }
            _lclose( hFile ) ;
        }
        else
        {
            lpText = NULL;
            uiFlags &= ~VT_LOADFILE;
        }
    }

    if (!lpText)
        lpText = "<<ERROR LOADING FILE>>";

    vts.lpText = lpText;

    lpProc = (DLGPROC)MakeProcInstance((FARPROC)ViewTextProc,hInst);

    wRet = (WORD)DialogBoxParam(hInst, ID(VIEWTEXT_DLG),hParent,lpProc,
                    (LONG)(LPSTR)&vts );

    FreeProcInstance( (FARPROC)lpProc );

    if (uiFlags & VT_LOADFILE )
        GlobalFreePtr( lpText );

    return wRet;

} //*** ViewText


//*************************************************************
//
//  ViewTextProc
//
//  Purpose:
//      Dialog box procedure
//
//
//  Parameters:
//      HWND hDlg
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (BOOL CALLBACK)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/14/92   MSM        Created
//
//*************************************************************

BOOL CALLBACK ViewTextProc(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
    static VIEWTEXTSTRUCT FAR *lpvts ;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            lpvts = (VIEWTEXTSTRUCT FAR *)lParam;

            SetWindowText( hDlg, lpvts->lpTitle );
            SetWindowText( GetDlgItem(hDlg,IDE_TEXT), lpvts->lpText );

//            if (lpvts->uiFlags & VT_NOCOPY)
                ShowWindow( GetDlgItem(hDlg,IDB_COPY), SW_HIDE );

            if (lpvts->uiFlags & VT_NOHELP)
                ShowWindow( GetDlgItem(hDlg,IDB_HELP), SW_HIDE );

            if (lpvts->uiFlags & VT_NOPRINT)
                ShowWindow( GetDlgItem(hDlg,IDB_PRINT), SW_HIDE );

            if (!(lpvts->uiFlags & VT_AGREE_DISAGREE))
            {
                SetDlgItemText( hDlg, IDOK, "OK" );
                SetDlgItemText( hDlg, IDCANCEL, "Cancel" );
            }

            SetWindowFont( GetDlgItem( hDlg, IDE_TEXT ), ghfontSmall, FALSE ) ;

            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;

            SendMessage( hDlg, WM_SIZE, 0, 0L ) ;
            return TRUE;
        }
        break;

        case WM_SIZE:
        {
            RECT rc;
            HWND hWnd;
            int  nW, nH;

#define DLG_SPACER  6

            hWnd = GetDlgItem( hDlg, IDOK );

            if (!hWnd)
                return FALSE;

            GetClientRect( hWnd, &rc );

            nW = rc.right;
            nH = rc.bottom;

            GetClientRect( hDlg, &rc );

            SetWindowRedraw( hDlg, FALSE );

            SetWindowPos(hWnd,NULL,rc.right-nW-DLG_SPACER,DLG_SPACER,
                0, 0, SWP_NOSIZE|SWP_NOZORDER );

            hWnd = GetDlgItem( hDlg, IDCANCEL );
            SetWindowPos(hWnd,NULL,rc.right-nW-DLG_SPACER,2*DLG_SPACER + nH,
                0, 0, SWP_NOSIZE|SWP_NOZORDER );

            hWnd = GetDlgItem( hDlg, IDB_HELP );
            SetWindowPos(hWnd,NULL,rc.right-nW-DLG_SPACER,3*DLG_SPACER + 2*nH,
                0, 0, SWP_NOSIZE|SWP_NOZORDER );

            hWnd = GetDlgItem( hDlg, IDB_COPY );
            SetWindowPos(hWnd,NULL,rc.right-nW-DLG_SPACER,4*DLG_SPACER + 3*nH,
                0, 0, SWP_NOSIZE|SWP_NOZORDER );

            hWnd = GetDlgItem( hDlg, IDB_PRINT );
            SetWindowPos(hWnd,NULL,rc.right-nW-DLG_SPACER,5*DLG_SPACER + 4*nH,
                0, 0, SWP_NOSIZE|SWP_NOZORDER );

            hWnd = GetDlgItem( hDlg, IDE_TEXT );
            SetWindowPos(hWnd,NULL,DLG_SPACER,DLG_SPACER,
                rc.right-3*DLG_SPACER-nW, rc.bottom-2*DLG_SPACER,SWP_NOZORDER);

            SetWindowRedraw( hDlg, TRUE );
            InvalidateRect( hDlg, NULL, TRUE );
        }
        break;

        case WM_COMMAND:
        {
            switch (wParam)
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, wParam);
                    return TRUE;
                break;

                case IDB_COPY:
                {
                    HWND hwndEdit = GetDlgItem( hDlg, IDE_TEXT );
                    long lSel;

                    lSel = SendMessage(hwndEdit,EM_GETSEL,0,0L);
                    SetWindowRedraw( hwndEdit, FALSE );
                    if (LOWORD(lSel)==HIWORD(lSel))
                        SendMessage(hwndEdit,EM_SETSEL,0,0xFFFF0000);
                    SendMessage(hwndEdit,WM_COPY,0,0L);
                    SendMessage(hwndEdit,EM_SETSEL,0,lSel);
                    SetWindowRedraw( hwndEdit, TRUE );
                }
                break;

               case IDB_PRINT:
               {
                  HFILE hfTempFile ;
                  char szFile[144] ;

                  GetTempFileName(0, "fs", 0, szFile ) ;
                  *(strrchr( szFile, '\\' )+1) = '\0' ;
                  strcat( szFile, "FONTCON.TXT" ) ;

                  hfTempFile = _lcreat((LPSTR)szFile, 0 ) ;
                  if (hfTempFile == -1)
                  {
                     DASSERTMSG( hWDB, 0, "Could not create temp file!" ) ;
                     return FALSE ;
                  }

                  _lwrite( hfTempFile, lpvts->lpText, lstrlen( lpvts->lpText ) ) ;
                  _lclose( hfTempFile ) ;

                  ShellExecute( hDlg, "print", szFile, NULL, "", SW_HIDE ) ;
               }
               break ;

                case IDB_HELP:
                {
                    HWND hWnd = (HWND)GetWindowWord( hDlg, GWW_HWNDPARENT );

                    if (IsWindow( hWnd ))
                        SendMessage( hWnd, VTN_HELP, (WORD)hDlg, 0L );
                }
                break;
            }
        }
        break;
    }
    return FALSE;

} //*** ViewTextProc

//*** EOF: viewtext.c
