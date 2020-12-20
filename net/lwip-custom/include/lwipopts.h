#ifndef XV6_X86_64_LWIPOPTS_H
#define XV6_X86_64_LWIPOPTS_H

#include "lwip/debug.h"

// ref. https://lwip.fandom.com/wiki/Porting_for_an_OS#cc.h
#define ETH_PAD_SIZE 2
#define MEM_ALIGNMENT 4

// We don't provide errno.h, so use it defined in lwip.
// See net/lwip/include/lwip/errno.h
#define LWIP_PROVIDE_ERRNO 1

// Enable debug if LWIP_DEBUG is defined
// We have to enable debug for each functions?
#ifdef LWIP_DEBUG
#define ETHARP_DEBUG LWIP_DBG_ON
#define NETIF_DEBUG LWIP_DBG_ON
#define PBUF_DEBUG LWIP_DBG_ON
#define API_LIB_DEBUG LWIP_DBG_ON
#define API_MSG_DEBUG LWIP_DBG_ON
#define SOCKETS_DEBUG LWIP_DBG_ON
#define ICMP_DEBUG LWIP_DBG_ON
#define IGMP_DEBUG LWIP_DBG_ON
#define INET_DEBUG LWIP_DBG_ON
#define IP_DEBUG LWIP_DBG_ON
#define IP_REASS_DEBUG LWIP_DBG_ON
#define RAW_DEBUG LWIP_DBG_ON
#define MEM_DEBUG LWIP_DBG_ON
#define MEMP_DEBUG LWIP_DBG_ON
#define SYS_DEBUG LWIP_DBG_ON
#define TIMERS_DEBUG LWIP_DBG_ON
#define TCP_DEBUG LWIP_DBG_ON
#define TCP_INPUT_DEBUG LWIP_DBG_ON
#define TCP_FR_DEBUG LWIP_DBG_ON
#define TCP_RTO_DEBUG LWIP_DBG_ON
#define TCP_CWND_DEBUG LWIP_DBG_ON
#define TCP_WND_DEBUG LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG LWIP_DBG_ON
#define TCP_RST_DEBUG LWIP_DBG_ON
#define TCP_QLEN_DEBUG LWIP_DBG_ON
#define UDP_DEBUG LWIP_DBG_ON
#define TCPIP_DEBUG LWIP_DBG_ON
#define SLIP_DEBUG LWIP_DBG_ON
#define DHCP_DEBUG LWIP_DBG_ON
#define AUTOIP_DEBUG LWIP_DBG_ON
#define ACD_DEBUG LWIP_DBG_ON
#define DNS_DEBUG LWIP_DBG_ON
#define IP6_DEBUG LWIP_DBG_ON
#define DHCP6_DEBUG LWIP_DBG_ON
#endif

#endif /* ifndef XV6_X86_64_LWIPOPTS_H */
