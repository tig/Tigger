/*                 filesys.h

*/
#include "typedefs2.h"

#define FORMAT TRUE

#define INODESPACE 32  /* number of sectors devoted to inodes */
#define INODESIZE 64
#define TRACKSIZE 16
#define SECTORSIZE 256
#define INODESPERSECTOR 4
#define PPOFTSIZE 16

#define SIZE_OF_DESCRIPTOR 6 /* in bytes */
#define ROOT_LOCATION 4
#define INODE_BOUNDS  ((INODESIZE -8) /2)

/* macros to convert inode number into track, sector and offest */
#define ItoTrack(X) (X / 64)
#define ItoSector(X) ((X % 64) / 4)
#define ItoOffset(X) (X % 4)

/* macros to convert index to a file into a track and a sector */
#define UtoTrack(X) (X >> 8)
#define UtoSector(X) (X & 0x000F)

/* this structure is used to indicate the offset into a file */
typedef struct findex
{
  unsigned short index ;
  unsigned short offset ;
} findex ;

/* inode typedef.  Total size is 64, thus */
/* 4 will fit in a block.                 */
typedef struct inode
{
  short Block [28] ;
  unsigned short Type ;
  unsigned short RefCount ;  
  unsigned short index ;
  unsigned short offset ;
}  inode, INODE, *INODEPTR ;
  
/* internal inode typdef - Active Inode Table entry */
typedef struct intinode
{
  struct intinode *next ;
  struct intinode *prev ;
  unsigned INodeNum ;
  unsigned RefCount ;
  int erase_file ;
 
  INODEPTR pINode ;
} intinode, INTINODE, *INTINODEPTR ;

/* Open file table entry */
typedef struct oftentry 
{
  unsigned mode ;
  findex SeekPtr ;
  unsigned RefCount ;

  INTINODEPTR pAITentry ;
} oftentry, OFTENTRY, *OFTENTRYPTR ;


/* PerProcess open file table entry */
typedef struct ppoftentry
{
  struct ppoftentry *next ;
  struct ppoftentry *prev ;
  unsigned pid ;
  int OpenFiles ;

  OFTENTRYPTR pOFTentry [PPOFTSIZE] ;  /* an array of open file table entries */
} ppoftentry, PPOFTENTRY, *PPOFTENTRYPTR ;

/* typedef. Total size is 256 bytes */
/* This is the size of a sector as specified in the
   USLOSS User's Manual */
typedef struct superblock
{
  unsigned INodeSpace ;
  unsigned DataSpace ;
  unsigned FreeData [48] ;
  unsigned short FreeINodes [28] ;
} superblock, SUPERBLOCK, *SUPERBLOCKPTR ;

/* typedef for a directory entry */
typedef struct direntry 
{
  unsigned short inode ;
  unsigned short size ;
  unsigned short length ;
  char name [250] ;
} direntry, DIRENTRY, *DIRENTRYPTR ;

/* typedef for the list of directory entries */   
typedef struct rootentry
{
  struct rootentry *next ;
  struct rootentry *prev ;
  unsigned inode ;
  int length ;
  char fname [256] ;
} rootentry, ROOTENTRY, *ROOTENTRYPTR ;

/* File Tables - used to maintain information about the files
   processes are currently accessing.
*/
QNODE PPOFT ;  /* this is the Per-Process Open File Table */
QNODE AIT ;    /* this is the Active Inode Table */ 
QNODE Our_Root ; /* this contains info on all files currently in
                    the system */	
int PPOFTmb ;   /* mailboxes used for synchronization */
int AITmb ;
int OurRootmb ;


/* Procedures used */
unsigned GetDiskBlock (void) ;
void FreeDiskBlock (unsigned block) ;
unsigned GetFreeDiskSpace (void) ;
unsigned GetINode (void) ;
void FreeINode (unsigned INodeNum) ;
void Uboot (void) ;
int Uformat (unsigned INodeSpace) ;
unsigned FindDirEntry (char *FileName) ;
void InitDir ( void ) ;
void Ucommand (void) ;

unsigned create_file  (char *name) ;
void remove_directory_entry (char *name) ;

int Uopen ( char *name, int mode ) ;
int Uclose ( int fd ) ;
int Uread ( int fd, char *buffer, int nbytes ) ;
int Uwrite ( int fd, char *buffer, int nbytes ) ;
int Uremove ( char * name ) ;
int UEOF ( OFTENTRYPTR file ) ;
void Get_Next_Block ( OFTENTRYPTR file ) ;
int Get_New_Block ( OFTENTRYPTR file ) ;
void Udir ( int term ) ;

