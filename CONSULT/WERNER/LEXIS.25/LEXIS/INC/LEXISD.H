/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  lexisd.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This file contains all #defines for resource items.
 *
 *       IDM   - Menu item id's
 *       IDB   - Button id's
 *       IDS   - String table id's
 *       IDBMP - Bitmap id's
 *       IDD   - Dialog box item id's
 *       IDDLG - Dialog box id's
 *       
 *
 *   Revisions:  
 *
 *
 ************************************************************************/

/* Dialog Box IDs */
/* Start at 1 */

/* Button IDs 8?
/* Start at 512 */


/* There are three menu's.  they are defined via these numbers */

#define LEGALMENU    1
#define CORPMENU     2
#define COMBINEDMENU 3


/* Menu items    */
/* Start at 1024 */

#define IDM_NEW                  1024
#define IDM_OPEN                 1025
#define IDM_CLOSE                1026

#define IDM_PRINT                1027

#define IDM_RECORDING            1028
#define IDM_DIARY                1029
#define IDM_PRINTER              1030

#define IDM_SETUP                1031
#define IDM_PAGESETUP            1032
#define IDM_PRINTSETUP           1033

#define IDM_SIGNON               1034
#define IDM_EXIT                 1035

#define IDM_COPY                 1036
#define IDM_PASTE                1037
#define IDM_COPYDIARY            1038
                           
#define IDM_NEWSEARCH            1039
#define IDM_SEARCHMODIFY         1040
#define IDM_SEARCHLEVEL          1041

#define IDM_LIBRARY              1042
#define IDM_FILE                 1043
#define IDM_CLIENT               1044

#define IDM_ECLIPSE              1045
#define IDM_ECLIPSERECALL        1046

#define IDM_SEARCHDISPLAY        1047

#define IDM_STOP                 1048

#define IDM_NEXTPAGE             1049
#define IDM_PREVPAGE             1050

#define IDM_NEXTDOC              1051
#define IDM_PREVDOC              1052

#define IDM_GOTOPAGE             1053
#define IDM_GOTODOC              1054
     
#define IDM_FIRSTPAGE            1055
#define IDM_FIRSTDOC             1056

#define IDM_KWIC                 1057
#define IDM_FULL                 1058
#define IDM_CITE                 1059
#define IDM_VARKWIC              1060
#define IDM_SEGMENTS             1061
#define IDM_SEQUENCE             1062

#define IDM_ENTERSTAR            1063
#define IDM_EXITSTAR             1064

#define IDM_FONTS                1065

#define IDM_AUTOCITE             1066
#define IDM_SHEPARD              1067
#define IDM_LEXSEE               1068
#define IDM_LEXSTAT              1069
#define IDM_BROWSE               1070
#define IDM_RESUME               1071
                           
#define IDM_SELECTSERVICE        1072
#define IDM_EXITSERVICE          1073

#define IDM_QUOTES               1074

#define IDM_PRINTSCREEN          1075
#define IDM_PRINTDOC             1076
#define IDM_PRINTALL             1077

#define IDM_TIME                 1078
#define IDM_REQUEST              1079
#define IDM_PAGES                1080
#define IDM_DEBUT                1081

#define IDM_HELP_INDEX           1084
#define IDM_HELP_KEYBOARD        1085
#define IDM_HELP_BASIC           1086
#define IDM_HELP_COMMANDS        1087
#define IDM_HELP_PROCEDURES      1088
#define IDM_HELP_HELPONHELP      1089

#define IDM_TUTORIAL             1090
#define IDM_LEXISHELP            1091

#define IDM_ABOUT                1092         

#define IDM_LEXDOC               1095

#define IDM_TL_FILE              1106               
#define IDM_TL_EDIT              1107
#define IDM_TL_SEARCH            1108
#define IDM_TL_VIEW              1109
#define IDM_TL_FORMATS           1110
#define IDM_TL_CITATORS          1111
#define IDM_TL_DELIVERY          1112
#define IDM_TL_INFO              1113
#define IDM_TL_HELP              1114

#define IDM_SETUP_USERPREF       1138
#define IDM_SETUP_DIRECTORIES    1139
#define IDM_SETUP_HARDWARE       1140

#define IDM_NEW_RECORDING        1141
#define IDM_NEW_DIARY            1142
#define IDM_NEW_NOTEPAD          1143

#define IDM_OPEN_RECORDING       1144
#define IDM_OPEN_DIARY           1145
#define IDM_OPEN_NOTEPAD         1146

#define IDM_CLOSE_RECORDING      1147
#define IDM_CLOSE_DIARY          1148

#define IDM_PRINT_RECORDING      1149
#define IDM_PRINT_DIARY          1150
#define IDM_PRINT_NOTEPAD        1151
#define IDM_PRINT_DOCUMENT       1152
#define IDM_PRINT_SCREEN         1153

#define IDS_SYSMENU_SIZE         1160
#define IDS_SYSMENU_MOVE         1161
#define IDS_SYSMENU_MINIMIZE     1162
#define IDS_SYSMENU_MAXIMIZE     1163
#define IDS_SYSMENU_CLOSE        1164
#define IDS_SYSMENU_RESTORE      1165
#define IDS_SYSMENU_TASKLIST     1166
#define IDS_SYSMENU              1167

#define IDS_MENU_NEW             IDM_NEW            

#define IDS_MENU_NEW_RECORDING   IDM_NEW_RECORDING
#define IDS_MENU_NEW_DIARY       IDM_NEW_DIARY
#define IDS_MENU_NEW_NOTEPAD     IDM_NEW_NOTEPAD

#define IDS_MENU_OPEN_RECORDING  IDM_OPEN_RECORDING
#define IDS_MENU_OPEN_DIARY      IDM_OPEN_DIARY    
#define IDS_MENU_OPEN_NOTEPAD    IDM_OPEN_NOTEPAD  

#define IDM_MENU_CLOSE_RECORDING IDM_CLOSE_RECORDING  
#define IDS_MENU_CLOSE_DIARY     IDM_CLOSE_DIARY     

#define IDS_MENU_PRINT_RECORDING IDM_PRINT_RECORDING
#define IDS_MENU_PRINT_DIARY     IDM_PRINT_DIARY
#define IDS_MENU_PRINT_NOTEPAD   IDM_PRINT_NOTEPAD
#define IDS_MENU_PRINT_DOCUMENT  IDM_PRINT_DOCUMENT
#define IDS_MENU_PRINT_SCREEN    IDM_PRINT_SCREEN   

#define IDS_MENU_OPEN            IDM_OPEN           
#define IDS_MENU_CLOSE  	      IDM_CLOSE          

#define IDS_MENU_PRINT           IDM_PRINT          

#define IDS_MENU_RECORDING       IDM_RECORDING      
#define IDS_MENU_DIARY           IDM_DIARY          
#define IDS_MENU_PRINTING        IDM_PRINTER        

#define IDS_MENU_SETUP           IDM_SETUP          
#define IDS_MENU_SETUP_USERPREF  IDM_SETUP_USERPREF
#define IDS_MENU_SETUP_DIRECTORIES IDM_SETUP_DIRECTORIES
#define IDS_MENU_SETUP_HARDWARE  IDM_SETUP_HARDWARE
#define IDS_MENU_PAGESETUP 	   IDM_PAGESETUP      
#define IDS_MENU_PRINTSETUP 	   IDM_PRINTSETUP     

#define IDS_MENU_SIGNON          IDM_SIGNON         
#define IDS_MENU_EXIT            IDM_EXIT           

#define IDS_MENU_COPY            IDM_COPY           
#define IDS_MENU_COPYDIARY       IDM_COPYDIARY
#define IDS_MENU_PASTE           IDM_PASTE      
                                                  
#define IDS_MENU_NEWSEARCH       IDM_NEWSEARCH      
#define IDS_MENU_SEARCHMODIFY    IDM_SEARCHMODIFY   
#define IDS_MENU_SEARCHLEVEL     IDM_SEARCHLEVEL    

#define IDS_MENU_LIBRARY 	      IDM_LIBRARY        
#define IDS_MENU_FILE            IDM_FILE           
#define IDS_MENU_CLIENT          IDM_CLIENT         

#define IDS_MENU_ECLIPSE         IDM_ECLIPSE        
#define IDS_MENU_ECLIPSERECALL   IDM_ECLIPSERECALL  

#define IDS_MENU_SEARCHDISPLAY   IDM_SEARCHDISPLAY  

#define IDS_MENU_STOP            IDM_STOP           

#define IDS_MENU_NEXTPAGE        IDM_NEXTPAGE 
#define IDS_MENU_PREVPAGE        IDM_PREVPAGE 

#define IDS_MENU_NEXTDOC         IDM_NEXTDOC  
#define IDS_MENU_PREVDOC         IDM_PREVDOC  

#define IDS_MENU_GOTOPAGE        IDM_GOTOPAGE 
#define IDS_MENU_GOTODOC         IDM_GOTODOC  
         
#define IDS_MENU_FIRSTPAGE       IDM_FIRSTPAGE
#define IDS_MENU_FIRSTDOC        IDM_FIRSTDOC 

#define IDS_MENU_KWIC            IDM_KWIC           
#define IDS_MENU_FULL            IDM_FULL           
#define IDS_MENU_CITE            IDM_CITE           
#define IDS_MENU_VARKWIC         IDM_VARKWIC        
#define IDS_MENU_SEGMENTS        IDM_SEGMENTS       
#define IDS_MENU_SEQUENCE 	      IDM_SEQUENCE       

#define IDS_MENU_ENTERSTAR       IDM_ENTERSTAR      
#define IDS_MENU_EXITSTAR        IDM_EXITSTAR       

#define IDS_MENU_FONTS           IDM_FONTS          

#define IDS_MENU_AUTOCITE        IDM_AUTOCITE       
#define IDS_MENU_SHEPARD         IDM_SHEPARD        
#define IDS_MENU_LEXSEE          IDM_LEXSEE         
#define IDS_MENU_LEXSTAT         IDM_LEXSTAT        
#define IDS_MENU_BROWSE          IDM_BROWSE         
#define IDS_MENU_RESUME          IDM_RESUME         
                                                  
#define IDS_MENU_SELECTSERVICE   IDM_SELECTSERVICE  
#define IDS_MENU_EXITSERVICE     IDM_EXITSERVICE    

#define IDS_MENU_QUOTES          IDM_QUOTES         

#define IDS_MENU_PRINTSCREEN     IDM_PRINTSCREEN    
#define IDS_MENU_PRINTDOC        IDM_PRINTDOC       
#define IDS_MENU_PRINTALL        IDM_PRINTALL       

#define IDS_MENU_TIME            IDM_TIME           
#define IDS_MENU_REQUEST         IDM_REQUEST        
#define IDS_MENU_PAGES           IDM_PAGES          
#define IDS_MENU_DEBUT           IDM_DEBUT          

#define IDS_MENU_INDEX           IDM_HELP_INDEX     
#define IDS_MENU_KEYBOARD        IDM_HELP_KEYBOARD  
#define IDS_MENU_BASIC           IDM_HELP_BASIC     
#define IDS_MENU_COMMANDS        IDM_HELP_COMMANDS  
#define IDS_MENU_PROCEDURES      IDM_HELP_PROCEDURES
#define IDS_MENU_HELPONHELP      IDM_HELP_HELPONHELP

#define IDS_MENU_TUTORIAL        IDM_TUTORIAL       
#define IDS_MENU_LEXISHELP       IDM_LEXISHELP      

#define IDS_MENU_ABOUT           IDM_ABOUT          

#define IDS_MENU_LEXDOC          IDM_LEXDOC

#define IDS_TOPMENU_FILE         IDM_TL_FILE
#define IDS_TOPMENU_EDIT         IDM_TL_EDIT
#define IDS_TOPMENU_SEARCH       IDM_TL_SEARCH
#define IDS_TOPMENU_VIEW         IDM_TL_VIEW
#define IDS_TOPMENU_FORMATS      IDM_TL_FORMATS
#define IDS_TOPMENU_CITATORS     IDM_TL_CITATORS
#define IDS_TOPMENU_DELIVERY     IDM_TL_DELIVERY
#define IDS_TOPMENU_INFO         IDM_TL_INFO
#define IDS_TOPMENU_HELP         IDM_TL_HELP


/**************************************************************
 * Button defines.  These are for buttons on the button bar.
 *
 **************************************************************/
#define IDB_START                (IDB_TRANSMIT)

#define IDB_TRANSMIT             1536
#define IDB_CHGLIB               1537
#define IDB_NEWSEARCH            1538
#define IDB_QUOTES               1539
#define IDB_CITATORS             1540
#define IDB_KWIC                 1541
#define IDB_NEXTPAGE             1542
#define IDB_NEXTDOC              1543

#define IDB_PRINTALL             1544
#define IDB_CHGFILE              1545
#define IDB_STOP                 1546
#define IDB_FOCUS                1547
#define IDB_LEXSTAT              1548
#define IDB_FULL                 1549
#define IDB_PREVPAGE             1550
#define IDB_PREVDOC              1551

#define IDB_PRINTDOC             1552
#define IDB_SIGNON               1553
#define IDB_RESUME               1554
#define IDB_LEXDOC               1555
#define IDB_LEXSEE               1556
#define IDB_CITE                 1557
#define IDB_FIRSTPAGE            1558
#define IDB_FIRSTDOC             1559

#define IDB_END                  (IDB_FIRSTDOC)

/* Corportate personality buttons.  These map into some of the buttons */
/* above */
#define IDB_SAVE                 1560
#define IDB_MODIFY               1561
#define IDB_SEGMENTS             1562
#define IDB_RECALL               1563
#define IDB_SIGNOFF              1564
#define IDB_EXITFOCUS            1565

#define IDB_TOPSTAT              1600

#define IDB_CLIENT_LBL           1600
#define IDB_CLIENT               1601
#define IDB_LIBRARY_LBL          1602
#define IDB_LIBRARY              1603
#define IDB_TIME_LBL             1604
#define IDB_TIME                 1605

#define IDB_BOTSTAT              1664

#define IDB_CAPSLOCK             1664
#define IDB_NUMLOCK              1665
#define IDB_DUMMY1_LBL           1666
#define IDB_RECORD_STATUS        1667
#define IDB_DIARY_STATUS         1668
#define IDB_PRINT_STATUS         1669
#define IDB_DUMMY2_LBL           1670
#define IDB_CD_STATUS            1671
#define IDB_DUMMY3_LBL           1672
#define IDB_DATE                 1673
#define IDB_MSGLINE              0xFFFF

#define IDS_BUTTON_TRANSMIT      IDB_TRANSMIT       
#define IDS_BUTTON_CHGLIB        IDB_CHGLIB    
#define IDS_BUTTON_NEWSEARCH     IDB_NEWSEARCH 
#define IDS_BUTTON_CITATORS      IDB_CITATORS  
#define IDS_BUTTON_KWIC          IDB_KWIC      
#define IDS_BUTTON_FIRSTPAGE     IDB_FIRSTPAGE 
#define IDS_BUTTON_FIRSTDOC      IDB_FIRSTDOC  
#define IDS_BUTTON_PRINTALL      IDB_PRINTALL  
#define IDS_BUTTON_CHGFILE       IDB_CHGFILE   
#define IDS_BUTTON_STOP          IDB_STOP      
#define IDS_BUTTON_LEXSTAT       IDB_LEXSTAT   
#define IDS_BUTTON_FULL          IDB_FULL      
#define IDS_BUTTON_NEXTPAGE      IDB_NEXTPAGE  
#define IDS_BUTTON_NEXTDOC       IDB_NEXTDOC   
#define IDS_BUTTON_PRINTDOC      IDB_PRINTDOC  
#define IDS_BUTTON_SIGNON        IDB_SIGNON    
#define IDS_BUTTON_SIGNOFF       IDB_SIGNOFF
#define IDS_BUTTON_RESUME        IDB_RESUME    
#define IDS_BUTTON_LEXSEE        IDB_LEXSEE    
#define IDS_BUTTON_CITE          IDB_CITE      
#define IDS_BUTTON_PREVPAGE      IDB_PREVPAGE  
#define IDS_BUTTON_PREVDOC       IDB_PREVDOC   

#define IDS_BUTTON_QUOTES        IDB_QUOTES
#define IDS_BUTTON_FOCUS         IDB_FOCUS
#define IDS_BUTTON_EXITFOCUS     IDB_EXITFOCUS
#define IDS_BUTTON_LEXDOC        IDB_LEXDOC

#define IDS_BUTTON_SAVE          IDB_SAVE    
#define IDS_BUTTON_MODIFY        IDB_MODIFY  
#define IDS_BUTTON_SEGMENTS      IDB_SEGMENTS
#define IDS_BUTTON_RECALL        IDB_RECALL  

#define IDS_STATBOX_CLIENT_LBL   IDB_CLIENT_LBL   
#define IDS_STATBOX_CLIENT       IDB_CLIENT       
#define IDS_STATBOX_LIBRARY_LBL  IDB_LIBRARY_LBL  
#define IDS_STATBOX_LIBRARY      IDB_LIBRARY      
#define IDS_STATBOX_TIME_LBL     IDB_TIME_LBL

#define IDS_STATBOX_MSGLINE      IDB_MSGLINE    
#define IDS_STATBOX_RECORD_STATUS IDB_RECORD_STATUS
#define IDS_STATBOX_DIARY_STATUS IDB_DIARY_STATUS
#define IDS_STATBOX_PRINT_STATUS IDB_PRINT_STATUS
#define IDS_STATBOX_CAPSLOCK     IDB_CAPSLOCK   
#define IDS_STATBOX_NUMLOCK      IDB_NUMLOCK    
#define IDS_STATBOX_SCRLLOCK     IDB_SCRLLOCK   
#define IDS_STATBOX_CD_STATUS    IDB_CD_STATUS  

/**************************************************************
 *
 * Other strings reside at string table entry #2048
 *
 **************************************************************/

#define IDS_APPTITLE_LEGAL       2048
#define IDS_APPTITLE_CORP        2049
#define IDS_APPTITLE_COMBINED    2050
#define IDS_APPNAME              2051
#define IDS_LONGTITLE            2052

#define IDS_IFN                  2053
#define IDS_FNF                  2054
#define IDS_REF                  2055
#define IDS_SCC                  2056
#define IDS_EOF                  2057
#define IDS_ECF                  2058
#define IDS_EXT                  2059

#define IDS_WNDCLASS_MAIN        2060
#define IDS_WNDCLASS_TERM        2061
#define IDS_APPDESC              2062

#define IDS_NOTIMPLEMENTED       2063

#define IDS_FILESPEC             2064
#define IDS_FILESPEC_RECORD      2064
#define IDS_FILESPEC_NOTEPAD     2065
#define IDS_FILESPEC_DIARY       2066
#define IDS_FILESPEC_DOCUMENT    2067

#define IDS_OPENTITLE            2070
#define IDS_OPENTITLE_RECORD     2070
#define IDS_OPENTITLE_NOTEPAD    2071
#define IDS_OPENTITLE_DIARY      2072

#define IDS_NEWTITLE             2073
#define IDS_NEWTITLE_RECORD      2073
#define IDS_NEWTITLE_NOTEPAD     2074
#define IDS_NEWTITLE_DIARY       2075

#define IDS_PRINTTITLE           2076
#define IDS_PRINTTITLE_RECORD    2076
#define IDS_PRINTTITLE_NOTEPAD   2077
#define IDS_PRINTTITLE_DIARY     2078
#define IDS_PRINTTITLE_DOCUMENT  2079

#define IDS_INI_WIN              2112
#define IDS_INI_WIN_XPOS         2113
#define IDS_INI_WIN_YPOS         2114
#define IDS_INI_WIN_XSIZE        2115
#define IDS_INI_WIN_YSIZE        2116
#define IDS_INI_WIN_NCMDSHOW     2117

#define IDS_INI_USERPREF         2128
#define IDS_INI_PERSONALITY      2129
#define IDS_INI_TOPSTAT          2130
#define IDS_INI_BOTSTAT          2131
#define IDS_INI_BUTTONWND        2132
#define IDS_INI_FASTPRINT        2133
#define IDS_INI_BELL             2134

#define IDS_INI_DIRS             2144
#define IDS_INI_DIRS_RECORD      2145
#define IDS_INI_DIRS_NOTEPAD     2146
#define IDS_INI_DIRS_DIARY       2147
#define IDS_INI_DIRS_DOCUMENT    2148

#define IDS_INI_PRINT            2160
#define IDS_INI_PRINT_HEADER     2161
#define IDS_INI_PRINT_FOOTER     2162

#define IDS_INI_HARDWARE         2176
#define IDS_INI_CONNECTOR        2177
#define IDS_INI_MODEM            2178
#define IDS_INI_L2000KEYBOARD    2179
#define IDS_INI_PARITY           2180
#define IDS_INI_DIALTYPE         2181

//
// [Primary], [1st Alternate], [2nd Alternate]
#define IDS_INI_PRIMARY          2192
#define IDS_INI_1STALTERNATE     2193
#define IDS_INI_2NDALTERANTE     2194

//
// Network =  , Speed = ,  Prefix = ,  Number =  , Node = ,
#define IDS_INI_NETWORK          2195
#define IDS_INI_SPEED            2196
#define IDS_INI_PREFIX           2197
#define IDS_INI_NUMBER           2198
#define IDS_INI_NODE             2199
                               
#define IDS_INI_NORMALFONT       2208
#define IDS_INI_HIGHLIGHTFONT    2209
#define IDS_INI_FACENAME         2210
#define IDS_INI_FACESIZEX        2211
#define IDS_INI_FACESIZEY        2212
#define IDS_INI_FGCOLOR          2213
#define IDS_INI_BGCOLOR          2214
#define IDS_INI_BOLD             2215
#define IDS_INI_ITALIC           2216
#define IDS_INI_UNDERLINE        2217
#define IDS_INI_STRIKEOUT        2218


#define IDS_STATE_OFFLINE        2300
#define IDS_STATE_MANUAL         2301
#define IDS_STATE_MANUALP        2302
#define IDS_STATE_SIGNON 	      2303
#define IDS_STATE_SIGNONP        2304
#define IDS_STATE_PROCESS        2305
#define IDS_STATE_READY          2306
#define IDS_STATE_PRINTING 	   2307
#define IDS_STATE_SWITCHING      2308
#define IDS_STATE_USERID 	      2309
#define IDS_STATE_PASSWORD 	   2310

#define IDS_SYSERROR             2500
#define IDS_SYSERROR_FMT         2501


/********************************************
#define IDS_MSG_BADDOCFILE       
#define IDS_MSG_LOG_FILE 	      
#define IDS_MSG_BECONNECTED      
#define IDS_MSG_LOCALMEMORY      
#define IDS_MSG_NOENTRIES        
#define IDS_MSG_BADCAPTUREFILE   
#define IDS_MSG_SURETODELETE     
#define IDS_MSG_FILEXIST         
#define IDS_MSG_BADOPEN          
#define IDS_MSG_BADNAME          
#define IDS_MSG_DISKFULL         
#define IDS_MSG_BADCOMM          
#define IDS_MSG_SPEEDHI 	      
#define IDS_MSG_SPEEDBAD1        
#define IDS_MSG_SPEEDBAD2        
#define IDS_MSG_SPEEDBAD3        
#define IDS_MSG_NOPRINTERS       
#define IDS_MSG_PREFIXBAD        
#define IDS_MSG_NUMBERBAD        
#define IDS_MSG_CALLFAILED       
#define IDS_MSG_NOCFGFILE        
#define IDS_MSG_BADINIFILE       

#define IDS_MODEM_ATT2224        2080 USED!
#define IDS_MODEM_HAYES1200      2081
#define IDS_MODEM_HAYES1200B     2082
#define IDS_MODEM_HAYES2400      2083
#define IDS_MODEM_HAYES2400B     2084  
#define IDS_MODEM_LEADING        2085
#define IDS_MODEM_MT212AH2       2086
#define IDS_MODEM_MT212HC        2087
#define IDS_MODEM_MT224PC        2088
#define IDS_MODEM_POPCOM         2089
#define IDS_MODEM_PROMODEM       2090
#define IDS_MODEM_VA3451         2091
#define IDS_MODEM_RACAL1200      2092
#define IDS_MODEM_RACAL2400      2093
#define IDS_MODEM_USR2400        2094
#define IDS_MODEM_USR9600        2095

#define IDS_DATA_PROFILE       2112  USED!!!
#define IDS_DATA_CONFIG 	    2113
#define IDS_DATA_WORKFILE 	    2114
#define IDS_DATA_DATA          2115
#define IDS_DATA_NETWORK       2116
#define IDS_DATA_MODEMS        2117
#define IDS_DATA_MENUS         2118
#define IDS_DATA_SCRIPTS       2119
#define IDS_DATA_LOGON 	       2120
#define IDS_DATA_MODEM 	       2121
#define IDS_DATA_NETWORK1      2122
#define IDS_DATA_NETWORK2      2123
#define IDS_DATA_NETWORK3      2124
#define IDS_DATA_PREFIX1       2125
#define IDS_DATA_PREFIX2       2126
#define IDS_DATA_PREFIX3       2127
#define IDS_DATA_NUMBER1       2128
#define IDS_DATA_NUMBER2       2129
#define IDS_DATA_NUMBER3       2130
#define IDS_DATA_NODE1         2131
#define IDS_DATA_NODE2         2132
#define IDS_DATA_NODE3         2133
#define IDS_DATA_FACE 	       2134
#define IDS_DATA_BELL 	       2135
#define IDS_DATA_FASTPRINT     2136
#define IDS_DATA_PORT 	       2137
#define IDS_DATA_HARDWIRE 	    2138
#define IDS_DATA_PBX           2139
#define IDS_DATA_DIAL 	       2140
#define IDS_DATA_PARITY 	    2141
#define IDS_DATA_SPEED1        2142
#define IDS_DATA_SPEED2        2143
#define IDS_DATA_SPEED3        2144
#define IDS_DATA_PITCH 	       2145
#define IDS_DATA_CHARSET 	    2146
#define IDS_DATA_HEIGHT 	    2147
#define IDS_DATA_WIDTH 	       2148
#define IDS_DATA_RECORD        2149
#define IDS_DATA_NOTEPAD       2150
#define IDS_DATA_MAPS          2151
#define IDS_DATA_DOC           2152
#define IDS_DATA_HEADER        2153
#define IDS_DATA_FOOTER        2154
#define IDS_DATA_STATUS        2155
#define IDS_DATA_MACRO         2156
#define IDS_DATA_INFO          2157

*/
/**************************************************************
 * end of lexisd.h
 **************************************************************/
