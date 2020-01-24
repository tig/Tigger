// escape.c
//
// Escape() information dialog box
//
//
         if (Escape( hPrnDC, GETTECHNOLOGY, NULL, NULL, (LPSTR)szBuf ))
         {
            SetDlgItemText( hDlg, IDD_TECHNOLOGY_TXT, szBuf ) ;
         }
         else
            SetDlgItemText( hDlg, IDD_TECHNOLOGY_TXT, "None" ) ;

         if (Escape( hPrnDC,  GETSETPAPERBINS, sizeof( BININFO ),
                              (LPSTR)NULL,
                              (LPSTR)&biOut ))
         {
            // Enumerate Paper bins
            SendDlgItemMessage( hDlg, IDD_PAPERBINS_LB, LB_RESETCONTENT, 0, 0L ) ;

            wsprintf( szBuf, (LPSTR)"# %d, cur %d",
                              biOut.NbrofBins, biOut.BinNumber ) ;
            SendDlgItemMessage( hDlg, IDD_PAPERBINS_LB, LB_ADDSTRING, 0,
               (LONG)(LPSTR)szBuf ) ;
         }
         else
            // Paper bins are not supported on this printer
            EnableWindow( GetDlgItem( hDlg, IDD_PAPERBINS_LB ), FALSE ) ;
         #endif

