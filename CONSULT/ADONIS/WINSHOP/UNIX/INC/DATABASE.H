/*************************************************************\
 ** FILE:  database.h
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **  Stuff relating to Window Shopper database.
 **  NOTE: some of these file descriptions are out-of-date or 
 **     are missing altogether.
 **
 ** HISTORY:   
 **  Doug Kent  February 26, 1990 (created)
 **  (4.4.90) D. Kent -- added PUB_INFO_FILE,
 **     changed FILETYPE() macro.
 **
\*************************************************************/

/** ********************************************************** 
    FILENAME REFERENCES.
    All files are named with eight hex numerical digits and a hex
    numerical extension (example:  f95d0040.003).

    The eight digits are a number which uniquely refers to the file.  
    Note that those eight digits can be represented by a single DWORD
    value.  The extension is the version number of the file.
    One checks whether a file is up-to-date by whether it exists
    with the extension given in an up-to-date reference.

    Some files exist in this naming convention only in their
    condensed form (for example, shopper.exe and the individual 
    clipart files.  The header of each condensed file specifies the
    name the file should be given in its uncondensed form.
    ********************************************************** 
**/

#ifndef MESSAGEH
#define MESSAGEH

#include "ctype.h"

/** There are several categories of files.  These categories are 
    described further below.  If you wanna add categories, then you gotta
    update the macros below to include references to the new category
    and you need to update filename.txt which is used by the database
    editor (edit.exe) to generate unique filenames. **/
#define RESERVED_FILE       0x1L
#define LIST_FILE           0x2L
#define SET_FILE            0x4L
#define LIBINFO_FILE        0x8L
#define THUMBNAIL_FILE      0x10L
#define THUMBNAIL_INFO_FILE 0x20L
#define CLIPART_FILE        0x40L
#define PUB_INFO_FILE       0x80L
#define WHATS_NEW_FILE      0x100L
#define SEARCH_LIST_FILE    0x200L
#define SEARCH_FILE         0x400L
#define FILELIST_FILE       0x800L

/** filename is the number that represents the file **/
#define BASE(filename) \
    ((DWORD)((filename == WHATS_NEW_FILE) ? WHATS_NEW_BASE :\
            ((filename == SEARCH_LIST_FILE) ? SEARCH_LIST_BASE : \
            ((filename == SEARCH_FILE) ? SEARCH_FILE_BASE : \
            ((filename == PUB_INFO_FILE) ? PUB_INFO_BASE : \
            ((filename == CLIPART_FILE) ? CLIPART_BASE : \
            ((filename == THUMBNAIL_INFO_FILE) ? THUMBNAIL_INFO_BASE : \
            ((filename == THUMBNAIL_FILE) ? THUMBNAIL_BASE : \
            ((filename == LIBINFO_FILE) ? LIBINFO_BASE : \
            ((filename == SET_FILE) ? SET_BASE : \
            ((filename == LIST_FILE) ? LIST_BASE : \
            ((filename == RESERVED_FILE) ? RESERVED_BASE : NADA ))))))))))))

/** filename is the number that represents the file **/
#define FILETYPE(filename) \
    ((DWORD)(bIS_FILELIST_FILE(filename) ? FILELIST_FILE : \
            (bIS_WHATS_NEW_FILE(filename) ? WHATS_NEW_FILE : \
            (bIS_SEARCH_LIST_FILE(filename) ? SEARCH_LIST_FILE : \
            (bIS_SEARCH_FILE (filename) ? SEARCH_FILE : \
            (bIS_PUB_INFO_FILE(filename) ? PUB_INFO_FILE : \
            (bIS_CLIPART_FILE(filename) ? CLIPART_FILE : \
            (bIS_THUMBNAIL_INFO_FILE(filename) ? THUMBNAIL_INFO_FILE : \
            (bIS_THUMBNAIL_FILE(filename) ? THUMBNAIL_FILE : \
            (bIS_LIBINFO_FILE(filename) ? LIBINFO_FILE : \
            (bIS_SET_FILE(filename) ? SET_FILE : \
            (bIS_LIST_FILE(filename) ? LIST_FILE : \
            (bIS_RESERVED_FILE(filename) ? RESERVED_FILE : NADA ))))))))))))

/* convert long value to a string */
#define LTOS(l,s) ultostr(l,s,8)

/* to convert string to long value, use the function:
    GetFileNum(char *szFileName)   (file.c) */

/* NULLFILEs are file references to files that don't yet exist */
#define bNULLFILE(str) (GetFileNum(str) == NADA)
#define SET_NULL(str) strcpy(str,szNADA)
#define SET_NA(str)   strcpy(str,"None Available")

/** *****************************************************************
    RESERVED FILES:
    Let these range from 0x00000000 to 0x0000001F
**/
#define RESERVED_BASE 0x00000000L
#define bIS_RESERVED_FILE(filename) \
    ((BOOL)((filename & 0x0000001FL) AND NOT (filename & 0xFFFFFFE0L)))

#define ERR         0xFFFFFFFFL  // reserved for error conditions

#define NADA        0x00000000L  // reserved for nothing
#define szNADA      "00000000"   // reserved for nothing
#define EXE         0x00000001L  // SHOPPER.EXE
#define MASTERLIST  0x00000002L  // This file gives the names of the top 
                                 // level list files to be opened first.
                                 // It has no extension and is down-loaded
                                 // with each logon.
                                 // It has a one-line header as the first line.
                                 // Each entry may be followed by a comment
                                 // terminated by an EOL.
#define szFILELIST "00000003"    // list of all database files on BBS
#define FILELIST   0x00000003L   // list of all database files on BBS
                                 // be named '00000003' with no extension.
#define LRU         0x00000004L  // reserved for LRU file
#define szLRU       "00000004"   // reserved for LRU file
#define szTMP1      "00000005"   // miscellaneous temporary files that would
#define TMP1        0x00000005L  // miscellaneous temporary files that would
#define szSEARCH    "00000006"   // reserved for Search List file
#define SEARCH      0x00000006L  //  ditto
#define szTMP3      "00000007"   // miscellaneous temporary files that would
#define TMP3        0x00000007L  // miscellaneous temporary files that would
#define szWHATSNEW  "40000000"   // reserved for whats new file
#define WHATSNEW    0x40000000L  //  ditto


/** *****************************************************************
    LIST FILES:  
    Each entry in a 'List' file refers to a 'Set' file. 
    Examples are lists of Publishers and Categories, where each Publisher
    or Category refers to a set file which is a set of libraries.
    Thus a list file is like a set of sets of libraries.  Each item in 
    the list file is a reference to a set of libraries.

    Each list file has a one-line header at the top of the file which should
    begin with the words 'Window Shopper'.
    Each record is terminated by an EOL.  Each field of each record
    is separated by a tab.
    Each record has two fields:
        name_of_item set_file_name

    Let these range from 0x00000020 to 0x000000FF (223 sets of sets of libraries)
**/
#define LIST_BASE 0x00000020L
#define bIS_LIST_FILE(filename) \
    ((BOOL)((filename & 0x000000E0L) AND NOT (filename & 0xFFFFFF00L)))

/** *****************************************************************
    SET FILES:
    Each entry in a 'Set' file refers to a Library Info file. 

    Each set file has a one-line header at the top of the file which should
    begin with the words 'Window Shopper'.
    Each record is terminated by an EOL.  Each field of each record
    is separated by a tab.
    Each record has one fields:
        library_file_name

    Let these range from 0x00000100 to 0x00003FFF (16127 sets of libraries)
**/
#define SET_BASE 0x00000100L
#define bIS_SET_FILE(filename) \
    ((BOOL)((filename & 0x00003F00L) AND NOT (filename & 0xFFFFC000L)))

/** *****************************************************************
    LIBRARY INFO FILES:
    Each file provides information about a single
    library, including sublibraries with their corresponding thumbnail 
    filenames, media available, prices, descriptions, etc.

    Each library file has a one-line header at the top of the file which 
    should begin with the words 'Window Shopper'.

    Let these range from 0x00004000 to 0x001FFFFF (2 million libraries)
**/
#define LIBINFO_BASE 0x00004000L
#define bIS_LIBINFO_FILE(filename) \
    ((BOOL)((filename & 0x001FC000L) AND NOT (filename & 0xFFE00000L)))


/** *****************************************************************
    THUMBNAIL FILES:
    Each file provides bitmaps for a set of cliparts.
    Each thumbnail corresponds to a sublibrary.

    Let these range from 0x00200000 to 0x007fffff (6 million thumbnails)
**/
#define THUMBNAIL_BASE 0x00200000L
#define bIS_THUMBNAIL_FILE(filename) \
    ((BOOL)((filename & 0x00600000L) AND NOT (filename & 0xFF800000L)))

/** *****************************************************************
    THUMBNAIL_INFO FILES:
    Each file provides ascii information relating to each of the cliparts
    contained in a corresponding thumbnail file.  There is a
    thumbnail_info file for every thumbnail.

    Each library file has a one-line header at the top of the file which 
    should begin with the words 'Window Shopper'.

    Information contained includes
        - # of cliparts in thumbnail
        - for each clipart:
            - description of clipart
            - size of clipart (uncompressed)
            - versions clipart is available in
            - price for each version of clipart (or if free)
            - suggested name of uncompressed clipart file

    Let these range from 0x00800000 to 0x00dfffff (6 million thumbnail_infos)
**/
#define THUMBNAIL_INFO_BASE 0x00800000L
#define bIS_THUMBNAIL_INFO_FILE(filename) \
    ((BOOL)((filename & 0x00800000L) AND NOT (filename & 0xFF000000L)))

/** *****************************************************************
    CLIPART FILES:
    Each file comprises a downloadable individual clipart file.

    Let these range from 0x01000000 to 0x0FFFFFFF (251 million cliparts)
    NOTE the gap between 0x01000000 and the previous max of 0x00dfffff.
**/
#define CLIPART_BASE 0x01000000L
#define bIS_CLIPART_FILE(filename) \
    ((BOOL)((filename & 0x0F000000L) AND NOT (filename & 0xF0000000L)))

/** *****************************************************************
    PUBLISHER INFO FILES:
    Each file comprises a publisher info file.

    Let these range from 0x10000000 to 0x10001FFF (8192 publisher info files)
**/
#define PUB_INFO_BASE 0x10000000L
#define bIS_PUB_INFO_FILE(filename) \
    ((BOOL)((filename & 0x10000000L) AND NOT (filename & 0x0FFFE000L)))

/** *****************************************************************
    WHATS NEW FILE:
    The one and only what's new file

    Only one.... that is 0x40000000 
**/
#define WHATS_NEW_BASE 0x40000000L
#define bIS_WHATS_NEW_FILE(filename) \
    ((BOOL) (filename == 0x40000000L) )


/** *****************************************************************
    LRU FILE:
    There is only one LRU file 

    it is 0x00000004
**/
#define LRU_BASE 0x00000004L
#define bIS_LRU_FILE(filename) \
    ((BOOL) (filename == 0x00000004L) )

/** *****************************************************************
    FILELIST FILE:
    There is only one FILELIST file

    it is 0x00000003
**/
#define FILELIST_BASE 0x00000003L
#define bIS_FILELIST_FILE(filename) \
    ((BOOL) (filename == 0x00000003L) )

/** *****************************************************************
    SEARCH LIST FILE:
    There is only one search list file in the following format:

    image_type (tab) pub/search suffix (tab) setfile suffix

    it is 0x00000006 
**/
#define SEARCH_LIST_BASE 0x00000006L
#define bIS_SEARCH_LIST_FILE(filename) \
    ((BOOL) (filename == 0x00000006L) )

/** *****************************************************************
    SEARCH FILES:
    There are three types of line in a 'Search' File

    The first type of line is a library header line
      image_type (tab) libID
    The Second type of line is the sublib header line 
      sublib 008 (tab) sublib 002
    The third type of line is a description line.
      description (tab) base filename (tab) price code

    Let these range from 0x00000600 to 0x000006FF (256 publishers)
**/
#define SEARCH_FILE_BASE 0x00000600L
#define bIS_SEARCH_FILE(filename) \
    ((BOOL)((filename & 0x00000600L) AND NOT (filename & 0xFFFFF900L)))

#endif
