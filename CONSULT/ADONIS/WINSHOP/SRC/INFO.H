/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
extern LONG FAR PASCAL WSShippingInfoDlgProc (HWND hWnd, unsigned message, WORD wParam, LONG lParam);
extern LONG FAR PASCAL WSCustomerInfoDlgProc (HWND hWnd, unsigned message, WORD wParam, LONG lParam);
extern BOOL            ValidShippingInfo(void);
extern BOOL            GetInfo(HWND hWndDlg, WORD wWhichDialog);
