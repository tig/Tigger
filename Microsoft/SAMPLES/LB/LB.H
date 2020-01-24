/************************************************************************
 *
 *     Project:  Owner Draw Listboxes For Road Runner
 *
 *      Module:  lb.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#define MAX_DESCLEN  81

/************************************************************************
 * Data structures
 ***********************************************************************/
typedef struct tagTOPICLBITEM
{
   //
   // String associated with the item.  In the 'real' app this will not
   // be allocated here, it'll just be a pointer to somewhere in Kraig
   // land.
   //
   char  szDesc[MAX_DESCLEN] ;


   //
   // Topics have nTypes < 0.  Incidents => 0
   //
   short  nType ;

   //
   // What level (nested) is this item at?
   //
   WORD  wLevel ;

   //
   // How many children does this item have?
   //
   BOOL  fChildren ;

} TOPICLBITEM, *PTOPICLBITEM, FAR *LPTOPICLBITEM ;

//
// Incident types
//
#define INC_SPECIAL     0
#define INC_NORMAL      1 
#define INC_BORING      2
#define INC_IMPORTANT   3
#define INC_QUESTION    4

BOOL FAR PASCAL lbReset( HWND hLB ) ;

BOOL FAR PASCAL lbFill( HWND hLB, LPSTR lpData, WORD wFlags ) ;

BOOL FAR PASCAL lbProcessCmd( HWND hWnd, WORD wCtlMsg ) ;
BOOL FAR PASCAL lbExpand( HWND hLB, WORD wIndex, LPTOPICLBITEM lpTLBI ) ;
BOOL FAR PASCAL lbCollapse( HWND hLB, WORD wIndex, LPTOPICLBITEM lpTLBI ) ;

LONG FAR PASCAL lbOwnerDraw( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam ) ;

BOOL NEAR PASCAL
lbFindFirstData( LPSTR lpData, LPSTR lpSubData,
                 WORD wFlags, LPTOPICLBITEM lpTLBI ) ;

BOOL NEAR PASCAL
lbFindNextData( LPSTR lpData, LPSTR lpSubData,
                 WORD wFlags, LPTOPICLBITEM lpTLBI ) ;

BOOL FAR PASCAL
   lbInsert( HWND hLB, LPTOPICLBITEM lpTLBI, WORD wIndex ) ;

BOOL FAR PASCAL
   lbGet( HWND hLB, LPTOPICLBITEM lpTLBI, WORD wIndex ) ;


/* Indexes into the mondo bitmap */
#define BM_INCIDENT_SPECIAL      0	
#define BM_INCIDENT_TEXT         1
#define BM_INCIDENT_BLANK        2
#define BM_INCIDENT_BANG         3
#define BM_INCIDENT_QUESTION     4
#define BM_TOPIC_CLOSE	         5
#define BM_TOPIC_CLOSEPLUS       6
#define BM_TOPIC_OPEN            7
#define BM_TOPIC_OPENPLUS        8
#define BM_TOPIC_OPENMINUS       9
#define BM_TOPIC_CLOSEMINUS      10
#define BM_TOPIC_CLOSEDFS	      11
#define BM_TOPIC_OPENDFS	      12

/************************************************************************
 * End of File: lb.h
 ***********************************************************************/

