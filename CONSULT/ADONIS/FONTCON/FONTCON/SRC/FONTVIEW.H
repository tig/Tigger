//*************************************************************
//  File name: fontview.h
//
//  Description: 
//      Header file for the fontview class
//
//  History:    Date       Author     Comment
//               2/13/92   MSM        Created
//
//*************************************************************

#ifndef _FONTVIEW_H_
#define _FONTVIEW_H_

#define SZ_FONTVIEWCLASS    "FONTSHOP.FONTVIEW"

#define FV_BMPQSIZE     50

#define FV_SETNODE      WM_USER+1000
#define FV_GETNODE      WM_USER+1001

#define FV_WNDEXTRA    2
#define FVWE_PFVQ      0

typedef struct
{
    LPDBNODE lpdb;
    LPFACE   alpFaces[ FV_BMPQSIZE ];
} FVQ, *PFVQ;

#define SETPFVQ(HW,PQ)  (PFVQ)SetWindowWord(HW, FVWE_PFVQ, (WORD)PQ)
#define GETPFVQ(HW)     (PFVQ)GetWindowWord(HW, FVWE_PFVQ)

//*** Function prototypes
    LONG WINAPI FVWndProc(HWND,UINT,WPARAM,LPARAM);
    BOOL WINAPI FVSetCurrentNode(HWND,LPDBNODE);
    VOID NEAR PASCAL FVAddFaceToQ(PFVQ,LPFACE);
    VOID WINAPI FVFreeQ(HWND);
    VOID WINAPI FVSetStatusToNode( LPDBNODE );
    VOID NEAR PASCAL FVPaintBitmap( HWND );

 HBITMAP WINAPI FVGetSmallThumb( LPFACE );
    VOID WINAPI FVFreeBitmap(LPFACE);


#endif

//*** EOF: fontview.h
