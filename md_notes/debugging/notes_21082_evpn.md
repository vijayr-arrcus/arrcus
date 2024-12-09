[[toc]]

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
