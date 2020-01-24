#include <sys/types.h>
void setblock(HANDLE hCom, BOOL onoff);
extern char *savetty;  
extern int set_tty_baud(HANDLE hCom, WORD baud);
extern int set_tty_local(HANDLE hCom, BOOL bOnOff);
extern int set_tty_flow(HANDLE hCom);
extern int set_tty_xclude(HANDLE hCom, BOOL bOnOff);
extern  int setraw(int hCom);
extern  void setwaitparms(int hFile,int chars,int time);
extern  void SendXoff(int hCom);
extern  void SendXon(int hCom);
extern  int Send_Message(int hCom,unsigned char Msg);
extern  int SendByte(int hCom,unsigned char ByteToSend);
extern  int SendBuf(int hCom,unsigned char *buf,int iBufLen);
extern int SendCRC(HANDLE hCom, BYTE *buf, int iBufLen);
extern  int ReceiveCRC(int hCom,unsigned char *buf,int iBufLen);
extern  int ReceiveStr(int hCom,unsigned char *buf,
				int iBufLen, char *str, WORD TimeToWait);
extern  int ReceiveBuf(int hCom,unsigned char *buf,
				int iBufLen, WORD TimeToWait);
extern  int ReceiveVarBuf(int hCom,unsigned char *buf,
	   int iBufLen, WORD TimeToWait, BYTE TerminateChar, int *len);
extern void restore_tty(HANDLE hFile);
extern void save_tty(HANDLE hFile);
extern void flush(HANDLE hCom,int whichQ);
extern time_t Elapsed_Time(BOOL bReset);
extern void LookIO ( int     device, FILE*   fout );
CRC CalcCRC(unsigned char *cCRCBuf,int dCRC_BUFFSIZE);
int
set_tty_parms(HANDLE hCom, 
			  short baud, 
			  char parity, 
			  char databits, 
			  char stopbits);
