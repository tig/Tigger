//  iniedit.h
//
//
//  INIedit
//
//  Windows 3.0 INI editor
//
//  Copyright (c) 1990 Microsoft
//
//  Author:  Charles E. Kindel, Jr. (charlkin)
//
#include <wdb.h>

#define INI_MAXVALUELEN    1024      // maximum value string length
#define INI_MAXSECTIONLEN  256    
#define INI_MAXNAMELEN     256
#define INI_MAXPATH        144
#define BUFFERSIZE         4096

#define INI_FILETYPE 0x0000      // file type for DlgDirSelectComboBox
#define DEFAULT_FILENAME "WIN.INI"
#define DEFAULT_FILESPEC "*.INI"

#define WM_MYINITDIALOG WM_USER + 1

//------------------------------------
// prototypes

void FAR PASCAL SaveWindowPos( HWND hWnd ) ;




