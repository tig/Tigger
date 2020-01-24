/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  resource.h
 *
 *     Remarks:  
 *
 *    This include file contains resource ID definitions for 
 *    the main Apache executable.
 *
 *    There is another resource ID file for dialog boxes since the
 *    WhiteWater Resource Toolbox fails on complex include files.
 *
 *    Resources need to be grouped together for optimization and also
 *    should have meaningful names.  To this end the resource IDs in
 *    Apache are prefixed with IDx_y, where x identifies the type
 *    of resource and y identifies the resource.
 *
 *       IDS_  - String table entries
 *       IDM_  - Menu items
 *       IDD_  - Dialog box ID values (in DLG_DEF.H).
 *
 *    ID Value    Point to
 *    ------------------------------------------------------------
 *    0    - 1023    Miscelaneous resources.
 *    1024 - 1535    Menu items.
 *    1536 - 2047    Buttons / button labels
 *    2048 - 4095    String table resources.
 *                   Note that there may be string resources associated
 *                   with an ID in another group, such as menus.
 *    4096 - 5140    Dialog box items.
 *    
 *
 *   Revisions:  
 *     00.00.000  3/24/91 cek   Organzied
 *
 ***********************************************************************/

/*
 *  Menu items start at 1024
 */

#define  IDM                           1024

#define  IDM_SESSION                   (IDM + 0)
#define  IDM_SESSIONDIR                (IDM + 1)
#define  IDM_OPENSESSION               (IDM + 2)
#define  IDM_CLOSE                     (IDM + 3)
#define  IDM_EXIT                      (IDM + 4)
#define  IDM_SETUPCONNECTOR            (IDM + 5)
#define  IDM_SETUPEMULATOR             (IDM + 6)

#define  IDM_CREATETASK                (IDM + 10)
#define  IDM_STARTTASK                 (IDM + 11)
#define  IDM_STOPTASK                  (IDM + 12)
#define  IDM_DESTROYTASK               (IDM + 13)

#define  IDM_WINDOW                    (IDM + 16)
#define  IDM_TILE	                     (IDM + 17)
#define  IDM_CASCADE	                  (IDM + 18)
#define  IDM_ARRANGE	                  (IDM + 19)
#define  IDM_CLOSEALL                  (IDM + 20)

#define  IDM_DOWNLOAD                  (IDM + 25)
#define  IDM_UPLOAD                    (IDM + 26)

#define  IDM_HELP                      (IDM + 32)
#define  IDM_HELPINDEX	               (IDM + 33)
#define  IDM_HELPKEYBOARD              (IDM + 34)
#define  IDM_HELPCOMMANDS              (IDM + 35)
#define  IDM_HELPPROCEDURES	         (IDM + 36)
#define  IDM_HELPUSINGHELP	            (IDM + 37)
#define  IDM_ABOUT	                  (IDM + 38)
#define  IDM_MENUCHANGE                (IDM + 39)

// this should be the last thing!

#define  IDM_FIRSTCHILD                (IDM + 40)

/*
 * Buttons start at 1536
 */
#define IDB                            1536

#define IDB_DIAL                       (IDB + 0)
#define IDB_CONNECT                    (IDB + 1)
#define IDB_EDIT                       (IDB + 2)
#define IDB_NEW                        (IDB + 3)
#define IDB_DELETE                     (IDB + 4)
#define IDB_OPTIONS                    (IDB + 5)

#define IDB_RECYCLE                    (IDB + 10)

/*
 * Strings start at 2048
 */
#define IDS                            2048

/*
 * Miscelaneous strings
 */

#define IDS_EMULATOR_LBL               IDB_EMULATOR_LBL
#define IDS_EMULATOR                   IDB_EMULATOR

/*
 *  IDS_CLASS_* are Window class names
 */
#define IDS_CLASS_FRAME                (IDS + 64)
#define IDS_CLASS_TERMINAL             (IDS + 65)
#define IDS_CLASS_NEWMDICLIENT         (IDS + 66)


/*
 *  IDS_STATBOX_* are for the bottom status bar
 */
#define IDS_STATBOXFACE                (IDS + 72)
#define IDS_STATBOX                    (IDS + 73)
#define IDS_STATBOX_CAPSON             (IDS + 74)
#define IDS_STATBOX_CAPSOFF            (IDS + 75)
#define IDS_STATBOX_NUMON              (IDS + 76)
#define IDS_STATBOX_NUMOFF             (IDS + 77)
#define IDS_STATBOX_SCRLON             (IDS + 78)
#define IDS_STATBOX_SCRLOFF            (IDS + 79)

/*
 *  IDS_EXT_* are file extensions
 */
#define IDS_EXT_CONNECTOR              (IDS + 128)
#define IDS_EXT_EMULATOR               (IDS + 129)
#define IDS_EXT_SCRIPT                 (IDS + 130)
#define IDS_EXT_CONFIG                 (IDS + 131)
#define IDS_EXT_PHONEBOOK              (IDS + 132)

#define IDS_FILE_SESSION               (IDS + 133)
#define IDS_FILE_CONNECT               (IDS + 134)

/*
 *  IDS_SESSIONDIRCOL_* are column names for the dialing directory
 */

#define IDS_SESSIONDIRCOL_NAME         (IDS + 170)
#define IDS_SESSIONDIRCOL_NUMBER       (IDS + 171)
#define IDS_SESSIONDIRCOL_CONNECTION   (IDS + 172)
#define IDS_SESSIONDIRCOL_SETTINGS     (IDS + 173)
#define IDS_SESSIONDIRCOL_LASTCALL     (IDS + 174)
#define IDS_SESSIONDIRCOL_TOTALCALLS   (IDS + 175)
#define IDS_SESSIONDIRCOL_EMULATION    (IDS + 176)
#define IDS_SESSIONDIRCOL_MODEM        (IDS + 177)
#define IDS_SESSIONDIRCOL_PROTOCOL     (IDS + 178)
 

/*
 *  IDS_ERR_* are error strings.  Some are "fmt" strings (printf style)
 */


/*
 *  IDS_INI_* are for the configuration files
 */

#define IDS_INI_DISPLAYNAME            (IDS + 200)
#define IDS_INI_DISPLAYNUMBER          (IDS + 201)
#define IDS_INI_DISPLAYCONNECTION      (IDS + 202)
#define IDS_INI_DISPLAYSETTINGS        (IDS + 203)
#define IDS_INI_DISPLAYLASTCALL        (IDS + 204)
#define IDS_INI_DISPLAYTOTALCALLS      (IDS + 205)
#define IDS_INI_DISPLAYEMULATION       (IDS + 206)
#define IDS_INI_DISPLAYMODEM           (IDS + 207)
#define IDS_INI_DISPLAYPROTOCOL        (IDS + 208)

#define IDS_INI_ENTRYNAME              (IDS + 210)
#define IDS_INI_ENTRYNUMBER            (IDS + 211)
#define IDS_INI_ENTRYCONNECTOR         (IDS + 212)
#define IDS_INI_ENTRYSETTINGS          (IDS + 213)
#define IDS_INI_ENTRYLASTCALL          (IDS + 214)
#define IDS_INI_ENTRYTOTALCALLS        (IDS + 215)
#define IDS_INI_ENTRYEMULATOR          (IDS + 216)
#define IDS_INI_ENTRYMODEM             (IDS + 217)
#define IDS_INI_ENTRYPROTOCOL          (IDS + 218)

#define IDS_INI_DIALPREFIX             (IDS + 220)
#define IDS_INI_DIALSUFFIX             (IDS + 221)
#define IDS_INI_ANSWERMODE             (IDS + 222)
#define IDS_INI_MODEMINIT              (IDS + 223)
#define IDS_INI_DISCONNECT             (IDS + 224)
#define IDS_INI_COMMANDOK              (IDS + 225)
#define IDS_INI_COMMANDERROR           (IDS + 226)
#define IDS_INI_CONNECT                (IDS + 227)
#define IDS_INI_BAUDCONNECT            (IDS + 228)
#define IDS_INI_ARQCONNECT             (IDS + 229)
#define IDS_INI_NOCARRIER              (IDS + 230)
#define IDS_INI_NODIALTONE             (IDS + 231)
#define IDS_INI_BUSY                   (IDS + 232)
#define IDS_INI_RINGING                (IDS + 233)

/*
 *  IDS_MODEM_* are modem configurations
 */

#define IDS_MODEM_2224B                (IDS + 260)
#define IDS_MODEM_7404                 (IDS + 261)
#define IDS_MODEM_DTDM                 (IDS + 262)
#define IDS_MODEM_BIZCOMP              (IDS + 263)
#define IDS_MODEM_BYTCOM               (IDS + 264)
#define IDS_MODEM_COMPAQ               (IDS + 265)
#define IDS_MODEM_CONCORD              (IDS + 266)
#define IDS_MODEM_DCA911               (IDS + 267)
#define IDS_MODEM_DCAFAST              (IDS + 268)
#define IDS_MODEM_GRID                 (IDS + 269)
#define IDS_MODEM_HAYES300             (IDS + 270)
#define IDS_MODEM_HAYES1200            (IDS + 271)
#define IDS_MODEM_HAYES2400            (IDS + 272)
#define IDS_MODEM_V2400                (IDS + 273)
#define IDS_MODEM_V9600                (IDS + 274)
#define IDS_MODEM_IBM5841              (IDS + 275)
#define IDS_MODEM_IBMCONV              (IDS + 276)
#define IDS_MODEM_IBM1200              (IDS + 277)
#define IDS_MODEM_MIGENT               (IDS + 278)
#define IDS_MODEM_MULTITECH            (IDS + 279)
#define IDS_MODEM_NOVATION             (IDS + 280)
#define IDS_MODEM_POPCOM               (IDS + 281)
#define IDS_MODEM_PM2400SA             (IDS + 282)
#define IDS_MODEM_RACAL3451            (IDS + 283)
#define IDS_MODEM_RACAL212             (IDS + 284)
#define IDS_MODEM_RIXON212             (IDS + 285)
#define IDS_MODEM_RIXON212A            (IDS + 286)
#define IDS_MODEM_TELEBIT              (IDS + 287)
#define IDS_MODEM_USR212               (IDS + 288)
#define IDS_MODEM_USR2400              (IDS + 289)
#define IDS_MODEM_HST9600              (IDS + 290)
#define IDS_MODEM_VEN1200              (IDS + 291)
#define IDS_MODEM_VEN2400              (IDS + 292)
#define IDS_MODEM_VENEC18K             (IDS + 293)
#define IDS_MODEM_DIRECT               (IDS + 294)
#define IDS_MODEM_CUSTOM               (IDS + 295)

#define IDD_OK                         IDOK
#define IDD_CANCEL                     IDCANCEL

#define IDD                            4096

// open session dialog box

#define IDD_DLGFRAME1                  (IDD + 0)
#define IDD_DLGFRAME2                  (IDD + 1)

#define IDD_BOTTOM                     (IDD + 20)

#define IDD_SESSIONDIRSLIDER           (IDD + 30)
#define IDD_SESSIONDIRLB               (IDD + 31)

#define IDD_NAMEEC                     (IDD + 40)
#define IDD_NUMBEREC                   (IDD + 41)
#define IDD_CONNECTORCB                (IDD + 42)
#define IDD_CONNECTORSETUP             (IDD + 43)
#define IDD_EMULATORCB                 (IDD + 44)
#define IDD_EMULATORSETUP              (IDD + 45)
#define IDD_PROTOCOLCB                 (IDD + 46)
#define IDD_MODEMCB                    (IDD + 47)
#define IDD_MODEMSETUP                 (IDD + 48)

#define IDD_OPTION_NAME                (IDD + 50)
#define IDD_OPTION_NUMBER              (IDD + 51)
#define IDD_OPTION_CONNECTOR           (IDD + 52)
#define IDD_OPTION_SETTINGS            (IDD + 53)
#define IDD_OPTION_LASTCALL            (IDD + 54)
#define IDD_OPTION_TOTALCALLS          (IDD + 55)
#define IDD_OPTION_EMULATOR            (IDD + 56)
#define IDD_OPTION_MODEM               (IDD + 57)
#define IDD_OPTION_PROTOCOL            (IDD + 58)

// modem configuration dialog box

#define IDD_DIALPREFIXEC               (IDD + 60)
#define IDD_DIALSUFFIXEC               (IDD + 61)
#define IDD_ANSWERMODEEC               (IDD + 62)
#define IDD_MODEMINITEC                (IDD + 63)
#define IDD_DISCONNECTEC               (IDD + 64)
#define IDD_COMMANDOKEC                (IDD + 65)
#define IDD_COMMANDERROREC             (IDD + 66)
#define IDD_CONNECTEC                  (IDD + 67)
#define IDD_BAUDCONNECTEC              (IDD + 68)
#define IDD_ARQCONNECTEC               (IDD + 69)
#define IDD_NOCARRIEREC                (IDD + 70)
#define IDD_NODIALTONEEC               (IDD + 71)
#define IDD_BUSYEC                     (IDD + 72)
#define IDD_RINGINGEC                  (IDD + 73)

#define IDD_NAME                       (IDD + 80)
#define IDD_NUMBER                     (IDD + 81)
#define IDD_SCRIPT                     (IDD + 82)
#define IDD_LASTRESULT                 (IDD + 83)
#define IDD_PASSNUMBER                 (IDD + 84)
#define IDD_ELAPSEDTIME                (IDD + 85)
#define IDD_LASTCONNECT                (IDD + 86)
#define IDD_TOTALCALLS                 (IDD + 87)
#define IDD_WAITEC                     (IDD + 88)
#define IDD_PAUSEEC                    (IDD + 89)
#define IDD_DIALTIME                   (IDD + 90)
#define IDD_CALLTIME                   (IDD + 91)

/*
 * Miscellaneous resource IDs
 */

#define SESSIONDIRDLG                  100
#define SESSIONDIRENTRYDLG             101
#define SESSIONDIROPTIONSDLG           102
#define MODEMSETTINGSDLG               103
#define DIALINGDLG                     104

#define CREDITSBMP                     200
#define APACHEBMP                      201

#define DIALUPBMP                      220
#define DIALDNBMP                      221
#define CONNECTUPBMP                   222
#define CONNECTDNBMP                   223
#define EDITUPBMP                      224
#define EDITDNBMP                      225
#define NEWUPBMP                       226
#define NEWDNBMP                       237
#define DELETEUPBMP                    238
#define DELETEDNBMP                    239
#define OPTIONSUPBMP                   240
#define OPTIONSDNBMP                   241
#define CANCELUPBMP                    242
#define CANCELDNBMP                    243

#define RECYCLEUPBMP                   250
#define RECYCLEDNBMP                   251

#define APACHEICON                     300
#define TERMINALICON                   301

#define SESSIONDIRBTNINFO              400
#define DIALINGBTNINFO                 401

/************************************************************************
 * End of File: resource.h
 ***********************************************************************/
