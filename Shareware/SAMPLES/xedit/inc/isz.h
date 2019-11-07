/************************************************************************
 *
 *     Project:  INIedit 2.0
 *
 *      Module:  isz.h
 *
 *     Remarks:  String resource constants
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#ifndef _ISZ_H_
#define _ISZ_H_

/*
 * IDS values from FIRST_IDS to LAST_IDS are preloaded.
 */
#define FIRST_IDS 0

#define IDS_APPNAME              (FIRST_IDS + 0)
#define IDS_VERSION              (FIRST_IDS + 1)
#define IDS_INI_FILENAME         (FIRST_IDS + 2)
#define IDS_INI_MAIN             (FIRST_IDS + 3)
#define IDS_INI_REG              (FIRST_IDS + 4)
#define IDS_INI_REG_NAME         (FIRST_IDS + 5) 
#define IDS_INI_REG_CODE         (FIRST_IDS + 6)
#define IDS_INI_OPTIONS          (FIRST_IDS + 7)
#define IDS_INI_OPTIONS_SPEC     (FIRST_IDS + 8)
#define IDS_DEFAULTOPT           (FIRST_IDS + 9)
#define IDS_INI_POS              (FIRST_IDS + 10)
#define IDS_INI_POS_SPEC         (FIRST_IDS + 11)
#define IDS_DEFAULTFONT          (FIRST_IDS + 12)
#define IDS_INI_FONT             (FIRST_IDS + 13)
#define IDS_INI_FONT_SPEC        (FIRST_IDS + 14)
#define IDS_INI_WSHOW            (FIRST_IDS + 15)

#define IDS_MNU_FILE                        (FIRST_IDS + 16)
#define IDS_MNU_FILE_OPEN                   (FIRST_IDS + 17) 
#define IDS_MNU_FILE_SAVE                   (FIRST_IDS + 18) 
#define IDS_MNU_FILE_SAVEAS                 (FIRST_IDS + 19)
#define IDS_MNU_FILE_EDIT                   (FIRST_IDS + 20)
#define IDS_MNU_FILE_PRINTSETUP             (FIRST_IDS + 21) 
#define IDS_MNU_FILE_PRINT                  (FIRST_IDS + 22) 
#define IDS_MNU_FILE_EXIT                   (FIRST_IDS + 23) 
#define IDS_MNU_EDIT                        (FIRST_IDS + 24) 
#define IDS_MNU_EDIT_UNDO                   (FIRST_IDS + 25) 
#define IDS_MNU_EDIT_CUT                    (FIRST_IDS + 26)
#define IDS_MNU_EDIT_COPY                   (FIRST_IDS + 27)
#define IDS_MNU_EDIT_PASTE                  (FIRST_IDS + 28) 
#define IDS_MNU_EDIT_DELETE                 (FIRST_IDS + 29)  
#define IDS_MNU_SEARCH                      (FIRST_IDS + 30) 
#define IDS_MNU_SEARCH_FIND                 (FIRST_IDS + 31) 
#define IDS_MNU_SEARCH_FINDNEXT             (FIRST_IDS + 32) 
#define IDS_MNU_SEARCH_REPLACE              (FIRST_IDS + 33) 
#define IDS_MNU_OPT                         (FIRST_IDS + 34) 
#define IDS_MNU_OPT_FONT                    (FIRST_IDS + 35) 
#define IDS_MNU_OPT_EDITOR                  (FIRST_IDS + 36) 
#define IDS_MNU_OPT_TOOLBAR                 (FIRST_IDS + 37) 
#define IDS_MNU_OPT_TOOLBARLABELS           (FIRST_IDS + 38) 
#define IDS_MNU_OPT_STAT                    (FIRST_IDS + 39) 
#define IDS_MNU_HELP                        (FIRST_IDS + 40) 
#define IDS_MNU_HELP_CONTENTS               (FIRST_IDS + 41) 
#define IDS_MNU_HELP_SEARCH                 (FIRST_IDS + 42) 
#define IDS_MNU_HELP_ONHELP                 (FIRST_IDS + 43) 
#define IDS_MNU_HELP_ABOUT                  (FIRST_IDS +  44)
#define IDS_SMNU                             (FIRST_IDS + 45)
#define IDS_SMNU_SIZE                        (FIRST_IDS + 46)
#define IDS_SMNU_MOVE                        (FIRST_IDS + 47)
#define IDS_SMNU_MINIMIZE                    (FIRST_IDS + 48)
#define IDS_SMNU_MAXIMIZE                    (FIRST_IDS + 49)
#define IDS_SMNU_CLOSE                       (FIRST_IDS + 50)
#define IDS_SMNU_RESTORE                     (FIRST_IDS + 51)
#define IDS_SMNU_TASKLIST                    (FIRST_IDS + 52)
#define IDS_DIRTY                            (FIRST_IDS + 53)
#define IDS_CLEAN                            (FIRST_IDS + 54)
#define IDS_HELP                             (FIRST_IDS + 55)
#define IDS_NOHELP                           (FIRST_IDS + 56)
#define IDS_HELPFILE                         (FIRST_IDS + 57)
#define IDS_HEXWORD                          (FIRST_IDS + 58)
#define IDS_HEXDWORD                         (FIRST_IDS + 59)
#define IDS_OPENFILENAMEFILTER               (FIRST_IDS + 60)
#define IDS_DEFEXT                           (FIRST_IDS + 61)
#define IDS_INI_EDITOR                       (FIRST_IDS + 62)
#define IDS_DEFAULTEDITOR                    (FIRST_IDS + 63)
#define IDS_PLEASEPAY                        (FIRST_IDS + 64)
#define IDS_DEFAULTFILE                      (FIRST_IDS + 65)
#define IDS_APPTITLE                         (FIRST_IDS + 66)
#define IDS_INI_OTHERFILES                   (FIRST_IDS + 67)


#define LAST_IDS (FIRST_IDS + 67)

#define IDS_ERR_CONFIRMDELETE       512
#define IDS_MSG_WORKING             515
#define IDS_MSG_PRINTERONPORT       516
#define IDS_ERR_PRINTERR            517
#define IDS_ERR_PRINTERR0           518
#define IDS_ERR_PRINTERR1           519
#define IDS_ERR_PRINTERR2           520
#define IDS_ERR_PRINTERR3           521
#define IDS_ERR_PRINTERR4           522
#define IDS_ERR_BADPRINTERDC        523
#define IDS_ERR_NOMEMORY            524
#define IDS_ERR_COMMANDLINE         525

#define IDS_ERR_SAVECHANGES         526
#define IDS_ERR_BADREGISTRATION     527
#define IDS_ERR_OPENFILE            528
#define IDS_ERR_BADEDITOR           529
#define IDS_ERR_BADDLLVER           530
#define IDS_ERR_SAVEOTHERFILE       531

#define IDS_MSG_PRINTING            540

#define IDS_PDERR_PRINTERCODES      550
#define IDS_PDERR_SETUPFAILURE      551
#define IDS_PDERR_PARSEFAILURE      552
#define IDS_PDERR_RETDEFFAILURE     553
#define IDS_PDERR_LOADDRVFAILURE    554
#define IDS_PDERR_GETDEVMODEFAIL    555
#define IDS_PDERR_INITFAILURE       556
#define IDS_PDERR_NODEVICES         557
#define IDS_PDERR_NODEFAULTPRN      558
#define IDS_PDERR_DNDMMISMATCH      559
#define IDS_PDERR_CREATEICFAILURE   560
#define IDS_PDERR_PRINTERNOTFOUND   561
#define IDS_CDERR_GENERALCODES      562
                                 

/* menu strings (help)     */
/* menu IDs begin at 4096  */


#endif

/************************************************************************
 * End of File: isz.h
 ***********************************************************************/

