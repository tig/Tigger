#ifndef STDCOMMH
#define STDCOMMH
#include "stdinc.h"

#define SOH                   1     /* CTRL F  Ack                  */
#define ACK                    6    /* CTRL F for UpLoad            */
#define NAK                  0x15    /* CTRL U for UpLoad            */  
#define EOT                   4     /* EOT char           */
#define CTRLX                0x18
#define RESEND              NAK

typedef unsigned char MSGTYPE;
typedef WORD CRC;

#define DATABLOCKSIZE 128
#define MAX_DATA_FILE_SIZE_BYTES 0x40000
#define MAX_DATA_FILE_SIZE_BLOCKS (0x40000 / DATABLOCKSIZE)
#define MAXRETRIES 5
#define STD_FILENAME_SIZE 12 
#define PADCHAR '~'

#pragma pack(1)

typedef struct
{
    BYTE Soh;
    BYTE BlockNum,BlockNumCom;
    BYTE Data[DATABLOCKSIZE];
    CRC Crc;
} XMBLOCK;

#pragma pack() // reset to default

#endif



