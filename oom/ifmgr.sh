#!/bin/bash
rm /tmp/asan.log
touch /tmp/asan.log
chmod 777 /tmp/asan.log
export ASAN_OPTIONS="log_path=/tmp/asan.log:verbosity=1:abort_on_error=1:disable_coredump=0:unmap_shadow_on_exit=1"
ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer /usr/lib/arcos/ifmgr.bin
