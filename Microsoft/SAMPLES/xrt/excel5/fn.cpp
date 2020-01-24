//fn.cpp
//
// WOSA/XRT DataObject access functions for XL4
//
#include "precomp.h"
#include "framewrk.h"
#include "..\\wosaxrt.h"

#include "clsid.h"
#include "fn.h"
#include "iadvsink.h"
#include "ddesrv.h"

#include "debug.h"
ASSERTDATA

UINT            g_xrtCF_XRTSTOCKS = 0 ;

// TODO:  All of these need to be instanced!
LPDATAOBJECT    g_pDataObject = NULL ;
LPADVISESINK    g_pAdviseSink = NULL ;
DWORD           g_dwAdvConn = 0L ;
DWORD           g_dwUpdateCount = 0L ;

// update list used by XRTQuote
LPXRTSTOCKS     g_lpUpdateList = NULL ;

// Update list used by XRTSetRange
//
// The update list is a two dimensional array of XLOPERs representing
// a table of the items.   It is stored as an xltypeMulti.
// 
XLOPER          g_xUpdateTable ;


// There is a column list (attribute names) and a row list
// (item names.)
//
LPXLOPER        g_pxColumnList ;
LPXLOPER        g_pxRowList ;   

// This is where we keep track of where to do the 'xlSet'.
// This also gives us the size.
//
// TODO:  This should be changed from an XLMREF to an XLOPER
// of type xltypeRef
//
XLMREF          g_xlmrefRange ;        

////////////////////////////////////////////////////////////
UINT    g_timerid = 0 ;

int WINAPI lpstricmp(LPSTR s, LPSTR t);

extern "C"
LPXLOPER _export WINAPI XRTCreateObject( LPXLOPER pxVal )
{
    static XLOPER xResult ;

    xResult.xltype = xltypeErr ;
    xResult.val.err = xlerrNull ;
    
    if (pxVal->xltype != xltypeStr)
    {    
        #ifdef _DEBUG
        OutputDebugString( "Invlaid type in XRTCreateObject\r\n" ) ;
        #endif
        
        xResult.val.err = xlerrValue ;
        return (LPXLOPER) &xResult ;
    }

    char szName[128] ;
    lstrcpyn( szName, pxVal->val.str+1, (UINT)*pxVal->val.str + 1 ) ;
    if (SUCCEEDED( InitXRTObject( szName ) ))
    {
        xResult.xltype = xltypeNum ;
        xResult.val.num = (double)(DWORD)g_pDataObject ;

        XLOPER  xDocText, xMacroText ;
        xDocText.xltype = xltypeStr ;
        xDocText.val.str = " XRTXLL|DATA!ID" ;
        *xDocText.val.str = (unsigned char)lstrlen( xDocText.val.str ) - 1 ;
        
        xMacroText.xltype = xltypeStr ;
        xMacroText.val.str = " XRTOnData" ;
        *xMacroText.val.str = (unsigned char)lstrlen( xMacroText.val.str ) - 1 ;
        
        Excel4( xlcOnData, &xResult, 2, (LPXLOPER)&xDocText, (LPXLOPER)&xMacroText ) ;        
    }
    #ifdef _DEBUG
    else
    {
        OutputDebugString( "InitXRTObject failed\r\n" ) ;
    }
    #endif

    return (LPXLOPER) &xResult ;

}

extern "C"
short int _export WINAPI XRTDestroyObject()
{
    return SUCCEEDED( UnInitXRTObject() ) ;
}

extern "C"
void CALLBACK __export fnTimer( HWND hwnd, UINT uiMsg, UINT idTimer, DWORD dwTime )
{
    g_dwUpdateCount++ ;
    #ifdef _DEBUG
    {
    char sz[128] ;
    wsprintf( sz, "Timer! g_dwUpdateCount = %lu\r\n", (DWORD)g_dwUpdateCount ) ;
    OutputDebugString( sz ) ;
    }
    #endif
    PostDDEServer() ;
}

HRESULT InitXRTObject( LPSTR lpszObjName )
{
    HRESULT hr ;
    LPCLASSFACTORY  pClassFactory ; 
    LPUNKNOWN       pUnknown ;
    g_xrtCF_XRTSTOCKS = RegisterClipboardFormat( "xrtCF_XRTSTOCKS" ) ;

    // Don't let 'em create more than one...
    //
    if (g_pDataObject != NULL)
        return ResultFromScode( S_OK ) ;

    ////////////////////////////////////////////////////////////////////
//    g_timerid = SetTimer( NULL, 0x4242, 1000, (TIMERPROC)fnTimer ) ;                                    
                                    
                                    
//    return ResultFromScode( S_OK ) ;
    ////////////////////////////////////////////////////////////////////

           
    if (FAILED(CoInitialize(NULL)))
    {
        Assert(0) ;
        return hr ;
    }

    // Get CLSID from string                     
    CLSID   clsid ;
    if (FAILED(hr = CLSIDFromString( lpszObjName, &clsid )))
    {
        #ifdef _DEBUG
        char szBuf[256] ; wsprintf( szBuf, "CLISDFromString( %s ) failed!\r\n", (LPSTR)lpszObjName ) ;
        OutputDebugString( szBuf ) ;
        #endif
        
        return hr ;
    }

    hr = CoGetClassObject( clsid, CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory,
                           (LPVOID FAR *)&pClassFactory ) ;
                 
    if (SUCCEEDED(hr))         
    { 
        // Create an instance of the object (Instance) and store
        // it in m_pIUnknown
        //
        hr = pClassFactory->CreateInstance( NULL, IID_IUnknown, 
                            (LPVOID FAR *)&pUnknown ) ;
        if (SUCCEEDED(hr))         
        {
            hr = pUnknown->QueryInterface( IID_IDataObject, (LPVOID FAR*)&g_pDataObject ) ;
            pUnknown->Release() ;

            // Create our advise sink
            //
            g_pAdviseSink = new CImpIAdviseSink ;
            if (g_pAdviseSink != NULL)
            {
                FORMATETC fetc ;
                
                fetc.cfFormat = g_xrtCF_XRTSTOCKS ;
                fetc.dwAspect = DVASPECT_CONTENT ;
                fetc.ptd = NULL ;
                fetc.tymed = TYMED_HGLOBAL ;
                fetc.lindex = - 1 ;
                
                // Setup the advise and make it a hotlink
                //        
                hr = g_pDataObject->DAdvise( &fetc, ADVF_PRIMEFIRST, g_pAdviseSink, &g_dwAdvConn ) ;
                if (FAILED(hr))
                {
                    AssertSz( 0, "DAdvise failed" ) ;
                    g_pAdviseSink = NULL ;
                    g_dwAdvConn = 0L ;
                }
            }
        }                                                                            
        else
        {   
            AssertSz( 0, "Could not get g_pDataObject" ) ;
            g_pDataObject = NULL ;
        }   
        
        pClassFactory->Release() ;
        Assert( g_pAdviseSink != NULL ) ;
    }
    else
    {   
        char sz[128] ;
        wsprintf( sz, "CoGetClassObject Failed: %08lX", (DWORD)hr ) ;
        AssertSz( 0, sz ) ;
    }   

    return hr ;
}

HRESULT UnInitXRTObject()
{
    
    /////////////////////////////////////////////////////////////////////////////
//    KillTimer( NULL, g_timerid ) ;
//    return ResultFromScode( S_OK ) ;
    /////////////////////////////////////////////////////////////////////////////

    if (g_pAdviseSink != NULL)
    {
        if (g_pDataObject != NULL)
        {
            g_pDataObject->DUnadvise( g_dwAdvConn ) ;
        }
        
        g_pAdviseSink->Release() ;
        g_pAdviseSink = NULL ;
    }

    if (g_pDataObject != NULL)
    {
        g_pDataObject->Release() ;
        g_pDataObject = NULL ;
        CoUninitialize() ;      
    }
    
    if (g_lpUpdateList != NULL)
    {
        GlobalFreePtr( g_lpUpdateList ) ;
        g_lpUpdateList = NULL ;
    }                
           
    return ResultFromScode( S_OK ) ;
}

HRESULT GotData( LPFORMATETC lpfetc, LPSTGMEDIUM lpstm )
{
    HRESULT hr = ResultFromScode( S_OK ) ;
    
    if (lpfetc->cfFormat == g_xrtCF_XRTSTOCKS && lpstm->tymed & TYMED_HGLOBAL)
    {     
        LPXRTSTOCKS lpUpdate = (LPXRTSTOCKS)GlobalLock( lpstm->hGlobal ) ;
        LPXRTSTOCK  lpQ ; 
        
        if (lpUpdate == NULL)
        {
            AssertSz( 0, "lpstm->hGlobal null" ) ;
            return ResultFromScode( E_FAIL ) ;
        }
        
        if (g_lpUpdateList == NULL)
        {
            UINT n = sizeof(XRTSTOCKS) + (lpUpdate->cStocks * sizeof(XRTSTOCK)) ;
            g_lpUpdateList = (LPXRTSTOCKS)GlobalAllocPtr( GHND, n ) ;
            _fmemcpy( g_lpUpdateList, lpUpdate, n ) ;
        }
        else
        {                                   
            // Some items are to be updated.
            //
            for ( UINT n = 0 ; n < lpUpdate->cStocks ; n ++ )
            {
                lpQ = &lpUpdate->rgStocks[0] + n ;
                if (lpQ->uiMembers)
                {
                    // Copy the entire XRTSTOCK structure.  Copying only the members that
                    // changed would be slower (many compares).   The CF_MARKETDATA clipboard
                    // format that the XRT spec eventually will define will make this
                    // unneccessary.
                    //
                    if (lpQ->uiIndex < g_lpUpdateList->cStocks)
                        _fmemcpy( &g_lpUpdateList->rgStocks[0] + lpQ->uiIndex, lpQ, sizeof( XRTSTOCK ) ) ;
                    else
                    {
                        OutputDebugString( "Yikes!  uiIndex >= cStocks!\r\n" ) ;
                    }
                }            
            }
        }

        g_dwUpdateCount++ ;
        
        #ifdef _DEBUG
        {
        char sz[128] ;
        wsprintf( sz, "GotData, g_dwUpdateCount = %lu\r\n", (DWORD)g_dwUpdateCount ) ;
        OutputDebugString( sz ) ;
        }
        #endif
            
        PostDDEServer() ;
    }

    ReleaseStgMedium( lpstm );
    
    return hr ;
}

LPXLOPER GetQuoteInfo( int n, LPXLOPER pxVal, LPXRTSTOCKS lpQuote ) ;

extern "C"
void __export FAR PASCAL XRTOnData( )
{                                       
    #ifdef _DEBUG
    OutputDebugString("XRTOnData\r\n") ;
    #endif 

/*
    if (g_xUpdateTable.xltype == xltypeMulti && g_xUpdateTable.val.array.lparray != NULL)
    {
        XLOPER xRef ;

        if (xlretSuccess != Excel4(xlSheetId, &xRef, 0))
            return ;

        xRef.xltype = xltypeRef ;
        xRef.val.mref.lpmref = (LPXLMREF)&g_xlmrefRange ;
    
        Excel4( xlSet, 0, 2, (LPXLOPER)&xRef, (LPXLOPER)&g_xUpdateTable ) ;
    }    
*/
    
}

extern "C"
LPXLOPER __export FAR PASCAL XRTSetRange( LPXLOPER pxVal )
{
    static XLOPER xResult ;
    xResult.xltype = xltypeErr ;
    xResult.val.err = xlerrValue ;
/*
    // pxVal has to specify a range
    //
    if (pxVal->xltype != xltypeRef)
    {
        #ifdef _DEBUG
        char sz[128] ; wsprintf( sz, "XRTSetRange:  pxVal->xltype not xltypeSRef (%#04x)\r\n", pxVal->xltype ) ;
        OutputDebugString( sz ) ;
        #endif ;

        XLOPER xDestType ;
        static XLOPER x ;
        
        xDestType.xltype = xltypeInt ;
        xDestType.val.w = xltypeRef ;
                
        if (xlretSuccess != Excel4( xlCoerce, &x, 2, (LPXLOPER)pxVal, (LPXLOPER)&xDestType ))
        {
            #ifdef _DEBUG
            OutputDebugString( "xlCoerce failed\r\n" ) ;
            #endif
            return (LPXLOPER)&xResult ;
        }
        
        pxVal = &x ;
        
        #ifdef _DEBUG
        wsprintf( sz, "Coerced!\r\n") ;
        OutputDebugString( sz) ;
        #endif 
    }

    if (pxVal->xltype != xltypeRef)
    {
        #ifdef _DEBUG
        char sz[128] ; wsprintf( sz, "XRTSetRange:  pxVal->xltype STILL not xltypeSRef (%#04x)\r\n", pxVal->xltype ) ;
        OutputDebugString( sz ) ;
        #endif ;
        return (LPXLOPER)&xResult ;
    }
                                   
    if (g_xUpdateTable.val.array.lparray != NULL)
    {
        GlobalFreePtr( g_xUpdateTable.val.array.lparray ) ;
        g_xUpdateTable.val.array.lparray = NULL ;
    }

    if (g_pxRowList != NULL)
    {
        GlobalFreePtr( g_pxRowList ) ;
        g_pxRowList = NULL ;
    }
    
    if (g_pxColumnList != NULL)
    {
        GlobalFreePtr( g_pxColumnList ) ;
        g_pxColumnList = NULL ;
    }

    g_xlmrefRange.count = 1 ;
    g_xlmrefRange = *pxVal->val.mref.lpmref ;

    LPXLOPER    p ;
    UINT        rows, cols ;
    UINT        nSize ;

    // Allocate the update table so that it's big enuf for the cells
    // that are within the row/column headings.
    //
    rows = g_xlmrefRange.reftbl[0].rwLast - g_xlmrefRange.reftbl[0].rwFirst ;
    cols = g_xlmrefRange.reftbl[0].colLast - g_xlmrefRange.reftbl[0].colFirst ;
    if (rows < 1 || cols < 1)
    {
        return (LPXLOPER)&xResult ;
    }

    nSize = sizeof( XLOPER ) * rows * cols ;
    
    if (!(p = (LPXLOPER)GlobalAllocPtr( GHND, nSize )))
    {
        #ifdef _DEBUG
        OutputDebugString( "GlobalAlloc failed!\r\n" ) ;
        #endif
        return (LPXLOPER)&xResult ;
    }

    // Row 0 of the range identifies the attributes requested
    // Column 0 identifies the items
    // Cell R0C0 is ignored
    if (g_pColumnList = GlobalAllocPtr( GHND, cols * sizeof( XLOPER ) ))
    {
        // Go down rwFirst from colFirst+1 to colLast and figure out which column
        // contains which attribute.
        //
        
        
    }
        
    
        FillArray( p, rows, cols ) ;
        
        xResult.xltype = xltypeBool ;
        xResult.val.bool = TRUE ;
        
        g_xUpdateTable.xltype = xltypeMulti ;
        g_xUpdateTable.val.array.lparray = p ;
        g_xUpdateTable.val.array.rows = rows ;                    
        g_xUpdateTable.val.array.columns = cols ;                    
    }
*/                               
    return (LPXLOPER)&xResult ;    
}

/*
BOOL FillArray( LPXLOPER p, UINT rows, UINT cols )
{
    for (UINT j = 0 ; j < rows ; j++)
    {
        for (UINT i = 0 ; i < cols ; i++)
        {
            (p + i + (j * cols))->xltype = xltypeNum ;
            (p + i + (j * cols))->val.num =  ;
        }
    }
}
*/

extern "C"
LPXLOPER __export FAR PASCAL XRTQuote( LPXLOPER pxTicker, LPXLOPER pxVal )
{
    static XLOPER xResult; 
    static char szBuf[256] ;

//    AssertSz( g_pDataObject, "g_pDataObject is NULL in XRTQuote!" ) ;
    
    if (g_pDataObject == NULL)
    {
        xResult.xltype = xltypeErr ;
        xResult.val.err = xlerrValue ;
        return (LPXLOPER) &xResult ;       
    }

    // The val has to be of type string    
    if (pxVal->xltype != xltypeStr)
    {    
        static XLOPER xStr ;
        XLOPER xDestType ;
        
        xDestType.xltype = xltypeInt ;
        xDestType.val.w = xltypeStr ;
                
        Excel4( xlCoerce, &xStr, 2, (LPXLOPER)pxVal, (LPXLOPER)&xDestType ) ;
        pxVal = &xStr ;                
    }
  
    xResult.xltype = xltypeErr ;  
    xResult.val.err = xlerrNum ;
   
    int n ;
    BOOL fFound = FALSE ;

    switch (pxTicker->xltype)
    {
        case xltypeStr:
        {
            for (n = 0 ; n < (int)g_lpUpdateList->cStocks ; n++)                        
            {
                wsprintf( szBuf, "%c%s", lstrlen( g_lpUpdateList->rgStocks[n].szSymbol ), 
                            (LPSTR)g_lpUpdateList->rgStocks[n].szSymbol ) ;                                
                if (lpstricmp( pxTicker->val.str, szBuf ) == 0)
                {
                    xResult = *GetQuoteInfo( n, pxVal, g_lpUpdateList ) ;
                    break ;
                }    
            }
        }       
        break ;
            
        case xltypeInt:
        {
            if (pxTicker->val.w < (int)g_lpUpdateList->cStocks)
                xResult = *GetQuoteInfo( pxTicker->val.w, pxVal, g_lpUpdateList ) ;
        }
        break ;
            
        case xltypeNum:
        default:
        {
            XLOPER xInt, xDestType ;
            xDestType.xltype = xltypeInt ;
            xDestType.val.w = xltypeInt ;
                
            Excel4( xlCoerce, &xInt, 2, (LPXLOPER)pxTicker, (LPXLOPER)&xDestType ) ;
            if (xInt.val.w < (int)g_lpUpdateList->cStocks)
                xResult = *GetQuoteInfo( xInt.val.w, pxVal, g_lpUpdateList ) ;
        }
        break ;
    }
    
    return (LPXLOPER) &xResult;
}


LPXLOPER GetQuoteInfo( int n, LPXLOPER pxVal, LPXRTSTOCKS lpStocks )
{       
    static XLOPER  xResult ;
    
    double d = -1;
    if (lpstricmp( pxVal->val.str, "\004Last" ) == 0)
        d = (double)lpStocks->rgStocks[n].ulLast / (double)1000 ;
    else if (lpstricmp( pxVal->val.str, "\006Volume" ) == 0)
        d = (double)lpStocks->rgStocks[n].ulVolume ;
    else if (lpstricmp( pxVal->val.str, "\004High" ) == 0)
        d = (double)lpStocks->rgStocks[n].ulHigh / (double)1000 ;
    else if (lpstricmp( pxVal->val.str, "\003Low" ) == 0)
        d = (double)lpStocks->rgStocks[n].ulLow / (double)1000 ;
    else if (lpstricmp( pxVal->val.str, "\006Symbol" ) == 0)
    {
        static char szBuf[32] ;
        xResult.xltype = xltypeStr ;
        wsprintf( szBuf, "%c%s", lstrlen( lpStocks->rgStocks[n].szSymbol ), 
                (LPSTR)lpStocks->rgStocks[n].szSymbol ) ;                                
        xResult.val.str = szBuf ;
        return (LPXLOPER)&xResult ;
    }    

    if (d == -1)                                
    {
        xResult.xltype = xltypeErr ;
        xResult.val.err = xlerrNull ;
    }
    else
    {
        xResult.xltype = xltypeNum ;
        xResult.val.num = d ;
    }
    
    return (LPXLOPER)&xResult ;
}


extern "C"
LPXLOPER __export FAR PASCAL XRTItemName( LPXLOPER pxTicker )
{
    static XLOPER xResult; 
    static char szBuf[256] ;

    if (g_pDataObject == NULL)
    {
        xResult.xltype = xltypeErr ;
        xResult.val.err = xlerrValue ;
        return (LPXLOPER) &xResult ;       
    }

    switch (pxTicker->xltype)
    {
        case xltypeStr:
        {
            return pxTicker ;
        }
        break ;
            
        case xltypeInt:
        {
            if (pxTicker->val.w < (int)g_lpUpdateList->cStocks)
            {                            
                wsprintf( szBuf, " %s", (LPSTR)g_lpUpdateList->rgStocks[pxTicker->val.w].szSymbol ) ;
                *szBuf = (unsigned char)lstrlen( szBuf )-1 ;
                xResult.xltype = xltypeStr ;
                xResult.val.str = szBuf ; 
            }
        }
        break ;
            
        case xltypeNum:
        default:
        {
            XLOPER xInt, xDestType ;
            xDestType.xltype = xltypeInt ;
            xDestType.val.w = xltypeInt ;
                
            Excel4( xlCoerce, &xInt, 2, (LPXLOPER)pxTicker, (LPXLOPER)&xDestType ) ;
            if (xInt.val.w < (int)g_lpUpdateList->cStocks)
            {                            
                wsprintf( szBuf, " %s", (LPSTR)g_lpUpdateList->rgStocks[xInt.val.w].szSymbol ) ;
                *szBuf = (unsigned char)lstrlen( szBuf )-1 ;
                xResult.xltype = xltypeStr ;
                xResult.val.str = szBuf ; 
            }
        }
        break ;
    }
    

    return (LPXLOPER)&xResult ;
}
 
extern "C" 
LPXLOPER __export FAR PASCAL XRTInfo( LPXLOPER x )
{                   
    static XLOPER  xlResult ;
    if (lpstricmp( x->val.str, "\005COUNT" ) == 0)
    {
        xResult.xltype = xltypeInt ;
//        xResult.val.w = lpStocks.cStocks ;
    }
    
    return (LPXLOPER)&xResult ;
}