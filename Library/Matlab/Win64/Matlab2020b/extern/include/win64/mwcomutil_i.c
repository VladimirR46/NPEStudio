

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for win64\mwcomutil.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=IA64 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

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

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IMWUtil,0xC47EA90E,0x56D1,0x11d5,0xB1,0x59,0x00,0xD0,0xB7,0xBA,0x75,0x44);


MIDL_DEFINE_GUID(IID, LIBID_MWComUtil,0xD449FE34,0xDEA4,0x4160,0xA4,0xC9,0x3D,0xAE,0xA9,0x34,0x2D,0x5E);


MIDL_DEFINE_GUID(CLSID, CLSID_MWField,0xEF3D3159,0xC644,0x4251,0x97,0xE4,0x74,0xCA,0x44,0x11,0x1F,0x99);


MIDL_DEFINE_GUID(CLSID, CLSID_MWStruct,0x4A976D9B,0x26BA,0x493E,0xAC,0x71,0x3A,0x1F,0xF6,0xCA,0xB2,0xAC);


MIDL_DEFINE_GUID(CLSID, CLSID_MWComplex,0x32D40F34,0x9A69,0x48C2,0xAE,0x99,0x81,0xFA,0xBE,0x64,0x54,0x6A);


MIDL_DEFINE_GUID(CLSID, CLSID_MWSparse,0xC57A6356,0x8000,0x4D4E,0xB6,0x94,0xE4,0x25,0x77,0xA2,0x0C,0x01);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArg,0x8BD33BE4,0x3974,0x4E93,0x8A,0x03,0x08,0xC7,0xFE,0x68,0xE5,0x8B);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArrayFormatFlags,0x3C404D03,0x511E,0x42FF,0xA9,0xEA,0xF3,0xF2,0x98,0x23,0x55,0xD7);


MIDL_DEFINE_GUID(CLSID, CLSID_MWDataConversionFlags,0x24D0195F,0x1A84,0x4F09,0xAF,0x99,0xD0,0xFE,0x59,0x3D,0xA5,0x2B);


MIDL_DEFINE_GUID(CLSID, CLSID_MWUtil,0x56CEE3BB,0x1DF1,0x4347,0x91,0xAB,0x5F,0x46,0x58,0x8A,0x2B,0x60);


MIDL_DEFINE_GUID(CLSID, CLSID_MWFlags,0x00AC180B,0xF113,0x4D75,0x85,0x49,0xFD,0xC5,0x60,0xEA,0x79,0x75);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



