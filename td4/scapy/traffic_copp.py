#!/usr/bin/python
from scapy.all import *

data="hello world 123456789123456789123456789123456789123456789"
src_ip="140.1.1.1"
dst_ip="140.1.1.2"
dst_ip6="1140:1::2"
src_ip6="1140:1::1"
dmac="00:18:23:30:e5:d7"
smac="00:00:05:9f:66:73"
interface="swp2"

def send_packet (p, intf, t_count):
    i = 0;
    p.show()
    raw_input("Press key to continue")
    sendp(p, iface=intf, count=t_count)

def send_ip_v6_traffic ():
    # IP/IP6
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/data
    return pkt

def send_ip_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/data
    return pkt

def send_bgp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=179, sport=179)/data
    return pkt

def send_bgp_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=179, sport=179)/data
    return pkt

def send_icmp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/ICMP()/data
    return pkt

def send_icmp_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/ICMPv6EchoRequest()/data
    return pkt

# STP:
def send_stp_traffic ():
    pkt = Ether(src=smac, dst="01:80:C2:00:00:00")/LLC()/STP()/data
    return pkt

# LACP:
def send_lacp_traffic ():
    pkt = Ether(src=smac, dst="01:80:C2:00:00:02")/data
    return pkt

# ISIS:
def send_isis_traffic ():
    print("1. dst mac = 01:80:C2:00:00:14")
    print("2. dst mac = 01:80:C2:00:00:15")
    print("3. dst mac = 09:00:2b:00:00:05")
    isis_sub_text = input("Enter isis traffic type > ")
    choice = int(isis_sub_text)
    if choice == 1:
        pkt = Ether(src=smac, dst="01:80:C2:00:00:14")/data
    if choice == 2:
        pkt = Ether(src=smac, dst="01:80:C2:00:00:15")/data
    if choice == 3:
        pkt = Ether(src=smac, dst="09:00:2b:00:00:05")/data
    return pkt

# VRRP:
def send_vrrp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/VRRP()/data
    return pkt

def send_vrrp_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/VRRP()/data
    return pkt

def send_trace_route_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=33434, sport=33434)/data
    return pkt

def send_trace_route_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=33434, sport=33434)/data
    return pkt

# SSH/TELNET:
def send_ssh_v4_traffic ():
    print("1. dport 21 ")
    print("2. dport 22 ")
    print("3. dport 23 ")
    ssh_v4_text = input("Enter ssh v4 traffic type > ")
    choice = int(ssh_v4_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=21, sport=10000)/data
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=22, sport=10000)/data
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=23, sport=10000)/data
    return pkt

def send_ssh_v6_traffic ():
    print("1. dport 21 ")
    print("2. dport 22 ")
    print("3. dport 23 ")
    ssh_v6_text = input("Enter ssh v6 traffic type > ")
    choice = int(ssh_v6_text)
    if choice == 1:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=21, sport=10000)/data
    if choice == 2:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=22, sport=10000)/data
    if choice == 3:
        pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=23, sport=10000)/data
    return pkt

# TFTP:
def send_tftp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=69, sport=10000)/data
    return pkt

def send_tftp_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=69, sport=10000)/data
    return pkt

# SFTP:
def send_sftp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=115, sport=10000)/data
    return pkt

def send_sftp_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=115, sport=10000)/data
    return pkt

# NTP:
def send_ntp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=123, sport=10000)/data
    return pkt

def send_ntp_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=123, sport=10000)/data
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

        in_text = input("Enter traffic option > ")
        in_traffic_class = int(in_text)
        print"Input provided is", in_traffic_class
        if in_traffic_class == 1:
            p = send_stp_traffic()
        if in_traffic_class == 2:
            p = send_lacp_traffic()
        if in_traffic_class == 3:
            p = send_isis_traffic()
        if in_traffic_class == 4:
            p = send_bgp_v4_traffic()
        if in_traffic_class == 5:
            p = send_ospf_v4_traffic()
        if in_traffic_class == 6:
            p = send_bgp_v6_traffic()
        if in_traffic_class == 7:
            p = send_ospf_v6_traffic()
        if in_traffic_class == 8:
            p = send_lldp_traffic()
        if in_traffic_class == 9:
            p = send_vrrp_v4_traffic()
        if in_traffic_class == 10:
            p = send_vrrp_v6_traffic()
        if in_traffic_class == 11:
            p = send_icmp_v6_traffic()
        if in_traffic_class == 12:
            p = send_sftp_v4_traffic()
        if in_traffic_class == 13:
            p = send_tftp_v4_traffic()
        if in_traffic_class == 14:
            p = send_ntp_v4_traffic()
        if in_traffic_class == 15:
            p = send_ssh_v4_traffic()
        if in_traffic_class == 16:
            p = send_telnet_v4_traffic()
        if in_traffic_class == 17:
            p = send_radius_v4_traffic()
        if in_traffic_class == 18:
            p = send_tacacs_v4_traffic()
        if in_traffic_class == 19:
            p = send_snmp_v4_traffic()
        if in_traffic_class == 20:
            p = send_sftp_v6_traffic()
        if in_traffic_class == 21:
            p = send_tftp_v6_traffic()
        if in_traffic_class == 22:
            p = send_ntp_v6_traffic()
        if in_traffic_class == 23:
            p = send_ssh_v6_traffic()
        if in_traffic_class == 24:
            p = send_telnet_v6_traffic()
        if in_traffic_class == 25:
            p = send_radius_v6_traffic()
        if in_traffic_class == 26:
            p = send_tacacs_v6_traffic()
        if in_traffic_class == 27:
            p = send_snmp_v6_traffic()
        if in_traffic_class == 28:
            p = send_icmp_monitor_v4_traffic()
        if in_traffic_class == 29:
            p = send_icmp_monitor_v6_traffic()
        if in_traffic_class == 30:
            p = send_dhcp_v4_traffic()
        if in_traffic_class == 31:
            p = send_dhcp_v6_traffic()
        if in_traffic_class == 32:
            p = send_arp_traffic()
        if in_traffic_class == 33:
            p = send_exception_v4_traffic()
        if in_traffic_class == 34:
            p = send_exception_v6_traffic()
        if in_traffic_class == 35:
            p = send_l3_dst_miss_v4_traffic()
        if in_traffic_class == 36:
            p = send_l3_dst_miss_v6_traffic()
        if in_traffic_class == 37:
            p = send_self_ip_v4_traffic()
        if in_traffic_class == 38:
            p = send_self_ip_v6_traffic()
        if in_traffic_class == 39:
            p = send_glean_v4_traffic()
        if in_traffic_class == 40:
            p = send_glean_v6_traffic()
        if in_traffic_class == 41:
            p = send_ip_v6_traffic()
        if in_traffic_class == 42:
            p = send_ip_v4_traffic()
        if in_traffic_class == 43:
            p = send_trace_route_v4_traffic()
        if in_traffic_class == 44:
            p = send_trace_route_v6_traffic()


        send_packet(p, interface, pkt_cnt)

if __name__ == "__main__":
    main(sys.argv[1:])
