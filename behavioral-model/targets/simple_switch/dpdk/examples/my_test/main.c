#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include <rte_eal.h>
#include <rte_common.h>
#include <rte_ethdev.h>
#include <rte_log.h>
#include <rte_mbuf.h>


#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250

#define RX_RING_SIZE 512
#define TX_RING_SIZE 512

#define BURST_SIZE 32

static volatile bool force_quit;

#define RTE_LOGTYPE_APP RTE_LOGTYPE_USER1


// 输出设备的mac地址
static void
print_mac(unsigned int port_id)
{
  struct ether_addr dev_eth_addr;
  rte_eth_macaddr_get(port_id, &dev_eth_addr);
    printf("Port %u, MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n\n",
                    (unsigned int) port_id,
                    dev_eth_addr.addr_bytes[0],
                    dev_eth_addr.addr_bytes[1],
                    dev_eth_addr.addr_bytes[2],
                    dev_eth_addr.addr_bytes[3],
                    dev_eth_addr.addr_bytes[4],
                    dev_eth_addr.addr_bytes[5]);
}

// 将pkt中的源mac地址和目的mac地址交换
static void
mac_swap(struct rte_mbuf **bufs, uint16_t nb_mbufs)
{
  struct ether_hdr *eth;
  struct ether_addr tmp;
  struct rte_mbuf *m;
  uint16_t buf;

  for (buf = 0; buf < nb_mbufs; buf++) {
    m = bufs[buf];
    eth = rte_pktmbuf_mtod(m, struct ether_hdr *);
    ether_addr_copy(&eth->s_addr, &tmp);
    ether_addr_copy(&eth->d_addr, &eth->s_addr);
    ether_addr_copy(&tmp, &eth->d_addr);
  }
}

// 用于检查端口连接状态
static int
check_link_status(uint16_t nb_ports)
{
  struct rte_eth_link link;
  uint8_t port;

  for (port = 0; port < nb_ports; port++) {
    rte_eth_link_get(port, &link);

    if (link.link_status == ETH_LINK_DOWN) {
      RTE_LOG(INFO, APP, "Port: %u Link DOWN\n", port);
      return -1;
    }

    RTE_LOG(INFO, APP, "Port: %u Link UP Speed %u\n",
      port, link.link_speed);
  }

  return 0;
}

// 在程序运行结束后统计收发包信息
static void
print_stats(void)
{
  struct rte_eth_stats stats;
  uint8_t nb_ports = rte_eth_dev_count();
  uint8_t port;

  for (port = 0; port < nb_ports; port++) {
    printf("\nStatistics for port %u\n", port);
    rte_eth_stats_get(port, &stats);
    printf("Rx:%9"PRIu64" Tx:%9"PRIu64" dropped:%9"PRIu64"\n",
      stats.ipackets, stats.opackets, stats.imissed);
  }
}

// 处理中断信号，统计信息并终止程序
static void
signal_handler(int signum)
{
  if (signum == SIGINT || signum == SIGTERM) {
    printf("\n\nSignal %d received, preparing to exit...\n",
        signum);
    force_quit = true;
    print_stats();
  }
}

// 初始化设备端口，配置收发队列
static inline int
port_init(uint8_t port, struct rte_mempool *mbuf_pool)
{
  struct rte_eth_conf port_conf;
  port_conf.link_speeds = ETH_LINK_SPEED_1G;
  port_conf.rxmode.max_rx_pkt_len = ETHER_MAX_LEN;
  const uint16_t nb_rx_queues = 1;
  const uint16_t nb_tx_queues = 1;
  int ret;
  uint16_t q;

  // 配置设备
  ret = rte_eth_dev_configure(port,
      nb_rx_queues,
      nb_tx_queues,
      &port_conf);
  if (ret != 0)
    return ret;

  // 配置收包队列
  for (q = 0; q < nb_rx_queues; q++) {
    ret= rte_eth_rx_queue_setup(port, q, RX_RING_SIZE,
        rte_eth_dev_socket_id(port),
        NULL, mbuf_pool);
    if (ret < 0)
      return ret;
  }

  // 配置发包队列
  for (q = 0; q < nb_tx_queues; q++) {
    ret= rte_eth_tx_queue_setup(port, q, TX_RING_SIZE,
        rte_eth_dev_socket_id(port),
        NULL);
    if (ret < 0)
      return ret;
  }

  // 启动设备
  ret = rte_eth_dev_start(port);
  if (ret < 0)
    return ret;

  // 开启混杂模式
  rte_eth_promiscuous_enable(port);

  return 0;
}

int lcore_main(void *arg)
{
  unsigned int lcore_id = rte_lcore_id();
  const uint8_t nb_ports = rte_eth_dev_count();
  uint8_t port;
  uint16_t buf;

  RTE_LOG(INFO, APP, "lcore %u running\n", lcore_id);

  while (!force_quit) {
    for (port = 0; port < nb_ports; port++) {
      struct rte_mbuf *bufs[BURST_SIZE];
      
      // 接受数据包
      const uint16_t nb_rx = rte_eth_rx_burst(port, 0,
          bufs, BURST_SIZE);

      if (unlikely(nb_rx == 0))
        continue;

      // 交换mac地址
      mac_swap(bufs, nb_rx);

      // 发送数据包
      const uint16_t nb_tx = rte_eth_tx_burst(port, 0,
          bufs, nb_rx);

      if (unlikely(nb_tx < nb_rx)) {
        for (buf = nb_tx; buf < nb_rx; buf++)
          rte_pktmbuf_free(bufs[buf]);
      }
    }
  }
  RTE_LOG(INFO, APP, "lcore %u exiting\n", lcore_id);
  return 0;
}


int main(int argc, char *argv[])
{
  int ret;
  uint8_t port;
  uint8_t nb_ports;
  struct rte_mempool *mbuf_pool;
  uint8_t portid;
  
  // 初始化DPDK
  ret = rte_eal_init(argc, argv);
  if (ret < 0)
    rte_exit(EXIT_FAILURE, "EAL Init failed\n");

  argc -= ret;
  argv += ret;

  // 注册中断信号处理函数
  force_quit = false;
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  nb_ports = rte_eth_dev_count();

  for (port = 0; port < nb_ports; port++) {
    print_mac(port);
  }

  RTE_LOG(INFO, APP, "Number of ports:%u\n", nb_ports);

  // 申请mbuf内存池
  mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL",
    NUM_MBUFS * nb_ports,
    MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
    rte_socket_id());

  if (mbuf_pool == NULL)
    rte_exit(EXIT_FAILURE, "mbuf_pool create failed\n");

  // 配置设备
  for (portid = 0; portid < nb_ports; portid++)
      if (port_init(portid, mbuf_pool) != 0)
        rte_exit(EXIT_FAILURE, "port init failed\n");

  // 检查连接状态
  ret = check_link_status(nb_ports);
  if (ret < 0)
    RTE_LOG(WARNING, APP, "Some ports are down\n");				
  
  // 线程核心绑定，循环处理数据包
  rte_eal_mp_remote_launch(lcore_main, NULL, SKIP_MASTER);

  rte_eal_mp_wait_lcore();

}
