/*** 
*idata.cpp
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  This file contains the INTERFACEDATA definitions for the methods
*  and properties on the CFileMgr object that are exposed for external
*  programmability via IDispatch.
*
*
*Implementation Notes:
*
*****************************************************************************/

#include "stdafx.h"

#include <ole2.h>
#if !defined(WIN32)
#include <olenls.h>
#endif
#include <dispatch.h>

#include "dispauto.h"


//---------------------------------------------------------------------
//                   INTERFACEDATA definitions 
//---------------------------------------------------------------------

/* The INTERFACEDATA table describes the methods and properties that
 * are being exposed for external programmability via IDispatch.
 * This table is used to construct a CDispTypeInfo for this interface,
 * and that TypeInfo is in turn used to initialize and drive the
 * default implementation of IDispatch.
 */


// PDATA1() declares the PARAMDATA for a methods that takes a single param
//
#define PDATA1(NAME, TYPE) \
    static PARAMDATA NEAR rgpdata ## NAME = {#NAME, TYPE}

// MDATA() declares a single METHODDATA entry
//
#define MDATA(NAME, PDATA, IDMEMBER, IMETH, CARGS, KIND, TYPE) \
    { #NAME, PDATA, IDMEMBER, IMETH, CC_CDECL, CARGS, KIND, TYPE }

// The following macro defines the METHODDATA entries for a 
// property Put/Get method pair.
//
// Note: this macro *assumes* that the Put/Get pair are adjacent
// in the vtable, and that the Put method comes first.
//
#define PROPERTY(NAME, IMETH, ID, TYPE) \
    MDATA(NAME, &rgpdata ## NAME, ID, IMETH, 1,DISPATCH_PROPERTYPUT,VT_EMPTY), \
    MDATA(NAME, NULL, ID, IMETH+1, 0, DISPATCH_PROPERTYGET, TYPE)

// The following macro is used to define a METHODDATA entry for
// a method that takes zero parameters.
//
#define METHOD0(NAME, IMETH, ID, TYPE) \
    MDATA(NAME, NULL, ID, IMETH, 0, DISPATCH_METHOD, TYPE)

// and for one param
#define METHOD1(NAME, IMETH, ID, TYPE) \
    MDATA(NAME, &rgpdata ## NAME, ID, IMETH, 1, DISPATCH_METHOD, TYPE)


PDATA1(TEXT,  VT_BSTR);

static METHODDATA NEAR rgmdataCFileMgr[] = 
{  
       PROPERTY(TEXT,   IMETH_TEXT,     IDMEMBER_TEXT,     VT_BSTR) 
    
    ,  METHOD0(CLEAR,   IMETH_CLEAR,    IDMEMBER_CLEAR,    VT_EMPTY)
    ,  METHOD0(ABOUT,   IMETH_ABOUT,    IDMEMBER_ABOUT,    VT_EMPTY)
    ,  METHOD0(DISPLAY, IMETH_DISPLAY,  IDMEMBER_DISPLAY,  VT_EMPTY)
    ,  METHOD0(QUIT,    IMETH_QUIT,     IDMEMBER_QUIT,     VT_EMPTY)
};

INTERFACEDATA NEAR g_idataCFileMgr = 
{
    rgmdataCFileMgr, DIM(rgmdataCFileMgr)
};
