#! /usr/bin/env python
from scapy.all import *
import math
import random

data = 'Hello world hello world hello world zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz'
src_ip = "1.1.1.1"
dst_ip = "1.1.1.2"
dst_mac = "b4:a9:fc:1c:be:be"
_src_mac =  ":21:5F:25:42:"
_src_mac =  "68:21:5F:25:42:6A"
_dport = 22
_sport = 33
i=0
j=0
mac_lsb_list = ['6A','6B','6C','6D','6E','6F','70','71','72','73']
while j < 500:
# src_mac = mac_lsb_list[(10-i)%10]+_src_mac + mac_lsb_list[i%10]
 src_mac = _src_mac
# dport = random.randint(0,65535)
 dport = random.randint(6784)
 sport = random.randint(0,65535)
# p = Ether(src=src_mac,dst=dst_mac) /IP(src=src_ip, dst=dst_ip) / UDP(sport=sport, dport=dport)/data
 p = Ether() /IP(src=src_ip, dst=dst_ip, ttl=255, tos=192) / UDP(sport=sport, dport=dport)/data
 num = random.randint(0,500)
 sendp(p, iface="bond30",count=num)
 i+=1
