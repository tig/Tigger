//*************************************************************
//  File name: dbwalk.c
//
//  Description: 
//      Code for Database helper routines.
//
//  History:    Date       Author     Comment
//               2/20/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"

#include "intFCDB.h"

BOOL NEAR PASCAL CheckNodeAgainstFilter( HFILTER hFilter, LPDBNODE lpdn );
BOOL NEAR PASCAL FindString( LPSTR lpSrc, LPSTR lpStr );

LPDBNODE NEAR PASCAL GetNextPurchasedNodeRecursive( LPDBNODE lpdn );

#define OFFSETIN(struc,field)    ((int)&(((struc *)0)->field))

//*************************************************************
//
//  GetFirstStyle
//
//  Purpose:
//      Retrieves the first style that passes the filter
//
//
//  Parameters:
//      LPFACE lpFace
//      HFILTER hFilter
//      
//
//  Return: (LPSTYLE)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/20/92   MSM        Created
//
//*************************************************************

LPSTYLE WINAPI GetFirstStyle( LPFACE lpFace, HFILTER hFilter )
{
    if (hFilter)    // Are we filtering
    {
        LPSTYLE lpS = FIRST_CHILD( LPSTYLE, lpFace );

        while (lpS)
        {
            if (CheckNodeAgainstFilter(hFilter, (LPDBNODE)lpS))
                return lpS;
            lpS = (LPSTYLE)fsmmListNextNode(GETHLIST(PARENT(LPFACE,lpS)),lpS);
        }
        return NULL;
    }
    else
        return FIRST_CHILD( LPSTYLE, lpFace );

} //*** GetFirstStyle


//*************************************************************
//
//  GetNextStyle
//
//  Purpose:
//      Retrieves the next style that matches the filter
//
//
//  Parameters:
//      LPSTYLE lpStyle
//      HFILTER hFilter
//      
//
//  Return: (LPSTYLE)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/20/92   MSM        Created
//
//*************************************************************

LPSTYLE WINAPI GetNextStyle( LPSTYLE lpStyle, HFILTER hFilter )
{
    if (hFilter)    // Are we filtering
    {
        LPSTYLE lpS;

        lpS = (LPSTYLE)fsmmListNextNode(GETHLIST(PARENT(LPFACE,lpStyle)),lpStyle);

        while (lpS)
        {
            if (CheckNodeAgainstFilter(hFilter, (LPDBNODE)lpS))
                return lpS;
            lpS = (LPSTYLE)fsmmListNextNode(GETHLIST(PARENT(LPFACE,lpS)),lpS);
        }
        return NULL;
    }
    else
        return fsmmListNextNode( GETHLIST(PARENT(LPFACE,lpStyle)), lpStyle );
    
} //*** GetNextStyle


//*************************************************************
//
//  GetFirstFace
//
//  Purpose:
//      Retrieves the first face in family that passes the filter
//
//
//  Parameters:
//      LPFACE lpFace
//      HFILTER hFilter
//      
//
//  Return: (LPSTYLE)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/20/92   MSM        Created
//
//*************************************************************

LPFACE WINAPI GetFirstFace( LPFAMILY lpFamily, HFILTER hFilter )
{
    if (hFilter)    // Are we filtering
    {
        LPFACE lpF = FIRST_CHILD( LPFACE, lpFamily );

        while (lpF)
        {
            if (CheckNodeAgainstFilter(hFilter, (LPDBNODE)lpF))
                return lpF;
            lpF = (LPFACE)fsmmListNextNode(GETHLIST(PARENT(LPFAMILY,lpF)),lpF);
        }
        return NULL;
    }
    else
        return FIRST_CHILD( LPFACE, lpFamily );

} //*** GetFirstFace


//*************************************************************
//
//  GetNextFace
//
//  Purpose:
//      Retrieves the next style that matches the filter
//
//
//  Parameters:
//      LPSTYLE lpFace
//      HFILTER hFilter
//      
//
//  Return: (LPSTYLE)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/20/92   MSM        Created
//
//*************************************************************

LPFACE WINAPI GetNextFace( LPFACE lpFace, HFILTER hFilter )
{
    if (hFilter)    // Are we filtering
    {
        LPFACE lpF;

        lpF = (LPFACE)fsmmListNextNode(GETHLIST(PARENT(LPFAMILY,lpFace)),lpFace);
        while (lpF)
        {
            if (CheckNodeAgainstFilter(hFilter, (LPDBNODE)lpF))
                return lpF;
            lpF = (LPFACE)fsmmListNextNode(GETHLIST(PARENT(LPFAMILY,lpF)),lpF);
        }
        return NULL;
    }
    else
        return fsmmListNextNode( GETHLIST(PARENT(LPFAMILY,lpFace)),lpFace);
    
} //*** GetNextFace


//*************************************************************
//
//  GetFirstFamily
//
//  Purpose:
//      Returns the first family that passes the filter
//
//
//  Parameters:
//      LPPUB lpPub
//      HFILTER hFilter
//      
//
//  Return: (LPFAMILY)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/21/92   MSM        Created
//
//*************************************************************

LPFAMILY WINAPI GetFirstFamily( LPPUB lpPub, HFILTER hFilter )
{
    if (hFilter)    // Are we filtering
    {
        LPFAMILY lpF = FIRST_CHILD( LPFAMILY, lpPub );

        while (lpF)
        {
            if (CheckNodeAgainstFilter(hFilter, (LPDBNODE)lpF))
                return lpF;
            lpF = (LPFAMILY)fsmmListNextNode(GETHLIST(PARENT(LPPUB,lpF)),lpF);
        }
        return NULL;
    }
    else
        return FIRST_CHILD( LPFAMILY, lpPub );

} //*** GetFirstFamily


//*************************************************************
//
//  GetNextFamily
//
//  Purpose:
//      Returns the next family that passes the filter
//
//
//  Parameters:
//      LPFAMILY lpFamily
//      HFILTER hFilter
//      
//
//  Return: (LPFAMILY)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/21/92   MSM        Created
//
//*************************************************************

LPFAMILY WINAPI GetNextFamily( LPFAMILY lpFamily, HFILTER hFilter )
{
    if (hFilter)    // Are we filtering
    {
        LPFAMILY lpF;

        lpF = (LPFAMILY)fsmmListNextNode(GETHLIST(PARENT(LPPUB,lpFamily)),lpFamily);
        while (lpF)
        {
            if (CheckNodeAgainstFilter(hFilter, (LPDBNODE)lpF))
                return lpF;
            lpF = (LPFAMILY)fsmmListNextNode(GETHLIST(PARENT(LPPUB,lpF)),lpF);
        }
        return NULL;
    }
    else
        return fsmmListNextNode( GETHLIST(PARENT(LPPUB,lpFamily)),lpFamily);

} //*** GetNextFamily


//*************************************************************
//
//  GetFirstCollection
//
//  Purpose:
//      Retrieves the first collection
//
//
//  Parameters:
//      LPPUB lpPub
//      
//
//  Return: (LPCOLL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

LPCOLL WINAPI GetFirstCollection( LPPUB lpPub )
{
    return fsmmListFirstNode( lpPub->hlstCollections );

} //*** GetFirstCollection


//*************************************************************
//
//  GetNextCollection
//
//  Purpose:
//      Retrieves the next collection in the list
//
//
//  Parameters:
//      LPCOLL lpColl
//      
//
//  Return: (LPCOLL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

LPCOLL WINAPI GetNextCollection( LPCOLL lpColl )
{
    LPPUB lpPub = PARENT( LPPUB, lpColl );

    return fsmmListNextNode( lpPub->hlstCollections, (LPNODE)lpColl );

} //*** GetNextCollection


//*************************************************************
//
//  GetFirstPublisher
//
//  Purpose:
//      Returns the first publisher that passes the filter
//
//
//  Parameters:
//      LPDB lpDB
//      HFILTER hFilter
//      
//
//  Return: (LPPUB)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPPUB WINAPI GetFirstPublisher( LPDB lpDB, HFILTER hFilter )
{
    if (hFilter)    // Are we filtering
    {
        LPPUB lpP = FIRST_CHILD( LPPUB, lpDB );

        while (lpP)
        {
            if (CheckNodeAgainstFilter(hFilter, (LPDBNODE)lpP))
                return lpP;
            lpP = (LPPUB)fsmmListNextNode(GETHLIST(PARENT(LPDB,lpP)),lpP);
        }
        return NULL;
    }
    else
        return FIRST_CHILD( LPPUB, lpDB );

} //*** GetFirstPublisher


//*************************************************************
//
//  GetNextPublisher
//
//  Purpose:
//      Returns the next publisher that passes the filter
//
//
//  Parameters:
//      LPPUB lpPub
//      HFILTER hFilter
//      
//
//  Return: (LPPUB)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPPUB WINAPI GetNextPublisher( LPPUB lpPub, HFILTER hFilter )
{
    if (hFilter)    // Are we filtering
    {
        LPPUB lpP;

        lpP = (LPPUB)fsmmListNextNode( GETHLIST(PARENT(LPDB,lpPub)),lpPub);
        while (lpP)
        {
            if (CheckNodeAgainstFilter(hFilter, (LPDBNODE)lpP))
                return lpP;
            lpP = (LPPUB)fsmmListNextNode(GETHLIST(PARENT(LPDB,lpP)),lpP);
        }
        return NULL;
    }
    else
        return fsmmListNextNode( GETHLIST(PARENT(LPDB,lpPub)),lpPub);

} //*** GetNextPublisher


//*************************************************************
//
//  CheckNodeAgainstFilter
//
//  Purpose:
//      Checks to see if the node passed the filter
//
//
//  Parameters:
//      HFILTER hFilter
//      LPDBNODE lpdn
//      
//
//  Return: (BOOL NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/26/92   MSM        Created
//
//*************************************************************

BOOL NEAR PASCAL CheckNodeAgainstFilter( HFILTER hFilter, LPDBNODE lpdn )
{
    if (hFilter->szPubName[0])
    {
        LPPUB lpPub = (LPPUB)lpdn;

        while ( lpPub && NODETYPE(lpPub)!=NT_PUB )
            lpPub = PARENT(LPPUB,lpPub);
        if (lpPub)
        {
            if (!FindString(lpPub->lpszPublisher,hFilter->szPubName))
                return FALSE;
        }
        else
            return FALSE;
    }

    if (hFilter->szFontName[0])
    {
        if ( NODETYPE(lpdn)==NT_STYLE )
        {
            char szTemp[2*LF_FACESIZE+2];
            LPFACE lpFace = PARENT(LPFACE,lpdn);

            wsprintf( szTemp, "%s %s", (LPSTR)NODENAME(lpFace),
                (LPSTR)NODENAME(((LPSTYLE)lpdn)) );
            if (!FindString(szTemp, hFilter->szFontName))
                return FALSE;
        }
    }

    if (hFilter->lPrice1!=-1)
    {
        if ( NODETYPE(lpdn)==NT_STYLE )
        {
            LPSTYLE lpS = (LPSTYLE)lpdn;

            if (hFilter->lPrice1 > (long)FONTPRICE(lpS))
                return FALSE;
        }
    }

    if (hFilter->lPrice2!=-1)
    {
        if ( NODETYPE(lpdn)==NT_STYLE )
        {
            LPSTYLE lpS = (LPSTYLE)lpdn;

            if (hFilter->lPrice2 < (long)FONTPRICE(lpS))
                return FALSE;
        }
    }

    if (hFilter->lSize1!=-1)
    {
        if ( NODETYPE(lpdn)==NT_STYLE )
        {
            LPSTYLE lpS = (LPSTYLE)lpdn;

            if (hFilter->lSize1 > (long)FONTSIZE(lpS))
                return FALSE;
        }
    }

    if (hFilter->lSize2!=-1)
    {
        if ( NODETYPE(lpdn)==NT_STYLE )
        {
            LPSTYLE lpS = (LPSTYLE)lpdn;

            if (hFilter->lSize2 < (long)FONTSIZE(lpS))
                return FALSE;
        }
    }
    return TRUE;    

} //*** CheckNodeAgainstFilter


//*************************************************************
//
//  FindString
//
//  Purpose:
//      Finds a string in another string (case insensitive)
//
//
//  Parameters:
//      LPSTR lpSrc
//      LPSTR lpStr
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/ 8/92   MSM        Created
//
//*************************************************************

BOOL NEAR PASCAL FindString( LPSTR lpSrc, LPSTR lpStr )
{
    int   c    = toupper(lpStr[0]);
    LPSTR lp   = lpSrc;
    int   nLen = lstrlen( lpStr );

    while (TRUE)
    {
        while ( toupper(*lp) != c )
        {
            if (*lp==0)
                return FALSE;
            lp++;
        }
        if (!_fstrnicmp(lp,lpStr,nLen))
            return TRUE;
        lp++;
    }

} //*** FindString

//************************************************************************
//*** Generic NODE routines
//************************************************************************


//*************************************************************
//
//  GetFirstChildDBNODE
//
//  Purpose:
//      Retrieves the first child of the node
//
//
//  Parameters:
//      LPDBNODE lpdn
//      HFILTER hFilter
//      
//
//  Return: (LPDBNODE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/24/92   MSM        Created
//
//*************************************************************

LPDBNODE WINAPI GetFirstChildDBNode( LPDBNODE lpdn, HFILTER hFilter )
{
    switch ( lpdn->wNodeType )
    {
        case NT_DB:
            return (LPDBNODE)GetFirstPublisher( (LPDB)lpdn, hFilter );

        case NT_PUB:   
            return (LPDBNODE)GetFirstFamily( (LPPUB)lpdn, hFilter );

        case NT_FAMILY:
            return (LPDBNODE)GetFirstFace( (LPFAMILY)lpdn, hFilter );

        case NT_FACE:  
            return (LPDBNODE)GetFirstStyle( (LPFACE)lpdn, hFilter );
    }
    return NULL;

} //*** GetFirstChildDBNode


//*************************************************************
//
//  GetNextDBNode
//
//  Purpose:
//      Returns the next node at the DB level
//
//
//  Parameters:
//      LPDBNODE lpdn
//      HFILTER hFilter
//      
//
//  Return: (LPDBNODE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/24/92   MSM        Created
//
//*************************************************************

LPDBNODE WINAPI GetNextDBNode( LPDBNODE lpdn, HFILTER hFilter )
{
    switch ( lpdn->wNodeType )
    {
        case NT_PUB:   
            return (LPDBNODE)GetNextPublisher( (LPPUB)lpdn, hFilter );

        case NT_FAMILY:
            return (LPDBNODE)GetNextFamily( (LPFAMILY)lpdn, hFilter );

        case NT_FACE:  
            return (LPDBNODE)GetNextFace( (LPFACE)lpdn, hFilter );

        case NT_STYLE:
            return (LPDBNODE)GetNextStyle( (LPSTYLE)lpdn, hFilter );
    }
    return NULL;

} //*** GetNextDBNode


//*************************************************************
//
//  GetFirstPurchasedNode
//
//  Purpose:
//      Retrieves the first purchased node in the database
//
//
//  Parameters:
//      LPDB lpdb
//      
//
//  Return: (LPDBNODE WINAPI)
//
//
//  Comments:
//      Walk collections, then walk the actual database
//
//  History:    Date       Author     Comment
//               2/24/92   MSM        Created
//
//*************************************************************

LPDBNODE WINAPI GetFirstPurchasedNode( LPDB lpdb )
{
    return GetNextPurchasedNode( lpdb, NULL );

} //*** GetFirstPurchasedNode


//*************************************************************
//
//  GetNextPurchasedNode
//
//  Purpose:
//      Retrieves the first purchased node in the database
//
//
//  Parameters:
//      LPDB lpdb
//      LPDBNODE lpdn
//      
//
//  Return: (LPDBNODE WINAPI)
//
//
//  Comments:
//      Walk collections, then walk the actual database
//
//  History:    Date       Author     Comment
//               2/24/92   MSM        Created
//
//*************************************************************

LPDBNODE WINAPI GetNextPurchasedNode( LPDB lpdb, LPDBNODE lpDN )
{
    LPPUB    lpPub;
    LPCOLL   lpColl;
    LPDBNODE lpdn;

    if (!lpDN)  //*** Start at the beginning
    {
        lpPub = GetFirstPublisher( lpdb, NULL );
        lpColl = NULL;
        goto coll_srch;
    }

    //*** Do collections first
    if(NODETYPE(lpDN) == NT_COLLECTION)
    {
        lpPub = PARENT(LPPUB,lpDN);
        lpColl = (LPCOLL)lpDN;

coll_srch:
        while (lpPub)
        {
            if (!lpColl)    //*** For initial entry
                lpColl = GetFirstCollection( lpPub );
            else
                lpColl = GetNextCollection( lpColl );

            while (lpColl)
            {
                if (VI_ISCHECKED(lpColl))
                    return (LPDBNODE)lpColl;
                lpColl = GetNextCollection( lpColl );
            }
            lpColl = NULL;  //*** Reset the entry point
            lpPub = GetNextPublisher( lpPub, NULL );
        }
        return GetNextPurchasedNodeRecursive( (LPDBNODE)lpdb );
    }

    //*** Now the rule is, if the current node is checked then no
    //*** node under it can be.

    lpdn = lpDN;

    while (TRUE)
    {
        LPDBNODE lptn, lpnn;

        if (!lpdn)
            return NULL;

        lpnn = GetNextDBNode( lpdn, NULL );

        //*** if there isn't a next node.  Then we climb up a level.
        if (!lpnn)
        {
            lpdn = PARENT( LPDBNODE, lpdn );
            continue;
        }

        if (NODETYPE(lpdn)==NT_DB)
            return NULL;

        //*** check this node
        lptn = GetNextPurchasedNodeRecursive( lpnn );
        if (lptn)
            return lptn;
        lpdn = lpnn;
    }

} //*** GetNextPurchasedNode


//*************************************************************
//
//  GetNextPurchasedNodeRecursive
//
//  Purpose:
//      This is the recursive work horse of the GetNextPurchasedNode
//
//
//  Parameters:
//      LPDBNODE lpdn
//      
//
//  Return: (LPDBNODE WINAPI)
//
//
//  Comments:
//      Assumtions coming in:
//      lpdn - is an unchecked node and we need to check all children
//             and then go onto the next node.  If no next node then
//             we climb until we get back to LPDB.
//
//  History:    Date       Author     Comment
//               3/30/92   MSM        Created
//
//*************************************************************

LPDBNODE NEAR PASCAL GetNextPurchasedNodeRecursive( LPDBNODE lpdn )
{
    LPDBNODE lpdbn;

    if (!lpdn)
        return NULL;

    //*** If checked then just return it
    if (VI_ISCHECKED(lpdn))
        return lpdn;

    //*** Otherwise attempt to go down a level
    lpdbn = GetFirstChildDBNode( lpdn, NULL );
    if (lpdbn)
    {
        lpdbn = GetNextPurchasedNodeRecursive( lpdbn );
        if (lpdbn)
            return lpdbn;
    }

    //*** If going down fails then try to go over
    lpdbn = GetNextDBNode( lpdn, NULL );
    if (lpdbn)
    {
        lpdbn = GetNextPurchasedNodeRecursive( lpdbn );
        if (lpdbn)
            return lpdbn;
    }
    return NULL;

} //*** GetNextPurchasedNodeRecursive

//*************************************************************
//  SortList
//
//  Purpose:
//      Sorts all the nodes in a list based on the index.  A (-)index 
//      indicates a LPSTR offset, a positive one indicates a char[];    
//      Just does a hardcore BUBBLE sort
//
//  Parameters:
//      HLIST hlst
//      int nIndex
//      
//  Return: (VOID NEAR PASCAL)
//
//  History:    Date       Author     Comment
//               7/18/92   MSM        Created
//
//*************************************************************

VOID WINAPI SortList(HLIST hlst, int nIndex)
{
    LPNODE lp1, lp2;
    LPSTR  lpS1, lpS2;

    lp1 = fsmmListFirstNode(hlst);

    while (lp1)
    {
        lpS1 = (LPSTR)lp1 + abs(nIndex);
        if (nIndex<0)
            lpS1 = *((LPSTR FAR *)lpS1);

        lp2 = fsmmListNextNode(hlst, lp1);

        while (lp2)
        {
            lpS2 = (LPSTR)lp2 + abs(nIndex);
            if (nIndex<0)
                lpS2 = *((LPSTR FAR *)lpS2);

            if (_fstricmp(lpS1,lpS2)>0)
            {
                LPSTR lpT;

                fsmmListSwapNodes(hlst,lp1,lp2);
                lpT=lp1; lp1=lp2; lp2=lpT;
                lpT=lpS1; lpS1=lpS2; lpS2=lpT;
            }

            lp2 = fsmmListNextNode(hlst,lp2);
        }
        lp1 = fsmmListNextNode(hlst,lp1);
    }

} //*** SortList

//*************************************************************
//  SortDatabase
//
//  Purpose:
//      Sorts the database
//
//  Parameters:
//      LPDB lpdb
//      
//  Return: (VOID WINAPI)
//
//  History:    Date       Author     Comment
//               7/18/92   MSM        Created
//
//*************************************************************

VOID WINAPI SortDatabase(LPDB lpDB)
{
    LPPUB lpPub;

    //*** Sort publishers
    SortList(lpDB->hlstList, -1 * OFFSETIN(PUB,lpszPublisher));

    lpPub = GetFirstPublisher( lpDB, NULL );

    while (lpPub)
    {
        LPFAMILY lpFam = GetFirstFamily( lpPub, NULL );

        while (lpFam)
        {
            LPFACE lpFace;
            
            //*** Sort Faces
            SortList(lpFam->hlstList,OFFSETIN(FACE,Info.szName[0]));

            lpFace = GetFirstFace( lpFam, NULL );

            while (lpFace)
            {
                LPSTYLE lpS;
                WORD    wIndex=0;

                //*** Sort Styles
                SortList(lpFace->hlstList,OFFSETIN(STYLE,Info.szName[0]));

                lpS = GetFirstStyle( lpFace, NULL );

                while (lpS)
                {
                    lpS->wBitmapIndex = wIndex;
                    wIndex++;

                    lpS = GetNextStyle( lpS, NULL );
                }

                lpFace = GetNextFace( lpFace, NULL );
            }
            lpFam = GetNextFamily( lpFam, NULL );
        }
        lpPub = GetNextPublisher( lpPub, NULL );
    }

} //*** SortDatabase

//*** EOF: dbwalk.c
