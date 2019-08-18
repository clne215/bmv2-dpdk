/* Copyright 2013-present Barefoot Networks, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Antonin Bas (antonin@barefootnetworks.com)
 *
 */

/* Switch instance */

#include <bm/config.h>
#include <signal.h>
#include <mutex>
#include <unistd.h>

#include "dpdk_api.h"

#include <bm/SimpleSwitch.h>
#include <bm/bm_runtime/bm_runtime.h>
#include <bm/bm_sim/options_parse.h>
#include <bm/bm_sim/target_parser.h>
#include <bm/bm_sim/packet.h>

#include "simple_switch.h"

static volatile bool force_quit;
#define BURST_SIZE 32
namespace {
SimpleSwitch *simple_switch;
using Mutex = std::mutex;

using Lock = std::unique_lock<Mutex>;
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
      //print_stats();
    }
  }
  
  //handler packets
  int lcore_main(void *arg)
  {
    unsigned int lcore_id = rte_lcore_id();
    const uint8_t nb_ports = rte_eth_dev_count();
    uint32_t port;
    uint16_t buf;
    struct rte_mbuf *m;
    uint16_t packet_len;
    char *s_data_buffer;

    while (!force_quit) {
      for (port = 0; port < nb_ports; port++) {
        struct rte_mbuf *bufs[BURST_SIZE];
        // 接受数据包
        const uint16_t nb_rx = rte_eth_rx_burst(port, 0,
            bufs, BURST_SIZE);

        if (unlikely(nb_rx == 0))
          continue;
        
        for (buf = 0; buf < nb_rx; ++buf) {
          m = bufs[buf];
          packet_len = rte_ctrlmbuf_len(m);
          s_data_buffer = rte_pktmbuf_mtod(m, char*);
          //simpleswitch receive packets
          simple_switch->receive_(port, s_data_buffer, (int)packet_len);
          
        }
        //free buffer
        for (buf = 0; buf < nb_rx; buf++) {
          rte_pktmbuf_free(bufs[buf]);
        }
      }
    }
    
    return 0;
  }
  
}  // namespace

namespace sswitch_runtime {
shared_ptr<SimpleSwitchIf> get_handler(SimpleSwitch *sw);
}  // namespace sswitch_runtime

int
main(int argc, char* argv[]) {
  bm::TargetParserBasicWithDynModules simple_switch_parser;
  simple_switch_parser.add_flag_option(
      "enable-swap",
      "enable JSON swapping at runtime");
  simple_switch_parser.add_uint_option(
      "drop-port",
      "choose drop port number (default is 511)");

  bm::OptionsParser parser;
  parser.parse(argc, argv, &simple_switch_parser);

  bool enable_swap_flag = false;
  if (simple_switch_parser.get_flag_option("enable-swap", &enable_swap_flag)
      != bm::TargetParserBasic::ReturnCode::SUCCESS) {
    std::exit(1);
  }

  uint32_t drop_port = 0xffffffff;
  {
    auto rc = simple_switch_parser.get_uint_option("drop-port", &drop_port);
    if (rc == bm::TargetParserBasic::ReturnCode::OPTION_NOT_PROVIDED)
      drop_port = SimpleSwitch::default_drop_port;
    else if (rc != bm::TargetParserBasic::ReturnCode::SUCCESS)
      std::exit(1);
  }

  simple_switch = new SimpleSwitch(enable_swap_flag, drop_port);

  int status = simple_switch->init_from_options_parser(parser);
  if (status != 0) std::exit(status);

  int thrift_port = simple_switch->get_runtime_port();
  bm_runtime::start_server(simple_switch, thrift_port);
  using ::sswitch_runtime::SimpleSwitchIf;
  using ::sswitch_runtime::SimpleSwitchProcessor;
  bm_runtime::add_service<SimpleSwitchIf, SimpleSwitchProcessor>(
      "simple_switch", sswitch_runtime::get_handler(simple_switch));
  simple_switch->start_and_return();
  //dpdk init
  dpdk_api_init(argc, argv);
  //init quit function
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  // 线程核心绑定，循环处理数据包
  rte_eal_mp_remote_launch(lcore_main, NULL, SKIP_MASTER);
  rte_eal_mp_wait_lcore();
  std::this_thread::sleep_for(std::chrono::seconds(5));
  print_stats();
  //while (true) std::this_thread::sleep_for(std::chrono::seconds(100));

  return 0;
}
