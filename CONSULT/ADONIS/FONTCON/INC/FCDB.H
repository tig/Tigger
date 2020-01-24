//*************************************************************
//  File name: FCDB.h
//
//  Description: 
//      Main header file for the FCDB.DLL 
//
//  History:    Date       Author     Comment
//               2/27/92   MSM        Created
//
//*************************************************************

#ifndef _FCDB_H_
#define _FCDB_H_

#if (_MSC_VER==600)
#define _find_t find_t
#endif

#include "..\..\inc\conxmem.h"

//*** Remove the // to make Fontshopper use RLE8 bitmaps
// #define MAKE_RLE8    1


#define ELB_FOLDERS                 501

#define THUMB_CX                    512
#define THUMB_CY                     64
#define THUMB_POINTSIZE              32

//*** String table defines (error codes) and such

    #define IDS_FS                  0x0000

    #define IDS_MEMALLOC            0x0001
    #define IDS_FILENOTFOUND        0x0002
    #define IDS_FILEOPENFAILED      0x0003
    #define IDS_FILEREADFAILED      0x0004
    #define IDS_FILEWRITEFAILED     0x0005
    #define IDS_FILENOTPUBDB        0x0006
    #define IDS_FILENOTFAMILYDB     0x0007
    #define IDS_DBCORRUPT           0x0008
    #define IDS_RENAMEFAILURE       0x0009
    #define IDS_DBNOTFOUND          0x000a

    #define SZ_ELBCLASS "SHOPPER.EXTENDED_LISTBOX"

    //*** Error strings
    #define IDS_TECHSUPPORT     0x0042 
    #define IDS_FATALSTARTUP    0x0043
    #define IDS_DISKERROR       0x0044
    #define IDS_CORRUPTWARN     0x0045
    #define IDS_MEMORYONLOAD    0x0046
    #define IDS_RECOVERERROR    0x0047
    #define IDS_NODB            0x0048


#ifndef RC_INVOKED

BOOL WINAPI InitializeFCDB( VOID );

//*************************************************************
//***
//***   Extended Listbox Definitions
//***
//*************************************************************
extern struct tgDB;
typedef struct tgDB FAR *LPDB;

#define ELBEXTRA        2
LONG    FAR PASCAL  ELBWndProc(HWND,UINT,WPARAM,LPARAM);

//*** Defines and macros

    //*** Specific to a DBNODE
    #define VIB_BMPFOUND     0x8000
    #define VIB_BMPNOTFOUND  0x4000

    #define VI_ISBMPFOUND(DBNODE)    ((DBNODE->dbn.wViewInfo & VIB_BMPFOUND))
    #define VI_ISBMPNOTFOUND(DBNODE) ((DBNODE->dbn.wViewInfo & VIB_BMPNOTFOUND))

    #define VI_BMPUNCHECKED(DBNODE)  (DBNODE->dbn.wViewInfo &= ~(VIB_BMPFOUND|VIB_BMPNOTFOUND))
    #define VI_BMPFOUND(DBNODE)      (DBNODE->dbn.wViewInfo |= VIB_BMPFOUND)
    #define VI_BMPNOTFOUND(DBNODE)   (DBNODE->dbn.wViewInfo |= VIB_BMPNOTFOUND)

    //*** Specific to a ELBNODE
    #define VIB_DOWNLOAD        0x0080
    #define VIB_DROPSHIP        0x0040
    #define VIB_CHECKED         0x0020
    #define VIB_NONEXPANDABLE   0x0010
    #define VIB_STATUSOPEN      0x0008
    #define VIB_TABS            0x0007

    #define VI_ISCHECKABLE(ENODE)    (((LPELBNODE)ENODE)->wViewInfo & (VIB_DOWNLOAD|VIB_DROPSHIP))
    #define VI_ISDOWNLOADABLE(ENODE) (((LPELBNODE)ENODE)->wViewInfo & VIB_DOWNLOAD)
    #define VI_ISSHIPABLE(ENODE)     (((LPELBNODE)ENODE)->wViewInfo & VIB_DROPSHIP)

    #define VI_CHECKNODE(ENODE)   (((LPELBNODE)ENODE)->wViewInfo |= VIB_CHECKED)  
    #define VI_UNCHECKNODE(ENODE) (((LPELBNODE)ENODE)->wViewInfo &= ~VIB_CHECKED)
    #define VI_ISCHECKED(ENODE)   (((LPELBNODE)ENODE)->wViewInfo & VIB_CHECKED)

    #define VI_EXPANDABLE(ENODE)  (!(((LPELBNODE)ENODE)->wViewInfo & VIB_NONEXPANDABLE))
    #define VI_OPENNODE(ENODE)    (((LPELBNODE)ENODE)->wViewInfo |= VIB_STATUSOPEN)
    #define VI_CLOSENODE(ENODE)   (((LPELBNODE)ENODE)->wViewInfo &= ~VIB_STATUSOPEN)
    #define VI_SETTABLEVEL(ENODE,LVL) (((LPELBNODE)ENODE)->wViewInfo = (((LPELBNODE)ENODE)->wViewInfo&~VIB_TABS)|LVL)
    #define VI_GETTABLEVEL(ENODE) (((LPELBNODE)ENODE)->wViewInfo & VIB_TABS)  
    #define VI_GETOPENSTATUS(ENODE)   (((LPELBNODE)ENODE)->wViewInfo & VIB_STATUSOPEN)  


//*** Typedefs and structures
    typedef struct tgELBNODE
    {
        WORD    wNodeType;          // NodeType
        WORD    wPANOSEInfo;        // Info for the PANOSE sort
        WORD    wViewInfo;          // Info for the view
    } ELBNODE, FAR *LPELBNODE;

    // Sort proc UINT uData and returns same notation as strcmp
    typedef long (CALLBACK* ELBSORTPROC)(HWND,LPELBNODE,LPELBNODE);
    typedef long (CALLBACK* ELBPAINTPROC)(PES,HDC,LPELBNODE,LPRECT);

    typedef struct
    {
        HWND            hELB;       // Handle to this ELB
        HWND            hLB;        // Handle to child ListBox
        HBITMAP         hBitmap;    // Folders bitamps
        WORD            wBmpH;      // Height of 1 folder
        WORD            wBmpW;      // Width of 1 folder
        HFONT           hFont;      // Font to use
        WORD            wFontH;     // Height of the font
        ELBPAINTPROC    lpfnPaint;  // Paint routine
        ELBSORTPROC     lpfnSort;   // Sort Routine
        LPDB            lpDB;       // Database pointer
        DRAWITEMSTRUCT  disPrev;    // previous DIS
    } ELBSTUFF, *PES, FAR *LPES;

//*** Notifications for the Extended ListBox
    #define ELBN_SELCHANGE          LBN_SELCHANGE
    #define ELBN_DBLCLK         LBN_DBLCLK
    #define ELBN_SETFOCUS       LBN_SETFOCUS
    #define ELBN_KILLFOCUS      LBN_KILLFOCUS
    #define ELBN_ENTER          10
    #define ELBN_RDBLCLK        11
    #define ELBN_SPACE          12


//*** APIs for the Extended ListBox
    BOOL WINAPI RegisterELBClass( HANDLE hInst, HANDLE hPrevInst );

    LONG        WINAPI ELBAddInto( HWND, UINT, LPELBNODE );
    LONG        WINAPI ELBCloseNode( HWND, UINT, UINT );
    LONG        WINAPI ELBDeleteNode( HWND, UINT );
    LPELBNODE   WINAPI ELBGetCurSel( HWND, LPINT );
    LPDB        WINAPI ELBGetDatabase( HWND );
    LPELBNODE   WINAPI ELBGetItem( HWND, UINT );
    ELBPAINTPROC WINAPI ELBGetPaintProc( HWND );
    ELBSORTPROC WINAPI ELBGetSortProc( HWND );
    LONG        WINAPI ELBInsertNode( HWND, UINT, LPELBNODE );
    LONG        WINAPI ELBRemoveNode( HWND, UINT );
    LONG        WINAPI ELBResetContent( HWND );
    LONG        WINAPI ELBSetBitmap( HWND, HBITMAP, UINT );
    LONG        WINAPI ELBSetCurSel( HWND, UINT );
    BOOL        WINAPI ELBSetDatabase( HWND, LPDB );
    LONG        WINAPI ELBSetFont( HWND, HFONT );
    LONG        WINAPI ELBSetPaintProc( HWND, ELBPAINTPROC );
    LONG        WINAPI ELBSetSortProc( HWND, ELBSORTPROC );
    VOID        WINAPI SelectByNode( HWND hELB, LPELBNODE lpen );



//*************************************************************
//***
//***   DataBase definitions
//***
//*************************************************************

//*** Important defines
    #define FSPUB_ID    0x20505346              
    #define FSF_ID      0x20465346

//*** Structures and types
    typedef WORD  FILEID;

    typedef struct
    {
        char    szPubName[ LF_FACESIZE ];
        char    szFontName[ LF_FACESIZE ];
        LONG    lPrice1;
        LONG    lPrice2;
        LONG    lSize1;
        LONG    lSize2;
    } FILTER, FAR *HFILTER;

    // The beginning elements of this structure must be the same
    // as a ELBNODE as defined in ELB.H.  Update both places!!!
    typedef struct tgDBNODE
    {
        WORD    wNodeType;              // NodeType
        WORD    wPANOSEInfo;            // Info for the PANOSE sort
        WORD    wViewInfo;              // Info for the view
        struct tgDBNODE FAR *lpParent;  // Generic pointer to parent
    } DBNODE, FAR *LPDBNODE;

    //*** These must be the same as DB_VIEW >> 1
    #define NT_PUB          0x0000
    #define NT_FAMILY       0x0001
    #define NT_FACE         0x0002
    #define NT_STYLE        0x0004
    #define NT_DB           0x0008
    #define NT_COLLECTION   0x0010


#define TTF_LENGTH  13
//*** Font Style structure as seen in memory
    typedef struct
    {
        DWORD   dwID;                           // ID for Ingres
        DWORD   dwPrice;                        // Price * 100
        DWORD   dwSize;                         // Size for download
        char    szName[LF_FACESIZE];            // Name of the Face
        PANOSE  Panose;                         // Panose number
        char    szTTFFile[TTF_LENGTH];          // TTF file name
        DWORD   dwReserved1;
        DWORD   dwReserved2;
    } STYLEINFO;

    typedef struct tgSTYLE
    {
        DBNODE      dbn;                        // Header for this node
        STYLEINFO   Info;                       // Style info
        WORD        wBitmapIndex;               // Index to bitmap for style
    } STYLE, FAR *LPSTYLE;

//*** Font Face structure as seen in memory
    typedef struct
    {
        DWORD   dwID;                           // ID for Ingres
        DWORD   dwPrice;                        // Price * 100
        DWORD   dwSize;                         // Size for download
        char    szName[LF_FACESIZE];            // Name of the Face
        WORD    wStyles;                        // Styles in the face
        DWORD   dwReserved1;
        DWORD   dwReserved2;
    } FACEINFO;

    typedef struct tgFACE
    {
        DBNODE      dbn;                        // Header for this node
        HLIST       hlstList;                   // List for this node
        FACEINFO    Info;                       // Face info
        HBITMAP     hBitmap;                    // Bitmap of the styles
        WORD        wBmpHeight;                 // Height of each style     
        WORD        wBmpWidth;                  // Width of each style     
    } FACE, FAR *LPFACE;

//*** Font Family header as seen in memory
    typedef struct
    {
        DWORD   dwID;                           // ID for Ingres
        DWORD   dwPrice;                        // Price * 100
        DWORD   dwSize;                         // Size for download
        char    szName[LF_FACESIZE];            // Name of the family
        WORD    wFaces;                         // Faces in the family
        WORD    wStyles;                        // Styles in the family
        DWORD   dwReserved1;
        DWORD   dwReserved2;
    } FSF;

    typedef struct tgFAMILY
    {
        DBNODE      dbn;                        // Header for this node
        HLIST       hlstList;                   // List for this node
        FSF         Info;                       // Family information
        FILEID      fiFF;                       // Family file
        WORD        wFileVersion;               // Version of the file
    } FAMILY, FAR *LPFAMILY;

    typedef struct
    {
        DWORD   dwID;                           // ID for Ingres
        DWORD   dwPrice;                        // Price * 100
        char    szName[LF_FACESIZE*2];          // Name of the family
        WORD    wTextSize;                      // Size of the text
        DWORD   rgShippingRates[5];             // Shipping rates
        DWORD   dwReserved1;
        DWORD   dwReserved2;
    } COLLINFO;

    typedef struct
    {
        DBNODE  dbn;                            // Header for this node
        COLLINFO Info;                          // Collection info
        LPSTR   lpszText;                       // Text when loaded
        WORD    wShipping;                      // Shipping preference
    } COLL, FAR *LPCOLL;

//*** Publisher header used in memory
    typedef struct
    {
        WORD    wNameSize;                      // Length of name + \0
        WORD    wSCopyrightSize;                // Length of scopyright+\0
        WORD    wLCopyrightSize;                // Length of lcopyright+\0
        WORD    wCollections;                   // # of collections       
        DWORD   dwReserved1;
        DWORD   dwReserved2;
    } FSP;

    typedef struct tgPUB
    {
        DBNODE  dbn;                            // Header for this node
        HLIST   hlstList;                       // List for this node
        FSP     PubInfo;                        // Publisher info
        WORD    wFamilies;                      // Family FSF files
        FILEID  fiFSPub;                        // For loading the Copyright
        WORD    wFileVersion;                   // Version of the file
        LPSTR   lpszPublisher;                  // Publisher name
        LPSTR   lpszSCopyright;                 // Copyright info
        LPSTR   lpszLCopyright;                 // Copyright info
        HLIST   hlstCollections;                // Collection List
    } PUB, FAR *LPPUB;

//*** Entire Database Header
    typedef struct tgDB
    {
        DBNODE  dbn;                            // Header for this node
        HLIST   hlstList;                       // List for this node
        HHEAP   hhHeap;                         // Heap for everything
        HFILTER hFilter;                        // Current filter
        WORD    wDBType;                        // FontShop or DBEdit
        UINT    uView;                          // Current view and grouping
        UINT    uSort;                          // Current sorting
        UINT    uPriceCol;                      // Location of column
        UINT    uSizeCol;                       // Location of column
        UINT    uPubCol;                        // Location of column
        WORD    wFonts;                         // Selected Fonts
        WORD    wColls;                         // Selected Collections
        DWORD   dwTotal;                        // Total of all selected fonts
        DWORD   dwSize;                         // Size of all selected fonts
        char    szPathName[120];                // Loaded path
    } DB, FAR *LPDB;
    #define DB_TYPE_FONTSHOP    0x0000
    #define DB_TYPE_DBEDIT      0x0001
    #define DB_TYPE_CORRUPT     0x0002

    //*** These must be the same as NT_???? << 1
    #define DB_VIEW_PUBLISHERS  0x0001
    #define DB_VIEW_FAMILIES    0x0002
    #define DB_VIEW_FACES       0x0004
    #define DB_VIEW_STYLES      0x0008
    #define DB_VIEW_ALL         0x000D

    #define DB_SORT_NAME        0x0000
    #define DB_SORT_PRICE       0x0001
    #define DB_SORT_SIZE        0x0002
    #define DB_SORT_PANOSE      0x0003
    

//*** Macros and wrappers
    #define NODETYPE(DBNODE)      (((LPDBNODE)DBNODE)->wNodeType)
    #define VIEWINFO(DBNODE)      (((LPDBNODE)DBNODE)->wViewInfo)
    #define GETHLIST(DBNODE)      (DBNODE->hlstList)
    #define GETHHEAP(DBNODE)      fsmmListHeap(DBNODE->hlstList)
    #define FACES(DBNODE)         (DBNODE->Info.wFaces)
    #define STYLES(DBNODE)        (DBNODE->Info.wStyles)
    #define NODEID(DBNODE)        (DBNODE->Info.dwID)
    #define FONTPRICE(DBNODE)     (DBNODE->Info.dwPrice)
    #define FONTSIZE(DBNODE)      (DBNODE->Info.dwSize)
    #define NODENAME(DBNODE)      (DBNODE->Info.szName)
    #define PARENT(TYPE,DBNODE)   ((TYPE)(((LPDBNODE)DBNODE)->lpParent))

    #define FIRST_CHILD(TYPE,DBNODE) (TYPE)fsmmListFirstNode(DBNODE->hlstList)

    #define ALLOC_LIST_NODE(DBNODE,NSIZE) fsmmListAllocNode(DBNODE->hlstList,NULL,NSIZE)
    #define FREE_LIST_NODE(DBNODE,THISNODE) fsmmListFreeNode(DBNODE->hlstList,(LPNODE)THISNODE)
    #define ADD_LIST_NODE(DBNODE,THISNODE) fsmmListInsertNode(DBNODE->hlstList,(LPNODE)THISNODE,LIST_INSERTTAIL)
    #define DELETE_LIST_NODE(DBNODE,THISNODE) fsmmListDeleteNode(DBNODE->hlstList,(LPNODE)THISNODE)
    #define ITEM_INDEX(DBNODE,THISNODE) fsmmListNode2Item(DBNODE->hlstList,(LPNODE)THISNODE)

    #define ALLOC_STRING(DBNODE,LPS) fsmmHeapAllocString(GETHHEAP(DBNODE),LPTR,LPS)
    #define FREE_STRING(DBNODE,LPS) fsmmHeapUnlockFree(GETHHEAP(DBNODE),LPS)

    #define ALLOCFROMHEAP(DBNODE,SZ) fsmmHeapAllocLock(GETHHEAP(DBNODE),LPTR,SZ)
    #define FREEFROMHEAP(DBNODE,LPM) fsmmHeapUnlockFree(GETHHEAP(DBNODE),(LPSTR)LPM)

//*** Function prototypes
    WORD WINAPI     ShrinkFilename( LPSTR );
    VOID WINAPI     ExpandFilename(LPDB, WORD, FILEID, FILEID, LPSTR);
    VOID WINAPI     SetFileVersion( LPSTR, WORD );
    WORD WINAPI     GetFileVersion( LPSTR );
    WORD WINAPI     ValidateFileAndGetVersion( LPSTR );
                    #define FDB_TYPE_FSPUB     0x0000 
                    #define FDB_TYPE_FF        0x0001
                    #define FDB_TYPE_FS        0x0002
                    #define FDB_TYPE_FT        0x0003
                    #define FDB_TYPE_FY        0x0004
                    #define FDB_TYPE_DIR       0x8000

    LPSTYLE WINAPI  CreateStyle( LPFACE );
    VOID WINAPI     FreeStyle( LPSTYLE );

    LPSTYLE WINAPI  GetFirstStyle( LPFACE, HFILTER );
    LPSTYLE WINAPI  GetNextStyle( LPSTYLE, HFILTER );

    LPFACE WINAPI   CreateFace( LPFAMILY );
    VOID WINAPI     FreeFace( LPFACE );

    LPFACE WINAPI   LoadFace( LPFAMILY, HFILE );
    BOOL WINAPI     SaveFace( LPFACE, HFILE );
    LPFACE WINAPI   GetFirstFace( LPFAMILY, HFILTER );
    LPFACE WINAPI   GetNextFace( LPFACE, HFILTER );


    HBITMAP WINAPI  LoadFaceBitmap( LPFACE );

    LPFAMILY WINAPI CreateFamily( LPPUB );
    VOID WINAPI     FreeFamily( LPFAMILY );

    LPFAMILY WINAPI LoadFamily( LPPUB, LPSTR  );
    BOOL WINAPI     SaveFamily( LPFAMILY, WORD );
    LPFAMILY WINAPI GetFirstFamily( LPPUB, HFILTER );
    LPFAMILY WINAPI GetNextFamily( LPFAMILY, HFILTER );
    HBITMAP WINAPI  LoadFamilyBitmap( LPFAMILY );

    BOOL WINAPI     AreThumbsPresent( LPFAMILY, WORD );

    LPCOLL WINAPI   CreateCollection( LPPUB );
    VOID   WINAPI   FreeCollection( LPCOLL );
    LPCOLL WINAPI   GetFirstCollection( LPPUB );
    LPCOLL WINAPI   GetNextCollection( LPCOLL );
    BOOL WINAPI     LoadPublisherCollections( LPPUB );
    VOID WINAPI     FreePublisherCollections( LPPUB );
    BOOL WINAPI     LoadPublisherCollectionsText( LPPUB );
    VOID WINAPI     FreePublisherCollectionsText( LPPUB );

    LPPUB WINAPI    CreatePublisher( LPDB );
    VOID WINAPI     FreePublisher( LPPUB );
    LPPUB WINAPI    LoadPublisher( LPDB, LPSTR, HWND );
    BOOL WINAPI     SavePublisher( LPPUB, WORD );
    LPSTR WINAPI    LoadPublisherLCopyright( LPPUB );
    VOID WINAPI     FreePublisherLCopyright( LPPUB );
    LPPUB WINAPI    GetFirstPublisher( LPDB, HFILTER );
    LPPUB WINAPI    GetNextPublisher( LPPUB, HFILTER );

    LPDB WINAPI     CreateDataBase( VOID );
    VOID WINAPI     FreeDataBase( LPDB );
    VOID WINAPI     EmptyDataBase( LPDB );
    LPDB WINAPI     LoadDataBase( LPSTR, HWND );
    BOOL WINAPI     SaveDataBase( LPDB, WORD );

    VOID WINAPI     SortDatabase(LPDB lpDB);
    
    WORD WINAPI     GetLoadBitmapError( VOID );
                    #define LBE_OK              0x0000
                    #define LBE_FILENOTFOUND    0x0001
                    #define LBE_LOADLIBRARY     0x0002
                    #define LBE_LOADBITMAP      0x0003

    WORD WINAPI     GetLoadError( VOID );
                    // returns LE_OK or a DBERR_???? error value
                    #define LE_OK              0x0000

    //*** Same as MessageBox + this following additional flag and uiError
    WORD WINAPI ReportFontshopError( HWND, LPSTR, LPSTR, UINT, UINT );
        #define MB_TECHNUMBER   0x4000

    LPDBNODE WINAPI GetFirstChildDBNode( LPDBNODE, HFILTER );
    LPDBNODE WINAPI GetNextDBNode( LPDBNODE, HFILTER );

    LPDBNODE WINAPI GetFirstPurchasedNode( LPDB lpdb );
    LPDBNODE WINAPI GetNextPurchasedNode( LPDB lpdb, LPDBNODE lpDN );

//*************************************************************
//***
//***   DataBase Slider/ELB routines
//***
//*************************************************************

//*** Slider column definitions
    #define NAME_COL    0
    #define PRICE_COL   1
    #define SIZE_COL    2
    #define PUB_COL     3

//*** Function prototypes
    UINT WINAPI     FillDBELB( HWND );
    BOOL WINAPI     ExpandDBELBNode( HWND, UINT, UINT );
    BOOL WINAPI     CollapseDBELBNode( HWND, UINT, UINT );
    BOOL WINAPI     SetDBELBToView( HWND, UINT );
    BOOL WINAPI     SetDBELBToSort( HWND, UINT );
    BOOL WINAPI     SetDBELBToFilter( HWND, HFILTER );

    UINT NEAR PASCAL InsertChildrenInto( HWND, LPDB, LPDBNODE, UINT, UINT );
    BOOL NEAR PASCAL SimulateInsertChildrenInto( LPDB, LPDBNODE );

    VOID WINAPI     SyncDBELBToSlider( HWND, HWND );
    LONG CALLBACK   DBELBPaintProc(LPES,HDC,LPELBNODE,LPRECT);
    LONG CALLBACK   DBELBSortProc(HWND,LPELBNODE,LPELBNODE);

    LONG WINAPI     HandleDBELBCommand(HWND,UINT,WPARAM,LPARAM);

    BOOL WINAPI     IsAncestorChecked( LPDBNODE );

//*************************************************************
//***
//***   Dir list routines
//***
//*************************************************************
    LPSTR WINAPI BuildDirList( LPSTR lpPath, LPINT lpLen );
    VOID  WINAPI DestroyDirList( LPSTR lpList );
    LPSTR WINAPI GetNextDirListItem( LPSTR lpCurr );
    LPSTR WINAPI IsItemInDirList( LPSTR lpList, LPSTR lpItem );

#endif
#endif

//*** EOF: FCDB.h
