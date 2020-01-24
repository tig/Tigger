/************************************************************************
 *
 * This file defines all version information for the Font Connection
 * project.  To change the file versions, or product version for
 * The Font Connection simply modify this file and run the
 * MAKEALL.BAT file with no parameters.
 *
 * NOTE:  All executables in the FontCon project use this file as
 *        a build dependency.  If you modify this file, all projects
 *        will be rebuilt.
 *
 * This file is included by each executable in the project.  Typically
 * it is included in two places, once in the "global.h" file, and again
 * in the .rc file (for version stamp info).
 *
 ***********************************************************************/

/* The following three lines define the product version.  This is
 * the version number that will appear in the about box of FONTCON.
 *
 * If VER_BUILD is defined as 0, the version will be displayed as
 * X.YY where X is VER_MAJOR and YY is VER_MINOR.  Otherwise, the
 * version number will be displayed as X.YY.ZZZ where ZZZ is
 * VER_BUILD.
 */
#define VER_MAJOR 1
#define VER_MINOR 1
#define VER_BUILD 0

#define VER_BETA_STRING    "Release 1.0"


/* For each executable in the FontCon project there is a define
 * that defines that executables file version number.  FontCon
 * is architected so that individual files can be updated without
 * having to update the entire system.
 */
/* CONXLIB.DLL */
#define VER_CONXLIB           10

/* CONXMEM.DLL */
#define VER_CONXMEM           10

/* CONXPRO.DLL (ZMODEM) */
#define VER_CONXPRO           10

/* CONXSRV.MOD */
#define VER_CONXSRV           11

/* CONXUP.MOD */              
#define VER_CONXUP            10

/* FONTCON.EXE */
#define VER_FONTCON           12

/* FCDB.DLL */
#define VER_FCDB              10

/* MODEM.INI */               
#define VER_MODEMINI          10      // NOTE: Must be edited manually!

/* FCHELP.HLP - FCHELP.BIN is used to track version!!! */
#define VER_HELPFILE          10

/************************************************************************
 * End of File: version.h
 ***********************************************************************/

