#!/usr/bin/python
# To be run on PE1 (10.27.101.21)
from scapy.all import *

data="hello world"
interface="swp9"
src_mac="B4:A9:FC:5B:6C:77"
dst_mac="B4:A9:FC:98:B2:2A"
sid_addresses_next_hop_end=["2001:abcd:cafe:2000:1::"]
sid_addresses_multi_hop_end=["2001:abcd:cafe:3000:1::", "2001:abcd:cafe:2000:1::"]
sid_addresses_next_hop_end_x=["2001:abcd:cafe:2000:8003::"]
sid_addresses_multi_hop_end_x=["2001:abcd:cafe:3000:8003::", "2001:abcd:cafe:2000:8003::"]
sid_addresses_next_hop_end_dt=["2001:abcd:cafe:2000:3::"]
sid_addresses_multi_hop_end_dt=["2001:abcd:cafe:3000:3::", "2001:abcd:cafe:2000:1::"]
src_inner_ip_v6="1::1"
dst_inner_ip_v6="1::2"
src_srv6_ip_v6="2001:10:1::1:1"
ttl_new = 0
udp_dst_port=33434

def send_to_END_ipv6_ipv6_next_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim= ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_next_hop_end, segleft=seg_left)/IPv6(src=src_inner_ip_v6, dst=dst_inner_ip_v6)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_ipv6_udp_next_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim= ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_next_hop_end, segleft=seg_left)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_ipv6_udp_next_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_ipv6_ipv6_next_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6()/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_ipv6_ipv6_multi_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=1
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_multi_hop_end, segleft=seg_left)/IPv6(src=src_inner_ip_v6, dst=dst_inner_ip_v6)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_ipv6_udp_multi_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=1
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_multi_hop_end, segleft=seg_left)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_ipv6_udp_multi_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:3000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_ipv6_ipv6_multi_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:3000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6()/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_X_ipv6_ipv6_next_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:8003::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_next_hop_end_x, segleft=seg_left)/IPv6(src=src_inner_ip_v6, dst=dst_inner_ip_v6)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_X_ipv6_udp_next_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:8003::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_next_hop_end_x, segleft=seg_left)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_X_ipv6_udp_next_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:8003::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_X_ipv6_ipv6_next_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:8003::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6()/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_X_ipv6_ipv6_multi_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:8003::"
    seg_left=1
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_multi_hop_end, segleft=seg_left)/IPv6(src=src_inner_ip_v6, dst=dst_inner_ip_v6)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_X_ipv6_udp_multi_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:8003::"
    seg_left=1
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_multi_hop_end, segleft=seg_left)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_X_ipv6_udp_multi_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:3000:8003::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_X_ipv6_ipv6_multi_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:3000:8003::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6()/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DX_ipv6_ipv6_next_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses, segleft=seg_left)/IPv6(src=src_inner_ip_v6, dst=dst_inner_ip_v6)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DX_ipv6_udp_next_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses, segleft=seg_left)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DX_ipv6_udp_next_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DX_ipv6_ipv6_next_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6()/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DX_ipv6_ipv6_multi_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=1
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_multi_hop_end, segleft=seg_left)/IPv6(src=src_inner_ip_v6, dst=dst_inner_ip_v6)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DX_ipv6_udp_multi_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=1
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_multi_hop_end, segleft=seg_left)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DX_ipv6_udp_multi_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DX_ipv6_ipv6_multi_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6()/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DT_ipv6_ipv6_next_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:3::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_next_hop_end_dt, segleft=seg_left)/IPv6(src=src_inner_ip_v6, dst=dst_inner_ip_v6)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DT_ipv6_udp_next_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:3::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_next_hop_end_dt, segleft=seg_left)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DT_ipv6_udp_next_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:3::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DT_ipv6_ipv6_next_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:3::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6()/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DT_ipv6_ipv6_multi_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=1
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_multi_hop_end, segleft=seg_left)/IPv6(src=src_inner_ip_v6, dst=dst_inner_ip_v6)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DT_ipv6_udp_multi_hop ():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:2000:1::"
    seg_left=1
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6ExtHdrSegmentRouting(addresses=sid_addresses_multi_hop_end, segleft=seg_left)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DT_ipv6_udp_multi_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:3000:3::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/UDP(dport=udp_dst_port)/data
    p.show()
    sendp(p, iface=interface, count=100000)

def send_to_END_DT_ipv6_ipv6_multi_hop_no_srh():
    global ttl_new
    dst_srv6_ip_v6="2001:abcd:cafe:3000:3::"
    seg_left=0
    p=Ether(src=src_mac, dst=dst_mac)/IPv6(src=src_srv6_ip_v6, dst=dst_srv6_ip_v6, hlim=ttl_new)/IPv6()/data
    p.show()
    sendp(p, iface=interface, count=100000)

def main(argv):
    global ttl_new
    while (1):
        print("1. Send IPv6/IPv6 END to next hop with SRH SL=0")
        print("2. Send IPv6/UDP END to next hop with SRH SL=0")
        print("3. Send IPv6/IPv6 END to next hop without SRH")
        print("4. Send IPv6/UDP END to next hop without SRH")
        print("5. Send IPv6/IPv6 END to multi hop with SRH SL>0")
        print("6. Send IPv6/UDP END to multi hop with SRH SL>0")
        print("7. Send IPv6/IPv6 END to multi hop without SRH")
        print("8. Send IPv6/UDP END to multi hop without SRH")
        print("9. Send IPv6/IPv6 END.X to next hop with SRH SL=0")
        print("10. Send IPv6/UDP END.X to next hop with SRH SL=0")
        print("11. Send IPv6/IPv6 END.X to next hop without SRH")
        print("12. Send IPv6/UDP END.X to next hop without SRH")
        print("13. Send IPv6/IPv6 END.X to multi hop with SRH SL>0")
        print("14. Send IPv6/UDP END.X to multi hop with SRH SL>0")
        print("15. Send IPv6/IPv6 END.X to multi hop without SRH")
        print("16. Send IPv6/UDP END.X to multi hop without SRH")
        print("17. Send IPv6/IPv6 END.DX to next hop with SRH SL=0")
        print("18. Send IPv6/UDP END.DX to next hop with SRH SL=0")
        print("19. Send IPv6/IPv6 END.DX to next hop without SRH")
        print("20. Send IPv6/UDP END.DX to next hop without SRH")
        print("21. Send IPv6/IPv6 END.DX to multi hop with SRH SL>0")
        print("22. Send IPv6/UDP END.DX to multi hop with SRH SL>0")
        print("23. Send IPv6/IPv6 END.DX to multi hop without SRH")
        print("24. Send IPv6/UDP END.DX to multi hop without SRH")
        print("25. Send IPv6/IPv6 END.DT to next hop with SRH SL=0")
        print("26. Send IPv6/UDP END.DT to next hop with SRH SL=0")
        print("27. Send IPv6/IPv6 END.DT to next hop without SRH")
        print("28. Send IPv6/UDP END.DT to next hop without SRH")
        print("29. Send IPv6/IPv6 END.DT to multi hop with SRH SL>0")
        print("30. Send IPv6/UDP END.DT to multi hop with SRH SL>0")
        print("31. Send IPv6/IPv6 END.DT to multi hop without SRH")
        print("32. Send IPv6/UDP END.DT to multi hop without SRH")
        in_text = input("Enter an option > ")
        choice = int(in_text)
        in_ttl = input("Enter TTL value > ")
        global ttl_new
        ttl_new = int(in_ttl)
        print"Input provide is", choice
        while ttl_new != -1 :
            if choice == 1:
                send_to_END_ipv6_ipv6_next_hop()
            if choice == 2:
                send_to_END_ipv6_udp_next_hop()
            if choice == 3:
                send_to_END_ipv6_ipv6_next_hop_no_srh()
            if choice == 4:
                send_to_END_ipv6_udp_next_hop_no_srh()
            if choice == 5:
                send_to_END_ipv6_ipv6_multi_hop()
            if choice == 6:
                send_to_END_ipv6_udp_multi_hop()
            if choice == 7:
                send_to_END_ipv6_ipv6_multi_hop_no_srh()
            if choice == 8:
                send_to_END_ipv6_udp_multi_hop_no_srh()
            if choice == 9:
                send_to_END_X_ipv6_ipv6_next_hop()
            if choice == 10:
                send_to_END_X_ipv6_udp_next_hop()
            if choice == 11:
                send_to_END_X_ipv6_ipv6_next_hop_no_srh()
            if choice == 12:
                send_to_END_X_ipv6_udp_next_hop_no_srh()
            if choice == 13:
                send_to_END_X_ipv6_ipv6_multi_hop()
            if choice == 14:
                send_to_END_X_ipv6_udp_multi_hop()
            if choice == 15:
                send_to_END_X_ipv6_ipv6_multi_hop_no_srh()
            if choice == 16:
                send_to_END_X_ipv6_udp_multi_hop_no_srh()
            if choice == 17:
                send_to_END_DX_ipv6_ipv6_next_hop()
            if choice == 18:
                send_to_END_DX_ipv6_udp_next_hop()
            if choice == 19:
                send_to_END_DX_ipv6_ipv6_next_hop_no_srh()
            if choice == 20:
                send_to_END_DX_ipv6_udp_next_hop_no_srh()
            if choice == 21:
                send_to_END_DX_ipv6_ipv6_multi_hop()
            if choice == 22:
                send_to_END_DX_ipv6_udp_multi_hop()
            if choice == 23:
                send_to_END_DX_ipv6_ipv6_multi_hop_no_srh()
            if choice == 24:
                send_to_END_DX_ipv6_udp_multi_hop_no_srh()
            if choice == 25:
                send_to_END_DT_ipv6_ipv6_next_hop()
            if choice == 26:
                send_to_END_DT_ipv6_udp_next_hop()
            if choice == 27:
                send_to_END_DT_ipv6_ipv6_next_hop_no_srh()
            if choice == 28:
                send_to_END_DT_ipv6_udp_next_hop_no_srh()
            if choice == 29:
                send_to_END_DT_ipv6_ipv6_multi_hop()
            if choice == 30:
                send_to_END_DT_ipv6_udp_multi_hop()
            if choice == 31:
                send_to_END_DT_ipv6_ipv6_multi_hop_no_srh()
            if choice == 32:
                send_to_END_DT_ipv6_udp_multi_hop_no_srh()
            in_ttl = input("Enter TTL value (-1 to enter new Function type) > ")
            ttl_new = int(in_ttl)

if __name__ == "__main__":
     main(sys.argv[1:])
