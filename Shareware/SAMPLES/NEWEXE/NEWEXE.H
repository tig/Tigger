/************************************************************************
 *
 *     Project:  NEWEXE - Windows New Executable Header Routines
 *
 *      Module:  newexe.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *     00.00.000  3/19/91 Charlkin  First Version
 *
 *
 ***********************************************************************/


/************************************************************************
 * Defines
 ***********************************************************************/
#define  INVALIDEXE        0xFEFE

/************************************************************************
 * Old EXE Header typedef
 ***********************************************************************/
#define  NEWEXE_OLDMAGIC   0x5A4D   // Old header magic number 
#define  NEWEXE_RESWDSOLD  0x0010   // No. of reserved words in old header 

typedef struct tagOLDHDR 
{
   WORD  wMagic ;                         // Magic number 
   WORD  cbLastPage ;                     // Bytes on last page of file 
   WORD  wNumPages ;                      // Pages in file 
   WORD  wNumReloc ;                      // Relocations 
   WORD  wHeaderSize ;                    // Size of header in paragraphs 
   WORD  wMinPara ;                       // Minimum extra paragraphs needed 
   WORD  wMaxPara ;                       // Maximum extra paragraphs needed 
   WORD  wSS ;                            // Initial (relative) SS value 
   WORD  wSP ;                            // Initial SP value 
   WORD  wCheckSum ;                      // Checksum 
   WORD  wIP ;                            // Initial IP value 
   WORD  wCS ;                            // Initial (relative) CS value 
   WORD  wRelocOffset ;                   // File address of relocation table 
   WORD  wOverlayNum ;                    // Overlay number 
   WORD  rgwReserved[ NEWEXE_RESWDSOLD ] ;// Reserved words 
   LONG  wNewExeOffset ;         // File address of new exe header 
} OLDEXEHDR, *POLDEXEHDR, FAR *LPOLDEXEHDR  ;

/************************************************************************
 * OLD format macros
 ***********************************************************************/


/************************************************************************
 * New EXE Header typedef
 ***********************************************************************/
#define  NEWEXE_NEWMAGIC    0x454E        // New magic number 
#define  NEWEXE_RESBYTES    8             // Eight bytes reserved (now) 

typedef struct tagNEWEXEHDR            
{
   WORD  wMagic ;                         // Magic number 
   BYTE  bVersion ;                       // Version number 
   BYTE  bRevision ;                      // Revision number 
   WORD  wEntryTableOffSet ;              // Offset of Entry Table 
   WORD  wEntryTableLen ;                 // Number of bytes in Entry Table 
   LONG  lCRC ;                           // Checksum of whole file 
   WORD  wKeyword ;                       // Flag word 
   WORD  wAutoDS ;                        // Automatic data segment number 
   WORD  wHeapSize ;                      // Initial heap allocation 
   WORD  wStackSize ;                     // Initial stack allocation 
   LONG  lCSIP ;                          // Initial CS:IP setting 
   LONG  lSSSP ;                          // Initial SS:SP setting 
   WORD  wNumSegs ;                       // Count of file segments 
   WORD  wNumMRTEntries ;                 // Entries in Module Reference Table 
   WORD  wNonResTableSize ;               // Size of non-resident name table 
   WORD  wSegTableOffset ;                // Offset of Segment Table 
   WORD  wResTableOffset ;                // Offset of Resource Table 
   WORD  wResNameTableOffset ;            // Offset of resident name table 
   WORD  wModRefTableOffset ;             // Offset of Module Reference Table 
   WORD  wImpNamesTableOffset ;           // Offset of Imported Names Table 
   LONG  lNonResNamesTableOffset ;        // Offset of Non-resident Names Table
   WORD  cMoveableEntryPts ;              // Count of movable entries 
   WORD  wSegAlignShift ;                 // Segment alignment shift count 
   WORD  cResEntries ;                    // Count of resource entries 
   BYTE  bTargetOpSys ;                   // Target operating system
   BYTE  bOtherEXEflags ;                 // Other undocumented flags
   BYTE  rgbReserved[NEWEXE_RESBYTES] ;
} NEWEXEHDR, *PNEWEXEHDR, FAR *LPNEWEXEHDR ;

/************************************************************************
 * NEW format macros
 ***********************************************************************/
#define newexeNUMSEGS( NewExe )     (NewExe.wNumSegs)
#define newexeCBNONRES( NewExe )    (NewExe.wNonResTableSize)
#define newexeNONRESOFFSET( NewExe )(NewExe.lNonResNamesTableOffset)
#define newexeOPSYS( NewExe )       (NewExe.bTargetOpSys)
#define newexeOTHERFLAGS( NewExe )  (NewExe.bOtherEXEflags)

/************************************************************************
 * Function prototypes
 ***********************************************************************/
int FAR PASCAL
newexeOpenNewExe( LPSTR       lpszFileName,
                  LPOLDEXEHDR lpOldHdr,
                  LPNEWEXEHDR lpNewHdr ) ;

/***********************************************************************
 * End of File: newexe.h
 ***********************************************************************/

