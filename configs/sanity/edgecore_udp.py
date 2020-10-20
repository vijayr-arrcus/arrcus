#!/usr/bin/python
from scapy.all import *

count=50
dst_ip="20.1.1.3"
src_ip="30.1.1.2"
src_mac="68:21:5f:25:42:6a"
dst_mac="b4:a9:fc:1c:be:be"
data = 'Hello world hello world hello world zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz'
i = 1
while i < count:
    str = "HelloWorld - %d"%(i)
    p = sendp(Ether()/IP(src=src_ip, dst=dst_ip, tos=184)/ICMP(), iface="bond30.60")
    print("pkt ",i," sent")
    i+=1

i=0
j=0
mac_lsb_list = ['6A','6B','6C','6D','6E','6F','70','71','72','73']
while j < 500:
# src_mac = mac_lsb_list[(10-i)%10]+_src_mac + mac_lsb_list[i%10]
 src_mac = _src_mac
 dport = random.randint(0,65535)
 sport = random.randint(0,65535)
 p = Ether(src=src_mac,dst=dst_mac) /IP(src=src_ip, dst=dst_ip, tos=184) / UDP(sport=sport, dport=dport)/data
 sendp(p, iface="bond30.60",count=random.randint(0,500))
 j += 1

