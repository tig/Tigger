/* input.h               (c) KindelCo Software Systems, 1987

                      INPUTCmnd - input command definitions (In)

Author:   Charles E. Kindel, Jr. (tigger)

Started:  July 22, 1987
Version:  1.00


DIRECTORY
int InpCmnd ()      Convert input from keyboard to a command. Returns cmnd.

*/

/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/
#define     ABRT  256   /* Abort the edit session                            */
#define     DBOL  257   /* Delete to the beginning of the line               */
#define     DBOW  258   /* Delete to the beginning fo the previous word      */
#define     DCHR  259   /* Delete char under the cursor                      */
#define     DELP  260   /* Delete previous character                         */
#define     DEOL  261   /* Delete to the end of the line                     */
#define     DEOW  262   /* Delete to the end of the next word                */
#define     MBOB  263   /* Move to the beginning of the text buffer          */
#define     MBOL  264   /* Move to the beginning of the line                 */
#define     MBOW  265   /* Move to the beginning of the previous word        */
#define     MEOB  266   /* Move to the end of the text buffer                */
#define     MEOL  267   /* Move to the end of the line                       */
#define     MEOW  268   /* Move to the end of the next word                  */
#define     MVDN  269   /* Move down one line                                */
#define     MVLF  270   /* Move left one character                           */
#define     MVNP  271   /* Move to the next page (PgDn)                      */
#define     MVPP  272   /* Move to the previous page                         */
#define     MVRT  273   /* Move right one character                          */
#define     MVUP  274   /* Move up one line                                  */
#define     QUIT  275   /* Write, the quit                                   */
#define     VERS  276   /* Print version number, refresh display             */
#define     WRIT  277   /* Write the text buffer to disk                     */
#define     MTLN  278   /* Move to a specified line.                         */
#define     HELP  279   /* display the help screen.                          */
#define     ESC   27    /* Escape                                            */

/* FUNCTION PROTOTYPES ------------------------------------------------------*/
int InpCmnd ();


/*==========================================================================*/
/*  END OF  input.c               (C) KindelCo Software Systems, 1987       */
/*==========================================================================*/
