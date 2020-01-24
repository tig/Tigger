/************************************************************************
 *
 *     Project:  
 *
 *      Module:  prtcore.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#ifndef DM_SPECVERSION
#include <drivinit.h>
#endif

#ifndef _MAX_PATH
#include <stdlib.h>
#endif

typedef enum tagPRTERR 
{  
   PRTERR_OK = 0,
   PRTERR_GEN,
   PRTERR_STARTDOC,
   PRTERR_NEWFRAME,
   PRTERR_USERABORT,
   PRTERR_SPOOLABORT,
   PRTERR_OUTOFDISK,
   PRTERR_OUTOFMEMORY,
   PRTERR_LAST 
} PRTERR ;

//
// _MAX_* are defined in stdlib.h. 
//
#define DRIVER_STRLEN   (_MAX_FNAME+_MAX_EXT)
#define DEVICE_STRLEN   (32)
#define PORT_STRLEN     (_MAX_PATH)
#define JOBDESC_STRLEN  (256)


//
// Flags
//
#define PRT_DEFAULT     (0x0000)
#define PRT_NOCANCEL    (0x0001)    
#define PRT_USEDEVMODE  (0x0002)

typedef struct tagPRTJOB
{
   // 
   // Public fields that can be accessed by the caller
   //
   DEVMODE  dm ;

   char     szDriver[ DRIVER_STRLEN ] ;
   char     szDevice[ DEVICE_STRLEN ] ;
   char     szPort[ PORT_STRLEN ] ;

   char     szJobDesc[ JOBDESC_STRLEN ] ;

   FARPROC  lpfnAbort ;

   HWND     hwndCancelDlg ;
   FARPROC  lpfnCancelDlg ;

   HDC      hDC ;

   //
   // "Reserved" fields that are used internally and should not
   // be messed with by the caller.
   //
   BOOL     fDefAbortProc ;
   BOOL     fDefCancelDlg ;

   BOOL     fUserAbort ;

   short    nJobNum ;

   HWND     hwndParent ;

} PRTJOB, *PPRTJOB, FAR *LPPRTJOB ;

#define HPRTJOB GLOBALHANDLE

HPRTJOB  FAR PASCAL 
         CreatePrintJob( HWND hwndParent, LPPRTJOB lpPJ, WORD wFlags ) ;
PRTERR   FAR PASCAL StartPrintJob( HPRTJOB hPJ, LPSTR lpszDesc ) ;
HDC      FAR PASCAL GetPrintJobDC( HPRTJOB hPJ ) ;
void     FAR PASCAL GetPrintJobDevMode( HPRTJOB hPJ, LPDEVMODE lpDM ) ;
PRTERR   FAR PASCAL NewFrame( HPRTJOB hPJ ) ;
PRTERR   FAR PASCAL SetPrintJobMsg( HPRTJOB hPJ, LPSTR lpszMsg ) ;
PRTERR   FAR PASCAL EndPrintJob( HPRTJOB hPJ ) ;
PRTERR   FAR PASCAL AbortPrintJob( HPRTJOB hPJ ) ;
BOOL     FAR PASCAL DestroyPrintJob( HPRTJOB hPJ ) ;
LPSTR    FAR PASCAL PrintJobErrorStr( PRTERR pe ) ;

/************************************************************************
 *      Module:  version.c
 ***********************************************************************/
LPSTR FAR PASCAL GetWINPRT2Version( LPSTR lpszVersion ) ;

/************************************************************************
 *      Module:  winprt2.c
 ***********************************************************************/
HANDLE FAR PASCAL GetWINPRT2hInst( void ) ;

/************************************************************************
 *      Module:  prtfile.c
 ***********************************************************************/

//
// The following structure describes the page.
//
#define HEADER_STRLEN   256
#define FOOTER_STRLEN   256

typedef struct tagPAGEDESC
{
   char     szHeader[ HEADER_STRLEN ] ;
   char     szFooter[ FOOTER_STRLEN ] ;

   LOGFONT  lfText ;
   LOGFONT  lfHF ;

   RECT     rcMargins ;
} PAGEDESC, *PPAGEDESC, FAR *LPPAGEDESC ;


BOOL FAR PASCAL
   WinPrtPrintFile( HWND hwndParent, 
                    LPSTR lpszDriver, LPSTR lpszDevice, LPSTR lpszPort,
                    LPDEVMODE lpDM, LPPAGEDESC lpPageDesc, LPSTR lpszFile ) ;



/************************************************************************
 * End of File: prtcore.h
 ***********************************************************************/

