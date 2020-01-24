/************************************************************************
 *
 *    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  tasks.h
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Definitions for predefined tasks
 *
 *   Revisions:  
 *     01.00.000  9/ 2/91 baw   Wrote it.
 *
 ************************************************************************/

// constant defintions

#define TF_CREATECONNECTOR   (TF_USER + 0x0100)
#define TF_OPENCONNECTOR     (TF_USER + 0x0101)
#define TF_CLOSECONNECTOR    (TF_USER + 0x0102)
#define TF_DESTROYCONNECTOR  (TF_USER + 0x0103)

#define TF_CREATEEMULATOR    (TF_USER + 0x0200)
#define TF_DESTROYEMULATOR   (TF_USER + 0x0201)

#define TF_TRANSMITBLOCK     (TF_USER + 0x0300)
#define TF_RECEIVEBLOCK      (TF_USER + 0x0301)

#define TF_CONNECTOREVENT    (TF_USER + 0x0400)

#define TF_EMULATOREVENT     (TF_USER + 0x0500)

/************************************************************************
 * End of File: tasks.h
 ************************************************************************/

