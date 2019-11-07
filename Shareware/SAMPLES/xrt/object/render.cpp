/*
 * RENDER.CPP
 * Data Object for Chapter 6
 *
 * CDataObject::Render* functions to create text, bitmaps, and metafiles
 * in a variety of sizes.
 *
 * Copyright (c)1993 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "dataobj.h"
#include <string.h>

/*
 * CDataObject::RenderText
 *
 * Purpose:
 *  Creates a global memory block containing the letter 'k' of sizes
 *  of 64 bytes, 1024 bytes, and 16384 bytes, into a caller-supplied
 *  STGMEDIUM.
 *
 * Parameters:
 *  pSTM            LPSTGMEDIUM in which to render.
 *
 * Return Value:
 *  HRESULT         Return value for ::GetData
 */

HRESULT CDataObject::RenderText(LPSTGMEDIUM pSTM)
    {
    DWORD       cch = 255 ;
    HGLOBAL     hMem;
    LPSTR       psz;

    //Get the size of data we're dealing with

    hMem=GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, 256);

    if (NULL==hMem)
        return ResultFromScode(STG_E_MEDIUMFULL);

    psz=(LPSTR)GlobalLock(hMem);

    lstrcpy( psz, "MSFT 98.25   IBM 23.5   AAPL 26.75" ) ;

    GlobalUnlock(hMem);

    pSTM->hGlobal=hMem;
    pSTM->tymed=TYMED_HGLOBAL;
    return NOERROR;
    }




