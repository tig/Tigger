/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Communication Devices (DEVICE.C)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.01 09/01/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include "lexis.h"
#include "device.h"


  USHORT usDeviceActive;

  static BOOL   bDevice = 0;
  static BOOL   bTextMode;
  static HANDLE hDevice;


USHORT DeviceOpen(USHORT usDevice, PSZ pszDevice)
  {
  usDeviceActive = usDevice;

  switch (usDevice)
    {
    case DEVICE_STANDARD:
      break;

    case DEVICE_INT14H:
      {
      if (*pszDevice != 'C' && *pszDevice != 'c')
        return (1);
      pszDevice++;
      if (*pszDevice != 'O' && *pszDevice != 'o')
        return (1);
      pszDevice++;
      if (*pszDevice != 'M' && *pszDevice != 'm')
        return (1);
      pszDevice++;
      hDevice = atoi(pszDevice) - 1;
      bDevice = TRUE;
      }
      break;

    case DEVICE_FILEPATH:
      break;
    }

  bTextMode = TRUE;

  return (0);
  }


USHORT DeviceClose()
  {
  switch (usDeviceActive)
    {
    case DEVICE_STANDARD:
      break;

    case DEVICE_INT14H:
      bDevice = FALSE;
      break;

    case DEVICE_FILEPATH:
      break;
    }

  return (0);
  }


USHORT DeviceDataRead(PBYTE pData, USHORT cData, PUSHORT pusData)
  {
  switch (usDeviceActive)
    {
    case DEVICE_STANDARD:
      break;

    case DEVICE_INT14H:
      {
      union REGS inregs, otregs;
      register i;

      inregs.x.dx = hDevice;
      *pusData = 0;
      for (i = 0; i < cData; i++)
        {
        inregs.h.ah = 0x03;
        int86(0x14, &inregs, &otregs);
        if ((otregs.h.ah & 0x01) == 0x00)
          break;
        inregs.h.ah = 0x02;
        int86(0x14, &inregs, &otregs);
        if (otregs.h.ah & 0x80)
          break;
        *pData++ = otregs.h.al;
        (*pusData)++;
        }
      }
      break;

    case DEVICE_FILEPATH:
      break;
    }

  return (0);
  }


USHORT DeviceDataWrite(PBYTE pData, USHORT cData, PUSHORT pusData)
  {
  switch (usDeviceActive)
    {
    case DEVICE_STANDARD:
      break;

    case DEVICE_INT14H:
      {
      union REGS inregs, otregs;
      register i;

      inregs.h.ah = 0x01;
      inregs.x.dx = hDevice;
      *pusData = 0;
      for (i = 0; i < cData; i++)
        {
        inregs.h.al = *pData++;
        int86(0x14, &inregs, &otregs);
        if (otregs.h.ah & 0x80)
          break;
        (*pusData)++;
        }
      }
      break;

    case DEVICE_FILEPATH:
      break;
    }

  return (0);
  }


USHORT DeviceTextRead(PBYTE pData, USHORT cData, PUSHORT pusData)
  {
  switch (usDeviceActive)
    {
    case DEVICE_STANDARD:
      break;

    case DEVICE_INT14H:
      break;

    case DEVICE_FILEPATH:
      break;
    }

  return (0);
  }


USHORT DeviceTextMode(BOOL bMode)
  {
  bTextMode = bMode;

  return (0);
  }


USHORT DeviceFlush(BOOL bInput, BOOL bOutput)
  {
  switch (usDeviceActive)
    {
    case DEVICE_STANDARD:
      break;

    case DEVICE_INT14H:
      break;

    case DEVICE_FILEPATH:
      break;
    }

  return (0);
  }


USHORT DeviceParams(BYTE byBaud, BYTE byParity, BYTE byStopBits, BYTE byBits)
  {
  switch (usDeviceActive)
    {
    case DEVICE_STANDARD:
      break;

    case DEVICE_INT14H:
      {
      union REGS regs;

      regs.h.ah = 0;
      regs.h.al = byBaud | byParity | byStopBits | byBits;
      regs.x.dx = hDevice;
      int86(0x14, &regs, &regs);
      }
      break;

    case DEVICE_FILEPATH:
      break;
    }

  return (0);
  }


USHORT DeviceHandshake(BOOL bSoftware, BOOL bHardware)
  {
  switch (usDeviceActive)
    {
    case DEVICE_STANDARD:
      break;

    case DEVICE_INT14H:
      break;

    case DEVICE_FILEPATH:
      break;
    }

  return (0);
  }


USHORT DeviceStatus(PUSHORT pusStatus)
  {
  switch (usDeviceActive)
    {
    case DEVICE_STANDARD:
      break;

    case DEVICE_INT14H:
      {
      union REGS regs;

      regs.h.ah = 0x03;
      regs.x.dx = hDevice;
      int86(0x14, &regs, &regs);
      if (regs.h.al & 0x80)
        *pusStatus = DEVICE_CARRIER;
      else
        *pusStatus = 0;
      }
      break;

    case DEVICE_FILEPATH:
      break;
    }

  return (0);
  }
