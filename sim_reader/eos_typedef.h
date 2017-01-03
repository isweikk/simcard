#ifndef __EOS_TYPEDEF_H
#define __EOS_TYPEDEF_H

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif


typedef void _VOID;

typedef char _CHAR8;
typedef unsigned char _UCHAR8;
typedef signed char _SCHAR8;

typedef int _INT32;
typedef unsigned int _UINT32;
typedef signed int _SINT32;

typedef short _SHORT16;
typedef unsigned short _USHORT16;
typedef signed short _SSHORT16;

typedef long _LONG32;
typedef unsigned long _ULONG32;
typedef signed long _SLONG32;

typedef float _FLOAT32;
typedef double _DOUBLE64;

#if 0//(EJOIN_OS_TYPE == EJOIN_OS_TYPE_WIN32)
typedef LONGLONG    _LLONG64;
typedef ULONGLONG    _ULLONG64;
#else
typedef long long _LLONG64;
typedef unsigned long long  _ULLONG64;
#endif


#define EOS_INT8_MAX        127
#define EOS_INT8_MIN        (-127 - (-1))

#define EOS_UINT8_MAX        255
#define EOS_UINT8_MIN        0

#define EOS_INT16_MAX        32767
#define EOS_INT16_MIN        ((-32767) - (-1))

#define EOS_UINT16_MAX        65535
#define EOS_UINT16_MIN        0

#define EOS_INT32_MAX        2147483647
#define EOS_INT32_MIN        (-2147483647 - (-1))

#define EOS_UINT32_MAX        4294967295
#define EOS_UINT32_MIN        0

#define EOS_INT64_MAX        (9223372036854775807)
#define EOS_INT64_MIN        (-9223372036854775807 - (-1))

#define EOS_UINT64_MAX        18446744073709551615
#define EOS_UINT64_MIN        0

#ifndef NULL
#define NULL 0
#endif

#ifndef OK
#define OK      0
#endif

#ifndef ERROR
#define ERROR   0xffffffff
#endif

#define    __STATIC__    static
#define    __CONST__    const
#define    __FAST__    register
#define    __EXTERN__    extern

/* EOS function return value definiation    */
#define    EOS_OK        0L
#define    EOS_FAIL    1L
#define    EOS_TRUE    1L
#define    EOS_FALSE    0L
#define    EOS_ERROR    0xffffffff

typedef _INT32    EOSBool;
typedef    _INT32    _EOSBool;
typedef    _INT32    _BOOL;

#if 1//(EJOIN_OS_TYPE != EJOIN_OS_TYPE_VXWORKS)

#ifndef     BOOL
#define     BOOL    _BOOL
#endif

#ifndef     TRUE
#define     TRUE    EOS_TRUE
#endif

#ifndef     FALSE
#define     FALSE   EOS_FALSE
#endif

#endif

#define    EOS_TRUE    1L
#define    EOS_FALSE    0L



#define EOS_SIZET_TYPE   size_t

typedef EOS_SIZET_TYPE EOSSize_t;


#if 1//(EOS_ARCH_ENDIAN == EOS_ARCH_LITTLE_ENDIAN)

#define eosConvertHostToNetwork32(_host) \
      ((((_host) & 0x000000ff) << 24) | \
             (((_host) & 0x0000ff00) <<  8) | \
             (((_host) & 0x00ff0000) >>  8) | \
             (((_host) & 0xff000000) >> 24))
#define eosConvertHostToNetwork16(_host) \
       ((((_host) & 0x00ff) << 8) | \
             (((_host) & 0xff00) >> 8))

#define eosConvertNetworkToHost32(_network) \
    ((((_network) & 0x000000ff) << 24) | \
             (((_network) & 0x0000ff00) <<  8) | \
             (((_network) & 0x00ff0000) >>  8) | \
             (((_network) & 0xff000000) >> 24))
#define eosConvertNetworkToHost16(_network) \
    ((((_network) & 0x00ff) << 8) | \
             (((_network) & 0xff00) >> 8))

#elif (EOS_ARCH_ENDIAN == EOS_ARCH_BIG_ENDIAN)

#define eosConvertHostToNetwork32(_host) (_host)
#define eosConvertNetworkToHost32(_network) (_network)
#define eosConvertHostToNetwork16(_host) (_host)
#define eosConvertNetworkToHost16(_network) (_network)

#endif

#define EOS_GET_STRUCT(_structTypedef, _fieldName, _fieldPtr) \
    ((_structTypedef *)(((_CHAR8*)(_fieldPtr))-offsetof(_structTypedef,_fieldName)))



#define EOS_NELEMENTS(array)        (sizeof (array) / sizeof ((array) [0]))

#define EOS_MAX(x, y)    (((x) < (y)) ? (y) : (x))
#define EOS_MIN(x, y)    (((x) < (y)) ? (x) : (y))

#define EOS_ROUND_UP(x, align)    (((_INT32) (x) + (align - 1)) & ~(align - 1))
#define EOS_ROUND_DOWN(x, align)    ((_INT32)(x) & ~(align - 1))
#define EOS_ALIGNED(x, align)    (((_INT32)(x) & (align - 1)) == 0)


#define EOS_ALIGN16(_x) (EOS_ROUND_UP(_x,2))
#define EOS_ALIGN32(_x) (EOS_ROUND_UP(_x,4))
#define EOS_ALIGN64(_x) (EOS_ROUND_UP(_x,8))


#define EOS_ALIGN_DATASIZE        (sizeof(_CHAR8))
#define EOS_ALIGN_PRTSIZE        (sizeof(_VOID *))
#define EOS_ALIGN_SIZE            (EOS_MAX(EOS_ALIGN_DATASIZE, EOS_ALIGN_PRTSIZE))


#define EOS_DECLARE_HANDLE(_handleName) \
        typedef struct { _VOID* pUnused; } _handleName##__ ; \
        typedef __CONST__ _handleName##__ * _handleName; \
        typedef _handleName*  LP##_handleName

#define EOS_UNUSED_ARG(_arg) if (_arg);
#define IN
#define OUT
#define INOUT
        

#if defined(__cplusplus)
}
#endif 
#endif
