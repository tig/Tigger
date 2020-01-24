/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 3.00.44 */
/* at Mon Oct 14 22:46:54 1996
 */
/* Compiler settings for Pager.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IPager = {0x23208895,0x15BF,0x11D0,{0x97,0xFB,0x00,0xAA,0x00,0x1F,0x73,0xC1}};


const IID LIBID_PagerLib = {0x23208893,0x15BF,0x11D0,{0x97,0xFB,0x00,0xAA,0x00,0x1F,0x73,0xC1}};


const CLSID CLSID_Pager = {0x23208899,0x15BF,0x11D0,{0x97,0xFB,0x00,0xAA,0x00,0x1F,0x73,0xC1}};


#ifdef __cplusplus
}
#endif

