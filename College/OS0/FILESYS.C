/*            filesys.c

*/

#include <string.h>
#include "filesys.h"

int DiskMB ;   /* mailbox for mutual exclusion within low level disk routines */
superblock SuperBlock ;
unsigned DirINode ;       /* INode number of the dir = 4 */
unsigned FreeDiskSpace ;  /* number of free sectors at any given time */
char FreeDiskSpaceCalc = FALSE ; /* don't want to calc free disk space except */
                                 /* at boot time */
int CurrentTerm ;

/* int Uboot (void) 
 *
 *  This routine is called when the machine is fired up.
 *  It's job is to check to see if the disk is formatted
 *  or not then initialize the interal data structures for
 *  the file system.
 *  If the disk has not been formatted, Uformat is called
 *  to format it.
 *  Once the disk has been formatted.  
*/
void Uboot (void)
{
  int i, j ; 
  char buf [256] ;
  INODE INbuf[4] ;
  DIRENTRY dir ;

  tprint ("OS/0 File System Running...\n") ;

  DiskMB = Createmb (1, 1) ;
  PPOFTmb = Createmb (1,1) ;
  AITmb = Createmb (1,1) ;
  OurRootmb = Createmb (1,1) ;

  initqu (&PPOFT) ;
  initqu (&AIT) ;
     
  DiskGet (&SuperBlock, 0, 0) ;
  if (SuperBlock.DataSpace == 0)
    Uformat (INODESPACE) ;
  else
    InitDir () ;

  tprint ("     %d sectors out of %d free.\n", GetFreeDiskSpace(), 
	  disk_ncylinders() * TRACKSIZE - (INODESPACE + 1)) ;

  CurrentTerm = 1 ;
  for (i = 1 ; i < 4 ; i++)
    Spawn (Ucommand, 8192*2, 3) ;

  CurrentTerm = 0 ;
  Ucommand () ;

  Terminate () ;
}

/* int Uformat (unsigned INodeSpace)
 *
 *   This routine blindly formats the disk.
 *   It initializes the superblock and formats
 *   space for INodeSpace.
 *
 *   The superblock contains the following info:
 *     unsigned INodeSpace - amount of disk space dedicated to inodes
 *                           specified in sectors.
 *     unsigned DataSpace  - amount of disk space dedicated to data.
 *       NOTE: remember that the superblock takes 1 sector.  So the
 *             total disk space = 1 + INodeSpace + DataSpace
 *    This leaves 248 bytes remaining in the superblock.
 *    Divide the 248 bytes as follows:
 *     unsigned FreeData [48] - stack of free data block pointers.
 *                            - FreeData[0] is the stack ptr.
 *       The free data block pointers consist of:
 *          least sig byte = sector 
 *          remaining      = track 
 *     unsigned short FreeINodes [28] - stack of free inode numbers
 *                             - FreeINodes [0] is the stack ptr.
 *
 *   RETURNS
 *       TRUE if format was successful
 *       FALSE if an error occured 
*/
int  Uformat (unsigned INodeSpace)
{
  int i, tmp, b ;
  int track, sector ;
  int t, s ;

  char cBuffer [256] ;
  DIRENTRY dir ;

  static INODE inodes [4] ;  /* simply declare size 4 and get 256 bytes! */
                             /* and make it static and it's full of 0's  */

  FreeDiskSpace = 0 ;
  /* Initialize entire disk to the character 'C' */
  /* First fill cBuffer with 'C's, then loop accross the disk */
  for (i = 0 ; i < 256 ; i++)
    cBuffer[i] = '\x0C' ;
  tprint ("Formatting disk (disk has %d tracks) with\n", disk_ncylinders()) ;
  tprint (" %d sectors reserved for inodes.\n", INodeSpace) ;
  tprint ("   Lowlevel...\n") ;
  tprint ("\ttrack\tsector   \n") ;
  for (track = 0 ; track < disk_ncylinders() ; track++)
    for (sector = 0 ; sector < 16 ; sector++)
      {
	tprint ("            \r") ;
	tprint ("\t %d \t %d \r", track, sector) ;
        if (DiskPut (&cBuffer, track, sector))
	  {
	    tprint ("error\n") ;
    	    return FALSE ;  
	  }
      }
    

  /* fill superblock variable (sb) and write it to track 0 sector 0 */
  SuperBlock.INodeSpace = INodeSpace ;
  SuperBlock.DataSpace  = disk_ncylinders() * TRACKSIZE - (INodeSpace + 1) ;

  /* initialize Free Inode list.  FreeINode[0] is the stack pointer */
  SuperBlock.FreeINodes[0] = 0 ;

  /* Initilize the Free Data List.  FreeData[0] is the stack pointer */
  /* and FreeData[1] contains the the track and sector of a free block */
  /* list.  */
  /* NextAvailTrack = quotient of (INodeSpace + 1) / 16 */
  /* NextAvailSector = remainder of (INodeSpace + 1) / 16 */
  t = (INodeSpace + 1) / 16 ;
  s = (INodeSpace + 1) % 16 ;
  i = 1 ;
  SuperBlock.FreeData [0] = 1 ;
  SuperBlock.FreeData [1] = 0xFFFF ;

  tprint ("\n   Highlevel...\n") ;
  tprint ("\ttrack\tsector\n") ;
  for (; s < 16 ; s++)
    {
      tprint ("\r                   \r ") ;
      tprint ("\t %d\t %d\t", t, s) ;
      FreeDiskBlock ((t << 8) | s) ;
    }

  for (++t; t < disk_ncylinders() ;t++)
    {
      for (s = 0 ; s < 16 ; s++)
	{
	  tprint ("\r                    \r") ;
	  tprint ("\t %d \t %d \t", t, s) ;
	  FreeDiskBlock ((t << 8) | s) ;
	}
    }

  /* OK, now we can find all of our free data on the disk */
  /* Now let's write all of the blank inodes....          */
  tprint ("\n   Writing inodes...\n") ;
  tprint ("\ttrack\tsector\tinode\n") ;
  for (i = 4 ; i < INodeSpace * 4 ; i++)
    {
      tprint ("\t %d \t %d \t %d   \r", ItoTrack(i), ItoSector(i), i) ;
      DiskPut (inodes, ItoTrack (i), ItoSector (i)) ;
    }
  for (i = 1 ; i < 27 ; i++)
    SuperBlock.FreeINodes[i] = i + 4 ;

  DiskPut (&SuperBlock, 0, 0) ; /* get superblock */

  DirINode = 4 ;
  inodes[ItoOffset(DirINode)].RefCount++ ;   /* This should be inode #4 (dir) */
  inodes[ItoOffset(DirINode)].Type = 1 ;
  inodes[ItoOffset(DirINode)].index = 0 ;
  inodes[ItoOffset(DirINode)].offset = 256 ;
  b = GetDiskBlock () ;
  inodes[ItoOffset(DirINode)].Block[0] = b ;

  dir.size = 256 ;
  dir.length = 2 ;
  dir.inode = 4 ;
  strcpy (dir.name, ".") ;

  DiskPut (&dir, UtoTrack(b), UtoSector(b)) ;

  tprint ("\n   The root directory is inode #%d.\n", DirINode) ;

  DiskPut(inodes, ItoTrack(DirINode), ItoSector(DirINode)) ;
  FreeDiskSpaceCalc = TRUE ;

  InitDir () ;
  tprint ("Format complete!\n") ;
  return TRUE ;
}

/* int GetDiskBlock (void)
 *
*/
unsigned GetDiskBlock (void)
{
  unsigned Block ;
  unsigned short FreeINodes [28] ;
  char xyz [1] ;

  Send (DiskMB, xyz) ;

  if (SuperBlock.FreeData[0] > 1)
    {
      Block = SuperBlock.FreeData[SuperBlock.FreeData[0]] ;
      SuperBlock.FreeData[0]-- ;
    }
  else 
    if (SuperBlock.FreeData[1] != 0xFFFF)
      {
	Block = SuperBlock.FreeData[1] ;
	bcopy (FreeINodes, SuperBlock.FreeINodes, 56) ;
	DiskGet (&SuperBlock, SuperBlock.FreeData[1] >> 8, SuperBlock.FreeData[1] & 0x000F) ;
	bcopy (SuperBlock.FreeINodes, FreeINodes, 56) ;
      }
    else
      {
	Receive (DiskMB, xyz) ;
	return FALSE ;
      }

  DiskPut (&SuperBlock, 0, 0) ;
  FreeDiskSpace-- ;
  Receive (DiskMB, xyz) ;
  return Block ;
}


/* void FreeDiskBlock (unsigned block)
 *
*/
void FreeDiskBlock (unsigned block)
{
  char xyz [1] ;

  Send (DiskMB, xyz) ;

  if (block == 0)
    {
      Receive (DiskMB, xyz) ;
      return ;
    }

  if (SuperBlock.FreeData[0] == 46)
    {
      DiskPut (&SuperBlock, block >> 8, block & 0x000F) ;
      SuperBlock.FreeData[0] = 1 ;
    }
  else
    SuperBlock.FreeData[0]++ ;

  SuperBlock.FreeData[SuperBlock.FreeData[0]] = block ;

  DiskPut (&SuperBlock, 0, 0) ;
  FreeDiskSpace++ ;

  Receive (DiskMB, xyz) ;
  return ;
}


/* unsigned GetFreeDiskSpace (void)
 *  
 * returns amount of free sectors on the disk.
*/
unsigned GetFreeDiskSpace (void)
{

   int t = 0, i = 0 ;
   unsigned *b ;

   if (!FreeDiskSpaceCalc)
     {
       b = (unsigned*) malloc (sizeof(unsigned)*disk_ncylinders() * 16) ;
       FreeDiskSpaceCalc++ ;
       tprint ("   Calculating free disk space...\n") ;
       while (b[i] = GetDiskBlock ())
	 i++ ;

       t = i ;

       for (--i; i >= 0 ; i--)
	 FreeDiskBlock (b[i]) ;
       FreeDiskSpace = t ;
       free ((char*)b) ;
     }

   t = FreeDiskSpace ;
   
   return t ;
}
    

/* unsigned GetINode (void)
 *
*/
unsigned GetINode (void)
{
  int i ;
  INODE INbuf [4] ;

  if (SuperBlock.FreeINodes[0] == 0)
    {
      for (i = 4 ; SuperBlock.FreeINodes[0] < 27 && i < SuperBlock.INodeSpace * 4 ; i++)
	{
	  if (!(i % 4))
	      DiskGet (INbuf, ItoTrack(i), ItoSector(i)) ;
	  if (INbuf[i % 4].RefCount < 1)
	    SuperBlock.FreeINodes[++SuperBlock.FreeINodes[0]] = i ;
	}
      
      if (SuperBlock.FreeINodes[0] == 0)
	  return FALSE ;
    }

  i = SuperBlock.FreeINodes[SuperBlock.FreeINodes[0]--] ;
  DiskPut (&SuperBlock, 0, 0) ;

  return i ;
}

/* void FreeINode (unsigned INodeNum)
 *
*/
void FreeINode (unsigned INodeNum)
{
  if (SuperBlock.FreeINodes[0] < 27)
    SuperBlock.FreeINodes[++SuperBlock.FreeINodes[0]] = INodeNum ;
    
  DiskPut (&SuperBlock, 0, 0) ;

  return;
}


/* unsigned FindDirEntry (char * FileName) 
 *
*/
unsigned FindDirEntry (char *FileName)
{
  ROOTENTRYPTR q ;
  char xyz [1] ;

  Send (OurRootmb, xyz) ;

  q = (ROOTENTRYPTR) Our_Root.next ;
  
  while (q != (ROOTENTRYPTR) &Our_Root)
    {
      if (!strcmp (FileName, q->fname))
	{
	  Receive (OurRootmb, xyz) ;
	  return q->inode ;
	}
      else
	q = q->next ;
    }
  Receive (OurRootmb, xyz) ;
  return FALSE ;
}


/* void Udir (int TermNum)
 *
*/
void Udir (int TermNum) 
{
  ROOTENTRYPTR q ;
  char buf [245], xyz[1] ;

  Send (OurRootmb, xyz) ;
  q = (ROOTENTRYPTR) Our_Root.next ;

  sprintf (buf, "   Size \tinode\tname\n") ;
  Writeterm (buf, strlen(buf), TermNum) ;
  while (q != (ROOTENTRYPTR) &Our_Root)
    {
      sprintf (buf, "%8d\t%4d\t%s\n", q->length, q->inode, q->fname) ;
      Writeterm (buf, strlen(buf), TermNum) ;
      q = q->next ;
    }
  sprintf (buf, "  %d bytes free on disk.\n", GetFreeDiskSpace() * SECTORSIZE) ;
  Writeterm (buf, strlen(buf), TermNum) ;

  Receive (OurRootmb, xyz) ;
}


