/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 3.00.08 */
/* at Wed Nov 22 13:35:26 1995
 */
/* Compiler settings for comcat.idl:
    Os, W1, Zp8, env=Win32, ms_ext, app_config, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef IID_DEFINED
#define IID_DEFINED

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // IID_DEFINED

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IEnumGUID = {0x40FC6ECD,0x2438,0x11cf,{0xA3,0xDB,0x08,0x00,0x36,0xF1,0x25,0x02}};


const IID IID_IEnumCATEGORYINFO = {0x40FC6ECE,0x2438,0x11cf,{0xA3,0xDB,0x08,0x00,0x36,0xF1,0x25,0x02}};


const IID IID_ICatRegister = {0x40FC6ECF,0x2438,0x11cf,{0xA3,0xDB,0x08,0x00,0x36,0xF1,0x25,0x02}};


const IID IID_ICatInformation = {0x40FC6ED0,0x2438,0x11cf,{0xA3,0xDB,0x08,0x00,0x36,0xF1,0x25,0x02}};


const IID LIBID_ComponentCategoryLib = {0x40FC6ED1,0x2438,0x11cf,{0xA3,0xDB,0x08,0x00,0x36,0xF1,0x25,0x02}};


#ifdef __cplusplus
}
#endif

