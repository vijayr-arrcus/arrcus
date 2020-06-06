#!/usr/bin/python
from scapy.all import *

count=10
dst_ip="10.10.10.2"
src_ip="10.10.10.1"
i = 1
while i < count:
    str = "HelloWorld - %d"%(i)
    p = send(IP(src=src_ip, dst=dst_ip)/ICMP()/"HelloWorld")
    print("pkt ",i," sent")
    i+=1

