/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** testwdb.h                                                           **/

#ifndef _TESTWDB_H_
#define _TESTWDB_H_


/** include WDB stuff--enabled if DEBUG is defined **/
    #define IDM_WDB_BASE    800         /* custom base for debug menu   */
    #include "wdb.h"                    /* include windows.h first      */


    #define IDD_VERSION             100 /* id for dlg version stuff     */

/* version control (?) */
    #define TEST_VERMAJ             1
    #define TEST_VERMIN             0
    #define TEST_VERSION            ((TEST_VERMAJ<<8)|(TEST_VERMIN))

/* misc. defines */
    #define IDM_FILE_EXIT           101

    #define IDM_ASSERT_ASSERT       201
    #define IDM_ASSERT_ASSERTMSG    202

    #define IDM_PUTS_DPS            300
    #define IDM_PUTS_DPS1           301
    #define IDM_PUTS_DPS2           302
    #define IDM_PUTS_DPS3           303
    #define IDM_PUTS_DPS4           304
    #define IDM_PUTS_DPS5           305

    #define IDM_PRINTF_DPF          400
    #define IDM_PRINTF_DPF1         401
    #define IDM_PRINTF_DPF2         402
    #define IDM_PRINTF_DPF3         403
    #define IDM_PRINTF_DPF4         404
    #define IDM_PRINTF_DPF5         405

    #define IDM_VPRINTF_DVPF        500
    #define IDM_VPRINTF_DVPF1       501
    #define IDM_VPRINTF_DVPF2       502
    #define IDM_VPRINTF_DVPF3       503
    #define IDM_VPRINTF_DVPF4       504
    #define IDM_VPRINTF_DVPF5       505

    #define IDM_PRINT_DP            600
    #define IDM_PRINT_DP1           601
    #define IDM_PRINT_DP2           602
    #define IDM_PRINT_DP3           603
    #define IDM_PRINT_DP4           604
    #define IDM_PRINT_DP5           605

    #define IDM_HELP_ABOUT          900


/* more misc. defines */
    #define TEST_WINDOW_WIDTH       CW_USEDEFAULT
    #define TEST_WINDOW_HEIGHT      CW_USEDEFAULT


#endif

/** EOF: testwdb.h **/


