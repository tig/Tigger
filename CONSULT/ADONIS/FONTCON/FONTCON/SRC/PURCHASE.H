//*************************************************************
//  File name: purchase.h
//
//  Description: 
//      Header file for the purchase dialog
//
//  History:    Date       Author     Comment
//               3/29/92   MSM        Created
//
//*************************************************************

#ifndef _PURCHASE_H_
#define _PURCHASE_H_

#define AMEX_SURCHARGE(X)   ((X*50L+500)/1000L)
#define WA_TAXES(X)         ((X*82L+500)/1000L)

//*** Function prototypes
    BOOL CALLBACK PurchaseProc( HWND, UINT, WPARAM, LPARAM);
   UINT WINAPI FillPurchaseDialog( HWND hDlg ) ;
VOID WINAPI PaintDollarAmount( HDC hDC, int nDP, int nY, DWORD dwPrice );

typedef struct tagPURCHASEDNODE
{
   LPDBNODE lpdn ;
   DWORD    dwOrderID ;

} PURCHASEDNODE, FAR * LPPURCHASEDNODE ;

#endif // _PURCHASE_H_

//*** EOF: purchase.h
