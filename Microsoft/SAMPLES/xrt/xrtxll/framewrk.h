/*
**      Microsoft Excel Software Development Kit
**      Version 4.0
**
**      File:           SAMPLE\FRAMEWRK\FRAMEWRK.H
**      Description:    Header file for Framework library
**      Platform:       Microsoft Windows
**
**      Include this file in any source files
**      that use the framework library.
**
*/

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/*
** Total amount of memory to allocate for all temporary XLOPERs
*/

#define MEMORYSIZE 1024


/* 
** Function prototypes
*/

void far cdecl debugPrintf(LPTSTR lpFormat, ...);
LPSTR GetTempMemory(int cBytes);
void FreeAllTempMemory(void);
int cdecl Excel(int xlfn, LPXLOPER pxResult, int count, ...);
LPXLOPER TempNum(double d);
LPXLOPER TempStr(LPSTR lpstr);
LPXLOPER TempBool(int b);
LPXLOPER TempInt(short int i);
LPXLOPER TempActiveRef(WORD rwFirst,WORD rwLast,BYTE colFirst,BYTE colLast);
LPXLOPER TempActiveCell(WORD rw, BYTE col);
LPXLOPER TempActiveRow(WORD rw);
LPXLOPER TempActiveColumn(BYTE col);
LPXLOPER TempErr(WORD i);
LPXLOPER TempMissing(void);
void InitFramework(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
