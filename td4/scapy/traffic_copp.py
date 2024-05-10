#!/usr/bin/python
from scapy.all import *
from scapy.contrib.ospf import *

data="hello world 123456789123456789123456789123456789123456789"
src_ip="10.10.10.3"
dst_ip="10.10.10.2"
dst_ip6="1140:1::2"
src_ip6="1140:1::1"
dmac="b4:a9:fc:70:c3:d6"
smac="5c:07:58:a4:e0:03"
interface="vlan100"

def send_packet (p, intf, t_count):
    i = 0;
    p.show()
    k = chexdump(p , dump=True)
    a  = k.replace(", 0x", "")
    print a
    raw_input("Press key to continue")
    sendp(p, iface=intf, count=t_count)

def send_ip_v6_traffic ():
    # IP/IP6
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)
    return pkt

def send_ip_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)
    return pkt

def send_lldp_traffic ():
    mac_lldp_multicast = '01:80:c2:00:00:0e'
    chassis = bytearray(7)
    chassis[0:3] = (0x02, 0x06, 0x07)
    chassis[3:] = str.encode('fakey', 'utf-8')
    sysname = bytearray(7)
    sysname[0:2] = (0x0a, 0x05)
    sysname[2:] = str.encode('Lies!', 'utf-8')
    sysdesc = bytearray(12)
    sysdesc[0:2] = (0x0c, 0x0a)
    sysdesc[2:] = str.encode('MS-DOS 1.0', 'utf-8')
    portID = bytearray((0x04, 0x07, 0x03, 0x00, 0x01, 0x02, 0xff, 0xfe, 0xfd)) # fake MAC address
    TTL = bytearray((0x06,0x02, 0x00,0x78))
    end = bytearray((0x00, 0x00))
    payload = bytes(chassis + portID + TTL + sysname + sysdesc + end)
    pkt = Ether(src=smac, dst=mac_lldp_multicast, type=0x88cc)/Raw(load=bytes(payload))/Padding(b'\x00\x00\x00\x00')
    return pkt

def send_arp_traffic ():
    pkt = ARP(hwsrc=smac, hwdst= ETHER_BROADCAST, op=ARP.who_has, psrc=src_ip, pdst=dst_ip)
    return pkt;

def send_ospf_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/OSPFv3_Hdr()/OSPFv3_Hello()
    return pkt

def send_ospf_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/OSPF_Hdr()/OSPF_Hello()
    return pkt

def send_bgp_v4_traffic ():
    print("1. dport 179")
    print("2. sport 179")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=179, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(sport=179, dport=10000)
    return pkt

def send_bgp_v6_traffic ():
    print("1. dport 179")
    print("2. sport 179")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=179, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(sport=179, dport=10000)
    return pkt

def send_icmp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/ICMP()
    return pkt

def send_icmp_v6_traffic ():
    print("1. Router Solicitation")
    print("2. Router Advertisement")
    print("3. Neighbor Solicitation")
    print("4. Neighbor Advertisement")
    icmp_sub_text = input("Enter ICMP type > ")
    choice = int(icmp_sub_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/ICMPv6ND_RS()
    if choice == 2:
# ETHER_BROADCAST
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/ICMPv6ND_RA()
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/ICMPv6ND_NS(tgt=dst_ip6)
    if choice == 4:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/ICMPv6ND_NA(tgt=dst_ip6)/ICMPv6NDOptDstLLAddr()
    return pkt

# STP:
def send_stp_traffic ():
    pkt = Ether(src=smac, dst="01:80:C2:00:00:00")/LLC()/STP()
    return pkt

# LACP:
def send_lacp_traffic ():
    pkt = Ether(src=smac, dst="01:80:C2:00:00:02")
    return pkt

# ISIS:
def send_isis_traffic ():
    print("1. dst mac = 01:80:C2:00:00:14")
    print("2. dst mac = 01:80:C2:00:00:15")
    print("3. dst mac = 09:00:2b:00:00:05")
    isis_sub_text = input("Enter isis traffic type > ")
    choice = int(isis_sub_text)
    if choice == 1:
        pkt = Ether(src=smac, dst="01:80:C2:00:00:14")
    if choice == 2:
        pkt = Ether(src=smac, dst="01:80:C2:00:00:15")
    if choice == 3:
        pkt = Ether(src=smac, dst="09:00:2b:00:00:05")
    return pkt

# VRRP:
def send_vrrp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/VRRP()
    return pkt

def send_vrrp_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/VRRP()
    return pkt

def send_trace_route_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=33434, sport=33434)
    return pkt

def send_trace_udp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/Dot1Q(vlan=100)/IP(src=src_ip, dst=dst_ip)/UDP(dport=4789, sport=1646)/data
    return pkt

def send_trace_route_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=33434, sport=33434)
    return pkt

# SSH/TELNET:
def send_ssh_v4_traffic ():
    print("1. dport 21 ")
    print("2. dport 22 ")
    print("3. sport 21 ")
    print("4. sport 22 ")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=21, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=22, sport=10000)
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(sport=21, dport=10000)
    if choice == 4:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(sport=22, dport=10000)
    return pkt


def send_telnet_v4_traffic ():
    print("1. dport 23 ")
    print("2. sport 23 ")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=23, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(sport=10000, dport=23)
    return pkt

def send_radius_v4_traffic ():
    print("1. dport 1812 ")
    print("2. dport 1813 ")
    print("3. dport 1645 ")
    print("4. dport 1646 ")
    print("5. sport 1812 ")
    print("6. sport 1813 ")
    print("7. sport 1645 ")
    print("8. sport 1646 ")
    in_text = input("Enter data type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=1812, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=1813, sport=10000)
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=1645, sport=10000)
    if choice == 4:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=1646, sport=10000)
    if choice == 5:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=1000, sport=1812)
    if choice == 6:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=1000, sport=1813)
    if choice == 7:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=1000, sport=1645)
    if choice == 8:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=1000, sport=1646)
    return pkt

def send_sftp_v6_traffic ():
    print("1. dport 115")
    print("2. sport 115")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=115, sport=1000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=1000, sport=115)
    return pkt

def send_snmp_v4_traffic ():
    print("1. dport 161 ")
    print("2. dport 162 ")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=161, sport=1000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=162, sport=1000)
    return pkt

def send_tacacs_v4_traffic ():
    print("1. dport 49 ")
    print("2. sport 49 ")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=49, sport=1000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=1000, sport=49)
    return pkt

def send_icmp_monitor_v4_traffic ():
    print("1. type 8")
    print("2. type 0")
    print("3. type 3, code = 1")
    print("4. type 3, code = 2")
    print("5. type 3, code = 3")
    print("6. type 11, code = 0")
    print("7. type 11, code = 0")
    print("8. dport 33434")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/ICMP(type=8)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/ICMP(type=0)
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/ICMP(type=3, code = 1)
    if choice == 4:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/ICMP(type=3, code = 2)
    if choice == 5:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/ICMP(type=3, code = 3)
    if choice == 6:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/ICMP(type=11, code = 0)
    if choice == 7:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/ICMP(type=11, code = 1)
    if choice == 8:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=33434, sport=1000)
    return pkt

def send_icmp_monitor_v6_traffic ():
    print("1. type 128")
    print("2. type 129")
    print("3. type 1 code 4")
    print("4. type 3 code 0")
    print("5. type 3 code 1")
    print("6. dport 33434")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/ICMPv6EchoRequest()
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/ICMPv6EchoReply()
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/ICMPv6DestUnreach(type=1, code=4)
    if choice == 4:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/ICMPv6TimeExceeded(type=3, code=1)
    if choice == 5:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/ICMPv6TimeExceeded(type=3, code=0)
    if choice == 6:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=33434, sport=1000)
    return pkt

def send_dhcp_v6_traffic ():
    print("1. dport 546 sport 546")
    print("2. dport 546 sport 547")
    print("3. dport 547 sport 546")
    print("4. dport 547 sport 547")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=546, sport=546)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=546, sport=547)
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=547, sport=546)
    if choice == 4:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=547, sport=547)
    return pkt

def send_dhcp_v4_traffic ():
    print("1. dport 67 sport 67")
    print("2. dport 67 sport 68")
    print("3. dport 68 sport 67")
    print("4. dport 68 sport 68")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=67, sport=67)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=67, sport=68)
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=68, sport=67)
    if choice == 4:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=68, sport=68)
    return pkt

def send_snmp_v6_traffic ():
    print("1. dport 161 ")
    print("2. dport 162 ")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=161, sport=1000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=162, sport=1000)
    return pkt

def send_tacacs_v6_traffic ():
    print("1. dport 49 ")
    print("2. sport 49 ")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=49, sport=1000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=1000, sport=49)
    return pkt

def send_radius_v6_traffic ():
    print("1. dport 1812 ")
    print("2. dport 1813 ")
    print("3. dport 1645 ")
    print("4. dport 1646 ")
    print("5. sport 1812 ")
    print("6. sport 1813 ")
    print("7. sport 1645 ")
    print("8. sport 1646 ")
    in_text = input("Enter data type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=1812, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=1813, sport=10000)
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=1645, sport=10000)
    if choice == 4:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=1646, sport=10000)
    if choice == 5:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=1000, sport=1812)
    if choice == 6:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=1000, sport=1813)
    if choice == 7:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=1000, sport=1645)
    if choice == 8:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=1000, sport=1646)
    return pkt


def send_telnet_v6_traffic ():
    print("1. dport 23 ")
    print("2. sport 23 ")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=23, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(sport=10000, dport=23)
    return pkt


def send_ssh_v6_traffic ():
    print("1. dport 21 ")
    print("2. dport 22 ")
    print("3. sport 21 ")
    print("4. sport 22 ")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=21, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=22, sport=10000)
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(sport=21, dport=10000)
    if choice == 4:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(sport=22, dport=10000)
    return pkt

# TFTP:
def send_tftp_v4_traffic ():
    print("1. dport 69 ")
    print("2. sport 69 ")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=69, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(sport=69, dport=10000)
    return pkt

def send_tftp_v6_traffic ():
    print("1. dport 69 ")
    print("2. sport 69 ")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=69, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(sport=69, dport=10000)
    return pkt

# SFTP:
def send_sftp_v4_traffic ():
    print("1. dport 115")
    print("2. sport 115")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=115, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=10000, sport=115)
    return pkt

# NTP:
def send_ntp_v4_traffic ():
    print("1. dport 123")
    print("2. sport 123")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=123, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(sport=123, dport=10000)
    return pkt

def send_ntp_v6_traffic ():
    print("1. dport 123")
    print("2. sport 123")
    in_text = input("Enter traffic type > ")
    choice = int(in_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=123, sport=10000)
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(sport=123, dport=10000)
    return pkt

def main(argv):
    in_text = input("Enter number of packets to send > ")
    pkt_cnt = int(in_text)
    while (1):
        print("1.  Send STP traffic")
        print("2.  Send LACP traffic")
        print("3.  Send ISIS traffic")
        print("4.  Send BGP v4 traffic")
        print("5.  Send OSPF v4 traffic")
        print("6.  Send BGP v6 traffic")
        print("7.  Send OSPF v6 traffic")
        print("8.  Send LLDP traffic")
        print("9.  Send VRRP v4 traffic")
        print("10. Send VRRP v6 traffic")
        print("11. Send ICMP v6 traffic")
        print("12. Send SFTP v4 traffic")
        print("13. Send TFTP v4 traffic")
        print("14. Send NTP v4 traffic")
        print("15. Send SSH v4 traffic")
        print("16. Send Telnet v4 traffic")
        print("17. Send Radius v4 traffic")
        print("18. Send Tacacs v4 traffic")
        print("19. Send SNMP v4 traffic")
        print("20. Send SFTP v6 traffic")
        print("21. Send TFTP v6 traffic")
        print("22. Send NTP v6 traffic")
        print("23. Send SSH v6 traffic")
        print("24. Send Telnet v6 traffic")
        print("25. Send Radius v6 traffic")
        print("26. Send Tacacs v6 traffic")
        print("27. Send SNMP v6 traffic")
        print("28. Send ICMP monitor v4 traffic")
        print("29. Send ICMP monitor v6 traffic")
        print("30. Send DHCP v4 traffic")
        print("31. Send DHCP v6 traffic")
        print("32. Send ARP traffic")
        print("33. Send v4 Exception traffic")
        print("34. Send v6 Exception traffic")
        print("35. Send v4 L3 Dest miss traffic")
        print("36. Send v6 L3 Dest miss traffic")
        print("37. Send v4 SelfIp traffic")
        print("38. Send v6 SelfIp traffic")
        print("39. Send v4 Glean traffic")
        print("40. Send v6 Glean traffic")
        print("41. Send IPv6 traffic")
        print("42. Send IPv4 traffic")
        print("43. Send TRACEROUTE v4 traffic")
        print("44. Send TRACEROUTE v6 traffic")
        print("45. Send UDP v4 traffic")

        in_text = input("Enter traffic option > ")
        in_traffic_class = int(in_text)
        if in_traffic_class == 1:
            tc_str = "STP"
            p = send_stp_traffic()
        if in_traffic_class == 2:
            tc_str = "LACP"
            p = send_lacp_traffic()
        if in_traffic_class == 3:
            tc_str = "isis"
            p = send_isis_traffic()
        if in_traffic_class == 4:
            tc_str = "bgp"
            p = send_bgp_v4_traffic()
        if in_traffic_class == 5:
            tc_str = "ospf"
            p = send_ospf_v4_traffic()
        if in_traffic_class == 6:
            tc_str = "bgp6"
            p = send_bgp_v6_traffic()
        if in_traffic_class == 7:
            tc_str = "ospf6"
            p = send_ospf_v6_traffic()
        if in_traffic_class == 8:
            tc_str = "lldp"
            p = send_lldp_traffic()
        if in_traffic_class == 9:
            p = send_vrrp_v4_traffic()
            tc_str = "vrrp4"
        if in_traffic_class == 10:
            tc_str = "vrrp6"
            p = send_vrrp_v6_traffic()
        if in_traffic_class == 11:
            tc_str = "icmp6"
            p = send_icmp_v6_traffic()
        if in_traffic_class == 12:
            tc_str = "sftp4"
            p = send_sftp_v4_traffic()
        if in_traffic_class == 13:
            tc_str = "tftp4"
            p = send_tftp_v4_traffic()
        if in_traffic_class == 14:
            tc_str = "ntp4"
            p = send_ntp_v4_traffic()
        if in_traffic_class == 15:
            tc_str = "ssh4"
            p = send_ssh_v4_traffic()
        if in_traffic_class == 16:
            tc_str = "telnet4"
            p = send_telnet_v4_traffic()
        if in_traffic_class == 17:
            tc_str = "radius4"
            p = send_radius_v4_traffic()
        if in_traffic_class == 18:
            tc_str = "tacacs4"
            p = send_tacacs_v4_traffic()
        if in_traffic_class == 19:
            tc_str = "snmp4"
            p = send_snmp_v4_traffic()
        if in_traffic_class == 20:
            tc_str = "sftp6"
            p = send_sftp_v6_traffic()
        if in_traffic_class == 21:
            tc_str = "tftp6"
            p = send_tftp_v6_traffic()
        if in_traffic_class == 22:
            tc_str = "ntp6"
            p = send_ntp_v6_traffic()
        if in_traffic_class == 23:
            tc_str = "ssh6"
            p = send_ssh_v6_traffic()
        if in_traffic_class == 24:
            tc_str = "telnet6"
            p = send_telnet_v6_traffic()
        if in_traffic_class == 25:
            tc_str = "radius6"
            p = send_radius_v6_traffic()
        if in_traffic_class == 26:
            tc_str = "tacacs6"
            p = send_tacacs_v6_traffic()
        if in_traffic_class == 27:
            tc_str = "snmp6"
            p = send_snmp_v6_traffic()
        if in_traffic_class == 28:
            tc_str = "icmp4"
            p = send_icmp_monitor_v4_traffic()
        if in_traffic_class == 29:
            tc_str = "icmp-mon6"
            p = send_icmp_monitor_v6_traffic()
        if in_traffic_class == 30:
            tc_str = "dhcp4"
            p = send_dhcp_v4_traffic()
        if in_traffic_class == 31:
            tc_str = "dhcp6"
            p = send_dhcp_v6_traffic()
        if in_traffic_class == 32:
            tc_str = "arp"
            p = send_arp_traffic()
        if in_traffic_class == 33:
            tc_str = "exception4"
            p = send_exception_v4_traffic()
        if in_traffic_class == 34:
            tc_str = "exception6"
            p = send_exception_v6_traffic()
        if in_traffic_class == 35:
            tc_str = "dst miss4"
            p = send_l3_dst_miss_v4_traffic()
        if in_traffic_class == 36:
            tc_str = "dst miss6"
            p = send_l3_dst_miss_v6_traffic()
        if in_traffic_class == 37:
            tc_str = "self-ip4"
            p = send_self_ip_v4_traffic()
        if in_traffic_class == 38:
            tc_str = "self-ip6"
            p = send_self_ip_v6_traffic()
        if in_traffic_class == 39:
            tc_str = "glean4"
            p = send_glean_v4_traffic()
        if in_traffic_class == 40:
            tc_str = "glean6"
            p = send_glean_v6_traffic()
        if in_traffic_class == 41:
            tc_str = "ip6"
            p = send_ip_v6_traffic()
        if in_traffic_class == 42:
            tc_str = "ip4"
            p = send_ip_v4_traffic()
        if in_traffic_class == 43:
            tc_str = "TraceRt4"
            p = send_trace_route_v4_traffic()
        if in_traffic_class == 44:
            tc_str = "TraceRt6"
            p = send_trace_route_v6_traffic()
        if in_traffic_class == 45:
            tc_str = "UDPtrafficv4"
            p = send_trace_udp_v4_traffic()
        print"Input provided is", tc_str


        send_packet(p, interface, pkt_cnt)

if __name__ == "__main__":
    main(sys.argv[1:])
