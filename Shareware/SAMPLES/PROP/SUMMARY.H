/*
 * SUMMARY.H
 *
 * Definitions and function prototypes for the Summary Information
 * support functions.
 *
 * Copyright (c)1993 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _SUMMARY_H_
#define _SUMMARY_H_


#include <windows.h>
#include <ole2.h>

#ifdef INITCLSID
#include <initguid.h>
#endif


DEFINE_GUID(FormatID_SummaryInformation, 0xF29F85E0, 0x4FF9, 0x1068, \
    0x91, 0xAB, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9)


#define SZSUMMARYINFOSTREAM "\005SummaryInformation"


#define PT_EMPTY            0
#define PT_NULL             1
#define PT_I2               2
#define PT_I4               3
#define PT_R4               4
#define PT_R8               5
#define PT_CURRENCY         6
#define PT_APPTIME          7
#define PT_BSTR             8
#define PT_BOOL             11
#define PT_VALUE            12
#define PT_SYSTIME          256
#define PT_LPSTR            257
#define PT_BLOB             258
#define PT_STREAM           259
#define PT_STORAGE          260
#define PT_STREAMED_OBJECT  261
#define PT_STORED_OBJECT    262
#define PT_UNICODE          263
#define PT_CF               264
#define PT_ARRAY            0x1000



typedef struct tagPROPERTYHEADER
    {
    WORD        wByteOrder;         //Always 0xFFFE
    WORD        wFormat;            //Always 0
    DWORD       dwOSVer;            //System version
    CLSID       clsID;              //Application CLSID
    DWORD       cSections;
    } PROPERTYHEADER, FAR * LPPROPERTYHEADER;


typedef struct tagFORMATIDOFFSET
    {
    GUID        formatID;
    DWORD       dwOffset;
    } FORMATIDOFFSET, FAR * LPFORMATIDOFFSET;

typedef struct tagSECTIONHEADER
    {
    DWORD       cbSection;
    DWORD       cProperties;
    } SECTIONHEADER, FAR * LPSECTIONHEADER;


typedef struct tagPROPERTYIDOFFSET
    {
    DWORD       propertyID;
    DWORD       dwOffset;
    } PROPERTYIDOFFSET, FAR * LPPROPERTYIDOFFSET;


typedef struct tagPROPERTY
    {
    DWORD       dwType;
    BYTE        rgbValue[1];
    } PROPERTY, FAR * LPPROPERTY;



#define CCHSUMMINFOMAX      256
#define CCHCOMMENTS         1024



/*
 * Structure describing summary information.
 *
 * When saving, if any string contains a 0 as the first byte, that
 * string is omitted from the property set.  If any time in a
 * ULARGE_INTEGER contains -1 then that property is omitted.  If any
 * DWORD or the UINT clipboard format contain 0 they are omitted.
 *
 * When loading, omitted strings will contain a 0 in the first character,
 * omitted times contain -1, and omitted counts or clipboard formats
 * contain 0.
 */


typedef struct tagSUMMARYINFO
    {
    char            szTitle[CCHSUMMINFOMAX];
    char            szSubject[CCHSUMMINFOMAX];
    char            szAuthor[CCHSUMMINFOMAX];
    char            szKeywords[CCHSUMMINFOMAX];
    char            szComments[CCHCOMMENTS];
    char            szTemplate[CCHSUMMINFOMAX];
    char            szLastAuthor[CCHSUMMINFOMAX];
    char            szRevision[CCHSUMMINFOMAX];
    char            szApplication[CCHSUMMINFOMAX];
    ULARGE_INTEGER  ulTimeEdit;
    ULARGE_INTEGER  ulTimeLastPrinted;
    ULARGE_INTEGER  ulTimeCreated;
    ULARGE_INTEGER  ulTimeLastSave;
    DWORD           cPages;
    DWORD           cWords;
    DWORD           cCharacters;
    UINT            cf;
    STGMEDIUM       stm;
    } SUMMARYINFO, FAR * LPSUMMARYINFO;



#define PID_TITLE               0x00000002
#define PID_SUBJECT             0x00000003
#define PID_AUTHOR              0x00000004
#define PID_KEYWORDS            0x00000005
#define PID_COMMENTS            0x00000006
#define PID_TEMPLATE            0x00000007
#define PID_LASTAUTHOR          0x00000008
#define PID_REVNUMBER           0x00000009
#define PID_EDITTIME            0x0000000A
#define PID_LASTPRINTED         0x0000000B
#define PID_CREATE_DTM          0x0000000C
#define PID_LASTSAVE_DTM        0x0000000D
#define PID_PAGECOUNT           0x0000000E
#define PID_WORDCOUNT           0x0000000F
#define PID_CHARCOUNT           0x00000010
#define PID_THUMBNAIL           0x00000011
#define PID_APPNAME             0x00000012


/*
 * Functions to just serialize the structure, load it, or prompt for
 * changes to the Title, Subject, Author, Keywords, and Comments fields.
 */
BOOL FSummaryInfoSave(CLSID, LPSTORAGE, LPSUMMARYINFO);
BOOL FSummaryInfoLoad(LPSTORAGE, LPSUMMARYINFO);
BOOL FSummaryInfoPrompt(CLSID, LPSTORAGE, LPSUMMARYINFO, BOOL);



#endif //_SUMMARY_H_
