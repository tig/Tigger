/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: AI Data Scanning Include (AISCAN.H)                              */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: This module receives pre-formatted line data that is being       */
/*           received and sent through the communication port.  The AISCAN    */
/*           module examines the data and set flags and/or execute routines.  */
/*           Currently, all we do is keep track of the client and search      */
/*           library so it can be displayed on the status line.               */
/*									      */
/******************************************************************************/


/******************************************************************************/
/*									      */
/*  Global Constants							      */
/*									      */
/******************************************************************************/

#define STATE_LOGON     0       /*                                          */
#define STATE_NULL      1       /*                                          */
#define STATE_CLIENT    2       /*                                          */
#define STATE_LIBRARY   3       /*                                          */
#define STATE_FILE      4       /*                                          */
#define STATE_ECLIPSE   5       /*                                          */


/******************************************************************************/
/*									      */
/*  Global Variables							      */
/*									      */
/******************************************************************************/

  extern SHORT sAiState;


/******************************************************************************/
/*									      */
/*  External Procedure's Prototypes                                           */
/*									      */
/******************************************************************************/

  extern void   AIscanReset();
  extern void   AIscanInpLine(PBYTE, SHORT);
  extern void   AIscanOutLine(PBYTE, SHORT);
