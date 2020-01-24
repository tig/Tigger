/************************************************************************
 *
 *     Project:  Apache For Windows, Copyright (c) 1991 Werner Associates
 *
 *      Module:  sliderul.c
 *
 *     Remarks:
 *
 *    This file exports the "slideruler" window class.  This window class
 *    behaves just like a list box, but allows the selection bar to have
 *    other controls on it (like combo boxes and edit controls).
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/


typedef struct tagSLIDERITEM
{
   char     szClass[MAX_CLASSNAMELEN] ;
   WORD     Style ;
   short    wWidth ;
   char     szTitle[MAX_TITLELEN] ;
} SLIDERITEM, *PSLIDERITEM, FAR *LPSLIDERITEM ;


typedef struct tagSLIDE
{
   short       nItems ;
   SLIDERITEM  rgItem[] ;
} SLIDE, *PSLIDE, FAR *LPSLIDE ;


/****************************************************************
 *  BOOL FAR PASCAL LocalSlideRulerInit( HANDLE hPrev, HANDLE hInstance )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL LocalSlideRulerInit( HANDLE hPrev, HANDLE hInstance )
{
   WNDCLASS wc ;

   if (!hPrev)
   {
      // 
      // Register the Child Windows class
      //
      wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW | CS_GLOBALCLASS ;

      wc.lpfnWndProc = StatusWndProc ;


      wc.cbClsExtra = 0 ;
      //
      // cbWndExtra holds a handle to the STATUS structure
      //
      wc.cbWndExtra = NUM_EXTRABYTES ; 
                          

      wc.hInstance = hInstance ;
      wc.hIcon = NULL ;
      wc.hCursor = LoadCursor( NULL, IDC_ARROW ) ;
      wc.hbrBackground = GetStockObject( LTGRAY_BRUSH ) ;
      wc.lpszMenuName = NULL ;

      wc.lpszClassName = szClass ;

      if (RegisterClass( &wc ))
         return TRUE ;
   }
   return TRUE ;
} /* LocalSlideRulerInit()  */


/************************************************************************
 * End of File: sliderul.c
 ***********************************************************************/

