// devmode.c
//
// DeviceMode routine.
// Simply loads the DeviceMode function from the driver
// and calls it.
//
               GetProfileString ( "devices", szPrinterName, "",
                                  szDevice, 255 ) ;
               psDriver = strtok (szDevice, ",") ;
               psDevice = strtok (NULL, ",") ;
               wsprintf ((LPSTR)szDriverFile, (LPSTR)"%s.DRV", (LPSTR)psDriver) ;
               hLibrary = LoadLibrary (szDriverFile) ;

               if (hLibrary >= 32)
               {
                  lpfnDM = GetProcAddress (hLibrary, "DEVICEMODE") ;
                  iIndex = (*lpfnDM) (hDlg, hLibrary, (LPSTR) szPrinterName, (LPSTR) psDevice) ;
                  FreeLibrary (hLibrary) ;
               }
               else
               {
                  MessageBox (hDlg, "Could not load Device Driver", szAppName, MB_OK | MB_ICONEXCLAMATION) ;
                  break;;
               }

  
