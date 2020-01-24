/************************************************************************
 *
 *     Project:  
 *
 *      Module:  hexedit.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#ifndef _HEXEDIT_H_
#define _HEXEDIT_H_

/*
 * Window Styles
 */
#define HS_DISABLETEXT     0x0001L
#define HS_16BIT           0x0002L
#define HS_32BIT           0x0004L
#define HS_BIGENDIAN       0x0008L
#define HS_DISABLEINDEX    0x0010L
#define HS_SEGOFFSET       0x0020L
#define HS_READONLY        0x0040L

/*
 * Control messages
 */
#define HM_FIRST_CONTROLMSG   (WM_USER + 0x1000)

#define HM_SETINDEXWIDTH      (HM_FIRST_CONTROLMSG + 0x00)
#define HM_GETINDEXWIDTH      (HM_FIRST_CONTROLMSG + 0x01)

#define HM_SETSEGMENT         (HM_FIRST_CONTROLMSG + 0x02)
#define HM_GETSEGMENT         (HM_FIRST_CONTROLMSG + 0x03)

#define HM_SETOFFSET          (HM_FIRST_CONTROLMSG + 0x04)
#define HM_GETOFFSET          (HM_FIRST_CONTROLMSG + 0x05)

#define HM_SETCOLUMNSPACE     (HM_FIRST_CONTROLMSG + 0x06)
#define HM_GETCOLUMNSPACE     (HM_FIRST_CONTROLMSG + 0x07)

#define HM_SETSEPCHAR         (HM_FIRST_CONTROLMSG + 0x08)
#define HM_GETSEPCHAR         (HM_FIRST_CONTROLMSG + 0x09)

#define HM_SETCOLUMNGROUP     (HM_FIRST_CONTROLMSG + 0x0A)
#define HM_GETCOLUMNGROUP     (HM_FIRST_CONTROLMSG + 0x0B)

#define HM_SETDATA            (HM_FIRST_CONTROLMSG + 0x0C)
#define HM_GETDATA            (HM_FIRST_CONTROLMSG + 0x0D)

#define HM_SETINDEX           (HM_FIRST_CONTROLMSG + 0x0E)
#define HM_GETINDEX           (HM_FIRST_CONTROLMSG + 0x0F)

#define HM_SETSEL             (HM_FIRST_CONTROLMSG + 0x10)
#define HM_GETSEL             (HM_FIRST_CONTROLMSG + 0x11)

#define HM_INVALIDATERANGE    (HM_FIRST_CONTROLMSG + 0x12)

#define HM_LAST_CONTROLMSG       HM_INVALIDATERANGE

/*
 * Control notification messages.
 * These should correspond roughly to the edit control
 * notifications.
 */



#endif

/************************************************************************
 * End of File: hexedit.h
 ***********************************************************************/

