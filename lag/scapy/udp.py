#! /usr/bin/env python
from scapy.all import *
import math

data = 'Hello world hello world hello world zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz'
src_ip = "20.20.20.1"
dst_ip = "10.10.10.1"
dst_mac = "b4:a9:fc:1c:be:be"
_src_mac =  ":21:5F:25:42:"
_dport = 22
_sport = 33
i=0
mac_lsb_list = ['6A','6B','6C','6D','6E','6F','70','71','72','73']
while i < 500:
 src_mac = mac_lsb_list[(10-i)%10]+_src_mac + mac_lsb_list[i%10]
 sport = math.floor(_sport * 1.5 + i)
 dport = math.floor(_dport * 1.75 + i)
 p = Ether(src=src_mac,dst=dst_mac) /IP(src=src_ip, dst=dst_ip) / UDP(sport=sport, dport=dport)/data
 sendp(p, iface="bond500")
 i+=1
