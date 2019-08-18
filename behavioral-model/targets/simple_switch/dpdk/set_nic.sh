#!/bin/bash

ifconfig enp0s3 down
#ifconfig enp0s8 down
modprobe uio
insmod build/kmod/igb_uio.ko
python tools/dpdk-devbind.py --bind=igb_uio enp0s3
#python tools/dpdk-devbind.py --bind=igb_uio enp0s8
