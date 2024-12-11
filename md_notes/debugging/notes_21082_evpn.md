# Table of Contents:
- [Topology](#topology)
- [eBGP Underlay Verification](#ebgp-underlay-verification)
  * [Verify the IPV4_UNICAST Spines peering is established](#verify-the-ipv4-unicast-spines-peering-is-established)
  * [Check routes received from the established IPv4 unicast neighbors](#check-routes-received-from-the-established-ipv4-unicast-neighbors)
  * [Check that underlay routes are in the BGP RIB](#check-that-underlay-routes-are-in-the-bgp-rib)
  * [Check the IPV4 RIB on Leaf 1 (brief output)](#check-the-ipv4-rib-on-leaf-1--brief-output-)
  * [To see full details of all entries in the IPV4 RIB run the command:](#to-see-full-details-of-all-entries-in-the-ipv4-rib-run-the-command-)
  * [To see details on any one specific entry:](#to-see-details-on-any-one-specific-entry-)
  * [An overview of the default RIB statistics can be seen per the command below.](#an-overview-of-the-default-rib-statistics-can-be-seen-per-the-command-below)
  * [Test ping remote loopback sourcing from local loopback.](#test-ping-remote-loopback-sourcing-from-local-loopback)
- [ISIS as the underlay IGP](#isis-as-the-underlay-igp)
  * [Check the remote loopbacks have been learnt via ISIS in the default Network Instance.](#check-the-remote-loopbacks-have-been-learnt-via-isis-in-the-default-network-instance)
  * [Check the ISIS routes are installed in the FIB.](#check-the-isis-routes-are-installed-in-the-fib)
  * [useful command to determine what protocols are using these entries is shown below](#useful-command-to-determine-what-protocols-are-using-these-entries-is-shown-below)
  * [more ISIS commands](#more-isis-commands)
  * [Verify reachability](#verify-reachability)
- [Overlay Vxlan Configuration.](#overlay-vxlan-configuration)


# Topology
                     +--------------------------------+                                                                                           +--------------------------------+
                     |                                | swp1                                               192.168.0.16/31                   swp1 |                                |
                     |                                --------+            +-----------------------------------------------------------------------                                |
                     |               RR               |       |            |                                                                      |              RR                |
                     |                                |       |            |                                                                      |                                |
                     |        lo0 - 2.1.1.3           |-------------------------------------------------------+            +----------------------|         lo0 - 2.1.1.4          |
                     +--------------------------------+ swp2  |            |               192.168.0.14/31    |            |192.168.0.18/31  swp2 +--------------------------------+
                                                              |            |                                  |            |
                                                              |            |                                  |            |
                                                              |            |                                  |            |
                                              192.168.0.12/31 |            |                                  |            |
                                                              |            |                                  |            |
                                                              |            |                                  |            |
                                                              |            |                                  |            |
                                                              |            |                                  |            |
                                                         swp4 |            | swp5                       swp4  |            |swp5
                                                       +------|------------|-----+                     +------|------------|-----+
                                                       |          spine1         |               swp2  |         spine2          |           192.168.0.22/31
                  +-------------------------------------                         |   +------------------                         ------------------------------------+
                  |                             swp1   |       lo0 - 2.1.1.1     |   |192.168.0.8/31   |    lo0 - 2.1.1.2        | swp6                              |
                  |                                    +-------|----|------------+   |                 +----|--------+-----+-----+                                   |
                  |                                            |    |      |         |                swp1  |        | swp3                                          |
                  |                                       swp2 |    |swp3  |swp6     |                      |        |                                               |
                  |                                            |    |      |         |                      |        |                                               |
    192.168.0.0/31|         192.168.0.6/31                     |    |      |         |  192.168.0.6/31      |        |                                               |
                  |            +-------------------------------|----|------|--------------------------------|        | 192.168.0.10/31                               |
                  |            |                               |    |      |         |                               |                                               |
                  |            |                 192.168.0.2/31|    |      |         | 192.168.0.4/31                |                                               |
                  |            |                               |    +------|-------------------------|               |                                               |
                  |            |                               |           |         |               |               |          192.168.0.20/31                      |
                  |            |                               |           +-----------------------------------------------------------------------------------|     |
                  |            |                               |                     |               |               |                                         |     |
            swp1  |       swp2 |                          swp1 |                swp2 |           swp1|          swp2 |                                    swp1 |     | swp2
         +--------|------------|---+                         +-|---------------------|-+         +---|---------------|-----+                            +------|-----|------------+
         |          Leaf1          |                         |         Leaf2           |         |        Leaf3            |                            |       Leaf4             |
         |                         |                         |                         |         |                         |                            |                         |
         |       lo0 - 1.1.1.1     |                         |      lo0 - 1.1.1.2      |         |      lo0 - 1.1.1.3      |                            |      lo0 - 1.1.1.4      |
         +---|---------|--------|--+                         +------|----|------|------+         +------|-----|------|-----+                            +---|---------|---------|-+
        swp3 |    swp4 |   swp5 |                               swp3|    |swp4  | swp5              swp3|     |swp4  | swp5                            swp3 |    swp4 |    swp5 |
             |         |        |                                   |    |      |------------------+    |     |      |                                      |         |         |
             |         |        |                                   |    |----------------+        |    |     |      |                                      |         |         |
             |         |        |                                   +-----------|  |------|--------|----+     |      |                                      |         |         |
             |         |        |                                               |  |      |  |-----|----------+      |                                      |         |         |
             |         |        |                                               |  |      |  |     | |---------------+                                      |         |         |
       vlan10|   vlan20|  vlan30|                                         vlan10|  |   vlan20|     | |vlan40                                          vlan10|   vlan20|         |
             |         |        |                                               |  |      |  |     | |                                                      |         |         |
             |         |        |                                               |  |      |  |     | |                                                      |         |    swp1 |
         +---|--+  +---|--+ +---|--+                                          +-|--|-+  +-|--|-+ +-|-|--+                                                +--|---+  +--|---+ +---|------------+
         | H1   |  |  H2  | | H3   |                                          | H4   |  |  H5  | |  H6  |                                                | H7   |  | H8   | |    Router1     |
         |      |  |      | |      |                                          |      |  |      | |      |                                                |      |  |      | |                |
         +------+  +------+ +------+                                          +------+  +------+ +------+                                                +------+  +------+ +----------------+

#show commands for EVPN.

```
Leaf1#
show network-instance default fib IPV4 ipv4-adjacency
ipv4-adjacency
IF LINK VRF NHT PATH
INDEX INTERFACE STATUS ID IPADDR     HWADDR            L2IFINDEX IP ID
------------------------------------------------------------------------------
5      swp1      UP    1 192.168.0.1 08:00:27:20:30:39 5
6      swp2      UP    1 192.168.0.7 08:00:27:ef:a2:ed 6
7      swp3      UP    1
8      swp4      UP    1
9      swp5      UP    1
10     swp6      UP    1
17  loopback0    UP    1

```

# eBGP Underlay Verification

## Verify the IPV4_UNICAST Spines peering is established

```
leaf1# show network-instance default protocol BGP default all-neighbor * afi-safi ipv4-
unicast
all-neighbor
WARNING SEND
NEIGHBOR AFI SAFI MAX PREVENT THRESHOLD RESTART DEFAULT
ADDRESS NAME PREFIXES TEARDOWN PCT TIMER ROUTE
--------------------------------------------------------------------------
192.168.0.1 IPV4_UNICAST 0 false 0 0.0 -
192.168.0.7 IPV4_UNICAST 0 false 0 0.0 -
```

## Check routes received from the established IPv4 unicast neighbors

```
leaf1# show network-instance default protocol BGP default all-neighbor * afi-safi
IPV4_UNICAST state prefixes | grep IPV4_UNICAST
192.168.0.1 IPV4_UNICAST 8 10 8 14 0 0
192.168.0.7 IPV4_UNICAST 6 10 6 14 0 0
```

## Check that underlay routes are in the BGP RIB

```
leaf1# show network-instance default protocol BGP default rib afi-safi IPV4_UNICAST
loc-rib route | grep rib\ route
loc-rib route 1.1.1.1/32 0.0.0.0 0
loc-rib route 1.1.1.2/32 192.168.0.1 0
loc-rib route 1.1.1.2/32 192.168.0.7 0
loc-rib route 1.1.1.3/32 192.168.0.1 0
loc-rib route 1.1.1.3/32 192.168.0.7 0
loc-rib route 1.1.1.4/32 192.168.0.1 0
loc-rib route 1.1.1.4/32 192.168.0.7 0
loc-rib route 2.1.1.1/32 192.168.0.1 0
loc-rib route 2.1.1.2/32 192.168.0.7 0
loc-rib route 2.1.1.3/32 192.168.0.1 0
loc-rib route 2.1.1.3/32 192.168.0.7 0
loc-rib route 2.1.1.4/32 192.168.0.1 0
loc-rib route 2.1.1.4/32 192.168.0.7 0
loc-rib route 192.168.1.0/24 192.168.0.1 0
loc-rib route 192.168.18.0/24 192.168.0.1 0
```

## Check the IPV4 RIB on Leaf 1 (brief output)

```
leaf1# show network-instance default rib IPV4 ipv4-entries displaylevel 1 | notab
ipv4-entries entry 1.1.1.1/32
ipv4-entries entry 1.1.1.2/32
ipv4-entries entry 1.1.1.3/32
ipv4-entries entry 1.1.1.4/32
ipv4-entries entry 2.1.1.1/32
ipv4-entries entry 2.1.1.2/32
ipv4-entries entry 2.1.1.3/32
ipv4-entries entry 2.1.1.4/32
ipv4-entries entry 192.168.0.0/31
ipv4-entries entry 192.168.0.0/32
ipv4-entries entry 192.168.0.1/32
ipv4-entries entry 192.168.0.6/31
ipv4-entries entry 192.168.0.6/32
ipv4-entries entry 192.168.0.7/32
ipv4-entries entry 192.168.1.0/24
ipv4-entries entry 192.168.18.0/24
```

## To see full details of all entries in the IPV4 RIB run the command:

```
show network-instance default rib IPV4 ipv4-entries
```

## To see details on any one specific entry:

```

show network-instance default rib IPV4 ipv4-entries entry <prefix>

```

## An overview of the default RIB statistics can be seen per the command below.

```
leaf1# show network-instance default rib IPV4 stats-entries
stats-entries routes 16
stats-entries paths 21
stats-entries rnhs 7
stats-entries route-add-msg 42
stats-entries route-delete-msg 0
stats-entries rnh-add-msg 13
stats-entries rnh-delete-msg 0
stats-entries label-add-msg 0
stats-entries label-delete-msg 0
stats-entries clients
 ROUTE ROUTE RNH RNH LABEL LABEL
ADD DELETE ADD DELETE ADD DELETE
NAME ROUTES PATHS RNHS MSG MSG MSG MSG MSG MSG
------------------------------------------------------------------------------------
BGP 9 14 7 13 0 13 0 0 0
ADJACENCY 2 2 0 2 0 0 0 0 0
DIRECTLY_CONNECTED 5 5 0 5 0 0 0 0 0
LDP 0 0 0 0 0 0 0 0 0
```

## Test ping remote loopback sourcing from local loopback.

```
leaf1# ping 1.1.1.3 -I 1.1.1.1
PING 1.1.1.3 (1.1.1.3) from 1.1.1.1 : 56(84) bytes of data.
64 bytes from 1.1.1.3: icmp_seq=1 ttl=63 time=1.60 ms
64 bytes from 1.1.1.3: icmp_seq=2 ttl=63 time=1.71 ms
64 bytes from 1.1.1.3: icmp_seq=3 ttl=63 time=1.88 ms
64 bytes from 1.1.1.3: icmp_seq=4 ttl=63 time=1.76 ms
^C
--- 1.1.1.3 ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3004ms
rtt min/avg/max/mdev = 1.604/1.741/1.883/0.100 ms
leaf1# ping 2.1.1.3 -I 1.1.1.1
PING 2.1.1.3 (2.1.1.3) from 1.1.1.1 : 56(84) bytes of data.
64 bytes from 2.1.1.3: icmp_seq=1 ttl=63 time=1.33 ms
64 bytes from 2.1.1.3: icmp_seq=2 ttl=63 time=1.55 ms
64 bytes from 2.1.1.3: icmp_seq=3 ttl=63 time=1.33 ms
64 bytes from 2.1.1.3: icmp_seq=4 ttl=63 time=1.41 ms
```

# ISIS as the underlay IGP

```
leaf1-isis# show network-instance default protocol ISIS default interface * level *
adjacency displaylevel 3
interface swp1
level 2
 adjacency 0020.0100.1001
interface swp2
level 2
 adjacency 0020.0100.1002
```

```
spine1-isis# show network-instance default protocol ISIS default interface * level *
adjacency displaylevel 3
interface swp1
level 2
 adjacency 0010.0100.1001
interface swp2
level 2
 adjacency 0010.0100.1002
interface swp3
level 2
 adjacency 0010.0100.1003
interface swp4
level 2
 adjacency 0020.0100.1003
interface swp5
level 2
 adjacency 0020.0100.1004
interface swp6
level 2
 adjacency 0010.0100.1004
```

## Check the remote loopbacks have been learnt via ISIS in the default Network Instance.


```

leaf1-isis# show network-instance default protocol ISIS default global af IPV4 UNICAST
route state prefix

route
PREFIX PREFIX
----------------------------------
1.1.1.1/32 1.1.1.1/32
1.1.1.2/32 1.1.1.2/32
1.1.1.3/32 1.1.1.3/32
2.1.1.1/32 2.1.1.1/32
2.1.1.2/32 2.1.1.2/32
2.1.1.3/32 2.1.1.3/32
2.1.1.4/32 2.1.1.4/32
192.168.0.0/31 192.168.0.0/31
192.168.0.2/31 192.168.0.2/31
192.168.0.4/31 192.168.0.4/31
192.168.0.6/31 192.168.0.6/31
192.168.0.8/31 192.168.0.8/31
192.168.0.10/31 192.168.0.10/31
192.168.0.12/31 192.168.0.12/31
192.168.0.14/31 192.168.0.14/31
192.168.0.16/31 192.168.0.16/31
192.168.0.18/31 192.168.0.18/31
192.168.18.0/24 192.168.18.0/24
```

## Check the ISIS routes are installed in the FIB.

```

leaf1# show network-instance default fib IPV4 ipv4-prefix-entry
ipv4-prefix-entry
 NEXT
 HOP PUBLISH
PREFIX ID ID
--------------------------------
0.0.0.0/0 - -
1.1.1.1/32 16 15
1.1.1.2/32 73 4194377
1.1.1.3/32 73 4194377
1.1.1.4/32 73 4194377
2.1.1.1/32 74 71
2.1.1.2/32 75 72
2.1.1.3/32 73 4194377
2.1.1.4/32 73 4194377
192.168.0.0/31 40 39
192.168.0.0/32 42 41
192.168.0.1/32 64 63
192.168.0.2/31 74 71
192.168.0.4/31 74 71
192.168.0.6/31 44 43
192.168.0.6/32 46 45
192.168.0.7/32 66 65
192.168.0.8/31 75 72
192.168.0.10/31 75 72
192.168.0.12/31 74 71
192.168.0.14/31 75 72
192.168.0.16/31 74 71
192.168.0.18/31 75 72
192.168.0.20/31 74 71
192.168.0.22/31 75 72
192.168.1.0/24 74 71

```

## useful command to determine what protocols are using these entries is shown below

```
leaf1# show network-instance default rib IPV4 ipv4-rnh-entries
ipv4-rnh-entries entry
 RES TRACKING COOKIE
PREFIX STATE METRIC RES PREFIX PROTOCOL VALUE
---------------------------------------------------------
1.1.1.2/32 true 30 1.1.1.2/32 BGP 9
1.1.1.3/32 true 30 1.1.1.3/32 BGP 9
1.1.1.4/32 true 30 1.1.1.4/32 BGP 9
2.1.1.3/32 true 30 2.1.1.3/32 BGP 9
2.1.1.4/32 true 30 2.1.1.4/32 BGP 9
```

## more ISIS commands

```

show network-instance default protocol ISIS default level 2 link-state-database lsp
```

```

show network-instance default protocol ISIS default level 2 link-state-database lsp * tlv
EXTENDED_IPV4_REACHABILITY

```

## Verify reachability

```

leaf1# ping 1.1.1.3 -I 1.1.1.1
PING 1.1.1.3 (1.1.1.3) from 1.1.1.1 : 56(84) bytes of data.
64 bytes from 1.1.1.3: icmp_seq=1 ttl=63 time=1.28 ms
64 bytes from 1.1.1.3: icmp_seq=2 ttl=63 time=1.61 ms
64 bytes from 1.1.1.3: icmp_seq=3 ttl=63 time=1.46 ms
leaf1# ping 2.1.1.3 -I 1.1.1.1
PING 2.1.1.3 (2.1.1.3) from 1.1.1.1 : 56(84) bytes of data.
64 bytes from 2.1.1.3: icmp_seq=1 ttl=63 time=2.12 ms
64 bytes from 2.1.1.3: icmp_seq=2 ttl=63 time=1.66 ms
64 bytes from 2.1.1.3: icmp_seq=3 ttl=63 time=1.77 ms

```

# Overlay Vxlan Configuration.

## Create the overlay (VTEP) interface.

```
overlay local-tunnel-endpoint 0
source-interface loopback0
!
```

## Overlay Forwarding Interface Verification

```
leaf1# show overlay local-tunnel-endpoint
overlay local-tunnel-endpoint
LTEP
LTEP RUN SOURCE SOURCE UDP
ID STATE INTERFACE IP ENCAPSULATION PORT
------------------------------------------------------
0 UP loopback0 1.1.1.1 VXLAN 4789
```


The overlay tunnels will be verified once the BGP EVPN peering is ESTABLISHED.
The flood-list, or IMET replication list, is built using EVPN RT-3 updates.


## Validate the EVPN Peering sessions are ESTABLISHED

```
leaf1# show network-instance default protocol BGP default all-neighbor state peer-group overlayevpn | select state session-state | select state session-elapsed-time | select state peer-as |
select state established-transitions | select state local-as
all-neighbor
 SESSION
NEIGHBOR PEER LOCAL SESSION ESTABLISHED ELAPSED
ADDRESS AS AS STATE TRANSITIONS TIME
------------------------------------------------------------
2.1.1.3 65100 65001 ESTABLISHED 3 00:49:51
2.1.1.4 65100 65001 ESTABLISHED 2 00:49:23

```

## BGP detailed information per neighbor.

```
arcrr1# show network-instance default protocol BGP default neighbor 1.1.1.1
neighbor 1.1.1.1
state peer-group leaf-evpn
state neighbor-address 1.1.1.1
state enabled true
state peer-as 65001
state local-as 65100
state peer-type EXTERNAL
state auth-password ""
state send-community BOTH
state description 1.1.1.1
state session-state ESTABLISHED
state last-established 1592486741
state established-transitions 5
state supported-capabilities [MPBGP ASN32 ROUTE_REFRESH GRACEFUL_RESTART ADD_PATHS]
--- snip ---
transport state tcp-mss 8922
transport state mtu-discovery false
transport state passive-mode false
transport state local-address 2.1.1.3
transport state local-port 45875
transport state remote-address 1.1.1.1
transport state remote-port 179
--- snip ---
ebgp-multihop state multihop-ttl 5

```

## Designated forwarder and ESI mapping.

```
root@leaf1# show evpn esi-info | tab
evpn esi-info counters esi-pruned-pkts 548221
evpn esi-info counters esi-pruned-octets 548221000
evpn esi-info esi
                                                                                                        ENTRY
                               LOCAL      OPER    LACP                                         OUR      NETWORK   DESIGNATED
ENTRY ESI                      INTERFACE  STATUS  STATUS  ORDERED PE LIST                      ORDINAL  INSTANCE  FORWARDER
-----------------------------------------------------------------------------------------------------------------------------------
00:01:10:10:10:10:10:10:10:10  bond10     UP      ACTIVE  [ 111.111.111.111 111.111.111.112 ]  0        vlan1001  111.111.111.112 ---> IP address of the Leaf2 loopback.
                                                                                                        vlan1002  111.111.111.111
                                                                                                        vlan1003  111.111.111.112
                                                                                                        vlan1004  111.111.111.111
                                                                                                        vlan1005  111.111.111.112
                                                                                                        vlan1006  111.111.111.111
                                                                                                        vlan1007  111.111.111.112
                                                                                                        vlan1008  111.111.111.111
```

```

root@Leaf2# show running-config overlay
overlay local-tunnel-endpoint 0
 source-interface loopback1
!

root@Leaf2# show running-config interface loopback1
interface loopback1
 type    softwareLoopback
 enabled true
 subinterface 0
  ipv4 address 111.111.111.112
   prefix-length 32
  exit
 exit
!
root@Leaf2#

```

## Anycast gateway on the device.

```
evpn anycast-gateway-mac aa:aa:aa:aa:aa:aa
```


This anycast gateway MAC configuration is OPTIONAL in ArcOS. If not configured the anycast MAC address is virtual router MAC address 00:00:5e:00:01:01. The reason to configure this again is for inter-op reasons, or when one needs to explicitly define the anycast MAC address.

## Mac mobility

```
evpn duplicate-mac-detection window 60
evpn duplicate-mac-detection threshold 4
evpn duplicate-mac-detection auto-recovery-time 5
```

### The default values are:
• Window of 60 seconds
• Threshold of 5 moves
• An auto-recovery time of “0”, a MAC that exceeds thresholds will be blocked forever.


## Arp ND supression.

* Enabled by default.


```
network-instance vlan10
vni 1010
local-tunnel-endpoint-id 0
arp-nd-suppression false
```

## Understanding and verifying the BGP EVPN overlay.

```
leaf1# show network-instance default protocol BGP default all-neighbor * afi-safi *
state prefixes
all-neighbor
NEIGHBOR AFI SAFI TOTAL TOTAL
ADDRESS NAME RECEIVED SENT RECEIVED SENT WITHDRAWN
---------------------------------------------------------------------
2.1.1.3 L2VPN_EVPN 51 65 56 76 10
2.1.1.4 L2VPN_EVPN 51 65 56 100 10
192.168.0.1 IPV4_UNICAST 8 10 8 14 0
192.168.0.7 IPV4_UNICAST 6 10 6 14 0
```

## Layer2 L2 interface status.

```
root@leaf1# show interface swp3-6 | select oper-status | select admin-status | select
counters in-unicast-pkts | select counters out-unicast-pkts | select counters in-bits-rate-
brief | select counters out-bits-rate-brief
interface
IN OUT
ADMIN OPER UNICAST UNICAST IN BITS RATE OUT BITS
NAME STATUS STATUS PKTS PKTS BRIEF RATE BRIEF
----------------------------------------------------------------------
swp3 UP UP 0 7 17194436608.0 15984336896.0
swp4 UP UP 1178 75621 0 bps 304 bps
swp5 UP UP 0 0 15984336896.0 15984336896.0
swp6 UP UP 0 74967 0 bps 304 bps
```

## Multihomed interfaces.

```
leaf2# show interface bond* | select oper-status | select admin-status | select counters in-
unicast-pkts | select counters out-unicast-pkts | select aggregation lag-type
interface
IN OUT
ADMIN OPER UNICAST UNICAST LAG
NAME STATUS STATUS PKTS PKTS TYPE
-----------------------------------------------
bond0 UP UP 0 69674 LACP
bond1 UP UP 93137 167578 LACP
bond2 UP UP 0 0 LACP
```

##  LACP Interfaces (Bond0 shown below)

```
leaf2# show lacp interface bond0 | notab
lacp interface bond0
state name bond0
state interval FAST
state system-id-mac 00:10:10:10:10:10
member swp3
state interface swp3
state timeout SHORT
state synchronization IN_SYNC
state aggregatable true
state collecting true
state distributing true
```

## IMET (Inclusive Multicast Ethernet Tag Routes) and VXLAN BUM traffic replication.

The IMET route is used to replicate Broadcast, Multicast and Unknown Unicast (BUM) traffic to interested receivers in the same VLAN. ArcOS uses Ingress Replication (IR) as a replication method, meaning the IP underlay can be unicast only.

Unless ARP/ND suppression has been explicitly disabled; once hosts are learnt through BGP then flooding of ARP is essentially eliminated because the VTEPs install these remote ARP entries locally and respond to any local ARP requests from clients. This is similarly true for IPv6 neighbor discovery (ND) requests.

### Verify the IMET routes have been received the and the BUM replication entries created on Leaf1.

```

leaf1# show network-instance default protocol BGP default rib afi-safi L2VPN_EVPN loc-rib
route route-type 3 | select state path-types
loc-rib route
PREFIX ORIGIN PATH ID PATH TYPES
-----------------------------------------------------------------------------------------
1.1.1.1:10:[3][0][32][1.1.1.1] 0.0.0.0 0 [ BEST_PATH EXPORTED_PATH ]
1.1.1.1:10:[3][0][32][1.1.1.2] 2.1.1.3 4525280622281023 [ BEST_PATH IMPORTED_PATH ]
1.1.1.1:10:[3][0][32][1.1.1.3] 2.1.1.3 4525332161888603 [ BEST_PATH IMPORTED_PATH ]
1.1.1.1:10:[3][0][32][1.1.1.4] 2.1.1.3 4525422356201832 [ BEST_PATH IMPORTED_PATH ]
1.1.1.1:20:[3][0][32][1.1.1.1] 0.0.0.0 0 [ BEST_PATH EXPORTED_PATH ]
1.1.1.1:20:[3][0][32][1.1.1.2] 2.1.1.3 4525276327313725 [ BEST_PATH IMPORTED_PATH ]
1.1.1.1:20:[3][0][32][1.1.1.3] 2.1.1.3 4525327866921305 [ BEST_PATH IMPORTED_PATH ]
1.1.1.1:20:[3][0][32][1.1.1.4] 2.1.1.3 4525426651169129 [ BEST_PATH IMPORTED_PATH ]
1.1.1.2:10:[3][0][32][1.1.1.2] 2.1.1.3 319 [ BEST_PATH IMPORT_SOURCE_PATH
1.1.1.2:10:[3][0][32][1.1.1.2] 2.1.1.4 280 [ IMPORT_SOURCE_PATH ]
1.1.1.2:20:[3][0][32][1.1.1.2] 2.1.1.3 317 [ BEST_PATH IMPORT_SOURCE_PATH
1.1.1.2:20:[3][0][32][1.1.1.2] 2.1.1.4 278 [ IMPORT_SOURCE_PATH ]
1.1.1.3:10:[3][0][32][1.1.1.3] 2.1.1.3 347 [ BEST_PATH IMPORT_SOURCE_PATH
1.1.1.3:10:[3][0][32][1.1.1.3] 2.1.1.4 262 [ IMPORT_SOURCE_PATH ]
1.1.1.3:20:[3][0][32][1.1.1.3] 2.1.1.3 345 [ BEST_PATH IMPORT_SOURCE_PATH
1.1.1.3:20:[3][0][32][1.1.1.3] 2.1.1.4 260 [ IMPORT_SOURCE_PATH ]
1.1.1.4:10:[3][0][32][1.1.1.4] 2.1.1.3 360 [ BEST_PATH IMPORT_SOURCE_PATH
1.1.1.4:10:[3][0][32][1.1.1.4] 2.1.1.4 296 [ IMPORT_SOURCE_PATH ]
1.1.1.4:20:[3][0][32][1.1.1.4] 2.1.1.3 361 [ BEST_PATH IMPORT_SOURCE_PATH
1.1.1.4:20:[3][0][32][1.1.1.4] 2.1.1.4 297 [ IMPORT_SOURCE_PATH ]

```


