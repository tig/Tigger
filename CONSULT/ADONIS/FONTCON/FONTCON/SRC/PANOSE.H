//*************************************************************
//  File name: panose.h
//
//  Description: 
//      Header file for the PANOSE stuff
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

#ifndef _PANOSE_H_
#define _PANOSE_H_

//*** Function prototypes
    BOOL CALLBACK PanoseDlg( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
    VOID FillPanoseDialog( HWND hDlg );
    VOID SuckPanoseDlgData( HWND hDlg );
    VOID DoPanoseNumbers(VOID);

#if 0 // defined in RES.H   
//*** Dialog controls
//*** These defines must always start at a 0x??00 and increment by 0x0001
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

    #define  ID_UPDATE              0x100
#endif

#endif // _PANOSE_H_

//*** EOF: panose.h
