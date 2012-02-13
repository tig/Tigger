/*******************************************************************/
/*    
   sandy3.c

   The following are the routines used to:
             open files,
             close files,
             read to files,
             write to files and
             remove files.
*/
/*******************************************************************/
#include "filesys.h"

#define UREADOP 1
#define UWRITEOP 2
#define UAPPENDOP 3
#define UERROR -1

/*******************************************************************/
/*  void InitDir (void)
 
    This subroutine is called from Uboot to initialize a list of
    files currently on the disk.  The information about each file is
    stored in a list which is accessed when Uopen, Uclose or Uremove
    is called.  Each node in the list contains the inode number,
    the file's name and the length of the file.  The list is ordered
    by inode number.  This structure is set up to avoid frequent
    Disk reads that are involved when file info is needed.

    Note :  The list of files is intialized each time this procedure 
            is called whether the list is empty or not.
*/
void InitDir ( void )
{
   int track,
       sector,
       i ;
   unsigned char size,
                 length ;
   unsigned location,
            root,
            fsize ;
   char out_str [80] ;
   INODE dir[4], Itmp[4] ;
   DIRENTRY info, Buf, *tbuf ;
   ROOTENTRYPTR Rtmp ;

   /* the list of files is initialized each time this procedure is called */
   initqu( &Our_Root ) ;

   /* get the root node inode entry */
   root = ROOT_LOCATION ;
   track = ItoTrack (root) ;
   sector = ItoSector (root) ;
   DiskGet (dir, track, sector) ;
 
   /* go through block associated with the inode to retrieve all the
      filenames */
   for ( i = 0 ; i < INODE_BOUNDS && dir[0].Block[i] != 0 ; i++ )
   {
      /* retrieve first block of data in directory */
      track = UtoTrack (dir[0].Block[i]) ;
      sector = UtoSector (dir[0].Block[i]) ;
      DiskGet (&Buf, track, sector) ;
      tbuf = &Buf ;

      while ( (char *)tbuf < (char *) &Buf + SECTORSIZE ) 
      {
         /* extract the information on each file entry */    
	 info.inode = tbuf->inode ;
	 info.size = tbuf->size ;
         info.length = tbuf->length ;
	 strcpy (info.name, tbuf->name) ;

  	 (char *) tbuf += info.size ;
         /* retreive the inode for the given file */
         track = ItoTrack (info.inode) ;
         sector = ItoSector (info.inode) ;
         DiskGet (Itmp, track, sector) ;

         /* calculate the size of the file based on the inode info */
         fsize = Itmp[ItoOffset(info.inode)].index * SECTORSIZE  +
                        Itmp[ItoOffset(info.inode)].offset ;
      
         /* set up the Root Directory list (Our_Root) which is used
            to reference files */      
         Rtmp = (ROOTENTRYPTR) malloc (sizeof(ROOTENTRY)) ;
         strcpy (Rtmp->fname, info.name) ;
         Rtmp->inode = info.inode ;
         Rtmp->length = fsize ;
         ordinsqu (&Our_Root, (QNODEPTR) Rtmp) ;
      }
   }
   if ( i == 0 )
      tprint ("Directory is empty\n") ;
   return ;
}

/*******************************************************************/
/*  int Uopen (char *name, int mode)
 
    This function is responsible for opening a file for reading,
    writing or appending.  If the file does not currently exist
    in the system a call to Uopen with mode set to write will
    create the file.  All open file information is stored in two
    tables:
        PPOFT - Per-Process Open File Table 
                this is a list of files a process is currently
                accessing.  Each node in this list contains a 
                list of Open Files - the maximum number of
                files a process may have open at one time is 16.
        AIT   - Active Inode Table
                this is a list of all files currently open in the
                system.  Each entry contains an inode number, a
                reference count, a remove flag and a copy of the
                inode.
    Uopen returns 0 if it was successful in opening the named file,
    -1 otherwise.
*/
int Uopen (char *name, int mode)
{
   unsigned inodenum ;
   
   int track, sector, offset ;
   int i ;
   
   INTINODEPTR tmp ;
   OFTENTRYPTR tmp2 ;
   PPOFTENTRYPTR tmp3 ;

   char Buf[SECTORSIZE],
        xyz [1] ;
   char *tbuf ;

   /* find the appropriate inodenum for the filename given */
   inodenum = FindDirEntry (name) ;
   if (( inodenum == 0 ) && ( mode == UWRITEOP ))
   {
      /* this file needs to be created as it does not exist
         in the directory
      */
      inodenum = create_file (name) ;
   }

   /* if inodenum is still 0 then there was a problem opening
      or creating the named file
   */
   if ( inodenum == 0 ) 
      return  UERROR ;

   /* first lookup the current process in the Per-Process Open File 
      Table (PPOFT) and see if it exists - if it doesn't then create
      an entry.
    */
   Send (PPOFTmb, xyz) ;
   if (NULL == (tmp3 = (PPOFTENTRYPTR) ordremqu( &PPOFT, get_pid()) ))
   {
      tmp3 = (PPOFTENTRYPTR) malloc (sizeof(PPOFTENTRY)) ;
      tmp3->pid = get_pid() ;
      tmp3->OpenFiles = 0 ;
      /* initialize the array of open files pointers to NULL */
      for ( i = 0 ; i < PPOFTSIZE ; i ++ )
         tmp3->pOFTentry[i] = NULL ;
   }
   ordinsqu (&PPOFT, (QNODEPTR) tmp3) ;
   /* check to see that the file does not exceed its open file limit */
   if ( tmp3->OpenFiles + 1 == PPOFTSIZE ) 
   {
      Receive (PPOFTmb, xyz) ;
      return UERROR ;
   }
   else
      ++tmp3->OpenFiles ; 
   Receive (PPOFTmb, xyz) ;

   /* proceed at this point to initialize all the table entries */
   /* the first entry we need to set up is the Active Inode Table 
      (AIT) entry
   */
   Send (AITmb, xyz) ;
   if ( NULL == ( tmp = (INTINODEPTR) ordremqu ( &AIT, inodenum) ))
   {  
      /* there is no current entry in the table for the given inode
         so one is created and inserted into the table
      */
      tmp = (INTINODEPTR) malloc (sizeof(INTINODE)) ;
      tmp->INodeNum = inodenum ;
      tmp->pINode = (INODEPTR) malloc (sizeof(INODE)) ;
      tmp->erase_file = 0 ;
      track = ItoTrack (inodenum) ;
      sector = ItoSector (inodenum) ;
      offset = ItoOffset (inodenum) * INODESIZE;
      DiskGet (Buf, track, sector) ;
      tbuf = (char *) Buf + offset ;
      bcopy (tbuf, tmp->pINode, INODESIZE) ; 
      tmp->RefCount = 0 ;
   }

   tmp->RefCount++ ;
   ordinsqu ( &AIT, (QNODEPTR) tmp ) ;
   Receive (AITmb, xyz) ;

   /* now that the AIT entry has been established, one must be
      created for the Open File Table (OFT) which is maintained
      for each process in the system which has files open.
      An entry is created for each file a process has open .
   */
   tmp2 = (OFTENTRYPTR) malloc (sizeof(OFTENTRY)) ;
   tmp2->mode = mode ;
   tmp2->pAITentry = tmp ;
   tmp2->RefCount = 1 ;

   /* the seek pointer (SeekPtr) into the file needs to be initialized
      based on the requested mode
   */
   if ( mode != UAPPENDOP )
   {
      tmp2->SeekPtr.index = 0 ;
      tmp2->SeekPtr.offset = 0 ;
      if ( mode == UWRITEOP )
      {
         /* the file will be written so the length of the file as
            indicated by the AIT entry needs to be reset */
         tmp->pINode->index = 0 ;
         tmp->pINode->offset = 0 ;
      }
   }
   else
   {
      tmp2->SeekPtr.index = tmp->pINode->index ;
      tmp2->SeekPtr.offset = tmp->pINode->offset ;
   }

   /* now that all entries have been initialized insert the new file
      info into the array of open files that is found in the PPOFT
      of the current process */
   for (i = 0; i < PPOFTSIZE && tmp3->pOFTentry[i] != NULL ; i++ ) ;
   tmp3->pOFTentry[i] = tmp2 ;

   return i ;

}
 
/*******************************************************************/
/*  int Uclose (int fd)

    This function performs a close on the named file; if successful
    it returns 0 and -1 otherwise.
    If the close operation results in the file's AIT reference
    count to equal 0 then all table entries are removed where
    appropriate.  
    If a process calls Uclose on a file and that is it's only file
    currently open, then it's entry in the PPOFT is removed.
    The inode information is not updated onto the disk until the
    last process calls Uclose.
*/
int Uclose ( int fd )
{
   INTINODEPTR tmp ;
   OFTENTRYPTR tmp2 ;
   PPOFTENTRYPTR tmp3 ;
   ROOTENTRYPTR Rtmp ;

   INODE in ;
   int i ;
   int track, sector, offset ;
   char Buf[SECTORSIZE],
        xyz [1],
        *tbuf ;

   /* check for a valid fd */
   if ( ( fd < 0 ) || ( fd >= PPOFTSIZE ) )
      return UERROR ;

   /* check that the process actually has open files - if it doesn't
      it won't exist in the PPOFT */
   Send (PPOFTmb, xyz) ;
   tmp3 = (PPOFTENTRYPTR) ordremqu ( &PPOFT, get_pid() ) ; 
   ordinsqu (&PPOFT, (QNODEPTR) tmp3 ) ;
   Receive (PPOFTmb, xyz) ;
   if ( tmp3 == NULL )
      return UERROR ;
  
   /* check that the fd given, is a valid one for the process */
   if ( NULL == ( tmp2 = tmp3->pOFTentry[fd] ) )
      return UERROR ;

   /* free up the entry indicated by fd in the PPOFT */
   if ( --tmp2->RefCount == 0 )
      tmp3->pOFTentry[fd] = NULL ;

   /* if the file is to be deleted then the SeekPtr must be 
      adjusted to reflect that */
   if ( tmp2->pAITentry->erase_file ) 
   {
      for ( i = 0 ; i < INODE_BOUNDS ; i++ )
      {
         FreeDiskBlock ( tmp2->pAITentry->pINode->Block[i] );
         tmp2->pAITentry->pINode->Block[i] = 0 ;
      }
   }

   /* need to update the length of the file if the access mode 
      was an append or write */
   if ( tmp2->mode != UREADOP && !tmp2->pAITentry->erase_file )
   {
      /* update the value of the index and offset in the file node */
      if ( tmp2->SeekPtr.offset == SECTORSIZE ) 
      {
         tmp2->pAITentry->pINode->offset = 0 ;
         tmp2->pAITentry->pINode->index = tmp2->SeekPtr.index + 1 ;
      }
      else
      {
         tmp2->pAITentry->pINode->offset = tmp2->SeekPtr.offset ;
         tmp2->pAITentry->pINode->index = tmp2->SeekPtr.index ;
      }
     
      /* update the length of the file in the directory list */ 
      Send (OurRootmb, xyz) ;
      if ( NULL != 
          (Rtmp = (ROOTENTRYPTR)
                    ordremqu (&Our_Root, tmp2->pAITentry->INodeNum)) ) 
      {
         Rtmp->length = tmp2->SeekPtr.index * SECTORSIZE +
                               tmp2->SeekPtr.offset  ;
         ordinsqu (&Our_Root, (QNODEPTR) Rtmp) ;
      }
      Receive (OurRootmb, xyz) ;

      /* free up all data blocks not required after the operation */
      for ( i = tmp2->SeekPtr.index + 1 ; i < INODE_BOUNDS  &&
                tmp2->pAITentry->pINode->Block[i] != 0 ; i++ )
      {
         FreeDiskBlock ( tmp2->pAITentry->pINode->Block[i] );
         tmp2->pAITentry->pINode->Block[i] = 0 ;
      }
   }

   /* need to decrement the reference count in the AIT and if necessary
      copy the INode back to the disk. */
   if ( --tmp2->pAITentry->RefCount == 0 )
   {
      /* remove INode from AIT table */
      Send (AITmb, xyz) ;
      tmp = (INTINODEPTR) ordremqu ( &AIT, tmp2->pAITentry->INodeNum ) ;  
      Receive (AITmb, xyz) ;

      /* check to see if the INode should be copied to the disk */
      if ( !tmp->erase_file ) 
      { 
         /* write all the info in the INode back to the disk */
         track = ItoTrack (tmp->INodeNum) ;
         sector = ItoSector (tmp->INodeNum) ;
         DiskGet ( Buf, track, sector ) ;
         tbuf = Buf + (ItoOffset (tmp->INodeNum) * INODESIZE ) ;
         bcopy ( tmp->pINode, tbuf, INODESIZE ) ;
         DiskPut ( Buf, track, sector ) ;
      }
      /* free up the memory occupied by the AIT entry */ 
      free ( (char *) tmp->pINode ) ;
      free ( (char *) tmp ) ;
   }
   /* free up the memory occupied by the OFT entry */
   free ( (char *) tmp2 ) ;

   /* check to see if the process still has files open - if not then 
      remove its entry from the PPOFT */
   Send (PPOFTmb, xyz) ; 
   if ( --tmp3->OpenFiles == 0 )
   {
      tmp3 = (PPOFTENTRYPTR) ordremqu (&PPOFT, get_pid()) ; 
      free ( (char *) tmp3 ) ;
   }
   Receive (PPOFTmb, xyz) ;
   return 0 ;

}   
/*******************************************************************/
/*  int Uread (int fd, char *buffer, int nbytes)

    This function reads a given number of bytes from the file to
    a buffer; if successful it returns the number of bytes read,
    -1 otherwise.
    This routine checks that the requested number of bytes does not
    exceed the actual number of bytes available for reading in the file. 
*/
int Uread ( int fd, char *buffer, int nbytes )
{
   OFTENTRYPTR tmp2 ;
   PPOFTENTRYPTR tmp3 ;
   ROOTENTRYPTR Rtmp ;
 
   char Buf [SECTORSIZE], 
        *tbuf, 
        *usersbuf,
        xyz [1] ;

   unsigned int bytes_left, size = 0, bytes_read = 0, 
                track, sector, value ;       

   /* check for a valid fd */
   if ( ( fd < 0 ) || ( fd > PPOFTSIZE ) )
      return UERROR ;

   /* check that the current process actually has files open */
   Send (PPOFTmb, xyz) ;
   if ( NULL == (tmp3 = (PPOFTENTRYPTR) ordremqu ( &PPOFT, get_pid() ) ) ) 
   {
      Receive (PPOFTmb, xyz) ;
      return UERROR ;
   }
   ordinsqu ( &PPOFT, (QNODEPTR) tmp3 ) ;
   Receive (PPOFTmb, xyz) ;

   /* check that the file requested is currently open within the 
      current process */
   if ( NULL == (tmp2 = tmp3->pOFTentry[fd] ) || tmp2->mode != UREADOP ) 
      return UERROR ;

   /* check for the end of the file */
   if ( UEOF (tmp2) )
      return 0 ;
  
   Send (OurRootmb, xyz) ;
   Rtmp = (ROOTENTRYPTR) ordremqu ( &Our_Root, tmp2->pAITentry->INodeNum ) ;
   ordinsqu ( &Our_Root, (QNODEPTR) Rtmp ) ;
   Receive (OurRootmb, xyz) ;

   /* calculate the number of bytes in the file available for reading
      and compare it to the requested number - if it exceeds what is
      actually available then nbytes is assigned that value */
    
   bytes_left =  Rtmp->length - ((tmp2->SeekPtr.index * SECTORSIZE)
                  + tmp2->SeekPtr.offset )  ;
   if ( nbytes > bytes_left )
      nbytes = bytes_left ;
   if ( SECTORSIZE - tmp2->SeekPtr.offset >= nbytes )
      size = nbytes ;
   else
      size = SECTORSIZE - tmp2->SeekPtr.offset ;
   usersbuf = buffer ;

   while (bytes_read < nbytes)  
   {
      tbuf = Buf + tmp2->SeekPtr.offset ;
      value = tmp2->pAITentry->pINode->Block[ tmp2->SeekPtr.index ] ;
      track = UtoTrack ( value ) ;
      sector = UtoSector ( value ) ;  
      DiskGet ( Buf, track, sector ) ;
      bcopy ( tbuf, usersbuf, size ) ;
      if ( (size + tmp2->SeekPtr.offset) == SECTORSIZE )
         Get_Next_Block ( tmp2 ) ;
      else
         tmp2->SeekPtr.offset += size ;
      bytes_read += size ;
      usersbuf += size ;
      if ( ( size = nbytes - bytes_read ) > SECTORSIZE )
         size = SECTORSIZE ;
   }
   return bytes_read ;

}

/*******************************************************************/
/*  int Uwrite (int fd, char *buffer, int nbytes)
    This procedure writes the given number of bytes to the file;
    it returns 0 if successful and -1 if an error occurs.
    Uwrite allocates new data blocks by calling Get_New_Block
    which returns a 1 if there is room on the disk and 0 otherwise.     
*/
int Uwrite ( int fd, char *buffer, int nbytes )
{
   OFTENTRYPTR tmp2 ;
   PPOFTENTRYPTR tmp3 ;

   char *usersbuf, 
        *tbuf, 
        Buf [SECTORSIZE],
        xyz [1] ;
   int bytes_written = 0,
       room_on_disk = 1,
       value,
       size, track, sector ;

   /* check for a valid fd */
   if ( ( fd < 0 ) || ( fd > PPOFTSIZE ) )
      return UERROR ;

   /* check that the current process actually has files open */
   Send (PPOFTmb, xyz) ;
   if ( NULL == (tmp3 = (PPOFTENTRYPTR) ordremqu ( &PPOFT, get_pid() ) ) ) 
   {
      Receive (PPOFTmb, xyz) ; 
      return UERROR ;
   }
   ordinsqu ( &PPOFT, (QNODEPTR) tmp3 ) ;
   Receive (PPOFTmb, xyz) ; 

   /* check that the file requested is currently open within the 
      current process */
   if ( NULL == (tmp2 = tmp3->pOFTentry[fd] ) || tmp2->mode == UREADOP ) 
      return UERROR ;

   /* if at end of file - get a new Disk Block */ 
   if ( UEOF( tmp2 ) )
      room_on_disk = Get_New_Block (tmp2) ; 
   
   /* if nbytes exceeds the number of bytes remaining in the current
      block then set size to equal that value */ 
   size = SECTORSIZE - tmp2->SeekPtr.offset ;
   if ( size > nbytes )
      size = nbytes ;
   usersbuf = buffer ;
   while ( room_on_disk && bytes_written < nbytes )
   {
      tbuf = Buf + tmp2->SeekPtr.offset ;
      value = tmp2->pAITentry->pINode->Block[ tmp2->SeekPtr.index ] ;
      track = UtoTrack (value) ;
      sector = UtoSector (value) ;
      DiskGet (Buf, track, sector) ; 
      bcopy ( usersbuf, tbuf, size ) ;
      bytes_written += size ;
      if ( (tmp2->SeekPtr.offset += size ) == SECTORSIZE && 
                     bytes_written < nbytes )
         room_on_disk = Get_New_Block (tmp2) ;
      DiskPut ( Buf, track, sector ) ;
      if ( bytes_written < nbytes && room_on_disk != 0 ) 
         usersbuf += size ;
      if ( ( size = nbytes - bytes_written ) > SECTORSIZE )
         size = SECTORSIZE ;
   }

   if ( !room_on_disk )
      return UERROR ;
   else
      return 0 ;
} 
         
/*******************************************************************/
/*  int Uremove (char *name)
   
    This routine is used to remove a named file from the system; it
    returns 0 if successful and -1 otherwise.
    Uremove makes use of a separate procedure to deal with deleting
    the entry in the directory list.
    If the current process is accessing this file Uremove will
    call Uclose on it.
*/
int Uremove ( char *name )
{
   INTINODEPTR tmp ;
   OFTENTRYPTR tmp2 ;
   PPOFTENTRYPTR tmp3 ;
 
   char Buf [SECTORSIZE],
        *tbuf,
        xyz [1] ;
   int track, sector, offset ;
   int inodenum , i, found = 0 ;

   /* find out if it is a valid filename */
   if ( 0 == ( inodenum = FindDirEntry ( name ) )  )
      return UERROR ;

   /* the directory entry is deleted */ 
   remove_directory_entry ( name ) ;

   /* find out if this file is currently open in AIT */ 
   Send (AITmb, xyz) ;
   if ( NULL != ( tmp = (INTINODEPTR) ordremqu ( &AIT, inodenum ) ) )
   {
      /* if file is currently in the AIT flag it for deletion */
      tmp->erase_file = 1 ;
      ordinsqu ( &AIT, (QNODEPTR) tmp ) ;
      Receive (AITmb, xyz) ;

      /* check if this process is currently accessing this file and
         if it is then call Uclose on the file */
      Send (PPOFTmb, xyz) ;
      if ( NULL != ( tmp3 = (PPOFTENTRYPTR) ordremqu ( &PPOFT, get_pid() ) ))
      {
         ordinsqu ( &PPOFT, (QNODEPTR) tmp3 ) ;
         Receive (PPOFTmb, xyz) ; 
         /* go through the process' file list and see if the file
            is within it */ 
         i = 0 ;
         while ( !found && i < PPOFTSIZE )
         {
            if ( tmp3->pOFTentry != NULL )
            {
               if (tmp3->pOFTentry[i]->pAITentry->INodeNum == inodenum ) 
                  found = 1 ;
            }
            i ++ ;
         } 
         if ( --i < PPOFTSIZE )
            /* file is currently open in this process so Uclose is called
               to remove it */
            if ( Uclose (i) != 0 )
               return UERROR ;
      }
      else
         Receive (PPOFTmb, xyz) ;
   } 
   else 
      Receive (AITmb, xyz) ;
   return 0 ;
}
      
/*******************************************************************/
/*  int UEOF (OFTENTRYPTR tmp)

    This routine simply determines if the file being accessed
    is at the end of the data it contains; it returns 1 if this
    is the case, 0 otherwise.
*/
int UEOF (OFTENTRYPTR tmp)
{
    if  ((tmp->SeekPtr.index == tmp->pAITentry->pINode->index) &&
         (tmp->SeekPtr.offset >= tmp->pAITentry->pINode->offset))
       return 1 ;
    else
       return 0 ;
}

/*******************************************************************/
/*  void Get_Next_Block ( OFTENTRYPTR tmp )

    This routine is called from Uread and it is responsible for
    setting the seek pointer in the OFT node to point to the
    next available block of data.  If there is none (at eof) then
    it sets the seek pointer to the end of file index.
*/ 
void Get_Next_Block ( OFTENTRYPTR tmp )
{
   int i = tmp->SeekPtr.index ;
 
   /* check that the pointer does exist */
   if ( tmp == NULL )
      return ;

   /* check to make sure that we are not at end of file */
   if ( i >= INODE_BOUNDS - 1 )
      return ;
    
   /* make sure the next block actually exists */
   if ( tmp->pAITentry->pINode->Block[i+1] == 0 )
   {
      /* make sure the file pointer indicates the end of the file */
      tmp->SeekPtr.index = tmp->pAITentry->pINode->index ;
      tmp->SeekPtr.offset = tmp->pAITentry->pINode->offset ; 
      return ;
   }
   tmp->SeekPtr.index = i + 1 ;
   tmp->SeekPtr.offset = 0 ;
   return ;
}

/*******************************************************************/
/*  int Get_New_Block ( OFTENTRYPTR tmp )

    This routine is called from Uwrite and is used to determine if
    a routine requires a new block of data to be allocated from
    the disk.  It sets all the fields in the node to indicate
    the new block.  It returns 1 if successful and 0 otherwise.
*/
int Get_New_Block ( OFTENTRYPTR tmp )
{
    int i = tmp->SeekPtr.index ;
   
    /* check that the pointer does exist */ 
    if ( tmp == NULL )
       return 0 ;

    /* check to insure the file has not exceeded its limit on size */
    if ( i >= INODE_BOUNDS - 1 )
       return 0 ;

    /* look at the current data block to determine the action required */
    if ( tmp->pAITentry->pINode->Block[i] != 0 )
       /* need to determine if the there is any space available in the 
          current block */
       if ( tmp->SeekPtr.offset < SECTORSIZE ) 
          /* free space still available in the current block */
          return 1 ;
       else
          /* move to next block indicator in the inode */
          i ++ ;

    /* need to get a new block of data from the disk */
    if ( ( tmp->pAITentry->pINode->Block[i] = GetDiskBlock() ) == 0 )
    { 
       /* if there are no free blocks available return 0 */
          tprint("No BLOCKS available\n");
          return 0 ;
    }
    /* otherwise everything went all right */
    tmp->SeekPtr.index = i ; 
    tmp->SeekPtr.offset = 0 ;
    return 1 ;
}
        
/*******************************************************************/
/*  unsigned create_file ( char *name )
    
    This routine creates a new file by finding an inode and 
    initializing all its fields.  The inode is then inserted 
    into the root directory as well as the root directory
    list.  The value of the new inode is returned, if any
    errors are encountered a value of 0 is returned.
*/
unsigned create_file (char *name)
{
   char Buf [SECTORSIZE], 
        *tbuf,
        xyz [1] ;
   unsigned char size, 
                 length,
                 offset ;
   unsigned int track, 
                sector,
                i,
                inserted = 0,
                root ;
   INODE dir, new_node ;
   DIRENTRY newfile, curr_entry ;
   ROOTENTRYPTR Rtmp ;
 
   /* check that it is a valid filename */
   if ( *name == NULL )
      return 0 ;

   /* go and retrieve the inode information */
   root = ROOT_LOCATION ;
   track = ItoTrack (root) ;
   sector = ItoSector (root) ;
   DiskGet (Buf, track, sector ) ;
   bcopy (Buf, &dir, INODESIZE) ;

   /* create the new file descriptor */
   if ( ( newfile.inode = GetINode() ) == 0 )
      return 0 ;

   /* once the new inode is created all its fields need to be initialized */
   new_node.index = 0 ;
   new_node.offset = 0 ;
   new_node.RefCount = 1 ;
   new_node.Type = 1 ;
   for ( i = 0 ; i < INODE_BOUNDS ; i++ ) 
      new_node.Block[i] = 0 ;
 
   /* initialize the new entry into the Root directory list */
   Rtmp = (ROOTENTRYPTR) malloc (sizeof(ROOTENTRY)) ;
   Rtmp->inode = newfile.inode ;
   Rtmp->length = 0 ;
   strcpy (Rtmp->fname, name) ;
   Send (OurRootmb, xyz) ;
   ordinsqu (&Our_Root, (QNODEPTR) Rtmp ) ;
   Receive (OurRootmb, xyz) ; 
  
   /* write the inode back to the disk */
   track = ItoTrack (newfile.inode) ;
   sector = ItoSector (newfile.inode) ;
   DiskGet (Buf, track, sector) ;
   offset = ItoOffset (newfile.inode) * INODESIZE ;
   bcopy ( &new_node, Buf + offset, INODESIZE ) ;
   DiskPut ( Buf, track, sector ) ;
   /* have a valid inode value for the file */
   newfile.length = strlen(name) + 1 ;
   newfile.size = newfile.length + 6 ;

   /* proceed to find a spot to insert this file into */
   for ( i = 0; i < INODE_BOUNDS && dir.Block[i] != 0 && !inserted ; i++ )
   {
      /* get a block of directory entries */
      track = UtoTrack (dir.Block[i]) ;
      sector = UtoSector (dir.Block[i]) ;
      DiskGet (Buf, track, sector) ;
      tbuf = Buf ;

      /* go through the buffer and try to find a spot to insert */
      while ( ( tbuf < Buf + SECTORSIZE - 1 ) && !inserted )
      {
         bcopy (tbuf, &curr_entry, 6) ; 
         /* check to see if there is room following this entry to 
            insert the new one */
         if ( curr_entry.size - ( curr_entry.length + 6 ) >= newfile.size )
         {
            /* we can insert the new entry immediately following this
               entry */
            newfile.size = curr_entry.size - ( curr_entry.length + 6 ) ;
            curr_entry.size = curr_entry.length + 6 ;
         /* copy the new size into the entry we are inserting after */
            bcopy (&curr_entry, tbuf, 6) ;
            tbuf += curr_entry.size ;
            inserted = 1 ;
         }
         else
            tbuf += curr_entry.size ;
      } 
   }
   i-- ;
   if ( !inserted && i >= INODE_BOUNDS )
      /* directory is full so we return error */
      return 0 ;

   if ( !inserted && dir.Block[i] == 0 )
      if ( (dir.Block[i] = GetDiskBlock()) != 0 ) 
      {
         /* we needed to get a new block for directory entries so the
            new file is inserted at the start of this block */
         track = UtoTrack (dir.Block[i]) ;
         sector = UtoSector (dir.Block[i]) ;
         DiskGet (Buf, track, sector) ;
         newfile.size = SECTORSIZE ;
      }
      else
         /* the disk is full */
         return 0 ;

   bcopy (&newfile, tbuf, 6) ;
   bcopy (name, tbuf + 6, newfile.length) ;
   /* now that the new entry has been inserted copy the buffer back
      to the disk and then return the new inode number */
   DiskPut (Buf, track, sector) ;
   return newfile.inode ;
     
}

/*******************************************************************/
/*  void remove_directory_entry ( char *name )
    
    This routine removes the named file from the root directory
    and if not currently active - deletes all its data blocks.
*/
void remove_directory_entry (char *name)
{
   char Buf [SECTORSIZE], 
        *tbuf, 
        *tmp,
        xyz [1] ;
   unsigned char size,
                 length ;
   int inode,
       track, 
       sector,
       i, j,
       removed = 0 ;
   INODE dir[4] ;
   DIRENTRY info, prev_entry ;
   ROOTENTRYPTR Rtmp ;
   int root ;

   /* check for a valid filename */
   if ( *name == NULL )
      return ;
 
   inode = FindDirEntry (name) ; 
   /* removing a file from the root directory list */
   Send (OurRootmb, xyz) ;
   Rtmp = (ROOTENTRYPTR) ordremqu (&Our_Root, inode ) ;
   free ((char *) Rtmp) ;
   Receive (OurRootmb, xyz) ;

   /* go and retrieve the directory */
   root = ROOT_LOCATION ;
   track = ItoTrack (root) ; 
   sector = ItoSector (root) ;
   DiskGet (Buf, track, sector) ;
   bcopy (Buf, dir, INODESIZE) ;
   
   for ( i = 0 ; i < INODE_BOUNDS && dir[ItoOffset(root)].Block[i] != 0 && !removed ; i++ )
   {
      /* get a block of directory entries */
      track = UtoTrack (dir[ItoOffset(root)].Block[i]) ;
      sector = UtoSector (dir[ItoOffset(root)].Block[i]) ;
      DiskGet (Buf, track, sector) ;
      tbuf = Buf ;
      tmp = Buf ;
 
      /* search the block for the desired entry */ 
      while ( tbuf < Buf + SECTORSIZE - 1 )
      {
         bcopy (tbuf, &info, 6) ;
         if ( strcmp( tbuf + 6, name ) == 0 )
         {
            /* directory entry has been found */
            if ( tbuf == Buf )
            {
               /* we are at the start of a block and so need to free it */
               FreeDiskBlock (dir[ItoOffset(root)].Block[i]) ;
               dir[0].Block[i] = 0 ;
               dir[0].index = i - 1 ;
               dir[0].offset = SECTORSIZE - 1;
            }
            else
            {
               /* we need to increase the size of the entry that preceeds
                  the one which we are deleting */
               bcopy (tmp, &prev_entry, 6) ;
               prev_entry.size += info.size ;
               bcopy (&prev_entry, tmp, 6) ;
               DiskPut (Buf, track, sector) ;
            }
            removed = 1 ;
         }
         tmp = tbuf ;
         tbuf += info.size ;
      }
   }
   track = ItoTrack (inode) ;
   sector = ItoSector (inode) ;
   DiskGet (dir, track, sector) ;
       
   for ( i = 0 ; i < INODE_BOUNDS ; i++ )
   {
      FreeDiskBlock (dir[ItoOffset(inode)].Block[i]) ;
      dir[ItoOffset(inode)].Block[i] = 0 ;
   }
   DiskPut (dir, track, sector) ;
   FreeINode (inode) ; 
   return ;     
 
}
/*******************************************************************/

