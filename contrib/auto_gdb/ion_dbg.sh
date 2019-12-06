#!/bin/bash
# use testnet settings,  if you need mainnet,  use ~/.ioncoin/iond.pid file instead
ion_pid=$(<~/.ioncoin/testnet3/iond.pid)
sudo gdb -batch -ex "source debug.gdb" iond ${ion_pid}
