#!/bin/bash
env LD_PRELOAD="/usr/lib/x86_64-linux-gnu/libtcmalloc.so" HEAPPROFILE=/tmp/mem_trace/bgp_mem_trace/gperf/bgp HEAPPROFILESIGNAL=12 HEAP_PROFILE_MMAP=true /usr/lib/arcos/bgp.bin
