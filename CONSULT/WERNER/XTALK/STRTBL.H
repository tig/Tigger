/*****************************************************************************\
*                                                                             *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   String Table constant definitions (from DIALOGS.H)              *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version of DIALOGS.H.                     *
*             1.1b 05/09/90 PJL Split up DIALOGS.H into two files.			   *
*                                                                             *
\*****************************************************************************/


#define IDS_APPNAME	    1
#define IDS_APPTITLE	    2
#define IDS_UNTITLED	    3
#define IDS_CLIPBRD 	    4
#define IDS_CONTROL 	    5
#define IDS_NOTEPAD 	    6

#define IDS_WELCOME1        10
#define IDS_WELCOME2        11
#define IDS_WELCOME3        12
#define IDS_WELCOME4        13
#define IDS_WELCOME5        14

#define IDS_IFN             18
#define IDS_FNF             19
#define IDS_REF             20
#define IDS_SCC             21
#define IDS_EOF             22
#define IDS_ECF             23
#define IDS_EXT             24

#define IDS_READ_ERROR	    26
#define IDS_WRITE_ERROR     27

#define IDS_WNDCLASS_MAIN	40
#define IDS_WNDCLASS_STATUS	41
#define IDS_WNDCLASS_INFO	42
#define IDS_WNDCLASS_TERM	43
#define IDS_WNDCLASS_MACRO	44
#define IDS_WNDCLASS_CISG	45
#define IDS_WNDCLASS_KERMIT	46
#define IDS_WNDCLASS_COLORBAR	47

#define IDS_EMULATOR_FIRST	50
#define IDS_EMULATOR_VT102	50
#define IDS_EMULATOR_VT52	51
#define IDS_EMULATOR_PCIBM	52
#define IDS_EMULATOR_3101	53
#define IDS_EMULATOR_VIDTEX	54
#define IDS_EMULATOR_LAST	54

#define IDS_PROTOCOL_FIRST	60
#define IDS_PROTOCOL_XTALK	60
#define IDS_PROTOCOL_DART	61
#define IDS_PROTOCOL_MODEM7	62
#define IDS_PROTOCOL_XMODEM	63
#define IDS_PROTOCOL_MODEM1K	64
#define IDS_PROTOCOL_YMODEM	65
#define IDS_PROTOCOL_KERMIT     66
#define IDS_PROTOCOL_CSERVEB    67
#define IDS_PROTOCOL_ZMODEM     68
#define IDS_PROTOCOL_LAST       68

#define IDS_CHRATTR_BLACK       71
#define IDS_CHRATTR_RED         72
#define IDS_CHRATTR_GREEN       73
#define IDS_CHRATTR_YELLOW      74
#define IDS_CHRATTR_BLUE        75
#define IDS_CHRATTR_MAGENTA     76
#define IDS_CHRATTR_CYAN        77
#define IDS_CHRATTR_WHITE       78
#define IDS_CHRATTR_BOLDED      79
#define IDS_CHRATTR_UNDERLINED  80
#define IDS_CHRATTR_SAMPLE      81
#define IDS_CHRATTR_BLINKING    82

#define IDS_STAT_EMULATOR	90
#define IDS_STAT_CONFIG 	91
#define IDS_STAT_INSERT 	94
#define IDS_STAT_KEYPAD 	95
#define IDS_STAT_CLOCK		96

#define IDS_MODEM_FIRST        100
#define IDS_MODEM_2224B        100
#define IDS_MODEM_7404         101
#define IDS_MODEM_DTDM         102
#define IDS_MODEM_BIZCOMP      103
#define IDS_MODEM_BYTCOM       104
#define IDS_MODEM_COMPAQ       105
#define IDS_MODEM_CONCORD      106
#define IDS_MODEM_DCA911       107
#define IDS_MODEM_DCAFAST      108
#define IDS_MODEM_GRID         109
#define IDS_MODEM_HAYES300     110
#define IDS_MODEM_HAYES1200    111
#define IDS_MODEM_HAYES2400    112
#define IDS_MODEM_V2400        113
#define IDS_MODEM_V9600        114
#define IDS_MODEM_IBM5841      115
#define IDS_MODEM_IBMCONV      116
#define IDS_MODEM_IBM1200      117
#define IDS_MODEM_MIGENT       118
#define IDS_MODEM_MULTITECH    119
#define IDS_MODEM_NOVATION     120
#define IDS_MODEM_POPCOM       121
#define IDS_MODEM_PM2400SA		122
#define IDS_MODEM_RACAL3451    123
#define IDS_MODEM_RACAL212     124
#define IDS_MODEM_RIXON212     125
#define IDS_MODEM_RIXON212A    126
#define IDS_MODEM_TELEBIT      127
#define IDS_MODEM_USR212       128
#define IDS_MODEM_USR2400      129
#define IDS_MODEM_HST9600      130
#define IDS_MODEM_VEN1200      131
#define IDS_MODEM_VEN2400      132
#define IDS_MODEM_VENEC18K     133
#define IDS_MODEM_DIRECT       134
#define IDS_MODEM_CUSTOM       135
#define IDS_MODEM_LAST         135

#define MSGDLG_KEYS_CONTROL1   136

#define MSG_LOG_FILE	       300
#define MSG_BECONNECTED        301
#define MSG_LOCALMEMORY        302
#define MSG_NOENTRIES	       303
#define MSG_BADCAPTUREFILE     304
#define MSG_SURETODELETE       305

#define MSGDIA_INIT	       306
#define MSGDIA_WAIT	       307
#define MSGDIA_DIAL	       308
#define MSGINF_COMPILING       309
#define MSGINF_DISCONNECTING   310
#define MSGINF_PLAYBACK        311
#define MSGINF_HELP	       312
#define MSGINF_KERMIT_IEXCH    313
#define MSGINF_KERMIT_SEXCH    314
#define MSGINF_KERMIT_NAME     315
#define MSGINF_KERMIT_RTEXT    316
#define MSGINF_KERMIT_ATTR     317
#define MSGINF_KERMIT_RDATA    318
#define MSGINF_KERMIT_DBAD     319
#define MSGINF_KERMIT_DGOOD    320
#define MSGINF_KERMIT_INVALID  321
#define MSGINF_KERMIT_SFILE    322
#define MSGINF_KERMIT_SDATA    323
#define MSGINF_KERMIT_UBAD     324
#define MSGINF_KERMIT_UGOOD    325
#define MSGINF_KERMIT_ABORTED  326
#define MSGKRM_NOERROR	       327
#define MSGKRM_PROCESSING      328
#define MSGKRM_IDLE	       329
#define MSGMNU_CONNECT	       330
#define MSGMNU_DISCONNECT      331
#define MSGMNU_CPY_FILE        332
#define MSGMNU_CPY_LOG	       333
#define MSGMNU_CPY_NOTES       334
#define MSGMNU_CPY_PRINTER     335
#define MSGMNU_SCR_FILE        336
#define MSGMNU_SCR_LOG	       337
#define MSGMNU_SCR_NOTES       338
#define MSGMNU_SCR_PRINTER     339
#define MSGMNU_SEND	       340
#define MSGMNU_RECEIVE	       341
#define MSGMNU_SCRIPT1	       342
#define MSGMNU_SCRIPT2	       343
#define MSGMNU_CAPTURE1        344
#define MSGMNU_CAPTURE2        345
#define MSGPRT_NOPRINTER       436
#define MSGPRT_PRINTERR        437
#define MSGVSS_NOREALMEM       438
#define MSGVSS_PTREALMEM       439
#define MSGXFR_FILENO          440
#define MSGXFR_FILEOF          441
#define MSGXFR_ALLDONE         442
#define MSGXFR_WAITUP          443
#define MSGXFR_WAITDOWN        444
#define MSGXFR_MODEM7SND       445
#define MSGXFR_MODEM7RCV       446
#define MSGXFR_XMODEMSND       447
#define MSGXFR_XMODEMRCV       448
#define MSGXFR_MODEM1KSND      449
#define MSGXFR_MODEM1KRCV      450
#define MSGXFR_YMODEMSND       451
#define MSGXFR_YMODEMRCV       452
#define MSGXFR_USEXMODEM       455
#define MSGXFR_ASCIITEMP       456
#define MSGXFR_ASCIISND        457
#define MSGXFR_ASCIIRCV        458
#define MSGXFR_ASCIIBUFF       459
#define MSGXFR_ASCIISEND       460
#define MSGXFR_ASCIIPACE       461
#define MSGXFR_ASCIIWAIT       462
#define MSGXFR_CROSSSND        465
#define MSGXFR_CROSSRCV        466
#define MSGXFR_MUSTCLOSE       467
#define MSGXFR_DARTSND         468
#define MSGXFR_DARTRCV         469
#define MSGXFR_QUICKBACT       470
#define MSGXFR_QUICKBSND       471
#define MSGXFR_QUICKBRCV       472
#define MSGXFR_ZMODEMSND       473
#define MSGXFR_ZMODEMRCV       474

#define KRMSTR_LOGOPEN         475

#define KRMSTR_SHOW01          476
#define KRMSTR_SHOW02          477
#define KRMSTR_SHOW03          478
#define KRMSTR_SHOW04          479
#define KRMSTR_SHOW06          481
#define KRMSTR_SHOW08          483
#define KRMSTR_SHOW09          484
#define KRMSTR_SHOW10          485
#define KRMSTR_SHOW11          486
#define KRMSTR_SHOW12          487
#define KRMSTR_SHOW13          488
#define KRMSTR_SHOW14          489
#define KRMSTR_SHOW15          490
#define KRMSTR_SHOW16          491
#define KRMSTR_SHOW17          492
#define KRMSTR_SHOW18          493

#define KRMSTR_STAT01          494
#define KRMSTR_STAT02          495
#define KRMSTR_STAT03          496
#define KRMSTR_STAT04          497
#define KRMSTR_STAT05          498
#define KRMSTR_STAT06          499

#define IDS_ERRDLG_PATIENCE    500
#define IDS_ERRDLG_REDIALCOUNT 501
#define IDS_ERRDLG_REDIALWAIT  502
#define IDS_ERRDLG_MAXERRORS   503
#define IDS_ERRDLG_LWAITCOUNT  504
#define IDS_ERRDLG_TABWIDTH    505
#define IDS_ERRDLG_CWAITDELAY  506
#define IDS_ERRDLG_LWAITDELAY  507
#define IDS_ERRDLG_LINETIME    508
#define IDS_ERRDLG_WORDWRAP    509
#define IDS_ERRDLG_BADDRIVE    510
#define IDS_ERRDLG_MAKEDIR     511
#define IDS_ERRDLG_BADMKDIR    512
#define IDS_ERRDLG_NOHELP      513
#define IDS_ERRDLG_NOFONTS     514
#define IDS_ERRDLG_COM1COM2    515
#define IDS_ERRDLG_COM1COM4    516
#define IDS_ERRDLG_COMMBAD     517
#define IDS_ERRDLG_DISCONNECT1 518
#define IDS_ERRDLG_DISCONNECT2 519

#define IDS_ERRCOM             600
#define IDS_ERRINT             IDS_ERRCOM + 29
#define IDS_ERRIOE             IDS_ERRINT + 57
#define IDS_ERRATH	       IDS_ERRIOE + 12
#define IDS_ERRSYS	       IDS_ERRATH + 7
#define IDS_ERRDOS             IDS_ERRSYS + 15
#define IDS_ERRDIA	       IDS_ERRDOS + 9
#define IDS_ERRMIS	       IDS_ERRDIA + 10
#define IDS_ERRDDE             IDS_ERRMIS + 6
#define IDS_ERRDEV	       IDS_ERRDDE + 8
#define IDS_ERRTRM             IDS_ERRDEV + 14
#define IDS_ERRXFR	       IDS_ERRTRM + 2
#define IDS_ERRKRM             IDS_ERRXFR + 18

/* Help constants */
#define INFO_SYSBOX            160
#define INFO_FILE              161
#define INFO_EDIT              162
#define INFO_MSESSION          163
#define INFO_SETUP             164
#define INFO_USER              165
#define INFO_WINDOW            166
#define INFO_HELP              167
#define INFO_RESTORE           168
#define INFO_MOVE              169
#define INFO_SIZE              170
#define INFO_REDUCE            171
#define INFO_ENLARGE           172
#define INFO_CLOSE             173
#define INFO_NEW               174
#define INFO_OPEN              175
#define INFO_SAVE              176
#define INFO_SAVEAS            177
#define INFO_CAPTURE           178
#define INFO_UPLOAD            179
#define INFO_PAGESETUP         180
#define INFO_PRINT             181
#define INFO_PRINTSETUP        182
#define INFO_SEND              183
#define INFO_RECEIVE           184
#define INFO_EXIT              185
#define INFO_COPY              186
#define INFO_COPYFILE          187
#define INFO_COPYCAPTURE       188
#define INFO_COPYNOTES         189
#define INFO_COPYPRINTER       190
#define INFO_COPYSEND          191
#define INFO_PASTE             192
#define INFO_PASTEUPLOAD       193
#define INFO_SCROLL            194
#define INFO_SEARCHSCROLL      195
#define INFO_CONNECT           196
#define INFO_DIAL              197
#define INFO_EXECUTE           198
#define INFO_NOTES             199
#define INFO_KERMIT            200
#define INFO_PLAYBACK          201
#define INFO_BREAK             202
#define INFO_RESET             203
#define INFO_SESSION           204
#define INFO_COMMUNICATIONS    205
#define INFO_MODEM             206
#define INFO_TERMINAL          207
#define INFO_PROTOCOL          208
#define INFO_ASCII             209
#define INFO_MACROS            210
#define INFO_DISPLAY           211
#define INFO_FONTS             212
#define INFO_SYSTEM            213
#define INFO_MENUNULL          214
#define INFO_MENUMACRO         215
#define INFO_WINDOW_XFER       263
#define INFO_WINDOW_NOTES      264
#define INFO_TOPICS            265
#define INFO_ABOUT             266
// MKL
#define INFO_INDEX	       267
#define INFO_PROC	       268
#define INFO_CMMD	       269
#define INFO_MOUSE	       270
#define INFO_KBD	       271
#define INFO_DDE	       272
#define INFO_HELPHELP	       273
