#! /usr/bin/env python
from scapy.all import *
data = 'Hello world'
src_ip = "20.20.20.1"
dst_ip = "10.10.10.1"
dst_mac = "b4:a9:fc:1c:be:be"
_src_mac =  ":21:5F:25:42:"
dport = 3323
sport = 3333
i=0
mac_lsb_list = ['6A','6B','6C','6D','6E','6F','70','71','72','73']
while i < 500:
 src_mac = mac_lsb_list[(10-i)%10]+_src_mac + mac_lsb_list[i%10]
 print src_mac
# p = sendp(Ether(src=src_mac,dst=dst_mac)/IP(src=src_ip, dst=dst_ip)/ICMP(),iface="bond500")
 p = sendp(Ether(src=src_mac,dst=dst_mac)/IP(src=src_ip, dst=dst_ip)/ICMP(),iface="swp6")
# p = sendpfast(Ether(src=src_mac,dst=dst_mac)/IP(src=src_ip, dst=dst_ip)/ICMP(),iface="bond500")
# i+=1
