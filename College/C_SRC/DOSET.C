/*****************************************************
*         DoSet.c (CALCULATES MANDELBRO-SET)
*    by Peter B. Schroeder 12-10-85 (updated 1-29-86)
*   11550 SW 108 Court  Miami, FL 33176 (305)238-5509
* Program asks for coordinates (lower left-hand corner)
* and range of display. It then opens for output a data
* file specified in the command line or, by default,
* called ZOOM.DATA; writes the coordinates and range to
* the file; then multiples the complex number based on
* the x,y values of a 200x200 pixel array for 1,000
* iterations or until the sum of the squares of two
* parts of the complex number reach or exceed four. The
* count-of-iterations for each pixel (a number from
* 1 to 1,000) are coded into two bytes and these values
* are written to the data file row by row for display
* by the ViewSet program.
******************************************************/
#include <stdio.h>
#include <math.h>

main(argc,argv) int argc;char *argv[];
{
 int y,x,count,totct;
 float x_coord,y_coord,range,gap,size,a,b,ac,bc,b1;
 char ct[201][2];
 FILE *OutFile;
  /* Input x-y coordinates and range from keyboard   */
  printf("Input X_COORDINATE: ");
  scanf("%f",&x_coord);
  printf("Input Y_COORDINATE: ");
  scanf("%f",&y_coord);
  printf("Input        RANGE: ");
  scanf("%f",&range);
  gap = range / 200.0;    /* Increment per pixel     */
  y_coord += range;       /* Start at top of display */
  /* Open output file     (default or command line)  */
  if(argc<=1) OutFile = fopen("df1:ZOOM.DATA","w");
  else OutFile = fopen(argv[1],"w");
  /* Write coordinates and range to data file        */
  fprintf(OutFile,"%7.6f\n",x_coord);
  fprintf(OutFile,"%7.6f\n",y_coord);
  fprintf(OutFile,"%8.7f\n",range);
  /* Calculate count value for each pixel (200X200)  */
  for(y=1;y<=200;y++)       /* Each row              */
  {
   bc = y_coord - y*gap; totct = 0;
   for(x=1;x<=200;x++)       /* Each pixel per row   */
   {
    ac = x*gap + x_coord;
    a = ac; b = bc; size = 0.0; count = 0;
    while(size < 4.0 && count < 1000)
    {
     /* Do complex-number multiply                   */
     b1 = 2*a*b;
     a = a*a - b*b + ac;
     b = b1 + bc;
     /* Pythagorean theorem                          */
     size = a*a + b*b;
     /* Don't need square root                       */
     count++;
    }
    totct += count;
    /* Code count in two bytes to save disk space    */
    ct[x][0] = count/256;
    ct[x][1] = count % 256;
   } /* End x loop                                   */
   /* Show row number and average count to CRT       */
   printf("%5d  %5d\n",y,totct/200);
   /* Print coded pixel-values this row to data file */
   for(x=1;x<=200;x++)
   {
    putc(ct[x][0],OutFile);
    putc(ct[x][1],OutFile);
   }
  } /* End y loop                                    */
  fclose(OutFile);  /* Close data file               */
 } /* End main                                       */
