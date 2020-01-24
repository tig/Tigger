/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Communication Routines (COMMLO.C)                               *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the low use routines to support            *
*             communications.                                                 *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "dialogs.h"


static char * near LoadToken(char *, char *, int);


/*****************************************************************************/
/*  InitCommunications()						     */
/*									     */
/*****************************************************************************/
BOOL InitCommunications()
{
  char buf1[8], buf2[128];
  register i;


  nCommID = -1;
  bConnected = FALSE;

  Comm.cPort	 = 0;
  Comm.nSpeed	 = 1200;
  Comm.cDataBits = 8;
  Comm.cParity	 = NOPARITY;
  Comm.cStopBits = ONESTOPBIT;
  Comm.nBreakLen = 500;
  Comm.cLocal	 = 0;
  Comm.cFlow	 = 0;
  Comm.cXonChar  = 0;
  Comm.cXoffChar = 0;

  ReadModemParameters(IDS_MODEM_HAYES1200);
  LoadModemParameters();

  CommPortUpdate();

  CommReset();

  return TRUE;
}


/*****************************************************************************/
/*  TermCommunications()						     */
/*									     */
/*****************************************************************************/
void TermCommunications()
{
}


/*****************************************************************************/
/*  ReadModemParameters(int)						     */
/*									     */
/*****************************************************************************/
void ReadModemParameters(nString)
  int  nString;
{
  char buf[8], tmp[512];
  register char *ptr;

  if (nString == IDS_MODEM_CUSTOM)
    {
    LoadCustomParameters();
    return;
    }

  LoadString(hWndInst, nString, tmp, sizeof(tmp));

  ptr = LoadToken(tmp, Modem.szName, sizeof(Modem.szName));
  ptr = LoadToken(ptr, Modem.szDialPrefix, sizeof(Modem.szDialPrefix));
  ptr = LoadToken(ptr, Modem.szDialSuffix, sizeof(Modem.szDialSuffix));
  ptr = LoadToken(ptr, Modem.szAnswerSetup, sizeof(Modem.szAnswerSetup));
  ptr = LoadToken(ptr, Modem.szModemInit, sizeof(Modem.szModemInit));
  ptr = LoadToken(ptr, Modem.szModemHangup, sizeof(Modem.szModemHangup));

  ptr = LoadToken(ptr, buf, sizeof(buf));
  if (buf[1] == '1')
    Modem.cChange = 1;
  else
    Modem.cChange = 0;

  ptr = LoadToken(ptr, Modem.szCommandOk, sizeof(Modem.szCommandOk));
  ptr = LoadToken(ptr, Modem.szCommandError, sizeof(Modem.szCommandError));
  ptr = LoadToken(ptr, Modem.szConnect, sizeof(Modem.szConnect));
  ptr = LoadToken(ptr, Modem.szConnectSpeed, sizeof(Modem.szConnectSpeed));
  ptr = LoadToken(ptr, Modem.szConnectARQ, sizeof(Modem.szConnectARQ));
  ptr = LoadToken(ptr, Modem.szDialTone, sizeof(Modem.szDialTone));
  ptr = LoadToken(ptr, Modem.szRingDetect, sizeof(Modem.szRingDetect));
  ptr = LoadToken(ptr, Modem.szNoDialTone, sizeof(Modem.szNoDialTone));
  ptr = LoadToken(ptr, Modem.szBusy, sizeof(Modem.szBusy));
  ptr = LoadToken(ptr, Modem.szNoCarrier, sizeof(Modem.szNoCarrier));
}


/*****************************************************************************/
/*  LoadModemParameters()						     */
/*									     */
/*****************************************************************************/
void LoadModemParameters()
{
  char buf[64];


  strcpy(buf, "ModemCom1");
  buf[8] += cCommPort;


  strcpy(&buf[9], "Name");
  GetProfileData(szAppName, buf, Modem.szName,
		   Modem.szName, sizeof(Modem.szName));

  strcpy(&buf[9], "Prefix");
  GetProfileData(szAppName, buf, Modem.szDialPrefix,
		   Modem.szDialPrefix, sizeof(Modem.szDialPrefix));
  strcpy(&buf[9], "Suffix");
  GetProfileData(szAppName, buf, Modem.szDialSuffix,
		   Modem.szDialSuffix, sizeof(Modem.szDialSuffix));
  strcpy(&buf[9], "Setup");
  GetProfileData(szAppName, buf, Modem.szAnswerSetup,
		   Modem.szAnswerSetup, sizeof(Modem.szAnswerSetup));
  strcpy(&buf[9], "Init");
  GetProfileData(szAppName, buf, Modem.szModemInit,
		   Modem.szModemInit, sizeof(Modem.szModemInit));
  strcpy(&buf[9], "Hangup");
  GetProfileData(szAppName, buf, Modem.szModemHangup,
		   Modem.szModemHangup, sizeof(Modem.szModemHangup));

  strcpy(&buf[9], "Change");
  Modem.cChange = (char)GetProfileInt(szAppName, buf, 1);

  strcpy(&buf[9], "Ok");
  GetProfileData(szAppName, buf, Modem.szCommandOk,
		   Modem.szCommandOk, sizeof(Modem.szCommandOk));
  strcpy(&buf[9], "Error");
  GetProfileData(szAppName, buf, Modem.szCommandError,
		   Modem.szCommandError, sizeof(Modem.szCommandError));
  strcpy(&buf[9], "Connect");
  GetProfileData(szAppName, buf, Modem.szConnect,
		   Modem.szConnect, sizeof(Modem.szConnect));
  strcpy(&buf[9], "Speed");
  GetProfileData(szAppName, buf, Modem.szConnectSpeed,
		   Modem.szConnectSpeed, sizeof(Modem.szConnectSpeed));
  strcpy(&buf[9], "ARQ");
  GetProfileData(szAppName, buf, Modem.szConnectARQ,
		   Modem.szConnectARQ, sizeof(Modem.szConnectARQ));
  strcpy(&buf[9], "Dial");
  GetProfileData(szAppName, buf, Modem.szDialTone,
		   Modem.szDialTone, sizeof(Modem.szDialTone));
  strcpy(&buf[9], "Ring");
  GetProfileData(szAppName, buf, Modem.szRingDetect,
		   Modem.szRingDetect, sizeof(Modem.szRingDetect));
  strcpy(&buf[9], "NoDial");
  GetProfileData(szAppName, buf, Modem.szNoDialTone,
		   Modem.szNoDialTone, sizeof(Modem.szNoDialTone));
  strcpy(&buf[9], "Busy");
  GetProfileData(szAppName, buf, Modem.szBusy,
		   Modem.szBusy, sizeof(Modem.szBusy));
  strcpy(&buf[9], "Carrier");
  GetProfileData(szAppName, buf, Modem.szNoCarrier,
		   Modem.szNoCarrier, sizeof(Modem.szNoCarrier));
}


/*****************************************************************************/
/*  SaveModemParameters()						     */
/*									     */
/*****************************************************************************/
void SaveModemParameters()
{
  char buf[64];
  char tmp[32];
  int  i;


  if (Modem.szName[1] == 'C' && Modem.szName[2] == 'U')
    SaveCustomParameters();

  strcpy(buf, "ModemCom1");
  buf[8] += cCommPort;

  strcpy(&buf[9], "Name");
  WriteProfileData(szAppName, buf, Modem.szName);

  strcpy(&buf[9], "Prefix");
  WriteProfileData(szAppName, buf, Modem.szDialPrefix);

  strcpy(&buf[9], "Suffix");
  WriteProfileData(szAppName, buf, Modem.szDialSuffix);

  strcpy(&buf[9], "Setup");
  WriteProfileData(szAppName, buf, Modem.szAnswerSetup);

  strcpy(&buf[9], "Init");
  WriteProfileData(szAppName, buf, Modem.szModemInit);

  strcpy(&buf[9], "Hangup");
  WriteProfileData(szAppName, buf, Modem.szModemHangup);

  strcpy(&buf[9], "Change");
  if (Modem.cChange != (char)GetProfileInt(szAppName, buf, 1))
    {
    tmp[0] = (char)('0' + Modem.cChange);
    tmp[1] = NULL;
    WriteProfileString(szAppName, buf, tmp);
    }

  strcpy(&buf[9], "Ok");
  WriteProfileData(szAppName, buf, Modem.szCommandOk);

  strcpy(&buf[9], "Error");
  WriteProfileData(szAppName, buf, Modem.szCommandError);

  strcpy(&buf[9], "Connect");
  WriteProfileData(szAppName, buf, Modem.szConnect);

  strcpy(&buf[9], "Speed");
  WriteProfileData(szAppName, buf, Modem.szConnectSpeed);

  strcpy(&buf[9], "ARQ");
  WriteProfileData(szAppName, buf, Modem.szConnectARQ);

  strcpy(&buf[9], "Dial");
  WriteProfileData(szAppName, buf, Modem.szDialTone);

  strcpy(&buf[9], "Ring");
  WriteProfileData(szAppName, buf, Modem.szRingDetect);

  strcpy(&buf[9], "NoDial");
  WriteProfileData(szAppName, buf, Modem.szNoDialTone);

  strcpy(&buf[9], "Busy");
  WriteProfileData(szAppName, buf, Modem.szBusy);

  strcpy(&buf[9], "Carrier");
  WriteProfileData(szAppName, buf, Modem.szNoCarrier);
}

/*****************************************************************************/
/*  LoadCustomParameters()						     */
/*									     */
/*****************************************************************************/
void LoadCustomParameters()
{
  char buf[32];


  ReadModemParameters(IDS_MODEM_HAYES1200);

  Modem.szName[0] = 6;
  Modem.szName[1] = 'C';
  Modem.szName[2] = 'U';
  Modem.szName[3] = 'S';
  Modem.szName[4] = 'T';
  Modem.szName[5] = 'O';
  Modem.szName[6] = 'M';
  Modem.szName[7] = NULL;

  strcpy(buf, "CustomPrefix");
  GetProfileData(szAppName, buf, Modem.szDialPrefix,
		   Modem.szDialPrefix, sizeof(Modem.szDialPrefix));
  strcpy(buf, "CustomSuffix");
  GetProfileData(szAppName, buf, Modem.szDialSuffix,
		   Modem.szDialSuffix, sizeof(Modem.szDialSuffix));
  strcpy(buf, "CustomSetup");
  GetProfileData(szAppName, buf, Modem.szAnswerSetup,
		   Modem.szAnswerSetup, sizeof(Modem.szAnswerSetup));
  strcpy(buf, "CustomInit");
  GetProfileData(szAppName, buf, Modem.szModemInit,
		   Modem.szModemInit, sizeof(Modem.szModemInit));
  strcpy(buf, "CustomHangup");
  GetProfileData(szAppName, buf, Modem.szModemHangup,
		   Modem.szModemHangup, sizeof(Modem.szModemHangup));

  strcpy(buf, "CustomChange");
  Modem.cChange = (char)GetProfileInt(szAppName, buf, 1);

  strcpy(buf, "CustomOk");
  GetProfileData(szAppName, buf, Modem.szCommandOk,
		   Modem.szCommandOk, sizeof(Modem.szCommandOk));
  strcpy(buf, "CustomError");
  GetProfileData(szAppName, buf, Modem.szCommandError,
		   Modem.szCommandError, sizeof(Modem.szCommandError));
  strcpy(buf, "CustomConnect");
  GetProfileData(szAppName, buf, Modem.szConnect,
		   Modem.szConnect, sizeof(Modem.szConnect));
  strcpy(buf, "CustomSpeed");
  GetProfileData(szAppName, buf, Modem.szConnectSpeed,
		   Modem.szConnectSpeed, sizeof(Modem.szConnectSpeed));
  strcpy(buf, "CustomARQ");
  GetProfileData(szAppName, buf, Modem.szConnectARQ,
		   Modem.szConnectARQ, sizeof(Modem.szConnectARQ));
  strcpy(buf, "CustomDial");
  GetProfileData(szAppName, buf, Modem.szDialTone,
		   Modem.szDialTone, sizeof(Modem.szDialTone));
  strcpy(buf, "CustomRing");
  GetProfileData(szAppName, buf, Modem.szRingDetect,
		   Modem.szRingDetect, sizeof(Modem.szRingDetect));
  strcpy(buf, "CustomNoDial");
  GetProfileData(szAppName, buf, Modem.szNoDialTone,
		   Modem.szNoDialTone, sizeof(Modem.szNoDialTone));
  strcpy(buf, "CustomBusy");
  GetProfileData(szAppName, buf, Modem.szBusy,
		   Modem.szBusy, sizeof(Modem.szBusy));
  strcpy(buf, "CustomCarrier");
  GetProfileData(szAppName, buf, Modem.szNoCarrier,
		   Modem.szNoCarrier, sizeof(Modem.szNoCarrier));
}


/*****************************************************************************/
/*  SaveCustomParameters()						     */
/*									     */
/*****************************************************************************/
void SaveCustomParameters()
{
  char buf[32];
  char tmp[32];
  register int  i;


  strcpy(buf, "CustomPrefix");
  WriteProfileData(szAppName, buf, Modem.szDialPrefix);

  strcpy(buf, "CustomSuffix");
  WriteProfileData(szAppName, buf, Modem.szDialSuffix);

  strcpy(buf, "CustomSetup");
  WriteProfileData(szAppName, buf, Modem.szAnswerSetup);

  strcpy(buf, "CustomInit");
  WriteProfileData(szAppName, buf, Modem.szModemInit);

  strcpy(buf, "CustomHangup");
  WriteProfileData(szAppName, buf, Modem.szModemHangup);

  strcpy(buf, "CustomChange");
  if (Modem.cChange != (char)GetProfileInt(szAppName, buf, 1))
    {
    tmp[0] = (char)('0' + Modem.cChange);
    tmp[1] = NULL;
    WriteProfileString(szAppName, buf, tmp);
    }

  strcpy(buf, "CustomOk");
  WriteProfileData(szAppName, buf, Modem.szCommandOk);

  strcpy(buf, "CustomError");
  WriteProfileData(szAppName, buf, Modem.szCommandError);

  strcpy(buf, "CustomConnect");
  WriteProfileData(szAppName, buf, Modem.szConnect);

  strcpy(buf, "CustomSpeed");
  WriteProfileData(szAppName, buf, Modem.szConnectSpeed);

  strcpy(buf, "CustomARQ");
  WriteProfileData(szAppName, buf, Modem.szConnectARQ);

  strcpy(buf, "CustomDial");
  WriteProfileData(szAppName, buf, Modem.szDialTone);

  strcpy(buf, "CustomRing");
  WriteProfileData(szAppName, buf, Modem.szRingDetect);

  strcpy(buf, "CustomNoDial");
  WriteProfileData(szAppName, buf, Modem.szNoDialTone);

  strcpy(buf, "CustomBusy");
  WriteProfileData(szAppName, buf, Modem.szBusy);

  strcpy(buf, "CustomCarrier");
  WriteProfileData(szAppName, buf, Modem.szNoCarrier);
}

static char * near LoadToken(ptr, buf, len)
  char *ptr;
  char *buf;
  int  len;
{
  char tmp[128];
  register i;

  i = 0;
  while (*ptr != 0 && *ptr != ',')
    {
    if (*ptr != ';')
      tmp[i++] = *ptr;
    else
      tmp[i++] = ',';
    ptr++;
    }
  tmp[i] = 0;

  LibDestore(tmp, buf, len);

  while (*ptr != 0 && *ptr != ',')
    ptr++;

  if (*ptr == ',')
    ptr++;

  return (ptr);
}
