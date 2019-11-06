// m_main.c

#include "markver.h"
#pragma comment (exestr, "File " __FILE__ ", Version " VERSION ", Last modified on " __TIMESTAMP__)
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mark30.h"
#include "mark30d.h" // resource identifiers

extern      HANDLE   hInst ;
extern      char *err_tbl [] ;
HWND        hFileList ;
char        szFileSpec [_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT] ;
char        szFileName [_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT] ;
WORD        wBits = 0x0402 ;
int         iCheckErr = ME_BADOPEN ;
char *err_tbl [] =
{
   "%s successfully marked for\nWindows 3.0!",
   "Can't open file %s.",
   "Can't read new executable header\nfrom %s.",
   "%s is not a\nWindows 2.x or 3.0 Application!\nFile not marked.",
   "Seek error on file %s!\nFile not marked.",
   "Read error on file %s!\nFile not marked.",
   "Write error on file %s!\nFile may have been corrupted.",
   "Write failed on file %s!\nFile may have been corrupted.",
   "%s is a Windows 3.0 application\nor has an unknown flag value.\nMark30 cannot mark this application."
} ;

long FAR PASCAL WndProc (HWND hDlg, unsigned iMessage, WORD wParam, LONG lParam) 
{
   FARPROC        lpfnDlgProc ;
   OFSTRUCT       of ;
   PAINTSTRUCT    ps ;
   TEXTMETRIC     tm ;
   HDC            hDC ;
   short          nEditLen ;
   int            iResult,i ;
   char           cLastChar ;
   //char           szBuf [80] ;

   switch (iMessage)
   {
      case WM_CREATE :
         break;

      case WM_MYINIT :
         CheckDlgButton (hDlg, IDD_MEMORY, bit(BIT_MEMORY, wBits)) ;
         CheckDlgButton (hDlg, IDD_FONT, bit(BIT_FONT, wBits) ) ;
         SetDlgItemText (hDlg, IDD_MARK, "&Select") ;
         #ifdef USEADVANCED
         EnableMenuItem (GetMenu (hDlg), IDM_ADV, MF_DISABLED | MF_GRAYED) ;
         #endif
         lstrcpy (szFileSpec, "*.exe") ;
         SendDlgItemMessage (hDlg, IDD_FNAME, EM_LIMITTEXT, 128, 0L) ;
         hFileList = GetDlgItem (hDlg, IDD_FILELIST) ;

         DlgDirList (hDlg, szFileSpec, IDD_FILELIST, IDD_CURPATH, 0x4010) ;
         SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
         break ;

      case WM_SETFOCUS:
         SetFocus (GetDlgItem (hDlg, IDD_FNAME)) ;
         break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_MEMORY:
            case IDD_FONT:
               CheckDlgButton (hDlg, wParam, IsDlgButtonChecked (hDlg, wParam) ? 0 : 1) ;
               if ( IsDlgButtonChecked (hDlg, IDD_MEMORY) )
                  wBits = wBits | 0x0400 ;
               else
                  wBits = wBits & 0xFBFF ;
               if ( IsDlgButtonChecked (hDlg, IDD_FONT) )
                  wBits = wBits | 0x0200 ;
               else
                  wBits = wBits & 0xFDFF ;
               EnableWindow (GetDlgItem(hDlg, IDD_FONT), bit(BIT_MEMORY, wBits)) ;
               break;

            case IDM_ABOUT:
               lpfnDlgProc = MakeProcInstance (AboutDlgProc, hInst) ;
               DialogBox (hInst, "AboutBox", hDlg, lpfnDlgProc) ;
               FreeProcInstance (lpfnDlgProc) ;
               break ;

            #ifdef USEADVANCED
            case IDM_ADV:
               if (iCheckErr == ME_OK)
               {
                  lpfnDlgProc = MakeProcInstance (AdvDlgProc, hInst) ;
                  DialogBox (hInst, "AdvBox", hDlg, lpfnDlgProc) ;
                  FreeProcInstance (lpfnDlgProc) ;
                  CheckDlgButton (hDlg, IDD_MEMORY, bit(BIT_MEMORY, wBits)) ;
                  CheckDlgButton (hDlg, IDD_FONT, bit(BIT_FONT, wBits)) ;
                  EnableWindow (GetDlgItem (hDlg, IDD_FONT), bit(BIT_MEMORY, wBits)) ;
               }
               break ;
            #endif

            case IDM_EXIT:
               SendMessage (hDlg, WM_CLOSE, 0, 0L) ;
               break ;

            case IDD_FILELIST:
               switch (HIWORD (lParam))
               {
                  case LBN_SELCHANGE:
                     if (DlgDirSelect (hDlg, szFileName, IDD_FILELIST))
                        lstrcat (szFileName, szFileSpec) ;
                     SetDlgItemText (hDlg, IDD_FNAME, szFileName) ;
                     break ;

                  case LBN_DBLCLK:
                     if (DlgDirSelect (hDlg, szFileName, IDD_FILELIST))
                     {
                        lstrcat (szFileName, szFileSpec ) ;
                        DlgDirList ( hDlg, szFileName, IDD_FILELIST, IDD_CURPATH, 0x4010 ) ;
                        SetDlgItemText (hDlg, IDD_FNAME, szFileSpec ) ;
                     }
                     else
                     {
                        SetDlgItemText (hDlg, IDD_FNAME, szFileName) ;
                        SendMessage (hDlg, WM_COMMAND, IDD_MARK, 0L) ;
                     }
                     break ;
               }
               break ;

            case IDD_FNAME:

               if (HIWORD (lParam) == EN_CHANGE)
               {
                  EnableWindow (GetDlgItem (hDlg, IDD_MARK),
                     (BOOL) SendMessage (LOWORD (lParam),  WM_GETTEXTLENGTH, 0, 0L)) ;
                  GetDlgItemText (hDlg, IDD_FNAME, szFileName, sizeof (szFileName) - 1) ;
                  wBits = Check30File (szFileName) ;
                  if (iCheckErr == ME_OK)
                  {
                     if (!bit (BIT_30, wBits))
                     {
                        CheckDlgButton (hDlg, IDD_MEMORY, bit (BIT_MEMORY, wBits) ) ;
                        CheckDlgButton (hDlg, IDD_FONT, bit (BIT_FONT, wBits) ) ;
                        EnableWindow (GetDlgItem(hDlg, IDD_FONT), bit (BIT_MEMORY, wBits) ) ;
                     }
                     EnableWindow (GetDlgItem(hDlg, IDD_MEMORY), !bit (BIT_30, wBits) ) ;
                     EnableWindow (GetDlgItem(hDlg, IDD_FONT), !bit (BIT_30, wBits) && bit(BIT_MEMORY, wBits)) ;
                     #ifdef USEADVANCED
                     EnableMenuItem (GetMenu (hDlg), IDM_ADV, MF_ENABLED) ;
                     #endif
                     SetDlgItemText (hDlg, IDD_MARK, "&Mark File") ;
                  }
                  else
                  {
                     EnableWindow (GetDlgItem(hDlg, IDD_MEMORY), FALSE) ;
                     EnableWindow (GetDlgItem(hDlg, IDD_FONT), FALSE) ;
                     SetDlgItemText (hDlg, IDD_MARK, "&Select") ;
                     #ifdef USEADVANCED
                     EnableMenuItem (GetMenu (hDlg), IDM_ADV, MF_DISABLED | MF_GRAYED) ;
                     #endif
                  }
               }
               break ;

            case IDD_MARK:
               GetDlgItemText (hDlg, IDD_FNAME, szFileName, sizeof (szFileName) - 1) ;
               nEditLen  = lstrlen (szFileName) ;
               cLastChar = *AnsiPrev (szFileName, szFileName + nEditLen) ;
               if (cLastChar == '\\' || cLastChar == ':')
                  lstrcat (szFileName, szFileSpec) ;
               if (cLastChar == '\\' || astrchr (szFileName, ':') || astrchr(szFileName, '*') || astrchr(szFileName, '?'))
               {
                  if (DlgDirList (hDlg, szFileName, IDD_FILELIST, IDD_CURPATH, 0x4010))
                  {
                     lstrcpy (szFileSpec, szFileName) ;
                     SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
                  }
                  else
                  {
                     SetFocus (GetDlgItem (hDlg, IDD_FNAME)) ;
                     MessageBeep (0) ;
                  }
                  break ;
               }
               lstrcat (lstrcat (szFileName, "\\"), szFileSpec) ;
               if (DlgDirList (hDlg, szFileName, IDD_FILELIST, IDD_CURPATH, 0x4010))
               {
                  lstrcpy (szFileSpec, szFileName) ;
                  SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
                  break ;
               }
               szFileName [nEditLen] = '\0' ;
               if (-1 == OpenFile (szFileName, &of, OF_READ | OF_EXIST))
               {
                  lstrcat (szFileName, szFileSpec) ;
                  if (-1 == OpenFile (szFileName, &of, OF_READ | OF_EXIST))
                  {
                     SetFocus (GetDlgItem (hDlg, IDD_FNAME)) ;
                     MessageBeep (0) ;
                     break ;
                  }
               }
               iResult = Check30File(szFileName) ;
               if (iCheckErr != ME_OK)
               {
                  OkMessageBox (hDlg, err_tbl[(-1 * iCheckErr) - 1], szFileName) ;
                  break;
               }
               lpfnDlgProc = MakeProcInstance (WarnDlgProc, hInst) ;
               iResult = DialogBox (hInst, "WarnBox", hDlg, lpfnDlgProc) ;
               FreeProcInstance (lpfnDlgProc) ;

               if (iResult)
               {
                  if ((iResult = Mark30File(szFileName, wBits)) < ME_OK)
                     OkMessageBox (hDlg, err_tbl[(-1 * iResult) -1], szFileName) ;
               }
               break ;

         }
         break ;

      case WM_PAINT :
         hDC = BeginPaint (hDlg, &ps) ;
         GetTextMetrics (hDC, &tm) ;
         DrawShadow (hDC, GetDlgItem (hDlg, IDD_FNAME), GetStockObject(GRAY_BRUSH), tm.tmAveCharWidth / 2) ;
         DrawShadow (hDC, GetDlgItem (hDlg, IDD_FILELIST), GetStockObject(GRAY_BRUSH), tm.tmAveCharWidth / 2) ;
         EndPaint (hDlg, &ps) ;
         return DefDlgProc (hDlg, iMessage, wParam, lParam) ;

      case WM_DESTROY:
         PostQuitMessage (0) ;
         break ;

      case WM_CLOSE:
         return DefWindowProc (hDlg, iMessage, wParam, lParam) ;

      default:
         return DefDlgProc (hDlg, iMessage, wParam, lParam) ;
   }
   return 0L ;
}

WORD Check30File ( PSTR szFileName )
{
   FILE *ioExe ;
   unsigned lBuffer[0x21C], lIndex ;

   if (!(ioExe = fopen( szFileName, "r+b" )))
   {
      iCheckErr = ME_BADOPEN ;
      return 0 ;
   }

   if (fread( lBuffer, 0x9F, 1, ioExe ) != 1)
   {
      fclose (ioExe) ;
      iCheckErr = ME_NOHEADER ;
      return 0 ;
   }

   if (lBuffer[0] != 0x5A4D)
   {
      fclose (ioExe) ;
      iCheckErr = ME_BADFORMAT ;
      return 0 ;
   }
    
   lIndex = lBuffer[0x1E];
   if (fseek( ioExe, (WORD) lIndex, SEEK_SET ))
   {
      fclose (ioExe) ;
      iCheckErr = ME_SEEK ;
      return 0 ;
   }

   if (fread( lBuffer, 0x1C * sizeof( lBuffer[0] ), 1, ioExe ) != 1)
   {
      fclose (ioExe) ;
      iCheckErr = ME_READ ;
      return 0 ;
   }

   if (lBuffer[0] != 0x454E)
   {
      fclose (ioExe) ;
      iCheckErr = ME_BADFORMAT ;
      return 0 ;
   }

   fclose (ioExe) ;
   iCheckErr = ME_OK ;
   return LOWORD(lBuffer[0x1B]) ;
   
}


int Mark30File( PSTR szFileName, WORD wBits )
{
   FILE *ioExe;
   unsigned lBuffer[0x21C], lIndex ;

   if (!(ioExe = fopen( szFileName, "r+b" )))
      return ME_BADOPEN ;

   if (fread( lBuffer, 0x9F, 1, ioExe ) != 1)
   {
      fclose (ioExe) ;
      return ME_NOHEADER ;
   }

   if (lBuffer[0] != 0x5A4D)
   {
      fclose (ioExe) ;
      return ME_BADFORMAT ;
   }
    
   lIndex = lBuffer[0x1E];
   if (fseek( ioExe, (WORD) lIndex, SEEK_SET ))
   {
      fclose (ioExe) ;
      return ME_SEEK ;
   }

   if (fread( lBuffer, 0x1C * sizeof( lBuffer[0] ), 1, ioExe ) != 1)
   {
      fclose (ioExe) ;
      return ME_READ ;
   }

   if (lBuffer[0] != 0x454E)
   {
      fclose (ioExe) ;
      return ME_BADFORMAT ;
   }

   lBuffer [0x1B] = wBits ;

   if (fseek( ioExe, (WORD)lIndex, SEEK_SET ))
   {
      fclose (ioExe) ;
      return ME_WRITE ;
   }
   
   if (fwrite( lBuffer, 0x1C * sizeof( lBuffer[0] ), 1, ioExe ) != 1)
   {
      fclose (ioExe) ;
      return ME_WRITEFAIL ;
   }

   fclose (ioExe) ;
   return( ME_OK );
}


static char *astrchr (char *str, int ch)
{
   while (*str)
   {
      if (ch == *str)
         return str ;
      str = AnsiNext (str) ;
   }
   return NULL ;
}


static char *astrrchr (char *str, int ch)
{
   char *strl = str + lstrlen (str) ;

   do
   {
      if (ch == *strl)
         return strl ;
      strl = AnsiPrev (str, strl) ;
   }
   while (strl > str) ;
   return NULL ;
}
