//*************************************************************
//  File name: string.c
//
//  Description: 
//      Module for the string stuff
//
//  History:    Date       Author     Comment
//               1/14/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "iCEK.h"

//*************************************************************
//
//  cekmmIsUsefulString
//
//  Purpose:
//      Checks a string to see if it has anything but white space
//
//
//  Parameters:
//      LPSTR lpText
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               1/ 5/92   MSM        Created
//
//*************************************************************

BOOL WINAPI cekmmIsUsefulString( LPSTR lpText )
{
    if (!lpText)
        return FALSE;

    while (*lpText)
    {
        if (!isspace(*lpText))
            return TRUE;
        lpText++;
    }
    return FALSE;

} //*** cekmmIsUsefulString

//*************************************************************
//
//  cekmmIsNumber
//
//  Purpose:
//      Determines if the string constitutes a number or not
//
//
//  Parameters:
//      LPSTR lpS
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               1/ 5/92   MSM        Created
//
//*************************************************************

BOOL WINAPI cekmmIsNumber( LPSTR lpS )
{
    if (!lpS)
        return FALSE;

    while ( isspace(*lpS) )
        lpS++;

    if (*lpS=='+' || *lpS=='-')
        lpS++;

    while ( isdigit(*lpS) )
        lpS++;

    while ( isspace(*lpS) )
        lpS++;

    return (*lpS==0);

} //*** cekmmIsNumber

//*************************************************************
//
//  cekmmSetExtension
//
//  Purpose:
//      Sets the extension of the file
//
//
//  Parameters:
//      LPSTR lpFile
//      LPSTR lpExt
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               1/ 5/92   MSM        Created
//
//*************************************************************

VOID WINAPI cekmmSetExtension( LPSTR lpFile, LPSTR lpExt )
{
    LPSTR lp = lpFile + lstrlen(lpFile) - 1;

    while (lp>lpFile)
    {
        switch (*lp)
        {
            case '.':
                lstrcpy( lp, lpExt );
                return;
            break;

            case '\\':
            case ':':
                lstrcat( lpFile, lpExt );
                return;
            break;
        }
        lp--;
    }
    lstrcat( lpFile, lpExt );
    return;

} //*** cekmmSetExtension

//*************************************************************
//
//  cekmmHackSpaces
//
//  Purpose:
//      Strips leading and trailing spaces
//
//
//  Parameters:
//      LPSTR lpszLine
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               1/ 5/92   MSM        Created
//                                       
//*************************************************************

void WINAPI cekmmHackSpaces( LPSTR lpszLine )
{
    LPSTR  ld, ls, lls; 

    ld = ls = lls = lpszLine;
    
    //
    // Nuke leading whitespace
    //
    while (isspace( *ls ) && *ls !='\0')
        ls++ ;

    //
    // Nuke trailing spaces
    //
    while (*ls != '\0')
    {
        *ld = *ls;
        if (!isspace( *ls ))
            lls = ld;
        ls++;
        ld++;
    }

    *++lls = '\0';

} //*** cekmmHackSpaces

//*************************************************************
//
//  cekmmGetValue
//
//  Purpose:
//      reads in a hex/decimal value and increments a pointer
//
//  Parameters:
//      LPSTR FAR *lplp
//      
//
//  Return: (LONG)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               1/ 5/92   MSM        Created
//
//*************************************************************

LONG WINAPI cekmmGetValue(LPSTR FAR *lplp)
{
    LPSTR lp = *lplp;
    DWORD dwValue=0;
    int   sign=1;

    while( isspace(*lp) )
        lp++;
    if( *lp=='-' )
    {
        sign = -1;
        lp++;
    }

    if( _fstrnicmp( lp, (LPSTR)"0x",2) )
    {
        
        while( isdigit(*lp) )
        {
            dwValue *= 10;
            dwValue += (*lp - '0');
            lp++;
        }
    }    
    else
    {
        lp+=2;

        while( isxdigit(*lp) )
        {
            dwValue *= 16;
            if( isdigit(*lp) )
                dwValue += (*lp - '0');
            else
                dwValue += (toupper(*lp) - 'A' + 10);
            lp++;
        }
    }
    while( isspace(*lp) )
        lp++;

    *lplp = lp;
    return ((long)dwValue * sign );

} //*** cekmmGetValue

//*** EOF: string.c
