
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



