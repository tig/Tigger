/*****************************************************************************\
*                                                                             *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Dialogs Include File (DIALOGS.H)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.1b 05/09/90 PJL Split file in two, use NOTRC flag to include  *
*								 2nd header during source file compilation.	   *
*                                                                             *
\*****************************************************************************/

// include string table constant defs?
#ifdef NOTRC
#include "strtbl.h"
#endif


// dialog box constant defs ...

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
#define ID_OPT2             0x0115
#define ID_DEFAULT          0x0116
#define ID_YES              0x0117
#define ID_NO               0x0118
#define ID_HELP             0x0119
#define ID_HIDE 	    0x011A

#define ID_ABOUT_CMEM           0x0320
#define ID_ABOUT_EMEM           0x0321

#define ID_HELP_TOPICS          0x0322
#define ID_HELP_NEXT            0x0323
#define ID_HELP_PREV            0x0324

#define ID_RUNAPP_CLIPBOARD     0x0331
#define ID_RUNAPP_CONTROL       0x0332
#define ID_RUNAPP_NOTEPAD       0x0333

#define ID_SESSION_DESCRIPTION	0x0300
#define ID_SESSION_NUMBER	0x0301
#define ID_SESSION_MODIFIER	0x0302
#define ID_SESSION_USERID	0x0303
#define ID_SESSION_PASSWORD	0x0304
#define ID_SESSION_SCRIPT	0x0305
#define ID_SESSION_NETID	0x0307
#define ID_SESSION_LOG		0x0308
#define ID_SESSION_NORMAL	0x0309
#define ID_SESSION_RAW		0x030A
#define ID_SESSION_VISUAL	0x030B
#define ID_SESSION_MANUAL	0x030C
#define ID_SESSION_PATIENCE	0x030D
#define ID_SESSION_REDIALCOUNT	0x030E
#define ID_SESSION_REDIALWAIT	0x030F
#define ID_SESSION_ANSWERBACK   0x0310

#define ID_TERM_VT102		0x0400
#define ID_TERM_VT52		0x0401
#define ID_TERM_IBMPC		0x0402
#define ID_TERM_IBM3101 	0x0403
#define ID_TERM_VIDTEX		0x0404
#define ID_TERM_COLUMNS 	0x0405
#define ID_TERM_80		0x0406
#define ID_TERM_132		0x0407
#define ID_TERM_BAR		0x0408
#define ID_TERM_LINE		0x0409
#define ID_TERM_BLOCK           0x040A
#define ID_TERM_LFAUTO		0x040B
#define ID_TERM_OFF		0x040C
#define ID_TERM_ADDLF		0x040D
#define ID_TERM_NEWLINE 	0x040E
#define ID_TERM_CR		0x040F
#define ID_TERM_CRLF		0x0410
#define ID_TERM_ENTER           0x0411
#define ID_TERM_TABWIDTH        0x0412
#define ID_TERM_AUTOWRAP        0x0413
#define ID_TERM_ECHO            0x0414
#define ID_TERM_EIGHTBIT        0x0415
#define ID_TERM_NOCOLOR         0x0416
#define ID_TERM_FORMFEED        0x0417
#define ID_TERM_PRINTEXTENT     0x0418
#define ID_TERM_SWAPDEL         0x0419
#define ID_TERM_WINDOWWRAP      0x041A
#define ID_TERM_GRAPHICS        0x041B

#define ID_MACRO_TITLE1 	 0x0440
#define ID_MACRO_TITLE2 	 0x0441
#define ID_MACRO_TITLE3 	 0x0442
#define ID_MACRO_TITLE4 	 0x0443
#define ID_MACRO_TITLE5 	 0x0444
#define ID_MACRO_TITLE6 	 0x0445
#define ID_MACRO_MACRO1          0x0446
#define ID_MACRO_MACRO2          0x0447
#define ID_MACRO_MACRO3          0x0448
#define ID_MACRO_MACRO4          0x0449
#define ID_MACRO_MACRO5          0x044A
#define ID_MACRO_MACRO6          0x044B
#define ID_MACRO_LABEL1          0x044C
#define ID_MACRO_LABEL2          0x044D
#define ID_MACRO_LABEL3          0x044E
#define ID_MACRO_LABEL4          0x044F
#define ID_MACRO_LABEL5          0x0450
#define ID_MACRO_LABEL6          0x0451
#define ID_MACRO_MFLAG1          0x0452
#define ID_MACRO_MFLAG2          0x0453
#define ID_MACRO_MFLAG3          0x0454
#define ID_MACRO_MFLAG4          0x0455
#define ID_MACRO_MFLAG5          0x0456
#define ID_MACRO_MFLAG6          0x0457
#define ID_MACRO_MACRON          0x0458
#define ID_MACRO_GROUP1          0x0459
#define ID_MACRO_GROUP2          0x045A
#define ID_MACRO_GROUP3          0x045B
#define ID_MACRO_GROUP4          0x045C
#define ID_MACRO_GROUP5          0x045D
#define ID_MACRO_GROUP6          0x045E
#define ID_MACRO_GROUP7          0x045F
#define ID_MACRO_GROUP8          0x0460
#define ID_MACRO_NOTE            0x0461
#define ID_MACRO_LABEL		 0x0462
#define ID_MACRO_COLORBAR1	 0x0463
#define ID_MACRO_COLORBAR2	 0x0464

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

#define ID_COMPILE_SCRIPT       0x0560
#define ID_COMPILE_LINE         0x0561

#define ID_DIAL_MANUAL		0x0600
#define ID_DIAL_NUMBER		0x0601
#define ID_DIAL_STATUS		0x0602
#define ID_DIAL_TITLE		0x0603
#define ID_DIAL_WATCH		0x0604
#define ID_DIAL_MSG             0x0605

#define ID_COMM_PORT        0x0700
#define ID_COMM_COM1	    0x0701
#define ID_COMM_COM2	    0x0702
#define ID_COMM_COM3	    0x0703
#define ID_COMM_COM4	    0x0704
#define ID_COMM_CURRENT     0x0705
#define ID_COMM_110	    0x0706
#define ID_COMM_300	    0x0707
#define ID_COMM_600	    0x0708
#define ID_COMM_1200	    0x0709
#define ID_COMM_2400	    0x070A
#define ID_COMM_4800	    0x070B
#define ID_COMM_9600	    0x070C
#define ID_COMM_19200	    0x070D
#define ID_COMM_WORD7	    0x070E
#define ID_COMM_WORD8	    0x070F
#define ID_COMM_EVEN	    0x0710
#define ID_COMM_ODD	    0x0711
#define ID_COMM_NOPAR	    0x0712
#define ID_COMM_MARK	    0x0713
#define ID_COMM_SPACE	    0x0714
#define ID_COMM_STOP1	    0x0715
#define ID_COMM_STOP2	    0x0716
#define ID_COMM_B10	    0x0717
#define ID_COMM_B150	    0x0718
#define ID_COMM_B200	    0x0719
#define ID_COMM_B250	    0x071A
#define ID_COMM_B300	    0x071B
#define ID_COMM_B500	    0x071C
#define ID_COMM_B1000	    0x071D
#define ID_COMM_B2000	    0x071E
#define ID_COMM_ON	    0x071F
#define ID_COMM_OFF	    0x0720
#define ID_COMM_NONE	    0x0721
#define ID_COMM_XON	    0x0722
#define ID_COMM_RTS	    0x0723
#define ID_COMM_BOTH	    0x0724
#define ID_COMM_COMBIOS     0x0725

#define ID_MODEM_COM1       0x0738
#define ID_MODEM_COM2       0x0739
#define ID_MODEM_COM3       0x0740
#define ID_MODEM_COM4       0x0741
#define ID_MODEM_PREFIX     0x0742
#define ID_MODEM_SUFFIX     0x0743
#define ID_MODEM_ANSWER     0x0744
#define ID_MODEM_INIT       0x0745
#define ID_MODEM_HANGUP     0x0746
#define ID_MODEM_CHANGE     0x0747
#define ID_MODEM_OK         0x0748
#define ID_MODEM_ERROR      0x0749
#define ID_MODEM_CONNECT    0x074A
#define ID_MODEM_SPEED      0x074B
#define ID_MODEM_ARQ        0x074C
#define ID_MODEM_DIALTONE   0x074D
#define ID_MODEM_RING       0x074E
#define ID_MODEM_NOTONE     0x074F
#define ID_MODEM_BUSY       0x0750
#define ID_MODEM_CARRIER    0x0751

#define ID_ALERT_MSG1           0x0770
#define ID_ALERT_MSG2           0x0771
#define ID_ALERT_EDIT           0x0772
#define ID_ALERT_BUTTON1        3
#define ID_ALERT_BUTTON2        4
#define ID_ALERT_BUTTON3        5
#define ID_ALERT_BUTTON4        6

#define ID_PRINT_HEADER 	0x0890
#define ID_PRINT_FOOTER 	0x0891

#define ID_PROTOCOL_XTALK	0x0900
#define ID_PROTOCOL_DART	0x0901
#define ID_PROTOCOL_MODEM7	0x0902
#define ID_PROTOCOL_XMODEM	0x0903
#define ID_PROTOCOL_MODEM1K	0x0904
#define ID_PROTOCOL_YMODEM	0x0905
#define ID_PROTOCOL_ZMODEM	0x0906
#define ID_PROTOCOL_KERMIT      0x0907
#define ID_PROTOCOL_CSERVEB     0x0908
#define ID_PROTOCOL_ASCII       0x090B
#define ID_PROTOCOL_TIGHT       0x090C
#define ID_PROTOCOL_NORMAL      0x090D
#define ID_PROTOCOL_LOOSE       0x090E
#define ID_PROTOCOL_SLOPPY      0x090F
#define ID_PROTOCOL_CREATE      0x0910
#define ID_PROTOCOL_ORIGINAL    0x0911
#define ID_PROTOCOL_RECON       0x0912
#define ID_PROTOCOL_RECOFF      0x0913
#define ID_PROTOCOL_MAXERRORS   0x0914
#define ID_PROTOCOL_RECVNORM    0x0915
#define ID_PROTOCOL_RECVALL     0x0916
#define ID_PROTOCOL_RECVNEWER   0x0917
#define ID_PROTOCOL_128         0x0918
#define ID_PROTOCOL_256         0x0919
#define ID_PROTOCOL_512         0x091A
#define ID_PROTOCOL_1024        0x091B
#define ID_PROTOCOL_HOSTON      0x091C
#define ID_PROTOCOL_HOSTOFF     0x091D
#define ID_PROTOCOL_PACKETSIZE  0x091E
#define ID_PROTOCOL_768         0x091F

#define ID_ASCII_LWAIT_NONE	0x0930
#define ID_ASCII_LWAIT_ECHO	0x0931
#define ID_ASCII_LWAIT_PROMPT	0x0932
#define ID_ASCII_LWAIT_COUNT	0x0933
#define ID_ASCII_LWAIT_DELAY	0x0934
#define ID_ASCII_LWAIT_TEXT     0x0935
#define ID_ASCII_CWAIT_NONE     0x0936
#define ID_ASCII_CWAIT_ECHO     0x0937
#define ID_ASCII_CWAIT_DELAY    0x0938
#define ID_ASCII_CWAIT_TEXT     0x0939
#define ID_ASCII_LINEDELIM      0x093A
#define ID_ASCII_BLANKEX        0x093B
#define ID_ASCII_TABEX_YES      0x093C
#define ID_ASCII_TABEX_NO       0x093D
#define ID_ASCII_LINETIME       0x093E
#define ID_ASCII_WORDWRAP       0x093F
#define ID_ASCII_LTYPE_TEXT	0x0940

#define ID_GLOBAL_STARTUP	0x0A00
#define ID_GLOBAL_LDNUMBER	0x0A01
#define ID_GLOBAL_OUTNUMBER	0x0A02
#define ID_GLOBAL_BACKUPS	0x0A05
#define ID_GLOBAL_SECRET	0x0A06
#define ID_GLOBAL_XWP		0x0A07
#define ID_GLOBAL_XWS		0x0A08
#define ID_GLOBAL_FIL		0x0A09
#define ID_GLOBAL_EDIT          0x0A0A
#define ID_GLOBAL_NONE          0x0A0B
#define ID_GLOBAL_16K           0x0A0C
#define ID_GLOBAL_32K           0x0A0D
#define ID_GLOBAL_64K           0x0A0E
#define ID_GLOBAL_CP2K          0x0A0F
#define ID_GLOBAL_CP4K          0x0A10
#define ID_GLOBAL_CP8K          0x0A11
#define ID_GLOBAL_CP12K         0x0A12
#define ID_GLOBAL_CP16K         0x0A13
#define ID_GLOBAL_PASTE         0x0A14

#define ID_ERROR_ERROR		0x0A40
#define ID_ERROR_TEXT		0x0A41
#define ID_ERROR_LINE		0x0A42
#define ID_ERROR_LINENO 	0x0A43

#define ID_XFER_TITLE		0x0B00
#define ID_XFER_BLOCK		0x0B01
#define ID_XFER_BYTES		0x0B02
#define ID_XFER_OFBYTES 	0x0B03
#define ID_XFER_EST		0x0B04
#define ID_XFER_TOTAL		0x0B05
#define ID_XFER_CONSEC		0x0B06
#define ID_XFER_LAST		0x0B07
#define ID_XFER_CPS		0x0B08
#define ID_XFER_GRAPHICS	0x0B09
#define ID_XFER_NAME		0x0B0A
#define ID_XFER_MSG		0x0B0C
#define ID_XFER_SIZE		0x0B0D
#define ID_XFER_LINES		0x0B0E

