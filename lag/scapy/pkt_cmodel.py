#! /usr/bin/env python
from scapy.all import *
data = '123456789'
src_mac = "00:00:00:00:00:07"
dst_mac = "00:00:00:00:00:03"
src_ip = "33.33.33.2"
dst_ip = "34.33.33.1"
dport = 3323
sport = 3333
vlan = 50
#p = Ether(src=src_mac, dst=dst_mac) / Dot1Q(vlan=vlan)/IP(src=src_ip, dst=dst_ip) / UDP(sport=sport, dport=dport)/data
p = (Dot1Q(vlan=vlan)/IP(src=src_ip, dst=dst_ip) / ICMP())
sendp(p)
k = chexdump( p , dump=True)
a  = k.replace( ", 0x", "" )
print a
