#include <stdio.h>

/* Bit masks for Headers/Footers */
#define HF_NONE      0x0000
#define HF_PAGENUM   0x0002
#define HF_PDATE     0x0004
#define HF_RDATE     0x0008
#define HF_FNAME     0x0010

/* defaults */
#define INCH_CM      "inches"
#define TOP_MARGIN   "0"
#define BOT_MARGIN   "0"
#define LEFT_MARGIN  "0"
#define RIGHT_MARGIN "0"
#define H_DEFAULT    (HF_PAGENUM | HF_FNAME | HF_RDATE)
#define F_DEFAULT    (HF_PDATE)
#define DEFHFMAR     "1"
#define TAB_SIZE     "8"
#define DEFPTSIZE    "10"
#define DEFFONT      "Courier"

/* WinPrint 1.32 stuff */
#define HF_BOLD      0x0002
#define HF_ITALIC    0x0004
#define HF_UNDERLINE 0x0008
#define HF_DEFSTYLE  HF_NONE
#define DEFSTYLE     HF_NONE

/* font constants */
#define MAX_FACES 16
#define MAX_SIZES 16
#define PT_SIZE_MULT 20

#define INI_SEPERATOR   "|"
#define MAX_CMDLINE     127 
#define CFG_NAME_LEN    16
#define CFG_LIST_LEN    255
#define HELP_NAME_LEN   32
#define HELP_LEN        60
#define REG_NAME_LEN    64
#define REG_NUM_LEN     11



/* ------------------ TYPEDEFS ----------------------- */
typedef struct tagOPTIONS
{
   LOGFONT  lfCurFont ;
   LOGFONT  lfHFFont ;
   WORD     wHeaderMask ;                    /* Bit mask for header */
   WORD     wFooterMask ;                    /* Bit mask for footer */
   BOOL     bHFMargins ;                     /* use margin settings for headers/footers? */
   BOOL     bExpandTabs ;                    /* Do we want to expand tabs on input */
   BOOL     bInches ;
   int      nTabSize ;                       /* how many space chars per tab */
   int      nTopMargin ;                     /* lines (should be inches/cm) */
   int      nBottomMargin ;                  
   int      nLeftMargin ;                    /* left margin size in chars (should be inches/cm) */
   int      nRightMargin ;                   /* right margin size in chars (should be inches/cm) */
   BOOL     bDraftMode ;
   BOOL     bClose ;
   BOOL     bMinimize ;
   BOOL     bGo ;

   /* Columns */
   short    nColumns ;

} OPTIONS, *OPTIONSPTR ;

typedef struct tagPAGE
{
   int         nTopMargin ;                  /* these are all in logical units */
   int         nBotMargin ;
   int         nRightMargin ;
   int         nLeftMargin ;
   int         xChar, yChar ; 
   int         nUnitsPerLine ;
   int         nUnitsPerPage ;
   int         nCharsPerLine ;
   int         nLinesPerPage ;
} PAGE, *PAGEPTR ;

typedef struct tagWENUMFACE
{
   short nNumFonts ;
   char  szFontNames [MAX_FACES] [LF_FACESIZE] ;
} WPENUMFACE ;

typedef struct tagWENUMSIZE
{
   short       nNumSizes ;
   int         nPtSize [MAX_SIZES] ;
} WPENUMSIZE ;

/* ------------------ FUNCTION PROTOTYPES ------------ */
/* winprint.c */
int PASCAL        WinMain (HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) ;
BOOL              IsMyDialogMessage (HWND hWind, LPMSG lpMsg) ;

