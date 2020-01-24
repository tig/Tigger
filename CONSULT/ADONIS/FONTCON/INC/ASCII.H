/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  ASCII.h
 *
 ***********************************************************************/


#ifndef _ASCII_H_
#define _ASCII_H_


// ascii definitions

#define ASCII_NUL     0x00
#define ASCII_SOH     0x01
#define ASCII_STX     0x02
#define ASCII_ETX     0x03
#define ASCII_EOT     0x04
#define ASCII_ENQ     0x05
#define ASCII_ACK     0x06
#define ASCII_BEL     0x07
#define ASCII_BS      0x08
#define ASCII_HT      0x09
#define ASCII_TAB     ASCII_HT
#define ASCII_LF      0x0A
#define ASCII_VT      0x0B
#define ASCII_FF      0x0C
#define ASCII_CR      0x0D
#define ASCII_SO      0x0E
#define ASCII_SI      0x0F
#define ASCII_DLE     0x10
#define ASCII_DC1     0x11
#define ASCII_XON     0x11
#define ASCII_DC2     0x12
#define ASCII_TON     0x12
#define ASCII_DC3     0x13
#define ASCII_XOFF    0x13
#define ASCII_DC4     0x14
#define ASCII_TOFF    0x14
#define ASCII_NAK     0x15
#define ASCII_SYN     0x16
#define ASCII_ETB     0x17
#define ASCII_CAN     0x18
#define ASCII_EM      0x19
#define ASCII_SUB     0x1A
#define ASCII_ESC     0x1B
#define ASCII_FS      0x1C
#define ASCII_GS      0x1D
#define ASCII_RS      0x1E
#define ASCII_US      0x1F
#define ASCII_MINUS   0x2D
#define ASCII_EQUAL   0x3D
#define ASCII_DEL     0x7F
#define ASCII_SS2     0x8E
#define ASCII_SS3     0x8F
#define ASCII_CSI     0x9B

#define DECANM        0x02
#define DECCOLM       0x03
#define DECSCLM       0x04  // not supported (so says bryan)
#define DECSCNM       0x05
#define DECOM         0x06
#define DECAWM        0x07

#endif
/************************************************************************
 * End of File: ASCII.h
 ***********************************************************************/

