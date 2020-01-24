/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Robot Automatic Data Retrieval Interface (ROBOT.H)               */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/


/******************************************************************************/
/*									      */
/*  Global Constants							      */
/*									      */
/******************************************************************************/

#define ROBOT_SIGNON    0
#define ROBOT_CLIENT    1
#define ROBOT_AUTOCITE  2
#define ROBOT_LEXSEE    3
#define ROBOT_LEXSTAT   4
#define ROBOT_SHEPARD   5


/******************************************************************************/
/*									      */
/*  Global Variables							      */
/*									      */
/******************************************************************************/

  extern BOOL  fRobotWorking;
  extern LONG  lRobotTimeout;
  extern char  szRobotStatus[8];


/******************************************************************************/
/*									      */
/*  External Procedure's Prototypes                                           */
/*									      */
/******************************************************************************/

  extern void  RobotReset(void);
  extern void  RobotTerm(void);
  extern BOOL  RobotDdeCommand(PSZ);
  extern BOOL  RobotCommand(USHORT, PSZ, HFILE, USHORT);
  extern BOOL  RobotKeyboardUnlock(void);
