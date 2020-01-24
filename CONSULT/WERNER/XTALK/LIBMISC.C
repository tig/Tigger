/*****************************************************************************\
*                                                                             *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Library Module (LIBMISC.C)                                      *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains various common library subroutines.        *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.                                             *
*  1.01 12/10/89 00066 Remove freezing delay (use Yield() now).               *
*  1.01 02/08/90 dca00021 PJL Fix SpawnApp to be Windows 3.0 compatible.      *
*  1.10 03/12/90 dca00044 PJL Add octal number support to Kent's SPRINTF.     *
*	1.1b 05/18/90 dca00098 PJL Fix Modem string save-to-disk problem.		   *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <direct.h>
#include <dos.h>
//#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "variable.h"
#include "dialogs.h"


typedef struct
{
  WORD   environment;
  LPSTR  commandline;
  LPSTR  FCB1;
  LPSTR  FCB2;
} EXECBLOCK;
static EXECBLOCK exec;
static char szSpawnAppName[32];
static char szCommandLine[64];
static WORD wFCB1Contents[2];


extern int Int21Function4B(BYTE, LPSTR, LPSTR);
// dca00021 PJL Fix SpawnApp to be Windows 3.0 compatible.
extern int PInt21Function4B(BYTE, LPSTR, LPSTR);


void SpawnApp(char *szAppName, char *szParams)
{

	strcpy(szSpawnAppName, szAppName);
	strcpy(szCommandLine,  szParams);

	GlobalCompact(-1L);
	LockData(0);

	exec.environment = 0;
	exec.commandline = szCommandLine;

	wFCB1Contents[0] = 2;
	wFCB1Contents[1] = SW_SHOWNORMAL;
	exec.FCB1 = (LPSTR)wFCB1Contents;
	exec.FCB2 = (LPSTR)NULL;

	// dca00021 PJL Fix SpawnApp to be Windows 3.0 compatible.
	if (sVersion < 3)
		Int21Function4B(0, (LPSTR)szSpawnAppName, (LPSTR)&exec);
	else
		PInt21Function4B(0, (LPSTR)szSpawnAppName, (LPSTR)&exec);

	UnlockData(0);
}


//
// Trim the blanks from the end of the string.
//
USHORT StrRightTrim(char *psz)
{
	register i;

	i = strlen(psz);
	while (i)
	{
		if (psz[i-1] == ' ')
			i--;
		else
			break;
	}

	return (i);
}


void ChangeMenuMsg(wMenu, nMsgNo)
WORD wMenu;
int  nMsgNo;
{
	char buf[64];

	LoadString(hWndInst, nMsgNo, buf, sizeof(buf));
	ChangeMenuText(wMenu, buf);
}


void ChangeMenuMsg1(wMenu, nMsgNo, pArg1)
WORD wMenu;
int  nMsgNo;
char *pArg1;
{
	char buf[80], fmt[64];

	LoadString(hWndInst, nMsgNo, fmt, sizeof(fmt));
	sprintf(buf, fmt, pArg1);
	ChangeMenu(hWndMenu, wMenu, buf, wMenu, MF_BYCOMMAND | MF_CHANGE | MF_STRING);
}


void ChangeMenuText(wMenu, pBuf)
WORD wMenu;
PSTR pBuf;
{
	char tmp[64];
	register int i;

	GetMenuString(hWndMenu, wMenu, tmp, sizeof(tmp), MF_BYCOMMAND);

	for (i = 0; i < sizeof(tmp) && tmp[i] != '(' && tmp[i]; i++);
		if (tmp[i] == '(')
		{
			strcpy(&tmp[i+1], pBuf);
			i = strlen(tmp);
			while (tmp[i-1] == ' ')
				i--;
			tmp[i++] = ')';
			tmp[i++] = '.';
			tmp[i++] = '.';
			tmp[i++] = '.';
			tmp[i] = 0;
		}
		else
		{
			for (i = 0; i < sizeof(tmp) && tmp[i] != '/' && tmp[i]; i++);
				if (tmp[i] == '/')
					strcpy(&tmp[i+1], pBuf);
				else
					strcpy(tmp, pBuf);
				i = strlen(tmp);
				while (tmp[i-1] == ' ')
					i--;
		}

	ChangeMenu(hWndMenu, wMenu, tmp, wMenu, MF_BYCOMMAND | MF_CHANGE | MF_STRING);
}


void LibDestore(inpstr, outstr, outsiz)
BYTE *inpstr;
BYTE *outstr;
int  outsiz;
{
	register int i;

	i = 1;
	while (*inpstr)
	{
		switch (*inpstr)
		{
		case '`':
			if (inpstr[1] == '^' || inpstr[1] == '`' || inpstr[1] == '|')
			{
				if (i < outsiz)
					outstr[i++] = inpstr[1];
				inpstr++;
			}
			else
			{
				if (i < outsiz)
					outstr[i++] = '`';
			}
			break;

		case '^':
			if (inpstr[1] >= '@' && inpstr[1] <= '_')
			{
				if (i < outsiz)
					outstr[i++] = inpstr[1] - '@';
				inpstr++;
			}
			else if (inpstr[1] >= '0' && inpstr[1] <= '9')
			{
				register j;

				inpstr++;

				outstr[i] = 0;
				for (j = 0; j < 3 && *inpstr >= '0' && *inpstr <= '9'; j++)
				{
					outstr[i] = (BYTE)(outstr[i] * 10 + (*inpstr - '0'));
					inpstr++;
				}
				inpstr--;
				i++;
			}
			else
			{
				if (i < outsiz)
					outstr[i++] = '^';
			}
			break;

		case '|':
			if (i < outsiz)
				outstr[i++] = ASC_CR;
			break;

		default:
			if (i < outsiz)
				outstr[i++] = *inpstr;
			break;
		}

		inpstr++;
	}

	outstr[0] = (BYTE)(i - 1);
	// dca00098 PJL Fix Modem string save-to-disk problem.
	outstr[i] = 0;
}


void LibEnstore(inpstr, outstr, outsiz)
BYTE *inpstr;
BYTE *outstr;
int  outsiz;
{
	int  inpsiz;
	register int i;

	i = 1;
	inpsiz = (int)(*inpstr++);
	while (inpsiz--)
	{
		if (*inpstr == '\r')
		{
			if (i < outsiz-1)
			{
				*outstr++ = '^';
				i++;
				*outstr++ = 'M';
				i++;
			}
		}
		else if (*inpstr == '^')
		{
			if (i < outsiz-1)
			{
				*outstr++ = '`';
				i++;
				*outstr++ = '^';
				i++;
			}
		}
		else if (*inpstr < ' ')
		{
			if (i < outsiz-1)
			{
				*outstr++ = '^';
				i++;
				*outstr++ = *inpstr + '@';
				i++;
			}
		}
		else
		{
			if (i < outsiz)
			{
				*outstr++ = *inpstr;
				i++;
			}
		}

		inpstr++;
	}

	// dca00098 PJL
	*outstr = 0;
}


int WhichRadioButton(hDlg, wFirstID, wLastID, wDefault)
HWND hDlg;
WORD wFirstID;
WORD wLastID;
WORD wDefault;
{
	register int i;

	for (i = wFirstID; i <= wLastID; i++)
	{
		if (IsDlgButtonChecked(hDlg, i))
			return (i);
	}

	return (wDefault);
}


void GetDlgItemData(hDlg, wID, vBuf, nMaxLen)
HWND hDlg;
WORD wID;
char *vBuf;
int  nMaxLen;
{
	char buf[128];

	GetDlgItemText(hDlg, wID, buf, sizeof(buf));
	LibDestore(buf, vBuf, nMaxLen);
}


/*---------------------------------------------------------------------------*/
/*  GetDlgItemPath()                                                         */
/*                                                                           */
/*  Description:                                                             */
/*    Gets a path name from a dialog and verfies it.  If in error this       */
/*    routine will prompt the user on the error and possible correction.     */
/*    We also do some formatting of the path name to make it pretty.         */
/*                                                                           */
/*  Return:                                                                  */
/*    TRUE if a valid path is return, else FALSE.                            */
/*---------------------------------------------------------------------------*/
BOOL GetDlgItemPath(HWND hDlg, WORD wID, char *szPath, SHORT cPath)
{
	register int i;

	GetDlgItemText(hDlg, wID, szPath, cPath);
	strupr(szPath);

	if (ValidateDiskDrive(szPath) == -1)
	{
		ErrorMessage(IDS_ERRDLG_BADDRIVE);
		return(FALSE);
	}

	if (!ValidatePath(szPath))
	{
		if (AskUserMessageStr(IDS_ERRDLG_MAKEDIR, szPath) != IDYES)
			return(FALSE);

		if (mkdir(szPath))
		{
			ErrorMessage(IDS_ERRDLG_BADMKDIR);
			return(FALSE);
		}
	}

	return(TRUE);
}


void SetDlgItemData(hDlg, wID, vBuf)
HWND hDlg;
WORD wID;
char *vBuf;
{
	char buf[128];

	LibEnstore(vBuf, buf, sizeof(buf));
	SetDlgItemText(hDlg, wID, buf);
}


void SetDlgItemLength(hDlg, wID, nLength)
HWND hDlg;
WORD wID;
int  nLength;
{

	SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, nLength, 0L);
}


void SetDlgItemLong(hDlg, nItem, uValue, fSign)
HWND	hDlg;
int 	nItem;
unsigned long uValue;
int	fSign;
{
	char buf[32];

	ltoa(uValue, buf, 10);
	SetDlgItemText(hDlg, nItem, buf);
}


/*---------------------------------------------------------------------------*/
/*  SetDlgItemPath()                                                         */
/*                                                                           */
/*  Description:                                                             */
/*    Set a path name into a dialog.  We also do some formatted so the user  */
/*    sees a consistant format.                                              */
/*---------------------------------------------------------------------------*/
void SetDlgItemPath(HWND hDlg, WORD wID, char *szPath)
{
	register i;

	strupr(szPath);
	i = strlen(szPath);
	if (szPath[i-1] == '\\' || szPath[i-1] == '/')
		szPath[i-1] = NULL;
	if (szPath[1] == ':' && (szPath[2] == '\0' || szPath[2] == ' '))
	{
		szPath[2] = '\\';
		szPath[3] = '\0';
	}

	SetDlgItemText(hDlg, wID, szPath);
}


/*---------------------------------------------------------------------------*/
/*  CreatePath()                                                             */
/*                                                                           */
/*  Description:                                                             */
/*    Creates a path name from the specified directory variable and file     */
/*    name, if the file name does not have an extension then one is added    */
/*    based on the directory number given.                                   */
/*---------------------------------------------------------------------------*/
void CreatePath(char *szPath, short sVarNo, char *szName)
{
	register i;

	// If the file name is already a path then just return it.
	if (strchr(szName, '\\') || strchr(szName, ':'))
	{
		strcpy(szPath, szName);
		strupr(szPath);
		return;
	}

	// Delete the silly stuff from a VAX system.
	if (*szName == '[')
	{
		while (*szName != ':' && *szName)
			*szName++;
		if (*szName)
			szName++;
	}

	// Lets get the requested directory from the variable handler.
	GetStrVariable(sVarNo, szPath, MAX_PATH);
	i = strlen(szPath);
	if (szPath[i-1] != '\\')
		szPath[i++] = '\\';

	// Add in the file name.
	strcpy(&szPath[i], szName);

	// If the file name does not have a file extension then add our own.
	if (strchr(szName, '.') == 0)
	{
		i = strlen(szPath);
		szPath[i++] = '.';
		switch (sVarNo)
		{
		case VAR_DIRXWP:
			szPath[i++] = 'X';
			szPath[i++] = 'W';
			szPath[i++] = 'P';
			break;

		case VAR_DIRXWS:
			szPath[i++] = 'X';
			szPath[i++] = 'W';
			szPath[i++] = 'S';
			break;

		case VAR_DIRFIL:
			i--;
			break;
		}

		szPath[i] = NULL;
	}
}


int lmemcmp(out, in, len)
LPSTR out;
LPSTR in;
WORD	len;
{

	while (len--)
	{
		if (*out++ != *in++)
			return (1);
	}

	return (0);
}


void lmemcpy(out, in, len)
LPSTR out;
LPSTR in;
WORD	len;
{

	while (len--)
		*out++ = *in++;
}


void lmemset(out, value, len)
LPSTR out;
BYTE	value;
WORD	len;
{

	while (len--)
		*out++ = value;
}


void LibBinToAsc(pAsc, pBin, nBin)
BYTE *pAsc;
BYTE *pBin;
int  nBin;
{
	register i;

	while (nBin-- > 0)
	{
		i = (*pBin >> 4) & 0x0F;
		if (i < 10)
			*pAsc++ = (char)('0' + i);
		else
			*pAsc++ = (char)('A' + i - 10);

		i = *pBin & 0x0F;
		if (i < 10)
			*pAsc++ = (char)('0' + i);
		else
			*pAsc++ = (char)('A' + i - 10);

		pBin++;
	}

	*pAsc = NULL;
}


void LibAscToBin(pBin, pAsc)
BYTE *pBin;
BYTE *pAsc;
{
	register i;

	while (*pAsc)
	{
		if (*pAsc < 'A')
			i = (*pAsc - '0') * 16;
		else
			i = (*pAsc - 'A' + 10) * 16;
		pAsc++;

		if (*pAsc < 'A')
			i += *pAsc - '0';
		else
			i += *pAsc - 'A' + 10;
		pAsc++;

		*pBin++ = (char)i;
	}
}


int LibStrToInt(char *szBuf, long *lInteger)
{
	char fNeg;
	register len;

	if (*szBuf == '-')
	{
		fNeg = TRUE;
		*szBuf++;
	}
	else
		fNeg = FALSE;

	*lInteger = 0L;
	switch (szBuf[(len = strlen(szBuf))-1])
	{
	case 'H':
	case 'h':
		while (len-- > 1)
		{
			if (!isxdigit(*szBuf))
				return (TRUE);
			if (*szBuf < 'A')
				*lInteger = *lInteger * 16L + (*szBuf - '0');
			else
				*lInteger = *lInteger * 16L + (toupper(*szBuf) - 'A' + 10);
			szBuf++;
		}
		break;

	case 'O':
	case 'o':
	case 'Q':
	case 'q':
		while (len-- > 1)
		{
			if (!isdigit(*szBuf) || *szBuf > '7')
				return (TRUE);
			*lInteger = *lInteger * 8L + (*szBuf - '0');
			szBuf++;
		}
		break;

	case 'B':
	case 'b':
		while (len-- > 1)
		{
			if (*szBuf != '0' && *szBuf != '1')
				return (TRUE);
			*lInteger = *lInteger * 2L + (*szBuf - '0');
			szBuf++;
		}
		break;

	case 'K':
	case 'k':
		while (len-- > 1)
		{
			if (!isdigit(*szBuf))
				return (TRUE);
			*lInteger = *lInteger * 10L + (*szBuf - '0');
			szBuf++;
		}
		*lInteger *= 1024L;
		break;

	default:
		while (len-- > 0)
		{
			if (!isdigit(*szBuf))
				return (TRUE);
			*lInteger = *lInteger * 10L + (*szBuf - '0');
			szBuf++;
		}
		break;
	}

	if (fNeg)
		*lInteger = -*lInteger;

	return (FALSE);
}


void LibFilterString(BYTE *pBuff, BYTE cBuff)
{
	register i, j;

	i = 0;
	for (j = 0; j < cBuff; j++)
	{
		if (isalnum(pBuff[j]))
			pBuff[i++] = pBuff[j];
	}

	pBuff[i] = NULL;
}


void SetScrollLock(hWnd, mode)
HWND hWnd;
int  mode;
{
	char buf[256];

	GetKeyboardState((LPSTR)buf);

	if (mode && ((buf[0x91] & 0x01) == 0))
		buf[0x91] |= 0x01;
	else if (mode == 0 && (buf[0x91] & 0x01) == 0x01)
		buf[0x91] &= 0xFE;
	else
		return;

	SetKeyboardState((LPSTR)buf);

	PostMessage(hWnd, WM_KEYDOWN, 0x91, 0x00460001L);
	PostMessage(hWnd, WM_KEYUP, 0x91, 0xC0460001L);
}




#define w_char( dest, ch )      (*(dest)++ = ch)
#define w_str( dest, str, len ) for ( ; (len)>0; (len)--, (str)++ ) \
                                        w_char( (dest), *(str) )
        

static void
w_string( char **pdest,
	  register const char *str,
          int minlen,
          int maxlen,
          char fill )
{
unsigned count, c2;
register const char *p;
char    *dest;
        dest = *pdest;

        count = 0;
        p = str;
        while ( *p++ )
                if ( ++count == maxlen )
                        break;

        if ( minlen > 0 ) {
                for ( ; minlen>count; --minlen )
                        w_char( dest, fill );
        }

        c2 = count;
        w_str( dest, str, c2 );

        if ( minlen < 0 ) {
                for ( minlen=-minlen; minlen>count; --minlen )
                        w_char( dest, fill );
        }
        *pdest = dest;
}


static char
get_int( va_list *parg, const char **pp, int *vp )
{
register const char *p;
register int val;
int sign;
char firstc;
        p = *pp;

        if ( *p=='-' ) {
                ++p;
                sign = -1;
        }
        else
                sign = 1;
        firstc = *p;

        if ( *p=='0' )          /* skip zero in case '0*' */
                ++ p;

        if ( *p=='*' ) {        /* get the value */
                ++ p;
                val = va_arg( *parg, int );
        }
        else {
                val = 0;
                for ( ; isdigit(*p); p++ )
                        val = val * 10 + *p - '0';
        }
        *vp = val * sign;
        *pp = p;
        return firstc;
}


void LibDelay(USHORT usDelay)
  {
  MSG  msg;
  LONG lTimerOut;


  lTimerOut = GetCurrentTime() + usDelay;
  while (lTimerOut > GetCurrentTime())
    {
    PeekMessage((LPMSG)&msg, NULL, 0, 0, FALSE);
    Yield();  // 00066 Use Yield() as a nicer function for this service.
    }
  }


int cdecl sprintf( char *dest, const char *fmt, ... )
{
va_list                 arg;
register const char     *reg_p;
const char              *auto_p;
char                    buf[ CHAR_BIT * sizeof(long) + 1 ];
int                     val, val2;
int                     base;
int			isnear, isfar, islong;
char                    fill;
char                    *org_dest;

    org_dest = dest;

    va_start(arg,fmt);
    reg_p = fmt;

    for ( ;*reg_p; reg_p++ ) {
        if ( *reg_p=='%' ) {
	    isnear = isfar = islong = FALSE;
            val = val2 = 0;
            base = 10;
            fill = ' ';
            auto_p = reg_p + 1;
            if ( get_int( &arg, &auto_p, &val ) == '0' )
                fill = '0';
            if ( *auto_p=='.' ) {
                ++auto_p;
                (void) get_int( &arg, &auto_p, &val2 );
            }
            reg_p = auto_p-1;
next_fmt:
            switch ( *++reg_p ) {

                case 'l':
                    islong = TRUE;
                    goto next_fmt;

                case 'B':
                    auto_p = reg_p+1;
                    (void) get_int( &arg, &auto_p, &base );
                    reg_p = auto_p-1;
                    goto next_fmt;

                case 'n':
                    *va_arg( arg, int * ) = dest - org_dest;
                    break;

                case 's':
                    w_string( &dest,
                              isfar ?
				va_arg(arg,const char *) :
                                (isnear ?
				 (const char *)va_arg(arg,
							  const char *) :
				 (const char *)va_arg(arg, const char *)),
                              val,
                              val2,
                              fill );
                    break;
                                    
                case 'u':
                    w_string( &dest,
                              ltoa( islong ?
                                     va_arg(arg,unsigned long) :
                                     (unsigned long)va_arg(arg,unsigned),
                                    buf,
                                    base ),
                              val,
                              INT_MAX,
                              fill );
                    break;

                case 'd':
                case 'i':
                    w_string( &dest,
                              ltoa( islong ?
                                     va_arg(arg,long) :
                                     (long) va_arg(arg,int),
                                    buf,
                                    base ),
                              val,
                              INT_MAX,
                              fill );
                    break;

                case 'x':
                    w_string( &dest,
                              ltoa( islong ?
                                     va_arg(arg,long) :
                                     (long) va_arg(arg,int),
                                    buf,
                                    16 ),
                              val,
                              INT_MAX,
                              fill );
                    break;
 
 				// dca00044 PJL Add octal number support.
               case 'o':
                    w_string( &dest,
                              ltoa( islong ?
                                     va_arg(arg,unsigned long) :
                                     (unsigned long)va_arg(arg,unsigned),
                                    buf,
                                    8 ),
                              val,
                              INT_MAX,
                              fill );
                    break;

                default:
                    w_char( dest, *reg_p );
            }
        }
        else {
            w_char( dest, *reg_p );
        }
    }
    w_char( dest, '\0' );
    return dest - org_dest - 1;         /* length of string */
}
