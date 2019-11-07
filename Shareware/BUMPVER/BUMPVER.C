// bumpver.c
//
// Copyright (c) 1990 Charles E. Kindel, Jr.
//
// Creates a version number include file of the form
//
//  #define VER_MAJOR 0
//  #define VER_MINOR 0
//  #define VER_BUILD 2
//
// Usage:
//    BUMPVER <filename> [/V <major> <minor> <build>] [/MA] [/MI]
//
// History
//     1.01  CEK   11/1/90  First version
//     1.02  CEK   11/20/90 Build is now of form XXX
//     1.03 12/12/90 cek    Added /MA and /MI
//
#include <stdio.h>
#include <string.h>
#include "bumpver.ver"

#define FALSE 0
#define TRUE 1

void WriteFile( FILE *pfFile, unsigned short sMajor,
                              unsigned short sMinor,
                              unsigned short sBuild )
{
   fprintf( pfFile, "#define VER_MAJOR %d\n", sMajor ) ;
   fprintf( pfFile, "#define VER_MINOR %d\n", sMinor ) ;
   fprintf( pfFile, "#define VER_BUILD %d\n", sBuild ) ;
}


void ReadFile( FILE *pfFile, unsigned short *psMajor,
                             unsigned short *psMinor,
                             unsigned short *psBuild ) 
{
   fscanf( pfFile, "#define VER_MAJOR %d\n", psMajor ) ;
   fscanf( pfFile, "#define VER_MINOR %d\n", psMinor ) ;
   fscanf( pfFile, "#define VER_BUILD %d\n", psBuild ) ;
}


int main( short argc, char *argv[] )
{
   if (VER_BUILD)
      printf( "BumpVer %d.%.2d.%.3d  ",
            VER_MAJOR,
            VER_MINOR,
            VER_BUILD ) ;
   else
      printf( "BumpVer %d.%.2d  ",
            VER_MAJOR,
            VER_MINOR ) ;

   printf( "Copyright 1990 Charles E. Kindel, Jr.  All Rights Reserved.\n" ) ;
   if ( argc > 1 )
   {
      unsigned short sMajor = 0,
                    sMinor = 0,
                    sBuild = 1;
      FILE *pfFile ;
      char fVerSpecified = FALSE ;
      char fMA = FALSE ;
      char fMI = FALSE ;

      if ( argc > 2 )
      {
         if (!stricmp( argv[2], "/V" ))
         {
            sMajor = atoi( argv[3] ) ;
            sMinor = atoi( argv[4] ) ;
            sBuild = atoi( argv[5] ) ;
            fVerSpecified = TRUE ;
         }
         else
            if (!stricmp( argv[2], "/MA" ))
               fMA = TRUE ;
            else
               if (!stricmp( argv[2], "/MI" ))
                  fMI = TRUE ;
      }

      if (pfFile = fopen( argv[1], "r" ))
      {
         if (!fVerSpecified)
         {
            ReadFile( pfFile, &sMajor, &sMinor, &sBuild ) ;
            printf( "   Updating %s version %d.%.2d.%.3d\n",
                  argv[1],
                  sMajor, sMinor, sBuild ) ;

            if (++sBuild == 1000 || fMI || fMA)
            {
               sBuild = 0 ;
               if (++sMinor == 100 || fMA)
               {
                  sMinor = 0 ;
                  ++sMajor ;
               }
            }
         }
         
         fcloseall() ;

         if (pfFile = fopen( argv[1], "w"))
         {
            WriteFile( pfFile, sMajor, sMinor, sBuild ) ;
            fcloseall() ;

            printf( "   %s version is now %d.%.2d.%.3d\n",
                  argv[1],
                  sMajor, sMinor, sBuild ) ;
         }
         else
         {
            printf( "   Could not re-open file for writing.\n" ) ;
            return -1;
         }
      }
      else
      {
         if (pfFile = fopen( argv[1], "w" ))
         {
            WriteFile( pfFile, sMajor, sMinor, sBuild ) ;
            fcloseall() ;
            printf( "   %s version is now %d.%.2d.%.3d\n",
                 argv[1],
                 0, 0, 1 ) ;
         }
         else
         {
            puts( "   Could not create file" ) ;
            return -1 ;
         }
      }
   }
   else
   {
      puts( "\n   Thanks to Curtis Palmer!\n" ) ;
      puts( "   Usage: BUMPVER <filename> [/V <major> <minor> <build>] [/MA] [/MI]\n" );
      puts( "   BumpVer creates and updates version number files of the form:");
      puts( "      #define VER_MAJOR 1 " ) ;
      puts( "      #define VER_MINOR 30" ) ;
      puts( "      #define VER_BUILD 2\n" ) ;
      puts( "   BumpVer increments the build number by default.\n" ) ;
      puts( "   Options:" ) ;
      puts( "      /V <major> <minor> <buld> - specify a specific version number" ) ;
      puts( "      /MA - Increment the major number by one." ) ;
      puts( "      /MI - Increment the minor number by one.\n" ) ;
      puts( "   Send multi-million dollar donations to:\n" ) ;
      puts( "     Charles E. Kindel, Jr." ) ;
      puts( "     3000 118th Ave. SE #A-203" ) ;
      puts( "     Bellevue, WA 98005" ) ;
      return -1;
   }

   return 0;
}

