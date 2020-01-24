/************************************************************************
 *
 *      Module:  fileopen.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/

/************************************************************************
 *                         FILEOPEN.C
 ************************************************************************/
#ifndef DLG_ERROR

#define DLG_CANCEL     IDCANCEL
#define DLG_OK         IDOK
#define DLG_ERROR      -1

#endif 


int FAR PASCAL FileOpenDialog( HWND    hwndParent,
                               LPSTR   lpszTitle,
                               LPSTR   lpszExtension,
                               WORD    wFlags,
                               LPSTR   lpszFileBuf,
                               WORD    wFileLen ) ;

#define DLGOPEN_MUSTEXIST	0x0001
#define DLGOPEN_NOSHOWSPEC	0x0002
#define DLGOPEN_SAVE		0x0004
#define DLGOPEN_OPEN		0x0008
#define DLGOPEN_SHOWDEFAULT	0x0020
#define DLGOPEN_NOBEEPS		0x0040
#define DLGOPEN_SEARCHPATH	0x0080
#define DLGOPEN_NOCHANGESPEC 0x0100

#define DLG_MISSINGFILE	DLG_ERROR

#define  IDCANCEL	2
#define  IDOK	1
#define  IDD_CURDIR	200
#define  IDD_FILES_LB	201
#define  IDD_DIR_LB	202
#define  IDD_FILESPEC_EC	203


/************************************************************************
 * End of File: fileopen.h
 ************************************************************************/

