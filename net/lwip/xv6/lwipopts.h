#define LWIP_COMPAT_MUTEX 1
#define LWIP_COMPAT_SOCKETS 0
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1
#endif


#ifndef BIG_ENDIAN
#define BIG_ENDIAN 2
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

