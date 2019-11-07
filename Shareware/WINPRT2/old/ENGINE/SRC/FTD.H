
typedef struct tagDLGBOXHEADER
{
    DWORD   lStyle ;
    BYTE    bNumberOfItems ;
    int     x ;
    int     y ;
    int     cx ;
    int     cy ;

    LPSTR   szMenuName ;
    LPSTR   szClassName ;
    LPSTR   szCaption ;

    UINT    wPointSize ;    /* only if DS_SETFONT */
    LPSTR   szFaceName ;  /* only if DS_SETFONT */
} DLGBOXHEADER, *PDLGBOXHEADER, FAR *LPDLGBOXHEADER ;

typedef struct tagCTRLDATA
{
    int     x ;
    int     y ;
    int     cx ;
    int     cy ;
    int     wID ;
    DWORD   lStyle ;
    LPSTR   szClass ;
    LPSTR   szText ;
} CTRLDATA, *PCTRLDATA, FAR *LPCTRLDATA ;


LPDLGBOXHEADER WINAPI GetDlgBoxRes( HMODULE ghmodDLL, LPSTR lpszTemplate ) ;

