// util.h

#ifndef _UTIL_H_
#define _UTIL_H_

/// Utilities   
BOOL RestorePosition( CWnd* pWnd, CString szKey ) ;
BOOL SavePosition( CWnd* pWnd, CString szKey ) ;


#define RCF_NORMAL      0x0000
#define RCF_ITALIC      0x0001
#define RCF_UNDERLINE   0x0002
#define RCF_STRIKEOUT   0x0004
#define RCF_BOLD        0x0008
#define RCF_NODEFAULT   0x0010
#define RCF_TRUETYPEONLY 0x0011

CFont *ReallyCreateFont( HDC hDC, LPSTR lpszFace, LPSTR lpszStyle, UINT nPointSize, UINT uiFlags ) ;

LPSTR FAR PASCAL PointerToNthField(LPSTR lpszString, int nField, char chDelimiter) ;

LONG WINAPI ParseOffNumber( LPSTR FAR *lplp, LPINT lpConv ) ;

void WINAPI DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient ) ;
void bmColorTranslateDC( CDC* pdcMem, BITMAP *pBM, COLORREF rgbOld,  COLORREF rgbNew ) ;
void bmColorTranslate( CBitmap* pbmSrc, COLORREF rgbNew ) ;
CBitmap* bmLoadAndTranslate( UINT id, COLORREF rgb );
void DrawListBoxBitmap( CDC* pdc, CBitmap *pbmt, int x,  int y ) ;
int WINAPI GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTM ) ;    

typedef struct _far tagCOLUMNSTRUCT
{
   int   nLeft ;       // starting x position of the column
   int   nRight ;      // ending x position of the column
   UINT  uiFlags ;      // format flags

} COLUMNSTRUCT, *PCOLUMNSTRUCT, FAR *LPCOLUMNSTRUCT ;

void WINAPI ColumnTextOut( CDC* pDC, int nX, int nY, CString* pStr,
                            int cColumns, LPCOLUMNSTRUCT rgColumns ) ;
LPSTR HRtoString( HRESULT hr ) ;
LPSTR VTtoString( VARTYPE vt ) ;
    
VOID WINAPI StripFilespec( LPSTR lpszPath ) ;
VOID WINAPI AddBackslash( LPSTR lpszPath ) ;

BOOL WINAPI GetPrivateProfileSections( LPSTR lpOutBuf, WORD cbMax, LPSTR lpszFile, WORD wFileSize ) ;

#endif 

