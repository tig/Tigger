/************************************************************************
 *
 *  Module:  WINTIME.C
 *
 *  System:  Microsoft C 6.00A, Windows 3.00
 *
 * Remarks:
 *
 *    This module contains functions for managing time and date strings
 *    using internationalization.
 *
 *    The functions in this module assume that all WIN.INI international
 *    settings are valid.  If the settings are not there (which is the
 *    case until a change is made through control panel) defaults are
 *    used.
 *
 *    If the WIN.INI strings (such as the sLongDate string) is invalid
 *    in someway (i.e. it was NOT written by Control Panel) this code
 *    will probably break.
 *
 *    The ResetInternational() function reads the day and month strings
 *    from the EXE or DLL file that exports the hInst instance handle.
 *    It only reads these strings the first time it is called.
 *
 * History:
 *
 *   01/22/91  - First version.
 *   02/19/91  - Fixed bug with default settings (i.e. none) in WIN.INI
 *   03/13/91  - Fixed documentation, cleaned up code some.
 *   03/15/91  - Previous version would gp fault if sLongDate did not have
 *               single quotes around the separator strings.  Control Panel
 *               always writes with the quotes, but some other application
 *               appears to be modifying this string and does not add the
 *               quotes.  The WININI2.TXT documents sLongDate as not having
 *               quotes, so this should have been done in the first place
 *               anyway.
 *
 *   11/29/91  - Discovered that by changing the country information in
 *               control panel, it will write the longdate format withou
 *               quotes.  Also discovered that I am not
 *               parsing correctly in this case.  
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
//
// Define WINDLL so that C6 with the -W3 switch will flag SS!=DS issues
// for us.  Since this code can be put into a DLL, we want to make sure
// we have no SS!=DS issues.
//
#define STRICT
#define WINDLL

#include <windows.h>
#include <time.h>
#include <string.h>

#ifdef SAMPLE
   #include "wintime.h"
#else
   #include "..\inc\isz.h"
   #include "..\inc\wintime.h"
#endif

/************************************************************************
 * Imported variables
 ************************************************************************/
//
// This should be the hInst of the module that contains the string
// resources for the day and month strings.  If these functions are
// put into a DLL, hInst should be the hModule passed to LibMain
//
extern HANDLE hinstApp ;


/************************************************************************
 * Local Defines
 ************************************************************************/
// 
// SZDEF_LONGDATE is the default "long" date format string
//
#define SZDEF_LONGDATE  "ddd', 'MMMM' 'dd', 'yyyy"
#define LEN_LONGDATE    35

//
// SZDEF_SHORTDATE is the default "short" date format string
//
#define SZDEF_SHORTDATE "MM/dd/yy"
#define LEN_SHORTDATE   12

#define NUM_DAYSOFWEEK     7
#define NUM_MONTHSINYEAR   12
#define LEN_LONGWEEKDAY    13
#define LEN_SHORTWEEKDAY   5
#define LEN_LONGMONTH      13
#define LEN_SHORTMONTH     5

/************************************************************************
 * Macros 
 ************************************************************************/
//
// These macros make taking apart the "tm" time structure easier.
//
#define YEAR(datetime)  (datetime -> tm_year % 100)
#define MONTH(datetime) (datetime -> tm_mon  + 1)
#define MDAY(datetime)  (datetime -> tm_mday)
#define WDAY(datetime)  (datetime -> tm_wday)
#define HOUR(datetime)  (datetime -> tm_hour)
#define MIN(datetime)   (datetime -> tm_min)
#define SEC(datetime)   (datetime -> tm_sec)

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
LPSTR NEAR PASCAL TimeFormatLongDate( LPSTR lpszDate,
                                      struct tm FAR *lpTM, WORD wFlags ) ;

LPSTR NEAR PASCAL TimeFormatShortDate( LPSTR lpszDate,
                                       struct tm FAR *lpTM, WORD wFlags ) ;

LPSTR NEAR PASCAL NextSepChar( LPSTR lpsz ) ;

LPSTR NEAR PASCAL NextNonSepChar( LPSTR lpsz ) ;

char NEAR PASCAL GetOrderIndicator( LPSTR lpsz ) ;
VOID NEAR PASCAL GetDayOfWeek( LPSTR lpsz, LPSTR lpszFmt, struct tm FAR *lpTM, WORD wFlags ) ;
VOID NEAR PASCAL GetMonth( LPSTR lpsz, LPSTR lpszFmt, struct tm FAR *lpTM, WORD wFlags ) ;
VOID NEAR PASCAL GetDay( LPSTR lpsz, LPSTR lpszFmt, struct tm FAR *lpTM, WORD wFlags ) ;
VOID NEAR PASCAL GetYear( LPSTR lpsz, LPSTR lpszFmt, struct tm FAR *lpTM, WORD wFlags ) ;

VOID NEAR PASCAL
ParseSeparators( LPSTR lpszLongDate,
                 LPSTR lpszSep1, LPSTR lpszSep2, LPSTR lpszSep3 ) ;

LPSTR NEAR PASCAL GetSeparator( LPSTR lpsz, LPSTR lpszSep ) ;

/************************************************************************
 * Local Variables
 *
 * Remember:  If this code goes into a DLL these variables will reside in
 * the DLLs data segment.  Thus 1) The DLL should NOT be a CODE only DLL,
 * and 2) these variables will be shared by all applications using the DLL.
 *
 *    These variables must be initialized by calling the
 *    TimeResetInternational() function.  If you are putting this in
 *    in a DLL then call this function in your LibMain.
 *
 *    Then, in your application, call TimeResetInternational everytime
 *    you detect that the [intl] section of the WIN.INI has changed.
 *    You do this by processing the WM_WININICHANGE message and
 *    comparing (LPSTR)lParam to "intl".
 *
 ************************************************************************/
// 
// These variables are initialized when ResetInternational() is called.
// They are filled with the corresponding values in the [Intl] section
// of the WIN.INI file.
//
char  sDate[3] ;
char  sTime[3] ;
char  sAMPM[3][6] ;

char  sShortDate[LEN_SHORTDATE] ;
char  sLongDate[LEN_LONGDATE] ;

int   iTime ;
int   iTLzero ;

//
// These strings contain the month and day of week strings.
// (i.e. "January", "Jan", "Sunday", "Sun")
// They are loaded from the application's (or DLL's) string table
// when ResetInternational() is called the first time.
//
char  szWday      [NUM_DAYSOFWEEK]     [LEN_LONGWEEKDAY] ;
char  szShortWday [NUM_DAYSOFWEEK]     [LEN_SHORTWEEKDAY] ;
char  szMonth     [NUM_MONTHSINYEAR]   [LEN_LONGMONTH] ;
char  szShortMonth[NUM_MONTHSINYEAR]   [LEN_SHORTMONTH] ;

//
// These variables are used by the functions to get the current time
// and date.
//
LONG           lTime ;
struct tm FAR  *lpTM ;

/************************************************************************
 * Exported Functions
 ************************************************************************/
/*************************************************************************
 *  LPSTR FAR PASCAL TimeGetCurDate( LPSTR lpszDate, WORD wFlags )
 *
 *  Description:
 *    Function returns the current date, formatted the way windows
 *    wants it!
 *
 *  Arguments:
 *    Type/Name
 *             Description
 *
 *    LPSTR lpszDate
 *             Points to a buffer that is to recieve the date.  Must be
 *             large enough to hold the longest date possible (64 bytes
 *             should do it).
 *
 *    struct tm *lpTM
 *             Pointer to a "tm" structure (from time.h) that contains
 *             the time and date you want formatted.
 *
 *    WORD  wFlags
 *             Specifies flags that modify the behavior of the function.
 *             May be any of the following.
 *
 *          Flag              Meaning
 *          0                 The date will be formatted exactly the way
 *                            Control Panel does it, using Windows'
 *                            international settings.  The date will be in
 *                            the Long Date format (Sunday, January 1, 1991).
 *
 *          DATE_SHORTDATE    Forces the date to be formatted using the
 *                            Short Date Format (1/01/91).
 *
 *          DATE_NODAYOFWEEK  The date will be formatted without the
 *                            day of week.  This flag may not be used with
 *                            the DATE_SHORTDATE flag.
 *                            (i.e. January 1, 1991)
 *
 *  Comments:
 *
 *************************************************************************/
LPSTR FAR PASCAL TimeGetCurDate( LPSTR lpszDate, WORD wFlags )
{
   time( &lTime ) ;
   lpTM = localtime( &lTime ) ;
   return TimeFormatDate( lpszDate, lpTM, wFlags ) ;
}

/*************************************************************************
 *  LPSTR FAR PASCAL TimeFormatDate( LPSTR lpszDate,
 *                                   struct tm FAR *lpTM,
 *                                   WORD wFlags )
 *
 *  Description:
 *    Function returns the date, specified in the tm structure,
 *    formatted the way windows
 *    wants it!
 *
 *  Arguments:
 *    Type/Name
 *             Description
 *
 *    LPSTR lpszDate
 *             Points to a buffer that is to recieve the date.  Must be
 *             large enough to hold the longest date possible (64 bytes
 *             should do it).
 *
 *    struct tm *lpTM
 *             Pointer to a "tm" structure (from time.h) that contains
 *             the time and date you want formatted.
 *
 *    WORD  wFlags
 *             Specifies flags that modify the behavior of the function.
 *             May be any of the following.
 *
 *          Flag              Meaning
 *          0                 The date will be formatted exactly the way
 *                            Control Panel does it, using Windows'
 *                            international settings.  The date will be in
 *                            the Long Date format (Sunday, January 1, 1991).
 *
 *          DATE_SHORTDATE    Forces the date to be formatted using the
 *                            Short Date Format (1/01/91).
 *
 *          DATE_NODAYOFWEEK  The date will be formatted without the
 *                            day of week.  This flag may not be used with
 *                            the DATE_SHORTDATE flag.
 *                            (i.e. January 1, 1991)
 *
 *  Comments:
 *    The functions related to this are long and complex.
 *    Because of the way Windows
 *    stores the internationalization info in the WIN.INI there is
 *    really no elegant way of doing this.  If you come up with
 *    a cleaner way please let me know.
 *
 *************************************************************************/
LPSTR FAR PASCAL TimeFormatDate( LPSTR lpszDate,
                                 struct tm FAR *lpTM,
                                 WORD wFlags )
{                               
   // 
   // See if it's to be short or long
   //
   if (wFlags & DATE_SHORTDATE)
      TimeFormatShortDate( lpszDate, lpTM, wFlags ) ;
   else  // Longdate
      TimeFormatLongDate( lpszDate, lpTM, wFlags ) ;

   return lpszDate ;

}/* TimeGetCurDate() */

/*************************************************************************
 *  LPSTR FAR PASCAL TimeGetCurTime( LPSTR lpszTime, WORD wFlags )
 *
 *  Description: 
 *    Function returns the current time, formatted the way windows
 *    wants it!
 *
 *  Arguments:
 *    Type/Name
 *             Description
 *
 *    LPSTR lpszTime
 *             Points to a buffer that is to recieve the formatted time.
 *             Must be large enough to hold the longest date possible
 *             (64 bytes should do it).
 *
 *    WORD  wFlags
 *             Specifies flags that modify the behavior of the function.
 *             May be any of the following.
 *
 *          Flag              Meaning
 *          0                 The time will be formatted exactly the way
 *                            Control Panel does it, using Windows'
 *                            international settings. (i.e. 1:00:00 AM)
 *
 *          TIME_12HOUR       The time will be formatted in 12 hour format
 *                            regardless of the WIN.INI internationalization
 *                            settings.  This flag may not be used with the
 *                            TIME_24HOUR flag. (i.e. 2:32:10 PM)
 *                            
 *          TIME_24HOUR       The time will be formatted in 24 hour format
 *                            regardless of the WIN.INI internationalization
 *                            settings.  This flag may not be used with the
 *                            TIME_12HOUR flag. (i.e. 14:32:10)
 *
 *          TIME_NOSECONDS    The time will be formatted without seconds.
 *                            (i.e. 2:32 PM)
 *
 *  Comments:
 *
 *************************************************************************/
LPSTR FAR PASCAL TimeGetCurTime( LPSTR lpszTime, WORD wFlags )
{
   // 
   // Get current time
   //
   time( &lTime ) ;
   lpTM = localtime( &lTime ) ;

   return TimeFormatTime( lpszTime, lpTM, wFlags ) ;
}


/*************************************************************************
 *  LPSTR FAR PASCAL TimeFormatTime( LPSTR lpszTime,
 *                                   struct tm FAR *lpTM,
 *                                   WORD wFlags )
 *
 *  Description:
 *    Function returns the time, specified in the tm structure,
 *    formatted the way windows
 *    wants it!
 *
 *  Arguments:
 *    Type/Name
 *             Description
 *
 *    LPSTR lpszTime
 *             Points to a buffer that is to recieve the formatted time.
 *             Must be large enough to hold the longest date possible
 *             (64 bytes should do it).
 *
 *    struct tm *lpTM
 *             Pointer to a "tm" structure (from time.h) that contains
 *             the time and date you want formatted.
 *
 *    WORD  wFlags
 *             Specifies flags that modify the behavior of the function.
 *             May be any of the following.
 *
 *          Flag              Meaning
 *          0                 The time will be formatted exactly the way
 *                            Control Panel does it, using Windows'
 *                            international settings. (i.e. 1:00:00 AM)
 *
 *          TIME_12HOUR       The time will be formatted in 12 hour format
 *                            regardless of the WIN.INI internationalization
 *                            settings.  This flag may not be used with the
 *                            TIME_24HOUR flag. (i.e. 2:32:10 PM)
 *                            
 *          TIME_24HOUR       The time will be formatted in 24 hour format
 *                            regardless of the WIN.INI internationalization
 *                            settings.  This flag may not be used with the
 *                            TIME_12HOUR flag. (i.e. 14:32:10)
 *
 *          TIME_NOSECONDS    The time will be formatted without seconds.
 *                            (i.e. 2:32 PM)
 *
 *  Comments:
 *
 *************************************************************************/
LPSTR FAR PASCAL TimeFormatTime( LPSTR lpszTime, struct tm FAR *lpTM, WORD wFlags )
{
   char szFmt[32] ;
   char szTemp[32] ;

   //
   // Leading zero?
   //
   if (iTLzero)
      lstrcpy( szFmt, "%02d" ) ;
   else
      lstrcpy( szFmt, "%d" ) ;

   //
   // See if we want seconds...
   // 
   if (wFlags & TIME_NOSECONDS)
   {
      wsprintf( szTemp, "%s%s%s",
               (LPSTR)szFmt,
               (LPSTR)sTime,
               (LPSTR)"%02d" ) ;
   }
   else               // HH :MM :SS
      wsprintf( szTemp, "%s%s%s%s%s",
               (LPSTR)szFmt,
               (LPSTR)sTime,
               (LPSTR)"%02d",
               (LPSTR)sTime,
               (LPSTR)"%02d" ) ;

   //
   // see if we want 12 hour or 24 hour format.
   //
   if (wFlags & TIME_24HOUR || iTime)
   {
      if (wFlags & TIME_NOSECONDS)
         wsprintf( lpszTime, szTemp, HOUR( lpTM ), MIN( lpTM ) ) ;
      else
         wsprintf( lpszTime, szTemp, HOUR( lpTM ), MIN( lpTM ), SEC( lpTM ) ) ;
   }
   else
   {
      lstrcat( szTemp, " %s" ) ;

      if (wFlags & TIME_NOSECONDS)
         wsprintf( lpszTime, szTemp,
                   (HOUR( lpTM ) % 12) ? (HOUR( lpTM ) % 12) : 12,
                   MIN( lpTM ),
                   (LPSTR)sAMPM[HOUR( lpTM ) / 12] ) ;
      else
         wsprintf( lpszTime, szTemp,
                   (HOUR( lpTM ) % 12) ? (HOUR( lpTM ) % 12) : 12,
                   MIN( lpTM ), SEC( lpTM ),
                   (LPSTR)sAMPM[HOUR( lpTM ) / 12] ) ;
   }

   return lpszTime ;   

}/* TimeGetCurTime() */


/*************************************************************************
 *  VOID FAR PASCAL TimeResetInternational( VOID ) ;
 *
 *  Description:
 *    This function sets some local static variables to Windows' current
 *    time and date settings.
 *
 *    It should be called everytime a WM_WININICHANGE is sent for the
 *    intl section.
 *
 *    The variables here are in the DLLs data segment so if one app
 *    changes the stuff, the others will see it.
 *
 *  Comments:
 *
 *************************************************************************/
VOID FAR PASCAL TimeResetInternational( VOID )
{
   static char szIntl[] = "intl" ;

   iTime = GetProfileInt( szIntl, "iTime", 0 ) ;
   iTLzero = GetProfileInt( szIntl, "iTLzero", 1 ) ;

   GetProfileString( szIntl, "sDate", "/", sDate,  2 ) ;

   GetProfileString( szIntl, "sTime", ":", sTime,  2 ) ;

   GetProfileString( szIntl, "s1159", "AM", sAMPM[0],  5 ) ;

   GetProfileString( szIntl, "s2359", "PM", sAMPM[1],  5 ) ;

   GetProfileString( szIntl, "sShortDate", SZDEF_SHORTDATE, sShortDate,
      LEN_SHORTDATE ) ;

   GetProfileString( szIntl, "sLongDate", SZDEF_LONGDATE, sLongDate,
      LEN_LONGDATE ) ;

   if (*szWday[0] == '\0')
   {
      WORD i ;

      for (i = 0 ; i <= 6 ; i++)
         LoadString( hinstApp, IDS_SUNDAY + i, szWday[i], 12 ) ;

      //
      // In an international setting "Sunday" may not be abbrieviated
      // as "Sun".
      //
      for (i = 0 ; i <= 6 ; i++)
         LoadString( hinstApp, IDS_SUNDAY_SHORT + i, szShortWday[i], 4 ) ;

      for (i = 0 ; i <= 11 ; i++)
         LoadString( hinstApp, IDS_JANUARY + i, szMonth[i],  12 ) ;

      for (i = 0 ; i <= 11 ; i++)
         LoadString( hinstApp, IDS_JANUARY_SHORT + i, szShortMonth[i],  4 ) ;
   }

}/* TimeResetInternational() */

/************************************************************************
 * Local Functions
 ************************************************************************/

/*************************************************************************
 *
 *  LPSTR NEAR PASCAL TimeFormatShortDate
 *
 *  NOTE:  This code is somewhat inefficient.  It makes heavy use of
 *         C Runtime functions to parse the strings.  This code was
 *         written so that it could be used by others, thus there is
 *         some emphasis on readability over efficiency.
 *
 *************************************************************************/
LPSTR NEAR PASCAL
TimeFormatShortDate( LPSTR lpszDate, struct tm FAR *lpTM, WORD wFlags )
{
   LPSTR lpCur ;
   BOOL  fDayLZ = FALSE ;
   BOOL  fMoLZ = FALSE ;
   BOOL  fYrCent = FALSE ;
   char  cSep = '/' ;
   char  szFmt[32] ;

   //
   // first find out if there's supposed to be a leading 0 on the day
   //
   if ((lpCur = _fstrchr( sShortDate, 'd' )) &&
       *(lpCur+1) == 'd' )
      fDayLZ = TRUE ;

   // 
   // month?
   //
   if ((lpCur = _fstrchr( sShortDate, 'M' )) &&
       *(lpCur+1) == 'M' )
      fMoLZ = TRUE ;


   // 
   // Does year have the century?
   //
   if ((lpCur = _fstrchr( sShortDate, 'y' )) &&
       *(lpCur+1) == 'y' &&
       *(lpCur+2) == 'y')
      fYrCent = TRUE ;

   // 
   // Get the separtor
   //
   if (lpCur = _fstrrchr( sShortDate, 'M' ))
      cSep = *(lpCur + 1) ;

   switch( *sShortDate )
   {

      case 'd' :
         wsprintf( szFmt, "%%0%dd%c%%0%dd%c%%%dd",
                     fDayLZ ? 2 : 1,
                     cSep,
                     fMoLZ  ? 2 : 1,
                     cSep,
                     fYrCent ? 4 : 2 ) ;
         wsprintf( lpszDate, szFmt,
                     MDAY ( lpTM ),
                     MONTH( lpTM ),
                     fYrCent ? YEAR( lpTM ) + 1900 : YEAR( lpTM ) ) ;
         break ;

      case 'y' :
         wsprintf( szFmt, "%%%dd%c%%0%dd%c%%0%dd",
                     fYrCent ? 4 : 2,
                     cSep,
                     fDayLZ ? 2 : 1,
                     cSep,
                     fMoLZ  ? 2 : 1 ) ;
         wsprintf( lpszDate, szFmt,
                     fYrCent ? YEAR( lpTM ) + 1900 : YEAR( lpTM ),
                     MDAY ( lpTM ),
                     MONTH( lpTM ) ) ;
         break ;

      case 'M' :
      default:
         wsprintf( szFmt, "%%0%dd%c%%0%dd%c%%%dd",
                     fMoLZ  ? 2 : 1,
                     cSep,
                     fDayLZ ? 2 : 1,
                     cSep,
                     fYrCent ? 4 : 2 ) ;

         wsprintf( lpszDate, szFmt,
                     MONTH( lpTM ),
                     MDAY ( lpTM ),
                     fYrCent ? YEAR( lpTM ) + 1900 : YEAR( lpTM ) ) ;
         break ;
   }
   return lpszDate ;
}

/*************************************************************************
 *
 *  LPSTR NEAR PASCAL TimeFormatLongDate
 *
 *  NOTE:  This code is somewhat inefficient.  It makes heavy use of
 *         C Runtime functions to parse the strings.  This code was
 *         written so that it could be used by others, thus there is
 *         some emphasis on readability over efficiency.
 *
 *************************************************************************/
LPSTR NEAR PASCAL
TimeFormatLongDate( LPSTR lpszDate, struct tm FAR *lpTM, WORD wFlags ) 
{
   char  szDateFmt[LEN_LONGDATE+1] ;

   char  szMo[32],
         szDay[32],
         szYr[32],
         szDOW[32] ;

   /*
    * There are two format's that we have to deal with.  The first is
    * the "quoted" format, which is what Control Panel writes when the
    * user changes a date format setting via the "Change..." button.
    * This format may look like this:
    *
    *   sLongDate=dddd', 'dd' 'MMMM', 'yyyy
    *
    * Or (in the case where there is no day of week):
    *
    *   sLongDate=' 'dd' 'MMMM', 'yyyy
    *
    * The second format looks like this (note the lack of quotes):
    *
    *   sLongDate=dddd, dd MMMM, yyyy
    *
    * Or
    *
    *   sLongDate=dd MMMM, yyyy
    *
    * This is written when the user changes country info (to see this
    * choose "Canada (English)").
    *
    * There is also a "mixed" possiblity (the spanish default):
    *
    *   sLongDate=dddd d' de 'MMMM' de 'yyyy
    *
    */

   lstrcpy( szDateFmt, sLongDate ) ;

   /*
    * I hate gotos too, but this gives us a convenient way of recovering
    * from a badly formatted string.  We will goto this label after we
    * lstrcpy( szDateFmt, SZ_DEFLONGDATE ) whenever there is a format
    * problem.  That way we never return a poor looking string.
    */
BEGINFORMAT:

   /*
    * When the following functions return they contain the appropriate
    * item plus any separator characters that appear *after* the item.
    *
    */
   GetDayOfWeek( szDOW, szDateFmt, lpTM, wFlags ) ;

   GetMonth( szMo, szDateFmt, lpTM, wFlags ) ;

   GetYear( szYr, szDateFmt, lpTM, wFlags ) ;

   GetDay( szDay, szDateFmt, lpTM, wFlags ) ;


   /*
    * We now have strings containing each item.
    * All that is left to do is to figure out the correct order
    * to concatinate the strings.
    *
    * The following is a list of all possible (?) orderings...
    *                
    *    dddd', 'MMMM' 'dd', 'yyyy
    *    dddd', 'dd' 'MMMM', 'yyyy
    *    dddd', 'yyyy' 'MMMM', 'dd
    *    dddd d' de 'MMMM' de 'yyyy  (variant of #2)
    *
    *    ''MMMM' 'dd', 'yyyy
    *    ''dd' 'MMMM', 'yyyy
    *    ''yyyy' 'MMMM', 'dd
    *
    * From this it is obvious that the day of week is always first.
    * thus it should be copied into the destintion first.  
    */
   if (wFlags & DATE_NODAYOFWEEK)
      lpszDate[0] = '\0' ;
   else
      lstrcpy( lpszDate, szDOW ) ;

   /*
    * Now we need to determine the ordering of the rest.  We do this
    * by looking at the first 'd', 'y', or 'M' after the day
    * of week (if there at all).
    */
   switch (GetOrderIndicator( szDateFmt ))
   {
      case 'y':
         /*
            * The year comes first (''yyyy' 'MMMM', 'dd)
            */
         lstrcat( lpszDate, szYr ) ;
         lstrcat( lpszDate, szMo ) ;
         lstrcat( lpszDate, szDay ) ;
      break ;

      case 'd':
         /*
            * The day comes first (''dd' 'MMMM', 'yyyy)
            */
         lstrcat( lpszDate, szDay ) ;
         lstrcat( lpszDate, szMo ) ;
         lstrcat( lpszDate, szYr ) ;

      break ;

      case 'M':
         /*
            * The month comes first (''MMMM' 'dd', 'yyyy) 
            */
         lstrcat( lpszDate, szMo ) ;
         lstrcat( lpszDate, szDay ) ;
         lstrcat( lpszDate, szYr ) ;

      break ;

      default:
         /*
            * There was a problem.  Use the default.
            */
         lstrcpy( szDateFmt, SZDEF_LONGDATE ) ;
         goto BEGINFORMAT ;
   }

   return lpszDate ;

} /* TimeFormatLongDate() */


/****************************************************************
 *  char NEAR PASCAL GetOrderIndicator( LPSTR lspz )
 *
 *  Description: 
 *
 *    Given a string in the long date format return a
 *    'y', 'd', or 'M' indicating the order.  '\0' is
 *    returned if there is a problem.
 *
 ****************************************************************/
char NEAR PASCAL GetOrderIndicator( LPSTR lpsz )
{
   LPSTR    lpCur ;

   /*
    * Below are possible formats we have to deal with:
    *
    *    dddd, MMMM dd, yyyy
    *    dddd, dd MMMM, yyyy
    *    dddd, yyyy MMMM, dd
    *    dddd d' de 'MMMM' de 'yyyy
    *
    *    ddd, MMMM dd, yyyy
    *    ddd, dd MMMM, yyyy
    *    ddd, yyyy MMMM, dd
    *
    *    MMMM dd, yyyy
    *    dd MMMM, yyyy
    *    yyyy MMMM, dd
    *
    * (and minor variants thereof).
    */

   /*
    * First:  Do we have a day of week?  If we have 3 or more
    * 'd's at the beginning then we do.
    */
   if (lpsz[0] == 'd' && lpsz[1] == 'd' && lpsz[2] == 'd')
   {
      if (lpsz[3] == 'd')
         lpCur = &lpsz[4] ;
      else
         lpCur = &lpsz[3] ;

      /*
       * The default format for spain/portugal/mexico et. al. is:
       *
       *    dddd d' de 'MMMM' de 'yyyy
       *        ^
       * We want to convert this to:
       *
       *    dddd' 'd' de 'MMMM' de 'yyyy
       *        ^
       */ 
      lpCur = NextNonSepChar( lpCur ) ;

      return *lpCur ;
   }
   else
   {
      /*
       * If the first char is a quote then we are quoted so copy and
       * return.
       */
      if (lpsz[0] == '\'')
      {
         lpCur = NextNonSepChar( lpsz ) ;
         return *lpCur ;
      }

      return lpsz[0] ;
   }

} /* GetOrderIndiator()  */


/****************************************************************
 * VOID NEAR PASCAL
 *    GetDayOfWeek( LPSTR lpszDOW, LPSTR lpszFmt,
 *                   struct tm FAR *lpTM, WORD wFlags )  
 *
 *  Description: 
 *
 *    Get's the day of week string, formatted, including
 *    any following separator characters.
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL GetDayOfWeek( LPSTR lpszDOW, LPSTR lpszFmt,
                               struct tm FAR *lpTM, WORD wFlags )  
{
   lpszDOW[0] = '\0' ;

   /*
    * First:  Do we have a day of week?  If we have 3 or more
    * 'd's at the beginning then we do.
    */
   if (lpszFmt[0] == 'd' && lpszFmt[1] == 'd' && lpszFmt[2] == 'd')
   {
      /*
       * full day strings or short (3 characters)?
       */
      if (lpszFmt[3] == 'd')
         /*
          * There are four 'd's ('dddd') so it's long
          */
         lstrcpy( lpszDOW, szWday[ WDAY( lpTM ) ] ) ;
      else
         lstrcpy( lpszDOW, szShortWday[ WDAY( lpTM ) ] ) ;

      lpszDOW += lstrlen(lpszDOW);

      /*
       * Find first separator char
       */
      lpszFmt = NextSepChar( lpszFmt ) ;
   }

   /*
    * We either have a DOW or not, but there might be a leading separator
    * which we must append.
    */
   if (*lpszFmt == '\'')   
   {
      /*
       * go to next quote
       */
      lpszFmt++ ;

      while (*lpszFmt != '\'' &&
             *lpszFmt != '\0')
      {
         *lpszDOW++ = *lpszFmt++ ;
         *lpszDOW = '\0' ;
      }
   }
   else
   {
      while (*lpszFmt != 'd' &&
               *lpszFmt != 'y' &&
               *lpszFmt != 'M' &&
               *lpszFmt != '\'' &&
               *lpszFmt != '\0')
      {
         *lpszDOW++ = *lpszFmt++ ;
         *lpszDOW = '\0' ;
      }
   }

} /* GetDayOfWeek()  */



/****************************************************************
 * VOID NEAR PASCAL
 *    GetMonth( LPSTR lpszMo, LPSTR lpszFmt,
 *                   struct tm FAR *lpTM, WORD wFlags )  
 *
 *  Description: 
 *
 *    Get's the month string, formatted, including
 *    any following separator characters.
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL GetMonth( LPSTR lpszMo, LPSTR lpszFmt,
                               struct tm FAR *lpTM, WORD wFlags )  
{
   LPSTR lp ;
   short n = 0 ;
   BOOL     fInQuotes = FALSE ;

   /*
    * Skip day of week if any.
    */
   if (lpszFmt[0] == 'd' && lpszFmt[1] == 'd' && lpszFmt[2] == 'd')
      lp = NextSepChar( lpszFmt ) ;
   else
      lp = lpszFmt ;


   /*
    * Scan for first 'M' that is not between quotes.
    * lp is currently pointing at where we can start.
    */
   while (*lp != '\0')
   {
      if (*lp == '\'')
         fInQuotes = !fInQuotes ;
      else
      {
         if (!fInQuotes && *lp == 'M')
            break ;
      }
      lp++ ;
   }

   if (*lp != '\0')
   {
      if (*(lp+1) != 'M')
      {
         lpszMo[0] = '\0' ;
         return ;
      }

      /*
       * Get a count of the number of 'M's
       */
      for ( n = 0 ; *lp == 'M' ; lp++ )
         n++ ;

      switch (n)
      {
         case 1:
            wsprintf( lpszMo, "%d", MONTH( lpTM ) ) ;
         break ;

         case 2:
            wsprintf( lpszMo, "%02d", MONTH( lpTM ) ) ;
         break ;

         case 3:
            lstrcpy( lpszMo, szShortMonth[ MONTH( lpTM )-1] ) ;
         break ;

         case 4:
         default:
            lstrcpy( lpszMo, szMonth[ MONTH( lpTM )-1 ] ) ;
         break ;
      }

      lpszMo += lstrlen(lpszMo) ;
   }

   /*
    * We either have a Month or not, but there might be a separator
    * which we must append.
    */
   if (!lp)
      if (!(lp = _fstrchr( lpszFmt, 'M' )))
      {
         *lpszMo = '\0' ;
         return ;
      }

   if (*lp == '\'')   
   {
      /*
       * go to next quote
       */
      lp++ ;

      while (*lp != '\'' &&
            *lp != '\0')
      {
         *lpszMo++ = *lp++ ;
         *lpszMo = '\0' ;
      }
   }
   else
   {
      while (*lp != '\'' &&
             *lp != 'y' &&
             *lp != 'M' &&
             *lp != 'd' &&
             *lp != '\0')
      {
         *lpszMo++ = *lp++ ;
         *lpszMo = '\0' ;
      }
   }

} /* GetMonth()  */

/****************************************************************
 * VOID NEAR PASCAL
 *    GetDay( LPSTR lpszDay, LPSTR lpszFmt,
 *                   struct tm FAR *lpTM, WORD wFlags )  
 *
 *  Description: 
 *
 *    Get's the day of month string, formatted, including
 *    any following separator characters.
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL GetDay( LPSTR lpszDay, LPSTR lpszFmt,
                               struct tm FAR *lpTM, WORD wFlags )  
{
   LPSTR    lp ;
   short    n = 0 ;
   BOOL     fInQuotes = FALSE ;

   /*
    * Skip day of week if any.
    */
   if (lpszFmt[0] == 'd' && lpszFmt[1] == 'd' && lpszFmt[2] == 'd')
      lp = NextSepChar( lpszFmt ) ;
   else
      lp = lpszFmt ;


   /*
    * Scan for first 'd' that is not between quotes.
    * lp is currently pointing at where we can start.
    */
   while (*lp != '\0')
   {
      if (*lp == '\'')
         fInQuotes = !fInQuotes ;
      else
      {
         if (!fInQuotes && *lp == 'd')
            break ;
      }
      lp++ ;
   }

   if (*lp == '\0')
   {
      /*
       * There was no 'd' specifiing day.  Null string and
       * return.
       */
      lpszDay[0] = '\0' ;
      return ;
   }

   /*
    * We can either have 'd' (i.e. 5) or 'dd' (i.e. 05):
    */
   if (*(lp+1) == 'd')
      wsprintf( lpszDay, "%02d", MDAY( lpTM ) ) ;
   else
      wsprintf( lpszDay, "%d", MDAY( lpTM ) ) ;

   lpszDay += lstrlen(lpszDay) ;


   /*
    * We have the Day, but there might be a separator
    * which we must append.
    */
   if (!(lp = NextSepChar( lp )))
      return ;

   if (*lp == '\'')   
   {
      /*
       * go to next quote
       */
      lp++ ;

      while (*lp != '\'' &&
            *lp != '\0')
      {
         *lpszDay++ = *lp++ ;
         *lpszDay = '\0' ;
      }
   }
   else
   {
      while (*lp != '\'' &&
             *lp != 'y' &&
             *lp != 'M' &&
             *lp != 'd' &&
             *lp != '\0')
      {
         *lpszDay++ = *lp++ ;
         *lpszDay = '\0' ;
      }
   }

} /* GetDay()  */

/****************************************************************
 * VOID NEAR PASCAL
 *    GetYear( LPSTR lpszYr, LPSTR lpszFmt,
 *                   struct tm FAR *lpTM, WORD wFlags )  
 *
 *  Description: 
 *
 *    Get's the year string, formatted, including
 *    any following separator characters.
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL GetYear( LPSTR lpszYr, LPSTR lpszFmt,
                               struct tm FAR *lpTM, WORD wFlags )  
{
   LPSTR    lp ;
   short    n = 0 ;
   BOOL     fInQuotes = FALSE ;

   /*
    * Skip day of week if any.
    */
   if (lpszFmt[0] == 'd' && lpszFmt[1] == 'd' && lpszFmt[2] == 'd')
      lp = NextSepChar( lpszFmt ) ;
   else
      lp = lpszFmt ;


   /*
    * Scan for first 'y' that is not between quotes.
    * lp is currently pointing at where we can start.
    */
   while (*lp != '\0')
   {
      if (*lp == '\'')
         fInQuotes = !fInQuotes ;
      else
      {
         if (!fInQuotes && *lp == 'y')
            break ;
      }
      lp++ ;
   }

   if (*lp == '\0')
   {
      /*
       * There was no 'y' specifiing year.  Null string and
       * return.
       */
      lpszYr[0] = '\0' ;
      return ;
   }

   /*
    * We can either have 'yy' (i.e. 91) or 'yyyy' (i.e. 1991):
    */
   if (*(lp+1) == 'y' && *(lp+2) == 'y' && *(lp+3) == 'y')
      wsprintf( lpszYr, "%d", YEAR( lpTM ) + 1900 ) ;
   else
      wsprintf( lpszYr, "%d", YEAR( lpTM ) ) ;

   lpszYr += lstrlen(lpszYr) ;

   /*
    * We a year, but there might be a separator
    * which we must append.
    */
   if (!(lp = NextSepChar( lp )))
      return ;


   if (*lp == '\'')   
   {
      /*
       * go to next quote
       */
      lp++ ;

      while (*lp != '\'' &&
            *lp != '\0')
      {
         *lpszYr++ = *lp++ ;
         *lpszYr = '\0' ;
      }
   }
   else
   {
      while (*lp != '\'' &&
             *lp != 'y' &&
             *lp != 'M' &&
             *lp != 'd' &&
             *lp != '\0')
      {
         *lpszYr++ = *lp++ ;
         *lpszYr = '\0' ;
      }
   }

} /* GetYear()  */



/****************************************************************
 *  LPSTR NEAR PASCAL NextSepChar( LPSTR lpsz )
 *
 *  Description: 
 *
 *    This function finds the next non-format character in a
 *    sLongDate string.  The format characters are M,d,and y.
 *
 ****************************************************************/
LPSTR NEAR PASCAL NextSepChar( LPSTR lpsz )
{
   LPSTR lpszCur = lpsz ;

   while (*lpszCur && (*lpszCur == 'M' ||
                       *lpszCur == 'd' ||
                       *lpszCur == 'y'))
         lpszCur++ ;

   return lpszCur ;

} /* NextSepChar()  */

/****************************************************************
 *  LPSTR NEAR PASCAL NextNonSepChar( LPSTR lpsz )
 *
 *  Description: 
 *
 *    This function finds the next format character (i.e. 'd', 'M', or 'y').
 *
 ****************************************************************/
LPSTR NEAR PASCAL NextNonSepChar( LPSTR lpsz )
{
   LPSTR lpszCur = lpsz ;

   while (*lpszCur && (*lpszCur != 'M' &&
                       *lpszCur != 'd' &&
                       *lpszCur != 'y'
                       ))
         lpszCur++ ;

   return lpszCur ;

} /* NextNonSepChar()  */

#if 0
/****************************************************************
 *  VOID NEAR PASCAL
 *    ParseSeparators( LPSTR lpszLongDate,
 *                     LPSTR lpszSep1, LPSTR lpszSep2, LPSTR lpszSep3 )
 *
 *  Description: 
 *
 *    This function parses the sLongDate string for the 3 separators.
 *    It works with both forms of the sLongDate string.
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL
ParseSeparators( LPSTR lpszLongDate,
                 LPSTR lpszSep1, LPSTR lpszSep2, LPSTR lpszSep3 )
{
   LPSTR lpszCur ;

   if (lpszCur = GetSeparator( lpszLongDate, lpszSep1 ))
   {
      if (lpszCur = GetSeparator( lpszCur, lpszSep2 ))
      {
         if (!GetSeparator( lpszCur, lpszSep3 ))
            lpszSep3[0] = '\0' ;
      }
      else
         lpszSep2[0] = '\0' ;
   }
   else
      lpszSep1[0] = '\0' ;

} /* ParseSeparators()  */

/****************************************************************
 *  LPSTR NEAR PASCAL GetSeparator( LPSTR lpsz, LPSTR lpszSep )
 *
 *  Description:
 *
 *    This function fills lpszSep with the next separator and
 *    returns a pointer to the character after the separator.
 *
 *  Comments:
 *
 ****************************************************************/
LPSTR NEAR PASCAL GetSeparator( LPSTR lpsz, LPSTR lpszSep )
{
   LPSTR lpCur, lpEnd ;
   short i ;

   lpEnd = NULL ;

   if (lpCur = NextSepChar( lpsz ))
   {
      if (lpCur > lpsz && *(lpCur - 1) == '\'' )
      {
         for ( i = 0, lpEnd = lpCur ; *lpCur && *lpCur != '\'' ; i++, lpCur++, lpEnd++ )
            lpszSep[i] = *lpCur ;

         lpszSep[i] = '\0' ;
      }
      else
         if (lpEnd = NextNonSepChar( lpCur ))
         {
            for ( i = 0 ; lpCur < lpEnd ; i++, lpCur++)
               lpszSep[i] = *lpCur ;

            lpszSep[i] = '\0' ;
         }
   }

   return lpEnd ;

} /* GetSeparator()  */
#endif

/************************************************************************
 * End of File: wintime.c
 ************************************************************************/

