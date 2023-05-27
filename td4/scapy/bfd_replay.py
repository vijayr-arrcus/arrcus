#!/usr/bin/python

from scapy.all import *
scapy_cap = rdpcap('bfd.pcap')
for packet in scapy_cap:
    packet.show()
packet=scapy_cap[0]
packet.show()

send(packet, iface="swp37", count=8888)

