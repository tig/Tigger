/************************************************************************
 *
 *     Project:  WinPrint 1.4
 *
 *      Module:  dlgs.h
 *
 *     Remarks:  Dialog resource ids
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#ifndef _DLGS_H_
#define _DLGS_H_


#define  STR_APPNAME	      500
#define  STR_DESCRIPT	   501
#define  STR_COPYRIGHT	   502
#define  STR_LICENCE	      503
#define  STR_ADDRESS1	   504
#define  STR_ADDRESS2	   505
#define  STR_ADDRESS3	   506

/* dialog boxes */
/*---------------------------------------------------------------------------*/
/* Dlgs.h : UI dialog header                                                 */
/*                                                                           */
/* Copyright (c) Microsoft Corporation, 1990-                                */
/*---------------------------------------------------------------------------*/

/*----Constants--------------------------------------------------------------*/
/*#define ctlFirst    0x0400
 *#define ctlLast     0x04ff
 */
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
#define chx16       0x041d
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

#define  IDD_FNAME	      2000
#define  IDD_CHOOSELIST	   2001
#define  IDD_SELECT	      2002
#define  IDD_REMOVE	      2003
#define  IDD_PRINT	      2004
#define  IDD_HEADER	      2005
#define  IDD_FOOTER	      2006
#define  IDD_LEFTMAR	      2007
#define  IDD_RIGHTMAR	   2008
#define  IDD_TOPMAR	      2009
#define  IDD_BOTMAR	      2010
#define  IDD_CHARS	      2011
#define  IDD_LINES	      2012
#define  IDD_TABS	         2013
#define  IDD_TABSIZE	      2014
#define  IDD_STRIP	      2015
#define  IDD_FONT	         2017
#define  IDD_FPATH	      2018
#define  IDD_SELECTLIST	   2019
#define  IDD_ONEJOB	      2020
#define  IDD_INFO	         2021
#define  IDD_PAGEDIM	      2025
#define  IDD_PRINTER	      2026
#define  IDD_HIDDENLIST	   2027
#define  IDD_TEMPLIST	   2028
#define  IDD_TEMPPATH	   2029
#define  IDD_INCH	         2030
#define  IDD_CM	         2031
#define  IDD_HFNAME	      2033
#define  IDD_HPAGENUM      2034
#define  IDD_HDATE	      2035
#define  IDD_HRDATE	      2036
#define  IDD_HPDATE	      2037
#define  IDD_FFNAME	      2038
#define  IDD_FPAGENUM      2039
#define  IDD_FDATE	      2040
#define  IDD_FRDATE	      2041
#define  IDD_FPDATE	      2042
#define  IDD_HFMARGINS	   2046
#define  IDD_DRAFTMODE	   2049
#define  IDD_MINIMIZE	   2050
#define  IDD_CLOSE	      2051
#define  IDD_CMDLINE	      2053
#define  IDD_NAME	         2057
#define  IDD_REGNUMBER	   2058
#define  IDD_WINPRINT_ICO	2059
#define  IDD_USERNAME      2113
#define IDD_NUMCOLS                 2200
#define IDD_NOSEPARATOR             2201
#define IDD_GRAPHSEPARATOR          2202
#define IDD_TEXTSEPARATOR           2203
#define IDD_COLSPACE                2204
#define IDD_PORTRAIT                2205
#define IDD_LANDSCAPE               2206
#define IDD_ORIENT                  2207


#define  IDD_BOX0	         3000
#define  IDD_BOX1	         3001
#define  IDD_BOX2	         3002


#define DLG_TEXTOPTIONS             5000
#define DLG_ABOUTBOX                5001
#define DLG_HFOPTIONS               5002
#define DLG_OPENCONFIG              5003
#define DLG_PRINTING                5004
#define DLG_SAVECONFIG              5005
#define DLG_SHAREWARE               5006
#define DLG_MAIN                    5007
#define DLG_LAYOUT                  5008


#define  IDM_ABOUT   	101
#define  IDM_OPEN	      102
#define  IDM_SAVE	      103
#define  IDM_EXIT	      104
#define  IDM_HELP	      105
#define  IDM_PRINTER	   106
#define  IDM_HEADFOOT	107
#define  IDM_OPTIONS	   109
#define  IDM_PAGELAYOUT 110
#define  IDM_CANCEL     111  /* cancel on system menu */
#define  IDM_HELPONHELP  112
#define  IDM_MINIMIZEONPRINT 113
#define  IDM_CLOSEWHENDONE 114
#define  IDM_SEPARATEJOBS  115

#define IDD_TEXTFONT                5009
#define IDD_HEADERFONT              5010
#define IDD_CHARSPERLINE            5011
#define IDD_LINESPERPAGE            5012
#define IDD_PRINTREGFORM            5014
#define IDD_HELP                    5013
#define IDD_SAVEPRINTER             5015
#define IDD_ORIENT_LBL              5020
#define IDD_TEXTFONT_LBL            5016
#define IDD_HEADERFONT_LBL          5017
#define IDD_PRINTER_LBL             5018
#define IDD_OTHERINFO_LBL           5019

#define IDD_FPATH_LBL               5021
#define IDD_COLUMNS_LBL             5022
#define IDD_COLUMNS                 5023
#define DLG_ABOUT                   4999
#define IDD_VERSION                 100

#endif





/************************************************************************
 * End of File: dlgs.h
 ***********************************************************************/
