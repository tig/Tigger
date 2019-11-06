/*
   ws_init.c - One time init routines for WinPrint

   (c) Copyright 1990, Charles E. Kindel, Jr.

*/

#include "PRECOMP.H"

#include "dlgs.h"
#include "isz.h"
#include "winprint.h"
#include "ws_dlg.h"
#include "ws_init.h"
#include "ws_open.h"
#include "ws_files.h"
#include "ws_reg.h"

extern char    **__argv ;
extern int     __argc ;


static BOOL       bError ;
static WORD       wTemp ;
static OPTIONS    CmdLineOpt ;

static BOOL       f_wHeaderMask ;   
static BOOL       f_wFooterMask ;   
static BOOL       f_bHFMargins ;    
static BOOL       f_bExpandTabs ;   
static BOOL       f_bInches ;
static BOOL       f_nTabSize ;      
static BOOL       f_nTopMargin ;    
static BOOL       f_nBottomMargin ; 
static BOOL       f_nLeftMargin ;   
static BOOL       f_nRightMargin ;  
static BOOL       f_bClose ;
static BOOL       f_bMinimize ;
static BOOL       f_bSeparateJobs ;

BOOL NEAR PASCAL  DoOption (PSTR psOption) ;
WORD NEAR PASCAL  StyleMask (PSTR psString) ;
WORD NEAR PASCAL  HFMask (PSTR psString) ;

BOOL FAR PASCAL WinPrintInit (HANDLE hInstance)
{
   WNDCLASS    wc ;

   wc.style             = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
   wc.lpfnWndProc       = fnMain ;
   wc.cbClsExtra        = 0 ;
   wc.cbWndExtra        = DLGWINDOWEXTRA ;
   wc.hInstance         = hInstance ;
   wc.hIcon             = LoadIcon( hInstance,
                                    MAKEINTRESOURCE( IDI_WINPRINT ) ) ;
   wc.hCursor           = LoadCursor (NULL, IDC_ARROW) ;
   wc.hbrBackground     = (HBRUSH)(COLOR_WINDOW + 1) ;
   wc.lpszMenuName      = GRCS(IDS_APPNAME) ;
   wc.lpszClassName     = GRCS(IDS_APPNAME) ;

   DP1( hWDB, "lpszMenuName = %s", wc.lpszMenuName ) ;
   DP1( hWDB, "lpszClassName = %s", wc.lpszClassName ) ;

   return RegisterClass (&wc) ;
}

#if 0

/****************************************************************
 *  GLOBALHANDLE FAR PASCAL GetStrings( hInst )
 *
 *  Description: 
 *
 *    Fills rgsz[] with resource strings.
 *
 *  Comments:
 *
 ****************************************************************/
GLOBALHANDLE FAR PASCAL GetStrings( HANDLE hInst )
{
   GLOBALHANDLE   ghMem ;
   LPSTR          lpMem ;
   LPSTR          lpCur ;
   short          i ;
   short          n ;

   if (ghMem = GlobalAlloc( GHND, (LAST_IDS - FIRST_IDS + 1) * MAX_INI_LEN ))
   {
      if (!(lpMem = GlobalLock( ghMem )))
      {
         GlobalFree( ghMem ) ;
         return FALSE ;
      }

      /*
       * Load each string and keep it's pointer
       */
      lpCur = lpMem ;

      for (i = FIRST_IDS ; i <= LAST_IDS ; i++)
      {
         if (n = LoadString( hInst, i, lpCur, MAX_INI_LEN ))
         {
            rglpsz[i] = lpCur ;
            lpCur += n + 1 ;
         }
         else
         {
            DP1( hWDB, "LoadString %d failed", i ) ;
            GlobalUnlock( ghMem ) ;
            GlobalFree( ghMem ) ;
            return NULL ;
         }
      }

      /*
       * Now reallocate the block so it is just big enough
       */
      GlobalReAlloc( ghMem, (lpCur - lpMem + 16), GHND ) ;
      GlobalLock( ghMem ) ;

      return ghMem ;
   }
   else
      return NULL ;

   return NULL ;

} /* GetStrings()  */

#endif

/****************************************************************
 *  BOOL FAR PASCAL DoCmdLine( HWND hDlg, char *szResult )
 *
 *  Description: 
 *
 *    Processes the command line.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL DoCmdLine( HWND hDlg, char *szResult )
{
   int      i ;

   memset( &CmdLineOpt, 0, sizeof( OPTIONS ) ) ;

   bError = FALSE ;
   for (i = 1 ; i < __argc ; i++)
   {
      DP3( hWDB, "__argv[%d] = '%s'", i, (LPSTR)__argv[i] ) ;
      if ((*__argv[i] == '/') || (*__argv[i] == '-'))
      {
         if (!DoOption (__argv[i]))
         {
            lstrcpy (szResult, __argv[i]) ;
            bError = TRUE ;
            break ;
         }
      }
      else
      {
         if (!SelectFiles (__argv[i]))
         {
            lstrcpy (szResult, __argv[i]) ;
            bError = TRUE ;
            break ;
         }
      }
   }

   if (bError)
      return FALSE ;

   return TRUE ;
} /* DoCmdLine()  */

/****************************************************************
 *  VOID FAR PASCAL MergeCmdLine( VOID )
 *
 *  Description: 
 *
 *    Merges 'CmdLineOpt' into 'Options'
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL MergeCmdLine( VOID )
{

   if (f_wHeaderMask)
      Options.wHeaderMask = CmdLineOpt.wHeaderMask ;

   if (f_wFooterMask)
      Options.wFooterMask = CmdLineOpt.wFooterMask ;

   if (f_bHFMargins)
      Options.bHFMargins = CmdLineOpt.bHFMargins ;

   if (f_bExpandTabs)
      Options.bExpandTabs = CmdLineOpt.bExpandTabs ;

   if (f_bInches)
      Options.bInches = CmdLineOpt.bInches ;

   if (f_nTabSize)
      Options.nTabSize = CmdLineOpt.nTabSize ;

   if (f_nTopMargin)
      Options.nTopMargin = CmdLineOpt.nTopMargin ;

   if (f_nBottomMargin)
      Options.nBottomMargin = CmdLineOpt.nBottomMargin ;

   if (f_nLeftMargin)
      Options.nLeftMargin = CmdLineOpt.nLeftMargin ;

   if (f_nRightMargin)
      Options.nRightMargin = CmdLineOpt.nRightMargin ;

   if (f_bClose)
      Options.bClose = CmdLineOpt.bClose ;

   if (f_bMinimize)
      Options.bMinimize = CmdLineOpt.bMinimize ;

   if (f_bSeparateJobs)
      Options.bSeparateJobs = CmdLineOpt.bSeparateJobs ;

} /* MergeCmdLine()  */

/****************************************************************
 *  BOOL NEAR PASCAL DoOption( PSTR psOption )
 *
 *  Description: 
 *
 *    Processes each option on the command line.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL DoOption( PSTR psOption )
{
   PSTR  psString ;

   psString = psOption ;
   /* check to see if it's a +,- option */
   if (!strchr( psString, ':'))
   {
      psString++ ;

      switch (toupper(*psString))
      {
         case 'G':         /* GO! */
            fGo = TRUE ;
            break ;

         case 'C':            /* Close opt */
            psString++ ;
            switch (toupper(*psString))
            {
               case '+' :
                  CmdLineOpt.bClose = TRUE ;
                  f_bClose = TRUE ;
                  break ;

               case '-' :
                  CmdLineOpt.bClose = FALSE ;
                  f_bClose = TRUE ;
                  break ;
               default:
                  return FALSE ;
            }
            break ;
         
         case 'M':
            psString++ ;
            switch (toupper(*psString))
            {
               case '+':
                  CmdLineOpt.bMinimize = TRUE ;
                  f_bMinimize = TRUE ;
                  break ;

               case '-':
                  CmdLineOpt.bMinimize = FALSE ;
                  f_bMinimize = TRUE ;
                  break ;
               default:
                  return FALSE ;
            }
            break ;

         case 'H':
            psString++ ;
            if (toupper(*psString) == 'M')
            {
               psString++ ;

               switch (toupper(*psString))
               {
                  case '+':
                     CmdLineOpt.bHFMargins = TRUE ;
                     f_bHFMargins = TRUE ;
                     break ;

                  case '-':
                     CmdLineOpt.bHFMargins = FALSE ;
                     f_bHFMargins = TRUE ;
                     break ;

                  default:
                     return FALSE ;
               }
            }
            else
               return FALSE ;
            break ;

         case 'J':  /* Print as sep jobs */
            psString++ ;
            if (toupper(*psString) == 'M')
            {
               psString++ ;
               switch (toupper(*psString))
               {
                  case '+':
                     CmdLineOpt.bSeparateJobs = TRUE ;
                     f_bSeparateJobs = TRUE ;
                     break ;

                  case '-':
                     CmdLineOpt.bSeparateJobs = FALSE ;
                     f_bSeparateJobs = TRUE ;
                     break ;

                  default:
                     return FALSE ;
               }
            }
            else
               return FALSE ;
            break ;


         default:
            return FALSE ;
      }
   }
   else  /* We have a colon (:) */
   {
      psString++ ;
      switch (toupper(*psString))
      {
         /*
          * In the case of a configuration name we just set
          * szCurrentConfig to be our config.
          */
         case 'S':   /* Configuration name */
            ++psString ;
            if (lstrlen(++psString) <= CFG_NAME_LEN)
            {
               lstrcpy (szCurrentConfig, psString) ;
               bConfigOnCmdLine = TRUE ;
            }
            else
               return FALSE ;
            break ;


         case 'H' : /* options beginning with an 'H' */
            psString++ ;
            switch (toupper (*psString))
            {
               case ':':  /* Header options */
                  wTemp = CmdLineOpt.wHeaderMask ;
                  f_wHeaderMask = TRUE ;
                  if (-1 == (CmdLineOpt.wHeaderMask = HFMask (++psString)))
                  {
                     CmdLineOpt.wHeaderMask = 0 ;
                     return FALSE ;
                  }
               break ;

               default:
                  return FALSE ;
            }
            break ;

         case 'F':   /* Footer options (only thing that begins with an F) */
            psString++ ;
            wTemp = CmdLineOpt.wFooterMask ;
            f_wFooterMask = TRUE ;
            if (-1 == (CmdLineOpt.wFooterMask = HFMask (++psString)))
            {
               CmdLineOpt.wFooterMask = 0 ;
               return FALSE ;
            }
            break ;

         case 'M':   /* Margin settings.  We assume CmdLineOpt.bInches is valid */
            psString++ ;
            switch (toupper (*psString))
            {
               case 'T':  /* top margin */
                  psString += 2 ;
                  CmdLineOpt.nTopMargin = (int)(1000 *
                     (double)((CmdLineOpt.bInches ? 1.0 : 0.254) *
                     atof (psString))) ;
                  f_nTopMargin = TRUE ;
                  break ;

               case 'L':   /* left margin */
                  psString += 2 ;
                  CmdLineOpt.nLeftMargin = (int)(1000 *
                     (double)((CmdLineOpt.bInches ? 1.0 : 0.254) *
                     atof (psString))) ;
                  f_nLeftMargin = TRUE ;
                  break ;

               case 'B':   /* bottom margin */
                  psString += 2 ;
                  CmdLineOpt.nBottomMargin = (int) (1000 * (double)((CmdLineOpt.bInches ? 1.0 : 0.254) * atof (psString))) ;
                  f_nBottomMargin = TRUE ;
                  break ;

               case 'R':   /* right margin */
                  psString += 2;
                  CmdLineOpt.nRightMargin = (int) (1000 * (double)((CmdLineOpt.bInches ? 1.0 : 0.254) * atof (psString))) ;
                  f_nRightMargin = TRUE ;
                  break ;

               default:
                  return FALSE ;
            }
            break ;

         case 'U':   /* Units */
            psString += 2 ;
            switch (toupper (*psString))
            {
               case 'I':
                  psString++ ;
                  if (toupper (*psString) == 'N')
                  {
                     CmdLineOpt.bInches = TRUE ;
                     f_bInches = TRUE ;
                  }
                  else
                     return FALSE ;
                  break ;

               case 'C':
                  psString++ ;
                  if (toupper (*psString) == 'M')
                  {
                     CmdLineOpt.bInches = FALSE ;
                     f_bInches = TRUE ;
                  }
                  else
                     return FALSE ;
                  break ;

               default:
                  return FALSE ;
            }
            break ;

         default:
            return FALSE ;
      } /* switch */
   } /* else */
   return TRUE ;
} /* DoOption()  */


WORD NEAR PASCAL StyleMask( PSTR psString )
{
   for (;*psString != '\0' ; psString++)
      switch (toupper (*psString))
      {
         case 'I':   /* italic */
            wTemp = wTemp | HF_ITALIC ;
            break ;

         case 'B':   /* bold */
            wTemp = wTemp | HF_BOLD ;
            break ;

         case 'U':   /* underline */
            wTemp = wTemp | HF_UNDERLINE ;
            break ;

         case 'N':   /* normal */
            return 0 ;

         default:
            return (WORD) -1 ;
      }
   return wTemp ;
}

WORD NEAR PASCAL HFMask( PSTR psString )
{
   for (;*psString != '\0' ; psString++)
      switch (toupper (*psString))
      {
         case 'F':   /* filename */
            wTemp |= HF_FNAME ;
            break ;

         case 'N':   /* page number */
            wTemp |= HF_PAGENUM ;
            break ;

         case 'R':   /* date revised */
            wTemp &= ~HF_PDATE ;
            wTemp |= HF_RDATE ;
            break ;

         case 'P':   /* date printed */
            wTemp &= ~HF_RDATE ;
            wTemp |= HF_PDATE ;
            break ;

         case '-':   /* nothing */
            return 0 ;

         default:
            return (WORD)-1 ;
      }
   return wTemp ;
} 
