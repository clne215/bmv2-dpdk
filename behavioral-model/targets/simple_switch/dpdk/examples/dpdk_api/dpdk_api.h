#ifndef _DPDK_API_H_
#define _DPDK_API_H_

#include <stdio.h>
#include <stdint.h>


#include <rte_eal.h>
#include <rte_ethdev.h>

#ifdef __cplusplus
extern "C" {
#endif
//send packet
int dpdk_api_send_packet(uint32_t port, char* data, uint16_t packet_len);

//init dpdk
int dpdk_api_init(int argc, char **argv);


#ifdef __cplusplus  
};  
#endif

#endif
