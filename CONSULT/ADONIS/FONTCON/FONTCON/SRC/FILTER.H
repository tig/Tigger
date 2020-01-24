//*************************************************************
//  File name: filter.h
//
//  Description: 
//      Header file for the filtering
//
//  History:    Date       Author     Comment
//               3/25/92   MSM        Created
//
//*************************************************************

#ifndef _FILTER_H_
#define _FILTER_H_

//*** Function prototypes
    BOOL CALLBACK SetFilterProc( HWND, UINT, WPARAM, LPARAM);
    VOID FillFilterDialog( HWND hDlg );
    BOOL SuckFilterDlgData( HWND hDlg );

#endif // _FILTER_H_

//*** EOF: filter.h
