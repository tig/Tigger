/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Connect to the Lexis/Nexis system (CONNECT.C)                    */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: This module is responsible for connecting to the Lexis/Nexis     */
/*           system.  Due to many options available to the user, this code    */
/*           is alittle on the complex side.  It must try up to three         */
/*           different networks and give the user manual if specified in the  */
/*           setup file.                                                      */
/*									      */
/******************************************************************************/

  extern void ConnectStart();
  extern void ConnectEnd();
  extern void DisconnectStart();
  extern void DisconnectEnd();
