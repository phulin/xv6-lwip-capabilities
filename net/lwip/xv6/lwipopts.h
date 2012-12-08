#define PPP_SUPPORT 1

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#define PBUF_LINK_HLEN  14

#define TCP_MSS         512
#define TCP_SND_BUF     2048
#define TCP_WND         8192
#define TCP_SND_QUEUELEN    16 * TCP_SND_BUF/TCP_MSS
#define MEM_SIZE        4096 * 16
#define PBUF_POOL_SIZE  256
#define MEMP_NUM_TCP_SEG    64

#define LWIP_STATS      1

#define LWIP_COMPAT_SOCKETS 1

#define DBG_FULL            DBG_ON | DBG_LEVEL_SEVERE

#define LWIP_DEBUG          1
#define NETIF_DEBUG         DBG_OFF
#define SYS_DEBUG           DBG_OFF
#define ETHARP_DEBUG        DBG_OFF
#define PBUF_DEBUG          DBG_OFF
#define TCPIP_DEBUG         DBG_OFF
#define SOCKETS_DEBUG       DBG_OFF
#define TCP_OUTPUT_DEBUG    DBG_OFF
#define TCP_INPUT_DEBUG     DBG_OFF
#define TCP_RST_DEBUG       DBG_OFF
#define ICMP_DEBUG          DBG_OFF
#define IP_DEBUG            DBG_OFF
#define UDP_DEBUG           DBG_OFF
#define TCP_DEBUG           DBG_OFF
#define TCP_QLEN_DEBUG      DBG_OFF

#define DBG_TYPES_ON 0xff


