#!/usr/bin/python
from scapy.all import *

data = 'Hello world hello world hello world zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz'
count=50
dst_ip="30.1.1.2"
src_ip="20.1.1.3"
src_mac="00:e0:ec:cb:37:b1"
dst_mac="b4:a9:fc:1c:be:be"
i = 1
while i < count:
    str = "HelloWorld - %d"%(i)
    p = sendp(Ether()/IP(src=src_ip, dst=dst_ip, tos=184)/ICMP(), iface="bond20")
    print("pkt ",i," sent")
    i+=1

i=0
j=0
while j < 500:
 dport = random.randint(0,65535)
 sport = random.randint(0,65535)
 p = Ether(src=src_mac,dst=dst_mac) /IP(src=src_ip, dst=dst_ip, tos=184) / UDP(sport=sport, dport=dport)/data
 sendp(p, iface="bond20",count=random.randint(0,500))
 j+=1
