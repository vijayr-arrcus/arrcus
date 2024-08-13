# show commands to debug EVPN:

```
show overlay local-tunnel-endpoint
overlay local-tunnel-endpoint
      LTEP
LTEP  RUN    SOURCE                                 UDP
ID    STATE  INTERFACE  SOURCE IP    ENCAPSULATION  PORT
----------------------------------------------------------
0     UP     loopback0  28.28.28.28  VXLAN          4789

root@Ufi-TH5-Leaf1#
```

```
root@Ufi-TH5-Leaf1#
root@Ufi-TH5-Leaf1# show overlay vni-all
overlay vni-all
      VNI                                                                                      INGRESS  INGRESS  INGRESS  EGRESS   EGRESS   EGRESS   INGRESS  INGRESS  INGRESS  EGRESS   EGRESS  EGRESS
      RUN    VNI      LTEP  VNI      IP OR                 VRF                         STATIC  UNICAST  MCAST    BCAST    UNICAST  MCAST    BCAST    UNICAST  MCAST    BCAST    UNICAST  MCAST   BCAST
VNID  STATE  TYPE     ID    IFINDEX  MAC VRF   VRF ID      IFINDEX  VNI ROUTER MAC     RTEPS   PACKETS  PACKETS  PACKETS  PACKETS  PACKETS  PACKETS  BYTES    BYTES    BYTES    BYTES    BYTES   BYTES
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
2000  UP     ROUTED   0     153074   Tenant-B  1073741827  10002    5c:07:58:a4:e0:03  -       0        0        0        0        0        0        0        0        0        0        0       0
2001  UP     BRIDGED  0     153075   vlan2001  2001        20007    -                  -       0        0        0        0        0        0        0        0        0        0        0       0
1000  UP     ROUTED   0     152074   Tenant-A  1073741826  10001    5c:07:58:a4:e0:03  -       0        0        0        0        0        0        0        0        0        0        0       0
1001  UP     BRIDGED  0     152075   vlan1001  1001        20005    -                  -       0        0        0        0        0        0        0        0        0        0        0       0
1002  UP     BRIDGED  0     152076   vlan1002  1002        20006    -                  -       0        0        0        0        0        0        0        0        0        0        0       0

root@Ufi-TH5-Leaf1#
```

```
root@Ufi-TH5-Leaf1#
root@Ufi-TH5-Leaf1# show network-instance vlan1001 vni
vni
      VNI                                                     VNI             INGRESS  INGRESS  INGRESS  EGRESS   EGRESS   EGRESS   INGRESS  INGRESS  INGRESS  EGRESS   EGRESS  EGRESS
VNI   RUN    VNI      LTEP  VNI      IP OR     VRF   VRF      ROUTER  STATIC  UNICAST  MCAST    BCAST    UNICAST  MCAST    BCAST    UNICAST  MCAST    BCAST    UNICAST  MCAST   BCAST
ID    STATE  TYPE     ID    IFINDEX  MAC VRF   ID    IFINDEX  MAC     RTEPS   PACKETS  PACKETS  PACKETS  PACKETS  PACKETS  PACKETS  BYTES    BYTES    BYTES    BYTES    BYTES   BYTES
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
1001  UP     BRIDGED  0     152075   vlan1001  1001  20005    -       -       0        0        0        0        0        0        0        0        0        0        0       0

root@Ufi-TH5-Leaf1# show network-instance ?
Description: Network instance information
Possible completions:
  Tenant-A       Network-instance name
  Tenant-B       Network-instance name
  default        Network-instance name
  management     Network-instance name
  vlan1001       Network-instance name
  vlan1002       Network-instance name
  vlan2001       Network-instance name
  ---
  displaylevel   Depth to show
  |              Output modifiers
  <cr>
root@Ufi-TH5-Leaf1# show network-instance Tenant-A vni
vni
      VNI                                                                                     INGRESS  INGRESS  INGRESS  EGRESS   EGRESS   EGRESS   INGRESS  INGRESS  INGRESS  EGRESS   EGRESS  EGRESS
VNI   RUN    VNI     LTEP  VNI      IP OR                 VRF                         STATIC  UNICAST  MCAST    BCAST    UNICAST  MCAST    BCAST    UNICAST  MCAST    BCAST    UNICAST  MCAST   BCAST
ID    STATE  TYPE    ID    IFINDEX  MAC VRF   VRF ID      IFINDEX  VNI ROUTER MAC     RTEPS   PACKETS  PACKETS  PACKETS  PACKETS  PACKETS  PACKETS  BYTES    BYTES    BYTES    BYTES    BYTES   BYTES
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
1000  UP     ROUTED  0     152074   Tenant-A  1073741826  10001    5c:07:58:a4:e0:03  -       0        0        0        0        0        0        0        0        0        0        0       0

root@Ufi-TH5-Leaf1#
```

```
show network-instance vlan100 l2rib mac-ipv4-entries | tab
l2rib mac-ipv4-entries entry
                                                                                                                                                             CONTROL
IPV4                          L3 LOCAL   L2 LOCAL        ORIGIN    DOMAIN   SEQ                                    NH                 ENCAP  L2     CONTROL  WORD     FLOW   NH NI
ADDRESS    MAC ADDRESS        INTERFACE  INTERFACE  ESI  PROTOCOL  NAME     NUM  LAST UPDATED                      TYPE  NEXT HOP     TYPE   LABEL  WORD     SEQ NUM  LABEL  NAME     RESOLVED
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
100.1.1.1  0a:bb:cc:00:00:01  -          -          -    BGP       WAN      1    2024-04-02T23:42:16.769524-00:00  -     10.10.10.10  VXLAN  10     -        -        -      default  true
100.1.1.2  0a:bb:cc:00:00:02  -          -          -    BGP       default  1    2024-04-02T23:42:16.770447-00:00  -     20.20.20.20  VXLAN  10     -        -        -      default  true
---

```
