/************************************************************************
 *
 *     Project:  WinPrint 2.0
 *
 *      Module:  prtcore.c
 *
 *     Remarks:
 *
 *       This module provides the printing core.  All other higher level
 *       print routines rely on this module.
 *
 *   Revisions:  
 *
 *     00.00.000  3/ 8/91 cek   First version
 *
 ***********************************************************************/
#include "..\inc\WINPRINT.h"
#include <memory.h>
#include <string.h>
#include "..\inc\dlgs.h"

extern HANDLE hDLLInst ;

/****************************************************************
 * Macros
 ****************************************************************/
#define ALLOCPRTJOB()      (GlobalAlloc( GHND, sizeof( PRTJOB ) ))
#define LOCKPRTJOB(gh)     ((LPPRTJOB)GlobalLock( gh ))
#define UNLOCKPRTJOB(gh)   (GlobalUnlock( gh ))
#define FREEPRTJOB(gh)     (GlobalFree( gh )) 


/****************************************************************
 * Local data
 ****************************************************************/
//
//    Since there may be more than one print job going at once
//    that uses this code and the AbortProc only gets hPrnDC and
//    nCode as parameters, we have a sleazy table that we can
//    use to key off of the DC to find the appropriate hPrtJob.
//
#define MAX_JOBS  4 

static HANDLE  rghYuk[MAX_JOBS][2] ;   // [][0] is hPrnDC, [][1] is hPrtJob
static short   nNumJobs ;
    
/****************************************************************
 * Local helpers
 ****************************************************************/
HWND NEAR PASCAL GethPrtJobFromSleazyTable( HDC hDC ) ;

#define HPRTJOB_PROP    "hPrtJob"

/****************************************************************
 * Put these in the EXPORTS SECTION !!!!!
 ****************************************************************/
BOOL FAR PASCAL fnCancel( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam) ;
BOOL FAR PASCAL fnAbort( HDC hPrnDC, short nCode ) ;

/****************************************************************
 *  HPRTJOB FAR PASCAL
 *    CreatePrintJob( HWND hwndParent, LPPRTJOB lpPJ, WORD wFlags ) ;
 *
 *  Description:
 *
 *       This function creates a print job.  It creates a printer DC,
 *       sets an abort proc, and puts up a cancel printing dialog box,
 *       all depending on wFlags and the default settings in the
 *       PRTJOB structure passed.
 *
 *       This function uses the PRTJOB structure passed for init-
 *       ialization only.  It copies the data into a Globally alloc'd
 *       structure.  The HPRTJOB returned is the handle to this
 *       internal structure.  if lpPJ is NULL, defaults will be used
 *       for everything.
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
HPRTJOB FAR PASCAL
CreatePrintJob( HWND hwndParent, LPPRTJOB lpPJ, WORD wFlags )
{
   HPRTJOB     hPrtJob ;
   LPPRTJOB    lpPrtJob ;

   DP( hWDB, "CreatePrintJob( %d, xxx, %d )", hwndParent, wFlags ) ;
   //
   // Allocate the print job structure.
   //
   if (!(hPrtJob = ALLOCPRTJOB()))
   {
      DP( hWDB,  "Could not allocate print job memory" ) ;
      return NULL ;
   }

   if (lpPrtJob = LOCKPRTJOB( hPrtJob ))
   {
      //
      // Copy default info into job if it's there...
      //
      if (lpPJ)
         _fmemcpy( lpPrtJob, lpPJ, sizeof( PRTJOB ) ) ;

      lpPrtJob->hwndParent = hwndParent ;

      //
      // Cancel Print Job Dialog Box.  PRT_NOCANCEL is set
      // no dialog box is presented.  If lpPJ->hwndCancelDlg
      // is NULL and PRT_NOCANCEL is not specified then
      // the default is used.
      //
      // If the user specifies a Cancel dialog box, it must be responsible
      // for disabling the parent window if it needs to.
      // 
      if (!(wFlags & PRT_NOCANCEL))
      {
         if (lpPJ->hwndCancelDlg)
         {
            lpPrtJob->fDefCancelDlg = FALSE ;
            lpPrtJob->hwndCancelDlg = lpPJ->hwndCancelDlg ;
         }
         else
         {
            FARPROC lpfn ;
            HWND    hDlg ;

            lpPrtJob->fDefCancelDlg = TRUE ;

            if (lpfn = MakeProcInstance( (FARPROC)fnCancel, hDLLInst ))
            {
               EnableWindow( hwndParent, FALSE ) ;
               if (hDlg = CreateDialogParam( hDLLInst, "CANCEL",
                                             hwndParent, (FARPROC)lpfn,
                                             (DWORD)(MAKELONG( hPrtJob, 0 ) )))
               {
                  lpPrtJob->lpfnCancelDlg = lpfn ;
                  lpPrtJob->hwndCancelDlg = hDlg ;
                  DP( hWDB, "hDlg = %d, GetParent( hDlg ) = %d, hwndParent = %d",
                     hDlg, GetParent( hDlg), hwndParent ) ;
               }
               else
               {
                  EnableWindow( hwndParent, TRUE ) ;
                  FreeProcInstance( lpfn ) ;
                  DP( hWDB, " Could not create the cancel dialog box" ) ;
                  FREEPRTJOB( hPrtJob ) ;
                  return NULL ;
               }
            }
            else
            {
               DP( hWDB, " Could not make proc instance on the cancel dlg" ) ;
               FREEPRTJOB( hPrtJob ) ;
               return NULL ;
            }
         }

      }
      DP( hWDB, "Dlg created") ;

      //
      // Abort proc.  If the caller didn't define one, we'll use our
      // default.  
      //
      //       If the caller specifies both an abort proc AND a
      //       CancelDlg, he must return FALSE from his abort proc
      //       when the user cancels.  He can keep track of whether
      //       the user has canceled any way he wants.
      //
      //
      if (!lpPJ->lpfnAbort)
      {
         FARPROC lpfn ;

         lpfn = MakeProcInstance( fnAbort, hDLLInst ) ;

         if (lpfn)
            lpPrtJob->lpfnAbort = lpfn ;
         else
         {
            lpPrtJob->lpfnAbort = NULL ;
            DP( hWDB,  "Could not make abort proc instance" ) ;
            if (lpPrtJob->fDefCancelDlg)
            {
               EnableWindow( hwndParent, TRUE ) ;
               DestroyWindow( lpPrtJob->hwndCancelDlg ) ;
            }
            FREEPRTJOB( hPrtJob ) ;
            return NULL ;
         }
      }
      else
         lpPrtJob->lpfnAbort = lpPJ->lpfnAbort ;

      //
      // Create the printer DC
      //
      if (wFlags & PRT_USEDEVMODE)
         lpPrtJob->hDC = CreateDC( lpPrtJob->szDriver,
                                   lpPrtJob->szDevice,
                                   lpPrtJob->szPort,
                                   (LPSTR)&lpPrtJob->dm ) ;
      else
         lpPrtJob->hDC = CreateDC( lpPrtJob->szDriver,
                                   lpPrtJob->szDevice,
                                   lpPrtJob->szPort,
                                   (LPSTR)NULL ) ;

      if (lpPrtJob->hDC)
         Escape( lpPrtJob->hDC, SETABORTPROC, NULL,
                 (LPSTR)lpPrtJob->lpfnAbort, (LPSTR)NULL ) ;
      else
      {
         DP( hWDB,  "Could not create printer DC" ) ;
         if (lpPrtJob->fDefCancelDlg)
         {   
            EnableWindow( hwndParent, TRUE ) ;
            DestroyWindow( lpPrtJob->hwndCancelDlg ) ;
         }
         FREEPRTJOB( hPrtJob ) ;
         return NULL ;
      }

      lpPrtJob->fUserAbort = FALSE ;

      //
      // at this point it's safe to say we have a print job
      // So let's add it to our sleazy table
      //
      if (nNumJobs >= MAX_JOBS - 1)
      {
         DP( hWDB, "Too many jobs, cannot add to table, aborting!") ;
         if (lpPrtJob->fDefCancelDlg)
         {
            EnableWindow( hwndParent, TRUE ) ;
            DestroyWindow( lpPrtJob->hwndCancelDlg ) ;
         }
         FREEPRTJOB( hPrtJob ) ;
         return NULL ;
      }
      rghYuk[nNumJobs][0] = lpPrtJob->hDC ;
      rghYuk[nNumJobs][1] = hPrtJob ;
      lpPrtJob->nJobNum = nNumJobs ;
      nNumJobs++ ;

      UNLOCKPRTJOB( hPrtJob ) ;
   }
   else
   {
      DP( hWDB, " Could not lock print job memory" ) ;
      FREEPRTJOB( hPrtJob ) ;
      return NULL ;
   }

   DP( hWDB, "hPrtJob = %d", hPrtJob ) ;
   return hPrtJob ;

} /* CreatePrintJob()  */

/****************************************************************
 *  PRTERR FAR PASCAL StartPrintJob( HPRTJOB hPJ, LPSTR lpszDesc )
 *
 *  Description:
 *
 *    This function does the STARTDOC.  It sets the job description
 *    for the spooler and the dialog box.
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
PRTERR FAR PASCAL StartPrintJob( HPRTJOB hPJ, LPSTR lpszDesc )
{
   LPPRTJOB    lpPrtJob ;
   PRTERR      pe ;

   if (lpPrtJob = LOCKPRTJOB( hPJ ))
   {
      short    len ;
      char     szDesc[32] ;

      //
      // if the default cancel dialog is being used
      //
      #if 0
      if (lpPrtJob->fDefCancelDlg && lpszDesc)
         SetDlgItemText( lpPrtJob->hwndCancelDlg, IDD_JOBNAME, lpszDesc ) ;

      #endif
      //
      // Pg 12-57 of Ref #2 statest that the spooler string length can
      // be no longer than 31 chars plus the NULL terminator.
      //
      _fstrncpy( szDesc, lpszDesc, 32 ) ;
      szDesc[31] = '\0' ;

      //
      // Do the STARTDOC
      //
      len = lstrlen( szDesc ) ;

      if (Escape( lpPrtJob->hDC, STARTDOC, len, (LPSTR)szDesc, NULL ) < 0)
      {
         DP( hWDB,  "StarPrinJob: STARTDOC failed" ) ;
         pe = PRTERR_STARTDOC ;
      }
      else
         pe = PRTERR_OK ;

      UNLOCKPRTJOB( hPJ ) ;
   }
   else
   {
      DP( hWDB, " StartPrintJob: Could not lock print job" ) ;
      pe = PRTERR_GEN ;
   }

   return pe ;

} /* StartPrintJob()  */

/****************************************************************
 *  HDC FAR PASCAL GetPrintJobDC( HPRTJOB hPJ )
 *
 *  Description: 
 *
 *    This function gets the DC of a print job that has been
 *    created with CreatePrintJob().
 *
 *  Comments:
 *
 ****************************************************************/
HDC FAR PASCAL GetPrintJobDC( HPRTJOB hPJ )
{
   LPPRTJOB    lpPrtJob ;
   HDC         hDC ;

   if (lpPrtJob = LOCKPRTJOB( hPJ ))
   {
      hDC = lpPrtJob->hDC ;
      UNLOCKPRTJOB( hPJ ) ;
   }
   else
   {
      DP( hWDB, " GetPrintJobDC: Could not lock print job" ) ;
      return NULL ;
   }

   return hDC ;

} /* GetPrintJobDC()  */

/****************************************************************
 *  void FAR PASCAL GetPrintJobDevMode( HPRTJOB hPJ, LPDEVMODE lpDM )
 *
 *  Description: 
 *
 *    This function copies the print job's DEVMODE structure to
 *    the DEVMODE pointed to by lpDM.
 *
 *  Comments:
 *
 ****************************************************************/
void FAR PASCAL GetPrintJobDevMode( HPRTJOB hPJ, LPDEVMODE lpDM )
{
   LPPRTJOB    lpPrtJob ;

   if (lpPrtJob = LOCKPRTJOB( hPJ ))
   {
      _fmemcpy( lpDM, &lpPrtJob->dm, sizeof( DEVMODE ) ) ;

      UNLOCKPRTJOB( hPJ ) ;
   }
   else
   {
      DP( hWDB, " GetPrintJobDevMode: Could not lock print job" ) ;
   }

   return ;
} /* GetPrintJobDevMode()  */



/****************************************************************
 *  PRTERR FAR PASCAL NewFrame( HPRTJOB hPJ )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
PRTERR FAR PASCAL NewFrame( HPRTJOB hPJ )
{
   LPPRTJOB   lpPrtJob ;
   PRTERR      pe ;

   if (lpPrtJob = LOCKPRTJOB( hPJ ))
   {
      short rv ;

      if ((rv = Escape( lpPrtJob->hDC, NEWFRAME, 0, 0L, 0L )) < 0)
      {
         switch (rv)
         {
            case SP_APPABORT:
               pe = PRTERR_USERABORT ;
            break ;

            case SP_USERABORT:
               pe = PRTERR_SPOOLABORT ;

            case SP_OUTOFDISK:
               pe = PRTERR_OUTOFDISK ;
            break ;

            case SP_OUTOFMEMORY:
               pe = PRTERR_OUTOFMEMORY ;

            case SP_ERROR:

            default:
               pe = PRTERR_GEN ;
         }
      }
      else
         pe = PRTERR_OK ;

      UNLOCKPRTJOB( hPJ ) ;
   }
   else
   {
      pe = PRTERR_GEN ;
   }

   return pe ;
} /* NewFrame()  */


/****************************************************************
 *  PRTERR FAR PASCAL SetPrintJobMsg( HPRTJOB hPJ, LPSTR lpszMsg )
 *
 *  Description:
 *
 *    The dialog box has two fields.  One is the job description, the
 *    other is the "message".  This could be percent or page number.
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
PRTERR FAR PASCAL SetPrintJobMsg( HPRTJOB hPJ, LPSTR lpszMsg )
{
   LPPRTJOB   lpPrtJob ;

   if (lpPrtJob = LOCKPRTJOB( hPJ ))
   {
      #if 0
      if (lpPrtJob->fDefCancelDlg && lpszMsg)
         SetDlgItemText( lpPrtJob->hwndCancelDlg, IDD_MESSAGE, lpszMsg ) ;
      #endif

      UNLOCKPRTJOB( hPJ ) ;
   }
   else
   {
      return PRTERR_GEN ;
   }

   return PRTERR_OK ;
} /* SetPrintJobMsg()  */

/****************************************************************
 *  PRTERR FAR PASCAL EndPrintJob( HPRTJOB hPJ )
 *
 *  Description:
 *
 *    This function essentially does the ENDDOC.  This function
 *    should only be called if the print job was not aborted.
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
PRTERR FAR PASCAL EndPrintJob( HPRTJOB hPJ )
{
   LPPRTJOB    lpPrtJob ;
   PRTERR      pe ;

   if (lpPrtJob = LOCKPRTJOB( hPJ ))
   {
      if (Escape (lpPrtJob->hDC, ENDDOC, 0, 0L, 0L) < 0)
      {
         pe = PRTERR_GEN ;
      }
      else
         pe = PRTERR_OK ;

      UNLOCKPRTJOB( hPJ ) ;
   }
   else
   {
      pe = PRTERR_GEN ;
   }

   return pe ;

} /* EndPrintJob()  */

/****************************************************************
 *  PRTERR FAR PASCAL AbortPrintJob( HPRTJOB hPJ )
 *
 *  Description:
 *
 *    This does essentially the ABORTDOC to abort a print job.
 *    This function should only be called if the first NewFrame or
 *    Nextband has not been reached.  It should not be called
 *    if there was an error...  The job is already aborted if
 *    an error occurs by GDI.
 *
 *  Comments:
 *
 ****************************************************************/
PRTERR FAR PASCAL AbortPrintJob( HPRTJOB hPJ )
{
   LPPRTJOB   lpPrtJob ;
   PRTERR     pe ;

   if (lpPrtJob = LOCKPRTJOB( hPJ ))
   {
      if (Escape( lpPrtJob->hDC, ABORTDOC, NULL, 0L, 0L ) < 0)
      {
         pe = PRTERR_GEN ;
      }
      else
         pe = PRTERR_OK ;

      UNLOCKPRTJOB( hPJ ) ;
   }
   else
   {
      return PRTERR_GEN ;
   }

   return pe ;

} /* AbortPrintJob()  */


/****************************************************************
 *  BOOL FAR PASCAL DestroyPrintJob( HPRTJOB hPJ )
 *
 *  Description:
 *
 *    This function destroys a print job created with CreatePrintJob.
 *
 *    It should be called only after EndPrintJob has been called or
 *    the job resulted in an error or was canceled.
 *
 *    This function destroys the default cancel dialog box if it
 *    is being used.
 *
 *    This function frees the default abort proc if it is being
 *    used.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL DestroyPrintJob( HPRTJOB hPJ )
{
   LPPRTJOB   lpPrtJob ;

   if (lpPrtJob = LOCKPRTJOB( hPJ ))
   {
      //
      // Destroy the dialog box
      //
      if (lpPrtJob->fDefCancelDlg)
      {
         EnableWindow( lpPrtJob->hwndParent, TRUE ) ;

         if (lpPrtJob->hwndCancelDlg)
         {
            DestroyWindow( lpPrtJob->hwndCancelDlg ) ;
         }

         FreeProcInstance( lpPrtJob->lpfnCancelDlg ) ;
      }

      //
      // Free the abort proc instance
      //
      if (lpPrtJob->fDefAbortProc)
         FreeProcInstance( lpPrtJob->lpfnAbort ) ;

      DeleteDC( lpPrtJob->hDC ) ;

      //
      // Now deal with the sleazy table...
      //
      rghYuk[lpPrtJob->nJobNum][0] = 0 ;
      rghYuk[lpPrtJob->nJobNum][1] = 0 ;
      nNumJobs-- ;

      UNLOCKPRTJOB( hPJ ) ;
      FREEPRTJOB( hPJ ) ;
   }
   else
   {
      return PRTERR_GEN ;
   }

   return TRUE ;
} /* DestroyPrintJob()  */

/****************************************************************
 *  LPSTR FAR PASCAL PrintJobErrorStr( PRTERR pe )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
LPSTR FAR PASCAL PrintJobErrorStr( PRTERR pe )
{
   static char *szErrorText [] = {
                                    "No Error",               
                                    "General Error",          
                                    "STARTDOC Failed",    
                                    "NEWFRAME Failed",    
                                    "Canceled by application",  
                                    "Canceled by Print Manager",
                                    "Spooler out of disk space",
                                    "Spooler out of memory",
                                    "LastErr"                 
                                 } ;

   if (pe > PRTERR_LAST)
      return szErrorText[ PRTERR_GEN ] ;

   return szErrorText [pe] ;

} /* PrintJobErrorStr()  */


/************************************************************************
 * Functions that are not external to this module.
 ***********************************************************************/

/****************************************************************
 *  HWND NEAR PASCAL GethPrtJobFromSleazyTable( HDC hDC )
 *
 *  Description: 
 *
 *    This function looks up hDC in the sleazy (!) table and
 *    returns the print job that corresponds to it.
 *
 *  Comments:
 *
 ****************************************************************/
HWND NEAR PASCAL GethPrtJobFromSleazyTable( HDC hDC )
{
   short i ;

   for (i = 0 ; i <= nNumJobs ; i++)
   {
      if ( rghYuk[i][0] == hDC )
         return rghYuk[i][1] ;
   }


   return NULL ;

} /* GethPrtJobSleazyTable()  */

/****************************************************************
 *  BOOL FAR PASCAL fnAbort( HDC hPrnDC, short nCode )
 *
 *  Description: 
 *
 *    The classic printer abort proc.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL fnAbort( HDC hPrnDC, short nCode )
{
   MSG      msg ;
   HPRTJOB  hPrtJob = GethPrtJobFromSleazyTable( hPrnDC ) ;
   BOOL     bUserAbort ; 
   HWND     hwndCancelDlg ;

   if (hPrtJob)
   {
      LPPRTJOB   lpPrtJob ;

      if (lpPrtJob = LOCKPRTJOB( hPrtJob ))
      {
         hwndCancelDlg = lpPrtJob->hwndCancelDlg ;
         bUserAbort = lpPrtJob->fUserAbort ;
         UNLOCKPRTJOB( hPrtJob ) ;
      }
      else
      {
         return FALSE ;
      }
   }
   else
   {
      return FALSE ;
   }

   while (!bUserAbort && PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
   {
      if (!hwndCancelDlg || !IsDialogMessage( hwndCancelDlg, &msg ))
      {
         TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;
      }
   }

   if (bUserAbort)
   {
   }
   return !bUserAbort ;
} /* fnAbort()  */

/****************************************************************
 *  BOOL FAR PASCAL
 *    fnCancel( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam)
 *
 *  Description: 
 *
 *    Dialog box window function for the default Cancel Dialog Box.
 *
 *  Comments:
 *
 *    This dialog function uses the property list functions to store
 *    the handle to the print job associated with the dialog box.
 *    This way the dialog box can access the print job information.
 *
 ****************************************************************/
BOOL FAR PASCAL fnCancel( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam)
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         HPRTJOB  hPrtJob ;

         hPrtJob = (HANDLE)LOWORD( lParam ) ;

         // Place hPrtJob in the windows property list
         if (!SetProp( hDlg, HPRTJOB_PROP, hPrtJob))
         {
            return FALSE ;
         }

//         EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE, MF_GRAYED) ;
      }
      break ;

      case WM_COMMAND:
      {
         HPRTJOB  hPrtJob ;
         LPPRTJOB   lpPrtJob ;

         if (!(hPrtJob = GetProp( hDlg, HPRTJOB_PROP )))
         {
            EnableWindow( GetParent( hDlg ), TRUE ) ;
            DestroyWindow( hDlg ) ;
            break ;
         }

         if (lpPrtJob = LOCKPRTJOB( hPrtJob ))
         {
            lpPrtJob->fUserAbort = TRUE ;
            lpPrtJob->hwndCancelDlg = NULL ;

            UNLOCKPRTJOB( hPrtJob ) ;
         }
         else
         {
            ;
         }
         DestroyWindow( hDlg ) ;
      }
      break ;

      case WM_DESTROY:
         RemoveProp( hDlg, HPRTJOB_PROP ) ;
      break ;

      default:
         return FALSE ;
   }
   return TRUE ;

} /* PrintDlgProc()  */



/************************************************************************
 * End of File: prtcore.c
 ***********************************************************************/

