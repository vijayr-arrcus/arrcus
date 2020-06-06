#!/usr/bin/python
from scapy.all import *

count=50
dst_ip="2.2.2.1"
src_ip="3.3.3.1"
src_mac="00:a0:c9:00:00:01"
dst_mac="b4:a9:fc:1c:be:be"
i = 1
while i < count:
    str = "HelloWorld - %d"%(i)
    p = sendp(Ether()/IP(src=src_ip, dst=dst_ip, tos=184)/ICMP(), iface="swp55")
    print("pkt ",i," sent")
    i+=1
