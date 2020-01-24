/*************************************************************\
 ** FILE:  ksearch.c
 **
 ** Copyright (c) Adonis Corporation. 1991
 **
 ** DESCRIPTION:
 **  Keyword search dialog box
 **
 ** HISTORY:   
 **  Lauren Bricker   4/11/91        Created
 **
\*************************************************************/

#include "nowin.h"
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include "ws.h"
#include "rc_symb.h"
#include "misc.h"
#include "file.h"
#include "error.h"
#include "help.h"
#include "ksearch.h"
#include "viewbox.h"
#include "busy.h"

extern BOOL bConnect;

// Local defines

#define MAX_STR_SEARCH  32
#define NUM_TYPE_CHARS  8
#define NUM_SRCH_CHARS  2
#define NUM_LIST_CHARS  2

#define IMAGE_TYPE_LEN  8
#define LIBID_LEN       3
#define SUBLIB8_LEN     5
#define SUBLIB2_LEN     5

#define LIB_HEADER      1
#define SUBLIB_HEADER   2

#define WM_SEARCHCANCEL_LOADING    (WM_USER + 10)
#define WM_SEARCHCANCEL_SEARCHING  (WM_USER + 11)

// Local Macros

#define DESTROY_DLDWND   if ( IsWindow(hDldWnd) )      \
                        {                    \
                            DestroyWindow (hDldWnd);  \
                            FreeProcInstance (lpprocStop); \
                        }

#define USERCANCEL      FreeProcInstance (lpprocStop); \
                        SetFocus (hWndSearch);

// Type declarations...

typedef struct {
#if 0
    BOOL    color;
#endif
    BOOL    download;
    char    keyword[MAX_STR_SEARCH + 1];
    char    displayKey[MAX_STR_SEARCH + 1];
    BOOL    allFormats;
    DWORD   formats;
    char    indexPath[_MAX_PATH + 1];
    char    searchPath[_MAX_PATH + 1];
    HANDLE  searchIndex;
    HANDLE  searchFile;
    FILE*   searchFileP;
    fpos_t  searchFileO;   /* for storing current offset in searchFile */
    BOOL    searchInProgress;    /* Are we in the middle of a search file? */
    BOOL    skipSubLib;   /* if we can't view current thumbnail */
} SEARCHSTRUCT;

// Global Variables

HWND    hWndSearch    = NULL;
static FARPROC lpprocStop    = NULL;
BOOL    bFixAfterFind = TRUE;

// Imported Variables

extern char newThumbFile[];
extern HWND hDldWnd;                // instead of all of download.h

// Local Variables

static SEARCHSTRUCT Search;
OFSTRUCT ofStruct;

static char sztmpCurPubFile[_MAX_PATH];
static char sztmpCurPub[MAX_STR_LEN];

static char szSearchMsg[MAX_MSG_LEN];
static BOOL bSearchCancelOpened = FALSE;

char   szSearchIndex[]  = "00000006";
char   szSearchPrefix[] = "000006";
char   szPubPrefix[]    = "100000";
char   szLibPrefix[]    = "00004";
char   szInfoPrefix[]   = "008";
char   szThumbPrefix[]  = "002";

#define INFO_PREFIX_LEN  3
#define THUMB_PREFIX_LEN  3

/*
   THESE MUST BE IN THE SAME ORDER AS THE BITS THAT ARE SET FOR FORMATS
   see ksearch.h
*/
static char*        FormatTypes[NUM_FORMATS] = { "EPS",
                                                 "PCX",
                                                 "TIF",
                                                 "CGM",
                                                 "TBK",
                                                 "CDR",
                                                 "DRW",
                                                 "AI",
                                                 "WPG",
                                                 "GEM",
                                                 "GED"  };

// Local functions
static BOOL SearchPubFile (void);
static BOOL FindMatch (BOOL first);
static BOOL QueryPubName (char *szPubInfoFileName, char *szPubName);
static int QuerySublibNumber (char* libFile, char *sublib2File,
                               char *sublib8File, int *whichSublib);

/*************************************************************\
 ** StartSearch
 **
 ** ARGUMENTS:   HWND    hWnd  - name of parent window
 **
 ** DESCRIPTION: Create the keyword search dialog
 **
 ** ASSUMES:
 **
 ** MODIFIES:  Search structure
 **
 ** RETURNS:   TRUE if successful, FALSE if not.
 **
 ** HISTORY:   Lauren Bricker  April 11, 1991 (written)
\*************************************************************/
BOOL
StartSearch (HWND hWnd)
{
    if (IsWindow(hWndSearch))
       SetFocus(hWndSearch);
    else
    {
       WORD wTmp;
       FILE*  searchIndexP;

       //for slow machines
       HCURSOR hCursor = SetCursor (LoadCursor (NULL, IDC_WAIT));

       //Load the control string for the search cancel dlg
       LoadString (hInstanceWS, IDS_MSG_SEARCHING_FOR,
                   (LPSTR) szSearchMsg, MAX_MSG_LEN);

       //set static dlg proc for search cancel dlg
       lpprocStop = MakeProcInstance ((FARPROC) WSStopSearchDlgProc, hInstanceWS);

       /* Check that the SearchIndex file exists */
       strcpy (Search.indexPath, szSearchIndex);
       AddFullPath(Search.indexPath);
       if (LoadFile(Search.indexPath) != OK)
       {
           ErrMsg (IDS_MSG_MISSING_SRCH_FILE);
           return FALSE;
       }

       /* Open and read the SearchIndex File for the list of Publisher search
          files; see that all of these files are available and, if logged on,
          up-to-date ( by a call to LoadFile() ).

          If any are missing we abort with several messages about a missing
          file and the need to logon to obtain it.  If we are logged on and the
          file is missing or out of date LoadFile() will download a new copy.
       */

       if ( (Search.searchIndex = OpenFile ((LPSTR)Search.indexPath,
                                     (LPOFSTRUCT)&ofStruct,
                                     OF_READ)) == -1 )
       {
           ErrMsg (IDS_MSG_MISSING_SRCH_FILE);
           return ( FALSE );
       }

       //setup for buffered input
       if ((searchIndexP = fdopen(Search.searchIndex, "rb")) == NULL)
       {
           ErrMsg (IDS_MSG_MISSING_SRCH_FILE);
           return FALSE;
       }

       while (fscanf(searchIndexP, "%*08lX\t%02X\r\n", &wTmp) == 1 ) // Eating the image type
       {
          // Get the pub's search file name and see if it's around
          wsprintf((LPSTR)Search.searchPath, (LPSTR)"%s%02X",
                   (LPSTR)szSearchPrefix, wTmp);
          AddFullPath(Search.searchPath);
          if ( LoadFile(Search.searchPath) != OK )
          {
              _lclose (Search.searchIndex);
              Search.searchIndex = -1;
              ErrMsg (IDS_MSG_MISSING_SRCH_FILE);
              return FALSE;
          }
       }
       _lclose (Search.searchIndex);
       Search.searchIndex = -1;

       // Initialization...
       //Search.color         = FALSE;
       Search.download      = (custinfo.searchParam & IT_DOWNLOAD) ? TRUE : FALSE;
       Search.formats       = custinfo.searchParam & ALL_TYPES;
       Search.allFormats    = (Search.formats == ALL_TYPES) ? TRUE : FALSE;
       Search.keyword[0]    = '\0';
       Search.displayKey[0] = '\0';
       Search.searchIndex   = -1;
       Search.searchFile    = -1;
       Search.searchFileP    = NULL;
       Search.searchInProgress = FALSE;
       Search.skipSubLib = FALSE;

       CreateDialog(hInstanceWS, (LPSTR)"SEARCH", hWnd, lpprocSearchDlg);
       SetCursor (hCursor);
    }

    return TRUE;
}

/*************************************************************\
 ** WSSearchDlgProc
 **
 ** ARGUMENTS:      HWND        hWndDlg
 **                 unsigned    message
 **                 WORD        wParam
 **                 LONG        lParam
 **
 ** DESCRIPTION:    Keyword search modeless dialog proc
 **
 ** ASSUMES:
 **
 ** MODIFIES:  Search structure
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Lauren Bricker  April 11, 1991 (written)
\*************************************************************/

LONG FAR PASCAL 
WSSearchDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    switch ( message )
    {
        case WM_INITDIALOG:
        {
            RECT    rWindow;
            int     xScreen = 0, yScreen = 0;
            int     height, width;
            POINT   start;
            WORD    ii;

            // pop the window up at the bottom of the screen
            GetWindowRect(hWndDlg, &rWindow);
            height  = rWindow.bottom - rWindow.top;
            width   = rWindow.right  - rWindow.left;
            xScreen = GetSystemMetrics (SM_CXSCREEN);
            yScreen = GetSystemMetrics (SM_CYSCREEN);
            start.x = (xScreen - width) >> 1;
            start.y = (yScreen - height);
            MoveWindow(hWndDlg, start.x, start.y, width, height, FALSE);

            // Load the format types into the listbox
            for ( ii = 0; ii < NUM_FORMATS; ii++ )
                SendDlgItemMessage (hWndDlg, ID_SELFORMATS, LB_ADDSTRING, 0,
                                    (LONG) (LPSTR) FormatTypes[ii]);

#if 0   // NOT CURRENTLY IMPLEMENTED - 4/17/91
            SendDlgItemMessage (hWndDlg, IDB_COLOR, BM_SETCHECK,
                                Search.color, 0L);
#endif
            SendDlgItemMessage (hWndDlg, IDB_DOWNLOAD, BM_SETCHECK,
                                Search.download, 0L);
            SendDlgItemMessage (hWndDlg, ID_KEYWORD, EM_LIMITTEXT,
                                MAX_STR_SEARCH, 0L);

            CheckRadioButton   (hWndDlg, IDB_ALL, IDB_SELECTED,
                                 Search.allFormats ? IDB_ALL : IDB_SELECTED);

            EnableWindow (GetDlgItem (hWndDlg, ID_SELFORMATS), !Search.allFormats);
            // Pre-select the user's previous format selections
            if (!Search.allFormats)
            {
               for ( ii = 0; ii < NUM_FORMATS; ii++ )
                   if ( Search.formats & (1 << (ii + FORMAT_SHIFT)))
                      SendDlgItemMessage (hWndDlg, ID_SELFORMATS,
                                          LB_SETSEL, 1, (LONG)ii);
            }

            EnableWindow (GetDlgItem (hWndDlg, IDB_FIND), FALSE);
            EnableWindow (GetDlgItem (hWndDlg, IDB_FINDNEXT), FALSE);

            hWndSearch = hWndDlg;
            hMommaWnd = hWndSearch;
            FREEZE_HWND_WS //Disable main window and inc disable count
            ShowWindow (hWndDlg, SW_SHOW);

            break;
        }

        case WM_COMMAND:
        {
            switch ( wParam )
            {
                case ID_SELFORMATS:
                {
                    if (HIWORD(lParam) == LBN_SELCHANGE)
                    {
                        // if a search param has changed, disable the find next
                        // button. Ideally we would just keep searching forward
                        // if the search parameters had narrowed and start over
                        // if the search has broadened, but for now we'll just
                        // make the user start over
                        EnableWindow (GetDlgItem (hWndDlg, IDB_FINDNEXT), FALSE);
                        SendDlgItemMessage (hWndDlg, IDB_FIND, BM_SETSTYLE, (WORD)BS_DEFPUSHBUTTON, 1L);
                        Search.searchInProgress = FALSE;
                     }
                     return (FALSE);
                     break;
                }

                case ID_KEYWORD:

                    if ( HIWORD(lParam) == EN_CHANGE )
                    {
                        char FindWord[MAX_STR_SEARCH + 1];

                        // if the keyword has changed, disable the find next button
                        EnableWindow (GetDlgItem (hWndDlg, IDB_FINDNEXT), FALSE);

                        // if EDIT are missing, disable the find button
                        GetDlgItemText (hWndDlg, ID_KEYWORD, (LPSTR)FindWord, MAX_STR_SEARCH);
                        EnableWindow (GetDlgItem (hWndDlg, IDB_FIND), FindWord[0]);
                        SendDlgItemMessage (hWndDlg, IDB_FIND, BM_SETSTYLE, (WORD)BS_DEFPUSHBUTTON, 1L);
                        Search.searchInProgress = FALSE;
                        return FALSE;
                    }
                    break;

                case IDB_DOWNLOAD:
                    Search.download = !Search.download;

                    // if a search param has changed, disable the find next
                    // button. Ideally we would just keep searching forward
                    // if the search parameters had narrowed and start over
                    // if the search has broadened, but for now we'll just
                    // make the user start over
                    EnableWindow (GetDlgItem (hWndDlg, IDB_FINDNEXT), FALSE);
                    SendDlgItemMessage (hWndDlg, IDB_FIND, BM_SETSTYLE, (WORD)BS_DEFPUSHBUTTON, 1L);
                    Search.searchInProgress = FALSE;

                    SendDlgItemMessage (hWndDlg, IDB_DOWNLOAD, BM_SETCHECK,
                                        Search.download, 0L);
                    break;

#if 0   // NOT CURRENTLY IMPLEMENTED - 4/17/91

                case IDB_COLOR:
                    Search.color = !Search.color;
                    SendDlgItemMessage (hWndDlg, IDB_COLOR, BM_SETCHECK,
                                        Search.color, 0L);
                    break;
#endif

                case IDB_ALL:
                case IDB_SELECTED:
                {
                    EnableWindow (GetDlgItem (hWndDlg, ID_SELFORMATS), TRUE);

                    // !!!!! SNEAKY - ASSUMES IDB_SELECTED = IDB_ALL + 1 !!!!

                    Search.allFormats = !(wParam - IDB_ALL);

                    if (Search.allFormats)   // Unselect all formats
                    {
                       SendDlgItemMessage (hWndDlg, ID_SELFORMATS, LB_SETSEL, 0, -1L);
                       Search.formats = ALL_TYPES;
                    }
                    else   //trigger setting of formats when user presses FIND
                       Search.formats = 0;


                    CheckRadioButton   (hWndDlg, IDB_ALL, IDB_SELECTED, wParam);
                    EnableWindow (GetDlgItem (hWndDlg, ID_SELFORMATS), !Search.allFormats);

                    // if a search param has changed, disable the find next
                    // button. Ideally we would just keep searching forward
                    // if the search parameters had narrowed and start over
                    // if the search has broadened, but for now we'll just
                    // make the user start over
                    EnableWindow (GetDlgItem (hWndDlg, IDB_FINDNEXT), FALSE);
                    SendDlgItemMessage (hWndDlg, IDB_FIND, BM_SETSTYLE, (WORD)BS_DEFPUSHBUTTON, 1L);
                    Search.searchInProgress = FALSE;
                    break;
                }

                case IDB_FIND:
                {
                    // start the search from the beginning
                    GetDlgItemText (hWndDlg, ID_KEYWORD, (LPSTR)Search.displayKey,
                                    MAX_STR_SEARCH);
                    strcpy (Search.keyword, Search.displayKey);
                    strupr (Search.keyword);

                    if ((NOT Search.allFormats) AND
                       (custinfo.searchParam) AND
                       (custinfo.searchParam != Search.formats)) //haven't changed
                    {
                        WORD  ii;
                        DWORD dwSelect;

                        /* Get the format types from the listbox */
                        /* and combine into the formats bitfield */
                        Search.formats = 0;
                        for ( ii = 0; ii < NUM_FORMATS; ii++ )
                        {
                            dwSelect = SendDlgItemMessage (hWndDlg,
                                                        ID_SELFORMATS,
                                                        LB_GETSEL, ii, 0L);
                            if ( dwSelect )
                                Search.formats |= (1 << (ii + FORMAT_SHIFT));
                        }
                        if (NOT Search.formats) //must select format(s)!!!
                        {
                             SetDest (hWndDlg, NOBEEP,
                                      MB_ICONEXCLAMATION | MB_OK,
                                      IDS_TITLE_SEARCH);
                             ErrMsg(IDS_ERR_SELECTFORMAT);
                             break;
                        }
                    }

                    //Record new search parameters in custinfo, win.ini
                    if (custinfo.searchParam !=
                        (Search.formats + Search.download))
                    {
                       custinfo.searchParam = Search.formats + Search.download;
                       WriteCustInfo(WRITE_SEARCHPARAM);
                    }

                    EnableWindow(GetDlgItem(hWndDlg, IDB_FINDNEXT),  FindMatch(TRUE));
                    break;
                }

                case IDB_FINDNEXT:
                    // continue the search from the current position
                    EnableWindow (GetDlgItem (hWndDlg, IDB_FINDNEXT), FindMatch (FALSE));
                    break;

                case IDCANCEL:
                    goto END_DIALOG;

                case ID_HELP:
                    GetHelp (GH_TEXT, IDH_SEARCH);
                    break;

                default:
                    return (FALSE);
                    break;
            }
            break;

            default:
               return (FALSE);
        }

END_DIALOG:
        case WM_CLOSE:
        {
           // Close up any open files....
           if ( Search.searchIndex != -1 )
           {
               _lclose (Search.searchIndex);
               Search.searchIndex = -1;
           }

           if ( Search.searchFile != -1 )
           {
               fclose (Search.searchFileP);
               //_lclose (Search.searchFile);
               Search.searchFile = -1;
               Search.searchFileP = NULL;
           }

           THAW_HWND_WS  //dec disable count and enable if no freeze count

           hWndSearch = NULL;
           DestroyWindow (hWndDlg);

           break;
        }

    } /* end switch message */

    return (TRUE);
}

/*************************************************************\
 ** SearchPubFile
 **
 ** ARGUMENTS:   NONE
 **
 ** DESCRIPTION: If there is no search file open, then open one.
 **              Then search the open file to see if there is a 
 **              match to the user's keyword. If there is, bring it up 
 **              in the viewbox.
 **
 ** ASSUMES:    Search.searchPath contains the full path of
 **                 file to search.
 **             The index file pointer is currently pointing to 
 **                 the publisher's filename field.
 **
 ** MODIFIES:   Search.searchIndex - increments location in the file
 **             Search.searchFile - opens a file if not already and/or
 **                                 increments the location in the file
 **             CurLib - if there is a match found (see ws.h)
 **
 ** RETURNS:    TRUE if a match was found and the viewbox was 
 **               successfully brought up, FALSE if not.
 **
 ** HISTORY:    Lauren Bricker  April 18, 1991 (written)
\*************************************************************/

static BOOL
SearchPubFile (void)
{
    BOOL        foundOne = FALSE;
    char        line[MAX_MSG_LEN + 1];
    long        imageType = 0;
    static int  whichSublib = 0;
    BOOL        skipToNextLib = FALSE;
    static int  imageNumber = 0;
    static char libFile[MAX_BASE_LEN + 1];
    static char sublib2File[MAX_BASE_LEN + 1];
    static char sublib8File[MAX_BASE_LEN + 1];
    int         is_msg;
    MSG         msg;
    BOOL nRetval = FALSE;

    // (Re)Open search file, and (re)set position
    if ( Search.searchFile == -1 )
    {
       //Get file handle
       if ( (Search.searchFile = OpenFile ((LPSTR)Search.searchPath,
                                     (LPOFSTRUCT)&ofStruct,
                                     OF_READ)) == -1 )
       {
           ErrMsg (IDS_MSG_MISSING_SRCH_FILE);
           END( FALSE )
       }

       //setup for buffered input
       if ((Search.searchFileP = fdopen(Search.searchFile, "rb")) == NULL)
       {
           ErrMsg (IDS_MSG_MISSING_SRCH_FILE);
           return FALSE;
       }
    }

    if (Search.searchInProgress)
    {
       //set file pointer position
       if ( fsetpos(Search.searchFileP, &Search.searchFileO) != 0)
       {
           ErrMsg(IDS_FILE_IO_ERR, ID_ERR_IN_SEARCH_FILE,
                    (LPSTR) StripPath(Search.searchPath));
           END ( FALSE )
       }
    }
    else   //reset  static var's
    {
       whichSublib = 0;
       imageNumber = 0;
       libFile[0] = '\0';
       sublib2File[0] = '\0';
       sublib8File[0] = '\0';
    }

    while ( !foundOne )
    {
        //Only peek at hDldWnd and hWndTimer Msg's once in a while for
         // performnace boost (÷15% w/out these peek loops)
        if (NOT imageNumber)
        {
           /** this is mainly for TIMER, but will also enable user to go
               to another app.  Not really, only lets another app get focus.
               This to intercept terminate msg's during long processes */

           while ( IsWindow(hDldWnd) )
           {
               is_msg = PeekMessage (&msg, hDldWnd, NULL, NULL, PM_REMOVE);

               if ( !is_msg )
                   break;

               if ( IsWindow (hDldWnd) )
                   if (IsDialogMessage (hDldWnd, &msg) )
                       continue;
           }

           while ( hWndTimer )
           {
               is_msg = PeekMessage( &msg, hWndTimer, NULL, NULL, PM_REMOVE);

               if ( !is_msg ) break;

               if ( hWndTimer && IsWindow (hWndTimer) )
                   if ( IsDialogMessage (hWndTimer, &msg) )
                       continue;
           }
        }

        if ( bSearchCancelOpened AND NOT IsWindow(hDldWnd) )
        {
            USERCANCEL
            END( TRUE )    //Enables user to continue search
        }

        //Not checking for EOF condition (using feof) since search files should
        //not contain the EOF character
        if ( (line[0] = (char)fgetc(Search.searchFileP)) == EOF)
        {
            if (ferror(Search.searchFileP))
            {
                ErrMsg(IDS_FILE_IO_ERR, ID_ERR_IN_SEARCH_FILE,
                         (LPSTR) StripPath(Search.searchPath));
                END ( FALSE )
            }
            else
               break;   //break read file loop
        }

        if ( line[0] == LIB_HEADER )
        {
            if ( skipToNextLib )        // start processing again...
                skipToNextLib = FALSE;

            if (fscanf(Search.searchFileP, "\t%08lX", &imageType) != 1)
            {
                ErrMsg(IDS_FILE_IO_ERR, ID_ERR_IN_SEARCH_FILE,
                         (LPSTR) StripPath(Search.searchPath));
                END ( FALSE )
            }

            // Go on to the next library if the user is only looking for 
            //  downloadable images but this library is not
            if ( Search.download && !(imageType & IT_DOWNLOAD) )
            {
                skipToNextLib = TRUE;
                fgets(line, MAX_STR_LEN, Search.searchFileP); //eat rest-o-line
                continue;
            }

            // Go on to the next library if this one has none of the formats
            // the user is looking for...
            if ( !(imageType & Search.formats) )
            {
                skipToNextLib = TRUE;
                fgets(line, MAX_STR_LEN, Search.searchFileP); //eat rest-o-line
                continue;
            }

            // Get the libfile name and null terminate
            strcpy (libFile, szLibPrefix);
            if (fscanf(Search.searchFileP, "\t%3[0-9A-F]\n",
                     libFile + strlen(libFile)) != 1)
            {
                ErrMsg(IDS_FILE_IO_ERR, ID_ERR_IN_SEARCH_FILE,
                        (LPSTR) StripPath(Search.searchPath));
                END ( FALSE )
            }
        }

        else
        {
            if ( skipToNextLib )  // don't do any more processing on this line
            {
                fgets(line, MAX_STR_LEN, Search.searchFileP);  //eat restoline
                continue;
            }

            if ( line[0] == SUBLIB_HEADER )
            {
                // Create the "Searching Window", we do this here so that if
                // we are resuming searching in the middle of a sublib and
                //we find another image in this sublib we don't waste time
                //putting up the SEARCHCANCEL window, we just go right to the image
                if (NOT IsWindow(hDldWnd))
                {
                   lpprocStop = MakeProcInstance ((FARPROC) WSStopSearchDlgProc, hInstanceWS);
                   if (CreateDialog (hInstanceWS, "SEARCHCANCEL", hWndSearch, lpprocStop) == NULL)
                      END (FALSE)
                   bSearchCancelOpened = TRUE;
                   //load desc text with new Pub and keyword
                   SendMessage(hDldWnd, WM_SEARCHCANCEL_SEARCHING, 0, 0L);
                }

                imageNumber = 0;
                Search.skipSubLib = FALSE;

                // Get the sublib8 filename and null terminate
                strcpy (sublib8File, szInfoPrefix);
                strcpy (sublib2File, szThumbPrefix);
                if (fscanf(Search.searchFileP, "\t%5[0-9A-F]\t%5[0-9A-F]\n",
                     sublib8File + INFO_PREFIX_LEN,
                     sublib2File + THUMB_PREFIX_LEN) != 2)
                {
                     ErrMsg(IDS_FILE_IO_ERR, ID_ERR_IN_SEARCH_FILE,
                             (LPSTR) StripPath(Search.searchPath));
                     END ( FALSE )
                }
            }
            else
            {
                if (Search.skipSubLib)  // don't do any more processing on this sublib
                {
                    fgets(line, MAX_STR_LEN, Search.searchFileP);  //eat restoline
                    continue;
                }

                if (fgets(line + 1, MAX_STR_LEN, Search.searchFileP) == NULL)
                {
                   if (feof(Search.searchFileP))
                        break;
                   else
                   {
                       ErrMsg(IDS_FILE_IO_ERR, ID_ERR_IN_SEARCH_FILE,
                               (LPSTR) StripPath(Search.searchPath));
                       END ( FALSE )
                   }
                }

                imageNumber++;
                // Here's where checking would be done for price match...
                //int lineLen = strlen (line);

                // Chop of the price code then search the image 
                //     description line for a match
                //line[--lineLen] = '\0';

                //Now search the line for a hit
                //Note: Lines are all uppercase in search files
                if ( strstr (line, Search.keyword) )
                    foundOne = TRUE;
            }
        }
    }
#ifdef INTERNAL
    AuxPrint("Ending Search of %s, %s finding match", sztmpCurPub, (foundOne ? "after" : "without"));
#endif

    if ( foundOne )
    {
       //Since there is a delay in loading the image let's let the user know
       // we are now loading an image instead of seraching for one
       if (IsWindow(hDldWnd))
           SendMessage(hDldWnd, WM_SEARCHCANCEL_LOADING, 0, 0L);

       //get position in file to reset to when Find Next is called
       if ( fgetpos(Search.searchFileP, &Search.searchFileO) != 0)
       {
           ErrMsg(IDS_FILE_IO_ERR, ID_ERR_IN_SEARCH_FILE,
                    (LPSTR) StripPath(Search.searchPath));
           END ( FALSE )
       }
       Search.searchInProgress = TRUE;
    }

    //close the search file
    if ( Search.searchFile != -1 )
    {
        fclose (Search.searchFileP);
        Search.searchFile = -1;
        Search.searchFileP = NULL;
    }

    if ( foundOne )
    {
       // Bring up the viewbox with the right library scrolled to the image.
       // To goto the right sublib we have to open the lib info file and find
       // which index position that sublib is in since the numbers assigned to
       // a sublib in a library are not garunteed to be in order (as they are
       // in the search file).

       switch (QuerySublibNumber (libFile, sublib2File,
                              sublib8File, &whichSublib))
       {
           case IDB_CONTINUE_SEARCH:
           {
               //can't get thumbs or libfile so skip this image
               Search.skipSubLib = TRUE; // don't look any more in this sublib
               //Do 'Find Next'
               PostMessage ( hWndSearch, WM_COMMAND, IDB_FINDNEXT, 0L);
               END ( TRUE )
           }
           case IDCANCEL:
           {
               //can't get thumbs or libfile so skip this image
               Search.skipSubLib = TRUE; // don't look any more in this sublib
               //Return focus to 'Find Next' button
               PostMessage ( hWndSearch, WM_NEXTDLGCTL,
                             GetDlgItem ( hWndSearch, IDB_FINDNEXT), 1L);
               END ( TRUE )
           }
           default:
               break;
       }

       strcpy (newThumbFile, sublib2File);  //viewbox.c variable
       //only set CURPUB when we've got a match and we're going into viewbox
       strcpy (CURPUB, sztmpCurPub);
       strcpy (CURPUBFILE, sztmpCurPubFile);

#ifdef INTERNAL
       //AuxPrint("Line containing match: %s", line);
       //AuxPrint("Match is image number: %d", imageNumber);
       AuxPrint("Image found in Publisher %s", CURPUB);
       //AuxPrint("Image found in library %s", CURLIB);
       AuxPrint("Image found in library file %s", CURLIBFILE);
       //AuxPrint("Image found in sub lib #%d", whichSublib);
       AuxPrint("Image found in sublib %s", CURSUBLIB);
       AuxPrint("Image found in thumbinfo %s", sublib8File);
       AuxPrint("Image found in thumbnail %s", sublib2File);
#endif

       CreateViewBox (whichSublib, imageNumber);

       bFixAfterFind = TRUE;    //flag to reset Main list boxes
       // After the viewbox is created return focus to the search dialog (Find Next)
       // to ease further searching
       DESTROY_DLDWND
       PostMessage ( hWndSearch, WM_NEXTDLGCTL,
                     GetDlgItem ( hWndSearch, IDB_FINDNEXT), 1L);
       END( TRUE )
    }
    else  //if not found one
    {
       Search.searchInProgress = FALSE;
       END ( FALSE )
    }

end:
    BusyClear(TRUE);
    return (nRetval);
}

/*************************************************************\
 ** FindMatch
 **
 ** ARGUMENTS:      BOOL    first - whether or not to find the first occurence
 **
 ** DESCRIPTION:    Find an instance of the search keyword
 **                 in the search files.  
 **
 ** ASSUMES:        Search structure is filled in
 **
 ** MODIFIES:
 **
 ** RETURNS:        TRUE if successful (found a match), FALSE if not.
 **
 ** HISTORY:        Lauren Bricker  April 15, 1991 (written)
\*************************************************************/

static BOOL
FindMatch (BOOL first)
{
    BOOL        foundOne = FALSE;
    static char searchFile[NUM_SRCH_CHARS + 1];
    char        szImageType[NUM_TYPE_CHARS + 1];
    long        imageType = 0;

    bSearchCancelOpened = FALSE;
    /* Open the search index file if not already */
    if ( Search.searchIndex == -1 )
    {
        Search.searchIndex = _lopen (Search.indexPath, READ);
#ifdef INTERNAL
        AuxPrint("Opend search index, file handle = %d", Search.searchIndex);
#endif
        if ( Search.searchIndex == -1 )
        {
            DESTROY_DLDWND
            ErrMsg (IDS_MSG_MISSING_SRCH_FILE);
            return FALSE;
        }
    }
    else    // Search index already opened
    {
        if ( first )
        {
            lseek (Search.searchIndex, 0, SEEK_SET);
            Search.searchInProgress = FALSE;
        }
        else
        {
            // Finish searching this search file if we're currently in one
            if ( Search.searchInProgress )
            {
                if ( SearchPubFile() )
                {
                    DESTROY_DLDWND
                    return TRUE;
                }
            }
        }
    }

    /* Keep checking all of the search files until the first match is found */
    while ( !foundOne )
    {
        if ( bSearchCancelOpened AND NOT IsWindow(hDldWnd) )
        {
            USERCANCEL
            //Enables user to continue search is already found one, reset if not
            return (Search.searchInProgress ? TRUE : FALSE);
        }

#ifdef INTERNAL
        AuxPrint("RS from FindMatch(), 1");
#endif
        if ( ReadStringToChar(Search.searchIndex, szImageType, '\t',
               sizeof(szImageType)) != OK )
        {
#ifdef INTERNAL
            AuxPrint("Search.searchIndex file handle = %d", Search.searchIndex);
#endif
            break;
        }
        imageType = strtol (szImageType, NULL, 16);
#ifdef INTERNAL
        if (NOT imageType)
            AuxPrint("Bad imagetype in search index");
#endif

        // Go on to the next publisher file if the user is only looking for
        // downloadable images but this publisher has none
        // OR
        // Go on to the next publisher file if this one has none of the formats
        // the user is looking for
        if ((Search.download AND
            (NOT (imageType & IT_DOWNLOAD))) OR (NOT (imageType & Search.formats)))
        {
            ReadToChar(Search.searchIndex, '\n');  // eat rest of line, begin again on next
            continue;
        }

        // Get the search and pub file's name and start the search
#ifdef INTERNAL
        AuxPrint("RS from FindMatch(), 2");
#endif
        if ( ReadStringToChar(Search.searchIndex, searchFile, '\n', sizeof(searchFile)) != OK )
            break;

        strcpy (sztmpCurPubFile, szPubPrefix);
        strcat (sztmpCurPubFile, searchFile);
        AddFullPath(sztmpCurPubFile);
        // If the current publisher search file is not available, abort
        if ( LoadFile (sztmpCurPubFile) == FAIL )
        {
            DESTROY_DLDWND
            _lclose (Search.searchIndex);
            Search.searchIndex = -1;
            return FALSE;
        }

        if ( !QueryPubName (sztmpCurPubFile, sztmpCurPub) )
            break;

        //Now load the publisher search file
        strcpy (Search.searchPath, szSearchPrefix);
        strcat (Search.searchPath, searchFile);
        AddFullPath(Search.searchPath);
        if ( LoadFile(Search.searchPath) == FAIL )
        {
            DESTROY_DLDWND
            _lclose (Search.searchIndex);
            Search.searchIndex = -1;
            ErrMsg (IDS_MSG_MISSING_SRCH_FILE);
            return FALSE;
        }
        else
        {
            // Check this publisher's search file for a match.
            // If a match is found, then we have found the first, 
            // otherwise we'll just keep searching....
            if (IsWindow(hDldWnd))
               SendMessage(hDldWnd, WM_SEARCHCANCEL_SEARCHING, 0, 0L);
            if ( SearchPubFile() )
                foundOne = TRUE;
        }
    }

    DESTROY_DLDWND
    if ( !foundOne )
    {
        if ( first )
            ErrMsg (IDS_MSG_NO_MATCH, (LPSTR)Search.displayKey);
        else
            ErrMsg (IDS_MSG_NO_MORE_MATCHES, (LPSTR)Search.displayKey);
        return FALSE;
    }
    return TRUE;
}

/*************************************************************\
 ** QueryPubName
 **
 ** ARGUMENTS:  char *szPubInfoFileName - full path nameof the pub file
 **             char *szPubName         - buffer to fill with pub name
 **
 ** DESCRIPTION: Fill inthe publisher's name
 **
 ** ASSUMES:    there is a buffer to put the publishers name into
 **
 ** MODIFIES:   szPubName
 **
 ** RETURNS:   TRUE if successful, FALSE if not.
 **
 ** HISTORY:   Lauren Bricker  April 29, 1991 (written)
\*************************************************************/
static BOOL 
QueryPubName (char *szPubInfoFileName, char *szPubName)
{
	 int	fp = -1;
    int nRetval = TRUE;

    /* open file */
    if ((fp = _lopen(szPubInfoFileName, READ)) == -1) 
    {
        ErrMsg(IDS_FILE_IO_ERR, ID_ERR_OPENING_FILE,
                 (LPSTR) StripPath(szPubInfoFileName));
        return FALSE;
    }

#ifdef INTERNAL
        AuxPrint("RS from QueryPubName(), 1");
#endif
    if (ReadStringToChar(fp, szPubName, '\t', MAX_STR_LEN) == FAIL)
    {
        ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                 (LPSTR) StripPath(szPubInfoFileName));
        nRetval = FALSE;
    }

    if ( fp != -1 )
        _lclose (fp);

    return nRetval;
}

/*************************************************************\
 ** WSStopSearchDlgProc
 **
 ** ARGUMENTS:      HWND     hWndDlg
 **                 unsigned message
 **                 WORD     wParam
 **                 LONG     lParam
 **
 ** DESCRIPTION:    Proc to handle canceling a search.
 **
 ** RETURNS:        TRUE if successful, FALSE if not.
 **
 ** HISTORY:        Lauren Bricker  May 10, 1991 (written)
\*************************************************************/
LONG FAR PASCAL 
WSStopSearchDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam) 
{
    static HCURSOR hCursor;
    REFERENCE_FORMAL_PARAM (lParam);

    switch ( message )
    {
        case WM_INITDIALOG:
            hCursor = SetCursor(hSearchCurs);
            SetCapture(GetDlgItem(hWndDlg, IDCANCEL));
            ShowWindow(GetDlgItem(hWndDlg, IDS_LOADING), SW_HIDE);
            hDldWnd = hWndDlg;
            CenterPopUp (hWndDlg);
            break;

        case WM_COMMAND:
            switch ( wParam )
            {
                case IDCANCEL:
                    SetDest (hWndDlg, NOBEEP, 
                             MB_ICONQUESTION | MB_YESNO, IDS_TITLE_SEARCH);
                    if ( ErrMsg (IDS_MSG_STOP_SEARCH) == IDYES )
                        DestroyWindow (hWndDlg);
                    else
                    {
                        SetCursor(hSearchCurs);
                        SetCapture(GetDlgItem(hWndDlg, IDCANCEL));
                        SetFocus (hWndDlg);
                    }
                    break;
            }
            break;

        case WM_DESTROY:
            hDldWnd = NULL;
            if (IsWindow(hWndSearch))
              SetFocus (hWndSearch);
            SetCursor(hCursor);
            ReleaseCapture();
            return FALSE;

        case WM_SEARCHCANCEL_LOADING:
            //let the user know we are now loading an image instead of searching
            EnableWindow(GetDlgItem(hWndDlg, IDCANCEL), FALSE);
            EnableWindow(GetDlgItem(hWndDlg, IDS_DESCRIPTION), FALSE);
            ShowWindow(GetDlgItem(hWndDlg, IDS_LOADING), SW_SHOW);
            UpdateWindow(hWndDlg);
            break;

        // let the user know what pub we are searching and what keyword(s)
        // we are searching for
        case WM_SEARCHCANCEL_SEARCHING:
        {
            char  szStopText[MAX_MSG_LEN + MAX_STR_LEN];

            wsprintf ((LPSTR)szStopText, (LPSTR)szSearchMsg,
                      (LPSTR)Search.displayKey, (LPSTR)sztmpCurPub);
            SetDlgItemText (hWndDlg, IDS_DESCRIPTION, (LPSTR)szStopText);
            break;
        }

        default:
            return (FALSE);
            break;
    } /* end switch message */

    return (TRUE);
}


/*************************************************************\
 ** QuerySublibNumber
 **
 ** ARGUMENTS:      char *libFile - base name of Lib (0004 file)
 **                 char *sublib2File - base name of 002 file
 **                 char *sublib8File - base name of 008 file
 **                 int  *whichSublib - which sublib this is in (0 based index)
 **
 ** DESCRIPTION:    Find out which sublib the found image is in
 **
 ** ASSUMES:
 **
 ** MODIFIES:   CURLIBFILE, CURLIB, CURSUBLIB, CURLIBNUMIMAGES, CURLIBDLDABLE
 **
 ** RETURNS:   TRUE if successful, FALSE if not.
 **
 ** HISTORY:   Lauren Bricker  April 29, 1991 (written)
\*************************************************************/
static int
QuerySublibNumber (char* libFile, char *sublib2File,
                   char *sublib8File, int *whichSublib)
{
    int  fh = -1;
    FILE* fp = NULL;
    int  nRetval = TRUE;
    static BOOL btmpDownload = FALSE;
    static char sztmpNumImages[6] = "\0";
    static char szLibPath[_MAX_PATH] = "\0";
    static char sztmpLib[MAX_STR_LEN] = "\0";
    static char sublibName[MAX_STR_LEN] = "\0";
    static char szLastSubLib[MAX_BASE_LEN + 1] = "\0";
    char file2[MAX_BASE_LEN + 1];
    char file8[MAX_BASE_LEN + 1];
    char sztmpOldPub[MAX_STR_LEN + 1] = "\0";

    //Filter: don't parse lib file if same sublib file as before,
    //just (re)set CURLIB stuff
    if (*sublib8File AND (strcmp(szLastSubLib, sublib8File) == 0))
    {
         END(TRUE)
    }
    strcpy(szLastSubLib, sublib8File);

    /*
       First, check up on these sublib files via LoadFile()
       before we call the ViewBox since if we run into a problem there (i.e.
       an I/O error) the viewbox will settle into a pub/lib/sublib#/image#
       which may not jive with the global LIB STRUCT stuff we set here
    */
    {
        char szFullPath2[_MAX_PATH];
        char szFullPath8[_MAX_PATH];
        HCURSOR hTmpCursor;
        int nResult;

        strcpy(szFullPath2, sublib2File);
        strcpy(szFullPath8, sublib8File);
        //if LoadFile can't locate the thumb it will need to tell the user
        //which Pub's floppy to put in, so temporarily switch CURPUB
        strcpy(sztmpOldPub, CURPUB);
        strcpy(CURPUB, sztmpCurPub);

        nMissingThumbContext = SEARCH_MODE;
        if (((nResult = LoadFile(AddFullPath(szFullPath2))) != OK) OR
            ((nResult = LoadFile(AddFullPath(szFullPath8))) != OK))
        {
           switch (nResult)
           {
               case IDB_CONTINUE_SEARCH:
               {
                   END(IDB_CONTINUE_SEARCH)
               }
               default:
               case FAIL:  //cancel search
               {
                  END(IDCANCEL)
               }
           }
           END(FALSE)
        }
        nMissingThumbContext = DEFAULT_MODE;

        //kludge: somtimes a call to LoadFile will result in decompressing
        //or downloading a file, these operations change the cursor to an
        //hourglass, when we return here the cursor is again changed back to
        //the search cursor. That is confusing, so if the cursor is changed
        //to an hourglass lets keep it an hourglass for the rest of the message
        hTmpCursor = SetCursor(hSearchCurs);
        SetCursor(hTmpCursor);
    }

    strcpy(szLibPath, libFile);
    AddFullPath(szLibPath);
    // If the current lib file is not available, get it or abort
    if ( LoadFile (szLibPath) == FAIL )
    {
         END (FAIL)  //LoadFile() displays proper error message
    }

    /* open file */
    if ( (fh = OpenFile (szLibPath, (LPOFSTRUCT)&ofStruct, READ)) == -1 )
    {
        ErrMsg(IDS_FILE_IO_ERR, ID_ERR_OPENING_FILE,
                 (LPSTR) StripPath(szLibPath));
        END(FAIL)
    }

    //set up for buffered input
    if ((fp = fdopen(fh, "rb")) == NULL)
    {
        ErrMsg(IDS_FILE_IO_ERR, ID_ERR_OPENING_FILE,
                 (LPSTR) StripPath(szLibPath));
         END (FAIL)
    }

    //get lib info from header
    //EX: "Libname\tPubID\t[0|1](download switch)\tLib description\tNumber of images\n"
    if (fscanf(fp, "%[^\t]\t%*[^\t]\t%d\t%*[^\t]\t%[0-9]\r\n",
               sztmpLib,   /* library name */
               &btmpDownload, /* downloadable switch */
               sztmpNumImages  /* num of images */
              ) != 3)
    {
       ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                 (LPSTR) StripPath(szLibPath));
       END (FAIL)
    }

    // Now figure out which sublib the 002 and 008 correspond to
    *whichSublib = 0;
    while ((fscanf(fp, "%[^\t]\t%[^\t]\t%[^\r\n]\r\n", sublibName, file8, file2)) == 3)
    {
        // If the base name of the 008 and 002 files match
        // database is inconsistent about case of hex digits so using strupr()
        if (strcmp(strupr(file8), sublib8File) OR
            strcmp(strupr(file2), sublib2File))
        {
            (*whichSublib)++;
        }
        else
        {
            END(TRUE)
        }
    }

end:
    if ( fh != -1 )
        fclose (fp);

    strcpy(CURPUB, sztmpOldPub); //reset the CURPUB

    switch ( nRetval )
    {
        case TRUE:
        {
            strcpy(CURSUBLIB, sublibName);  // fill in more LIBRARY stuff
            strcpy(CURLIB, sztmpLib);
            strcpy(CURLIBFILE, szLibPath);
            strcpy(CURLIBNUMIMAGES, sztmpNumImages);
            CURLIBDLDABLE = (btmpDownload == 1) ? TRUE : FALSE;
            break;
        }
        default:
        {
            szLastSubLib[0] = '\0';
            break;
        }
    }
    return nRetval;
}
