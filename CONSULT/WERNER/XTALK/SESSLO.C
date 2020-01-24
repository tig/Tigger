/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Session Data Init Module (SESSLO.C)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the logic to initialize the session data   *
*             fields.                                                         *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "sess.h"


BOOL InitSession()
{
  NewSessionData();

  return (TRUE);
}

void NewSessionData()
{
  memset(&Sess, 0, sizeof(Sess));

  Sess.cLogOption = SESSION_NORMAL;

  Sess.nPatience    = 45;
  Sess.nRedialCount = 10;
  Sess.nRedialWait  = 10;

  Sess.fGraphics = TRUE;
}
