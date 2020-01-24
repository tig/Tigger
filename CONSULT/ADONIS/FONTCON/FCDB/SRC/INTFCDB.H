//*************************************************************
//  File name: intFCDB.h
//
//  Description: 
//      Internal global include file
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//                                          
//*************************************************************

#include "FCDB.h"

#ifndef RC_INVOKED
#include "..\..\inc\struct.h"
#include "..\..\inc\CONXLIB.h"
#endif

#define PRICE_ENCODE 0x816C5AB4

#define WE_PES              0
#define NUM_FOLDERS         6
#define SHIP_ICON           3
#define DOWNLOAD_ICON       5 
#define NO_ICON             3
#define EMPTY_ICON          3
#define GRAYCHECKBOX_ICON   5 

extern WORD      gwBitmapError;
extern WORD      gwLoadError;
extern HINSTANCE ghInstLib;
extern OFSTRUCT  gof;
extern struct _find_t gff;
extern struct _find_t gff2;

VOID FAR PASCAL bmColorTranslateDC( HDC hdcBM, LPBITMAP lpBM,
                                  COLORREF rgbOld, COLORREF rgbNew,
                                  WORD  wStyle ) ;

HBITMAP WINAPI LoadLBBitmap( HINSTANCE hinst, LPCSTR lpsz ) ;
VOID WINAPI SortDatabase(LPDB lpdb);
VOID WINAPI SortList(HLIST hlst, int nIndex);

//*** EOF: intFCDB.h
