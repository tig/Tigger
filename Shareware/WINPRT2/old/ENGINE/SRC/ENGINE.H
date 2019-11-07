/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Main header file for WinPrint 2.0 Print Engine DLL.
 *
 ***********************************************************************/

#ifndef _ENGINE_H_
#define _ENGINE_H_

#ifndef _INC_PRINT
#include <print.h>
#endif


#ifdef _INTERNAL_
/*----------------------------------------------------------------------------*/
#include "version.h"                                                            
#include <cek.h>
#include "..\res\engine.str"                                                    
#include "engine.h"                                                             
                                                                                
#define INTERNAL NEAR PASCAL                                                    
                                                                                
#define GRCS(x) (LPSTR)GetRCString( x, ghmodDLL )                               
                                                                                
#define FAIL(x) {dwLastError = x;goto Fail;}                                    

#ifdef DEFINE_GLOBALS                                                           
                                                                                
HMODULE         ghmodDLL ;                                                          
DWORD           dwLastError ;                                                       

/* Global configuration info
 */
UINT            uiMeasureUnit = 0 ;
BOOL            fUsePrintOffset = FALSE ;

#else                                                                           
                                                                                
extern  HMODULE     ghmodDLL ;                                                  
extern  DWORD       dwLastError ;                                               
extern  UINT        uiMeasureUnit ;
extern  BOOL        fUsePrintOffset ;
                                                                                
#endif                                                                          

BOOL WINAPI RegisterPreview( HINSTANCE hinst, LPSTR lpszClass ) ;
                                                                                
/*******************************************************                        
 * Internal parameter validation macros                                         
 *******************************************************/                       
#define VALIDATEPARAM(p1)\
{\
    if (p1 == NULL)\
        {dwLastError = ENGERR_INVALIDPTR; return FALSE ;}\
}                                                                               
                                                                                
                                                                       
/*----------------------------------------------------------------------------*/
#endif // _INTERNAL_


/**************************************************************************
 * Basic type declarations
 **************************************************************************/

/* Unsigned SIZE structure (Hungarian: usz)
 */
typedef struct tagUSIZE
{
    DWORD   dx ;
    DWORD   dy ;
} USIZE, *PUSIZE, FAR *LPUSIZE ;

/* Unsigned POINT structure (Hungarian: upt)
 */
typedef struct tagUPOINT
{
    DWORD   cx ;
    DWORD   cy ;
} UPOINT, *PUPOINT, FAR *LPUPOINT ;

/* Unsigned RECT structure (Hungarian: urc)
 */
typedef struct tagURECT
{
    DWORD  cx ;
    DWORD  cy ;
    DWORD  dx ;
    DWORD  dy ;
} URECT, *PURECT, FAR *LPURECT ;

/**************************************************************************
 * WinPrint 2.0 Engine type declarations
 **************************************************************************/

typedef LRESULT (CALLBACK* FTDPROC)(UINT, LPARAM, LPARAM, LPARAM) ;

/************* Engine flags
 */
#define ENG_SHADE_LTGRAY            0x00000001 
#define ENG_SHADE_GRAY              0x00000002 

#define ENG_TOPBORDER               0x00010000 
#define ENG_LEFTBORDER              0x00020000 
#define ENG_RIGHTBORDER             0x00040000 
#define ENG_BOTTOMBORDER            0x00080000 
#define ENG_TOPBORDER_BOLD          0x00100000 
#define ENG_LEFTBORDER_BOLD         0x00200000 
#define ENG_RIGHTBORDER_BOLD        0x00400000 
#define ENG_BOTTOMBORDER_BOLD       0x00800000 

/************* Error values (engGetLastError())
 *
 * The LOWORD of these error values is the primary error code.
 * In some cases, the HIWORD contains additional error information.
 */
#define ENGERR_ZERO                 0x00000000
#define ENGERR_NOERROR              ENGERR_ZERO
#define ENGERR_GENERAL              0x00000001
#define ENGERR_INVALIDPTR           0x00000002
#define ENGERR_GLOBALALLOC          0x00000003
#define ENGERR_GLOBALREALLOC        0x00000004
#define ENGERR_INVALIDSTRUCTSIZE    0x00000005
#define ENGERR_CONFIGNOTFOUND       0x00000006
#define ENGERR_CONFIGCORRUPT        0x00000007
#define ENGERR_STRINGTOOLONG        0x00000008
#define ENGERR_NODEFAULTPRN         0x00000009
#define ENGERR_STARTDOCFAILED       0x0000000a
#define ENGERR_STARTPAGEFAILED      0x0000000b
#define ENGERR_ENDPAGEFAILED        0x0000000c
#define ENGERR_USERABORTED          0x0000000d
#define ENGERR_PRINTERSETUP         0x0000000e
#define ENGERR_FTDOUTPUTFAILED      0x0000000f

/************* HeaderFooter Section struct
 *
 * Headers/Footers are made up of three sections, right, left and
 * center.  Each can have it's own font.
 */
#define MAX_HFTEXTLEN 128
typedef struct tagENGINEHF_SECT 
{
    /**** Non-Saved information ****/
    UINT    uiStructSize ;

    /**** Saved information ****/
    char    szText[MAX_HFTEXTLEN] ;

    char    szFace[LF_FACESIZE+1] ;
    char    szStyle[LF_FACESIZE+1] ;
    WORD    wPoints ;  

} ENGINEHF_SECT, *PENGINEHF_SECT, FAR *LPENGINEHF_SECT ;

/************* EngineHF - Engine HeaderFooter struct
 *
 * The EngineHF struct defines the generic header/footer that shodwd
 * handle 80% of all header footer needs.
 *
 * All measurements are stored as 100ths of a millimeter.
 *
 */
typedef struct tagENGINEHF
{
    /**** Non-Saved information ****/
    UINT    uiStructSize ;

    /**** Saved information ****/
    DWORD   dwFlags ;     
        
    UINT    uiHeight ;          // If ENGHF_NOAUTOHEIGHT then this is saved

    ENGINEHF_SECT Left ;
    ENGINEHF_SECT Center ;
    ENGINEHF_SECT Right ;

    DWORD  dwBorderFlags ;     // overrides ENGINEPAGE setting

    DWORD  dwShadeFlags ;  

} ENGINEHF, *PENGINEHF, FAR *LPENGINEHF ;

#define ENGHF_NOAUTOHEIGHT      0x00000001 

typedef struct tagFTDINFO
{
    UINT    uiStructSize ;
    LPARAM  lpData ;
} FTDINFO, *PFTDINFO, FAR *LPFTDINFO ;

#define MAX_CONFIGNAMELEN       64
#define MAX_DRIVERLEN           13
#define MAX_DEVICELEN           32
#define MAX_UNCSHARELEN         64
#define MAX_PATHLEN             168     // 162 might be correct
#define MAX_OUTPUTLEN           MAX_PATHLEN 

/************* ENGINECONFIG makes up the root of a WinPrint configuration.
 *
 */
typedef struct tagENGINECONFIG
{
    /**** Non-Saved information ****/
    UINT        uiStructSize ;
    char        szConfigName[MAX_CONFIGNAMELEN] ;

    char        szFTDD[MAX_PATHLEN] ;
    char        szFTD[MAX_CONFIGNAMELEN] ;
    FTDPROC     lpfnFTD ;
    FTDINFO     FTDInfo ;         

    DWORD       dwReserved1 ;       // (really lpJob)
    DWORD       dwReserved2 ;       

    /**** Saved information ****/
    DWORD       dwFlags ;           

    /* The following three members are used when the ENGCFG_USEDEFAULTPRN
     * flag is NOT set.  If that flag is set all three of these members will
     * be NULL.
     */
    char        szDriver[MAX_DRIVERLEN] ;
                                    // NULL terminated string that identifies
                                    // the printer driver (HPPCL.DRV)

    char        szDevice[MAX_DEVICELEN] ;      
                                    // NULL terminated string that identifies
                                    // the device (HP LaserJet II)

    char        szOutput[MAX_OUTPUTLEN] ;
                                    // NULL terminated string that identifies
                                    // the DOS device name for the physical
                                    // output medium (port)

    /* The following are the parts of a DEVMODE structure that are worth
     * saving configuration.  The dmFields member is used by the engine
     * to determine which members to use.  If a member's bit is not set
     * in dmFields, the printer's defadwt value will be used.
     */
    DWORD       dmFields;           // specifies which members are meaningfdw.
    short       dmOrientation;      // supported in UI
    short       dmPaperSize;        // supported in UI
    short       dmPaperLength;      // not used in UI
    short       dmPaperWidth;       // not used in UI
    short       dmScale;            // not used in UI
    short       dmCopies;           // supported in UI
    short       dmDefaultSource;    // not used in UI
    short       dmPrintQuality;     // not used in UI
    short       dmColor;            // not used in UI
    short       dmDuplex;           // not used in UI
//    short       dmCollate;          // not used in UI (in win32?)

    /* Margins
     */
    UINT        uiTopMargin ;       // Margins define the printable area.
    UINT        uiBottomMargin ;    // NOTHING will ever be printed outside
    UINT        uiLeftMargin ;      // of the margins.
    UINT        uiRightMargin ;

    DWORD       dwBorderFlags ; 
    DWORD       dwShadeFlags ;          // (margins only)
    
    /* Headers/Footers
     */
    ENGINEHF    Header ;
    ENGINEHF    Footer ;

    /* Multiple pages up is defined in terms of rows and columns.
     * A one-up job would have one row and one column.  A two up
     * job might have a 1 row and 2 columns.  Four pages up would
     * probably be 2 rows and 2 columns.
     */
    UINT        uiPageRows ;
    UINT        uiPageColumns ;

    /* There can be space between the multiple pages up.
     */
    UINT        uiHorzSep ;
    UINT        uiVertSep ;

} ENGINECONFIG, * PENGINECONFIG, FAR *LPENGINECONFIG ;

#define MAX_ROWSCOLUMNS         8

#define ENGCFG_USEDEFAULTPRN     0x000000001

/**************************************************************************
 * WinPrint 2.0 Engine APIs
 **************************************************************************/

DWORD WINAPI engGetLastError( VOID ) ;

/************* Configuration APIs
 *
 * For reading and writing configurations.
 *
 * There are three classes of configuration information.  "Engine", "UI" and
 * "FileType".
 *
 * Engine configuration information is stored in the WINPRINT.INI file in
 * sections with section names of the form "[Engine - <config>]" where
 * <config> is the name of the configuration.
 *
 * Each FileType configuration references a Engine configuration.  2.0 will
 * probably have a 1 to 1 relationship between Engine and FileType 
 * configs, but it's possible that this will be expanded later.
 *
 * The Engine configuration data is stored as HEX.  The entire ENGINECONFIG
 * structure is written in HEX, and pointers within the structures are
 * fixed up so that they become offsets from the beginning of the 
 * ENGINECONFIG structure.  The data pointed to (all sz strings) is appended
 * to the end.
 **************/

/************* engConfigRead()
 *
 * Retrieves ENGINECONFIG info.
 *
 * HWND     hwnd        Parent window for error dialogs.
 * LPSTR   lpszName    Config name.  If lpszFile is NULL, "Engine -" is 
 *                      prepended automatically.
 * LPSTR   lpszFile    INI filename.  If NULL, WINPRINT.INI is used.
 * LPENGINECONFIG       Pointer to buffer that is big enough to hold the
 *                      Engine configuration structure.
 * 
 * Since the structure contains LPSTR members, this function will 
 * allocate buffers big enough to hold the strings for these members 
 * and will copy the sz strings into the buffers.  Thus it is a requirement
 * that the caller either free these additional buffers or call 
 * engFreeConfig(). 
 *
 * The buffers are allocated using GlobalAllocPtr() and thus shodwd be
 * freed using GlobalFreePtr().
 *
 * Returns 0 if the configuration was not found or if an error
 * occurred.  Otherwise the return value is the number of bytes copied.
 *
 * Use engGetLastError() to determine the cause of the error.
 *************/
BOOL WINAPI engConfigRead( HWND hwnd, LPSTR lpszName, LPSTR lpszFile, LPENGINECONFIG lpEC );

/************* engConfigWrite()
 *
 * Saves ENGINECONFIG info to an configuration file.
 *
 * HWND     hwnd        Parent window for error dialogs.
 * LPSTR   lpszName    Config name.  If lpszFile is NULL, "Engine -" is 
 *                      prepended automatically.
 * LPSTR   lpszFile    INI filename.  If NULL, WINPRINT.INI is usned.
 * LPENGINECONFIG       Pointer to buffer that holds the Engine configuration.
 * 
 * Returns the number of bytes written to the file.  0 if an error occured.
 *
 * Use engGetLastError() to determine the cause of the error.
 *************/
BOOL    WINAPI engConfigWrite( HWND hwnd, LPSTR lpszName, LPSTR lpszFile, LPENGINECONFIG lpEC ) ; 
LPENGINECONFIG WINAPI engConfigAlloc( VOID ) ;
LPENGINECONFIG WINAPI engConfigCopy( LPENGINECONFIG lpECout, LPENGINECONFIG lpECin ) ;
BOOL    WINAPI engConfigFree( LPENGINECONFIG lpEC ) ;

BOOL WINAPI engConfigSetDefaults( HWND hwnd, LPSTR lpszName, LPSTR lpszFile, LPENGINECONFIG lpEC ) ;

#ifdef _INTERNAL_
/*----------------------------------------------------------------------------*/

/******************************************************                         
 * Engine Job structure                                                         
 ******************************************************/                        
typedef struct tagENGINEJOB                                                     
{                                                                               
    UINT            uiStructSize ;                                              
    DWORD           dwFlags ;
                                                                                
    LPENGINECONFIG  lpEC ;                                                      

    HDC             hdc ;
                                                                                    
    /* Page size                                                                
     * Calculated at run-time...device dependent.                               
     */                                                                         
    UINT        nPageX ;           // in device units                          
    UINT        nPageY ;           // in device units                          
                                                                                
    UINT        nPixelPer10CMX ;                                                  
    UINT        nPixelPer10CMY ;                                                  
                                                                                
    POINT       ptPrintOffset ;                                                 
                                                                                
    /* The workspace is defined by the margins and header/footer                
     * dimensions.                                                              
     *                                                                          
     * PageWorkspace encompases the entire workspace for a physical             
     * page.  That is, it includes the sum of all the logical pages.            
     *                                                                          
     */                                                                         
    RECT         rcPhysWorkspace ;  // cx,cy is origin of workspace             
                                    // dx - cx is width of workspace            
                                    // dy - cy is height of workspace           
                                    // Workspace size is device dependent       
                                    // because it can vary because of           
                                    // page size.                               

    /* Dimensions of a sub-page.
     * 
     * NOTE!:  Because of rounding errors when dealing with a large number
     * of 'sub-pages', the uiSubPage values should be used only to determine
     * the dimensions of a given sub-page.  These values will typically be
     * just a tad bit smaller than the actual dimensions of a given sub-page.
     */                                                                         
    UINT        uiSubPageX ;                                                    
    UINT        uiSubPageY ;                                                    
                                                                                
    /* Page numbers                                                             
     */                                                                         
    UINT        nPhysicalPage ; 
    UINT        nPhysicalPages ;                                                
    UINT        nLogicalPage ;                                                  
    UINT        nLogicalPages ;
                                                                                
    /* hWnd to be used as the parent of status dialogs etc...                   
     * This window will receive notification messages...
     */                                                                         
    HWND        hwndParent ; 
    
    /* Cancel Print/Status Dialog Box
     */
    HWND        hdlgStatus ;
    DLGPROC     lpfnStatus ;
    
    /* AbortProc 
     */
    ABORTPROC   lpfnAbort ;                                                   
    BOOL        fUserAborted ;

    LPSTR       lpszDocName ;
    /* Place to hold return value (error number)
     */
    DWORD       dwResult ;
                                                                                                
    /* File information                                                         
     */                                                                         
    LPSTR       lpszFile ;          // points to fully qualified filename       
    HFILE       hFile ;             // Handle to open file                      
    DWORD       dwBytes ;           // file size
    DWORD       dwBytesRemaining ;  // bytes remaining to be processed
                                                                                
    LPSTR       lpCustData ;        // custom filetype data
                                                                                
} ENGINEJOB, *PENGINEJOB, FAR *LPENGINEJOB ;                                    

#define ENGJOB_NOSTATUSDLG      0x00010000
#define HENGINEJOB  LPENGINEJOB                                                 

/* These two macros will return an accurate x and y offset to the
 * top-left corner of a sub-page given a column or row number and
 * an lpJob
 */
#define COL_POS( col, lpJob )\
    MulDiv( col, lpJob->rcPhysWorkspace.right -\
                 MulDiv( lpEC->uiVertSep,\
                     lpJob->nPixelPer10CMY, 10000 ) * (lpEC->uiPageColumns-1),\
            lpEC->uiPageColumns )

#define ROW_POS( row, lpJob)\
    MulDiv( row, lpJob->rcPhysWorkspace.bottom -\
                 MulDiv( lpEC->uiHorzSep,\
                       lpJob->nPixelPer10CMX, 10000 ) * (lpEC->uiPageRows-1),\
            lpEC->uiPageRows )

/* UTIL.C
 */
DWORD WINAPI Div( DWORD n, DWORD d ) ;
VOID WINAPI AnsiPathToOEM( LPSTR szPath ) ;
BOOL WINAPI IsWild( LPSTR lpszPath ) ;
VOID WINAPI AddBackslash( LPSTR lpszPath ) ;
VOID WINAPI StripBackslash( LPSTR lpszPath ) ;
VOID WINAPI StripFilespec( LPSTR lpszPath ) ;
VOID WINAPI StripPath( LPSTR lpszPath ) ;
LPSTR WINAPI GetExtension( LPSTR lpszFile ) ;
BOOL WINAPI FindExtensionInList( LPSTR lpszExt, LPSTR lpszList ) ;
BOOL WINAPI IsProgram( LPSTR lpszPath ) ;

/* PRINTER.C internals
 */
LPENGINEJOB WINAPI GetJobFromSleazyTable( HDC hdc ) ;
VOID WINAPI PutJobInSleazyTable( LPENGINEJOB lpJob ) ;
VOID WINAPI RemoveJobFromSleazyTable( LPENGINEJOB lpJob ) ;


/* FTD stuff
 */

typedef BOOL (WINAPI *FTDD_INIT)( VOID ) ;
#define ORD_FTDD_INIT                   2
       
typedef BOOL (WINAPI *FTDD_UNINIT)( VOID ) ;
#define ORD_FTDD_UNINIT                 3

typedef LPSTR (WINAPI *LPFN_FINDFIRSTFTD)( LPSTR ) ;
#define ORD_FTDD_FINDFIRSTFTD           4

typedef LPSTR (WINAPI *LPFN_FINDNEXTFTD)( LPSTR, LPSTR ) ;
#define ORD_FTDD_FINDNEXTFTD            5

typedef FTDPROC (WINAPI *LPFN_GPA)(LPSTR) ; // FTDD_GetProcAddress
#define ORD_FTDD_GPA                    6

typedef struct tagFTDD
{
    LPSTR       lpszFTD_Name ;  // if HIWORD is NULL, LOWORD contains IDS
    FTDPROC     lpfnFTD ;       // points to FTD proc
} FTDD ;

BOOL    WINAPI ftdChoose( HWND hwndParent, LPENGINECONFIG lpEC )  ;
LRESULT WINAPI ftdDefaultProc( UINT uiCmd, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3 ) ;

LPSTR   WINAPI ftdConfigGetName( LPSTR lpszCfg, LPSTR lpszFN ) ;
BOOL    WINAPI ftdConfigWrite( HWND hwnd, LPENGINECONFIG lpEC, LPSTR lpszSection, LPSTR lpszFile ) ;
BOOL    WINAPI ftdConfigRead( HWND hwnd, LPENGINECONFIG lpEC, LPSTR lpszSection, LPSTR lpszFile ) ;
BOOL    WINAPI ftdConfigInit( HWND hwnd, LPENGINECONFIG lpEC ) ;
FTDPROC WINAPI ftdLoadFTD( LPSTR lpszFTDD, LPSTR lpszFTD )  ;
BOOL    WINAPI ftdFreeFTD( FTDPROC lpfnFTD ) ;
BOOL    WINAPI ftdFillListBoxWithFTDs( HWND hwnd ) ;
BOOL    WINAPI ftdGetSelectedFTDFromListBox( HWND hwnd, LPSTR lpszFTDD, LPSTR lpszFTD ) ;
BOOL    WINAPI ftdSelectFTDInListBox( HWND hwnd, LPSTR lpszFTDD, LPSTR lpszFTD ) ;

#define FTD_GETMODULEHANDLE         0x0001

#define FTD_READCONFIG              0x0002
#define FTD_WRITECONFIG             0x0003
#define FTD_GETDLGTEMPLATE          0x0004
#define FTD_CONFIGDLGMSG            0x0005
#define FTD_INITCONFIG              0x0006
#define FTD_STARTDOC                0x0007
#define FTD_ENDDOC                  0x0008
#define FTD_OUTPUTPAGE              0x0009
#define FTD_QUERYDATALEFT           0x000A

/*----------------------------------------------------------------------------*/
#else
#define HENGINEJOB  DWORD
#endif // _INTERNAL_
                                                                          

#define CREATEJOB_BYCONFIGNAME       0x0000
#define CREATEJOB_BYENGINECONFIG     0x0001

HENGINEJOB  WINAPI engJobCreate( HWND hWnd, LPARAM lParam, DWORD dwFlags ) ;
BOOL        WINAPI engJobReCalc( HENGINEJOB lpJob, LPENGINECONFIG lpEC ) ;
BOOL        WINAPI engJobDestroy( HENGINEJOB lpJob ) ;
BOOL        WINAPI engJobReset( HENGINEJOB lpJob, LPENGINECONFIG lpEC ) ;

HDC         WINAPI engPrinterCreateDC( HWND hwndParent, LPENGINECONFIG lpEC ) ;
BOOL        WINAPI engPrinterGetDefault( HWND hwndParent, LPENGINECONFIG lpEC ) ;
BOOL        WINAPI engPrinterChoose( HWND hwndParent, LPENGINECONFIG lpEC ) ;
LPDEVMODE   WINAPI engPrinterGetDevMode( HWND hwnd, LPENGINECONFIG lpEC ) ;

BOOL WINAPI engPrint( HWND hWnd, LPSTR lpszFile, LPSTR lpszConfig ) ;


UINT        WINAPI engHFCalcHeight( HENGINEJOB lpJob, LPENGINEHF lpEHF ) ;
LPSTR       WINAPI engHFExpand( HENGINEJOB lpJob, LPSTR lpszIn ) ;
BOOL        WINAPI engHFConfigure( HWND hwndParent, LPENGINECONFIG lpEC );

BOOL        WINAPI engPageConfigure( HWND hwndParent, LPENGINECONFIG lpEC ) ;

BOOL        WINAPI engDlgSetMeasure( HWND hDlg, UINT ui, UINT  uiMM ) ;
UINT        WINAPI engDlgGetMeasure( HWND hDlg, UINT ui ) ;
UINT        WINAPI engStrToUINT( LPSTR lpsz, UINT uiDecPts ) ;
LPSTR       WINAPI engInchToString( LPSTR lpsz, UINT  uiIn ) ;
LPSTR       WINAPI engMMToString( LPSTR lpsz, UINT  uiMM ) ;
UINT        WINAPI engMMToInch( UINT  uiMM ) ;
UINT        WINAPI engInchToMM( UINT  uiIn ) ;

UINT        WINAPI engGetMeasureUnit(void) ;    
#define UNIT_INCHES         0
#define UNIT_CENTIMETERS    1
#define UNIT_POINTS         2   // not implemented

HWND WINAPI engPreviewCreate( HWND hwndParent, DWORD dwFlags, int x, int y, int dx, int dy, LPENGINECONFIG lpEC ) ;
BOOL        WINAPI engPreviewDestroy( HWND hwndPreview ) ;
VOID        WINAPI engPreviewReDraw( HWND hwndPreview, BOOL fReset, BOOL fErase ) ;
LPENGINECONFIG WINAPI engPreviewSetConfig( HWND hwndPreview, LPENGINECONFIG lpEC ) ;
LPENGINECONFIG WINAPI engPreviewGetConfig( HWND hwndPreview ) ;
BOOL        WINAPI engPreviewPaintPage( LPENGINECONFIG lpEC, HWND hwndClient, HDC hdcClient ) ;

BOOL        WINAPI engPaintPage( HENGINEJOB lpJob, HDC hdc ) ;
BOOL        WINAPI engPaintFrame( HENGINEJOB lpJob, HDC hdc )  ;
BOOL        WINAPI engPaintHF( LPENGINEJOB lpJob, HDC hdc, LPENGINEHF lpehf, LPRECT lprc ) ;


BOOL WINAPI engWinFileInstall( VOID ) ;
BOOL WINAPI engWinFileRemove( VOID ) ;

BOOL WINAPI engAssociate( HWND hwnd, LPSTR lpszExt, LPSTR lpszConfig ) ;
BOOL WINAPI engFileTypeModify( HWND hwnd, LPSTR lpszConfig ) ;
BOOL WINAPI engFileTypeCopy( HWND hwnd, LPSTR lpszConfigOld, LPSTR  lpszConfigNew );
#endif

/************************************************************************
 * End of File: ENGINE.H
 ***********************************************************************/

