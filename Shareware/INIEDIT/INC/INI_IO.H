int GetPrivateProfileSections( LPSTR   lpszReturnedString,
                               LPSTR   lpszFileName ) ;
PSTR IsSectionLine( PSTR psLine ) ;
int DelPrivateProfileString( LPSTR lpszApplicationName,
                             LPSTR lpszKeyName,
                             LPSTR lpFilename ) ;
int CleanUpPrivateProfile( LPSTR lpszFileName ) ;



