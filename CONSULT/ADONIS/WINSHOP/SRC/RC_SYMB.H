#ifndef RC_SYMBH
#define RC_SYMBH

#define SHOPPER                       10

#define BAUD_ID_BASE                  2001
#define ID_300                        2001
#define ID_600                        2002
#define ID_1200                       2003
#define ID_2400                       2004
#define ID_4800                       2005
#define ID_9600                       2006

#define PORT_ID_BASE                  2007
#define ID_COM1                       2007
#define ID_COM2                       2008
#define ID_COM3                       2009
#define ID_COM4                       2010

#define DIAL_ID_BASE                  2011
#define ID_TONE                       2011
#define ID_PULSE                      2012

#define ID_PPREFIX                    2013
#define ID_PSUFFIX                    2014

#define HELP                          800
#define IDH_CONNECT                   801
#define IDH_BILLING                   802
#define IDH_DEMO                      803
#define IDH_INFO                      804
#define IDH_ORDERING                  805
#define IDH_MAIN                      809
#define IDH_NEWUSER                   810
#define IDH_CUSTINFO                  811
#define IDH_SHIPINFO                  812
#define IDH_COMM                      813
#define IDH_LIBMGR                    814
#define IDH_UPDATEDB                  815
#define IDH_VIEW                      816
#define IDH_DLOADCLIP                 817
#define IDH_DLDIR                     818
#define IDH_SELECTBILL                819
#define IDH_ORDERLIB                  820
#define IDH_BILLINFO                  821
#define IDH_LIBDRIVE                  822
#define IDH_ADONIS                    823
#define IDH_WELCOME                   824
#define IDH_CHANGEPW                  825
#define IDH_VERIFYPW                  826
#define IDH_ORDERWS                   827
#define IDH_SEARCH                    828
#define IDH_DECISION                  829

#define ID_CATEGORY                   102
#define ID_CLOSE                      103
#define ID_CURDIR                     104
#define ID_DESCRIPTION                105
#define ID_DIR                        106
#define ID_DOWNLOAD                   107
#define ID_EDIT                       108
#define ID_FORMAT                     109
#define ID_HELP_LIST                  110
#define ID_HELP                       111
#define ID_LIBNAME                    114
#define ID_LIST                       115
#define ID_MORE                       116
#define ID_NAME                       117
#define ID_ORDER                      118
#define ID_PERCENT_COMPLETE           119
#define ID_PUBLISHER                  120
#define ID_RESELECT                   121
#define ID_SCROLL                     122
#define ID_SELECT                     123
#define ID_THUMBNAIL                  125
#define ID_COMPANY                    129
#define ID_ADDRESS                    130
#define ID_STATE                      131
#define ID_POSTAL                     132
#define ID_COUNTRY                    133
#define ID_AREA                       134
#define ID_PHONE                      135
#define ID_EXTENSION                  136
#define ID_SAVE                       137
#define ID_WELCOME                    138
#define ID_ADONIS                     140
#define ID_DEMO                       141
#define ID_DLDTIME                    142
#define ID_USEALTDIR                  143
#define ID_SELECT_DIR                 144
#define ID_SEARCH                     145

#define IDB_BILLING_INFO              201

#define CC_ID_BASE                    202
#define IDB_MASTERCARD                202
#define IDB_VISA                      203
#define IDB_DISCOVER                  204
#define IDB_AMEX                      205
#define IDB_CORP_ACCT                 206
#define IDS_EXP_DATE_MO               207
#define IDS_EXP_DATE_YR               208
#define IDB_CARD_NUMBER               209
#define IDB_LOG_ON_BBS                210
#define IDS_CITY                      211

#define IDB_SELECT_BILLING            220
#define IDB_CUSTOMER_INFO             221
#define IDB_DLDDIR                    222
#define IDB_DLDINFO                   223
#define IDB_SUBMIT                    224

#define IDB_SHIPPING_INFO             231

/* courier types -- code is dependent on the order of these */
#define COURIER_ID_BASE               232
#define IDB_1_2DAY                    232
#define IDB_2_3DAY                    233
#define IDB_USPS                      234
/* country  -- order is important-- don't touch */
#define COUNTRY_ID_BASE               235
#define IDB_US                        235
#define IDB_CANADA                    236
#define IDB_OTHER                     237

#define MINMAX_ID_BASE                238
#define IDB_MINIMAL_UPDATE            238
#define IDB_MAXIMAL_UPDATE            239

#define IDL_MEDIA                     240
#define IDL_SUBLIBRARIES              241
#define IDL_VERSION                   242
#define IDS_CARD_NO                   257
#define IDS_CARDOWNER                 258
#define IDS_CATEGORY                  259
#define IDS_CLIP_DIR                  260
#define IDS_CLIP_FILENAME             261
#define IDS_CLIPART_NAME              262
#define IDS_CLIPSIZE                  263
#define IDS_COPYRIGHTER               264
#define IDS_DESCRIPTION               265
/*
#define IDS_BLANK_USER                IDS_USER_ID + 1
#define ID_PASSWORD                   IDS_USER_ID + 2
#define ID_BLANK                      IDS_USER_ID + 3
*/
// dont change the next four values
#define IDS_USER_ID                   266
#define IDS_BLANK_USER                267
#define ID_PASSWORD                   268
#define ID_BLANK                      269

#define IDB_BBS                       270
#define IDB_FLOPPY                    271
#define IDB_NOUPDATE                  272
#define ID_LB_CACHE                   273

#define IDS_HOURS                     274
#define IDS_MINUTES                   275
#define IDS_SECONDS                   276

#define IDS_DHOURS                    277
#define IDS_DMINUTES                  278
#define IDS_DSECONDS                  279

#define IDB_LOGON                     280
#define IDB_THUMBDIR_A                281
#define IDB_THUMBDIR_B                282
#define IDB_THUMBDIR_C                283
#define IDS_SHOPPERICON               285
#define IDB_QUICK_CHECK               287

/* Search feature dialog defines */
#define ID_KEYWORD                    290
#define ID_SELFORMATS                 291
#define IDB_DOWNLOAD                  292
#define IDB_COLOR                     293
#define IDB_ALL                       294
#define IDB_SELECTED                  295
#define IDB_FIND                      296
#define IDB_FINDNEXT                  297
#define IDB_CONTINUE_SEARCH           298

#define IDM_CUSTOMER_INFO             500
#define IDM_SHIPPING_INFO             501
#define IDM_BILLING_INFO              502
#define IDM_PASSWORD                  503
#define IDM_ABOUT                     504
#define IDM_COMM                      505
#define IDM_HELP_TOPIC                506
#define IDM_NEW                       507
#define IDM_NEW_SUBSCRIPTION          508
#define IDM_UPGRADE_SUBSCRIPTION      509
#define IDM_UPDATE_THUMB              510
#define IDM_UPDATE_DB                 511
#define IDM_LIB_MANAGEMENT            512
#define IDM_INSTALL_WS                513
#define IDM_UPDATE_LOCAL_DB           514
#define IDM_WHATS_NEW                 515

#ifdef INTERNAL
#define IDM_SET_PHONENUM              516
#endif

#define IDS_ERR_CREATING_FILE         1001
#define IDS_ERR_DISK_FULL             1002
#define IDS_ERR_ENTRY_NOT_FOUND       1003
#define IDS_ERR_GMEM_FULL             1004
#define IDS_ERR_INVALID_CATEGORY      1005
#define IDS_ERR_INVALID_LIBRARY       1006
#define IDS_ERR_INVALID_PUBLISHER     1007
#define IDS_ERR_INVALID_USER_NAME     1008
#define IDS_ERR_LIB_NOT_SELECTED      1009
#define IDS_ERR_LIB_NOTAVAIL          1010
#define IDS_ERR_LOAD_FAILED           1011
#define IDS_ERR_LOADING_RES_LIB       1012
#define IDS_ERR_MISSING_ADMIN_FILES   1013
#define IDS_ERR_OPENING_FILE          1014
#define IDS_ERR_READING_FILE          1015
#define IDS_ERR_SECOND_INSTANCE       1016
#define IDS_ERR_TITLE                 1017
#define IDS_ERR_TOO_MANY_TIMERS       1018
#define IDS_ERR_TOO_MANY              1019
#define IDS_ERR_UNABLE_TO_CONNECT     1020
#define IDS_FATAL_ERR_TITLE           1021
#define IDS_ERR_BUSY                  1022
#define IDS_ERR_UNABLE_TO_DOWNLOAD    1023
#define IDS_ERR_BBS_TIMED_OUT         1024
#define IDS_MSG_NEW_EXE               1025
#define IDS_ERR_NO_MEMORY             1026
#define IDS_ERR_COMPRESSING           1027
#define IDS_ERR_LMEM_FULL             1028
#define IDS_ERR_INVALID_CARD_INFO     1029
#define IDS_ERR_INVALID_CARD          1030
#define IDS_ERR_MUST_SETUP            1031
#define IDS_ERR_MUST_LOGON            1032
#define IDS_ERR_MISSING_NAME          1033
#define IDS_ERR_INVALID_EXPR          1034
#define IDS_ERR_DELETING_FILE         1035
#define IDS_ERR_MOVING_FILE           1036
#define IDS_ERR_MUST_HAVE_PASSWORD    1037
#define IDS_ERR_INVALID_PASSWORD      1038
#define IDS_ERR_MUST_SELECT           1039
#define IDS_ERR_NEED_INFO             1040
#define IDS_ERR_INVALID_BILLING       1041
#define IDS_ERR_MISSING_LRU           1042
#define IDS_ERR_READING_WININI        1043
#define IDS_ERR_WRITING_WININI        1044
#define IDS_ERR_INITIALIZING          1045
#define IDS_ERR_INVALID_USERID        1046
#define IDS_ERR_UNENCRYPTING          1047
#define IDS_ERR_ENCRYPTING            1048
#define IDS_ERR_UPDATING_FILE         1049
#define IDS_ERR_NOT_DOWNLOADABLE      1050
#define IDS_ERR_CHECK_MODEM           1051
#define IDS_ERR_MISSING_NUMBER        1052
#define IDS_ERR_CREATING_DIR          1053
#define IDS_FILE_IO_ERR               1054
#define IDS_IO_ERR                    1055
#define IDS_ERR_CACHE_FULL            1056
#define IDS_ERR_BACKUP_TO_DIFF        1057
#define IDS_ERR_CREDIT_NOT_ACCEPT     1058
#define IDS_ERR_EXCEEDED_CORP_ACCT    1059
#define IDS_ERR_CANT_VALID_CARD       1060
#define IDS_ERR_SET_CUST_INFO         1061
#define IDS_ERR_BAD_NEW_ACCOUNT       1062
#define IDS_ERR_BAD_USERID            1063
#define IDS_ERR_MISSING_VIEW_THUMB    1064
#define IDS_ERR_SETTING_PASSWD        1065
#define IDS_ERR_PASSWD_LEN            1066
#define IDS_ERR_SELECT_NEW_COURIER    1067
#define IDS_ERR_BAD_BITMAP            1068
#define IDS_ERR_MISSING_ALT_DRIVE     1069
#define IDS_ERR_COMM                  1070
#define IDS_ERR_DELETE_LIBS           1071
#define IDS_ERR_DISK_UNAVAIL          1072
#define IDS_ERR_NOHELP				     1073
#define IDS_ERR_CREDIT_SECURE		     1074
#define IDS_ERR_REMOVE_LIBRARY		  1075
#define IDS_ERR_ARE_USER		        1076
#define IDS_ERR_ORDER_UNAVAILABLE	  1077
#define IDS_ERR_UNABLE_TO_ORDER		  1078
#define IDS_ERR_CONDITIONAL_ORDER	  1079
#define IDS_ERR_WINDOW_IN_USE		     1080
#define IDS_ERR_ADDLRU                1081
#define IDS_ERR_EXPIRED_ID    		  1082
#define IDS_ERR_UNABLE_TO_ORDER_CLIP  1083
#define IDS_ERR_TOO_LATE              1084
#define IDS_ERR_DIR_DOESNT_EXIST      1085
#define IDS_ERR_OLDWINVER             1086
#define IDS_ERR_PLEASE_WAIT           1087
#define IDS_ERR_MUST_NEWUSER          1088
#define IDS_ERR_SELECTFORMAT          1089
#define IDS_ERR_MISSING_SEARCH_THUMB  1099

#define IDS_MODULENAME                1100
#define IDS_APPNAME                   1101
#define IDS_WININILABEL               1102
#define IDS_TITLE_APPNAME             1103
#define IDS_TITLE_BILLING             1104
#define IDS_TITLE_CUSTOMER            1105
#define IDS_TITLE_SHIPPING            1106
#define IDS_TITLE_NEW_USER            1107
#define IDS_TITLE_CHANGE_PW           1108
#define IDS_TITLE_PASSWORD_VERIFY     1109

#define IDS_MSG_MISSING_LIB           1110
#define IDS_MSG_NEW_USER_DONE         1112
#define IDS_MSG_NEW_USER_START        1111
#define IDS_MSG_UPDATE_SHOPPER        1114
#define IDS_MSG_HAVE_ID               1115
#define IDS_MSG_REMEMBER              1116
#define IDS_MSG_SUBSCRIBE             1117
#define IDS_MSG_WANT_TO_BACKUP        1118
#define IDS_MSG_CLIPART_DOWN          1119
#define IDS_MSG_CLIPART_NOT_DOWN      1120
#define IDS_MSG_ORDER_OUTOFSTOCK      1121
#define IDS_MSG_UPDATE_BY_MAIL        1122
#define IDS_MSG_SHIP_CHARGE_NA        1123
#define IDS_MSG_QUERY_INTERRUPT       1124
#define IDS_MSG_QUERY_HANGUP          1125
#define IDS_MSG_DISCONNECTED          1126
#define IDS_MSG_VALIDATE_CREDIT       1127
#define IDS_MSG_INIT_CACHE            1128
#define IDS_MSG_SAVE_USERID		  	  1129
#define IDS_MSG_SAVE_ACTIVE_USERID    1130
#define IDS_MSG_NEW_USERID		  	     1131
#define IDS_MSG_NEW_PURCHASE_ORDER	  1132
#define IDS_MSG_ORDER_PLACED		     1133
#define IDS_MSG_CONNECT_TO_WS		     1134
#define IDS_MSG_ALREADY_VIEWING		  1135
#define IDS_MSG_USERID_ASSIGNED		  1136
#define IDS_MSG_MINUTE_WARNING        1137
#define IDS_MSG_DOWNLOADING           1138
#define IDS_MSG_TOTALING              1139
#define IDS_MSG_CONNECTING            1140
#define IDS_MSG_INITIALIZING          1141
#define IDS_MSG_NOSHIPPING            1142
#define IDS_MSG_COMPLETE_RENEWAL      1143
#define IDS_MSG_MISSING_WHATS_NEW     1144
#define IDS_MSG_CHECK_WHATS_NEW       1145
#define IDS_MSG_NO_RENEWAL_PKG        1146
#define IDS_MSG_NO_MATCH              1147
#define IDS_MSG_NO_MORE_MATCHES       1148
#define IDS_MSG_MISSING_SRCH_FILE     1149
#define IDS_MSG_STOP_SEARCH           1150
#define IDS_MSG_SEARCHING_FOR         1151
#define IDS_MSG_CHECKING_DATABASE     1152
#define IDS_MSG_UPDATING_CUST_REC     1153
#define IDS_LOADING                   1154
#define IDS_MSG_WS_FRMT               1155
#define IDS_MSG_EXE                   1156
#define IDS_MSG_DOWNLOADING_DB		  1157

#ifdef INTERNAL
#define IDS_MSG_SELECT_PHONENUM		1158
#endif

#ifdef NO_LOGON
#define  IDS_MSG_NO_LOGON           1159
#endif

#define IDS_NOT_DOWNLOADABLE          1160

#define IDS_FLOPPY_THUMB              1200
#define IDS_DATA_DIR                  1201
#define IDS_THUMB_DIR                 1202

#define IDS_INVOICE                   300
#define IDS_LIBRARY                   301
#define IDS_NUMBER_CLIPART            302
#define IDS_PRICE                     303
#define IDS_PUBLISHER                 304
#define IDS_SHIPPING_CHARGE           305
#define IDS_SR_PRICE                  306
#define IDS_SUBTOTAL                  307
#define IDS_SURCHARGE                 308
#define IDS_TAX                       309
#define IDS_TOTAL                     310
#define IDS_VERSION                   311
#define IDS_WS_PRICE_LABEL            312
#define IDS_WS_PRICE                  313
#define IDS_REQD_SPACE                314
#define IDS_SPACE_AVAIL               315
#define IDS_NUM_IMAGES                316
#define IDS_DISCOUNT_LABEL            317
#define IDS_DISCOUNT_AMT              318
#define IDS_LIB_QUANTITY              319
#define IDL_UID_LABEL                 320
#define IDS_PW_TEXT                   321
#define IDS_BUSY_MSG                  322
#define IDS_TITLE_USE_ALT_DIR         323
#define IDS_TITLE_BACKUP_DIR          324
#define IDS_SELECT_DIR_CLIPART        325
#define IDS_SELECT_DIR_ALT_THUMBNAIL  326
#define IDS_TITLE_ALT_THUMBNAIL	     327
#define IDS_TITLE_SEARCH              328
		
#define ID_PLUS                       600
#define ID_MINUS                      601

/** don't muck with these values.  They are indices into hChildren 
    array.  (4.24.90) D. Kent **/
#define NUM_LIB_CHILDREN       18
#define ID_LIB_LOGON            0
#define ID_LIB_NEWUSER          1
#define ID_LIB_SEARCH           2
#define ID_LIB_VIEW             3
#define ID_LIB_ORDER            4
#define ID_LIB_HELP             5
#define ID_LIB_CATEGORY         6
#define ID_LIB_PUBLISHER        7
#define ID_LIB_CAT_PUB_LIST     8
#define ID_LIB_LIST             9 
#define ID_LIB_STATUSBAR       10
#define ID_LIB_CAT_PUB_TITLE   11
#define ID_LIB_LIST_TITLE      12
#define ID_LIB_DESCRIPTION     13
#define ID_LIB_DESC_TITLE      14
#define ID_LIB_PUB_TITLE       15
#define ID_LIB_PUB_NAME        16
#define ID_LIB_ASTERISK_DESC   17
#define ID_DLDTIME_LABEL       18

/* These values must be contiguous for loading cursors using MAKEINTRESOURCE() */
#define PHONE1_CURSOR         4001
#define PHONE2_CURSOR         4002
#define PHONE3_CURSOR         4003
#define PHONE4_CURSOR         4004

#define SEARCH_CURSOR        4011

/* for BusyBoxWithIcon(), must be in consecutive order and each must be defined
   in BUSYWICON dlg template!! */

#define IDS_BUSY_FIRST_ICON            4020
#define IDS_BUSY_THUMB_ICON            4020
#define IDS_BUSY_CALC_ICON             4021
#define IDS_BUSY_PENCIL_ICON           4022
#define IDS_BUSY_CREDIT_ICON           4023
#define IDS_BUSY_CONNECT_ICON          4024
#define IDS_BUSY_DB_ICON               4025
#define IDS_BUSY_LAST_ICON             4025

/* for Missing Thumb Dlg */
#define IDS_NOTHUMB_ICON                4026

/* bitmaps for downloading dlg */

#define CLIPART_BMP          4030
#define DB_BMP               4031
#define WSBAG_BMP            4032

/* Descriptions for downloading dlg */

#define IDS_MSG_GETTING_CLIP          CLIPART_BMP
#define IDS_MSG_UPDATE_DATABASE       DB_BMP
#define IDS_MSG_UPDATE_SHOPPER_SHORT  WSBAG_BMP
#define IDS_MSG_UPDATE_SHOPPER_LONG   WSBAG_BMP+1


/* dialog coords for icon in BusyBox */
#define ICON_X1   9
#define ICON_Y1   7
#define ICON_X2   16
#define ICON_Y2   16

/* Dlg coords for meter control in Download cancel dialog */

#define   ID_PERCENT_COMPLETE_X1 47
#define   ID_PERCENT_COMPLETE_Y1 93
#define   ID_PERCENT_COMPLETE_X2 88
#define   ID_PERCENT_COMPLETE_Y2 11


/* For the Descicion dialog */
/* in setting nMissingThumbContext */
#define SEARCH_MODE  5000
#define VIEW_MODE  5001
#define DEFAULT_MODE VIEW_MODE

/* description control */
#define ID_DESC  5002

/* Dimensional values for Missing Thumb dlg when in view mode (smaller
   since we don't need continue search button) */
#define MT_VIEWMODE_CX  218
#define MT_VIEWMODE_CY  112

/* return value */
#define ID_GET_FROM_FLOPPY 5003

/* For GetAltDrive Dialog */
#define ID_CURPUB 5010
#define IDS_CURPUB 5011

#endif
