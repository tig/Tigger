/*************************************************************\
 ** FILE:  pref.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **
 ** HISTORY:   
 **   Lauren Bricker  May 25, 1990 (created)
 **
\*************************************************************/

#include <windows.h>
#include "ws.h"
#include "rc_symb.h"
#include "custinfo.h"
#include "pref.h"
#include "help.h"
#include "misc.h"

/*************************************************************\
 ** WSUpdateDBDlgProc
 **
 ** ARGUMENTS:   HWND    hDlg
 **              unsigned message
 **              WORD    wParam
 **              LONG    lParam
 **
 ** DESCRIPTION: Communications dialog handling procedure
 **
 ** ASSUMES:     custinfo is a global structure
 **
 ** MODIFIES:    custinfo
 **
 ** RETURNS:     OK if successful, FAIL if not.
 **
 ** HISTORY:     Lauren Bricker  May 25, 1990 (written)
                 beng - replaced by high extension tracking of local database
                        vs. bbs master database
\*************************************************************/

#if 0
LONG FAR PASCAL 
WSUpdateDBDlgProc (hDlg, message, wParam, lParam)
    HWND     hDlg;
    unsigned message;
    WORD     wParam;
    LONG     lParam;
{

    static  WORD    tmpUpdate;

    REFERENCE_FORMAL_PARAM (lParam);


    REFERENCE_FORMAL_PARAM(lParam);
    switch ( message ) 
    {
        case WM_INITDIALOG:
        {
           CenterPopUp(hDlg);
			  CheckRadioButton (hDlg, IDB_MINIMAL_UPDATE, IDB_MAXIMAL_UPDATE,
                                custinfo.updateDB + MINMAX_ID_BASE);
           if (custinfo.updateOR)
              SendMessage(GetDlgItem(hDlg, IDB_QUICK_CHECK), BM_SETCHECK, 1, 0L);
			  tmpUpdate = custinfo.updateDB + MINMAX_ID_BASE;
           SetFocus (GetDlgItem (hDlg, IDOK));
			  return FALSE;
		     break;
        }
        case WM_COMMAND:
		    switch ( wParam )
            {
                case IDB_MINIMAL_UPDATE:
                case IDB_MAXIMAL_UPDATE:
                    tmpUpdate = wParam;
			        CheckRadioButton (hDlg, IDB_MINIMAL_UPDATE,
                                      IDB_MAXIMAL_UPDATE, tmpUpdate);
					return TRUE;
					break;

                case ID_HELP:
		            GetHelp(GH_TEXT, IDH_UPDATEDB);
                    return TRUE;
                    break;

				case IDOK:
				    custinfo.updateDB = tmpUpdate - MINMAX_ID_BASE;
                custinfo.updateOR =
                  (WORD) SendMessage(GetDlgItem(hDlg, IDB_QUICK_CHECK),
                           BM_GETCHECK, 0, 0L);
                WriteCustInfo (WRITE_CUSTDB);
				    EndDialog (hDlg, TRUE);
				    break;

				case IDCANCEL:
				    EndDialog (hDlg, TRUE);
				    break;

				default:
				    break;
			}
            break;

        default:
		    break;
    }
    return 0L;
}
#endif
