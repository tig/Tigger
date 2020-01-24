/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
extern LONG FAR PASCAL WSBillingInfoDlgProc (HWND, unsigned, WORD, LONG);
extern LONG FAR PASCAL WSSelectBillingDlgProc (HWND, unsigned, WORD, LONG);

extern BOOL            ValidBillingInfo (WORD aCard);
extern int             GetCardNumber (int buttonNumber);
extern int             StripSpaces (LPSTR oldNumber, 
                                    LPSTR newNumber, int size);
extern void            StripCardDashes (LPSTR ccNumber);
