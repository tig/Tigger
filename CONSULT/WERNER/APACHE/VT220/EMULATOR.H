/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  emulator.h
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  This contains the create structure definition
 *               for emulator types.
 *
 *   Revisions:  
 *     01.00.000  7/ 9/91 baw   Wrote it.
 *
 ************************************************************************/

typedef struct tagEMULATORCREATESTRUCT
{
   WORD        wSize ;
   LPSTR       szSettingsSection ;
   LPSTR       szSettingsFile ;
   EXTLOGFONT  lfBase ;

} EMULATORCREATESTRUCT, FAR *LPEMULATORCREATESTRUCT ;

/************************************************************************
 * End of File: emulator.h
 ************************************************************************/

