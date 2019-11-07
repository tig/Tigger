/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  res.h
 *
 ***********************************************************************/

/* dialog boxes */
// COMMDLG stuff (0x0400 - 0x0500 and 0x6000 and up)
/*****************************************************************************\
*                                                                             *
* dlgs.h -      Common dialog's dialog element ID numbers                     *
*                                                                             *
*               Version 1.0                                                   *
*                                                                             *
*               Copyright (c) 1992, Microsoft Corp.  All rights reserved.     *
*                                                                             *
\*****************************************************************************/

#ifndef _INC_DLGS
#define _INC_DLGS

//#define ctlFirst    0x0400
//#define ctlLast     0x04ff
    /* Push buttons */
#define psh1        0x0400
#define psh2        0x0401
#define psh3        0x0402
#define psh4        0x0403
#define psh5        0x0404
#define psh6        0x0405
#define psh7        0x0406
#define psh8        0x0407
#define psh9        0x0408
#define psh10       0x0409
#define psh11       0x040a
#define psh12       0x040b
#define psh13       0x040c
#define psh14       0x040d
#define psh15       0x040e
#define pshHelp     psh15
#define psh16       0x040f
    /* Checkboxes */
#define chx1        0x0410
#define chx2        0x0411
#define chx3        0x0412
#define chx4        0x0413
#define chx5        0x0414
#define chx6        0x0415
#define chx7        0x0416
#define chx8        0x0417
#define chx9        0x0418
#define chx10       0x0419
#define chx11       0x041a
#define chx12       0x041b
#define chx13       0x041c
#define chx14       0x041d
#define chx15       0x041e
#define chx16       0x041f
    /* Radio buttons */
#define rad1        0x0420
#define rad2        0x0421
#define rad3        0x0422
#define rad4        0x0423
#define rad5        0x0424
#define rad6        0x0425
#define rad7        0x0426
#define rad8        0x0427
#define rad9        0x0428
#define rad10       0x0429
#define rad11       0x042a
#define rad12       0x042b
#define rad13       0x042c
#define rad14       0x042d
#define rad15       0x042e
#define rad16       0x042f
    /* Groups, frames, rectangles, and icons */
#define grp1        0x0430
#define grp2        0x0431
#define grp3        0x0432
#define grp4        0x0433
#define frm1        0x0434
#define frm2        0x0435
#define frm3        0x0436
#define frm4        0x0437
#define rct1        0x0438
#define rct2        0x0439
#define rct3        0x043a
#define rct4        0x043b
#define ico1        0x043c
#define ico2        0x043d
#define ico3        0x043e
#define ico4        0x043f
    /* Static text */
#define stc1        0x0440
#define stc2        0x0441
#define stc3        0x0442
#define stc4        0x0443
#define stc5        0x0444
#define stc6        0x0445
#define stc7        0x0446
#define stc8        0x0447
#define stc9        0x0448
#define stc10       0x0449
#define stc11       0x044a
#define stc12       0x044b
#define stc13       0x044c
#define stc14       0x044d
#define stc15       0x044e
#define stc16       0x044f
#define stc17       0x0450
#define stc18       0x0451
#define stc19       0x0452
#define stc20       0x0453
#define stc21       0x0454
#define stc22       0x0455
#define stc23       0x0456
#define stc24       0x0457
#define stc25       0x0458
#define stc26       0x0459
#define stc27       0x045a
#define stc28       0x045b
#define stc29       0x045c
#define stc30       0x045d
#define stc31       0x045e
#define stc32       0x045f
    /* Listboxes */
#define lst1        0x0460
#define lst2        0x0461
#define lst3        0x0462
#define lst4        0x0463
#define lst5        0x0464
#define lst6        0x0465
#define lst7        0x0466
#define lst8        0x0467
#define lst9        0x0468
#define lst10       0x0469
#define lst11       0x046a
#define lst12       0x046b
#define lst13       0x046c
#define lst14       0x046d
#define lst15       0x046e
#define lst16       0x046f
    /* Combo boxes */
#define cmb1        0x0470
#define cmb2        0x0471
#define cmb3        0x0472
#define cmb4        0x0473
#define cmb5        0x0474
#define cmb6        0x0475
#define cmb7        0x0476
#define cmb8        0x0477
#define cmb9        0x0478
#define cmb10       0x0479
#define cmb11       0x047a
#define cmb12       0x047b
#define cmb13       0x047c
#define cmb14       0x047d
#define cmb15       0x047e
#define cmb16       0x047f
    /* Edit controls */
#define edt1        0x0480
#define edt2        0x0481
#define edt3        0x0482
#define edt4        0x0483
#define edt5        0x0484
#define edt6        0x0485
#define edt7        0x0486
#define edt8        0x0487
#define edt9        0x0488
#define edt10       0x0489
#define edt11       0x048a
#define edt12       0x048b
#define edt13       0x048c
#define edt14       0x048d
#define edt15       0x048e
#define edt16       0x048f
    /* Scroll bars */
#define scr1        0x0490
#define scr2        0x0491
#define scr3        0x0492
#define scr4        0x0493
#define scr5        0x0494
#define scr6        0x0495
#define scr7        0x0496
#define scr8        0x0497

/* These dialog resource ordinals really start at 0x0600, but the
 * RC Compiler can't handle hex for resource IDs, hence the decimal.
 */
#define FILEOPENORD      1536
#define MULTIFILEOPENORD 1537
#define PRINTDLGORD      1538
#define PRNSETUPDLGORD   1539
#define FINDDLGORD       1540
#define REPLACEDLGORD    1541
#define FONTDLGORD       1542
#define FORMATDLGORD31   1543
#define FORMATDLGORD30   1544

#endif  /* !_INC_DLGS */

#define MNU_WINFILE                 75      // ID of the WFX menu

#define IDM_PRINT                   20
#define IDM_PAGESETUP               21
#define IDM_PRINTERSETUP            22
#define IDM_OPTIONS                 23
#define IDM_ABOUT                   24
#define IDM_HELP                    25
#define IDM_HELPONHELP              26
#define IDM_ASSOCIATE               27

#define IDB_WINPRINT                21

/* These are the ids for WinFiles File menu.
 *
 * We use the id's from 118-199 for ours since it appears that
 * winfile does not use them.
 *
 * Actually, to be safe, we start at 125
 */
#define IDM_WF_FILE                0
#define IDM_WF_OPEN                101
#define IDM_WF_PRINT               102
#define IDM_WF_ASSOCIATE           103
#define IDM_WF_SEARCH              104
#define IDM_WF_RUN                 105
#define IDM_WF_MOVE                106
#define IDM_WF_COPY                107
#define IDM_WF_DELETE              108
#define IDM_WF_RENAME              109
#define IDM_WF_ATTRIBS             110
#define IDM_WF_MAKEDIR             111
#define IDM_WF_SELALL              112
#define IDM_WF_DESELALL            113
#define IDM_WF_UNDO                114
#define IDM_WF_EXIT                115
#define IDM_WF_SELECT              116
#define IDM_WF_UNDELETE            117

#define IDM_WP_PRINT               (IDM_PRINT + 105)           // 125
#define IDM_WP_PAGESETUP           (IDM_PAGESETUP  + 106)
#define IDM_WP_PRINTERSETUP        (IDM_PRINTERSETUP + 107)
#define IDM_WP_ASSOCIATE           (IDM_ASSOCIATE + 108)


#define DLG_HFCONFIGURE             200
#define DLG_PAGESETUP               201
#define DLG_PRINT                   202
#define DLG_PRNSETUP                203
#define DLG_PRINTSTATUS             204
#define DLG_FTDCONFIGURE            205
#define DLG_FTD_RAWTEXT             206
#define DLG_ASSOCIATE               207
#define DLG_FILETYPE                208


#define IDD_TEXT1                   301
#define IDD_TEXT2                   302
#define IDD_TEXT3                   303
#define IDD_HELP                    304
#define IDD_PAGENUMBER              305
#define IDD_NUMBEROFPAGES           306
#define IDD_CURRENTDATE             307
#define IDD_CURRENTTIME             308
#define IDD_REVISEDDATE             309
#define IDD_REVISEDTIME             310
#define IDD_FULLPATHNAME            311
#define IDD_LEFT_H                  312
#define IDD_CENTER_H                313
#define IDD_RIGHT_H                 314
#define IDD_LEFT_F                  315
#define IDD_CENTER_F                316
#define IDD_RIGHT_F                 317

#define IDD_LINE1                   318
#define IDD_LINE2                   319
#define IDD_LINE3                   320
#define IDD_LINE4                   321

#define IDD_CURPRINTER              322
#define IDD_PORTRAIT                323
#define IDD_LANDSCAPE               324
#define IDD_ORIENT                  325
#define IDD_LEFTMARGIN              326
#define IDD_TOPMARGIN               327
#define IDD_RIGHTMARGIN             328
#define IDD_BOTTOMMARGIN            329

#define IDD_ONEPAGEUP               330
#define IDD_TWOPAGESUP              331
#define IDD_FOURPAGESUP             332
#define IDD_CUSTOM                  333

#define IDD_ROWS                    334
#define IDD_COLUMNS                 335
#define IDD_CHOOSEPRINTER           336
#define IDD_FONT                    5003
#define IDD_DLG_HF                  338
#define IDD_DLG_PRNSETUP            339
#define IDD_SAMPLE                  340
#define IDD_NOORIENT                341
#define IDD_WINPRINT                342

#define IDD_HORZSEP                 343
#define IDD_VERTSEP                 344
#define IDD_SHADEBORDERS            345
#define IDD_BORDERLINES             346

#define IDD_ROWS_MU                 347
#define IDD_COLUMNS_MU              348

#define IDD_FILETYPEDRIVER          349
#define IDD_FTD                     350
#define IDD_LINENUMBERS             5002
#define IDD_FONT_LBL                5004
#define IDD_EXPANDTABS              5000
#define IDD_SPACES                  5001

#define IDD_EXTENSION               356
#define IDD_FILETYPES               357
#define IDD_NEW                     358
#define IDD_MODIFY                  359
#define IDD_COPY                    360
#define IDD_DELETE                  361

#define IDD_IDENTIFIER              362
#define IDD_FILETYPE                363
#define IDD_BROWSE                  364
#define IDD_PRINT_COMMAND           365
#define IDD_PRINT_USESDDE           366
#define IDD_PRINT_DDEMESSAGE        367
#define IDD_PRINT_APPLICATION       368
#define IDD_PRINT_NOTRUNNING        369
#define IDD_PRINT_TOPIC             370
#define IDD_PRINT_DDEMESSAGE_LBL    371
#define IDD_PRINT_APPLICATION_LBL   372
#define IDD_PRINT_NOTRUNNING_LBL    373
#define IDD_PRINT_TOPIC_LBL         374
#define IDD_OPEN_COMMAND            375
#define IDD_OPEN_USESDDE            376
#define IDD_OPEN_DDEMESSAGE         377
#define IDD_OPEN_APPLICATION        378
#define IDD_OPEN_NOTRUNNING         379
#define IDD_OPEN_TOPIC              380
#define IDD_OPEN_DDEMESSAGE_LBL     381
#define IDD_OPEN_APPLICATION_LBL    382
#define IDD_OPEN_NOTRUNNING_LBL     383
#define IDD_OPEN_TOPIC_LBL          384
#define IDD_USE_WINPRINT            385
#define IDD_PRINT_COMMAND_LBL       386
#define IDD_IDENTIFIER_STATIC       209
#define IDD_ADVANCED                5005
#define DLG_FTD_BITMAP              210
#define IDD_BESTFIT                 5100
#define IDD_STRETCHTOPAGE           5101
#define IDD_SCALEXANDY              5102
#define IDD_XSCALE                  5104
#define IDD_YSCALE                  5106
#define IDD_USEPRINTERRES           5107
