/************************************************************************
 *
 *      Module:  WINTIME.H
 *
 ************************************************************************/

/************************************************************************
 *                          WINTIME.C STUFF
 ************************************************************************/
LPSTR WINAPI TimeGetCurDate( LPSTR lpszDate, UINT wFlags ) ;
LPSTR WINAPI TimeGetCurTime( LPSTR lpszTime, UINT wFlags ) ;
LPSTR WINAPI TimeFormatDate( LPSTR lpszDate,
                                 struct tm FAR *pTM,
                                 UINT wFlags ) ;

LPSTR WINAPI TimeFormatTime( LPSTR lpszTime,
                                 struct tm FAR *pTM,
                                 UINT wFlags ) ;

LPSTR WINAPI TimeFormatDateEx( LPSTR lpszDate,
                                 FILETIME FAR* pFileTime,
                                 UINT wFlags ) ;

LPSTR WINAPI TimeFormatTimeEx( LPSTR lpszTime,
                                 FILETIME FAR* pFileTime,
                                 UINT wFlags ) ;

void WINAPI TimeResetInternational( void ) ;

#define DATE_DEFAULT     0x0000
#define DATE_SHORTDATE   0x0001
#define DATE_NODAYOFWEEK 0x0002

#define TIME_DEFAULT     0x0000
#define TIME_24HOUR      0x0001
#define TIME_12HOUR      0x0002
#define TIME_NOSECONDS   0x0004

/* these are used by WinTime */
#define  IDS_SUNDAY 1024
#define  IDS_MONDAY 1025
#define  IDS_TUESDAY    1026
#define  IDS_WEDNESDAY  1027
#define  IDS_THURSDAY   1028
#define  IDS_FRIDAY 1029
#define  IDS_SATURDAY   1030
#define  IDS_SUNDAY_SHORT   1031
#define  IDS_MONDAY_SHORT   1032
#define  IDS_TUESDAY_SHORT  1033
#define  IDS_WEDNESDAY_SHORT    1034
#define  IDS_THURSDAY_SHORT 1035
#define  IDS_FRIDAY_SHORT   1036
#define  IDS_SATURDAY_SHORT 1037
#define  IDS_JANUARY    1040
#define  IDS_FEBRUARY   1041
#define  IDS_MARCH  1042
#define  IDS_APRIL  1043
#define  IDS_MAY    1044
#define  IDS_JUNE   1045
#define  IDS_JULY   1046
#define  IDS_AUGUST 1047
#define  IDS_SEPTEMBER  1048
#define  IDS_OCTOBER    1049
#define  IDS_NOVEMBER   1050
#define  IDS_DECEMBER   1051
#define  IDS_JANUARY_SHORT  1056
#define  IDS_FEBRUARY_SHORT 1057
#define  IDS_MARCH_SHORT    1058
#define  IDS_APRIL_SHORT    1059
#define  IDS_MAY_SHORT  1060
#define  IDS_JUNE_SHORT 1061
#define  IDS_JULY_SHORT 1062
#define  IDS_AUGUST_SHORT   1063
#define  IDS_SEPTEMBER_SHORT    1064
#define  IDS_OCTOBER_SHORT  1065
#define  IDS_NOVEMBER_SHORT 1066
#define  IDS_DECEMBER_SHORT 1067
#define  IDS_LONGDATE         1068
#define  IDS_SHORTDATE        1069


/************************************************************************
 * End of File: WINTIME.H
 ************************************************************************/

