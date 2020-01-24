#include "nowin.h"
#undef NODRAWTEXT
#include <windows.h>
#include <winundoc.h>
#include <stdio.h>
#include <io.h>
//#include <direct.h>
//#include <ctype.h>
//#include <sys\types.h>
//#include <sys\stat.h>
#include "rc_symb.h"
#include "fcntl.h"

#include "stdinc.h"
#include "comm.h"
#include "file.h"
#include "settings.h"
#include "messages.h"
#include "ws.h"
#include "custinfo.h"
#include "misc.h"
#include "error.h"
#include "help.h"
#include <stdarg.h> // don't worry about warning msg
#include "download.h"
#include "math.h"                     /* moved from ftoa */
#include "string.h"
#include "database.h"
#include "dos.h"    // for _dos_getdiskfree()
#include "direct.h" // for mkdir()
#include "busy.h"

HANDLE  hWndTimer = NULL;
FARPROC lpprocTimerDlg;

extern WORD wWinVer;

/*--------------------------------------------------------------------------
/*			                                                   
/*     Function:  CenterPopUp
/*                                                               
/*  Description:  Causes the popup window whose handle is passed in to
/*                be centered relative to entire display area.  
/*
/*    Called by:  Any pop-up who wants to be centered.  Mostly called
/*                by dialog box procedures when the receive WM_INITDIALOG
/*                messages.
/*
/*        Input:  hWnd - PopUp window's handle
/*
/*      Returns:  Nothing
/*
/*          NOTE: THIS FUNCTION CANNOT BE USED TO CENTER CHILD WINDOWS.
/*                CHILD WINDOW POSITIONING IS RELATIVE TO THE PARENT.
/*                THE POSITIONING OF POPUPS (INCLUDING DIALOG BOXES) IS
/*                RELATIVE TO THE UPPER-LEFT OF THE DISPLAY.
/*
--------------------------------------------------------------------------*/
VOID FAR PASCAL 
CenterPopUp(HWND hWnd)
{
	RECT	rWindow;
	static  xScreen=0;
	static  yScreen=0;
	int	 height, width;
	POINT   start;


	GetWindowRect(hWnd, &rWindow);

	height=rWindow.bottom-rWindow.top;
	width=rWindow.right-rWindow.left;

	if (!xScreen)
	{
		xScreen=GetSystemMetrics(SM_CXSCREEN);
		yScreen=GetSystemMetrics(SM_CYSCREEN);
	}

    start.x=(xScreen-width) >> 1;
	start.y=(yScreen-height) >> 1;

	MoveWindow(hWnd, start.x, start.y, width,
			height, FALSE);
}




/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  SetStatusBarText                                       */
/*                                                                        */
/*   Description:  Sets the text in the Window Shopper Status bar window  */
/*                 to the string passed in.                               */
/*                                                                        */
/*       History:  modified 4/26/91 to ident text to align with list boxes*/
/*                 etc.                                                   */
/*                                                                        */
/*------------------------------------------------------------------------*/
VOID FAR PASCAL 
SetStatusBarText (char *szMsg) 
{
	if (hWndStatusBar)
   {
       char szTemp[80] = "   ";

	    SetWindowText(hWndStatusBar, (LPSTR)strcat(szTemp, szMsg));
   }
}


void FAR 
wprintf(NPSTR msg, ...)
{
#ifdef DEBUG
    va_list argList;
    char    sBuffer[384];

    va_start(argList, msg);
    WSvsprintf(sBuffer, msg, argList);
    va_end(argList);
    MessageBox(GetActiveWindow(), (LPSTR)sBuffer, (LPSTR)"wprintf",  MB_OK);
#endif
}

#ifdef INTERNAL
void FAR
AuxPrint(char *smsg,...)
{
#ifdef COMPAQ
{
    FILE *fp;
    va_list argList;
    char LogFile[_MAX_PATH];

    wsprintf((LPSTR), (LPSTR)"%s%s", (LPSTR)szExePath, (LPSTR)"wsdebug.log");

    if ((fp = fopen(LogFile, "a")) == NULL)
        return;

    va_start(argList, smsg);    
    vfprintf(fp,smsg,argList);
    va_end(argList);
    fprintf(fp,"\n\r");
    fflush(fp);
    fclose(fp);
}
#else
{
    va_list argList;
    char buf[200];

    va_start(argList, smsg);    
    vsprintf(buf,smsg,argList);
    va_end(argList);
    strcat(buf,"\n\r");
    OutputDebugString((LPSTR)buf);
}
#endif
}
#endif


/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSAboutDlgProc                                         */
/*                                                                        */
/*   Description:  The About Box.                                         */
/*                                                                        */
/*       History:  1-20-90 Originally created.  [lds]                     */
/*                 7-10-91 Added color WS logo! pt                        */
/*                                                                        */
/*------------------------------------------------------------------------*/

#define WSLOGOWIDTH  291   
#define WSLOGOHEIGHT 137
#define LOGOPOSX     24
#define LOGOPOSY     24

LONG FAR PASCAL 
WSAboutDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    REFERENCE_FORMAL_PARAM(lParam);
	switch(message) {
		
		case WM_INITDIALOG:
				CenterPopUp(hWndDlg);
				break;

        case WM_PAINT:
            {
            /* Display the Clip-Art Window Shopper logo, in color!
               The way we do this is with 3 black-and-white, same-sized 
               overlays (since an image in 16-bit color takes muuuuch 
               more disk space than 3 2-bit ones) ..pt */

	        HDC	hDCDlg;
            HDC hDCMem;
            HBITMAP hBitmap, hBitmapPrev;
            PAINTSTRUCT ps;

            hDCDlg = BeginPaint (hWndDlg, &ps);
            hDCMem = CreateCompatibleDC (hDCDlg);

            hBitmap = LoadBitmap (hInstanceWS, "logoovl2");
            hBitmapPrev = SelectObject (hDCMem, hBitmap);
            SetBkColor(hDCDlg, RGB(0xff, 0xff, 0xff) );
            SetTextColor(hDCDlg, RGB(0xff, 0, 0) );
            BitBlt(hDCDlg, LOGOPOSX, LOGOPOSY, WSLOGOWIDTH, WSLOGOHEIGHT, hDCMem, 0, 0, SRCCOPY);
            SelectObject(hDCMem, hBitmapPrev);
            DeleteObject(hBitmap);

            hBitmap = LoadBitmap (hInstanceWS, "logoovl1");
            hBitmapPrev = SelectObject (hDCMem, hBitmap);
            SetTextColor(hDCDlg, RGB(0, 0, 0) );
            BitBlt(hDCDlg, LOGOPOSX, LOGOPOSY, WSLOGOWIDTH, WSLOGOHEIGHT, hDCMem, 0, 0, SRCAND);
            SelectObject(hDCMem, hBitmapPrev);
            DeleteObject(hBitmap);

            hBitmap = LoadBitmap (hInstanceWS, "logoovl3");
            hBitmapPrev = SelectObject (hDCMem, hBitmap);
            SetTextColor(hDCDlg, RGB(0, 0, 0xff) );
            BitBlt(hDCDlg, LOGOPOSX, LOGOPOSY, WSLOGOWIDTH, WSLOGOHEIGHT, hDCMem, 0, 0, SRCAND);
            SelectObject(hDCMem, hBitmapPrev);
            DeleteObject(hBitmap);

            ReleaseDC(hWndDlg, hDCMem);
            EndPaint(hWndDlg, &ps);
            return(FALSE);
            }

		case WM_COMMAND:
            switch (wParam)
            {
                case ID_ADONIS:
                    GetHelp(GH_TEXT, IDH_ADONIS);
                    break;
                case ID_WELCOME:
                    GetHelp(GH_TEXT, IDH_WELCOME);
                    break;           
                case ID_DEMO:
                    GetHelp(GH_TEXT, IDH_DEMO);
                    break;
                case IDOK:
				    EndDialog(hWndDlg, TRUE);
				    break;
                default:
                    return(FALSE);

			}
			break;
		default:
			return(FALSE);
	} /* end switch message */
	return(TRUE);
}

int FAR PASCAL
QueryListSel(HWND hWnd)
{
    int nIndex,nCount;

    nCount = (WORD)SendMessage(hWnd,LB_GETCOUNT,0,0L);
    if (nCount == 0)
        return -1;

    if ((nIndex= (int) SendMessage(hWnd,LB_GETCURSEL, 0, 0l)) == LB_ERR)
    /* SIDE EFFECT: correct for selecting below last item */
    {
        nIndex = nCount - 1;
        SendMessage(hWnd,LB_SETCURSEL,nIndex,0L);
    }

    return nIndex;
}

char* FAR PASCAL
StripPath(char *str)
/* return address into string of the base file name */
{
    char c;
    char* f;
    char* p = str;

    if (NOT *p)
        return p;

    for (f = p; c = *p;)
    {
		p = (PSTR)AnsiNext((LPSTR)p);
		if (c == ':' || c == '\\' || c == '/')
			f = p;
	 }
    return f;
}

#if 0
/* remove trailing filename, filespec, directory and or backslash and
   return address to string of path to parent dir
   Note: if the parent directory is the root directory, this will leave the
   trailing backslash
   ex:
      char path[_PATH_] = "A:\\BOB";
      MakeParentDir(path) returns "A:\\"

      char path[_PATH_] = "A:\\BOB\\JOHN.DOE";
      MakeParentDir(path) returns "A:\\BOB"

*/
char* FAR PASCAL
MakeParentDir(char *szPath)
{
    LPSTR p;
    char c;

    if (NOT *szPath)
        return szPath;

    for (p = (LPSTR)(szPath + strlen(szPath) - 1); c = *p;)
    {
		   if (c != ':' AND c != '\\' AND c != '/')
         {
            *p = '\0';
	         p = AnsiPrev((LPSTR)szPath, p);
         }
         else
         {
            if ((c == '\\') AND (*(AnsiPrev((LPSTR)szPath, p)) == ':')) //keep backslash after root dir
               return szPath;
            else
            {
               if (c == ':')   // add backslash after root dir
               {
                  p = AnsiNext(p);
                  *p = '\\';
               }
               else
                  *p = '\0';
            }
            return szPath;
         }
	 }
    return szPath; //which is null if not a valid path to begin with
}
#endif

char *
BaseName(char *szFileName)
{
    char *p;
    szFileName = StripPath(szFileName);
    if (p = strchr(szFileName,'.'))
        *p = '\0';
    return (szFileName);
}



#if 0
char *FAR PASCAL
sReadToChar(char *SrcStr, char chartoget)
{
    while (*SrcStr != chartoget)
        ++SrcStr;

    return SrcStr;
}


char *FAR PASCAL
sReadStringToChar(char *SrcStr, char *DestStr, char chartoget)
{
    while (*SrcStr != chartoget)
        *DestStr++ = *SrcStr++;

    *DestStr++ = *SrcStr++;
    return SrcStr;
}
#endif

void FAR PASCAL
ftoa(double f, char *str, int fract_digits)
/** convert double to string with fract_digits digits in fractional part **/
{
    double fWhole,fFract;
    int sig_digits,len;
    BOOL small;

    /* get sign digits */
    fFract = modf(f,&fWhole);
    if (fWhole == 0.0)
    {
        f = f + 1.0;
        small = TRUE;
    }
    else
        small = FALSE;

    itoa((int)fWhole,str,10);
    sig_digits = strlen(str);

    gcvt(f,sig_digits+fract_digits,str);

    /* pad */
    if (small)        
        *str = '0';

    str = strchr(str,'.');
    str++;

    /** set fract_digits.  str points to fract. */
    if ((len = strlen(str)) < fract_digits) // then pad with zeroes 
    {
        str += len; // point to '\0';
        while (len < fract_digits)
        {
            *str++ = '0';
            ++len;
        }
        *str = '\0';
    }
    else // truncate
        *(str + fract_digits) = '\0';
}

void FAR PASCAL
ultostr(unsigned long l, char *s, int padchars)
/* convert l to string (hex) with padchars leading zeroes */
{
    int i;
    BYTE remainder;

    i = 0;

    do                          /* generate digits in reverse order */
    {
        remainder = (BYTE)(l % 16L);  /* get next digit */
        if (remainder < 10)
            s[i++] = (char)(remainder + '0');  /* get next digit */
        else
            s[i++] = (char)(remainder - 10 + 'A');  /* get next digit */
    }
    while (l /= 16L);            /* delete it */

    padchars -= i; // i is number of digits thus far
    if (padchars > 0)
    {
        while(padchars--)
            s[i++] = '0';
    }

    s[i] = '\0';
    strrev(s);
}


void FAR PASCAL
ultostr10(unsigned long l, char *s, int padchars)
/* convert l to string (decimal) with padchars leading zeroes */
{
    int i;
    BYTE remainder;

    i = 0;

    do                          /* generate digits in reverse order */
    {
        remainder = (BYTE)(l % 10L);  /* get next digit */
        s[i++] = remainder + '0';  /* get next digit */
    }
    while (l /= 10L);            /* delete it */

    padchars -= i; // i is number of digits thus far
    if (padchars > 0)
    {
        while(padchars--)
            s[i++] = '0';
    }

    s[i] = '\0';
    strrev(s);
}


int PASCAL
CopyFile(char *sSrcName,char *sDestName)
{
    int hSrcFile= -1,hDestFile= -1, nRetval = OK;
    char buf[256];
    unsigned int n;

    /* open src file */
    if ((hSrcFile = _lopen(sSrcName,READ)) == -1)
        END(FAIL)


    /* open dest file */
    if ((hDestFile = _lcreat(sDestName,0)) == -1)
        END(FAIL)

    while ((n = _lread(hSrcFile, buf, 256)) > 0)
        if (_lwrite(hDestFile, buf, n) != n)
            END(FAIL)

    end:
    if (hSrcFile != -1)
        _lclose(hSrcFile);
    if(hDestFile != -1)
        _lclose(hDestFile);
	
    return(nRetval);
}

char * FAR PASCAL
AddFullPath(char *str)
/* !!! this doesn't work for wildcarded file specs !!! (10.4.90) D. Kent */
{
    char tmp[_MAX_PATH];

    /** don't add path to NULL file **/
    if (bNULLFILE(str))
        return(str);

    if (str[1] == ':') /* fullpath already set */
        return(str);

    lstrcpy(tmp,str);
    lstrcpy(str,szDataPath);
    lstrcat(str,tmp);
    return (str);
}

int FAR PASCAL 
FillEditBox(int fp, HANDLE hEdit, char StopChar)
{
    int nRetval = OK,count=0;
    HANDLE hBuf = NULL;
    NPSTR pBuf = NULL;
    char c;

    if ((hBuf = LocalAlloc(LMEM_MOVEABLE,1)) == NULL)
        END(FAIL)

    /** now read one character at a time, reallocing as we go (is there a
        better way? **/
    while (1)
    {
        c = readch(fp);

        if ((c == StopChar) OR (c == FAIL))
            break;

        /** ignore '\n' since these are ascii files **/
        //if (c == '\n')
            //continue;

        if ((pBuf = LocalLock(hBuf)) == NULL)
            END(FAIL)

        pBuf[count++] = c;

        LocalUnlock(hBuf);

        if ((hBuf = LocalReAlloc(hBuf,count+1,LMEM_MOVEABLE)) == NULL)
            END(FAIL)
    }

    if ((pBuf = LocalLock(hBuf)) == NULL)
        END(FAIL)

    if ( StopChar == EOF )
        count--;

    pBuf[count] = '\0';

    SetWindowText(hEdit,(LPSTR)pBuf);

    end:
    if (hBuf)
        LocalUnlock(hBuf);

    if (pBuf)
        LocalFree(hBuf);

	return (nRetval);
}


/*************************************************************\
 ** RoutineName FileSize
 **
 ** ARGUMENTS: char *szSrc -- fully delimited file name (path+fname)
 **
 ** RETURNS: (DWORD)
 **  size of file if successful, NULL if not.
 **
 ** HISTORY:   John Ano  May 31, 1990 (written)
 **  (8.24.90) D. Kent -- Modified, added filelength and close calls.
\*************************************************************/

DWORD FAR PASCAL
FileSize(char *szSrc)

{
HANDLE fh = -1;
DWORD siz;
    if ((fh=_lopen(szSrc,0))== -1) /* open it */
        return (0L);
#if 0
    if ((siz = (DWORD)_llseek(fh,0L,2)) == -1) // position 0 past EOF
    {
        _lclose(fh);
        return (0L) ;
    }
#else
    if ((siz = (DWORD)filelength(fh)) == -1)
    {
        _lclose(fh);
        return (0L) ;
    }
#endif
    if (fh != -1)
        _lclose(fh) ;
return(siz) ;
}


void FAR PASCAL
FixSpec(char *str)
/* append WILDCARD to directory */
{
    Append(str,NULL); // guarantee trailing backslash

    lstrcat(str,WILDCARD);
}

char* FAR PASCAL
UnFixSpec(char *str)
/* take off the "\." at the end of a directory */
{
    char* c = str + strlen(str) - 1 ;
    if ((*c == '.') OR
        (*c == '\\'))
        *c = '\0';
    return str;
}

char* FAR PASCAL
Append(char *path1, char *path2)
/* append path2 onto end of path1 */
{
    /* first string will have trailing backslash */
    if (path1[0])
    {
        int len = strlen(path1);

        if (path1[len-1] == '.')
           path1[len-1] = '\0';
        else if (path1[len-1] != '\\')
            lstrcat(path1,"\\");
    }

    /* second string will not have leading backslash */
    if (path2[0])
    {
        if (path2[0] == '\\')
            path2 = (char *)AnsiNext((LPSTR)path2);
        lstrcat(path1,path2);
    }
    return path1; // path2 is null
}

BOOL FAR PASCAL
IsFile(char *str)
{
    int len = strlen(str);

    if (len == 0)
        return FALSE;

    switch (str[len-1])
    {
        case '\\':  case ':': case '.':
            return FALSE;
        break;
        default:
            return TRUE;
        break;
    }
}

void
get_cur_dir(char *dir)
{
    //if (NOT DirExists("."))  // If cur drive is unaccessible
      // lstrcpy(dir, szExePath);
    //else
    //{
       OFSTRUCT OpenStr;
       OpenFile(".", &OpenStr, OF_PARSE);
       lstrcpy(dir, OpenStr.szPathName);
//    }
}

BOOL FAR PASCAL
GetExePath(void)
{
    int  cch;
    char DataDir[_MAX_PATH];

    if (cch = GetModuleFileName(hInstanceWS, szExePath, _MAX_PATH));
    {
        /* guarantee that \ is on end of path */
       while(szExePath[cch] != '\\')
         szExePath[cch--] = 0;
    }

    /*  compose data directory name */
    lstrcpy(szDataPath, szExePath);
    LoadString(hInstanceWS, IDS_DATA_DIR, DataDir, sizeof(DataDir));
    Append(szDataPath, DataDir);

    /*
     * make directory if required
     */
    if (NOT DirExists(szDataPath))
    {
        if (mkdir(szDataPath) != 0)
        {
            ErrMsg(IDS_ERR_CREATING_DIR, (LPSTR)szDataPath);
            return(FALSE);
        }
    }

    /* 
     * guarantee trailing backslash
     */
    Append(szDataPath, NULL);
    return(TRUE);
}

BOOL FAR PASCAL
DirExists(char* path)
{
    char tmpPath[_MAX_PATH];
    strcpy(tmpPath, path);

    //Need to tack on a trailing "\." to path
    UnFixSpec(tmpPath);
    Append(tmpPath, ".");

    if (access (tmpPath, 0) != 0)
    {
#ifdef DEBUG
        AuxPrint("DirExists return FALSE for %s", path);
#endif
        return(FALSE);
    }
    return( TRUE );
}


BOOL FAR PASCAL 
CreateLibWnd(void)
{
    HMENU hWSMenu;
    int scWidth,scHeight;

    WNDCLASS	wndclass;

	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc=WSLibraryDlgProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance=hInstanceWS;
	wndclass.hIcon= LoadIcon(hInstanceWS, "shopper");
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=COLOR_WINDOW + 1;
	wndclass.lpszMenuName="LIBMENU";   /* no Menu */
	wndclass.lpszClassName=szAppName; /* class name */

	if(!RegisterClass(&wndclass))
		return(FAIL);

    hWSMenu=LoadMenu(hInstanceWS,"LIBMENU");

    /* get screen dimensions */
    scWidth = GetSystemMetrics(SM_CXSCREEN);
    scHeight = GetSystemMetrics(SM_CYSCREEN);


    /**
        Original window dimensions on VGA are 460x425.
    **/

        scWidth  = (int)(scWidth * 0.71875 + 0.5);
        scHeight = (int)(scHeight * 0.8854167 + 0.5),

#ifdef INTERNAL
    //AuxPrint("screen x: %d  screen y: %d",scWidth,scHeight);
#endif

    /** !!! if you change the dimensions of the window, see CreateChildren() */
    hWndWS = CreateWindow(szAppName, 
                          szTitleAppName,
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                          CW_USEDEFAULT,CW_USEDEFAULT,
                          scWidth, scHeight,
                          NULL,
                          hWSMenu,
                          hInstanceWS,
                          NULL);
    if (hWndWS == NULL)
        return FAIL;

    NewUserConfig(hWSMenu,bNewUser);
    EnableMenuItem(hWSMenu,3,MF_GRAYED|MF_BYPOSITION|MF_DISABLED);


    return OK;
}

void FAR PASCAL
NewUserConfig(HMENU hM, BOOL bFlag)

{
    if (bFlag)
    {
        EnableWindow(hChildren[ID_LIB_NEWUSER].hWnd, 1);
        ShowWindow(hChildren[ID_LIB_NEWUSER].hWnd, SW_SHOW);

        EnableWindow(hChildren[ID_LIB_LOGON].hWnd, 0);
        ShowWindow(hChildren[ID_LIB_LOGON].hWnd, SW_HIDE);

        EnableMenuItem(hM, IDM_CUSTOMER_INFO, MF_GRAYED | MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(hM, IDM_SHIPPING_INFO, MF_GRAYED | MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(hM, IDM_BILLING_INFO, MF_GRAYED | MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(hM, IDM_PASSWORD, MF_GRAYED | MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(hM, 1, MF_ENABLED | MF_BYPOSITION);
        EnableMenuItem(hM, IDM_COMM, MF_GRAYED | MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(hM, IDM_LIB_MANAGEMENT, MF_ENABLED | MF_BYCOMMAND);

        EnableMenuItem(hM, 2, MF_ENABLED | MF_BYPOSITION);
        EnableMenuItem(hM, IDM_UPDATE_THUMB, MF_ENABLED | MF_BYCOMMAND);

#if 0
        EnableMenuItem(hM, IDM_SET_PHONENUM, MF_ENABLED | MF_BYCOMMAND);
#endif

        SetFocus(hChildren[ID_LIB_NEWUSER].hWnd);

        /* zap userid from win.ini */
        custinfo.userid[0] = '\0';
        WriteCustInfo(WRITE_USERID);
    }
    else
    {
        EnableWindow(hChildren[ID_LIB_LOGON].hWnd, 1);
        ShowWindow(hChildren[ID_LIB_LOGON].hWnd, SW_SHOW);
        EnableWindow(hChildren[ID_LIB_NEWUSER].hWnd, 0);
        ShowWindow(hChildren[ID_LIB_NEWUSER].hWnd, SW_HIDE);
        SetFocus(hChildren[ID_LIB_LOGON].hWnd);
        EnableMenuItem(hM,0,MF_ENABLED|MF_BYPOSITION);
        EnableMenuItem(hM,1,MF_ENABLED|MF_BYPOSITION);
        EnableMenuItem(hM,IDM_LIB_MANAGEMENT,MF_ENABLED|MF_BYCOMMAND);
        EnableMenuItem(hM,IDM_COMM,MF_ENABLED|MF_BYCOMMAND);

        EnableMenuItem(hM,2,MF_ENABLED|MF_BYPOSITION);
        EnableMenuItem(hM,IDM_UPDATE_THUMB,MF_ENABLED|MF_BYCOMMAND);

#if 0
        EnableMenuItem(hM,IDM_SET_PHONENUM,MF_ENABLED|MF_BYCOMMAND);
#endif

    }
    if ( IsWindow (hWndWS) )
        DrawMenuBar(hWndWS);
    bNewUser = bFlag;
}

WORD
GetWindowsVersion(void)
/* returns a hex 3-digit number like 210, 200, 300, ... */
{
    WORD nVersion,wRetval;
    
    nVersion = GetVersion();
    wRetval = (nVersion & 0xff) << 8;
    wRetval += ((nVersion & 0xff00) >> 8);

    return wRetval;
}

/*************************************************************\
 ** SuperClassEdits
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  This creates a class of control called "SuperEdit" which can be 
 **  used as "read only" multiline edit boxes.  The proc 
 **  WSMultiEditSubProc() processes the messages in such a way as 
 **  to effect the read only character of the control.
 **
 **  It also creates a class of control called "SuperPassEdit" which can be 
 **  used to handle password edit boxes.  The proc 
 **  WSPasswordEditProc() processes the messages in such a way as 
 **  put blank characters where the typed in characters are.
 **  A control of type "SuperPassEdit" uses one 
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  May 2, 1990 (written)
\*************************************************************/
int
SuperClassEdits(void)
{
    HWND        hEditWnd;
    WNDCLASS    Class;

    /* We only create this window to be able to get information out of it. */
    hEditWnd = CreateWindow((LPSTR)"EDIT",
                            (LPSTR)"",
                            WS_POPUP,
                            CW_USEDEFAULT,CW_USEDEFAULT,
                            CW_USEDEFAULT,CW_USEDEFAULT,
                            NULL,             /* no parent */
                            (HMENU)NULL,       /* use class menu */
                            (HANDLE)hInstanceWS, /* handle to window instance */
                            (LPSTR)NULL        /* no params to pass on */
                            );

    if (hEditWnd == NULL)
        return FAIL;

    /* Get the stuff we need to save from the class, and make our new class. */

    /* Save this for use later in our new window proc. */
    lpprocEditWnd = (FARPROC)GetWindowLong(hEditWnd, GWL_WNDPROC);

    if (lpprocEditWnd == NULL)
        return FAIL;

    /* Register the new super class to the standard edit control.
       We make it the same as the original except for the new function,
       hInstance, and class name. */
    /* Note that we must be very careful about the window extra bytes. If
       our superclassing procedure doesn't need them, the code given here
       is correct. If we want to use them, we must take note of how many
       bytes the standard control procedure uses, add ours to cbWndExtra,
       and access them after the standard control's bytes since it will
       access the bytes starting at zero. */
    Class.style         = GetClassWord(hEditWnd, GCW_STYLE);
    Class.lpfnWndProc   = WSMultiEditSubProc;
    Class.cbClsExtra    = GetClassWord(hEditWnd, GCW_CBCLSEXTRA);
    Class.cbWndExtra    = GetClassWord(hEditWnd, GCW_CBWNDEXTRA);

    /* We want to use our instance here so that the class will be destroyed
       when the last instance terminates.
           Classes are destroyed at app exit time by searching the list of
       classes looking for any classes with the current hModule. (Derived
       from hInstance.) Hence, to get the class "SuperEdit" destroyed, we
       must set its hInstance to our own. The internal class structure
       actually stores hModule rather than hInstance. (This gets converted
       by RegisterClass().) */
    Class.hInstance     = hInstanceWS;

    Class.hIcon         = GetClassWord(hEditWnd, GCW_HICON);
    Class.hCursor       = GetClassWord(hEditWnd, GCW_HCURSOR);
    Class.hbrBackground = GetClassWord(hEditWnd, GCW_HBRBACKGROUND);
    Class.lpszMenuName  = NULL; /* It doesn't have one. */
    Class.lpszClassName = (LPSTR)"SuperEdit";

    if (!RegisterClass( (LPWNDCLASS)&Class ) ) {
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FAIL;
        }

    /* Now we'll do the super class for the SuperPassEdit class.
       Its really similar to the above, only we'll add a bit in 
       Class.cbWndExtra field */
    Class.lpfnWndProc   = WSPasswordEditProc;
    Class.lpszClassName = (LPSTR)"SuperPassEdit";

    nOffsetToPass      = Class.cbWndExtra >> 1;
    Class.cbWndExtra   += 2;

    if (!RegisterClass( (LPWNDCLASS)&Class ) ) {
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FAIL;
        }

    /* We don't need it any more. */
    DestroyWindow(hEditWnd);

    return OK;        /* Initialization succeeded */
}

#if 0
void PASCAL
HandleIfNewUser(void)
{
    SetDest (ES_ACTIVEWND, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
	if (ErrMsg(IDS_ERR_ARE_USER) == IDNO)
	{
        NewUserConfig(GetMenu(hWndWS),TRUE);
        ResetCustInfo (TRUE);
	}
}
#endif

unsigned long FAR PASCAL
AvailDiskSpace(int nWhichDisk)
/* 0 = default drive, -1 = exepath */
{
#if 0
    struct diskfree_t diskspace;

    if (nWhichDisk == -1)
        nWhichDisk = tolower(szDataPath[0]) - 'a' + 1;

    /** see if space is available **/
    _dos_getdiskfree(nWhichDisk,&diskspace);
    return((unsigned long)diskspace.avail_clusters * 
                 diskspace.sectors_per_cluster *
                 diskspace.bytes_per_sector);
#else
    extern unsigned long FAR PASCAL DosDiskFreeSpace(int);
    if (nWhichDisk == -1)
        nWhichDisk = tolower(szDataPath[0]) - 'a' + 1;

#ifdef INTERNAL
    AuxPrint("disk avail: %ld",DosDiskFreeSpace(nWhichDisk));
#endif
    return (DosDiskFreeSpace(nWhichDisk));
#endif
}

/* FixDirPath()
      - if Window handle is null than func just fixes end of string
      - removed side effect of deleteing a trailing backslash and "." in the
        input string
         - beng
 */

BOOL
FixDirPath (HWND hWndText, char* OldText, char* NewText, int size)
{
	HDC	hDC;
	RECT	wndRect, textRect;
	char  *LastDir, *startRest;
	BOOL	retVal = TRUE;
	int	length;
	DWORD	dwExtent;
   char  szCopyOld[_MAX_PATH];

   strcpy(szCopyOld, OldText);
   if ((length = strlen (szCopyOld)) > 3)
   {
	   if ((szCopyOld[length - 1] == '.')  AND (szCopyOld[length - 2] == '\\'))
         szCopyOld[length - 1] = '\0';

      length = strlen (szCopyOld);

	   if ((szCopyOld[length - 1] == '\\') AND (szCopyOld[length - 2] != ':'))
         szCopyOld[length - 1] = '\0';
   }

	if (NOT hWndText)
   {
      strcpy(NewText, szCopyOld);
      return TRUE;
   }

	hDC = GetDC (hWndText);
   GetWindowRect (hWndText, &wndRect);
	CopyRect (&textRect, &wndRect);

	dwExtent = GetTextExtent (hDC, szCopyOld, strlen (szCopyOld));

	if ( LOWORD (dwExtent) <= (WORD)(wndRect.right - wndRect.left) )
	{
		strncpy (NewText, szCopyOld, size);
		retVal = FALSE;
	}
	else
	{
		NewText[0] = szCopyOld[0];
		NewText[1] = szCopyOld[1];
		NewText[2] = '\0';
		strcat (NewText, "\\...");
		startRest = NewText + strlen(NewText);
		LastDir = strrchr (szCopyOld, '\\');
		strcpy (startRest, LastDir);
		retVal = TRUE;
	}
	ReleaseDC (hWndText, hDC);
	return retVal;
}


/**************************************************************
 WSDecisionDlgProc is used when a thumbnail file cannot be obtained.
 The user is asked if they want to obtain the thumb from floppy, cancel
 the operation or logon. Text for the buttons and ID_DESC is changed
 according to the operation which tried to obtain the thumbnail: at this
 point either view or search mode. 05-Aug-1991 - beng
**************************************************************/

LONG FAR PASCAL 
WSDecisionDlgProc (HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    REFERENCE_FORMAL_PARAM(lParam);

    switch ( message ) 
    {
        case WM_INITDIALOG:
        {
            char szDesc[MAX_MSG_LEN + MAX_STR_LEN];
            char sztmp[MAX_MSG_LEN];
            switch (nMissingThumbContext)
            {
               case SEARCH_MODE :
               {
                  //Display description of error contextually
           	      LoadString(hInstanceWS, IDS_ERR_MISSING_SEARCH_THUMB,
                             (LPSTR)sztmp, MAX_MSG_LEN);
                  //More literal "Cancel" button
                  SetDlgItemText(hDlg, IDCANCEL, (LPSTR)"&Cancel Search");
                  //Make continue search default button
                  SendDlgItemMessage (hDlg, IDB_CONTINUE_SEARCH, BM_SETSTYLE, (WORD)BS_DEFPUSHBUTTON, 0L);
                  //"Un-default" cancel button
                  SendDlgItemMessage (hDlg, IDCANCEL, BM_SETSTYLE, (WORD)BS_PUSHBUTTON, 0L);
                  break;
               }

               default:
               case VIEW_MODE :
               {
                  //Hide "continue Search button
                  ShowWindow(GetDlgItem(hDlg, IDB_CONTINUE_SEARCH), SW_HIDE);
                  //Display description of error contextually
           	      LoadString(hInstanceWS, IDS_ERR_MISSING_VIEW_THUMB,
                             (LPSTR)sztmp, MAX_MSG_LEN);
                  //resize due to missing "Continue Search" button
                  MoveWindow(hDlg, 0, 0,
                              DLG_TO_SCR_X(MT_VIEWMODE_CX),
                              DLG_TO_SCR_Y(MT_VIEWMODE_CY),
                              FALSE);
                  //set focus to cancel button
                  PostMessage ( hDlg, WM_NEXTDLGCTL,
                                GetDlgItem ( hDlg, IDCANCEL), 1L);
                  break;
               }
            }

            //Add publisher to description  (contains %s in string)
            wsprintf((LPSTR)szDesc, (LPSTR)sztmp, (LPSTR)CURPUB);

            SetDlgItemText(hDlg, ID_DESC, (LPSTR)szDesc);
            if (bNewUser)
                EnableWindow(GetDlgItem(hDlg,IDB_LOGON),FALSE);
				CenterPopUp(hDlg);
			   return TRUE;
        }

        case WM_COMMAND:
        {
            int nResult;

		      switch ( wParam )
            {
                case IDB_LOGON:
                    if (IsWindow(hWndView))
                        PostMessage(hWndView, WM_CLOSE, 0, 0L);
                    else if (IsWindow(hWndSearch))
                        PostMessage(hWndSearch, WM_CLOSE, 0, 0L);
                    PostMessage(hWndWS, WM_COMMAND, ID_LIB_LOGON, 0L);
                    nResult = IDB_LOGON;
                    break;

			       case IDOK:
                    nResult = ID_GET_FROM_FLOPPY;
                    break;

                case IDCANCEL:
                    nResult = IDCANCEL;
                    break;

                case IDB_CONTINUE_SEARCH:
                    nResult = IDB_CONTINUE_SEARCH;
                    break;

				    default:
				        return FALSE;
			   }
            nMissingThumbContext = DEFAULT_MODE;
            EndDialog(hDlg, nResult);
            break;
        }

        default:
            break;
    }
    return 0L;
}

/**************************************************************
 WSDecisionDlgProc now allows the user to choose the THUMB 
 directory on either floppy drive or the hard drive under
 the current Shopper directory, simple as that.  Users can
 modify it directly if they insist by changing win.ini, of
 course. pt 7/9/91
**************************************************************/

LONG FAR PASCAL 
WSGetAltDriveDlgProc (HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    REFERENCE_FORMAL_PARAM(lParam);

    switch ( message ) 
    {
        case WM_INITDIALOG:
        {
            char szStr[MAX_STR_LEN];
            char szCurPub[MAX_MSG_LEN+1];
            char sztmp[MAX_MSG_LEN+1];
            char szThumb[MAX_STR_LEN];  /* for "THUMB" */

           	LoadString(hInstanceWS, IDS_CURPUB,
                       (LPSTR)(sztmp), MAX_MSG_LEN);
            wsprintf((LPSTR)szCurPub, (LPSTR)sztmp, (LPSTR)CURPUB);
            SetDlgItemText(hDlg, ID_CURPUB, szCurPub);

            szThumb[0] = '\\';
           	LoadString(hInstanceWS, IDS_THUMB_DIR, /* ignore error returned */
                       (LPSTR)(szThumb+1), MAX_STR_LEN-1);

            /* gray out "b" if they don't have a b: drive */
            if (GetDriveType(1) <= 1)   //Param "1" corrosponds to B: drive
                EnableWindow(GetDlgItem(hDlg, IDB_THUMBDIR_B), OFF);

            switch(custinfo.altcache[0])
            {
                /*** If their thumbnail directory is set to A: or B:, then
                     their third choice is constructed to be the thumb
                     directory underneath Shopper (if it exists). pt */

                case 'a':
                case 'A':
       	            CheckDlgButton (hDlg, IDB_THUMBDIR_A, ON);
                    goto LSkip;
                case 'b':
                case 'B':
       	            CheckDlgButton (hDlg, IDB_THUMBDIR_B, ON);
                   LSkip:
                    FixDirPath (GetDlgItem (hDlg, IDB_THUMBDIR_C), 
			                    szExePath, szStr, MAX_STR_LEN);
                    strcat(szStr, szThumb);

                    /* their directory might not exist so we do them a 'favor' */
                    if (NOT DirExists(szStr))
                        mkdir(szStr);
                    break;

                /*** If their thumbnail directory is set to the hard drive,
                     then that's the reasonable thing for the third choice */

                default:
       	            CheckDlgButton (hDlg, IDB_THUMBDIR_C, ON);
                    FixDirPath (GetDlgItem (hDlg, IDB_THUMBDIR_C), 
			                    custinfo.altcache, szStr, sizeof (szStr));
                    break;
            }
            SetDlgItemText (hDlg, IDB_THUMBDIR_C, szStr);

            SetFocus (GetDlgItem (hDlg, IDOK));
				CenterPopUp(hDlg);
			return FALSE;
        }

        case WM_COMMAND:
		    switch ( wParam )
            {
                case ID_HELP:
                    GetHelp(GH_TEXT, IDH_DECISION);
                    return TRUE;

			    case IDOK:
                    if (IsDlgButtonChecked(hDlg, IDB_THUMBDIR_A))
                        GetDlgItemText(hDlg, IDB_THUMBDIR_A, custinfo.altcache, _MAX_PATH);
                    else if (IsDlgButtonChecked(hDlg, IDB_THUMBDIR_B))
                        GetDlgItemText(hDlg, IDB_THUMBDIR_B, custinfo.altcache, _MAX_PATH);
                    else
                    {
                        Assert (IsDlgButtonChecked(hDlg, IDB_THUMBDIR_C));
                        GetDlgItemText(hDlg, IDB_THUMBDIR_C, custinfo.altcache, _MAX_PATH);
                    }

                    WriteCustInfo (WRITE_USECACHE);
                    EndDialog(hDlg, TRUE);
                    return(TRUE);

                case IDCANCEL:
                case IDB_LOGON:
	                EndDialog (hDlg, IDCANCEL);
                    break;

				default:
				    break;
			}
            break;

        default:
		    break;
    }
    return 0L;
}

/*************************************************************\
 ** FixAmpersands
 **
 ** ARGUMENTS:		char *InString, *FixedString
 **  				int	size
 **
 ** DESCRIPTION:	fix InString so that any ampersand in the string is
 **  				doubled (so it doesn't print out as an underscore)
 **  
 **  				can be modified later so that any character in the	
 **  				string can be doubled or "fixed"
 **
 ** ASSUMES:		FixedString is a buffer big enough to hold the 
 **  				expanded string.
 **
 ** MODIFIES:		FixedString
 **
 ** RETURNS:		Nothing
 **
 ** HISTORY:		Lauren Bricker  September 18, 1990 (written)
\*************************************************************/
void FAR PASCAL FixString (char* InString, char* FixedString, int size)
{
	int   ii     = 0;
	char *InPtr  = InString;
	char *OutPtr = FixedString;

	while ( *InPtr && (ii < size - 1) )	// Always leave one for the NULL char!
	{
		if ( *InPtr == '&' )
		{
			if ( ii < size - 2 )		// If doubling a char, make sure there
			{							// is enough room...
				*OutPtr = '&';
				OutPtr++;
				ii++;
			}
		}
		*OutPtr++ = *InPtr++;
		ii++;

	}
	*OutPtr = '\0';
}

HWND hMommaWnd=NULL;

HCURSOR FAR PASCAL
MySetCursor(HCURSOR hCursor)
{
#if 0
    if (hMommaWnd)
        SetClassWord(hMommaWnd,GCW_HCURSOR,hCursor);
    if (hWndWS)
        SetClassWord(hWndWS,GCW_HCURSOR,hCursor);
    if (hWndView)
        SetClassWord(hWndView,GCW_HCURSOR,hCursor);

    {
        HWND hTmp,hParentWnd;
        hParentWnd = GetActiveWindow();
        if (hParentWnd)
        {
            if (hTmp = GetParent(hParentWnd))
                hParentWnd = hTmp;
            SetClassWord(hParentWnd,GCW_HCURSOR,hCursor);
        }
    }
#endif

    return (SetCursor(hCursor));
}

FARPROC lpEnableWindowProc=NULL;

void
EnableAllModeless(BOOL bEnable)
{
    if (IsWindow(hWndHelpText))
        EnableWindow(hWndHelpText, bEnable);

    if (IsWindow(hBusyBox))
        EnableWindow(hBusyBox, bEnable);

    if (IsWindow(hWndTimer))
        EnableWindow(hWndTimer, bEnable);

    if (IsWindow(hWndView))
        EnableWindow(hWndView, bEnable);

    if (IsWindow(hDldWnd))
        EnableWindow(hDldWnd, bEnable);

    if (IsWindow(hWndSearch))
        EnableWindow(hWndSearch, bEnable);
}

void
EnableAllWindows(BOOL bOnOff)
{

#ifdef INTERNAL
    //AuxPrint("hTask: %x, hInstance: %x",hTaskWS,hInstanceWS);
#endif

    if (NOT hInstanceWS)
        return;

    if (lpEnableWindowProc == NULL)
        lpEnableWindowProc = MakeProcInstance(EnableWindowProc,hInstanceWS);
        
    while(EnumTaskWindows(hTaskWS,lpEnableWindowProc,(long)bOnOff) == 0)
        ;
}

BOOL FAR PASCAL
EnableWindowProc(HWND hWnd, LONG OnOff)
{
    //if ((hWnd != hWndHelpText) AND (hWnd != hWndTimer))
   if (hWnd == hWndWS)
   {
      if ((BOOL)OnOff)
      {
         THAW_HWND_WS
      }
      else
      {
         FREEZE_HWND_WS
      }
   }
   else
      EnableWindow(hWnd,(BOOL)OnOff);
#ifdef INTERNAL
    //AuxPrint("Enabling %x, %s",hWnd,OnOff ? "TRUE" : "FALSE");
#endif
    return 1;
}


BOOL isWashington (char *state)
{
    char st[MAXLEN_STATE+1];

    /* convert to upper case */
    lstrcpy(st,state);
    stoupper(st);

    if (lstrcmp(st,"WA") == 0) return(TRUE);

    /* some old duffers still use WN as abbreviation... */
    if (lstrcmp(st,"WN") == 0) return(TRUE);
    return(FALSE);
}


char * FAR PASCAL stoupper(char *str)
{
    char *save;

    save = str;
    while (*str != '\0')
    {
        if (('a' <= *str) && (*str <= 'z'))
            *str = *str - 'a' + 'A';
        str++;
    }
    return(str);
}


double round_cents(double d)
{
    return( ((double) ((long) ((d + .005) * 100))) / 100);
}


