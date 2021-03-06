/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Compile Include File (COMPILE.H)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

/******************************************************************************/
/*									      */
/*  Global Definitions							      */
/*									      */
/******************************************************************************/

#define CL_SYMBL    0x0000			/* Symbol	    */
#define CL_OPERN    0x8000			/* Operand	    */
#define CL_KEYWD    0x4000			/* Keyword	    */
#define CL_FUNCT    0x6000			/* Function	    */
#define CL_SYVAR    0x1000			/* Variable	    */
#define CL_PAREN    0x0800			/* Funct has parens */
#define CL_OTPAR    0x0400			/* Can have parens  */
#define CL_STRFN    0x6200			/* Funct str output */


/******************************************************************************/
/*									      */
/*  Global Token Definitions                                                  */
/*									      */
/******************************************************************************/

#define TK_COMMA	 (CL_SYMBL |  1)	/* ,  */
#define TK_EOLL 	 (CL_SYMBL |  2)
#define TK_EOPL 	 (CL_SYMBL |  3)
#define TK_EOFM 	 (CL_SYMBL |  4)
#define TK_IDENT	 (CL_SYMBL |  5)
#define TK_LABEL	 (CL_SYMBL |  6)
#define TK_LBAC 	 (CL_SYMBL |  7)	/* {  */
#define TK_LBRK 	 (CL_SYMBL |  8)	/* [  */
#define TK_LPAR 	 (CL_SYMBL |  9)	/* (  */
#define TK_NUMCON	 (CL_SYMBL | 10)
#define TK_NUMVAR	 (CL_SYMBL | 11)
#define TK_RBAC 	 (CL_SYMBL | 12)	/* }  */
#define TK_RBRK 	 (CL_SYMBL | 13)	/* ]  */
#define TK_RPAR 	 (CL_SYMBL | 14)	/* )  */
#define TK_SEMI 	 (CL_SYMBL | 15)	/* ;  */
#define TK_INTEGER	 (CL_SYMBL | 16)
#define TK_STRING	 (CL_SYMBL | 17)
#define TK_VARIABLE	 (CL_SYMBL | 18)
#define TK_AMP		 (CL_SYMBL | 19)	/* @  */
#define TK_POUND	 (CL_SYMBL | 20)	/* #  */
#define TK_EOVL 	 (CL_SYMBL | 21)	/* #  */

#define TK_AND		 (CL_OPERN | 0x20 | 0x00)	/* AND */
#define TK_BITAND	 (CL_OPERN | 0x60 | 0x00)	/* &   */
#define TK_BITNOT	 (CL_OPERN | 0x70 | 0x00)	/* ~   */
#define TK_BITOR	 (CL_OPERN | 0x50 | 0x01)	/* |   */
#define TK_BITXOR	 (CL_OPERN | 0x60 | 0x01)	/* ^   */
#define TK_DIV		 (CL_OPERN | 0x60 | 0x02)	/* /   */
#define TK_EQ		 (CL_OPERN | 0x40 | 0x00)	/* =   */
#define TK_GE		 (CL_OPERN | 0x40 | 0x01)	/* >=  */
#define TK_GT		 (CL_OPERN | 0x40 | 0x02)	/* >   */
#define TK_INEQ 	 (CL_OPERN | 0x40 | 0x03)	/* <>  */
#define TK_IDIV 	 (CL_OPERN | 0x60 | 0x03)	/* \   */
#define TK_LE		 (CL_OPERN | 0x40 | 0x04)	/* <=  */
#define TK_LT		 (CL_OPERN | 0x40 | 0x05)	/* <   */
#define TK_MAX		 (CL_OPERN | 0x40 | 0x06)	/* MAX */
#define TK_MIN		 (CL_OPERN | 0x40 | 0x07)	/* MAX */
#define TK_MOD		 (CL_OPERN | 0x60 | 0x04)	/* MOD */
#define TK_MUL		 (CL_OPERN | 0x60 | 0x05)	/* *   */
#define TK_NEG		 (CL_OPERN | 0x70 | 0x01)	/* -   */
#define TK_NOT		 (CL_OPERN | 0x30 | 0x00)	/* NOT */
#define TK_OR		 (CL_OPERN | 0x10 | 0x00)	/* OR  */
#define TK_PLUS 	 (CL_OPERN | 0x50 | 0x00)	/* +   */
#define TK_ROL		 (0)				/* ROL */
#define TK_ROR		 (0)				/* ROR */
#define TK_SHL		 (0)				/* SHL */
#define TK_SHR		 (0)				/* SHR */
#define TK_SUB		 (CL_OPERN | 0x50 | 0x02)	/* -   */

#define TK_ABS		 (CL_FUNCT | CL_PAREN |   1)
#define TK_ACTIVATE      (CL_KEYWD |              2)
#define TK_ACTIVE        (CL_FUNCT |              3)
#define TK_ACTIVITY	 (0)
#define TK_ADD           (CL_KEYWD |              4)
#define TK_ALARM         (CL_KEYWD |              5)
#define TK_ALERT         (CL_KEYWD |              6)
#define TK_ALTNUMS	 (0)
#define TK_ANSWER        (0)
#define TK_ANSWERBACK    (CL_KEYWD | CL_SYVAR |   8)
#define TK_ANSWERSETUP   (CL_KEYWD | CL_SYVAR |   9)
#define TK_APPEND        (CL_KEYWD |             10)
#define TK_ARG           (CL_STRFN | CL_OTPAR |  11)
#define TK_AS            (CL_KEYWD |             12)
#define TK_ASC           (CL_FUNCT | CL_PAREN |  13)
#define TK_ASSUME	 (0)
#define TK_AT            (CL_KEYWD |             14)
#define TK_ATTACH	 (0)
#define TK_ATTEN	 (0)
#define TK_AUTOWRAP      (CL_KEYWD | CL_SYVAR |  15)
#define TK_BACKUPS       (CL_KEYWD | CL_SYVAR | 345)
#define TK_BINARY        (CL_STRFN | CL_PAREN |  16)
#define TK_BITSTRIP      (CL_STRFN | CL_PAREN |  17)
#define TK_BLANKEX       (CL_KEYWD | CL_SYVAR |  18)
#define TK_BLOCKSIZE     (0)
#define TK_BOL           (CL_KEYWD |             20)
#define TK_BOOKNAME      (CL_STRFN | CL_PAREN |  21)
#define TK_BOW           (CL_KEYWD |             22)
#define TK_BOX		 (0)
#define TK_BREAKLEN      (CL_KEYWD | CL_SYVAR |  23)
#define TK_BROWSE	 (0)
#define TK_BYE           (CL_KEYWD |             24)
#define TK_CALL          (CL_KEYWD |             25)
#define TK_CAPACITY      (0)
#define TK_CAPCHARS      (CL_FUNCT |             26)
#define TK_CAPFILE       (CL_STRFN |             27)
#define TK_CAPTURE       (CL_KEYWD |             28)
#define TK_CASE          (CL_KEYWD |             29)
#define TK_CBUFSIZE	 (0)
#define TK_CHAIN         (CL_KEYWD |             30)
#define TK_CHDIR         (CL_KEYWD |             31)
#define TK_CHECKBOX      (CL_KEYWD |             32)
#define TK_CHECKER	 (0)
#define TK_CHMOD         (CL_KEYWD |             33)
#define TK_CHR           (CL_STRFN | CL_PAREN |  34)
#define TK_CKSUM         (CL_FUNCT | CL_PAREN |  35)
#define TK_CLASS         (CL_FUNCT | CL_PAREN |  36)
#define TK_CLEAR         (CL_KEYWD |             37)
#define TK_CLOSE         (CL_KEYWD |             38)
#define TK_CLS           (CL_KEYWD |             39)
#define TK_CMODE         (CL_KEYWD |             40)
#define TK_CODESAVE      (0)
#define TK_COLORSCREEN   (CL_FUNCT |             42)
#define TK_COLUMNS       (CL_KEYWD | CL_SYVAR |  43)
#define TK_COMPILE       (CL_KEYWD |             44)
#define TK_CONNECTARQ    (CL_FUNCT |             45)
#define TK_CONNECTED     (CL_FUNCT |             46)
#define TK_CONNECTSPEED  (CL_FUNCT |             47)
#define TK_COPY 	 (0)
#define TK_COUNT         (CL_FUNCT | CL_OTPAR |  48)
#define TK_CRC           (CL_FUNCT | CL_PAREN |  49)
#define TK_CRN		 (0)
#define TK_CTEXT         (CL_KEYWD |             50)
#define TK_CURDAY        (CL_FUNCT |             51)
#define TK_CURDIR        (CL_STRFN | CL_OTPAR |  52)
#define TK_CURDRIVE      (CL_STRFN |             53)
#define TK_CURHOUR       (CL_FUNCT |             54)
#define TK_CURMINUTE     (CL_FUNCT |             55)
#define TK_CURMONTH      (CL_FUNCT |             56)
#define TK_CURSECOND     (CL_FUNCT |             57)
#define TK_CURSHAPE      (CL_KEYWD | CL_SYVAR |  58)
#define TK_CURYEAR       (CL_FUNCT |             59)
#define TK_CWAIT         (CL_KEYWD |             60)
#define TK_DATABITS      (CL_KEYWD | CL_SYVAR |  61)
#define TK_DATE          (CL_STRFN | CL_OTPAR |  62)
#define TK_DATEMODE      (CL_KEYWD | CL_SYVAR |  63)
#define TK_DAYNO	 (0)
#define TK_DDE           (CL_KEYWD |             64)
#define TK_DDEEXECUTE    (CL_KEYWD |             66)
#define TK_DDEINITIATE   (CL_KEYWD |             67)
#define TK_DDEPOKE       (CL_KEYWD |             68)
#define TK_DDEREQUEST    (CL_KEYWD |             69)
#define TK_DDESTATUS     (CL_FUNCT | CL_PAREN |  70)
#define TK_DDETERMINATE  (CL_KEYWD |             71)
#define TK_DECRYPT       (0)
#define TK_DEFINPUT      (CL_KEYWD | CL_SYVAR |  74)
#define TK_DEFOUTPUT     (CL_KEYWD | CL_SYVAR |  75)
#define TK_DEFPUSHBUTTON (CL_KEYWD |             76)
#define TK_DEFWINDOW	 (0)
#define TK_DEHEX         (CL_STRFN | CL_PAREN |  77)
#define TK_DELAY         (CL_KEYWD |             78)
#define TK_DELETE        (CL_STRFN | CL_OTPAR |  79)
#define TK_DESCRIPTION   (CL_KEYWD | CL_SYVAR |  80)
#define TK_DESTORE       (CL_STRFN | CL_PAREN |  81)
#define TK_DETEXT        (CL_STRFN | CL_PAREN |  82)
#define TK_DEVICE	 (0)
#define TK_DIAL          (CL_KEYWD |             83)
#define TK_DIALOGBOX     (CL_KEYWD |             84)
#define TK_DIALPREFIX    (CL_KEYWD | CL_SYVAR |  85)
#define TK_DIALSUFFIX    (CL_KEYWD | CL_SYVAR |  86)
#define TK_DIR           (0)
#define TK_DIRMODE	 (0)
#define TK_DIRSORT	 (0)
#define TK_DIRT 	 (0)
#define TK_DISKSPACE     (0)
#define TK_DISPLAY       (CL_KEYWD | CL_SYVAR |  87)
#define TK_DO            (0)
#define TK_DOSVERSION    (CL_STRFN |             89)
#define TK_DRAW 	 (0)
#define TK_DRIVE         (CL_KEYWD |             91)
#define TK_DROP 	 (0)
#define TK_ECHO          (CL_KEYWD | CL_SYVAR |  92)
#define TK_EDIT 	 (0)
#define TK_EDITOR        (CL_KEYWD | CL_SYVAR | 348)
#define TK_EDITTEXT      (CL_KEYWD |             93)
#define TK_ELSE          (CL_KEYWD |             94)
#define TK_ENCRYPT       (0)
#define TK_END           (CL_KEYWD |             96)
#define TK_ENDCASE       (CL_KEYWD |             97)
#define TK_ENDDIALOG     (CL_KEYWD |             98)
#define TK_ENDPROC       (0)
#define TK_ENDWATCH      (CL_KEYWD |             99)
#define TK_ENHEX         (CL_STRFN | CL_PAREN | 100)
#define TK_ENSTORE       (CL_STRFN | CL_PAREN | 101)
#define TK_ENTER         (CL_KEYWD | CL_SYVAR | 102)
#define TK_ENTEXT        (CL_STRFN | CL_PAREN | 103)
#define TK_ENVIRON       (CL_STRFN | CL_PAREN | 104)
#define TK_EOF           (CL_FUNCT | CL_OTPAR | 105)
#define TK_EOL           (CL_FUNCT | CL_OTPAR | 106)
#define TK_EOW           (CL_KEYWD |            107)
#define TK_ERRCLASS      (CL_FUNCT |            108)
#define TK_ERRNO         (CL_FUNCT |            109)
#define TK_ERROR         (CL_FUNCT |            110)
#define TK_EXCODE	 (0)
#define TK_EXEC 	 (0)
#define TK_EXISTS        (CL_FUNCT | CL_PAREN | 111)
#define TK_EXIT          (0)
#define TK_EXTRACT       (CL_STRFN | CL_PAREN | 113)
#define TK_FALSE         (CL_FUNCT |            114)
#define TK_FIELD	 (0)
#define TK_FILEATTR      (CL_FUNCT | CL_OTPAR | 115)
#define TK_FILEDATE      (CL_FUNCT | CL_OTPAR | 116)
#define TK_FILEFIND      (CL_STRFN | CL_OTPAR | 117)
#define TK_FILESIZE      (CL_FUNCT | CL_OTPAR | 118)
#define TK_FILETIME      (CL_FUNCT | CL_OTPAR | 119)
#define TK_FILTER	 (0)
#define TK_FKEY          (CL_STRFN | CL_OTPAR | 120)
#define TK_FLASH	 (0)
#define TK_FLISTCOUNT	 (0)
#define TK_FLISTNAME	 (0)
#define TK_FLOAT	 (0)
#define TK_FLOOD	 (0)
#define TK_FLOW          (CL_KEYWD | CL_SYVAR | 121)
#define TK_FNCHECK       (CL_FUNCT | CL_PAREN | 122)
#define TK_FNSTRIP       (CL_STRFN | CL_PAREN | 123)
#define TK_FOOTER        (CL_KEYWD | CL_SYVAR | 124)
#define TK_FOR           (CL_KEYWD |            125)
#define TK_FORGET	 (0)
#define TK_FORM 	 (0)
#define TK_FORMFEED      (CL_KEYWD | CL_SYVAR | 126)
#define TK_FRAC 	 (0)
#define TK_FRAMINGERRORS (CL_KEYWD | CL_SYVAR | 127)
#define TK_FREEFILE      (CL_FUNCT |            128)
#define TK_FREEMEM       (CL_FUNCT |            129)
#define TK_FREETRACK	 (0)
#define TK_FREEWIN	 (0)
#define TK_GET           (CL_KEYWD |            130)
#define TK_GLOBAL        (CL_KEYWD |            132)
#define TK_GO            (CL_KEYWD |            133)
#define TK_GOSUB         (CL_KEYWD |            134)
#define TK_GOTO          (CL_KEYWD |            135)
#define TK_GRAB          (CL_KEYWD |            136)
#define TK_GRAPHICS      (CL_KEYWD | CL_SYVAR | 137)
#define TK_GROUP         (CL_KEYWD |            138)
#define TK_GROUPBOX      (CL_KEYWD |            139)
#define TK_GUEST	 (0)
#define TK_HALT          (CL_KEYWD |            140)
#define TK_HEADER        (CL_KEYWD | CL_SYVAR | 141)
#define TK_HELP 	 (0)
#define TK_HEX           (CL_STRFN | CL_PAREN | 142)
#define TK_HIDE          (CL_KEYWD |            143)
#define TK_HMS           (CL_STRFN | CL_PAREN | 144)
#define TK_HOSTMODE      (CL_KEYWD | CL_SYVAR | 145)
#define TK_HOSTSCRIPT    (CL_KEYWD | CL_SYVAR | 146)
#define TK_HOUR          (CL_KEYWD |            147)
#define TK_IF            (CL_KEYWD |            148)
#define TK_INBOOK        (CL_FUNCT | CL_OTPAR | 149)
#define TK_INCLUDE	 (0)
#define TK_INDEX	 (0)
#define TK_INJECT        (CL_STRFN | CL_PAREN | 150)
#define TK_INKEY         (CL_FUNCT |            151)
#define TK_INP		 (0)
#define TK_INPUT         (CL_KEYWD |            152)
#define TK_INSCRIPT      (CL_FUNCT | CL_PAREN | 153)
#define TK_INSERT        (CL_STRFN | CL_PAREN | 154)
#define TK_INSTR         (CL_FUNCT | CL_PAREN | 155)
#define TK_INT		 (0)
#define TK_INTEGERKEY    (CL_KEYWD |            156)
#define TK_INTVAL        (CL_KEYWD | CL_FUNCT | 157)
#define TK_JUMP          (CL_KEYWD |            158)
#define TK_KBBIOS	 (0)
#define TK_KBFLAGS	 (0)
#define TK_KCLEAR        (CL_KEYWD |            160)
#define TK_KEEP 	 (0)
#define TK_KERMIT        (CL_KEYWD |            161)
#define TK_KEY           (CL_KEYWD |            162)
#define TK_KEYBOARD	 (0)
#define TK_KEYCLICK	 (0)
#define TK_KEYNAME	 (0)
#define TK_KEYS 	 (0)
#define TK_KEYVAL	 (0)
#define TK_KLOAD         (CL_KEYWD |            163)
#define TK_KSAVE         (CL_KEYWD |            164)
#define TK_LDNUMBER      (CL_KEYWD | CL_SYVAR | 165)
#define TK_LEFT          (CL_STRFN | CL_PAREN | 166)
#define TK_LENGTH        (CL_FUNCT | CL_PAREN | 167)
#define TK_LFAUTO        (CL_KEYWD | CL_SYVAR | 168)
#define TK_LIFT 	 (0)
#define TK_LINE          (CL_KEYWD |            169)
#define TK_LINEDELIM     (CL_KEYWD | CL_SYVAR | 170)
#define TK_LINETIME      (CL_KEYWD | CL_SYVAR | 171)
#define TK_LIST 	 (0)
#define TK_LISTBOX       (CL_KEYWD |            172)
#define TK_LOAD          (CL_KEYWD |            173)
#define TK_LOC           (CL_FUNCT | CL_OTPAR | 174)
#define TK_LOCAL         (CL_KEYWD | CL_SYVAR | 175)
#define TK_LOCKFKEY      (0)
#define TK_LOG           (0)
#define TK_LOWCASE       (CL_STRFN | CL_PAREN | 177)
#define TK_LPRINT        (CL_KEYWD |            178)
#define TK_LPT		 (0)
#define TK_LTEXT         (CL_KEYWD |            179)
#define TK_LWAIT         (CL_KEYWD |            180)
#define TK_MACRO	 (0)
#define TK_MANUAL        (CL_KEYWD |            181)
#define TK_MASTER	 (0)
#define TK_MATCH         (CL_STRFN |            182)
#define TK_MAXERRORS     (CL_KEYWD | CL_SYVAR | 183)
#define TK_MAXIMIZE      (CL_KEYWD |            184)
#define TK_MENUS	 (0)
#define TK_MESSAGE       (CL_KEYWD | CL_SYVAR | 185)
#define TK_MID           (CL_STRFN | CL_PAREN | 186)
#define TK_MILLISEC      (CL_KEYWD |            187)
#define TK_MINIMIZE      (CL_KEYWD |            188)
#define TK_MINUTE        (CL_KEYWD |            189)
#define TK_MISC          (CL_KEYWD | CL_SYVAR | 190)
#define TK_MKDIR         (CL_KEYWD |            191)
#define TK_MKINT         (CL_FUNCT | CL_PAREN | 192)
#define TK_MKSTR         (CL_STRFN | CL_PAREN | 193)
#define TK_MODE 	 (0)
#define TK_MODEMCHANGE   (CL_KEYWD | CL_SYVAR | 194)
#define TK_MODEMHANGUP   (CL_KEYWD | CL_SYVAR | 195)
#define TK_MODEMINIT     (CL_KEYWD | CL_SYVAR | 196)
#define TK_MODEMNAME     (CL_KEYWD | CL_SYVAR | 197)
#define TK_MODEMRESULTS  (CL_KEYWD | CL_SYVAR | 198)
#define TK_MODIFIED	 (0)
#define TK_MODIFIER      (CL_KEYWD | CL_SYVAR | 199)
#define TK_MONEY	 (0)
#define TK_MOVE          (CL_KEYWD |            200)
#define TK_MSEC          (CL_KEYWD |            201)
#define TK_MSGCOUNT	 (0)
#define TK_NAME          (CL_STRFN |            202)
#define TK_NETID         (CL_KEYWD | CL_SYVAR | 203)
#define TK_NEW           (CL_KEYWD |            204)
#define TK_NEWLINE       (CL_KEYWD | CL_SYVAR | 205)
#define TK_NEXT          (CL_KEYWD |            206)
#define TK_NEXTCHAR      (CL_STRFN |            207)
#define TK_NEXTLINE      (CL_KEYWD |            208)
#define TK_NEXTMSG	 (0)
#define TK_NOCOLOR       (CL_KEYWD | CL_SYVAR | 209)
#define TK_NONE          (CL_KEYWD |            210)
#define TK_NORMAL        (CL_KEYWD |            211)
#define TK_NULL          (CL_FUNCT | CL_PAREN | 212)
#define TK_NUMBER        (CL_KEYWD | CL_SYVAR | 213)
#define TK_OCTAL         (CL_STRFN | CL_PAREN | 214)
#define TK_OFF           (CL_FUNCT |            215)
#define TK_ON            (CL_FUNCT |            216)
#define TK_ONLINE        (CL_FUNCT |            217)
#define TK_ONTIME        (CL_KEYWD | CL_SYVAR | 218)
#define TK_OPEN          (CL_KEYWD |            219)
#define TK_OUT		 (0)
#define TK_OUTNUMBER     (CL_KEYWD | CL_SYVAR | 220)
#define TK_OUTPUT        (CL_KEYWD |            221)
#define TK_OVERRUNERRORS (CL_KEYWD | CL_SYVAR | 222)
#define TK_PACK          (CL_STRFN | CL_PAREN | 223)
#define TK_PACKETSIZE    (CL_KEYWD | CL_SYVAR | 224)
#define TK_PAD           (CL_STRFN | CL_PAREN | 225)
#define TK_PAINT	 (0)
#define TK_PAN           (0)
#define TK_PARENT        (0)
#define TK_PARITY        (CL_KEYWD | CL_SYVAR | 228)
#define TK_PARITYERRORS  (CL_KEYWD | CL_SYVAR | 229)
#define TK_PASSWORD      (CL_KEYWD | CL_SYVAR | 230)
#define TK_PATIENCE      (CL_KEYWD | CL_SYVAR | 231)
#define TK_PAUSE         (CL_KEYWD |            232)
#define TK_PEEK 	 (0)
#define TK_PEEK16	 (0)
#define TK_PERFORM       (0)
#define TK_PERMISSION	 (0)
#define TK_PICTURE	 (0)
#define TK_PMODE         (CL_KEYWD | CL_SYVAR | 234)
#define TK_POKE 	 (0)
#define TK_POP           (CL_KEYWD |            235)
#define TK_PORT          (CL_KEYWD | CL_SYVAR | 236)
#define TK_PRESERVE	 (0)
#define TK_PRESS         (0)
#define TK_PRINT         (CL_KEYWD |            238)
#define TK_PRINTER       (CL_KEYWD | CL_SYVAR | 239)
#define TK_PRINTEXTENT   (CL_KEYWD | CL_SYVAR | 240)
#define TK_PROC          (0)
#define TK_PROMPT        (CL_KEYWD |            241)
#define TK_PROTOCOL      (CL_KEYWD | CL_SYVAR | 242)
#define TK_PUSHBUTTON    (CL_KEYWD |            244)
#define TK_PUT           (CL_KEYWD |            245)
#define TK_QUIET         (CL_KEYWD |            246)
#define TK_QUIT          (CL_KEYWD |            247)
#define TK_QUOTE         (CL_STRFN | CL_PAREN | 248)
#define TK_RANDOM        (CL_KEYWD |            249)
#define TK_RAW           (CL_KEYWD |            250)
#define TK_READ          (CL_KEYWD |            251)
#define TK_RECEIVE       (CL_KEYWD |            252)
#define TK_RECOVERY      (CL_KEYWD | CL_SYVAR | 253)
#define TK_RECVMODE      (CL_KEYWD | CL_SYVAR | 254)
#define TK_REDIALCOUNT   (CL_KEYWD | CL_SYVAR | 255)
#define TK_REDIALWAIT    (CL_KEYWD | CL_SYVAR | 256)
#define TK_RELEASE	 (0)
#define TK_REMOVE        (0)
#define TK_RENAME        (CL_KEYWD |            258)
#define TK_REPEAT        (CL_KEYWD |            259)
#define TK_REPLACE	 (0)
#define TK_REPLY         (CL_KEYWD |            260)
#define TK_REQUEST       (CL_KEYWD |            261)
#define TK_RESTORE       (CL_KEYWD |            262)
#define TK_RESULT	 (0)
#define TK_RETURN        (CL_KEYWD |            263)
#define TK_REVIEW        (CL_KEYWD | CL_SYVAR | 264)
#define TK_REWIND        (0)
#define TK_RIGHT         (CL_STRFN | CL_PAREN | 266)
#define TK_RMDIR         (CL_KEYWD |            267)
#define TK_ROUND	 (0)
#define TK_RTEXT         (CL_KEYWD |            268)
#define TK_RUN           (CL_KEYWD |            269)
#define TK_SAVE          (CL_KEYWD |            270)
#define TK_SCREEN	 (0)
#define TK_SCREENHEIGHT  (0)
#define TK_SCREENWIDTH	 (0)
#define TK_SCRIPT        (CL_KEYWD | CL_SYVAR | 271)
#define TK_SCROLL        (CL_KEYWD |            272)
#define TK_SECNO         (CL_FUNCT | CL_OTPAR | 273)
#define TK_SECOND        (CL_KEYWD |            274)
#define TK_SECRET        (CL_KEYWD | CL_SYVAR | 275)
#define TK_SEEK          (CL_KEYWD |            276)
#define TK_SEND          (CL_KEYWD |            277)
#define TK_SENDBREAK     (CL_KEYWD |            278)
#define TK_SESSION	 (0)
#define TK_SESSNAME	 (0)
#define TK_SESSNO	 (0)
#define TK_SETUP	 (0)
#define TK_SHOW          (CL_KEYWD |            280)
#define TK_SHOWACTIVE    (CL_KEYWD | CL_SYVAR | 281)
#define TK_SHOWINFOBAR   (CL_KEYWD | CL_SYVAR | 282)
#define TK_SHOWINPUT     (CL_KEYWD | CL_SYVAR | 283)
#define TK_SHOWMACROBAR  (CL_KEYWD | CL_SYVAR | 284)
#define TK_SHOWSTATUSBAR (CL_KEYWD | CL_SYVAR | 285)
#define TK_SHOWHSCROLL   (CL_KEYWD | CL_SYVAR | 286)
#define TK_SHOWVSCROLL   (CL_KEYWD | CL_SYVAR | 287)
#define TK_SHUT          (0)
#define TK_SIZE          (CL_KEYWD |            288)
#define TK_SLICE         (CL_STRFN | CL_PAREN | 289)
#define TK_SORT 	 (0)
#define TK_SOUND	 (0)
#define TK_SOUNDEX	 (0)
#define TK_SPACE         (CL_KEYWD |            290)
#define TK_SPEED         (CL_KEYWD | CL_SYVAR | 291)
#define TK_STARTUP       (CL_KEYWD | CL_SYVAR | 292)
#define TK_STATUS	 (0)
#define TK_STOPBITS      (CL_KEYWD | CL_SYVAR | 293)
#define TK_STR           (CL_STRFN | CL_PAREN | 294)
#define TK_STRINGKEY     (CL_KEYWD |            295)
#define TK_STRIP         (CL_STRFN | CL_PAREN | 296)
#define TK_STROKE        (0)
#define TK_SUBST         (CL_STRFN | CL_PAREN | 298)
#define TK_SUBTITLE	 (0)
#define TK_SWAP          (CL_KEYWD |            299)
#define TK_SWAPDEL       (CL_KEYWD | CL_SYVAR | 300)
#define TK_SWITCH	 (0)
#define TK_SYSTIME       (CL_KEYWD | CL_SYVAR | 301)
#define TK_TABEX         (CL_KEYWD | CL_SYVAR | 302)
#define TK_TABSTOP       (CL_KEYWD |            303)
#define TK_TABWIDTH      (CL_KEYWD | CL_SYVAR | 304)
#define TK_TELL 	 (0)
#define TK_TERMINAL      (CL_KEYWD | CL_SYVAR | 305)
#define TK_TERMINATE     (CL_KEYWD |            306)
#define TK_THEN          (CL_KEYWD |            307)
#define TK_TICK          (CL_KEYWD |            308)
#define TK_TIME          (CL_STRFN | CL_OTPAR | 309)
#define TK_TIMEOUT       (CL_FUNCT |            310)
#define TK_TIMING        (CL_KEYWD | CL_SYVAR | 311)
#define TK_TITLE	 (0)
#define TK_TOGGLE        (CL_KEYWD |            312)
#define TK_TRACE         (CL_KEYWD |            313)
#define TK_TRACK	 (0)
#define TK_TRAP          (CL_KEYWD |            314)
#define TK_TRUE          (CL_FUNCT |            315)
#define TK_TRUNC	 (0)
#define TK_UNTIL         (CL_KEYWD |            316)
#define TK_UP            (CL_KEYWD |            317)
#define TK_UPCASE        (CL_STRFN | CL_PAREN | 318)
#define TK_UPLOAD        (CL_KEYWD |            319)
#define TK_USERID        (CL_KEYWD | CL_SYVAR | 320)
#define TK_VAL           (0)
#define TK_VERSION       (CL_STRFN |            321)
#define TK_VIDEO	 (0)
#define TK_VIEW 	 (0)
#define TK_VISUAL        (CL_KEYWD |            322)
#define TK_WAIT          (CL_KEYWD |            323)
#define TK_WATCH         (CL_KEYWD |            324)
#define TK_WEEKDAY       (CL_FUNCT | CL_OTPAR | 325)
#define TK_WEND          (CL_KEYWD |            326)
#define TK_WHILE         (CL_KEYWD |            327)
#define TK_WINATX	 (0)
#define TK_WINATY	 (0)
#define TK_WINCHAR       (CL_FUNCT | CL_PAREN | 328)
#define TK_WINCOLOR	 (0)
#define TK_WINDOW        (CL_KEYWD |            297)
#define TK_WINDOWWRAP    (CL_KEYWD | CL_SYVAR | 330)
#define TK_WINLOCKS	 (0)
#define TK_WINSIZEX      (CL_FUNCT |            331)
#define TK_WINSIZEY      (CL_FUNCT |            332)
#define TK_WINSTRING     (CL_STRFN | CL_PAREN | 333)
#define TK_WINVERSION    (CL_STRFN |            334)
#define TK_WRITE         (CL_KEYWD |            335)
#define TK_XONCHAR       (CL_KEYWD | CL_SYVAR | 336)
#define TK_XOFFCHAR      (CL_KEYWD | CL_SYVAR | 337)
#define TK_XPOS          (CL_FUNCT |            338)
#define TK_XTPATH	 (0)
#define TK_YPOS          (CL_FUNCT |            339)
#define TK_FOCUS         (CL_KEYWD |            340)
#define TK_RADIOBUTTON   (CL_KEYWD |            341)
#define TK_OK            (CL_KEYWD |            342)
#define TK_CANCEL        (CL_KEYWD |            343)
#define TK_CHOICE        (CL_KEYWD | CL_SYVAR | 344)
#define TK_CPM           (CL_KEYWD |            346)
#define TK_DOS           (CL_KEYWD |            347)

/******************************************************************************/
/*									      */
/*  Global Variables                                                          */
/*									      */
/******************************************************************************/

  extern HANDLE hObjectCode;        /* Memory handle to the object code     */
  extern BYTE far *lpObjectCode;    /* Address of the object code           */


/******************************************************************************/
/*									      */
/*  External Procedure's Prototypes                                           */
/*									      */
/******************************************************************************/

  extern BOOL NeedCompile(char *);
  extern BOOL CompileScript(char *);
  extern BOOL LoadScriptObject(char *);
  extern BOOL SaveScriptObject(char *);
  extern void CompilerError();
