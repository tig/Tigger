//*************************************************************
//  File name: inilite.h
//
//  Description: 
//      Global include file for #defines and prototypes
//
//  History:    Date       Author     Comment
//              1/13/93    EricFlo    Created
//              1/13/93    JJJ        Created
//*************************************************************
#if !defined(__INILITE_H__)
#define __INILITE_H__
typedef struct 
{
    UINT    uiFileSize;
    LPSTR   lpMemFile;
    char    szFileName[128];
    LPSTR  lpRLBuf, lpSecBuf, lpKeyBuf, lpValBuf;
} IFINFO, FAR * LPIFINFO;

typedef HANDLE HINIFILE;
typedef UINT FAR* LPUINT;
void    FAR _cdecl DebugOutput(UINT flags, LPCSTR lpsz, ...);
/* DebugOutput flags values */
#define DBF_TRACE           0x0000
#define DBF_WARNING         0x4000
#define DBF_ERROR           0x8000
#define DBF_FATAL           0xc000
//----------------------------------------
//  Global Defines
#define DEFAULT_INI_STRLEN            256
#define SPACE                          ' '
#define SEMI_COLON                     ';'
#define OPEN_BRACKET                   '['
#define CLOSE_BRACKET                  ']'
#define EQUAL_SIGN                     '='
#define MAX_INI_FILE_SIZE            65534

#define WPPSE_ALLOW_DUPES            0x0001
#define WPPSE_CREATE_SECTION         0x0002
#define DPP_ALL_MATCHING_KEYS        0x0004
#define DPP_IN_ALL_SECTIONS          0x0008
#define DPP_ANY_VALUE                0x0010

#define IL_DEFAULTFLAG               0x0001                                     
//----------------------------------------
//  ReadLine return values                           
#define RL_UNKNOWN                    0x0000  
#define RL_SECTION                    0x0001  
#define RL_KEY                        0x0002  
#define RL_VALUE                      0x0004  
#define RL_COMMENT                    0x0008  
#define RL_BLANK                      0x0010  
#define RL_ERROR                      0x0020  

#define AK_ALLOWDUPLICATES            0x0001
#define AK_DONTCREATESECTION          0x0002


BOOL CALLBACK     WritePrivateProfileStringEx
                        ( LPSTR lpszFileName, LPSTR lpszSection,
                          LPSTR lpszKey, LPSTR lpszValue, UINT uiFlags);
                  // With this you can add a "key=value" combination to a 
                  // given section. By default, you can have duplicates. 
                  // But a flag can be specified so that it does not write 
                  // it if there is a dupliate  available.


BOOL CALLBACK     DeletePrivateProfile
                        ( LPSTR lpszFileName, LPSTR lpszSection,
                          LPSTR lpszKey, LPSTR lpszValue, UINT uiFlags);
                  // Deletes a "key=value" combination in a specific section. 
                  // By default it will be deleted if there is a perfect match.
                  // But you can set flags that make all matching "key" in the 
                  // section will be deleted.


/////////////////////////////////////////////////////////////////////
//               H E L P E R      F U N C T I O N S
/////////////////////////////////////////////////////////////////////            
HINIFILE CALLBACK   ILOpenINIFile( LPSTR lpFileName );
                    // Opens a INI file. If the open was unsuccessful, we will
                    // return a NULL otherwise  return the HINIFILE associated
                    // with the INI file
    
BOOL    CALLBACK    ILCloseINIFile( HINIFILE hINIFile );
                    // Closes the INI file associated with the HINIFILE and 
                    // returns TRUE. 

BOOL    CALLBACK    ILCleanUp( HINIFILE hINIFile );
                    // Cleans up all the global data of the hINIFile
                
                
// --------------------------------------
// Query functions
// --------------------------------------    
BOOL    CALLBACK    ILIsExistSection( HINIFILE hINIFile, LPSTR lpSection, LPUINT lpuiIndex );
                    // Returns TRUE if the specified section exists in the INI file
                
BOOL    CALLBACK    ILIsExistKey( HINIFILE hINIFile, LPSTR lpSection, LPSTR lpKey, 
                                  LPSTR lpValue, LPUINT lpuiIndex );
                    // Returns TRUE if the specified key exists in the INI file. If
                    // the LPsection is NULL, then we search the entire INI file
                
// -----------------------------------
// Data Deletion functions
// -----------------------------------
BOOL    CALLBACK     ILDeleteKey( HINIFILE hINIFile, LPSTR lpszSection, LPSTR lpszKey,
                                  LPSTR lpszValue, UINT uiFlags );
                     // Deletes the specified key. If the lpszValue is NULL, it deletes 
                     // the first key that finds. To delete one among duplicate keys, 
                     // specify the value
                
// -----------------------------------
// Data addition functions
// -----------------------------------                
BOOL    CALLBACK     ILAddSection( HINIFILE hINIFile, LPSTR lpszSection );
                     // Adds  a specified section.
                
BOOL    CALLBACK     ILAddKeyString( HINIFILE hINIFile, LPSTR lpszSection, LPSTR lpszKey,
                                     LPSTR lpszValue );
                     // Adds  a specified key within a section.
                
UINT    CALLBACK     ILHReadLine ( HINIFILE hINIFile, LPSTR lpText );
                     // Reads one line of the ini file starting at lpText into the
                     // appropriate LPIFINO buffer


#endif
