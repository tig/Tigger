/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*  Include:   Communication Devices (DEVICE.H)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.01 09/01/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#define DEVICE_STANDARD     0
#define DEVICE_INT14H       1
#define DEVICE_FILEPATH     2

#define DEVICE_BAUD_110     0x00
#define DEVICE_BAUD_150     0x20
#define DEVICE_BAUD_300     0x40
#define DEVICE_BAUD_600     0x60
#define DEVICE_BAUD_1200    0x80
#define DEVICE_BAUD_2400    0xA0
#define DEVICE_BAUD_4800    0xC0
#define DEVICE_BAUD_9600    0xE0

#define DEVICE_PARITY_NONE  0x00
#define DEVICE_PARITY_ODD   0x08
#define DEVICE_PARITY_EVEN  0x18

#define DEVICE_STOPBITS_1   0x00
#define DEVICE_STOPBITS_2   0x04

#define DEVICE_DATABITS_7   0x02
#define DEVICE_DATABITS_8   0x03

#define DEVICE_CARRIER      0x01

extern USHORT usDevice;

USHORT DeviceOpen(USHORT usDevice, PSZ pszDevice);
USHORT DeviceClose(VOID);
USHORT DeviceDataRead(PBYTE pData, USHORT cData, PUSHORT pusData);
USHORT DeviceDataWrite(PBYTE pData, USHORT cData, PUSHORT pusData);
USHORT DeviceTextRead(PBYTE pData, USHORT cData, PUSHORT pusData);
USHORT DeviceTextMode(BOOL bMode);
USHORT DeviceFlush(BOOL bInput, BOOL bOutput);
USHORT DeviceParams(BYTE byBaud, BYTE byParity, BYTE byStopBits, BYTE byBits);
USHORT DeviceHandshake(BOOL bSoftware, BOOL bHardware);
USHORT DeviceStatus(PUSHORT pusStatus);
