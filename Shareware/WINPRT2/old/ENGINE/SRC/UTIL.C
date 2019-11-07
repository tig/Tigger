/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  UTIL 
 *
 *      Module:  UTIL.C
 * 
 *      This module contians the main entry points for the WinPrint 2.0
 *      engine Header/Footer stuff.  The Header/Footer setup dialog
 *      box is also handled here.
 *
 *   Revisions:  
 *       9/15/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "ENGINE.h"

DWORD WINAPI Div( DWORD n, DWORD d )
{
    DWORD  l ;

    l = n / d ;
    if ( ((n % d) != 0) && ((n % d) >= (d / 2)))
        l++ ;

    return l ;
}    

/* Convert a millimeter value (1/100 mm) to an inch value
 * (1/10000 inch).
 */
UINT  WINAPI engMMToInch( UINT uiMM )
{
   return (UINT)Div( 10000L * (DWORD)uiMM, 2540L ) ;

}

UINT  WINAPI engInchToMM( UINT uiIn )
{
   return (UINT)Div( (DWORD)uiIn * 2540L, 10000L ) ; 
}

DWORD INTERNAL Power( DWORD n, DWORD p )
{
    DWORD l = n ;

    if (p == 0)
        return 1 ;

    p-- ;
    while (p)
    {
        l = l * n ;
        p-- ;
    }

    if (l == 0)
        return 1 ;

    return l ;
}

UINT INTERNAL NumDigits( LONG dw )
{
    UINT n = 0 ;
    while (dw > 0)
    {
        dw = dw / 10L ;
        n++ ;
    }
    return n ;
}


UINT  WINAPI engStrToUINT( LPSTR lpsz, UINT uiDecPts )
{
    BOOL    fDec ;
    UINT    uiWhole, uiDec ;
    UINT    n ;
    UINT    nLeadZeros = 0 ;

    for (fDec=FALSE, uiWhole=0, uiDec = 0; *lpsz ; lpsz++ )
    {
        if (*lpsz == '.')
        {
            fDec = TRUE ;
            continue ;
        }

        if (isdigit(*lpsz))
        {
            n = *lpsz - '0' ;

            if (fDec)
            {
                if (uiDec == 0 && n == 0)
                    nLeadZeros++ ;
                else
                    uiDec = (uiDec * 10) + n ;
            }
            else
                uiWhole = (uiWhole * 10) + n ;
        }
    }

    n = NumDigits( uiDec ) + nLeadZeros ;
    if (n < uiDecPts)
        uiDec = uiDec * (UINT)Power( 10, uiDecPts - n ) ;
    else if (n > uiDecPts)
        uiDec = (UINT)Div( uiDec, Power( 10, n - uiDecPts ) ) ;
        
    return (uiWhole * (UINT)Power( 10, uiDecPts )) + uiDec ;
}    


UINT  WINAPI engDlgGetMeasure( HWND hDlg, UINT ui )
{
    HWND hwnd ;
    char szBuf[17] ;

    if (ui == 0)
        hwnd = hDlg ;
    else
        hwnd = GetDlgItem( hDlg,ui ) ;

    Edit_GetText( hwnd, szBuf, 16 ) ;

    switch (uiMeasureUnit)
    {
        case UNIT_INCHES:
            return engInchToMM( engStrToUINT ( szBuf, 4 ) ) ;
        break ;

        default:
            return engStrToUINT ( szBuf, 3 ) ;
        break ;
    }
}

BOOL WINAPI engDlgSetMeasure( HWND hDlg, UINT ui, UINT uiMM )
{
    HWND hwnd ;
    char szBuf[17] ;
    LPSTR lp ;
    UINT uiWhole ;
    UINT uiDec ;
    UINT  n ;

    if (ui == 0)
        hwnd = hDlg ;
    else
        hwnd = GetDlgItem( hDlg,ui ) ;

    switch (uiMeasureUnit)
    {
        case UNIT_INCHES:
            uiMM = engMMToInch( uiMM ) ;
            uiWhole = (UINT)((DWORD)uiMM / 10000L) ;
            uiDec = (UINT)((DWORD)uiMM % 10000L) ;
            n = NumDigits( uiDec ) ;
            if (n > 3)
            {
                uiDec = (UINT)Div( uiDec, Power( 10, n - 3 ) ) ;
                wsprintf( szBuf, "%u.%u", uiWhole, uiDec ) ;
                for (lp = szBuf + lstrlen(szBuf) - 1 ; lp >= szBuf && *lp == '0' ; lp-- )
                    *lp = '\0' ;
            }
            else
            {
                uiDec = (UINT)Div( uiDec, 10 ) ;
                switch (n)
                {
                    case 3:
                        if (uiDec > 50)
                            wsprintf( szBuf, "%u.1", uiWhole ) ;
                        else
                            wsprintf( szBuf, "%u.0%u", uiWhole, uiDec ) ;
                    break ;

                    case 2:
                        if (uiDec > 5)
                            wsprintf( szBuf, "%u.01", uiWhole ) ;
                        else
                            wsprintf( szBuf, "%u.00%u", uiWhole, uiDec ) ;
                    break ;

                    case 1:
                    default:
                        if (uiDec == 1)
                            wsprintf( szBuf, "%u.001", uiWhole ) ;
                        else
                            wsprintf( szBuf, "%u", uiWhole ) ;
                    break ;
                }
            }
        break ;

        default:
            uiWhole = (UINT)((DWORD)uiMM / 1000L) ;
            uiDec = (UINT)((DWORD)uiMM % 1000L) ;
            n = NumDigits( uiDec ) ;
            if (n > 2)
            {
                wsprintf( szBuf, "%u.%u", uiWhole, uiDec ) ;
                for (lp = szBuf + lstrlen(szBuf) - 1 ; lp >= szBuf && *lp == '0' ; lp-- )
                    *lp = '\0' ;
            }
            else
            {
//                uiDec = Div( uiDec, 10 ) ;
                switch (n)
                {
                    case 2:
                        wsprintf( szBuf, "%u.0%u", uiWhole, uiDec ) ;
                    break ;

                    case 1:
                        wsprintf( szBuf, "%u.00%u", uiWhole, uiDec ) ;
                    break ;

                    default:
                        wsprintf( szBuf, "%u", uiWhole ) ;
                    break ;
                }
            }
        break ;
    }

    Edit_SetText( hwnd, szBuf ) ;

    return TRUE ;
}
    
/* Given a UINT  that represents a measurement in 100ths of a mm,
 * format a string that shows cm with three decimalpoints.
 */
LPSTR WINAPI engMMToString( LPSTR lpsz, UINT uiMM )
{
   UINT     n ;
   UINT     uiWhole = (UINT)((DWORD)uiMM / 100L) ;
   UINT     uiDec = (UINT)((DWORD)uiMM % 100L) ;

   n = NumDigits( uiDec ) ;
   if (n < 2)
        uiDec = uiDec * (UINT)Power( 10, 2 - n ) ;

   wsprintf( lpsz, "%01u.%02u", uiWhole, uiDec ) ;
   return lpsz ;
}

LPSTR WINAPI engInchToString( LPSTR lpsz, UINT uiIn )
{
   UINT     n ;
   UINT     uiWhole = (UINT)((DWORD)uiIn / 10000L) ;
   UINT     uiDec = (UINT)((DWORD)uiIn % 10000L) ;

   n = NumDigits( uiDec ) ;
   if (n < 3)
        uiDec = uiDec * (UINT)Power( 10, 3 - n ) ;

   wsprintf( lpsz, "%01u.%04u", uiWhole, uiDec ) ;

   return lpsz ;
}

/* Filename manipulation functions
 */
/* Given an ANSI path, convert it to upper-case, then to OEM so DOS
 * is happy.
 */
VOID WINAPI AnsiPathToOEM( LPSTR szPath )
{
    AnsiUpper( szPath ) ;
    AnsiToOem( szPath, szPath ) ;
}


BOOL WINAPI IsWild( LPSTR lpszPath )

{
    while (*lpszPath)
    {
        if (*lpszPath == '?' || *lpszPath == '*')
            return TRUE ;
        lpszPath++ ;
    }
    
    return FALSE ;
}

/* Ensures that a path ends with a backslash.
 */
VOID WINAPI AddBackslash( LPSTR lpszPath )
{
    if (lpszPath[lstrlen( lpszPath ) - 1] == '\\')
        return ;
    lstrcat( lpszPath, "\\" ) ;
}

/* Removes a trailing backslash from a proper directory name UNLESS it is
 * the root directory.  Assumes a fully qualified directory path.
 */
VOID WINAPI StripBackslash( LPSTR lpszPath )
{
    register WORD len ;

    len = lstrlen( lpszPath ) - 1 ;

    if ((len == 2) || (lpszPath[len] != '\\'))
        return ;

    lpszPath[len] = NULL ;
}

/* Remove the filespec portion from a path (including the backslash).
 */
VOID WINAPI StripFilespec( LPSTR lpszPath )
{
    LPSTR     p;

    p = lpszPath + lstrlen( lpszPath ) ;
    while ((*p != '\\') && (*p != ':') && (p != lpszPath))
        p-- ;

    if (*p == ':')
        p++ ;

    /* Don't strip backslash from root directory entry. */
    if (p != lpszPath)
    {
        if ((*p == '\\') && (*(p-1) == ':'))
            p++ ;
    }

    *p = NULL ;
}

/* Extract only the filespec portion from a path. 
 */
VOID WINAPI StripPath( LPSTR lpszPath )
{
    LPSTR     p ;

    // make sure there's no whitespace! (helps get rid of command line args)
    for (p = lpszPath ; *p != '\0' ; p++)
        if (isspace(*p))
        {
            *p = '\0' ;
            goto outahere;
        }
outahere:

    p = lpszPath + lstrlen(lpszPath);
    while ((*p != '\\') && (*p != ':') && (p != lpszPath))
        p-- ;

    if (p != lpszPath)
        p++ ;

    if (p != lpszPath)
        lstrcpy( lpszPath, p ) ;
}

/* Returns the extension part of a filename. This does not
 * include the period.
 */
LPSTR WINAPI GetExtension( LPSTR lpszFile )
{
    LPSTR p, pSave = NULL ;

    p = lpszFile ;
    while (*p)
    {
        if (*p == '.')
            pSave = p ;
        p++ ;
    }

    if (!pSave)
        return p ;

    return pSave + 1 ;
}

/* Returns TRUE if 'lpszExt' is somewhere in 'lpszList'. 
 * lpszList is a space deliminated list of extensions.
 * 
 * This is used to determine if a file is an executable.  This way
 */
BOOL WINAPI FindExtensionInList( LPSTR lpszExt, LPSTR lpszList )
{
    LPSTR p2 ;
    char ch ;
    
    while (*lpszList)
    {
        /* Move to the next item in the list. */
        while ((*lpszList) && (*lpszList == ' '))
            lpszList++ ;
        
        if (!*lpszList)
            break ;
        
        /* NULL-terminate this item. */
        p2 = lpszList+1 ;
        while ((*p2) && (*p2 != ' '))
            p2++ ;

        ch = *p2 ;
        *p2 = NULL ;
        if (!lstrcmpi( lpszExt, lpszList ))
        {
            *p2 = ch ;
            return TRUE ;
        }
        *p2 = ch ;
        lpszList = p2 ;
    }
    return FALSE ;
}

BOOL WINAPI IsProgram( LPSTR lpszPath )
{
    LPSTR szExt;
    char szTemp[MAX_PATHLEN] ;
    char szPrograms[256] ;
    
    lstrcpy( szTemp, lpszPath ) ;
    
    /* Get the file's extension. */
    StripPath( szTemp ) ;
    szExt = GetExtension( szTemp ) ;
    
    if (!*szExt)
        return FALSE ;
    
    /* Get programs from WIN.INI
     */
    szPrograms[0] = '\0' ;
    GetProfileString( "windows", "programs", "", szPrograms, 255 ) ;
    /* Add some others
     */
    lstrcat( szPrograms, " dll" ) ;
    lstrcat( szPrograms, " drv" ) ;
    lstrcat( szPrograms, " 386" ) ;

    return FindExtensionInList( szExt, szPrograms ) ;
}


/************************************************************************
 * End of File: UTIL.C
 ***********************************************************************/
