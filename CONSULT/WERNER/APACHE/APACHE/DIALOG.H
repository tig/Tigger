/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  dialog.h
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Header file for dialog.c
 *
 *   Revisions:  
 *     01.00.000  6/25/91 baw   Wrote this comment block.
 *
 ************************************************************************/

// constants

#define MAXLEN_COLUMNDATA          256
#define MAXLEN_TEMPSTR             81
#define MAXLEN_MODEMSTR            256

#define MAXLEN_MODEMRESULT         256

#define MAXLEN_MODEMLIST           36

#define MAXLEN_SESSIONDIRBTNS      7
#define MAXLEN_SESSIONDIRCOLS      9

#define MAXLEN_SESSIONDIRNAME      30
#define MAXLEN_SESSIONDIRNUMBER    20

#define MAXLEN_DIALINGBTNS         2

#define TIMERID_DLGFSM             0x100
#define TIMERINTERVAL_DLGFSM       250

#define SHADOWWIDTH                1

#define MODEM_WAITCHAR             '~'

// atom definitions

#define ATOM_DISPLAYOPTIONS        0x0100
#define ATOM_XSIZE                 0x0101
#define ATOM_YSIZE                 0x0102
#define ATOM_ENTRY                 0x0103
#define ATOM_INEDITMODE            0x0104
#define ATOM_MODEM                 0x0105
#define ATOM_SLIDERPOS             0x0106

// dialing directory specific atoms

#define ATOM_NUMSEL                0x0200
#define ATOM_SELARRAY              0x0201
#define ATOM_CALLING               0x0202
#define ATOM_TIMEL                 0x0203
#define ATOM_TIMEH                 0x0204
#define ATOM_PAUSETIME             0x0205
#define ATOM_WAITTIME              0x0206
#define ATOM_PASSNUMBER            0x0207
#define ATOM_HSCB                  0x0208
#define ATOM_MODEMRESULT           0x0209
#define ATOM_RESULTPOS             0x020A
#define ATOM_MODEMSTRINDEX         0x020B
#define ATOM_INITTIME              0x020C

#define ATOM_FSMSTATE              0x0300

// dialing directory FSM functions

#define DDF_INIT                   0x0100
#define DDF_RECYCLE                0x0101
#define DDF_DELETE                 0x0102
#define DDF_UPDATE                 0x0103
#define DDF_CLEANUP                0x0104
#define DDF_CANCEL                 0x0105

// dialing directory FSM states

#define DDS_PAUSE                  0x0200
#define DDS_NEXTCALL               0x0201
#define DDS_CONNECT                0x0202
#define DDS_INITMODEM              0x0203
#define DDS_WAITINITRESPONSE       0x0204
#define DDS_DIALMODEM              0x0205
#define DDS_WAITDIALRESPONSE       0x0206
#define DDS_DISCONNECT             0x0207
#define DDS_CONNECTED              0x0208

// data types

typedef struct tagEPSSTRUCT
{
   char         szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;
   BOOL NEAR *  npDisplayOptions ;
   HINSTANCE    hInstance ;
   HWND         hCtrlWnd ;

} EPSSTRUCT, FAR *LPEPSSTRUCT ;

typedef struct tagSESSIONDIRDLGWNDOFFSETS
{
   POINT  ptFrameTop ;
   POINT  ptFrameBottom ;
   POINT  ptLBTop ;
   POINT  ptLBBottom ;
   POINT  ptSliderTop ;
   POINT  ptSliderBottom ;

   POINT  ptButtons ;

} SESSIONDIRDLGWNDOFFSETS, NEAR *NPSESSIONDIRDLGWNDOFFSETS ;

// macros

#define HCTRLWND( x )   ( ((LPEPSSTRUCT) x) -> hCtrlWnd )
#define SZINIFILE( x )  ( ((LPEPSSTRUCT) x) -> szINIFile )
#define NPDSPOPT( x )   ( ((LPEPSSTRUCT) x) -> npDisplayOptions )
#define HINST( x )      ( ((LPEPSSTRUCT) x) -> hInstance )

#define SET_PROP( x, atom,  y )  SetProp( x, MAKEINTATOM( atom ), y )
#define GET_PROP( x, atom )      GetProp( x, MAKEINTATOM( atom ) )
#define REMOVE_PROP( x, atom )   RemoveProp( x, MAKEINTATOM( atom ) )

// function prototypes (private)

BOOL NEAR SessionDirDlgInit( HWND ) ;
BOOL NEAR DlgFillSessionDirLB( HWND, BOOL NEAR * ) ;

WORD NEAR ReadSessionDirDisplayOptions( HINSTANCE, BOOL NEAR * ) ;
BOOL NEAR WriteSessionDirDisplayOptions( HINSTANCE, BOOL NEAR * ) ;

BOOL NEAR EnableSessionDirControls( HWND, BOOL ) ;

BOOL NEAR SessionDirEntryDlgInit( HWND, LPARAM ) ;
BOOL NEAR DlgModemListCB( HWND, int, int ) ;
WORD NEAR CBFindModule( HWND, LPSTR ) ;
BOOL NEAR SessionDirEntryDlgTerm( HWND, BOOL ) ;

BOOL NEAR SessionDirOptionsDlgInit( HWND, LPARAM ) ;
BOOL NEAR SessionDirOptionsDlgTerm( HWND, BOOL ) ;

BOOL NEAR ModemSettingsDlgInit( HWND, LPARAM ) ;
BOOL NEAR ModemSettingsDlgTerm( HWND, BOOL ) ;

BOOL NEAR DialingDlgInit( HWND, LPARAM ) ;
VOID NEAR DialingDlgFSM( HWND, WPARAM ) ;
BOOL NEAR DialingDlgTerm( HWND ) ;

// owner-draw helper functions

VOID NEAR GetColumnData( LPSTR, int, NPSTR, int ) ;
BOOL NEAR DlgDrawItem( HWND, LPDRAWITEMSTRUCT ) ;
VOID NEAR DlgMeasureItem( HWND, LPMEASUREITEMSTRUCT ) ;
BOOL NEAR DlgCompareItem( HWND, LPCOMPAREITEMSTRUCT ) ;
LPWORD NEAR GetBtnInfo( HINSTANCE, int ) ;
BOOL NEAR ReleaseBtnInfo( LPWORD ) ;
VOID NEAR UpdateSliderPos( HWND, LPRECT, int, int ) ;
VOID GetPhoneINIPath( LPSTR ) ;

BOOL FAR PASCAL LinkDialog( HWND ) ;
BOOL FAR PASCAL UnlinkDialog( HWND ) ;

// function prototypes (exported)

BOOL FAR PASCAL SessionDirDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL EPSCallbackProc( LPSTR, LPARAM ) ;
BOOL FAR PASCAL SessionDirEntryDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL SessionDirOptionsDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL ModemSettingsDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL DialingDlgProc( HWND, UINT, WPARAM, LPARAM ) ;

/************************************************************************
 * End of File: dialog.h
 ************************************************************************/

