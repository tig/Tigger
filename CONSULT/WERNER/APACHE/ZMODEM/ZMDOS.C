The reliability of ZMODEM file transfers has been enhanced with the
addition of a table driven 32 bit CRC.  ZMODEM downloads from Unix to
PC's are much more efficient than XMODEM or Kermit.  For example, 9600
baud downloads from a 9 mHz PC-AT Xenix machine to Crosstalk XVI gave
343 characters per second throughput, Kermit gave 327 characters per
second, and ZMODEM gives 915 characters per second.  At 19200 baud,
ZMODEM downloads to a 4.77 mHz PC hard disk attain 1870 characters per
second throughput.  Note that the 343 cps throughput with Crosstalk was
with an 8 bit checksum while ZMODEM uses 32 bit CRC for orders of
magnitude better data reliability. 

/*
 *   Z M O D E M . H     Manifest constants for ZMODEM
 *    application to application file transfer protocol
 *    01-15-87  Chuck Forsberg Omen Technology Inc
 */
#define ZPAD '*'        /* 052 Padding character begins frames */
#define ZDLE 030        /* Ctrl-X Zmodem escape - `ala BISYNC DLE */
#define ZDLEE (ZDLE^0100)       /* Escaped ZDLE as transmitted */
#define ZBIN 'A'        /* Binary frame indicator */
#define ZHEX 'B'        /* HEX frame indicator */
#define ZBIN32 'C'      /* Binary frame with 32 bit FCS */

/* Frame types (see array "frametypes" in zm.c) */
#define ZRQINIT 0       /* Request receive init */
#define ZRINIT  1       /* Receive init */
#define ZSINIT 2        /* Send init sequence (optional) */
#define ZACK 3          /* ACK to above */
#define ZFILE 4         /* File name from sender */
#define ZSKIP 5         /* To sender: skip this file */
#define ZNAK 6          /* Last packet was garbled */
#define ZABORT 7        /* Abort batch transfers */
#define ZFIN 8          /* Finish session */
#define ZRPOS 9         /* Resume data trans at this position */
#define ZDATA 10        /* Data packet(s) follow */
#define ZEOF 11         /* End of file */
#define ZFERR 12        /* Fatal Read or Write error Detected */
#define ZCRC 13         /* Request for file CRC and response */
#define ZCHALLENGE 14   /* Receiver's Challenge */
#define ZCOMPL 15       /* Request is complete */
#define ZCAN 16         /* Other end canned session with CAN*5 */
#define ZFREECNT 17     /* Request for free bytes on filesystem */
#define ZCOMMAND 18     /* Command from sending program */
#define ZSTDERR 19      /* Output to standard error, data follows */

/* ZDLE sequences */
#define ZCRCE 'h'       /* CRC next, frame ends, header packet follows */
#define ZCRCG 'i'       /* CRC next, frame continues nonstop */
#define ZCRCQ 'j'       /* CRC next, frame continues, ZACK expected */
#define ZCRCW 'k'       /* CRC next, ZACK expected, end of frame */
#define ZRUB0 'l'       /* Translate to rubout 0177 */
#define ZRUB1 'm'       /* Translate to rubout 0377 */

/* zdlread return values (internal) */
/* -1 is general error, -2 is timeout */
#define GOTOR 0400
#define GOTCRCE (ZCRCE|GOTOR)   /* ZDLE-ZCRCE received */
#define GOTCRCG (ZCRCG|GOTOR)   /* ZDLE-ZCRCG received */
#define GOTCRCQ (ZCRCQ|GOTOR)   /* ZDLE-ZCRCQ received */
#define GOTCRCW (ZCRCW|GOTOR)   /* ZDLE-ZCRCW received */
#define GOTCAN  (GOTOR|030)     /* CAN*5 seen */

/* Byte positions within header array */
#define ZF0     3       /* First flags byte */
#define ZF1     2
#define ZF2     1
#define ZF3     0
#define ZP0     0       /* Low order 8 bits of position */
#define ZP1     1
#define ZP2     2
#define ZP3     3       /* High order 8 bits of file position */

/* Bit Masks for ZRINIT flags byte ZF0 */
#define CANFDX  01      /* Rx can send and receive true FDX */
#define CANOVIO 02      /* Rx can receive data during disk I/O */
#define CANBRK  04      /* Rx can send a break signal */
#define CANCRY  010     /* Receiver can decrypt */
#define CANLZW  020     /* Receiver can uncompress */
#define CANFC32 040     /* Receiver can use 32 bit Frame Check */

/* Parameters for ZSINIT frame */
#define ZATTNLEN 32     /* Max length of attention string */

/* Parameters for ZFILE frame */
/* Conversion options one of these in ZF0 */
#define ZCBIN   1       /* Binary transfer - inhibit conversion */
#define ZCNL    2       /* Convert NL to local end of line convention */
#define ZCRESUM 3       /* Resume interrupted file transfer */
/* Management options, one of these in ZF1 */
#define ZMNEW   1       /* Transfer if source newer or longer */
#define ZMCRC   2       /* Transfer if different file CRC or length */
#define ZMAPND  3       /* Append contents to existing file (if any) */
#define ZMCLOB  4       /* Replace existing file */
#define ZMSPARS 5       /* Encoding for sparse file */
#define ZMDIFF  6       /* Transfer if dates or lengths different */
#define ZMPROT  7       /* Protect destination file */
/* Transport options, one of these in ZF2 */
#define ZTLZW   1       /* Lempel-Ziv compression */
#define ZTCRYPT 2       /* Encryption */
#define ZTRLE   3       /* Run Length encoding */

/* Parameters for ZCOMMAND frame ZF0 (otherwise 0) */
#define ZCACK1  1       /* Acknowledge, then do command */

long rclhdr();

/* Globals used by ZMODEM functions */
int Rxframeind;         /* ZBIN ZBIN32, or ZHEX type of frame received */
int Rxtype;             /* Type of header received */
int Rxcount;            /* Count of data bytes received */
extern Rxtimeout;       /* Tenths of seconds to wait for something */
char Rxhdr[4];          /* Received header */
char Txhdr[4];          /* Transmitted header */
long Rxpos;             /* Received file position */
long Txpos;             /* Transmitted file position */
int Txfcs32;            /* TURE means send binary frames with 32 bit FCS */
int Znulls;             /* Number of nulls to send at beginning of ZDATA hdr */
char Attn[ZATTNLEN+1];  /* Attention string rx sends to tx on err */

SHAR_EOF
echo shar: extracting zm.c '(12381 characters)'
cat << \SHAR_EOF > zm.c
/*
 *   Z M . C
 *    ZMODEM protocol primitives
 *    01-15-87  Chuck Forsberg Omen Technology Inc
 *
 * Entry point Functions:
 *      zsbhdr(type, hdr) send binary header
 *      zshhdr(type, hdr) send hex header
 *      zgethdr(hdr, eflag) receive header - binary or hex
 *      zsdata(buf, len, frameend) send data
 *      zrdata(buf, len) receive data
 *      stohdr(pos) store position data in Txhdr
 *      long rclhdr(hdr) recover position offset from header
 */

#ifndef CANFDX
#include "zmodem.h"
int Rxtimeout = 100;            /* Tenths of seconds to wait for something */
#endif

static char *frametypes[] = {
        "Carrier Lost",         /* -3 */
        "TIMEOUT",              /* -2 */
        "ERROR",                /* -1 */
#define FTOFFSET 3
        "ZRQINIT",
        "ZRINIT",
        "ZSINIT",
        "ZACK",
        "ZFILE",
        "ZSKIP",
        "ZNAK",
        "ZABORT",
        "ZFIN",
        "ZRPOS",
        "ZDATA",
        "ZEOF",
        "ZFERR",
        "ZCRC",
        "ZCHALLENGE",
        "ZCOMPL",
        "ZCAN",
        "ZFREECNT",
        "ZCOMMAND",
        "ZSTDERR",
        "xxxxx"
#define FRTYPES 22      /* Total number of frame types in this array */
                        /*  not including psuedo negative entries */
};

/* Send ZMODEM binary header hdr of type type */
zsbhdr(type, hdr)
register char *hdr;
{
        register n;
        register unsigned short crc;

        vfile("zsbhdr: %s %lx", frametypes[type+FTOFFSET], rclhdr(hdr));
        if (type == ZDATA)
                for (n = Znulls; --n >=0; )
                        BufferData(0);

        BufferChar(ZPAD); BufferChar(ZDLE);

        if (Txfcs32)
                zsbh32(hdr, type);
        else {
                BufferChar(ZBIN); BufferData(type); crc = updcrc(type, 0);

                for (n=4; --n >= 0;) {
                        BufferData(*hdr);
                        crc = updcrc((0377& *hdr++), crc);
                }
                crc = updcrc(0,updcrc(0,crc));
                BufferData(crc>>8);
                BufferData(crc);
        }
        if (type != ZDATA)
                flushmo();
}


/* Send ZMODEM binary header hdr of type type */
zsbh32(hdr, type)
register char *hdr;
{
        register n;
        register unsigned long crc;

        BufferChar(ZBIN32);  BufferData(type);
        crc = UPDC32(type, 0xFFFFFFFF);

        for (n=4; --n >= 0;) {
                BufferData(*hdr);
                crc = UPDC32((0377& *hdr++), crc);
        }
        crc = ~crc;
        for (n=4; --n >= 0;) {
                BufferData(crc);  crc >>= 8;
        }
}

/* Send ZMODEM HEX header hdr of type type */
zshhdr(type, hdr)
register char *hdr;
{
        register n;
        register unsigned short crc;

        vfile("zshhdr: %s %lx", frametypes[type+FTOFFSET], rclhdr(hdr));
        BufferChar(ZPAD); BufferChar(ZPAD); BufferChar(ZDLE); BufferChar(ZHEX);
        BufferHexC(type);

        crc = updcrc(type, 0);
        for (n=4; --n >= 0;) {
                BufferHexC(*hdr); crc = updcrc((0377& *hdr++), crc);
        }
        crc = updcrc(0,updcrc(0,crc));
        BufferHexC(crc>>8); BufferHexC(crc);

        /* Make it printable on remote machine */
        BufferChar(015); BufferChar(012);
        /*
         * Uncork the remote in case a fake XOFF has stopped data flow
         */
        if (type != ZFIN)
                BufferChar(021);
        flushmo();
}

/*
 * Send binary array buf of length length, with ending ZDLE sequence frameend
 */
zsdata(buf, length, frameend)
register char *buf;
{
        register unsigned short crc;

        vfile("zsdata: length=%d end=%x", length, frameend);
        if (Txfcs32)
                zsda32(buf, length, frameend);
        else {
                crc = 0;
                for (;--length >= 0;) {
                        BufferData(*buf); crc = updcrc((0377& *buf++), crc);
                }
                BufferChar(ZDLE); BufferChar(frameend);
                crc = updcrc(frameend, crc);

                crc = updcrc(0,updcrc(0,crc));
                BufferData(crc>>8); BufferData(crc);
        }
        if (frameend == ZCRCW) {
                BufferChar(XON);  flushmo();
        }
}

zsda32(buf, length, frameend)
register char *buf;
{
        register unsigned long crc;

        crc = 0xFFFFFFFF;
        for (;--length >= 0;) {
                BufferData(*buf); crc = UPDC32((0377& *buf++), crc);
        }
        BufferChar(ZDLE); BufferChar(frameend);
        crc = UPDC32(frameend, crc);

        crc = ~crc;
        for (length=4; --length >= 0;) {
                BufferData(crc);  crc >>= 8;
        }
}

/*
 * Receive array buf of max length with ending ZDLE sequence
 *  and CRC.  Returns the ending character or error code.
 */
zrdata(buf, length)
register char *buf;
{
        register c;
        register unsigned short crc;
        register d;

        if (Rxframeind == ZBIN32)
                return zrdat32(buf, length);

        crc = Rxcount = 0;
        for (;;) {
                if ((c = zdlread()) & ~0377) {
crcfoo:
                        switch (c) {
                        case GOTCRCE:
                        case GOTCRCG:
                        case GOTCRCQ:
                        case GOTCRCW:
                                crc = updcrc((d=c)&0377, crc);
                                if ((c = zdlread()) & ~0377)
                                        goto crcfoo;
                                crc = updcrc(c, crc);
                                if ((c = zdlread()) & ~0377)
                                        goto crcfoo;
                                crc = updcrc(c, crc);
                                if (crc & 0xFFFF) {
                                        zperr("Bad data CRC %x", crc);
                                        return ERROR;
                                }
                                vfile("zrdata: cnt = %d ret = %x", Rxcount, d);
                                return d;
                        case GOTCAN:
                                zperr("ZMODEM: Sender Canceled");
                                return ZCAN;
                        case TIMEOUT:
                                zperr("ZMODEM data TIMEOUT");
                                return c;
                        default:
                                zperr("ZMODEM bad data subpacket ret=%x", c);
                                return c;
                        }
                }
                if (--length < 0) {
                        zperr("ZMODEM data subpacket too long");
                        return ERROR;
                }
                ++Rxcount;
                *buf++ = c;
                crc = updcrc(c, crc);
                continue;
        }
}
zrdat32(buf, length)
register char *buf;
{
        register c;
        register unsigned long crc;
        register d;

        crc = 0xFFFFFFFF;  Rxcount = 0;
        for (;;) {
                if ((c = zdlread()) & ~0377) {
crcfoo:
                        switch (c) {
                        case GOTCRCE:
                        case GOTCRCG:
                        case GOTCRCQ:
                        case GOTCRCW:
                                crc = UPDC32((d=c)&0377, crc);
                                if ((c = zdlread()) & ~0377)
                                        goto crcfoo;
                                crc = UPDC32(c, crc);
                                if ((c = zdlread()) & ~0377)
                                        goto crcfoo;
                                crc = UPDC32(c, crc);
                                if ((c = zdlread()) & ~0377)
                                        goto crcfoo;
                                crc = UPDC32(c, crc);
                                if ((c = zdlread()) & ~0377)
                                        goto crcfoo;
                                crc = UPDC32(c, crc);
                                if (crc != 0xDEBB20E3) {
                                        zperr("Bad data CRC %lX", crc);
                                        return ERROR;
                                }
                                vfile("zrdat32: cnt = %d ret = %x", Rxcount, d);
                                return d;
                        case GOTCAN:
                                zperr("ZMODEM: Sender Canceled");
                                return ZCAN;
                        case TIMEOUT:
                                zperr("ZMODEM data TIMEOUT");
                                return c;
                        default:
                                zperr("ZMODEM bad data subpacket ret=%x", c);
                                return c;
                        }
                }
                if (--length < 0) {
                        zperr("ZMODEM data subpacket too long");
                        return ERROR;
                }
                ++Rxcount;
                *buf++ = c;
                crc = UPDC32(c, crc);
                continue;
        }
}


/*
 * Read a ZMODEM header to hdr, either binary or hex.
 *  eflag controls local display of non zmodem characters:
 *      0:  no display
 *      1:  display printing characters only
 *      2:  display all non ZMODEM characters
 *  On success, set Zmodem to 1 and return type of header.
 *   Otherwise return negative on error
 */
zgethdr(hdr, eflag)
char *hdr;
{
        register c, n, cancount;

        n = Baudrate;   /* Max characters before start of frame */
        cancount = 5;
again:
        Rxframeind = Rxtype = 0;
        switch (c = noxread7()) {
        case RCDO:
        case TIMEOUT:
                goto fifi;
        case CAN:
                if (--cancount <= 0) {
                        c = ZCAN; goto fifi;
                }
        /* **** FALL THRU TO **** */
        default:
agn2:
                if ( --n == 0) {
                        zperr("ZMODEM Garbage count exceeded");
                        return(ERROR);
                }
                if (eflag && ((c &= 0177) & 0140))
                        bttyout(c);
                else if (eflag > 1)
                        bttyout(c);
                if (c != CAN)
                        cancount = 5;
                goto again;
        case ZPAD:              /* This is what we want. */
                break;
        }
        cancount = 5;
splat:
        switch (c = noxread7()) {
        case ZPAD:
                goto splat;
        case RCDO:
        case TIMEOUT:
                goto fifi;
        default:
                goto agn2;
        case ZDLE:              /* This is what we want. */
                break;
        }

        switch (c = noxread7()) {
        case RCDO:
        case TIMEOUT:
                goto fifi;
        case ZBIN:
                Rxframeind = ZBIN;
                c =  zrbhdr(hdr);
                break;
        case ZBIN32:
                Rxframeind = ZBIN32;
                c =  zrbhdr32(hdr);
                break;
        case ZHEX:
                Rxframeind = ZHEX;
                c =  zrhhdr(hdr);
                break;
        case CAN:
                if (--cancount <= 0) {
                        c = ZCAN; goto fifi;
                }
                goto agn2;
        default:
                goto agn2;
        }
        Rxpos = hdr[ZP3] & 0377;
        Rxpos = (Rxpos<<8) + (hdr[ZP2] & 0377);
        Rxpos = (Rxpos<<8) + (hdr[ZP1] & 0377);
        Rxpos = (Rxpos<<8) + (hdr[ZP0] & 0377);
fifi:
        switch (c) {
        case GOTCAN:
                c = ZCAN;
        /* **** FALL THRU TO **** */
        case ZNAK:
        case ZCAN:
        case ERROR:
        case TIMEOUT:
        case RCDO:
                zperr("ZMODEM: Got %s %s", frametypes[c+FTOFFSET],
                  (c >= 0) ? "header" : "error");
        /* **** FALL THRU TO **** */
        default:
                if (c >= -3 && c <= FRTYPES)
                        vfile("zgethdr: %s %lx", frametypes[c+FTOFFSET], Rxpos);
                else
                        vfile("zgethdr: %d %lx", c, Rxpos);
        }
        return c;
}

/* Receive a binary style header (type and position) */
zrbhdr(hdr)
register char *hdr;
{
        register c, n;
        register unsigned short crc;

        if ((c = zdlread()) & ~0377)
                return c;
        Rxtype = c;
        crc = updcrc(c, 0);

        for (n=4; --n >= 0;) {
                if ((c = zdlread()) & ~0377)
                        return c;
                crc = updcrc(c, crc);
                *hdr++ = c;
        }
        if ((c = zdlread()) & ~0377)
                return c;
        crc = updcrc(c, crc);
        if ((c = zdlread()) & ~0377)
                return c;
        crc = updcrc(c, crc);
        if (crc & 0xFFFF) {
                zperr("Bad Header CRC"); return ERROR;
        }
        Zmodem = 1;
        return Rxtype;
}

/* Receive a binary style header (type and position) with 32 bit FCS */
zrbhdr32(hdr)
register char *hdr;
{
        register c, n;
        register unsigned long crc;

        if ((c = zdlread()) & ~0377)
                return c;
        Rxtype = c;
        crc = UPDC32(c, 0xFFFFFFFF);

        for (n=4; --n >= 0;) {
                if ((c = zdlread()) & ~0377)
                        return c;
                crc = UPDC32(c, crc);
                *hdr++ = c;
        }
        for (n=4; --n >= 0;) {
                if ((c = zdlread()) & ~0377)
                        return c;
                crc = UPDC32(c, crc);
        }
        if (crc != 0xDEBB20E3) {
                zperr("Bad Header CRC %lX", crc); return ERROR;
        }
        Zmodem = 1;
        return Rxtype;
}


/* Receive a hex style header (type and position) */
zrhhdr(hdr)
char *hdr;
{
        register c;
        register unsigned short crc;
        register n;

        if ((c = zgethex()) < 0)
                return c;
        Rxtype = c;
        crc = updcrc(c, 0);

        for (n=4; --n >= 0;) {
                if ((c = zgethex()) < 0)
                        return c;
                crc = updcrc(c, crc);
                *hdr++ = c;
        }
        if ((c = zgethex()) < 0)
                return c;
        crc = updcrc(c, crc);
        if ((c = zgethex()) < 0)
                return c;
        crc = updcrc(c, crc);
        if (crc & 0xFFFF) {
                zperr("Bad Header CRC"); return ERROR;
        }
        if (readline(1) == '\r')        /* Throw away possible cr/lf */
                readline(1);
        Zmodem = 1; return Rxtype;
}

/* Send a byte as two hex digits */
BufferHexC(c)
register c;
{
        static char     digits[]        = "0123456789abcdef";

        if (Verbose>4)
                vfile("BufferHexC: %x", c);
        BufferChar(digits[(c&0xF0)>>4]);
        BufferChar(digits[(c)&0xF]);
}

/*
 * Send character c with ZMODEM escape sequence encoding.
 *  Escape XON, XOFF. Escape CR following @ (Telenet net escape)
 */
BufferData(c)
register c;
{
        static lastsent;

        switch (c & 0377) {
        case ZDLE:
                BufferChar(ZDLE);
                BufferChar (lastsent = (c ^= 0100));
                break;
        case 015:
        case 0215:
                if ((lastsent & 0177) != '@')
                        goto sendit;
        /* **** FALL THRU TO **** */
        case 020:
        case 021:
        case 023:
        case 0220:
        case 0221:
        case 0223:
#ifdef ZKER
                if (Zctlesc<0)
                        goto sendit;
#endif
                BufferChar(ZDLE);
                c ^= 0100;
sendit:
                BufferChar(lastsent = c);
                break;
        default:
#ifdef ZKER
                if (Zctlesc>0 && ! (c & 0140)) {
                        BufferChar(ZDLE);
                        c ^= 0100;
                }
#endif
                BufferChar(lastsent = c);
        }
}

/* Decode two lower case hex digits into an 8 bit byte value */
zgethex()
{
        register c;

        c = zgeth1();
        if (Verbose>4)
                vfile("zgethex: %x", c);
        return c;
}
zgeth1()
{
        register c, n;

        if ((c = noxread7()) < 0)
                return c;
        n = c - '0';
        if (n > 9)
                n -= ('a' - ':');
        if (n & ~0xF)
                return ERROR;
        if ((c = noxread7()) < 0)
                return c;
        c -= '0';
        if (c > 9)
                c -= ('a' - ':');
        if (c & ~0xF)
                return ERROR;
        c += (n<<4);
        return c;
}

/*
 * Read a byte, checking for ZMODEM escape encoding
 *  including CAN*5 which represents a quick abort
 */
zdlread()
{
        register c;

        if ((c = readline(Rxtimeout)) != ZDLE)
                return c;
        if ((c = readline(Rxtimeout)) < 0)
                return c;
        if (c == CAN && (c = readline(Rxtimeout)) < 0)
                return c;
        if (c == CAN && (c = readline(Rxtimeout)) < 0)
                return c;
        if (c == CAN && (c = readline(Rxtimeout)) < 0)
                return c;
        switch (c) {
        case CAN:
                return GOTCAN;
        case ZCRCE:
        case ZCRCG:
        case ZCRCQ:
        case ZCRCW:
                return (c | GOTOR);
        case ZRUB0:
                return 0177;
        case ZRUB1:
                return 0377;
        default:
                if ((c & 0140) ==  0100)
                        return (c ^ 0100);
                break;
        }
        zperr("Got bad ZMODEM escape sequence %x", c);
        return ERROR;
}

/*
 * Read a character from the modem line with timeout.
 *  Eat parity, XON and XOFF characters.
 */
noxread7()
{
        register c;

        for (;;) {
                if ((c = readline(Rxtimeout)) < 0)
                        return c;
                switch (c &= 0177) {
                case XON:
                case XOFF:
                        continue;
                default:
                        return c;
                }
        }
}

/* Store long integer pos in Txhdr */
stohdr(pos)
long pos;
{
        Txhdr[ZP0] = pos;
        Txhdr[ZP1] = pos>>8;
        Txhdr[ZP2] = pos>>16;
        Txhdr[ZP3] = pos>>24;
}

/* Recover a long integer from a header */
long
rclhdr(hdr)
register char *hdr;
{
        register long l;

        l = (hdr[ZP3] & 0377);
        l = (l << 8) | (hdr[ZP2] & 0377);
        l = (l << 8) | (hdr[ZP1] & 0377);
        l = (l << 8) | (hdr[ZP0] & 0377);
        return l;
}

SHAR_EOF
echo shar: extracting sz.c '(27639 characters)'
cat << \SHAR_EOF > sz.c
#define VERSION "sz 1.23 01-15-87"
#define PUBDIR "/usr/spool/uucppublic"

/*% cc -O -K -i -DCRCTABLE -DREADCHECK sz.c -lx -o sz; size sz

 * sz.c By Chuck Forsberg
 *
 *      cc -O sz.c -o sz                USG (SYS III/V) Unix
 *      cc -O -DSVR2 sz.c -o sz         Sys V Release 2 with non-blocking input
 *                                      Define to allow reverse channel checking
 *      cc -O -DV7  sz.c -o sz          Unix Version 7, 2.8 - 4.3 BSD
 *
 *      ln sz sb                        **** All versions ****
 *
 *              define CRCTABLE to use table driven CRC
 *
 *  ******* Some systems (Venix, Coherent, Regulus) do not *******
 *  ******* support tty raw mode read(2) identically to    *******
 *  ******* Unix. ONEREAD must be defined to force one     *******
 *  ******* character reads for these systems.             *******
 *
 * A program for Unix to send files and commands to computers running
 *  Professional-YAM, PowerCom, YAM, IMP, or programs supporting Y/XMODEM.
 *
 *  Sz uses buffered I/O to greatly reduce CPU time compared to UMODEM.
 *
 *  USG UNIX (3.0) ioctl conventions courtesy Jeff Martin
 */


char *substr(), *getenv();

#define LOGFILE "/tmp/szlog"
#define zperr vfile

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>

#define PATHLEN 256
#define OK 0
#define FALSE 0
#define TRUE 1
#define ERROR (-1)

#define HOWMANY 2
int Zmodem=0;           /* ZMODEM protocol requested */
unsigned Baudrate;
int Fromcu = 0;         /* Were called from cu or yam */
#include "rbsb.c"       /* most of the system dependent stuff here */

/*
 * Attention string to be executed by receiver to interrupt streaming data
 *  when an error is detected.  A pause (0336) may be needed before the
 *  ^C (03) or after it.
 */
#ifdef READCHECK
char Myattn[] = { 0 };
#else
#ifdef USG
char Myattn[] = { 03, 0336, 0 };
#else
char Myattn[] = { 0 };
#endif
#endif

FILE *in;

/* Ward Christensen / CP/M parameters - Don't change these! */
#define ENQ 005
#define CAN ('X'&037)
#define XOFF ('s'&037)
#define XON ('q'&037)
#define SOH 1
#define STX 2
#define EOT 4
#define ACK 6
#define NAK 025
#define CPMEOF 032
#define WANTCRC 0103    /* send C not NAK to get crc not checksum */
#define WANTG 0107      /* Send G not NAK to get nonstop batch xmsn */
#define TIMEOUT (-2)
#define RCDO (-3)
#define RETRYMAX 10
#define SECSIZ 128      /* cp/m's Magic Number record size */
#define KSIZE 1024

char Lastrx;
char Crcflg;
int Wcsmask=0377;
int Verbose=0;
int Modem=0;            /* MODEM - don't send pathnames */
int Restricted=0;       /* restricted; no /.. or ../ in filenames */
int Quiet=0;            /* overrides logic that would otherwise set verbose */
int Ascii=0;            /* Add CR's for brain damaged programs */
int Fullname=0;         /* transmit full pathname */
int Unlinkafter=0;      /* Unlink file after it is sent */
int Dottoslash=0;       /* Change foo.bar.baz to foo/bar/baz */
int firstsec;
int errcnt=0;           /* number of files unreadable */
int blklen=SECSIZ;              /* length of transmitted records */
int Optiong;            /* Let it rip no wait for sector ACK's */
int Noeofseen;
int Totsecs;            /* total number of sectors this file */
char txbuf[KSIZE];
int Filcnt=0;           /* count of number of files opened */
int Lfseen=0;
unsigned Rxbuflen = 16384;      /* Receiver's max buffer length */
int Tframlen = 0;       /* Override for tx frame length */
int blkopt=0;           /* Override value for zmodem blklen */
int Rxflags = 0;
int Wantfcs32 = TRUE;   /* want to send 32 bit FCS */
char Lzconv;    /* Local ZMODEM file conversion request */
char Lzmanag;   /* Local ZMODEM file management request */
char Lztrans;
char zconv;             /* ZMODEM file conversion request */
char zmanag;            /* ZMODEM file management request */
char ztrans;            /* ZMODEM file transport request */
int Command;            /* Send a command, then exit. */
char *Cmdstr;           /* Pointer to the command string */
int Cmdtries = 11;
int Cmdack1;            /* Rx ACKs command, then do it */
int Exitcode;
int Testattn;           /* Force receiver to send Attn, etc with qbf. */
char *qbf="The quick brown fox jumped over the lazy dog's back 1234567890\r\n";

long Lastread;          /* Beginning offset of last buffer read */
int Lastc;              /* Count of last buffer read or -1 */
int Dontread;           /* Don't read the buffer, it's still there */

jmp_buf tohere;         /* For the interrupt on RX timeout */
jmp_buf intrjmp;        /* For the interrupt on RX CAN */

/* called by signal interrupt or terminate to clean things up */
bibi(n)
{
        canit(); fflush(stdout); mode(0);
        fprintf(stderr, "sz: caught signal %d; exiting\n", n);
        if (n == SIGQUIT)
                abort();
        exit(128+n);
}
/* Called when Zmodem gets an interrupt (^X) */
onintr()
{
        signal(SIGINT, SIG_IGN);
        longjmp(intrjmp, -1);
}


#define BufferChar(c) putchar(c & Wcsmask)

flushmo()
{
        fflush(stdout);
}

#define ZKER
int Zctlesc;    /* Encode control characters */
int Nozmodem = 0;       /* If invoked as "sb" */
char *Progname = "sz";
#include "zm.c"


main(argc, argv)
char *argv[];
{
        register char *cp;
        register npats;
        int agcnt; char **agcv;
        char **patts;
        static char xXbuf[BUFSIZ];

        if ((cp = getenv("ZNULLS")) && *cp)
                Znulls = atoi(cp);
        if ((cp=getenv("SHELL")) && (substr(cp, "rsh") || substr(cp, "rksh")))
                Restricted=TRUE;
        chkinvok(argv[0]);

        Rxtimeout = 600;
        npats=0;
        if (argc<2)
                usage();
        setbuf(stdout, xXbuf);          
        while (--argc) {
                cp = *++argv;
                if (*cp++ == '-' && *cp) {
                        while ( *cp) {
                                switch(*cp++) {
                                case '+':
                                        Lzmanag = ZMAPND; break;
                                case '1':
                                        iofd = 1; break;
#ifdef CSTOPB
                                case '2':
                                        Twostop = TRUE; break;
#endif
                                case '7':
                                        Wcsmask=0177; break;
                                case 'a':
                                        Lzconv = ZCNL;
                                        Ascii = TRUE; break;
                                case 'b':
                                        Lzconv = ZCBIN; break;
                                case 'C':
                                        if (--argc < 1) {
                                                usage();
                                        }
                                        Cmdtries = atoi(*++argv);
                                        break;
                                case 'i':
                                        Cmdack1 = ZCACK1;
                                        /* **** FALL THROUGH TO **** */
                                case 'c':
                                        if (--argc != 1) {
                                                usage();
                                        }
                                        Command = TRUE;
                                        Cmdstr = *++argv;
                                        break;
                                case 'd':
                                        ++Dottoslash;
                                        /* **** FALL THROUGH TO **** */
                                case 'f':
                                        Fullname=TRUE; break;
                                case 'E':
                                        Zctlesc = -1; break;
                                case 'e':
                                        Zctlesc = 1; break;
                                case 'k':
                                        blklen=KSIZE; break;
                                case 'L':
                                        if (--argc < 1) {
                                                usage();
                                        }
                                        blkopt = atoi(*++argv);
                                        if (blkopt<24 || blkopt>1024)
                                                usage();
                                        break;
                                case 'l':
                                        if (--argc < 1) {
                                                usage();
                                        }
                                        Tframlen = atoi(*++argv);
                                        if (Tframlen<32 || Tframlen>1024)
                                                usage();
                                        break;
                                case 'N':
                                        Lzmanag = ZMDIFF;  break;
                                case 'n':
                                        Lzmanag = ZMNEW;  break;
                                case 'o':
                                        Wantfcs32 = FALSE; break;
                                case 'p':
                                        Lzmanag = ZMPROT;  break;
                                case 'r':
                                        Lzconv = ZCRESUM;
                                case 'q':
                                        Quiet=TRUE; Verbose=0; break;
                                case 't':
                                        if (--argc < 1) {
                                                usage();
                                        }
                                        Rxtimeout = atoi(*++argv);
                                        if (Rxtimeout<10 || Rxtimeout>1000)
                                                usage();
                                        break;
                                case 'T':
                                        Testattn = TRUE; break;
                                case 'u':
                                        ++Unlinkafter; break;
                                case 'v':
                                        ++Verbose; break;
                                case 'X':
                                        ++Modem; break;
                                case 'y':
                                        Lzmanag = ZMCLOB; break;
                                default:
                                        usage();
                                }
                        }
                }
                else if ( !npats && argc>0) {
                        if (argv[0][0]) {
                                npats=argc;
                                patts=argv;
                                if ( !strcmp(*patts, "-"))
                                        iofd = 1;
                        }
                }
        }
        if (npats < 1 && !Command) 
                usage();
        if (Verbose) {
                if (freopen(LOGFILE, "a", stderr)==NULL) {
                        printf("Can't open log file %s\n",LOGFILE);
                        exit(0200);
                }
                setbuf(stderr, NULL);
        }
        if ((Fromcu=from_cu()) && !Quiet) {
                if (Verbose == 0)
                        Verbose = 2;
        }

        mode(1);

        if (signal(SIGINT, bibi) == SIG_IGN) {
                signal(SIGINT, SIG_IGN); signal(SIGKILL, SIG_IGN);
        } else {
                signal(SIGINT, bibi); signal(SIGKILL, bibi);
        }
        if ( !Fromcu)
                signal(SIGQUIT, SIG_IGN);

        if ( !Modem) {
                if (!Nozmodem) {
                        printf("rz\r");  fflush(stdout);
                }
                if (!Command && !Quiet && Verbose != 1) {
                        fprintf(stderr, "%s: %d file%s requested:\r\n",
                         Progname, npats, npats>1?"s":"");
                        for ( agcnt=npats, agcv=patts; --agcnt>=0; ) {
                                fprintf(stderr, "%s ", *agcv++);
                        }
                        fprintf(stderr, "\r\n");
                        printf("\r\n\bSending in Batch Mode\r\n");
                }
                if (!Nozmodem) {
                        stohdr(0L);
                        if (Command)
                                Txhdr[ZF0] = ZCOMMAND;
                        zshhdr(ZRQINIT, Txhdr);
                }
        }
        fflush(stdout);

        if (Command) {
                if (getzrxinit()) {
                        Exitcode=0200; canit();
                }
                else if (zsendcmd(Cmdstr, 1+strlen(Cmdstr))) {
                        Exitcode=0200; canit();
                }
        } else if (wcsend(npats, patts)==ERROR) {
                Exitcode=0200;
                canit();
        }
        fflush(stdout);
        mode(0);
        exit((errcnt != 0) | Exitcode);
        /*NOTREACHED*/
}

wcsend(argc, argp)
char *argp[];
{
        register n;

        Crcflg=FALSE;
        firstsec=TRUE;
        for (n=0; n<argc; ++n) {
                Totsecs = 0;
                if (wcs(argp[n])==ERROR)
                        return ERROR;
        }
        Totsecs = 0;
        if (Filcnt==0) {        /* bitch if we couldn't open ANY files */
                if (1) {
                        Command = TRUE;
                        Cmdstr = "echo \"sz: Can't open any requested files\"";
                        if (getnak()) {
                                Exitcode=0200; canit();
                        }
                        if (!Zmodem)
                                canit();
                        else if (zsendcmd(Cmdstr, 1+strlen(Cmdstr))) {
                                Exitcode=0200; canit();
                        }
                        Exitcode = 1; return OK;
                }
                canit();
                fprintf(stderr,"\r\nCan't open any requested files.\r\n");
                return ERROR;
        }
        if (Zmodem)
                saybibi();
        else
                wctxpn("");
        return OK;
}

wcs(oname)
char *oname;
{
        register c;
        register char *p;
        struct stat f;
        char name[PATHLEN];

        strcpy(name, oname);

        if (Restricted) {
                /* restrict pathnames to current tree or uucppublic */
                if ( substr(name, "../")
                 || (name[0]== '/' && strncmp(name, PUBDIR, strlen(PUBDIR))) ) {
                        canit();
                        fprintf(stderr,"\r\nsz:\tSecurity Violation\r\n");
                        return ERROR;
                }
        }

        if ( !strcmp(oname, "-")) {
                if ((p = getenv("ONAME")) && *p)
                        strcpy(name, p);
                else
                        sprintf(name, "s%d.sz", getpid());
                in = stdin;
        }
        else if ((in=fopen(oname, "r"))==NULL) {
                ++errcnt;
                return OK;      /* pass over it, there may be others */
        }
        ++Noeofseen;  Lastread = 0;  Lastc = -1; Dontread = FALSE;
        /* Check for directory or block special files */
        fstat(fileno(in), &f);
        c = f.st_mode & S_IFMT;
        if (c == S_IFDIR || c == S_IFBLK) {
                fclose(in);
                return OK;
        }

        ++Filcnt;
        switch (wctxpn(name)) {
        case ERROR:
                return ERROR;
        case ZSKIP:
                return OK;
        }
        if (!Zmodem && wctx()==ERROR)
                return ERROR;
        if (Unlinkafter)
                unlink(oname);
        return 0;
}

/*
 * generate and transmit pathname block consisting of
 *  pathname (null terminated),
 *  file length, mode time and file mode in octal
 *  as provided by the Unix fstat call.
 *  N.B.: modifies the passed name, may extend it!
 */
wctxpn(name)
char *name;
{
        register char *p, *q;
        char name2[PATHLEN];
        struct stat f;

        if (Modem) {
                if ((in!=stdin) && *name && fstat(fileno(in), &f)!= -1) {
                        fprintf(stderr, "Sending %s, %ld blocks: ",
                          name, f.st_size>>7);
                }
                fprintf(stderr, "Give your local XMODEM receive command now.\r\n");
                return OK;
        }
        logent("\r\nAwaiting pathname nak for %s\r\n", *name?name:"<END>");
        if ( !Zmodem)
                if (getnak())
                        return ERROR;

        q = (char *) 0;
        if (Dottoslash) {               /* change . to . */
                for (p=name; *p; ++p) {
                        if (*p == '/')
                                q = p;
                        else if (*p == '.')
                                *(q=p) = '/';
                }
                if (q && strlen(++q) > 8) {     /* If name>8 chars */
                        q += 8;                 /*   make it .ext */
                        strcpy(name2, q);       /* save excess of name */
                        *q = '.';
                        strcpy(++q, name2);     /* add it back */
                }
        }

        for (p=name, q=txbuf ; *p; )
                if ((*q++ = *p++) == '/' && !Fullname)
                        q = txbuf;
        *q++ = 0;
        p=q;
        while (q < (txbuf + KSIZE))
                *q++ = 0;
        if (!Ascii && (in!=stdin) && *name && fstat(fileno(in), &f)!= -1)
                sprintf(p, "%lu %lo %o", f.st_size, f.st_mtime, f.st_mode);
        /* force 1k blocks if name won't fit in 128 byte block */
        if (txbuf[125])
                blklen=KSIZE;
        else {          /* A little goodie for IMP/KMD */
                if (Zmodem)
                        blklen = SECSIZ;
                txbuf[127] = (f.st_size + 127) >>7;
                txbuf[126] = (f.st_size + 127) >>15;
        }
        if (Zmodem)
                return zsendfile(txbuf, 1+strlen(p)+(p-txbuf));
        if (wcputsec(txbuf, 0, SECSIZ)==ERROR)
                return ERROR;
        return OK;
}

getnak()
{
        register firstch;

        Lastrx = 0;
        for (;;) {
                switch (firstch = readock(800,1)) {
                case ZPAD:
                        if (getzrxinit())
                                return ERROR;
                        Ascii = 0;
                        return FALSE;
                case TIMEOUT:
                        logent("Timeout on pathname\n");
                        return TRUE;
                case WANTG:
#ifdef USG
                        mode(2);        /* Set cbreak, XON/XOFF, etc. */
#endif
                        Optiong = TRUE;
                        blklen=KSIZE;
                case WANTCRC:
                        Crcflg = TRUE;
                case NAK:
                        return FALSE;
                case CAN:
                        if ((firstch = readock(20,1)) == CAN && Lastrx == CAN)
                                return TRUE;
                default:
                        break;
                }
                Lastrx = firstch;
        }
}


wctx()
{
        register int sectnum, attempts, firstch;

        firstsec=TRUE;

        while ((firstch=readock(Rxtimeout, 2))!=NAK && firstch != WANTCRC
          && firstch != WANTG && firstch!=TIMEOUT && firstch!=CAN)
                ;
        if (firstch==CAN) {
                logent("Receiver CANcelled\n");
                return ERROR;
        }
        if (firstch==WANTCRC)
                Crcflg=TRUE;
        if (firstch==WANTG)
                Crcflg=TRUE;
        sectnum=1;
        while (filbuf(txbuf, blklen)) {
                if (wcputsec(txbuf, sectnum, blklen)==ERROR) {
                        return ERROR;
                } else
                        sectnum++;
        }
        if (Verbose>1)
                fprintf(stderr, " Closing ");
        fclose(in);
        attempts=0;
        do {
                logent(" EOT ");
                purgeline();
                BufferChar(EOT);
                fflush(stdout);
                ++attempts;
        }
                while ((firstch=(readock(Rxtimeout, 1)) != ACK) && attempts < RETRYMAX);
        if (attempts == RETRYMAX) {
                logent("No ACK on EOT\n");
                return ERROR;
        }
        else
                return OK;
}

wcputsec(buf, sectnum, cseclen)
char *buf;
int sectnum;
int cseclen;    /* data length of this sector to send */
{
        register checksum, wcj;
        register char *cp;
        unsigned oldcrc;
        int firstch;
        int attempts;

        firstch=0;      /* part of logic to detect CAN CAN */

        if (Verbose>1)
                fprintf(stderr, "\rSector %3d %2dk ", Totsecs, Totsecs/8 );
        for (attempts=0; attempts <= RETRYMAX; attempts++) {
                Lastrx= firstch;
                BufferChar(cseclen==KSIZE?STX:SOH);
                BufferChar(sectnum);
                BufferChar(-sectnum -1);
                oldcrc=checksum=0;
                for (wcj=cseclen,cp=buf; --wcj>=0; ) {
                        BufferChar(*cp);
                        oldcrc=updcrc((0377& *cp), oldcrc);
                        checksum += *cp++;
                }
                if (Crcflg) {
                        oldcrc=updcrc(0,updcrc(0,oldcrc));
                        BufferChar((int)oldcrc>>8);
                        BufferChar((int)oldcrc);
                }
                else
                        BufferChar(checksum);

                if (Optiong) {
                        firstsec = FALSE; return OK;
                }
                firstch = readock(Rxtimeout, (Noeofseen&&sectnum) ? 2:1);
gotnak:
                switch (firstch) {
                case CAN:
                        if(Lastrx == CAN) {
cancan:
                                logent("Cancelled\n");  return ERROR;
                        }
                        break;
                case TIMEOUT:
                        logent("Timeout on sector ACK\n"); continue;
                case WANTCRC:
                        if (firstsec)
                                Crcflg = TRUE;
                case NAK:
                        logent("NAK on sector\n"); continue;
                case ACK: 
                        firstsec=FALSE;
                        Totsecs += (cseclen>>7);
                        return OK;
                case ERROR:
                        logent("Got burst for sector ACK\n"); break;
                default:
                        logent("Got %02x for sector ACK\n", firstch); break;
                }
                for (;;) {
                        Lastrx = firstch;
                        if ((firstch = readock(Rxtimeout, 2)) == TIMEOUT)
                                break;
                        if (firstch == NAK || firstch == WANTCRC)
                                goto gotnak;
                        if (firstch == CAN && Lastrx == CAN)
                                goto cancan;
                }
        }
        logent("Retry Count Exceeded\n");
        return ERROR;
}

/* fill buf with count chars padding with ^Z for CPM */
filbuf(buf, count)
register char *buf;
{
        register c, m;

        if ( !Ascii) {
                m = read(fileno(in), buf, count);
                if (m <= 0)
                        return 0;
                while (m < count)
                        buf[m++] = 032;
                return count;
        }
        m=count;
        if (Lfseen) {
                *buf++ = 012; --m; Lfseen = 0;
        }
        while ((c=getc(in))!=EOF) {
                if (c == 012) {
                        *buf++ = 015;
                        if (--m == 0) {
                                Lfseen = TRUE; break;
                        }
                }
                *buf++ =c;
                if (--m == 0)
                        break;
        }
        if (m==count)
                return 0;
        else
                while (--m>=0)
                        *buf++ = CPMEOF;
        return count;
}
/* fill buf with count chars */
zfilbuf(buf, count)
register char *buf;
{
        register c, m;

        m=count;
        while ((c=getc(in))!=EOF) {
                *buf++ =c;
                if (--m == 0)
                        break;
        }
        return (count - m);
}

/* VARARGS1 */
vfile(f, a, b, c)
register char *f;
{
        if (Verbose > 1) {
                fprintf(stderr, f, a, b, c);
                fprintf(stderr, "\n");
        }
}


alrm()
{
        longjmp(tohere, -1);
}


/*
 * readock(timeout, count) reads character(s) from file descriptor 0
 *  (1 <= count <= 3)
 * it attempts to read count characters. If it gets more than one,
 * it is an error unless all are CAN
 * (otherwise, only normal response is ACK, CAN, or C)
 *  Only looks for one if Optiong, which signifies cbreak, not raw input
 *
 * timeout is in tenths of seconds
 */
readock(timeout, count)
{
        register int c;
        static char byt[5];

        if (Optiong)
                count = 1;      /* Special hack for cbreak */

        fflush(stdout);
        if (setjmp(tohere)) {
                logent("TIMEOUT\n");
                return TIMEOUT;
        }
        c = timeout/10;
        if (c<2)
                c=2;
        if (Verbose>3) {
                fprintf(stderr, "Timeout=%d Calling alarm(%d) ", timeout, c);
                byt[1] = 0;
        }
        signal(SIGALRM, alrm); alarm(c);
#ifdef ONEREAD
        c=read(iofd, byt, 1);           /* regulus raw read is unique */
#else
        c=read(iofd, byt, count);
#endif
        alarm(0);
        if (Verbose>5)
                fprintf(stderr, "ret cnt=%d %x %x\n", c, byt[0], byt[1]);
        if (c<1)
                return TIMEOUT;
        if (c==1)
                return (byt[0]&0377);
        else
                while (c)
                        if (byt[--c] != CAN)
                                return ERROR;
        return CAN;
}
readline(n)
{
        return (readock(n, 1));
}


purgeline()
{
#ifdef USG
        ioctl(iofd, TCFLSH, 0);
#else
        lseek(iofd, 0L, 2);
#endif
}


/* send cancel string to get the other end to shut up */
canit()
{
        static char canistr[] = {
         24,24,24,24,24,24,24,24,24,24,8,8,8,8,8,8,8,8,8,8,0
        };

        printf(canistr);
        fflush(stdout);
}

/*VARARGS1*/
logent(a, b, c)
char *a, *b, *c;
{
        if(Verbose)
                fprintf(stderr, a, b, c);
}

/*
 * return 1 iff stdout and stderr are different devices
 *  indicating this program operating with a modem on a
 *  different line
 */
from_cu()
{
        struct stat a, b;
        fstat(1, &a); fstat(2, &b);
        return (a.st_rdev != b.st_rdev);
}

/*
 * substr(string, token) searches for token in string s
 * returns pointer to token within string if found, NULL otherwise
 */
char *
substr(s, t)
register char *s,*t;
{
        register char *ss,*tt;
        /* search for first char of token */
        for (ss=s; *s; s++)
                if (*s == *t)
                        /* compare token with substring */
                        for (ss=s,tt=t; ;) {
                                if (*tt == 0)
                                        return s;
                                if (*ss++ != *tt++)
                                        break;
                        }
        return NULL;
}

char *babble[] = {
        "Send file(s) with ZMODEM/YMODEM/XMODEM Protocol",
        "       (Y) = Option applies to YMODEM only",
        "       (Z) = Option applies to ZMODEM only",
        "Usage: sz [-12+aBbdefkLlNnquvXy] [-] file ...",
        "       sz [-1Beqv] -c COMMAND",
        "       1 Use stdout for modem input",
#ifdef CSTOPB
        "       2 Use 2 stop bits",
#endif
        "       + Append to existing destination file (Z)",
        "       a (ASCII) change NL to CR/LF",
        "       b Binary file transfer override",
        "       c send COMMAND (Z)",
        "       d Change '.' to '/' in pathnames (Y/Z)",
        "       e Escape all control characters (Z)",
        "       f send Full pathname (Y/Z)",
        "       i send COMMAND, ack Immediately (Z)",
        "       k Send 1024 byte packets (Y)",
        "       L N Limit subpacket length to N bytes (Z)",
        "       l N Limit frame length to N bytes (l>=L) (Z)",
        "       n send file if source Newer or longer (Z)",
        "       N send file if source different length or date (Z)",
        "       o Use 16 bit CRC instead of 32 bit CRC (Z)",
        "       p Protect existing destination file (Z)",
        "       r Resume/Recover interrupted file transfer (Z)",
        "       q Quiet (no progress reports)",
        "       u Unlink file after transmission",
        "       v Verbose - debugging information",
        "       X XMODEM protocol - send no pathnames",
        "       y Yes, overwrite existing file (Z)",
        "- as pathname sends standard input as sPID.sz or environment ONAME",
        ""
};

usage()
{
        char **pp;

        for (pp=babble; **pp; ++pp)
                fprintf(stderr, "%s\n", *pp);
        fprintf(stderr, "%s for %s by Chuck Forsberg  ", VERSION, OS);
        exit(1);
}

/*
 * Get the receiver's init parameters
 */
getzrxinit()
{
        register n;
        struct stat f;

        for (n=10; --n>=0; ) {
                
                switch (zgethdr(Rxhdr, 1)) {
                case ZCHALLENGE:        /* Echo receiver's challenge numbr */
                        stohdr(Rxpos);
                        zshhdr(ZACK, Txhdr);
                        continue;
                case ZCOMMAND:          /* They didn't see out ZRQINIT */
                        stohdr(0L);
                        zshhdr(ZRQINIT, Txhdr);
                        continue;
                case ZRINIT:
                        Rxflags = 0377 & Rxhdr[ZF0];
                        if (Wantfcs32 && (Rxflags & CANFC32))
                                Txfcs32 = TRUE;
                        Rxbuflen = (0377 & Rxhdr[ZP0])+((0377 & Rxhdr[ZP1])<<8);
                        vfile("Rxbuflen=%d Tframlen=%d", Rxbuflen, Tframlen);
                        if ( !Fromcu)
                                signal(SIGINT, SIG_IGN);
#ifndef READCHECK
#ifdef USG
                        mode(2);        /* Set cbreak, XON/XOFF, etc. */
#else
                        /* Use 1024 byte frames if no sample/interrupt */
                        if (Rxbuflen < 32 || Rxbuflen > 1024) {
                                Rxbuflen = 1024;
                                vfile("Rxbuflen=%d", Rxbuflen);
                        }
#endif
#endif
                        /* Override to force shorter frame length */
                        if (Rxbuflen && (Rxbuflen>Tframlen) && (Tframlen>=32))
                                Rxbuflen = Tframlen;
                        if ( !Rxbuflen && (Tframlen>=32) && (Tframlen<=1024))
                                Rxbuflen = Tframlen;
                        vfile("Rxbuflen=%d", Rxbuflen);

                        /* If using a pipe for testing set lower buf len */
                        fstat(iofd, &f);
                        if ((f.st_mode & S_IFMT) != S_IFCHR
                          && (Rxbuflen == 0 || Rxbuflen > 4096))
                                Rxbuflen = 4096;
                        /*
                         * If input is not a regular file, force ACK's each 1024
                         *  (A smarter strategey could be used here ...)
                         */
                        fstat(fileno(in), &f);
                        if (((f.st_mode & S_IFMT) != S_IFREG)
                          && (Rxbuflen == 0 || Rxbuflen > 1024))
                                Rxbuflen = 1024;
                        vfile("Rxbuflen=%d", Rxbuflen);

                        return (sendzsinit());
                case ZCAN:
                case TIMEOUT:
                        return ERROR;
                case ZRQINIT:
                        if (Rxhdr[ZF0] == ZCOMMAND)
                                continue;
                default:
                        zshhdr(ZNAK, Txhdr);
                        continue;
                }
        }
        return ERROR;
}

/* Send send-init information */
sendzsinit()
{
        register c;
        register errors;

        if (Myattn[0] == '\0')
                return OK;
        errors = 0;
        for (;;) {
                stohdr(0L);
                zsbhdr(ZSINIT, Txhdr);
                zsdata(Myattn, 1+strlen(Myattn), ZCRCW);
                c = zgethdr(Rxhdr, 1);
                switch (c) {
                case ZCAN:
                        return ERROR;
                case ZACK:
                        return OK;
                default:
                        if (++errors > 9)
                                return ERROR;
                        continue;
                }
        }
}

/* Send file name and related info */
zsendfile(buf, blen)
char *buf;
{
        register c;

        for (;;) {
                Txhdr[ZF0] = Lzconv;    /* file conversion request */
                Txhdr[ZF1] = Lzmanag;   /* file management request */
                Txhdr[ZF2] = Lztrans;   /* file transport request */
                Txhdr[ZF3] = 0;
                zsbhdr(ZFILE, Txhdr);
                zsdata(buf, blen, ZCRCW);
again:
                c = zgethdr(Rxhdr, 1);
                switch (c) {
                case ZRINIT:
                        goto again;
                case ZCAN:
                case TIMEOUT:
                case ZABORT:
                case ZFIN:
                        return ERROR;
                case ZSKIP:
                        fclose(in); return c;
                case ZRPOS:
                        fseek(in, Rxpos, 0);
                        Txpos = Rxpos; Lastc = -1; Dontread = FALSE;
                        return zsendfdata();
                case ERROR:
                default:
                        continue;
                }
        }
}

/* Send the data in the file */
zsendfdata()
{
        register c, e;
        register newcnt;
        register long tcount = 0;
        static int tleft = 6;   /* Counter for test mode */

        if (Baudrate > 300)
                blklen = 256;
        if (Baudrate > 2400)
                blklen = KSIZE;
        if (Rxbuflen && blklen>Rxbuflen)
                blklen = Rxbuflen;
        if (blkopt && blklen > blkopt)
                blklen = blkopt;
        vfile("Rxbuflen=%d blklen=%d", Rxbuflen, blklen);
somemore:
        if (setjmp(intrjmp)) {
waitack:
                c = getinsync();
                switch (c) {
                default:
                case ZCAN:
                        fclose(in);
                        return ERROR;
                case ZSKIP:
                        fclose(in);
                        return c;
                case ZACK:
                case ZRPOS:
                        break;
                case ZRINIT:
                        return OK;
                }
#ifdef READCHECK
                /*
                 * If the reverse channel can be tested for data,
                 *  this logic may be used to detect error packets
                 *  sent by the receiver, in place of setjmp/longjmp
                 *  rdchk(fdes) returns non 0 if a character is available
                 */
                while (rdchk(iofd)) {
#ifdef SVR2
                        switch (checked)
#else
                        switch (readline(1))
#endif
                        {
                        case CAN:
                        case ZPAD:
                                goto waitack;
                        }
                }
#endif
        }

        if ( !Fromcu)
                signal(SIGINT, onintr);
        newcnt = Rxbuflen;
        stohdr(Txpos);
        zsbhdr(ZDATA, Txhdr);

        /*
         * Special testing mode.  This should force receiver to Attn,ZRPOS
         *  many times.  Each time the signal should be caught, causing the
         *  file to be started over from the beginning.
         */
        if (Testattn) {
                if ( --tleft)
                        while (tcount < 20000) {
                                printf(qbf); fflush(stdout);
                                tcount += strlen(qbf);
#ifdef READCHECK
                                while (rdchk(iofd)) {
#ifdef SVR2
                                        switch (checked)
#else
                                        switch (readline(1))
#endif
                                        {
                                        case CAN:
                                        case ZPAD:
#ifdef TCFLSH
                                                ioctl(iofd, TCFLSH, 1);
#endif
                                                goto waitack;
                                        }
                                }
#endif
                        }
                signal(SIGINT, SIG_IGN); canit();
                sleep(3); purgeline(); mode(0);
                printf("\nsz: Tcount = %ld\n", tcount);
                if (tleft) {
                        printf("ERROR: Interrupts Not Caught\n");
                        exit(1);
                }
                exit(0);
        }

        do {
                if (Dontread) {
                        c = Lastc;
                } else {
                        c = zfilbuf(txbuf, blklen);
                        Lastread = Txpos;  Lastc = c;
                }
                if (Verbose > 10)
                        vfile("Dontread=%d c=%d", Dontread, c);
                Dontread = FALSE;
                if (c < blklen)
                        e = ZCRCE;
                else if (Rxbuflen && (newcnt -= c) <= 0)
                        e = ZCRCW;
                else
                        e = ZCRCG;
                zsdata(txbuf, c, e);
                Txpos += c;
                if (e == ZCRCW)
                        goto waitack;
#ifdef READCHECK
                /*
                 * If the reverse channel can be tested for data,
                 *  this logic may be used to detect error packets
                 *  sent by the receiver, in place of setjmp/longjmp
                 *  rdchk(fdes) returns non 0 if a character is available
                 */
                fflush(stdout);
                while (rdchk(iofd)) {
#ifdef SVR2
                        switch (checked)
#else
                        switch (readline(1))
#endif
                        {
                        case CAN:
                        case ZPAD:
#ifdef TCFLSH
                                ioctl(iofd, TCFLSH, 1);
#endif
                                /* zcrce - dinna wanna start a ping-pong game */
                                zsdata(txbuf, 0, ZCRCE);
                                goto waitack;
                        }
                }
#endif
        } while (c == blklen);
        if ( !Fromcu)
                signal(SIGINT, SIG_IGN);

        for (;;) {
                stohdr(Txpos);
                zsbhdr(ZEOF, Txhdr);
                switch (getinsync()) {
                case ZACK:
                        continue;
                case ZRPOS:
                        goto somemore;
                case ZRINIT:
                        return OK;
                case ZSKIP:
                        fclose(in);
                        return c;
                default:
                        fclose(in);
                        return ERROR;
                }
        }
}

/*
 * Respond to receiver's complaint, get back in sync with receiver
 */
getinsync()
{
        register c;

        for (;;) {
                if (Testattn) {
                        printf("\r\n\n\n***** Signal Caught *****\r\n");
                        Rxpos = 0; c = ZRPOS;
                } else
                        c = zgethdr(Rxhdr, 0);
                switch (c) {
                case ZCAN:
                case ZABORT:
                case ZFIN:
                case TIMEOUT:
                        return ERROR;
                case ZRPOS:
                        if (Lastc >= 0 && Lastread == Rxpos) {
                                Dontread = TRUE;
                        } else {
                                clearerr(in);   /* In case file EOF seen */
                                fseek(in, Rxpos, 0);
                        }
                        Txpos = Rxpos;
                        return c;
                case ZACK:
                        return c;
                case ZRINIT:
                case ZSKIP:
                        fclose(in);
                        return c;
                case ERROR:
                default:
                        zsbhdr(ZNAK, Txhdr);
                        continue;
                }
        }
}
/* Say "bibi" to the receiver, try to do it cleanly */
saybibi()
{
        for (;;) {
                stohdr(0L);
                zsbhdr(ZFIN, Txhdr);
                switch (zgethdr(Rxhdr, 0)) {
                case ZFIN:
                        BufferChar('O'); BufferChar('O'); flushmo();
                case ZCAN:
                case TIMEOUT:
                        return;
                }
        }
}

/* Local screen character display function */
bttyout(c)
{
        if (Verbose)
                putc(c, stderr);
}

/* Send command and related info */
zsendcmd(buf, blen)
char *buf;
{
        register c, errors;
        long cmdnum;

        cmdnum = getpid();
        errors = 0;
        for (;;) {
                stohdr(cmdnum);
                Txhdr[ZF0] = Cmdack1;
                zsbhdr(ZCOMMAND, Txhdr);
                zsdata(buf, blen, ZCRCW);
listen:
                Rxtimeout = 100;                /* Ten second wait for resp. */
                c = zgethdr(Rxhdr, 1);

                switch (c) {
                case ZRINIT:
                        continue;
                case ERROR:
                case TIMEOUT:
                        if (++errors > Cmdtries)
                                return ERROR;
                        continue;
                case ZCAN:
                case ZABORT:
                case ZFIN:
                case ZSKIP:
                case ZRPOS:
                        return ERROR;
                default:
                        if (++errors > 10)
                                return ERROR;
                        continue;
                case ZCOMPL:
                        Exitcode = Rxpos;
                        saybibi();
                        return OK;
                case ZRQINIT:
                        vfile("******** RZ *******");
                        system("rz");
                        vfile("******** SZ *******");
                        goto listen;
                }
        }
}

/*
 * If called as sb use YMODEM protocol
 */
chkinvok(s)
char *s;
{
        register char *p;

        p = s;
        while (*p == '-')
                s = ++p;
        while (*p)
                if (*p++ == '/')
                        s = p;
        if (*s == 'v') {
                Verbose=1; ++s;
        }
        Progname = s;
        if (s[0]=='s' && s[1]=='b') {
                Nozmodem = TRUE; blklen=KSIZE;
        }
}

SHAR_EOF
#       End of shell archive
exit 0
