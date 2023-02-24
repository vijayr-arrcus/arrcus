#! /usr/bin/env python
from scapy.all import *
data = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
src_mac = "00:00:00:00:00:07"
dst_mac = "00:00:00:00:00:03"
src_ip = "33.33.33.3"
dst_ip = "33.33.33.1"
dport = 3323
sport = 3333
vlan =77
#p = Ether(src=src_mac, dst=dst_mac) / IP(src=src_ip, dst=dst_ip) / UDP(sport=sport, dport=dport)/data
p = Ether(src=src_mac, dst=dst_mac) / Dot1Q(vlan=vlan)/IP(src=src_ip, dst=dst_ip) / UDP(sport=sport, dport=dport)/data
#p = Ether(src=src_mac, dst=dst_mac) /IP(src=src_ip, dst=dst_ip) / UDP(sport=sport, dport=dport)/data
k = chexdump( p , dump=True)
a  = k.replace( ", 0x", "" )
print a
