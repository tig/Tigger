
#define	WIDTH_BITMAP	64
#define	HEIGHT_BITMAP	64
#define 	DELTA_X			 8   /* Pixel units between bitmaps */
#define  X_START			 9   /* changed from 4 to center on screen..pt 6/91 */

#define  HIGHLIGHT_WIDTH 2
#define  BRACKET_OFFSET 2
#define  Y_SPACING      7  /* How much below the thumbnails the line is..pt */
#define  INTER_BRACKET  0
#define  EXTER_BRACKET  4

#define  LINE_SIZE        1

typedef struct tViewBoxStruc {
		HWND	hWndList;
		RECT	rList;			/* Rect containing bmps */
		WORD	nNumBitmaps;	/* Count of bitmaps in this sublibrary */
		WORD	nNumClips;	    /* Count of cliparts in this sublibrary */
		WORD	nSelect;			/* Index of selected bitmap */
		HWND	hWndScroll;		
		SWORD	nScrollPos;		/* Current Position of scroll bar */
		SWORD	nMaxScrollPos;		
		SWORD	nMaxVisible;   /* Max number of bmps visible at one time */
		SWORD	nPageInc;	   /* Increment factor for processing PAGEUP and PAGEDOWN */
		HANDLE hLib;
} VBS;	


/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/

extern LONG FAR PASCAL	WSViewBoxDlgProc(HWND, unsigned, WORD, LONG);
extern VOID FAR PASCAL	CreateViewBox(int whichSublib, WORD whichImage);

/*-------------------------------------------------------------------------*/
/*                           L O C A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/

BOOL NEAR PASCAL UpdateVBScroll(WORD, LONG, VBS *);
LPSTR	FAR PASCAL LoadRCData(HANDLE, WORD);
void FAR PASCAL UnloadRCData(LPSTR lpData);
