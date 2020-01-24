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

#ifndef _RES_H_
#define _RES_H_

//*** Child Window IDs
    #define ID_STATUS               0x0900
    #define ID_SLIDER               0x0901
    #define ID_ELB                  0x0902
    #define ID_FONTVIEW             0x0903

//*** Menu IDs
    #define IDM_FILE_IMPORT         0x1000
    #define IDM_FILE_SAVE           0x1001
    #define IDM_FILE_FONT           0x1002
    #define IDM_FILE_ABOUT          0x1003
    #define IDM_FILE_EXIT           0x1004

//*** Resource IDs
    #define IDI_FSICON          ID(256)
    #define IDD_FONTSHOPDLG     ID(257)    

//*** String Table IDs
    #define IDS_APPNAME         0x0081
    #define IDS_MAINCLASS       0x0082
    #define IDS_MAINMENU        0x0083
    #define IDS_INIFILE         0x0084
    #define IDS_WNDPOS          0x0085
    #define IDS_FONT            0x0086
    #define IDS_CONFIG          0x0087
    #define IDS_SLIDER          0x0088
    #define IDS_SLDRNAME        0x0089
    #define IDS_SLDRPRICE       0x008a
    #define IDS_SLDRSIZE        0x008b
    #define IDS_SLDRPUBLISHER   0x008c
    #define IDS_STAT_READY      0x008d
    #define IDS_PANOSE_STRING   0x008e

    #define IDS_MEMDENIED       0x0090
    #define IDS_OPENFAILED      0x0091
    #define IDS_READFAILED      0x0092
    #define IDS_WRITEFAILED     0x0093
    #define IDS_ININOTFOUND     0x0094
    #define IDS_FILENOTFOUND2   0x0095
    #define IDS_NOFAMILIESFOUND 0x0096
    #define IDS_FAMILYNOTFOUND  0x0097
    #define IDS_FACENOTFOUND    0x0098
    #define IDS_STYLENOTFOUND   0x0099
    #define IDS_TTFNOTFOUND     0x009a
    #define IDS_INVALIDPRICE    0x009b
    #define IDS_INVALIDFONT     0x009c
    #define IDS_SAVEFAILED      0x009d
    #define IDS_INVALIDID       0x009e
    #define IDS_GETTMFAILED     0x009f

    #define IDS_PUBLISHER       0x00a0
    #define IDS_NAME            0x00a1
    #define IDS_ID              0x00a2
    #define IDS_SHORTCOPYRIGHT  0x00a3
    #define IDS_LONGCOPYRIGHT   0x00a4
    #define IDS_FAMILY_F        0x00a5
    #define IDS_FACE_F          0x00a6
    #define IDS_STYLE_F         0x00a7
    #define IDS_PRICE           0x00a8
    #define IDS_COLLECTIONS     0x00a9
    #define IDS_COLLECTION_F    0x00aa
    #define IDS_FACES           0x00ab
    #define IDS_INGRESID        0x00ac
    #define IDS_INVALIDIID      0x00ad
    #define IDS_INVALIDSHIPPINGRATES 0x00ae

    #define IDS_NOBMPPRESENT    0x00b0
    #define IDS_BIGBMPPRESENT   0x00b1
    #define IDS_ERRCREATEBMP    0x00b2
    #define IDS_ERRSAVEBMP      0x00b3
    #define IDS_NOCOLLFOUND     0x00b4
    #define IDS_COLLNOTFOUND    0x00b5
    #define IDS_COLLFILENOTFOUND 0x00b6
    #define IDS_FACESNOTFOUND   0x00b7

    #define IDS_MAKEALL_ASM     0x00b8
    #define IDS_MAKEALL_RCR     0x00b9
    #define IDS_MAKEALL_LINK    0x00ba
    #define IDS_MAKEALL_RCV     0x00bb
    #define IDS_MAKEALL_DONE    0x00bc
    #define IDS_DEF_LIBRARY     0x00bd
    #define IDS_DEF_DESC        0x00be
    #define IDS_DEF_EXECODEDATA 0x00bf

    #define IDS_SMNU            0x0980
    #define IDS_SMNU_SIZE       0x0981
    #define IDS_SMNU_MOVE       0x0982
    #define IDS_SMNU_MINIMIZE   0x0983
    #define IDS_SMNU_MAXIMIZE   0x0984
    #define IDS_SMNU_CLOSE      0x0985
    #define IDS_SMNU_RESTORE    0x0986
    #define IDS_SMNU_TASKLIST   0x0987

    #define IDS_POPUPMENUSTART  0x0990
    #define IDS_FILEMENU        IDS_POPUPMENUSTART+0
    #define IDS_POPUPMENUEND    IDS_POPUPMENUSTART+0



#endif

//*** EOF: res.h
