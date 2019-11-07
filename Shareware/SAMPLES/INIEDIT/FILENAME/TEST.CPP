#include <afx.h>
#include <iostream.h>
#include "filename.h"

void main()
{
    CFilename FileName ;
    char sz[255] ;

    while (FileName != "q")
    {
        cout << "\nEnter filename: " ;
            
        cin >> sz ;
        
        FileName = (CString)sz ;

        cout << "\n" << FileName <<  "\n" ;
    
        cout << "Filename.GetDrive      = '" << FileName.GetDrive() << "'\n" ;    
        cout << "Filename.GetDir        = '" << FileName.GetDir  () << "'\n" ;    
        cout << "Filename.GetPath       = '" << FileName.GetPath () << "'\n" ;    
        cout << "Filename.GetFname      = '" << FileName.GetFname() << "'\n" ;    
        cout << "Filename.GetExt        = '" << FileName.GetExt  () << "'\n" ;    
        cout << "Filename.GetFilename   = '" << FileName.GetFilename() << "'\n" ;    

    }
}