cmd_base/vnic_wq.o = gcc -Wp,-MD,base/.vnic_wq.o.d.tmp  -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE  -I/home/qs/Desktop/dpdk-stable-16.11.9/build/include -include /home/qs/Desktop/dpdk-stable-16.11.9/build/include/rte_config.h -I/home/qs/Desktop/dpdk-stable-16.11.9/drivers/net/enic/base/ -I/home/qs/Desktop/dpdk-stable-16.11.9/drivers/net/enic -O3 -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wformat-nonliteral -Wformat-security -Wundef -Wwrite-strings -Wno-strict-aliasing    -o base/vnic_wq.o -c /home/qs/Desktop/dpdk-stable-16.11.9/drivers/net/enic/base/vnic_wq.c 