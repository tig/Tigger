/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Dialog Definition File (DIALOGS.H)                               */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#define ID_OK		    1
#define ID_CANCEL	    2

#define IDOK		    1
#define IDCANCEL	    2

#define ID_EDIT 	    0x0100
#define ID_LISTBOX	    0x0101
#define ID_PATH 	    0x0102
#define ID_NAME 	    0x0103
#define ID_FIND 	    0x0104
#define ID_NEXT 	    0x0105
#define ID_SAVE 	    0x0106
#define ID_ADD		    0x0107
#define ID_DELETE	    0x0108
#define ID_CREATE	    0x0109
#define ID_ABORT	    0x010A
#define ID_MSG1 	    0x010B
#define ID_MSG2 	    0x010C
#define ID_RECALL	    0x010D
#define ID_RESET	    0x010E
#define ID_COPY 	    0x010F
#define ID_CUT		    0x0110
#define ID_PASTE	    0x0111
#define ID_MORE 	    0x0112
#define ID_REPLACE	    0x0113
#define ID_OPT1 	    0x0114
#define ID_OPT2 	    0x0116
#define ID_DEFAULT	    0x0115

#define ID_CONNECT_INIT         0x0430
#define ID_CONNECT_DIAL         0x0431
#define ID_CONNECT_PRI          0x0432
#define ID_CONNECT_1ST          0x0433
#define ID_CONNECT_2ND          0x0434
#define ID_CONNECT_LIGHT1       0x0435
#define ID_CONNECT_LIGHT2       0x0436

#define ID_NEW_RECORD           0x0320
#define ID_NEW_DIARY            0x0321
#define ID_NEW_NOTEPAD          0x0322

#define ID_OPEN_RECORD          0x0324
#define ID_OPEN_DIARY           0x0325
#define ID_OPEN_NOTEPAD         0x0326

#define ID_CLOSE_RECORD         0x0327
#define ID_CLOSE_DIARY          0x0328

#define ID_HELP_TOPICS          0x0360
#define ID_HELP_NEXT            0x0361
#define ID_HELP_PREV            0x0362

#define ID_SEARCH_CREATE        0x0370
#define ID_SEARCH_FILES         0x0371
#define ID_SEARCH_RUN           0x0372
#define ID_SEARCH_EDIT          0x0373
#define ID_SEARCH_DELETE        0x0374
#define ID_SEARCH_CLIENT        0x0375
#define ID_SEARCH_DESC          0x0376
#define ID_SEARCH_LIB           0x0377
#define ID_SEARCH_FILE          0x0378
#define ID_SEARCH_TEXT          0x0379
#define ID_SEARCH_SM            0x037A
#define ID_SEARCH_SD            0x037B
#define ID_SEARCH_SY            0x037C
#define ID_SEARCH_EM            0x037D
#define ID_SEARCH_ED            0x037E
#define ID_SEARCH_EY            0x037F
#define ID_SEARCH_CONNECTORS    0x0380
#define ID_SEARCH_FORMATS       0x0381
#define ID_SEARCH_MODIFY        0x0382
#define ID_SEARCH_LEVEL         0x0383
#define ID_SEARCH_SUMMARY       0x0384
#define ID_SEARCH_SEQUENCE      0x0385

#define ID_SETUP_FIRST          0x0300
#define ID_SETUP_ID             0x0300
#define ID_SETUP_BELL           0x0301
#define ID_SETUP_STATUS         0x0302
#define ID_SETUP_MACRO          0x0303
#define ID_SETUP_INFO           0x0304
#define ID_SETUP_FASTPRINT      0x0305
/*
#define ID_SETUP_LEXIS2000	0x0306
*/
#define ID_SETUP_COM1		0x0307
#define ID_SETUP_COM2		0x0308
#define ID_SETUP_DIRECT 	0x0309
#define ID_SETUP_PBX		0x030A
#define ID_SETUP_MODEMS 	0x030B
#define ID_SETUP_EVEN		0x030C
#define ID_SETUP_ODD		0x030D
#define ID_SETUP_NONE		0x030E
#define ID_SETUP_TONE		0x030F
#define ID_SETUP_PULSE		0x0310
#define ID_SETUP_MANUAL 	0x0311
#define ID_SETUP_NEWMODEM	0x0312
#define ID_SETUP_PRIMARY	0x0313
#define ID_SETUP_1STALT 	0x0314
#define ID_SETUP_2NDALT 	0x0315
#define ID_SETUP_NETWORK	0x0316
#define ID_SETUP_PREFIX 	0x0317
#define ID_SETUP_NUMBER 	0x0318
#define ID_SETUP_SPEED		0x0319
#define ID_SETUP_NODE		0x031A
#define ID_SETUP_MESSAGE	0x031B
#define ID_SETUP_RECORD 	0x031C
#define ID_SETUP_NOTEPAD	0x031D
#define ID_SETUP_MAPS		0x031E
#define ID_SETUP_DOC		0x031F
#define ID_SETUP_LAST		0x0320
#define ID_SETUP_NETWORK1	0x0321
#define ID_SETUP_NETWORK2	0x0322
#define ID_SETUP_NETWORK3	0x0323
#define ID_SETUP_PREFIX1	0x0324
#define ID_SETUP_PREFIX2	0x0325
#define ID_SETUP_PREFIX3	0x0326
#define ID_SETUP_NUMBER1	0x0327
#define ID_SETUP_NUMBER2	0x0328
#define ID_SETUP_NUMBER3	0x0329
#define ID_SETUP_SPEED1 	0x032A
#define ID_SETUP_SPEED2 	0x032B
#define ID_SETUP_SPEED3 	0x032C
#define ID_SETUP_NODE1		0x032D
#define ID_SETUP_NODE2		0x032E
#define ID_SETUP_NODE3		0x032F

#define ID_CITE_AUTOCITE        0x0385
#define ID_CITE_SHEPARD         0x0386
#define ID_CITE_CURRENT         0x0387
#define ID_CITE_USER            0x0388

#define ID_PRINT_FULL           0x0550
#define ID_PRINT_KWIC           0x0551
#define ID_PRINT_VKWIC          0x0552
#define ID_PRINT_CITE           0x0553
#define ID_PRINT_SEGMENTS       0x0554
#define ID_PRINT_FILE           0x0555
#define ID_PRINT_PRINTER        0x0556
#define ID_PRINT_STOP           0x0557
#define ID_PRINT_APPEND         0x0558
#define ID_PRINT_ERASE          0x0559
#define ID_PRINT_NUMBER         0x055A
#define ID_PRINT_RECORD         0x055B
#define ID_PRINT_DIARY          0x055C
#define ID_PRINT_NOTEPAD        0x055D
#define ID_PRINT_DOCUMENT       0x055F

#define ID_RESUME_PRIOR         0x03A0
#define ID_RESUME_LEXIS         0x03A1
#define ID_RESUME_AUTOCITE      0x03A2
#define ID_RESUME_SHEPARD       0x03A3
#define ID_RESUME_LEXSEE        0x03A4
#define ID_RESUME_LEXSTAT       0x03A5

#define ID_FONT_FONTLIST	0x0500
#define ID_FONT_FONTTEXT	0x0501
#define ID_FONT_SIZELIST	0x0502
#define ID_FONT_SIZETEXT	0x0503
#define ID_FONT_TYPE		0x0504

#define ID_DISPLAY_STATUS	0x0510
#define ID_DISPLAY_MACRO	0x0511
#define ID_DISPLAY_INFO 	0x0512
#define ID_DISPLAY_VERT 	0x0513
#define ID_DISPLAY_HORZ 	0x0514
#define ID_DISPLAY_INPUT	0x0515
#define ID_DISPLAY_NOBLINK	0x0516
#define ID_DISPLAY_REVERSE	0x0517
#define ID_DISPLAY_ONECHAR	0x0518
#define ID_DISPLAY_SLOWCUR	0x0519
#define ID_DISPLAY_FOREGROUND	0x051A
#define ID_DISPLAY_BACKGROUND	0x051B
#define ID_DISPLAY_BOLDING	0x051C
#define ID_DISPLAY_BLINKING	0x051D
#define ID_DISPLAY_TEXT 	0x051E
#define ID_DISPLAY_BOLD 	0x051F
#define ID_DISPLAY_BLINK	0x0520

#define ID_DIAL_MANUAL		0x0600
#define ID_DIAL_NUMBER		0x0601
#define ID_DIAL_STATUS		0x0602
#define ID_DIAL_TITLE		0x0603
#define ID_DIAL_WATCH		0x0604

#define ID_DIARY_ON             0x0770
#define ID_DIARY_OFF            0x0771
#define ID_DIARY_SNAP           0x0772

#define ID_PRINT_HEADER 	0x0890
#define ID_PRINT_FOOTER 	0x0891

#define ID_REPORT_FILE          0x0892
#define ID_REPORT_PRINT         0x0893
#define ID_REPORT_LIGHT1        0x0894
#define ID_REPORT_LIGHT2        0x0895


#define ID_ASCII_LWAIT_NONE	0x0930
#define ID_ASCII_LWAIT_ECHO	0x0931
#define ID_ASCII_LWAIT_PROMPT	0x0932
#define ID_ASCII_LWAIT_COUNT	0x0933
#define ID_ASCII_LWAIT_DELAY	0x0934
#define ID_ASCII_LWAIT_TEXT	0x0935
#define ID_ASCII_CWAIT_NONE	0x0936
#define ID_ASCII_CWAIT_ECHO	0x0937
#define ID_ASCII_CWAIT_DELAY	0x0938
#define ID_ASCII_CWAIT_TEXT	0x0939
#define ID_ASCII_LINEDELIM	0x093A
#define ID_ASCII_BLANKEX	0x093B
#define ID_ASCII_TABEX_YES	0x093C
#define ID_ASCII_TABEX_NO	0x093D
#define ID_ASCII_LINETIME	0x093E
#define ID_ASCII_WORDWRAP	0x093F

#define ID_GLOBAL_STARTUP	0x0A00
#define ID_GLOBAL_LDNUMBER	0x0A01
#define ID_GLOBAL_OUTNUMBER	0x0A02
#define ID_GLOBAL_MISC		0x0A03
#define ID_GLOBAL_CODESAVE	0x0A04
#define ID_GLOBAL_BACKUPS	0x0A05
#define ID_GLOBAL_SECRET	0x0A06
#define ID_GLOBAL_XWP		0x0A07
#define ID_GLOBAL_XWS		0x0A08
#define ID_GLOBAL_FIL		0x0A09
#define ID_GLOBAL_NONE		0x0A0A
#define ID_GLOBAL_16K		0x0A0B
#define ID_GLOBAL_32K		0x0A0C
#define ID_GLOBAL_64K		0x0A0D
#define ID_GLOBAL_CP2K		0x0A0E
#define ID_GLOBAL_CP4K		0x0A0F
#define ID_GLOBAL_CP8K		0x0A10
#define ID_GLOBAL_CP12K 	0x0A11
#define ID_GLOBAL_CP16K 	0x0A12

#define ID_ERROR_ERROR		0x0A40
#define ID_ERROR_TEXT		0x0A41
#define ID_ERROR_LINE		0x0A42
#define ID_ERROR_LINENO 	0x0A43

#define IDS_APPNAME	    1
#define IDS_APPTITLE	    2
#define IDS_UNTITLED	    3

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

#define IDS_CHRATTR_BLACK	70
#define IDS_CHRATTR_RED 	71
#define IDS_CHRATTR_GREEN	72
#define IDS_CHRATTR_YELLOW	73
#define IDS_CHRATTR_BLUE	74
#define IDS_CHRATTR_MAGENTA	75
#define IDS_CHRATTR_CYAN	76
#define IDS_CHRATTR_WHITE	77
#define IDS_CHRATTR_BOLDED	78
#define IDS_CHRATTR_UNDERLINED	79
#define IDS_CHRATTR_SAMPLE	80
#define IDS_CHRATTR_BLINKING	81

#define IDS_MACRO_FIRST        100
#define IDS_MACRO_NEXT_PAGE    100
#define IDS_MACRO_NEXT_DOC     101
#define IDS_MACRO_NEW_SEARCH   102
#define IDS_MACRO_CITATORS     103
#define IDS_MACRO_PRINT_DOC    104
#define IDS_MACRO_CHG_LIB      105
#define IDS_MACRO_RESUME       106
#define IDS_MACRO_PREV_PAGE    107
#define IDS_MACRO_PREV_DOC     108
#define IDS_MACRO_LEXSTAT      109
#define IDS_MACRO_LEXSEE       110
#define IDS_MACRO_PRINT_ALL    111
#define IDS_MACRO_CHG_FILE     112
#define IDS_MACRO_STOP         113
#define IDS_MACRO_STOP1        114
#define IDS_MACRO_STOP2        115
#define IDS_MACRO_STOP3        116
#define IDS_MACRO_STOP4        117
#define IDS_MACRO_STOP5        118
#define IDS_MACRO_STOP6        119
#define IDS_MACRO_STOP7        120
#define IDS_MACRO_LAST         120

#define IDS_MODEM_FIRST        130
#define IDS_MODEM_ATT2224      130
#define IDS_MODEM_HAYES1200    131
#define IDS_MODEM_HAYES1200B   132
#define IDS_MODEM_HAYES2400    133
#define IDS_MODEM_HAYES2400B   134
#define IDS_MODEM_LEADING      135
#define IDS_MODEM_MT212AH2     136
#define IDS_MODEM_MT212HC      137
#define IDS_MODEM_MT224PC      138
#define IDS_MODEM_POPCOM       139
#define IDS_MODEM_PROMODEM     140
#define IDS_MODEM_VA3451       141
#define IDS_MODEM_RACAL1200    142
#define IDS_MODEM_RACAL2400    143
#define IDS_MODEM_USR2400      144
#define IDS_MODEM_USR9600      145
#define IDS_MODEM_LAST         145

#define IDS_DATA_PROFILE       150
#define IDS_DATA_CONFIG        151
#define IDS_DATA_WORKFILE      152
#define IDS_DATA_DATA          153
#define IDS_DATA_NETWORK       154
#define IDS_DATA_MODEMS        155
#define IDS_DATA_MENUS         156
#define IDS_DATA_SCRIPTS       157
#define IDS_DATA_LOGON         158
#define IDS_DATA_MODEM         159
#define IDS_DATA_NETWORK1      160
#define IDS_DATA_NETWORK2      161
#define IDS_DATA_NETWORK3      162
#define IDS_DATA_PREFIX1       163
#define IDS_DATA_PREFIX2       164
#define IDS_DATA_PREFIX3       165
#define IDS_DATA_NUMBER1       166
#define IDS_DATA_NUMBER2       167
#define IDS_DATA_NUMBER3       168
#define IDS_DATA_NODE1         169
#define IDS_DATA_NODE2         170
#define IDS_DATA_NODE3         171
#define IDS_DATA_FACE          172
#define IDS_DATA_BELL          173
#define IDS_DATA_PORT          174
#define IDS_DATA_HARDWIRE      175
#define IDS_DATA_PBX           176
#define IDS_DATA_DIAL          177
#define IDS_DATA_PARITY        178
#define IDS_DATA_SPEED1        179
#define IDS_DATA_SPEED2        180
#define IDS_DATA_SPEED3        181
#define IDS_DATA_PITCH         182
#define IDS_DATA_CHARSET       183
#define IDS_DATA_HEIGHT        184
#define IDS_DATA_WIDTH         185
#define IDS_DATA_RECORD        186
#define IDS_DATA_NOTEPAD       187
#define IDS_DATA_MAPS          188
#define IDS_DATA_DOC           189
#define IDS_DATA_HEADER        190
#define IDS_DATA_FOOTER        191
#define IDS_DATA_STATUS        192
#define IDS_DATA_MACRO         193
#define IDS_DATA_INFO          194
#define IDS_DATA_FASTPRINT     195
/*
#define IDS_DATA_LEXIS2000     196
*/

#define IDS_INFO_OFFLINE       197
#define IDS_INFO_MANUAL        198
#define IDS_INFO_MANUALP       199
#define IDS_INFO_SIGNON        200
#define IDS_INFO_SIGNONP       201
#define IDS_INFO_PROCESS       202
#define IDS_INFO_READY	       203
#define IDS_INFO_PRINTING      204
#define IDS_INFO_PRINTINGP     205
#define IDS_INFO_SWITCHING     206
#define IDS_INFO_SWITCHINGP    207
#define IDS_INFO_USERID        208
#define IDS_INFO_USERIDP       209
#define IDS_INFO_PASSWORD      210
#define IDS_INFO_PASSWORDP     211

#define INFO_SYSBOX            212
#define INFO_MFILE             213
#define INFO_EDIT              214
#define INFO_SEARCH            215
#define INFO_VIEW              216
#define INFO_FORMATS           217
#define INFO_CITATORS          218
#define INFO_DELIVERY          219
#define INFO_INFO              220
#define INFO_HELP              221
#define INFO_RESTORE           222
#define INFO_MOVE              223
#define INFO_SIZE              224
#define INFO_REDUCE            225
#define INFO_ENLARGE           226
#define INFO_CLOSE             227
#define INFO_NEW               228
#define INFO_OPEN              229
#define INFO_CLOSEFILE         230
#define INFO_PRINT	       231
#define INFO_RECORDING	       232
#define INFO_DIARY	       233
#define INFO_PRINTING	       234
#define INFO_SETUP	       235
#define INFO_PAGESETUP	       236
#define INFO_PRINTSETUP        237
#define INFO_HYPERROM          238
#define INFO_SIGNON	       239
#define INFO_EXIT	       240
#define INFO_COPY	       241
#define INFO_COPYDIARY	       242
#define INFO_PASTE	       243
#define INFO_NEWSEARCH	       244
#define INFO_SEARCHMODIFY      245
#define INFO_SEARCHLEVEL       246
#define INFO_LIBRARY	       247
#define INFO_FILE	       248
#define INFO_CLIENT	       249
#define INFO_ECLIPSE	       250
#define INFO_ECLIPSERECALL     251
#define INFO_SEARCHDISPLAY     252
#define INFO_STOP	       253
#define INFO_NEXTPAGE	       254
#define INFO_PREVPAGE	       255
#define INFO_NEXTDOC	       256
#define INFO_PREVDOC	       257
#define INFO_GOTOPAGE	       258
#define INFO_GOTODOC	       259
#define INFO_FIRSTPAGE	       260
#define INFO_FIRSTDOC	       261
#define INFO_KWIC	       262
#define INFO_FULL	       263
#define INFO_CITE	       264
#define INFO_VARKWIC	       265
#define INFO_SEGMENTS	       266
#define INFO_SEQUENCE	       267
#define INFO_ENTERSTAR	       268
#define INFO_EXITSTAR	       269
#define INFO_FONTS	       270
#define INFO_AUTOCITE	       271
#define INFO_SHEPARD	       272
#define INFO_LEXSTAT	       273
#define INFO_LEXSEE	       274
#define INFO_BROWSE	       275
#define INFO_RESUME	       276
#define INFO_SELECTSERVICE     277
#define INFO_EXITSERVICE       278
#define INFO_PRINTSCREEN       279
#define INFO_PRINTDOC	       280
#define INFO_PRINTALL	       281
#define INFO_TIME	       282
#define INFO_REQUEST	       283
#define INFO_PAGES	       284
#define INFO_DEBUT	       285
#define INFO_GENERAL	       286
#define INFO_TUTORIAL	       287
#define INFO_LEXISHELP	       288
#define INFO_ABOUT	       289

#define IDS_MSG_BADDOCFILE     290
#define IDS_MSG_SFILEEXIST     291
#define IDS_MSG_FILEXIST       292
#define IDS_MSG_BADOPEN        293
#define IDS_MSG_BADNAME        294
#define IDS_MSG_DISKFULL       295
#define IDS_MSG_BADCOMM        296
#define IDS_MSG_SPEEDHI        297
#define IDS_MSG_SPEEDBAD1      298
#define IDS_MSG_SPEEDBAD2      299
#define IDS_MSG_SPEEDBAD3      300
#define IDS_MSG_NOPRINTERS     301
#define IDS_MSG_PREFIXBAD      302
#define IDS_MSG_NUMBERBAD      303
#define IDS_MSG_CALLFAILED     304
#define IDS_MSG_NOCFGFILE      305
#define IDS_MSG_BADINIFILE     306

#define MSG_LOG_FILE           307
#define MSG_BECONNECTED        308
#define MSG_LOCALMEMORY        309
#define MSG_NOENTRIES          310
#define MSG_BADCAPTUREFILE     311
#define MSG_SURETODELETE       312

#define MSGDIA_INIT            313
#define MSGDIA_WAIT            314
#define MSGDIA_DIAL            315
#define MSGINF_COMPILING       316
#define MSGINF_PLAYBACK        317
#define MSGMNU_CONNECT         318
#define MSGMNU_DISCONNECT      319
#define MSGMNU_CPY_FILE        320
#define MSGMNU_CPY_LOG         321
#define MSGMNU_CPY_PRINTER     322
#define MSGMNU_SCR_FILE        323
#define MSGMNU_SCR_LOG         324
#define MSGMNU_SCR_PRINTER     325
#define MSGPRT_NOPRINTER       326

#define IDH_OK                 398
#define IDH_CANCEL             399

#define IDH_SETUP_FIRST        400
#define IDH_SETUP_ID           400
#define IDH_SETUP_BELL         401
#define IDH_SETUP_STATUS       402
#define IDH_SETUP_MACRO        403
#define IDH_SETUP_INFO         404
#define IDH_SETUP_FASTPRINT    405
/*
#define IDH_SETUP_LEXIS2000    406
*/
#define IDH_SETUP_COM1	       407
#define IDH_SETUP_COM2	       408
#define IDH_SETUP_DIRECT       409
#define IDH_SETUP_PBX	       410
#define IDH_SETUP_MODEMS       411
#define IDH_SETUP_EVEN	       412
#define IDH_SETUP_ODD	       413
#define IDH_SETUP_NONE	       414
#define IDH_SETUP_TONE	       415
#define IDH_SETUP_PULSE        416
#define IDH_SETUP_MANUAL       417
#define IDH_SETUP_NEWMODEM     418
#define IDH_SETUP_PRIMARY      419
#define IDH_SETUP_1STALT       420
#define IDH_SETUP_2NDALT       421
#define IDH_SETUP_NETWORK      422
#define IDH_SETUP_PREFIX       423
#define IDH_SETUP_NUMBER       424
#define IDH_SETUP_SPEED        425
#define IDH_SETUP_NODE	       426
#define IDH_SETUP_MESSAGE      427
#define IDH_SETUP_RECORD       428
#define IDH_SETUP_NOTEPAD      429
#define IDH_SETUP_MAPS	       430
#define IDH_SETUP_DOC	       431
#define IDH_SETUP_LAST	       431
