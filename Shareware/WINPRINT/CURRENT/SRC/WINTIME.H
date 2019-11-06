/************************************************************************
 *
 *      Module:  WINTIME.H
 *
 ************************************************************************/

/************************************************************************
 *                          WINTIME.C STUFF
 ************************************************************************/
LPSTR FAR PASCAL TimeGetCurDate( LPSTR lpszDate, WORD wFlags ) ;
LPSTR FAR PASCAL TimeGetCurTime( LPSTR lpszTime, WORD wFlags ) ;
LPSTR FAR PASCAL TimeFormatDate( LPSTR lpszDate,
                                 struct tm FAR *pTM,
                                 WORD wFlags ) ;

LPSTR FAR PASCAL TimeFormatTime( LPSTR lpszTime,
                                 struct tm FAR *pTM,
                                 WORD wFlags ) ;
void FAR PASCAL TimeResetInternational( void ) ;

#define DATE_SHORTDATE   0x0001
#define DATE_NODAYOFWEEK 0x0002

#define TIME_24HOUR      0x0001
#define TIME_12HOUR      0x0002
#define TIME_NOSECONDS   0x0004


/************************************************************************
 * End of File: WINTIME.H
 ************************************************************************/

