
#include  <termio.h>
#include  <stdio.h>
#include  <fcntl.h>


static char   workBuf [ 4 ];

static  char*
s_AnyInterpret ( int  charValue )
{
   workBuf[0] = 0;
   workBuf[1] = 0;
   workBuf[2] = 0;
   workBuf[3] = 0;
   if  ( charValue < (int) '@' )  {
      workBuf[0] = '^';
      workBuf[1] = '@' + charValue;
   }
   else if ( charValue == 255 )  {
      workBuf[0] = 'D';
      workBuf[1] = 'E';
      workBuf[2] = 'L';
   }
   else if ( charValue > 127 )  { 
     workBuf[0] = '*';
      workBuf[1] = '*';
      workBuf[2] = '*';
   }
   else  {
     workBuf[0] = charValue;
   }
   return ( workBuf );
}



static  char*
s_VInterpret ( int  vIndex )
{
   static char str[5];

   if  ( vIndex == VINTR )   return "VINTR";
   if  ( vIndex == VQUIT )   return "VQUIT";
   if  ( vIndex == VERASE )  return "ERASE";
   if  ( vIndex == VKILL )   return "VKILL";
   if  ( vIndex == VEOF )    return "VEOF";
   if  ( vIndex == VEOL )    return "VEOL";
   if  ( vIndex == VEOL2 )   return "VEOL2";
   if  ( vIndex == VMIN )    return "VMIN";
   if  ( vIndex == VTIME )   return "VTIME";
   if  ( vIndex == VSWTCH )  return "SWTCH";
	sprintf(str,"%d",vIndex);
	return str;
}



static  char*
s_CInterpret ( int  ctrlChar )
{
   static char str[5];
   if  ( ctrlChar == CNUL )     return "CNUL";
   if  ( ctrlChar == CDEL )     return "CDEL";
   if  ( ctrlChar == CESC )     return "CESC";
   if  ( ctrlChar == CINTR )    return "CINTR";
   if  ( ctrlChar == CQUIT )    return "CQUIT";
   if  ( ctrlChar == CERASE )   return "CERASE";
   if  ( ctrlChar == CKILL )    return "CKILL";
   if  ( ctrlChar == CEOF )     return "CEOF";
   if  ( ctrlChar == CSTART )   return "CSTART";
   if  ( ctrlChar == CSTOP )    return "CSTOP";
   if  ( ctrlChar == CSWTCH )   return "CSWTCH";
   if  ( ctrlChar == CNSWTCH )  return "CNSWTCH";
   if  ( ctrlChar == CSUSP )    return "CSUSP";
   if  ( ctrlChar == VCEOF )    return "VCEOF";
   if  ( ctrlChar == VCEOL )    return "VCEOL";
	sprintf(str,"%x",ctrlChar);
	return str;
}



void
LookIO ( int     device,
         FILE*   fout )

{

   int  idx;
   int status;

   struct termio  this;

   if (ioctl ( device, TCGETA, &this ) == -1)
		perror("LookIO: ioctl");

/*
   fprintf (  "\nThese are manifest constants from <termio.h>" );
   fprintf (  "\nNCC    (8) = %d", NCC );
   fprintf (  "\nVINTR  (0) = %d", VINTR );
   fprintf (  "\nVQUIT  (1) = %d", VQUIT );
   fprintf (  "\nVERASE (2) = %d", VERASE );
   fprintf (  "\nVKILL  (3) = %d", VKILL );
   fprintf (  "\nVEOF   (4) = %d", VEOF );
   fprintf (  "\nVEOL   (5) = %d", VEOL );
   fprintf (  "\nVEOL2  (6) = %d", VEOL2 );
   fprintf (  "\nVMIN   (4) = %d", VMIN );
   fprintf (  "\nVTIME  (5) = %d", VTIME );
   fprintf (  "\nVSWTCH (7) = %d", VSWTCH );
   fprintf (  "\nCNUL    (000) = %03o",  CNUL );
   fprintf (  "\nCDEL    (0377)= %03o",  CDEL );
   fprintf (  "\nCESC    ('\\') = %c",   CESC );
   fprintf (  "\nCINTR   (0177)= %03o",  CINTR );
   fprintf (  "\nCQUIT   (034) = %03o",  CQUIT );
   fprintf (  "\nCERASE  (010) = %03o",  CERASE );
   fprintf (  "\nCKILL   (025) = %03o",  CKILL );
   fprintf (  "\nCEOF    (004) = %03o",  CEOF );
   fprintf (  "\nCSTART  (021) = %03o",  CSTART );
   fprintf (  "\nCSTOP   (023) = %03o",  CSTOP );
   ffprintf (  "\nCSWTCH  (032) = %03o",  CSWTCH );
   fprintf (  "\nCNSWTCH (000) = %03o",  CNSWTCH );
   fprintf (  "\nCSUSP   (032) = %03o",  CSUSP );
   fprintf (  "\nVCEOF   (NCC) = %03o",  VCEOF );
   fprintf (  "\nVCEOL (NCC+1) = %03o",  VCEOL );
   fprintf (  "\n" );
*/

   printf ("\n***  input modes  ***" );
   printf ("\nc_iflag = %07o (octal)", this.c_iflag );
   if (this.c_iflag&IGNBRK)
	   printf ("\nIGNBRK is set");
   printf ("\nBRKINT  = 0000002%s", this.c_iflag&BRKINT? "  set":"");
   printf ("\nIGNPAR  = 0000004%s", this.c_iflag&IGNPAR? "  set":"");
   printf ("\nPARMRK  = 0000010%s", this.c_iflag&PARMRK? "  set":"");
   printf ("\nINPCK   = 0000020%s", this.c_iflag&INPCK?  "  set":"");
   printf ("\nISTRIP  = 0000040%s", this.c_iflag&ISTRIP? "  set":"");
   printf ("\nINLCR   = 0000100%s", this.c_iflag&INLCR?  "  set":"");
   printf ("\nIGNCR   = 0000200%s", this.c_iflag&IGNCR?  "  set":"");
   printf ("\nICRNL   = 0000400%s", this.c_iflag&ICRNL?  "  set":"");
   printf ("\nIUCLC   = 0001000%s", this.c_iflag&IUCLC?  "  set":"");
   printf ("\nIXON    = 0002000%s", this.c_iflag&IXON?   "  set":"");
   printf ("\nIXANY   = 0004000%s", this.c_iflag&IXANY?  "  set":"");
   printf ("\nIXOFF   = 0010000%s", this.c_iflag&IXOFF?  "  set":"");
   printf ("\nDOSMODE = 0100000%s", this.c_iflag&DOSMODE?"  set":"");
   printf ("\n" );


   printf ("\n***  output modes  ***" );
   printf ("\nc_oflag= %07o (octal)", this.c_oflag );
   printf ("\nOPOST  = 0000001%s", this.c_oflag&OPOST?  "  set":"" );
   printf ("\nOLCUC  = 0000002%s", this.c_oflag&OLCUC?  "  set":"" );
   printf ("\nONLCR  = 0000004%s", this.c_oflag&ONLCR?  "  set":"" );
   printf ("\nOCRNL  = 0000010%s", this.c_oflag&OCRNL?  "  set":"" );
   printf ("\nONOCR  = 0000020%s", this.c_oflag&ONOCR?  "  set":"" );
   printf ("\nONLRET = 0000040%s", this.c_oflag&ONLRET? "  set":"" );
   printf ("\nOFILL  = 0000100%s", this.c_oflag&OFILL?  "  set":"" );
   printf ("\nOFDEL  = 0000200%s", this.c_oflag&OFDEL?  "  set":"" );
   printf ("\nNLDLY  = 0000400%s", this.c_oflag&NLDLY?  "  set":"" );
   printf ("\nNL0    = 0      %s", this.c_oflag&NL0?    "  set":"" );
   printf ("\nNL1    = 0000400%s", this.c_oflag&NL1?    "  set":"" );
   printf ("\nCRDLY  = 0003000%s", this.c_oflag&CRDLY?  "  set":"" );
   printf ("\nCR0    = 0      %s", this.c_oflag&CR0?    "  set":"" );
   printf ("\nCR1    = 0001000%s", this.c_oflag&CR1?    "  set":"" );
   printf ("\nCR2    = 0002000%s", this.c_oflag&CR2?    "  set":"" );
   printf ("\nCR3    = 0003000%s", this.c_oflag&CR3?    "  set":"" );
   printf ("\nTABDLY = 0014000%s", this.c_oflag&TABDLY? "  set":"" );
   printf ("\nTAB0   = 0      %s", this.c_oflag&TAB0?   "  set":"" );
   printf ("\nTAB1   = 0004000%s", this.c_oflag&TAB1?   "  set":"" );
   printf ("\nTAB2   = 0010000%s", this.c_oflag&TAB2?   "  set":"" );
   printf ("\nTAB3   = 0014000%s", this.c_oflag&TAB3?   "  set":"" );
   printf ("\nBSDLY  = 0020000%s", this.c_oflag&BSDLY?  "  set":"" );
   printf ("\nBS0    = 0      %s", this.c_oflag&BS0?    "  set":"" );
   printf ("\nBS1    = 0020000%s", this.c_oflag&BS1?    "  set":"" );
   printf ("\nVTDLY  = 0040000%s", this.c_oflag&VTDLY?  "  set":"" );
   printf ("\nVT0    = 0      %s", this.c_oflag&VT0?    "  set":"" );
   printf ("\nVT1    = 0040000%s", this.c_oflag&VT1?    "  set":"" );
   printf ("\nFFDLY  = 0100000%s", this.c_oflag&FFDLY?  "  set":"" );
   printf ("\nFF0    = 0      %s", this.c_oflag&FF0?    "  set":"" );
   printf ("\nFF1    = 0100000%s", this.c_oflag&FF1?    "  set":"" );
   printf ("\n" );

   printf ("\n***  control modes  ***" );
   printf ("\nc_cflag = %07o (octal)", this.c_cflag );
   printf ("\n&CBAUD  = %07o", this.c_cflag & CBAUD );
   switch ( this.c_cflag & CBAUD )  {
      default:           printf ("  => ???" );         break;
      case ( B0 ):       printf ("  => hangup" );      break;
      case ( B50 ):      printf ("  => 50" );          break;
      case ( B75 ):      printf ("  => 75" );          break;
      case ( B110 ):     printf ("  => 110" );         break;
      case ( B134 ):     printf ("  => 134.5" );       break;
      case ( B150 ):     printf ("  => 150" );         break;
      case ( B200 ):     printf ("  => 200" );         break;
      case ( B300 ):     printf ("  => 300" );         break;
      case ( B600 ):     printf ("  => 600" );          break;
      case ( B1200 ):    printf ("  => 1200" );        break;
      case ( B1800 ):    printf ("  => 1800" );        break;
      case ( B2400 ):    printf ("  => 2400" );        break;
      case ( B4800 ):    printf ("  => 4800" );        break;
      case ( B9600 ):    printf ("  => 9600" );        break;
      case ( B19200 ):   printf ("  => 19200" );        break;
      case ( B38400 ):    printf ("  => 38400" );        break;
      //case ( EXTA ):     printf ("  => EXTA/19.2k" );  break;
      //case ( EXTB ):     printf ("  => EXTB/38.4k" );  break;
   }
   printf ("\nCS5     = 0      %s",(this.c_cflag&CSIZE)==CS5?    "  set":"" );
   printf ("\nCS6     = 0000020%s",(this.c_cflag&CSIZE)==CS6?    "  set":"" );
   printf ("\nCS7     = 0000040%s",(this.c_cflag&CSIZE)==CS7?    "  set":"" );
   printf ("\nCS8     = 0000060%s",(this.c_cflag&CSIZE)==CS8?    "  set":"" );
   printf ("\nCSTOPB  = 0000100%s", this.c_cflag&CSTOPB?  "  set":"" );
   printf ("\nCREAD   = 0000200%s", this.c_cflag&CREAD?   "  set":"" );
   printf ("\nPARENB  = 0000400%s", this.c_cflag&PARENB?  "  set":"" );
   printf ("\nPARODD  = 0001000%s", this.c_cflag&PARODD?  "  set":"" );
   printf ("\nHUPCL   = 0002000%s", this.c_cflag&HUPCL?   "  set":"" );
   printf ("\nCLOCAL  = 0004000%s", this.c_cflag&CLOCAL?  "  set":"" );
   printf ("\nRCV1EN  = 0010000%s", this.c_cflag&RCV1EN?  "  set":"" );
   printf ("\nXMT1EN  = 0020000%s", this.c_cflag&XMT1EN?  "  set":"" );
   printf ("\nLOBLK   = 0040000%s", this.c_cflag&LOBLK?   "  set":"" );
   printf ("\nXCLUDE  = 0100000%s", this.c_cflag&XCLUDE?  "  set":"" );
   printf ("\nXLOBLK  = 0010000%s", this.c_cflag&XLOBLK?  "  set":"" );
   printf ("\nCTSFLOW = 0020000%s", this.c_cflag&CTSFLOW? "  set":"" );
   printf ("\nRTSFLOW = 0040000%s", this.c_cflag&RTSFLOW? "  set":"" );
   printf ("\n" );

   printf ("\n***  line discipline modes  ***" );
   printf ("\n.c_lflag  = %7o (octal)", this.c_lflag );
   printf ("\nISIG      = 0000001%s", this.c_lflag&ISIG?   "  set":"" );
   printf ("\nICANON    = 0000002%s", this.c_lflag&ICANON? "  set":"" );
   printf ("\nXCASE     = 0000004%s", this.c_lflag&XCASE?  "  set":"" );
   printf ("\nECHO      = 0000010%s", this.c_lflag&ECHO?   "  set":"" );
   printf ("\nECHOE     = 0000020%s", this.c_lflag&ECHOE?  "  set":"" );
   printf ("\nECHOK     = 0000040%s", this.c_lflag&ECHOK?  "  set":"" );
   printf ("\nECHONL    = 0000100%s", this.c_lflag&ECHONL? "  set":"" );
   printf ("\nNOFLSH    = 0000200%s", this.c_lflag&NOFLSH? "  set":"" );
   printf ("\n" );

   printf ("\n***  line discipline  ***" );
   printf ("\n.c_line = %x", this.c_line );
   printf ("\n" );
   
   printf ("\n***  control characters  ***" );
   for  ( idx = 0;  idx < NCC;  idx++ )  {
      printf ("\n.c_cc[%s] = %s",
               s_VInterpret ( idx ),
               s_CInterpret ( this.c_cc[idx] ) );
   }
   printf ("\n" );
   fflush(fout);
}


