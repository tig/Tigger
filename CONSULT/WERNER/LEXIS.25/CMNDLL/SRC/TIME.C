/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL, Common DLL For Werner Apps.
 *
 *      Module:  time.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
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
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
//
// Define WINDLL so that C6 with the -W3 switch will flag SS!=DS issues
// for us.  Since this code can be put into a DLL, we want to make sure
// we have no SS!=DS issues.
//
#define WINDLL

#include <windows.h>
#include <time.h>
#include <string.h>
//#include "wintime.h"


#include "..\inc\cmndll.h"
// Include the dialog defines
#include "..\inc\cmndlld.h"

/************************************************************************
 * Imported variables
 ************************************************************************/
//
// This should be the hInst of the module that contains the string
// resources for the day and month strings.  If these functions are
// put into a DLL, hInst should be the hModule passed to LibMain
//
extern HANDLE hInst ;

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
         LoadString( hInst, IDS_SUNDAY + i, szWday[i], 12 ) ;

      //
      // In an international setting "Sunday" may not be abbrieviated
      // as "Sun".
      //
      for (i = 0 ; i <= 6 ; i++)
         LoadString( hInst, IDS_SUNDAY_SHORT + i, szShortWday[i], 4 ) ;

      for (i = 0 ; i <= 11 ; i++)
         LoadString( hInst, IDS_JANUARY + i, szMonth[i],  12 ) ;

      for (i = 0 ; i <= 11 ; i++)
         LoadString( hInst, IDS_JANUARY_SHORT + i, szShortMonth[i],  4 ) ;
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
   if (lpCur = _fstrchr( sShortDate, 'd' ))
      if (_fstrchr( lpCur+1, 'd' ))
         fDayLZ = TRUE ;

   // 
   // month?
   //
   if (lpCur = _fstrchr( sShortDate, 'M' ))
      if (_fstrchr( lpCur+1, 'M' ))
         fMoLZ = TRUE ;


   // 
   // Does year have the century?
   //
   if (lpCur = _fstrchr( sShortDate, 'y' ))
      if (lpCur = _fstrchr( lpCur + 1, 'y' ))
         if (_fstrchr( lpCur+1, 'y' ))
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
                     fMoLZ  ? 2 : 1,
                     cSep,
                     fDayLZ ? 2 : 1,
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
                     fDayLZ ? 2 : 1,
                     cSep,
                     fMoLZ  ? 2 : 1,
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
   LPSTR lpCur ;
   BYTE  bMo = 4 ;
   BYTE  fShortDay ;
   char  szSep1[16], szSep2[16], szSep3[16] ;
   char  szMo[16], szDay[3], szYr[5], szFullDay[16] ;
   char  szDate[LEN_LONGDATE+1] ;

   //
   //  number of month digits.
   //
   if (lpCur = _fstrchr( sLongDate, 'M' ))
      for ( bMo = 0 ; lpCur ; lpCur = _fstrchr( lpCur+1, 'M' ) )
         bMo++ ;
   switch (bMo)
   {
      case 1:
         wsprintf( szMo, "%d", MONTH( lpTM ) ) ;
         break ;

      case 2:
         wsprintf( szMo, "%02d", MONTH( lpTM ) ) ;
         break ;

      case 3:
         wsprintf( szMo, "%s", (LPSTR)szShortMonth[ MONTH( lpTM )-1] ) ;
         break ;

      case 4:
         wsprintf( szMo, "%s", (LPSTR)szMonth[ MONTH( lpTM )-1 ] ) ;
         break ;
   }

   //
   // number of year digits
   //
   if (lpCur = _fstrchr( sLongDate, 'y' ))
      if (*(lpCur+2) == 'y')
         wsprintf( szYr, "%d", YEAR( lpTM ) + 1900 ) ;
      else
         wsprintf( szYr, "%d", YEAR( lpTM ) ) ;


   //
   // full day strings or just 3 characters?
   //
   if (*sLongDate == 'd')
   {
      if (*(sLongDate+3) == 'd')
      {
         fShortDay = FALSE ;
         wsprintf( szFullDay, "%s", (LPSTR)szWday[ WDAY( lpTM ) ] ) ;
      }
      else
      {
         fShortDay = TRUE ;
         wsprintf( szFullDay, "%s", (LPSTR)szShortWday[ WDAY( lpTM ) ] ) ;
      }
   }
   else
      szFullDay[0] = '\0' ;

   //
   // number of day digits
   //
   lpCur = sLongDate + (fShortDay ? 3 : 4) ;
   if (lpCur = _fstrchr( lpCur, 'd' ))
      if (*(lpCur+1) == 'd')
         wsprintf( szDay, "%02d", MDAY( lpTM ) ) ;
      else
         wsprintf( szDay, "%d", MDAY( lpTM ) ) ;

   //
   // Get the separator strings
   //
   ParseSeparators( sLongDate, szSep1, szSep2, szSep3 ) ;

   //
   // Now we have fully parsed the format string.  The only thing that is
   // missing is  the order (MMMM, dd, yyyy/ dd MMMM yyyy / yyyy MMMM dd)
   //
   if ((lpCur = NextSepChar( sLongDate )) &&
       (lpCur = NextNonSepChar( lpCur )))
   {
      switch (*lpCur)
      {
         case 'd':  //              ,   ,
            wsprintf( szDate, "%s%s%s%s%s",
                        (LPSTR)szDay, (LPSTR)szSep2,
                        (LPSTR)szMo, (LPSTR)szSep3,
                        (LPSTR)szYr ) ;
         break ;

         case 'y':  //              ,   ,
            wsprintf( szDate, "%s%s%s%s%s",
                        (LPSTR)szYr, (LPSTR)szSep2,
                        (LPSTR)szMo, (LPSTR)szSep3,
                        (LPSTR)szDay ) ;
         break ;

         case 'M':
         default:
            wsprintf( szDate, "%s%s%s%s%s",
                        (LPSTR)szMo, (LPSTR)szSep2,
                        (LPSTR)szDay, (LPSTR)szSep3,
                        (LPSTR)szYr ) ;
         break ;
      }
   }
   else
      szDate[0] = '\0' ;

   if (!(wFlags & DATE_NODAYOFWEEK))
   {
      lstrcat( szFullDay, szSep1 ) ;
      wsprintf( lpszDate, "%s%s",
                     (LPSTR)szFullDay, (LPSTR)szDate ) ;
   }
   else
      lstrcpy( lpszDate, szDate ) ;

   return lpszDate ;
}


/****************************************************************
 *  LPSTR NEAR PASCAL NextSepChar( LPSTR lpsz )
 *
 *  Description: 
 *
 *    This function finds the next non-format character in a
 *    sLongDate string.  The format characters are M,d,and y.
 *    If the string has "separator quotes" in it it finds the
 *    first char after the quote.
 *
 *    This is here because there are two ways the sLongDate string
 *    could appear.  The first is the way it's documented in
 *    WININI2.TXT:
 *
 *       sLongDate=dddd, dd MMMM, yyyy
 *
 *    The second is the way Control Panel actually writes it:
 *
 *       sLongDate=dddd', 'dd' 'MMMM', 'yyyy
 *
 *  Comments:
 *
 ****************************************************************/
LPSTR NEAR PASCAL NextSepChar( LPSTR lpsz )
{
   LPSTR lpszCur = lpsz ;

   while (*lpszCur && (*lpszCur == 'M' ||
                       *lpszCur == 'd' ||
                       *lpszCur == 'y' ||
                       *lpszCur == '\''))
         lpszCur++ ;

   return lpszCur ;

} /* NextSepChar()  */

/****************************************************************
 *  LPSTR NEAR PASCAL NextNonSepChar( LPSTR lpsz )
 *
 *  Description: 
 *
 *    This function finds the next format character (i.e. 'd', 'M', or 'y').
 *    If the string has quotes in it, it finds the next format char
 *    after the next quote.
 *
 *  Comments:
 *
 ****************************************************************/
LPSTR NEAR PASCAL NextNonSepChar( LPSTR lpsz )
{
   LPSTR lpszCur = lpsz ;

   while (*lpszCur && (*lpszCur != 'M' &&
                       *lpszCur != 'd' &&
                       *lpszCur != 'y' &&
                       *lpszCur != '\''))
         lpszCur++ ;

   if (*lpszCur && *lpszCur == '\'')
      lpszCur++ ;

   return lpszCur ;

} /* NextNonSepChar()  */


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
      i = 0 ;

      for (i = 0, lpEnd = lpCur ;
           *lpCur && (*lpCur != 'M' &&
                      *lpCur != 'd' &&
                      *lpCur != 'y') ;
           lpEnd++, lpCur++ )
      {
         if (*lpCur != '\'')
            lpszSep[i++] = *lpCur ;
      }

      lpszSep[i] = '\0' ;
   }

   return lpEnd ;

} /* GetSeparator()  */
/************************************************************************
 * End of File: wintime.c
 ************************************************************************/

