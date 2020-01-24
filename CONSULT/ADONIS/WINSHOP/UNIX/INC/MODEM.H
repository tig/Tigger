#include "stdinc.h"

#define WAIT_FOR_BBS 5000L // time in milliseconds to wait for response from bbs
#define COMM_OK             '0'
#define COMM_RING           '2'
#define COMM_NO_CARRIER     '3'
#define COMM_ERROR          '4'
#define COMM_NO_DIAL_TONE   '6'
#define COMM_BUSY           '7'
#define COMM_NO_ANSWER      '8'

/* Modem Result Codes, in Numeric format, as strings */
#define	RC_OK				"0" 
#define	RC_RING				"2" 
#define	RC_ERROR			"4" 

#define	RC_CONNECT_300		"1" 
#define	RC_CONNECT_1200		"5" 
#define	RC_CONNECT_2400		"10" 
#define	RC_CONNECT_4800		"11" 
#define	RC_CONNECT_9600		"12" 
#define	RC_CONNECT_19200	"14" 
#define	RC_CONNECT_38400	"28" 

#define	RC_CARRIER_300		"40" 
#define	RC_CARRIER_1200		"46" 
#define	RC_CARRIER_2400		"47" 
#define	RC_CARRIER_4800		"48" 
#define	RC_CARRIER_9600		"50" 

#define	RC_COMPRESSION_MNP5	"66"
#define	RC_COMPRESSION_V42BIS	"67"
#define	RC_COMPRESSION_NONE	"69"

#define	RC_PROTOCOL_NONE	"70"
#define	RC_PROTOCOL_V42LAPM	"77"
#define	RC_PROTOCOL_MNP		"80"

int FAR PASCAL Connect(char *whichtty, char *PhoneNum ,WORD howlongtowait);
void FAR PASCAL HangUp(HANDLE hCom, char *whichtty);
int WriteModemCommand(HANDLE hCom, char *szCommand);
int WaitForConnection(HANDLE hCom, WORD howlong);


