/* registration routines for WinPrint */

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ws_types.h"
#include "ws_defs.h"
#include "ws_reg.h"


PSTR CreateRegistration (PSTR psNum, PSTR psName)
{
PSTR  psCurrent ;
PSTR  psTemp ;
WORD  crc = 486 ;
char  szName [REG_NAME_LEN + 1] ;
char  szText [80] ;

   strcpy (psNum, "W") ;         

   strcpy (szName, psName) ;
   psCurrent = strtok (szName, " ") ;
   do
   {
      for (psTemp = psCurrent  ; *psTemp != '\0' ; psTemp++)
         crc += *psTemp ;
      sprintf (szText, "%2.2X", LOBYTE(crc)) ;
      strcat (psNum, szText) ;
      crc += 486 ; 
   }
   while ((strlen (psNum) < REG_NUM_LEN - 2)  && (psCurrent = strtok (NULL, " "))) ;

   crc = 0 ;
   while (strlen (psNum) < REG_NUM_LEN)
   {
      for (psTemp = psName ; *psTemp != '\0' ; psTemp++)
         crc += *psTemp ;
      sprintf (szText, "%2.2X", LOBYTE (crc)) ;
      strcat (psNum, szText) ;
   }
   return psNum ;
}

BOOL VerifyRegistration (PSTR psName, PSTR psNumber)
{
char szNum [REG_NUM_LEN+1] ;

   return (!stricmp (CreateRegistration (szNum, psName), psNumber)) ;
}

