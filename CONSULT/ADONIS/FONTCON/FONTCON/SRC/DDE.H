//*************************************************************
//  File name: dde.h
//
//  Description: 
//      Header file for the dde stuff
//
//  History:    Date       Author     Comment
//               3/18/92   MSM        Created
//
//*************************************************************

#ifndef _DDE_H_
#define _DDE_H_


//*** Function Prototypes
    HDDEDATA CALLBACK fsddeCallback(UINT wType, UINT wFmt, HCONV hConv,
            HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1, DWORD dwData2);

    BOOL WINAPI fsddeConnect(HWND hWnd,LPSTR lpApp,LPSTR lpService,LPSTR lpTopic);
    BOOL WINAPI fsddeDisconnect();
    BOOL WINAPI fsddeExecute(LPSTR lpString, DWORD dwTimeOut);
    BOOL WINAPI fsddePoke( LPCSTR lpszItem, LPVOID lpData, DWORD cb, DWORD dwTimeOut ) ;
    DWORD WINAPI fsddeRequest( LPCSTR lpszItem, LPVOID lpData, DWORD cb, DWORD dwTimeOut ) ;
    DWORD WINAPI fsddeGetError();


//*** DDE error codes
    #define DDE_ERR_OK                  0x0000
    #define DDE_ERR_ALREADY_CONNECTED   0x3001 
    #define DDE_ERR_DML_INIT            0x3002 
    #define DDE_ERR_WINEXEC             0x3003 
    #define DDE_ERR_CONNECT_FAILED      0x3004 
    #define DDE_ERR_NO_CONNECTION       0x3005 

#endif // _DDE_H_

//*** EOF: dde.h
