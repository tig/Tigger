//*************************************************************
//  File name: res.h
//
//  Description: 
//      Header file for the resources
//
//  History:    Date       Author     Comment
//               2/12/92   MSM        Created
//
//*************************************************************

#include "dlgs.h" //commdlgs

#define COLLECTIONS_DLG         11
#define PANOSE_DLG              12
#define PURCHASE_DLG            13
#define NEWPASSWORD_DLG         14
#define FILTER_DLG              15
#define ABOUT_DLG               16
#define USERID_DLG              17
#define NEWUSER_DLG             18
#define ORDERINFO_DLG           19
#define VIEWTEXT_DLG            20
#define OPTIONS_DLG             21
#define POSTPURCHASE_DLG        22
#define NOUSERID_DLG            23
#define FILEVERSIONS_DLG        24
#define RENEWUSER_DLG           25


//*** Toolbar Buttons
#define IDTB_SELECTED           10

//*** Child Window IDs
#define ID_STATUS               IDM_HELP_INDEX
#define ID_SLIDER               901
#define ID_ELB                  902
#define ID_FONTVIEW             903
#define ID_TOOLBAR              904

//*** Menu IDs.  Keep these in sequence and incrementing by 1!!!!
#define IDM_ACTION_UPDATE       1000
#define IDM_ACTION_UNMARK       1001
#define IDM_ACTION_DOWNLOAD     1002
#define IDM_ACTION_PURCHASE_SUB 1003
#define IDM_ACTION_PASSWORD     1004
#define IDM_ACTION_EXIT         1005
#define IDM_ACTION_CONNECT      1006

#define IDM_INFO_WHATSNEW       1007
#define IDM_ACTION_COLLECTIONS  1008

#define IDM_SORT_NAME           1009
#define IDM_SORT_PRICE          1010
#define IDM_SORT_SIZE           1011
#define IDM_SORT_PANOSE         1012
#define IDM_SET_FILTER          1013
#define IDM_CLEAR_FILTER        1014
#define IDM_GROUP_PUB           1015
#define IDM_GROUP_FACE          1016
                                
#define IDM_SETTINGS_SETUP      1017
#define IDM_INFO_CUSTOMER       1018
#define IDM_INFO_SHIPPING       1019
#define IDM_INFO_BILLING        1020
                                
#define IDM_SETTINGS_FONT       1021
                                
#define IDM_HELP_INDEX          1022 
#define IDM_HELP_SEARCH         1023
#define IDM_HELP_USINGHELP      1024
#define IDM_HELP_ABOUT          1025
                                
#define IDM_INGRES              1026
#define IDM_ERRORS              1027
#define IDM_RELOAD              1028
                                    

//*** Resource IDs
#define IDI_FSICON          ID(256)
#define IDD_FONTSHOPDLG     ID(257)    
#define IDB_FSABOUT         ID(258)
#define IDB_FSABOUT2        ID(259)
#define IDB_TOOLBAR         ID(260)
#define IDB_FONTCON         ID(261)
#define IDB_ADONIS          ID(262)
#define IDB_PANOSE          ID(263)

//*** String Table IDs
#define IDS_POPUPMENUSTART  0x0990
#define IDS_POPUPMENUEND    IDS_POPUPMENUSTART+3

#ifndef RC_INVOKED
#include "..\res\strtbl.str"
#include "..\res\panose.str"
#endif

//*** Dialog control defines
#define IDB_HELP                0x85c0
#define IDL_COLLECTIONS         0x100
#define IDE_DESC                0x101

#define  ID_FAMILY_WEIGHT       0x0200
#define  ID_FAMILY_DIGITS       0x0300
#define  ID_SERIF_WEIGHT        0x0201
#define  ID_SERIF_DIGITS        0x0301
#define  ID_WEIGHT_WEIGHT       0x0202
#define  ID_WEIGHT_DIGITS       0x0302
#define  ID_PROP_WEIGHT         0x0203
#define  ID_PROP_DIGITS         0x0303
#define  ID_CONTRAST_WEIGHT     0x0204
#define  ID_CONTRAST_DIGITS     0x0304
#define  ID_STROKE_WEIGHT       0x0205
#define  ID_STROKE_DIGITS       0x0305
#define  ID_ARMSTYLE_WEIGHT     0x0206
#define  ID_ARMSTYLE_DIGITS     0x0306
#define  ID_LTRFORM_WEIGHT      0x0207
#define  ID_LTRFORM_DIGITS      0x0307
#define  ID_MIDLINE_WEIGHT      0x0208
#define  ID_MIDLINE_DIGITS      0x0308
#define  ID_XHEIGHT_WEIGHT      0x0209
#define  ID_XHEIGHT_DIGITS      0x0309

#define  ID_UPDATE              100
#define  IDS_START              100
#define  IDS_PUB	              100
#define  IDS_NAME	               101
#define  IDS_PRICE1	            102
#define  IDS_PRICE2	            103
#define  IDS_SIZE1	            104
#define  IDS_SIZE2              105

#define  IDE_START              110
#define  IDE_PUB	               110
#define  IDE_NAME	               111
#define  IDE_PRICE1	            112
#define  IDE_PRICE2	            113
#define  IDE_SIZE1	            114
#define  IDE_SIZE2	            115

#define  IDL_ITEMS              300
#define  IDS_SUBTOTAL           301
#define  IDS_SHIPPING           302
#define  IDS_SURCHARGE          303
#define  IDS_TAX                304
#define  IDS_TOTAL              305
#define  IDS_DOWNLOAD           306
#define  IDB_SUBMIT             307
#define  IDB_COMM               308
#define  IDB_REMOVE             309
#define  IDB_BILLING            310
#define  IDB_SHIPPING           311

#define IDD_CURPASSWORD            5100
#define IDD_NEWPASSWORD1           5101
#define IDD_NEWPASSWORD2           5102
#define IDD_NEWUSER                5103
#define IDD_USERID                 5104
#define IDD_HELP                   5105
#define IDD_STREET2                5106
#define IDD_CITY                   5107
#define IDD_COUNTRY                5108
#define IDD_PHONE                  5109
#define IDD_BILLINGINFO            5110
#define IDD_COMMSETUP              5111
#define IDD_NAME                   5112
#define IDD_CUSTADDRESS            5113
#define IDD_BILLINGMETHOD          5114
#define IDD_EXPDATE                5115
#define IDD_BILLING_GROUP          5116
#define IDD_SHIPPING_GROUP         5117
#define IDD_ACCOUNTNUMBER          5118
#define IDD_COMPANYNAME            5119
#define IDD_STREET1                5120
#define IDD_ZIP                    5121
#define IDD_STATE                  5122
#define IDD_SHIPPINGMETHOD         5123
#define IDD_SHIPPINGINFO           5124
#define IDB_12DAY                  5125
#define IDB_23DAY                  5126
#define IDB_UPS                    5127
#define IDD_TEXT                   5128
#define IDD_MEDIA                  5129
#define IDD_VERSION                5130
#define IDD_COPYRIGHT              5131
#define IDD_WRITTENBY              5132
#define IDD_ALLRIGHTS              5133
#define IDD_PANOSE                 5134
#define IDD_TEXT2                  5135
#define IDD_TEXT3                  5136
#define IDD_TIMEONLINE             5137
#define IDD_PURCHASE               5138
#define IDD_ENTERID                5139
#define IDD_REQUESTDEMOID          5140
