/*****************************************************
*        ViewSet.c  (DISPLAYS MANDELBROT-SET)
*    by Peter B. Schroeder 12-10-85 (updated 1-29-86)
*   11550 SW 108 Court  Miami, FL 33176 (305)238-5509
* Program opens data file specified in command line or
* by default VIEW1.DATA; reads first three values from
* data file (x-y coordinates and range), then reads in
* and decodes display datum for each pixel; displays
* data as a 200x200 low-resolution array; shows x-y
* coordinates and range; and, in all but last view,
* asks for keyboeard input to continue; after input,
* outlines area in next-smaller display, then waits
* for CLOSEWINDOW event from close-window gadget.
* By opening additional command-line interfaces,
* several of these displays may be run in parallel.
******************************************************/

#include <stdio.h>
#include <exec/types.h>
#include <ctype.h>
#include <intuition/intuition.h>
#include <libraries/translator.h>
#include <devices/narrator.h>
long TranslatorBase;            /* For translation   */
UBYTE allocationMap[]={1,2,4,8}; /* Audio channels   */
extern struct MsgPort *CreatePort(); /* For speech   */
struct narrator_rb spk;              /* For speech   */
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct NewScreen NewScreen =
{
 0,                   /* LeftEdge                    */
 0,                   /* TopEdge                     */
 320,                 /* Width (low resolution)      */
 200,                 /* Height                      */
 4,                   /* Depth (16 colors)           */
 0,1,                 /* DetailPen and BlockPen      */
 NULL,                /* No special display modes    */
 CUSTOMSCREEN,        /* Screen type (not WorkBench) */
 NULL,                /* Use default font            */
 "Mandelbrot Screen", /* Compiled as a text pointer  */
 NULL,                /* No special screen gadgets   */
 NULL,                /* No special CustomBitMap     */
};

main(argc,argv)  int argc; char *argv[];
{
 struct Screen *Screen;
 struct NewWindow NewWindow;
 struct Window *Window;
 struct RastPort *rp;
 struct ViewPort *vp;
 struct View *v;
 struct IntuiMessage *message;
 extern struct narrator_rb spk;
 int x,y,l,count,class,rtncode,error;
 extern long TranslatorBase;     /* Must be external */
 char file[15],axis[4][20],*sp,trs[250];
 unsigned char a,b;              /* Used for coding  */
 FILE *InFile;
 /* Open file from command line or default           */
  if(argc<=1) strcpy(file,"VIEW1.DATA");
  else strcpy(file,argv[1]);
  InFile = fopen(file,"r");
  if(InFile==NULL) exit(TRUE);
  /* Read in coordinates and range                   */
  for(x=0;x<3;x++) fgets(axis[x],20,InFile);
  /* Translate speech string                         */
  sp =
 "Press any key to show area covered by next display.";
  TranslatorBase=OpenLibrary("translator.library",0);
  rtncode=Translate(sp,strlen(sp),trs,250);
  /* Phonetic code now in string trs - CloseLibrary  */
  CloseLibrary(TranslatorBase);
  error=OpenDevice("narrator.device",0,&spk,0);
  /* Set up narrator message                         */
   spk.message.io_Message.mn_ReplyPort =
                                 CreatePort("sayit",0);
   spk.ch_masks=allocationMap; /* All audio channels */
   spk.nm_masks=4;             /* Four of them       */
   spk.message.io_Command=CMD_WRITE; /* What to do   */
   spk.message.io_Data=trs;    /* What to verbalize  */
   spk.message.io_Length=strlen(trs);
  /* Open Intuition Library                          */
  IntuitionBase = (struct IntuitionBase *)
                    OpenLibrary("intuition.library",0);
  if(IntuitionBase == NULL) exit(TRUE);
  /* Open Graphics Library                           */
  GfxBase =
   (struct GfxBase *)OpenLibrary("graphics.library",0);
   if(GfxBase == NULL) exit(TRUE);
  /* Open Screen in Intuition                        */
  if((Screen = (struct Screen *)OpenScreen(&NewScreen))
             == NULL) exit(TRUE);
  /* Initialize new window structure                 */
  NewWindow. LeftEdge = 0;
  NewWindow.TopEdge = 0;
  NewWindow.Width = 290; /* don't cover screen width */
  NewWindow.Height =200;
  NewWindow.DetailPen = 0;
  NewWindow.BlockPen = 1;
  if(index(file,"W1")>=0)
                NewWindow.Title = "Mandelbrot View #1";
  else NewWindow.Title = "Mandelbrot View";
  NewWindow.Flags =WINDOWCLOSE|SMART_REFRESH|ACTIVATE|
    GIMMEZEROZERO|WINDOWDRAG|WINDOWDEPTH|WINDOWSIZING|
    NOCAREREFRESH;    /* See these flags in manual   */
  /* Need RAWKEY for INKEY type function             */
  /* Also need to know about CLOSEWINDOW events      */
  NewWindow.IDCMPFlags = RAWKEY|CLOSEWINDOW;
  NewWindow.Type = CUSTOMSCREEN;
  NewWindow.FirstGadget = NULL; /* No special gadget */
  NewWindow.CheckMark = NULL;   /* Don't need        */
  NewWindow.Screen = Screen;
  NewWindow.BitMap = NULL;      /* Use default       */
  NewWindow.MinWidth = 100;
  NewWindow.MinHeight = 25;
  NewWindow.MaxWidth = 320;
  NewWindow.MaxHeight = 200;
  /* Open new window but end if can't have new one   */
  if((Window = (struct Window *)OpenWindow(&NewWindow))
             == NULL) exit(TRUE);
  /* Need the RastPort pointer from window structure */
  (struct RastPort *)rp = Window->RPort;
  /* Also need ViewPort and View structure pointers  */
  (struct ViewPort *)vp = ViewPortAddress(Window);
  (struct View *)v = ViewAddress();
  SetDrMd(rp,JAM1); /* Set draw mode to one pen      */
  /* Get count value from file and color each pixel  */
   for(y=1;y<200;y++)     /* Each row                */
   {
    for(x=0;x<200;x++)    /* Each column or pixel    */
    {
     a=getc(InFile);          /* Unsigned characters */
     b=getc(InFile);          /* for coded numbers   */
     count = a*255 + b;   /* Decode count            */
     /* Select a color based on counts using color   *
     ** function and color each pixel accordingly    */
     SetAPen(rp,color(count)); /* Select a color     */
     WritePixel(rp,x,y);       /* and color pixel    */
    }
   }
  fclose(InFile);              /* Close data file    */
  /* Show coordinates and range of present display   */
  SetAPen(rp,1);               /* Text in white      */
  Move(rp,210,30);
  Text(rp," x =",4);
  Move(rp,201,40);
  Text(rp,axis[0],9);
  Move(rp,210,55);
  Text(rp," y =",4);
  Move(rp,201,65);
  Text(rp,axis[1],9);
  Move(rp,210,80);
  Text(rp,"range =",7);
  Move(rp,201,90);
  Text(rp,axis[2],9);
  /* Determine coordinates of next-smaller display    */
  l=0; if(index(file,"W1")>=0)
  { x=80; y=108;l=1; }            /* First view       */
  else if(index(file,"W2")>=0)
  { x=150;y=64; l=1; }            /* Second view      */
  else if(index(file,"W3")>=0)
  { x=170;y=1;  l=1; }            /* Third view       */
  else if(index(file,"W4")>=0)
  { x=140;y=100;l=1; }            /* Fourth view      */
  if(l>0)
  {
   DoIO(&spk); /* speak and wait for any key pressed  */
   Wait(1<<Window->UserPort->mp_SigBit);
   /* Draw box showing next smaller display, if any   */
   Move(rp,x,y);
   x += 20;
   Draw(rp,x,y);
   y += 20;
   Draw(rp,x,y);
   x -= 20;
   Draw(rp,x,y);
   y -= 20;
   Draw(rp,x,y);
  }
  /* Wait here for CLOSEWINDOW event, ignore RAWKEY  */
  cycle:
   Wait(1<<Window->UserPort->mp_SigBit); /*Wait input*/
   message = GetMsg(Window->UserPort);   /* Get it   */
   class = message->Class;             /* What is it */
   if(class==RAWKEY)  goto cycle;   /* Ignore RAWKEY */
    /* If CLOSEWINDOW event then end program         */
    DeletePort(spk.message.io_Message.mn_ReplyPort);
    CloseDevice(&spk);
    CloseWindow(Window);
    CloseScreen(Screen);
    CloseLibrary(GfxBase);
    CloseLibrary(IntuitionBase);
    exit(TRUE);
} /*end main*/

/* Function index from Kernighan and Ritchie p.68    */
 index(s,t) char s[],t[];
 {
  int i,j,k;
  for(i=0;s[i]!='\0';i++)
  {
   for(j=i,k=0;t[k]!='\0' && s[j]==t[k];j++,k++);
   if(t[k]=='\0') return(i);
  } 
  return(-1);
 } /* End function index                             */

/* Function color (assigns color to counts).         *
** Input: integer count. Output integer nc (color)   *
** This code can be modified to give the desired     *
** colors for different counts and ranges of counts. */
 int color(count) int count;
 {
  int nc;                        /*  2 = black       */
   if(count>995) nc=9;           /*  3 = orange      */
   else if(count>400) nc =  1;   /*  4 = bright blue */
   else if(count>350) nc = 10;   /*  5 = violet      */
   else if(count>300) nc =  4;   /*  6 = sky blue    */
   else if(count>250) nc =  3;   /*  7 = white       */
   else if(count>200) nc =  8;   /*  8 = brown       */
   else if(count>150) nc = 11;   /*  9 = red         */
   else if(count>100) nc = 13;   /* 10 = green       */
   else if(count> 90) nc =  6;   /* 11 = yellow      */
   else if(count> 80) nc = 15;   /* 12 = bright blue */
   else if(count> 70) nc =  5;   /* 13 = grey blue   */
   else if(count> 60) nc =  2;   /* 14 = green       */
   else if(count> 50) nc =  7;   /* 15 = grey        */
   else if(count> 45) nc = 12;
   else if(count> 40) nc =  0;
   else if(count> 35) nc = 15;
   else if(count> 30) nc =  3;
   else if(count> 20) nc = 11;
   else if(count> 10) nc =  4;
   else nc = 14;
   if(nc ==  4 && count % 2 == 0) nc = 8;
   if(nc == 14 && count % 2 == 0) nc = 8;
  return(nc);
 } /*End function color */


                                                                                       