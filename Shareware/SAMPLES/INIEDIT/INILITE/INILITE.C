//************************************************************
//  File name: inilite.c
//
//  Description:
//     Library routines to munge INI files
//
//  History:    Date       Author     Comment
//              1/14/93     JJJ        Created
//              1/14/93     EricFlo    Created
//
// by Microsoft Developer Support, Windows Developer Support
// Copyright (c) 1993 Microsoft Corp.  All rights reserved.
//*************************************************************
#include "precomp.h"
#include "inilite.h"
#include <io.h>
#include <string.h>

//*************************************************************
//
//  WritePrivateProfileStringEx()
//
//  Purpose:    Write a "key=value" string to an INI file
//
//  Parameters: 
//             LPSTR   Name of INI file in ANSI
//             LPSTR   Section Name
//             LPSTR   Key Name
//             LPSTR   Value
//             UINT    Flags ( See comments )
//
//  Return:  (BOOL) TRUE if successful
//                  False if an error occurs
//
//  Comments:
//      Flags allowed
//          WPPSE_ALLOW_DUPES    - Allow adding of duplicates keys
//          WPPSE_CREATE_SECTION - Create the section if it does not
//                                exist
//
//  History:    Date        Author     Comment
//              1/24/93     JJJ        Created
//
//*************************************************************
BOOL CALLBACK     WritePrivateProfileStringEx( LPSTR lpszFileName, LPSTR lpszSection,
                          LPSTR lpszKey, LPSTR lpszValue, UINT uiFlags)
{
    HINIFILE hINIFile;
    LPIFINFO lpIFInfo=NULL;
    UINT  uiFilePtr=0;

    if ( !lpszFileName || !(*lpszFileName) )
        return ( FALSE );

    if ( !lpszSection || !(*lpszSection) )
        return ( FALSE );
    
    if ( !lpszKey || !(*lpszKey) )
        return ( FALSE );

    AnsiToOem( lpszFileName, lpszFileName );
    if ( !(hINIFile = ILOpenINIFile(lpszFileName)) )
        return ( FALSE );

    lpIFInfo = (LPIFINFO) GlobalLock(hINIFile);

    // Check for NULL Pointer
    if (lpIFInfo == NULL)
    {
        ILCleanUp( hINIFile );
        return ( FALSE );
    }

    
    if ( !ILIsExistSection(hINIFile, lpszSection, &uiFilePtr) )
    {
        if ( !(uiFlags & WPPSE_CREATE_SECTION) )
        {
            ILCleanUp( hINIFile );
            return ( FALSE );
        }
        else
        {
            // The 8 is the total of extra chars needed lie \r\n etc.
            if ( (lstrlen(lpszSection) + lstrlen(lpszKey) 
                + lstrlen(lpszKey) + 8 + uiFilePtr) > MAX_INI_FILE_SIZE )
            {
                ILCleanUp( hINIFile );
                return ( FALSE );
            }
        }
    }
    else  // The section exists
    {
        if ( ILIsExistKey(hINIFile, lpszSection, lpszKey, NULL, &uiFilePtr) )
        {
            if ( !(uiFlags & WPPSE_ALLOW_DUPES) )
            {
                ILCleanUp( hINIFile );
                return ( FALSE );
            }
        }
    }
    if ( (lstrlen(lpszSection) + lstrlen(lpszKey) 
        + lstrlen(lpszKey) + 8 + uiFilePtr) > MAX_INI_FILE_SIZE )
    {
        ILCleanUp( hINIFile );
        return ( FALSE );
    }

    if ( !ILAddKeyString(hINIFile, lpszSection, lpszKey, lpszValue) )
    {
        ILCleanUp( hINIFile );
        return ( FALSE );
    }

    ILCloseINIFile( hINIFile );
    return ( TRUE );
}



//*************************************************************
//
//  DeletePrivateProfile()
//
//  Purpose:    Write a "key=value" string to an INI file
//
//  Parameters: 
//             LPSTR   Name of INI file in ANSI
//             LPSTR   Section Name
//             LPSTR   Key Name
//             LPSTR   Value
//             UINT    Flags ( See comments )
//
//  Return:  (BOOL) TRUE if successful
//                  False if an error occurs
//
//  Comments:
//      Deletes a "key=value" combination in a specific section. By 
//      default it will be deleted if there is a perfect match. But
//      you can set flags that make all matching "key" in the section
//      will be deleted.
//
//      Flags allowed
//          DPP_ALL_MATCHING_KEYS - Delete all matching keys
//          DPP_IN_ALL_SECTIONS   - Any section
//          DPP_ANY_VALUE       - Any section
//
//  History:    Date        Author     Comment
//              1/24/93     JJJ        Created
//
//*************************************************************
BOOL CALLBACK     DeletePrivateProfile( LPSTR lpszFileName, LPSTR lpszSection,
                          LPSTR lpszKey, LPSTR lpszValue, UINT uiFlags)
{
    HINIFILE hINIFile;
    LPIFINFO lpIFInfo=NULL;
    UINT     uiFilePtr=0, uiRLRet=0;
    BOOL     fDone=FALSE, fDeleted =FALSE;

    if ( !lpszFileName || !(*lpszFileName) )
        return ( FALSE );

    if ( !lpszKey || !(*lpszKey) )
        return ( FALSE );

    if ( !(hINIFile = ILOpenINIFile(lpszFileName)) )
        return ( FALSE );

    // If we don't care about the section lets set it to NULL
    if ( uiFlags & DPP_IN_ALL_SECTIONS )
    {
        lpszSection = NULL;
    }

    // If DPP_ANY_VALUE is specfied thelpszValue is et to NULL
    if ( uiFlags & DPP_ANY_VALUE )
       lpszValue = NULL;
        
    lpIFInfo = (LPIFINFO) GlobalLock(hINIFile);

    while( !fDone )
    {
        // ILDeleteKey will return FALSE after it has deleted the last match
        if ( ILDeleteKey( hINIFile, lpszSection, lpszKey, lpszValue, 0) )
        {
            fDeleted = TRUE;
            if ( !(uiFlags & DPP_ALL_MATCHING_KEYS) )
                fDone = TRUE;
        }
        else
           fDone = TRUE;
    }

    if ( fDeleted )
    {
        ILCloseINIFile( hINIFile );
        return ( TRUE );
    }
    ILCleanUp( hINIFile );
    return ( FALSE );
}        


//*************************************************************
//
//  ILOpenINIFile()
//
//  Purpose:    Opens / Creates the requested ini file, and
//              initializes the data structure.
//
//
//  Parameters: LPSTR lpFileName  : Filename of ini file
//              UINT  uiFlags     : Access flags
//
//      
//
//  Return:     ( HINIFILE ) Handle if successful
//                           Zero if an error occurs
//
//
//  Comments:   The access flags are:  IM_CREATE, IM_READ, IM_WRITE,
//              and IM_READ_WRITE.
//
//              The alarming number of gotos should be viewed as a
//              way of making the code a little smaller. There are a
//              bunch of gotos. Every single one is a CleanUp. 
//              The way it works is .. lets say we allocate _A_ and if 
//              we fail. We goto CleanUp1. CleanUp1 cleans up _A_. If 
//              we did not fail, we allocate something else.. say _B_.
//              and if we fail there, we go to CleanUp2. CleanUp2 will
//              clean up _B_ and then fall through to CleanUp1. So _A_
//              will also get cleaned up. (OK, I'll get a C in CS101) 
//              This this makes the code shorter and quite readable (!!)
//              
//
//  History:    Date        Author     Comment
//              1/16/93    Eric Flo    Created
//              1/23/92    JJJ         Goto'ed it to death! 
//                                     (see above for comments)                      
//
//*************************************************************
HINIFILE CALLBACK ILOpenINIFile( LPSTR lpFileName )
{
    HFILE           hFile;
    HLOCAL          hMem;
    OFSTRUCT FAR *  npOfStruct;
    LPIFINFO        lpIFInfo;
    HINIFILE        hINIFile;
    HGLOBAL         hMemIni;
    LPSTR           lpMemIni;
    UINT            uiFileSize;
    HGLOBAL         hScratch;


    // Write any cached ini files to disk
    WriteProfileString (NULL, NULL, NULL);

    // Locally allocate memory for the OFSTRUCT
    hMem = LocalAlloc (LHND, sizeof (OFSTRUCT)); 
    if (hMem == NULL)
       return ( 0 );

    npOfStruct = (OFSTRUCT FAR *)LocalLock (hMem);

    if ( !npOfStruct )
    {
       LocalFree (hMem);
       return ( 0 );
    }

    hFile = OpenFile ( lpFileName, npOfStruct, OF_SHARE_DENY_WRITE );

    if (hFile == HFILE_ERROR)
        goto CleanUp10;

    // Get the file length
    uiFileSize = (UINT) _filelength (hFile);
    if ( uiFileSize > MAX_INI_FILE_SIZE )
        goto CleanUp20;

   // Allocate memory for the ini file structure
   hINIFile = GlobalAlloc ( GHND, sizeof (IFINFO) );
   if ( !hINIFile )
       goto CleanUp20;

   lpIFInfo = GlobalLock ( hINIFile );
   if ( !lpIFInfo )
       goto CleanUp30;

   // Allocate memory for the complete ini file
   hMemIni = GlobalAlloc ( GHND, MAX_INI_FILE_SIZE );
   if ( !hMemIni )
       goto CleanUp40;

   // This global lock won't be freed until ILCloseINIFile
   lpMemIni = GlobalLock ( hMemIni );
   if ( !lpMemIni )
       goto CleanUp50;

   // Allocate memory for the complete ini file
   // Buffer for the return from ReadLine()
   hScratch = GlobalAlloc ( GHND, DEFAULT_INI_STRLEN );
   if ( !hScratch )
       goto CleanUp60;

   // This global lock won't be freed until ILCloseINIFile
   lpIFInfo->lpRLBuf = GlobalLock ( hScratch );
   if ( !lpIFInfo->lpRLBuf )
       goto CleanUp70;

   // Buffer for the return from GetSection()
   hScratch = GlobalAlloc ( GHND, DEFAULT_INI_STRLEN );
   if ( !hScratch )
       goto CleanUp80;

   // This global lock won't be freed until ILCloseINIFile
   lpIFInfo->lpSecBuf = GlobalLock ( hScratch );
   if ( !lpIFInfo->lpSecBuf )
       goto CleanUp80;
   
   // Buffer for the return from GetKey()
   hScratch = GlobalAlloc ( GHND, DEFAULT_INI_STRLEN );
   if ( !hScratch )
       goto CleanUp90;

   // This global lock won't be freed until ILCloseINIFile
   lpIFInfo->lpKeyBuf = GlobalLock ( hScratch );
   if ( !lpIFInfo->lpKeyBuf )
       goto CleanUp90;

   // Buffer for the return from GetKey()
   hScratch = GlobalAlloc ( GHND, DEFAULT_INI_STRLEN );
   if ( !hScratch )
       goto CleanUp100;

   // This global lock won't be freed until ILCloseINIFile
   lpIFInfo->lpValBuf = GlobalLock ( hScratch );
   if ( !lpIFInfo->lpValBuf )
       goto CleanUp100;
   
   // Initialize the ini file structure.
   lpIFInfo->uiFileSize    = uiFileSize;
   lpIFInfo->lpMemFile     = lpMemIni;
   lstrcpy ((LPSTR)lpIFInfo->szFileName, lpFileName);

   // Read the ini file into memory
   if ( _lread (hFile, lpIFInfo->lpMemFile, uiFileSize) != uiFileSize)
       goto CleanUp110;

   // Null terminate this buffer
   lpIFInfo->lpMemFile[uiFileSize] = (char) 0;

   // close the ini file
   _lclose (hFile);

   // Unlock memory
   GlobalUnlock ( hINIFile );

   // Free up memory used in OpenFile call
   LocalUnlock (hMem);
   LocalFree (hMem);

   return ( hINIFile );

CleanUp110:
    GlobalUnlock( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpValBuf)) );
    GlobalFree( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpValBuf)) );

CleanUp100:
    GlobalUnlock( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpKeyBuf)) );
    GlobalFree( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpKeyBuf)) );

CleanUp90:
    GlobalUnlock( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpSecBuf)) );
    GlobalFree( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpSecBuf)) );

CleanUp80:
    GlobalUnlock( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpRLBuf)) );
    GlobalFree( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpRLBuf)) );

CleanUp70:
    GlobalFree (hScratch);

CleanUp60:
    GlobalUnlock (hMemIni);

CleanUp50:
    GlobalFree (hMemIni);

CleanUp40:
    GlobalUnlock (hINIFile);

CleanUp30:
    GlobalFree (hINIFile);

CleanUp20:
    _lclose (hFile);

CleanUp10:
    LocalUnlock (hMem);
    LocalFree (hMem);
    return ( 0 );

}



//*************************************************************
//
//  ILCloseINIFile()
//
//  Purpose:    Closes an ini file by free the memory associated
//              the file
//
//
//  Parameters: HINIFILE hINIFile :  Handle to the ini file
//              UINT     uiFlags  :  Overwrite flags
//
//      
//
//  Return:  (BOOL) TRUE if successful
//                  False if an error occurs
//
//
//  Comments:
//
//
//  History:    Date        Author     Comment
//              1/16/93     Eric Flo   Created
//
//*************************************************************

BOOL CALLBACK ILCloseINIFile( HINIFILE hINIFile )
{
   LPIFINFO  lpIFInfo;
   HLOCAL    hMem;
   OFSTRUCT  FAR * npOfStruct;
   HFILE     hFile;


   lpIFInfo = (LPIFINFO) GlobalLock (hINIFile);

   // Locally allocate memory for the OFSTRUCT
   hMem = LocalAlloc (LHND, sizeof (OFSTRUCT));
   if ( hMem == NULL)
      return ( 0 );

   npOfStruct = (OFSTRUCT FAR *)LocalLock (hMem);

   if ( !npOfStruct )
   {
      LocalFree (hMem);
      return ( 0 );
   }

   hFile = OpenFile ( (LPSTR)lpIFInfo->szFileName, npOfStruct, OF_CREATE | OF_SHARE_EXCLUSIVE );

   if (hFile == HFILE_ERROR)
   {
      LocalUnlock (hMem);
      LocalFree (hMem);
      GlobalUnlock (hINIFile);
      return ( 0 );
   }

   _lwrite (hFile, lpIFInfo->lpMemFile, lpIFInfo->uiFileSize );

   _lclose (hFile);
   LocalUnlock ( hMem );
   LocalFree ( hMem );

   ILCleanUp( hINIFile );

   return ( TRUE );
}

//*************************************************************
//
//  ILCleanUp()
//
//  Purpose:    Cleansup All the data structures
//
//  Parameters: HINIFILE hINIFile :  Handle to the ini file
//
//  Return:  (BOOL) TRUE if successful
//                  False if an error occurs
//
//  Comments:
//
//
//  History:    Date        Author     Comment
//              1/24/93     JJJ/EF        Created
//
//*************************************************************
BOOL CALLBACK ILCleanUp( HINIFILE hINIFile )
{
   LPIFINFO  lpIFInfo;

   lpIFInfo = (LPIFINFO) GlobalLock (hINIFile);

   GlobalUnlock( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpRLBuf)) );
   GlobalFree( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpRLBuf)) );

   GlobalUnlock( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpSecBuf)) );
   GlobalFree( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpSecBuf)) );

   GlobalUnlock( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpKeyBuf)) );
   GlobalFree( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpKeyBuf)) );

   GlobalUnlock( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpValBuf)) );
   GlobalFree( (HGLOBAL)GlobalHandle (HIWORD(lpIFInfo->lpValBuf)) );

   GlobalUnlock ( (HGLOBAL) GlobalHandle (HIWORD(lpIFInfo->lpMemFile)) );
   GlobalFree( (HGLOBAL) GlobalHandle (HIWORD(lpIFInfo->lpMemFile)) );

   GlobalUnlock ( hINIFile );
   GlobalFree ( hINIFile );
   return ( TRUE );
}

//*************************************************************
//  ILIsExistSection()
//
//  Purpose:
//	   Check if a section existsi in the INI file
//
//  Parameters:
//      HINIFILE  hIniFile      - HINIFILE of the file to be checked
//      LPSTR     lpszSection   - The section to check.
//
//  Return: ( BOOL )
//
//  Comments:
//      Returns TRUE if the specified section exists in the INI file
//
//  History:    Date       Author     Comment
//              1/14/93   JJJ        Created
//
//*************************************************************
BOOL CALLBACK ILIsExistSection( HINIFILE hINIFile, LPSTR lpSection, LPUINT lpuiIndex )
{
    LPIFINFO  lpIFInfo;
    LPSTR     lpMemFile;
    UINT      uiFilePtr=0;

    lpIFInfo = (LPIFINFO) GlobalLock(hINIFile);

    // Check for NULL Pointer
    if (lpIFInfo == NULL)
        return ( FALSE );

    lpMemFile = lpIFInfo->lpMemFile;

    while ( uiFilePtr < lpIFInfo->uiFileSize )
    {
        if ( ILHReadLine(hINIFile, &lpMemFile[uiFilePtr] ) & RL_SECTION )
        {
            if ( !lstrcmp(lpIFInfo->lpSecBuf,lpSection) )
            {
                GlobalUnlock( hINIFile );
                if ( lpuiIndex )
                    *lpuiIndex = uiFilePtr;
                return ( TRUE );
            }
        }
        uiFilePtr += lstrlen( lpIFInfo->lpRLBuf );
    }

    // No Cigar
    GlobalUnlock( hINIFile );
    if ( lpuiIndex )
        *lpuiIndex = uiFilePtr;
    return ( FALSE );
}
                
//*************************************************************
//  ILIsExistKey()
//
//  Purpose:
//	   Check if a key exists in the INI file
//
//  Parameters:
//      HINIFILE  hIniFile      - HINIFILE of the file to be checked
//      LPSTR     lpszSection   - The section to check.
//      LPSTR     lpszKey       - The key to check for
//
//  Return: ( BOOL )
//
//  Comments:
//      Returns TRUE if the specified key exists in the INI file. If
//      the LPsection is NULL, then we search the entire INI file
//
//  History:    Date       Author     Comment
//              1/14/93   JJJ        Created
//
//*************************************************************
BOOL CALLBACK ILIsExistKey( HINIFILE hINIFile, LPSTR lpSection, LPSTR lpKey, 
                           LPSTR lpValue, LPUINT lpuiIndex  )
{
    LPIFINFO  lpIFInfo=NULL;
    LPSTR     lpMemFile=NULL;
    UINT      uiFilePtr=0, uiRLReturn=0;
    BOOL      fSpecificSection=TRUE, fSectionFound=FALSE, fSpecificValue=TRUE;

    lpIFInfo = (LPIFINFO) GlobalLock(hINIFile);

    // Check for NULL Pointer
    if (lpIFInfo == NULL)
        return ( FALSE );


    lpMemFile = lpIFInfo->lpMemFile;

    if ( !lpSection || !(*lpSection) )
        fSpecificSection = FALSE;

    if ( !lpValue || !(*lpValue) )
        fSpecificValue = FALSE;

    if ( fSpecificSection )
    {
        while ( (uiFilePtr < lpIFInfo->uiFileSize) && (!fSectionFound) )
        {
            if ( ILHReadLine(hINIFile, &lpMemFile[uiFilePtr] ) & RL_SECTION )
            {
                if ( !lstrcmp(lpIFInfo->lpSecBuf, lpSection) )
                    fSectionFound = TRUE;
            }
            uiFilePtr += lstrlen( lpIFInfo->lpRLBuf );
        }
    }
    else
        fSectionFound = TRUE;

    if ( !fSectionFound )
    {
        GlobalUnlock( hINIFile );
        return ( FALSE );
    }

    while ( uiFilePtr < lpIFInfo->uiFileSize )
    {
        uiRLReturn = ILHReadLine(hINIFile, &lpMemFile[uiFilePtr] );
        if (  uiRLReturn & RL_KEY )
        {
            if ( !lstrcmp(lpIFInfo->lpKeyBuf, lpKey) )
            {
                if ( fSpecificValue )
                {
                    if ( !lstrcmp(lpValue, lpIFInfo->lpValBuf) )
                    {
                        GlobalUnlock( hINIFile );
                        *lpuiIndex = uiFilePtr;
                        return ( TRUE );
                    }
                }
                else
                {
                    GlobalUnlock( hINIFile );
                    *lpuiIndex = uiFilePtr;
                    return ( TRUE );
                }
            }
        }
        else if ( (uiRLReturn & RL_SECTION) && (fSpecificSection) )
        {
            GlobalUnlock( hINIFile );
            return ( FALSE );
        }
        uiFilePtr += lstrlen( lpIFInfo->lpRLBuf );
    }

    GlobalUnlock( hINIFile );
    return ( FALSE );
}
    

                
//*************************************************************
//  ILDeleteKey()
//
//  Purpose:
//	   Deletes a specific section
//
//  Parameters:
//      HINIFILE  hIniFile      - HINIFILE of the file to be checked
//      LPSTR     lpszSection   - Section to look in
//      LPSTR     lpszKey       - Key to Delete
//      LPSTR     lpszValue     - Value to look for
//      UINT      uiFlags       - Deletion flags
//
//  Return: ( BOOL )
//
//  Comments:
//      Deletes the specified key. If the lpszValue is NULL, it deletes the 
//      first key that finds. To delete one among duplicate keys, specify 
//      the value
//
// History:    Date       Author     Comment
//              1/14/93   JJJ        Created
//
//*************************************************************
BOOL CALLBACK ILDeleteKey( HINIFILE hINIFile, LPSTR lpszSection, LPSTR lpszKey, LPSTR lpszValue, UINT uiFlags )
{
    LPIFINFO  lpIFInfo;
    LPSTR     lpMemFile=NULL, lpKeyToDelete=NULL;
    UINT      uiFilePtr=0;
    BOOL      fKeyFound = FALSE, fNextSection=FALSE;
    UINT      uiKeyLength =0;

    lpIFInfo = (LPIFINFO) GlobalLock(hINIFile);

    if ( !ILIsExistKey( hINIFile, lpszSection, lpszKey, lpszValue, &uiFilePtr ) )
    {
        GlobalUnlock( hINIFile );
        return ( FALSE );
    }

    lpMemFile = lpIFInfo->lpMemFile;

    lpKeyToDelete = &lpMemFile[uiFilePtr];
    uiKeyLength = lstrlen( lpIFInfo->lpRLBuf );

    if ( (uiFilePtr + uiKeyLength) < lpIFInfo->uiFileSize )
    {
        _fmemmove( &lpMemFile[uiFilePtr], 
                   &lpMemFile[uiFilePtr+uiKeyLength],
                   lpIFInfo->uiFileSize-(uiFilePtr+uiKeyLength) );

        _fmemset( &lpMemFile[lpIFInfo->uiFileSize-uiKeyLength], 
                  0, uiKeyLength );
    }
    else
    {
        _fmemset( &lpMemFile[lpIFInfo->uiFileSize-uiKeyLength], 
                  0, uiKeyLength );
    }
    lpIFInfo->uiFileSize -= uiKeyLength;

    GlobalUnlock( hINIFile );
    return ( TRUE );
}    
                
                
//*************************************************************
//  ILAddSection()
//
//  Purpose:
//	   Adds a specific section
//
//  Parameters:
//      HINIFILE  hIniFile      - HINIFILE of the file to be checked
//      LPSTR     lpszSection   - Section to look in
//      UINT      uiFlags       - Deletion flags
//
//  Return: ( BOOL )
//
//  Comments:
//      Adds  a specified section. 
//
// History:    Date       Author     Comment
//              1/14/93   JJJ        Created
//
//*************************************************************
BOOL CALLBACK ILAddSection( HINIFILE hINIFile, LPSTR lpszSection )
{
    LPIFINFO  lpIFInfo;
    LPSTR     lpTemp=lpszSection;
    UINT      uiIndex1=0;
    BOOL      fFormatted=FALSE;
    char      szTemp[256];

    if ( ILIsExistSection(hINIFile, lpszSection, NULL) )
        return ( FALSE );

    lpIFInfo = (LPIFINFO) GlobalLock(hINIFile);
    // Check for NULL Pointer
    if (lpszSection == NULL)
        return ( FALSE );

    while( (*lpTemp) && (*lpTemp!=OPEN_BRACKET) )
        lpTemp++;

    if ( *lpTemp )
        while( (*lpTemp) && (*lpTemp!=CLOSE_BRACKET) )
            lpTemp++;

    if ( *lpTemp )
        fFormatted = TRUE;

    if ( !fFormatted )
    {
        szTemp[0] = '\r';
        szTemp[1] = '\n';
        szTemp[2] = '\r';
        szTemp[3] = '\n';
        szTemp[4] = OPEN_BRACKET;
        lstrcpy( (LPSTR)&szTemp[5], lpszSection );
        szTemp[lstrlen(lpszSection)+5] = CLOSE_BRACKET;
        szTemp[lstrlen(lpszSection)+6] = '\r';
        szTemp[lstrlen(lpszSection)+7] = '\n';
        szTemp[lstrlen(lpszSection)+8] = '\0';
    }
    else
        lstrcpy( (LPSTR)szTemp, lpszSection );

    lstrcpy( (LPSTR)&lpIFInfo->lpMemFile[lpIFInfo->uiFileSize], (LPSTR)szTemp );
    lpIFInfo->uiFileSize += lstrlen(szTemp);

    GlobalUnlock( hINIFile );
    return ( TRUE );
}

//*************************************************************
//  ILAddKeyString()
//
//  Purpose:
//	   Adds a specific section
//
//  Parameters:
//      HINIFILE  hIniFile      - HINIFILE of the file to be checked
//      LPSTR     lpszSection   - Section to add to
//      LPSTR     lpszKey       - Key to add
//      LPSTR     lpszValue     - Value for the key
//
//  Flags:
//      AK_ALLOWDUPLICATES
//      AK_DONTCREATESECTION
//
//  Return: ( BOOL )
//
//  Comments:
//      Adds  a specified key within a section. 
//
// History:    Date       Author     Comment
//              1/14/93   JJJ        Created
//
//*************************************************************
BOOL CALLBACK ILAddKeyString( HINIFILE hINIFile, LPSTR lpszSection, 
                              LPSTR lpszKey, LPSTR lpszValue )
{
    LPIFINFO  lpIFInfo;
    LPSTR     lpFormattedBuf=NULL;
    HGLOBAL   hFormattedBuf=NULL;
    UINT      uiFilePtr=0;
    int       nStrLen=0;
    LPSTR     lpMemFile=NULL;

    lpIFInfo = (LPIFINFO) GlobalLock(hINIFile);

    // Check for NULL Pointer
    if (lpIFInfo == NULL)
        return ( FALSE );

    // If the section does not exist, craete it
    if ( !ILIsExistSection(hINIFile, lpszSection, &uiFilePtr) )
        ILAddSection( hINIFile, lpszSection );

    uiFilePtr = 0;

    // Get the FilePtr to point at the right spot.
    if ( !ILIsExistSection(hINIFile, lpszSection, &uiFilePtr) )
        return ( FALSE );

    // Create a buffer to store the formatted string
    hFormattedBuf = GlobalAlloc( GHND, DEFAULT_INI_STRLEN );
    if ( !hFormattedBuf )
    {
        GlobalUnlock( hINIFile );
        return ( FALSE );
    }
    lpFormattedBuf = GlobalLock ( hFormattedBuf );
    _fmemset( lpFormattedBuf, 0, DEFAULT_INI_STRLEN );

    if ( *lpszValue )
        wsprintf( lpFormattedBuf, "%s=%s\r\n", (LPSTR)lpszKey, (LPSTR)lpszValue );
    else
        wsprintf( lpFormattedBuf, "%s=\r\n", (LPSTR)lpszKey );

    nStrLen = lstrlen( lpFormattedBuf );

    lpMemFile = lpIFInfo->lpMemFile;

    // If we are at the end of the file we hold need to move anything 
    // down
    if ( (uiFilePtr+lstrlen(lpIFInfo->lpRLBuf)) < (lpIFInfo->uiFileSize) )
        _fmemmove( &lpMemFile[uiFilePtr + nStrLen + lstrlen(lpIFInfo->lpRLBuf)], 
                   &lpMemFile[uiFilePtr + lstrlen(lpIFInfo->lpRLBuf)],
                   lpIFInfo->uiFileSize-(uiFilePtr+lstrlen(lpIFInfo->lpRLBuf)) );

    _fmemcpy( &lpMemFile[uiFilePtr+lstrlen(lpIFInfo->lpRLBuf)], 
              lpFormattedBuf, nStrLen );
    lpIFInfo->uiFileSize +=nStrLen;

    GlobalUnlock( hINIFile );
    return ( TRUE );

}

//*************************************************************
//
//  ILHReadLine()
//
//  Purpose:
//      Reads from the lpText and stuffs the appropriate buffers in
//      the HINFILE structure. It returns a UINT which could be 
//      one of the #defined RL_ values.
//
//      This is the workhorse of INIMunge ad INILite. 
//
//  Parameters:
//      LPSTR lpText      Points to the big block of text we have to
//                        to read from.
//  Return: ( UINT )
//      RL_SECTION  
//      RL_KEY      
//      RL_COMMENT  
//      RL_BLANK    
//      RL_UNKNOWN  
//      RL_ERROR
//      RL_VALUE 
//
//  Comments:
//
//  History:    Date       Author     Comment
//              1/16/93    EricFlo    Created
//              1/16/93    JJJ        Created
//
//*************************************************************
UINT CALLBACK ILHReadLine ( HINIFILE hINIFile, LPSTR lpText )
{
    LPIFINFO   lpIFInfo;
    LPSTR      lpTemp = lpText;
    int        nIndex=0, nIndexOld=0, nTempIndex=0, nStrLen=0;
    UINT       uiRet=RL_UNKNOWN;


    // get a pointer 
    lpIFInfo = (LPIFINFO)GlobalLock( hINIFile );
    if ( !lpIFInfo )
        return ( (UINT)RL_ERROR );

    // Check if we are dealing with a good text buffer. Check for 
    // end of buffer
    if ( !(*lpText) )
    {
        GlobalUnlock( hINIFile );
        return ( (UINT)RL_ERROR );
    }

    // zero out the buffer
    _fmemset( (LPSTR)lpIFInfo->lpRLBuf, 0, DEFAULT_INI_STRLEN );
    _fmemset( (LPSTR)lpIFInfo->lpSecBuf, 0, DEFAULT_INI_STRLEN );
    _fmemset( (LPSTR)lpIFInfo->lpKeyBuf, 0, DEFAULT_INI_STRLEN );
    _fmemset( (LPSTR)lpIFInfo->lpValBuf, 0, DEFAULT_INI_STRLEN );

    // Copy in the line into the return buffer
    while ( (*lpTemp) && (*lpTemp != '\r') )
    {
        lpIFInfo->lpRLBuf[nIndex++] = *lpTemp;
        lpTemp++;
    }

    // Add the carriage return and line feed and terminate the string
    lpIFInfo->lpRLBuf[nIndex++] = '\r';
    lpIFInfo->lpRLBuf[nIndex++] = '\n';
    lpIFInfo->lpRLBuf[nIndex++] = '\0';

    // Now determine what kind of a line it is.
    lpTemp = lpIFInfo->lpRLBuf;
    nIndex = 0;

    // Chomping the white space
    while ( (lpTemp[nIndex] <= SPACE) && (lpTemp[nIndex] != '\r') )
        nIndex ++;

    // Is this a blank line ?
    if ( lpTemp[nIndex] == '\r' )
    {
        GlobalUnlock( hINIFile );
        return ( (UINT)RL_BLANK );
    }

    // Save the index of the first char
    nIndexOld = nIndex;

    // A comment perhaps ?
    if ( (lpTemp[nIndex]) == SEMI_COLON )
    {
        GlobalUnlock( hINIFile );
        return ( (UINT)RL_COMMENT );
    }

    // A Section eh ?
    
    if ( lpTemp[nIndex] != OPEN_BRACKET )
        goto Not_Section;

    nIndex++;

    // Here we are using the lpValBuf as a temporary buffer. This is because
    // we want to save an allocation and we know that a line cannot be a 
    // section and a key/value combination at the same time.
    while ( (lpTemp[nIndex] != CLOSE_BRACKET) && (lpTemp[nIndex] != '\r') )
    {
        lpIFInfo->lpValBuf[nTempIndex++] = lpTemp[nIndex];
        nIndex ++;
    }
    
    if ( lpTemp[nIndex] == CLOSE_BRACKET )
    {

        lpIFInfo->lpValBuf[nTempIndex] = '\0';
        lstrcpy( lpIFInfo->lpSecBuf, lpIFInfo->lpValBuf );
        GlobalUnlock( hINIFile );
        return ( (UINT)RL_SECTION );
    }
    else
        nIndex = nIndexOld;

Not_Section:
    // I know, I know, its a key
    // For a key, you have to have at least 1 character for a token and then
    // there has to be a '='. The token can start with almost any character
    // we know '^', '.' etc are valid chars.. So we will skip the first
    // char and go till we find  a '='. if we find a space then the 
    // '=' should come before any other non white-space chars.

    _fmemset( (LPSTR)lpIFInfo->lpSecBuf, 0, DEFAULT_INI_STRLEN );

    nTempIndex=0;
    while ( (lpTemp[nIndex] != EQUAL_SIGN) && (lpTemp[nIndex] != '\r') )
    {
        lpIFInfo->lpSecBuf[nTempIndex++] = lpTemp[nIndex];
        nIndex++;
    }

    if ( lpTemp[nIndex] == EQUAL_SIGN )
    {
        lpIFInfo->lpSecBuf[nTempIndex] = '\0';
        lstrcpy( lpIFInfo->lpKeyBuf, lpIFInfo->lpSecBuf );
        uiRet |= RL_KEY;

        // Now lets get the rest of the line as the value and then
        // remove the last two characters because they are the CRLF
        lstrcpy( lpIFInfo->lpSecBuf, &lpTemp[nIndex+1]);
        nStrLen = lstrlen( lpIFInfo->lpSecBuf );
        if ( nStrLen > 1 )
            lpIFInfo->lpSecBuf[nStrLen-2] = '\0';
        if ( nStrLen > 0 )
            lpIFInfo->lpSecBuf[nStrLen-1] = '\0';
        lstrcpy( lpIFInfo->lpValBuf, lpIFInfo->lpSecBuf );
        uiRet |= RL_VALUE;

    }

    _fmemset( (LPSTR)lpIFInfo->lpSecBuf, 0, DEFAULT_INI_STRLEN );
    GlobalUnlock( hINIFile );

    return ( uiRet );
}
