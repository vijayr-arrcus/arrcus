from scapy.all import *

dst_ip="140.1.1.2"
src_ip="140.1.1.1"
dst_ip6="1140:1::2"
src_ip6="1140:1::1"
dmac="00:18:23:30:e5:d7"
smac="00:00:05:9f:66:73"
port="swp2s1"

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
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip6, dst=dst_ip6)/data
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
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=33434, sport=33434)/data
    return pkt

def send_trace_route_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=33434, sport=33434)/data
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
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=69, sport=10000)/data
    return pkt

def send_tftp_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=69, sport=10000)/data
    return pkt

# SFTP:
def send_sftp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/TCP(dport=115, sport=10000)/data
    return pkt

def send_sftp_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/TCP(dport=115, sport=10000)/data
    return pkt

# NTP:
def send_ntp_v4_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IPv6(src=src_ip6, dst=dst_ip6)/UDP(dport=123, sport=10000)/data
    return pkt

def send_ntp_v6_traffic ():
    pkt = Ether(src=smac, dst=dmac)/IP(src=src_ip, dst=dst_ip)/UDP(dport=123, sport=10000)/data
    return pkt

def main(argv):
    in_text = input("Enter number of packets to send > ")
    pkt_cnt = int(in_text)
    while (1):
        print("1. Send STP traffic")
        print("2. Send LACP traffic")
        print("3. Send ISIS traffic")
        print("4. Send IPv6 traffic")
        print("5. Send IPv4 traffic")
        print("6. Send BGP v4 traffic")
        print("7. Send BGP v6 traffic")
        print("8. Send ICMP v4 traffic")
        print("9. Send ICMP v6 traffic")
        print("10. Send VRRP v4 traffic")
        print("11. Send VRRP v6 traffic")
        print("12. Send TRACEROUTE v4 traffic")
        print("13. Send TRACEROUTE v6 traffic")
        print("14. Send SSH/Telnet v4 traffic")
        print("15. Send SSH/Telnet v6 traffic")
        print("16. Send TFTP v4 traffic")
        print("17. Send TFTP v6 traffic")
        print("18. Send SFTP v4 traffic")
        print("19. Send SFTP v6 traffic")
        print("20. Send NTP v4 traffic")
        print("21. Send NTP v6 traffic")

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
            p = send_ip_v6_traffic()
        if in_traffic_class == 5:
            p = send_ip_v4_traffic()
        if in_traffic_class == 6:
            p = send_bgp_v4_traffic()
        if in_traffic_class == 7:
            p = send_bgp_v6_traffic()
        if in_traffic_class == 8:
            p = send_icmp_v4_traffic()
        if in_traffic_class == 9:
            p = send_icmp_v6_traffic()
        if in_traffic_class == 10:
            p = send_vrrp_v4_traffic()
        if in_traffic_class == 11:
            p = send_vrrp_v6_traffic()
        if in_traffic_class == 12:
            p = send_trace_route_v4_traffic()
        if in_traffic_class == 13:
            p = send_trace_route_v6_traffic()
        if in_traffic_class == 14:
            p = send_ssh_v4_traffic()
        if in_traffic_class == 15:
            p = send_ssh_v6_traffic()
        if in_traffic_class == 16:
            p = send_tftp_v4_traffic()
        if in_traffic_class == 17:
            p = send_tftp_v6_traffic()
        if in_traffic_class == 18:
            p = send_stftp_v4_traffic()
        if in_traffic_class == 19:
            p = send_stftp_v6_traffic()
        if in_traffic_class == 20:
            p = send_ntp_v4_traffic()
        if in_traffic_class == 21:
            p = send_ntp_v6_traffic()
        send_packet(p, interface, pkt_cnt)

if __name__ == "__main__"
    main(sys.argv[1:])
