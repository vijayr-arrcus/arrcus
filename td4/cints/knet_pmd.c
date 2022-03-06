cint_reset();
bcm_pktio_netif_t netif;
bcm_pktio_filter_t filter;
bcm_mac_t sys_mac = {0x00, 0x02, 0x03, 0x04, 0x06, 0x07};
sal_memset(&netif, 0, sizeof(netif));
netif.port = 82;
netif.flags |= BCM_PKTIO_NETIF_F_RCPU_ENCAP | BCM_PKTIO_NETIF_F_BIND_TX_PORT;
netif.max_frame_size = 9000;
sal_memcpy(netif.name, "netdev-swp18", 20);
netif.mac_addr = sys_mac;
int unit = 0;
print bcm_pktio_netif_create(unit, &netif);

bcm_pktio_filter_t_init(&filter);
filter.type = BCM_PKTIO_FILTER_T_RX_PKT;
filter.priority = 10;
filter.dest_type = BCM_PKTIO_DEST_T_NETIF;
sal_memcpy(filter.desc, "Filter netdev-swp18-100", 20);
filter.dest_id = netif.id;
filter.match_flags = BCM_PKTIO_FILTER_M_REASON;
BCM_PKTIO_REASON_SET(filter.m_flex_reason,
                     BCM_PKTIO_RXPMD_FLEX_REASON_L2_SRC_STATIC_MOVE);
print bcm_pktio_filter_create(unit, &filter);

[539914.199030] Rx packet sent up to ndev19 (64 bytes).

cint_reset();
bcm_pktio_netif_t netif;
bcm_pktio_filter_t filter;
bcm_mac_t sys_mac = {0x00, 0x02, 0x03, 0x04, 0x06, 0x07};
sal_memset(&netif, 0, sizeof(netif));
netif.port = 82;
netif.flags |= BCM_PKTIO_NETIF_F_BIND_TX_PORT;
netif.max_frame_size = 9000;
sal_memcpy(netif.name, "netdev-swp18", 20);
netif.mac_addr = sys_mac;
int unit = 0;
print bcm_pktio_netif_create(unit, &netif);

bcm_pktio_filter_t_init(&filter);
filter.type = BCM_PKTIO_FILTER_T_RX_PKT;
filter.priority = 10;
filter.dest_type = BCM_PKTIO_DEST_T_BCM_RX_API;
sal_memcpy(filter.desc, "Filter netdev-swp18-100", 20);
filter.dest_id = netif.id;
filter.match_flags = BCM_PKTIO_FILTER_M_INGPORT;
filter.m_ingport = 82;
print bcm_pktio_filter_create(unit, &filter);


bcm_pktio_filter_destroy(0, )
bcm_pktio_netif_destroy(0, )

cint_reset();
bcm_pktio_netif_t netif;
bcm_pktio_filter_t filter;
bcm_mac_t sys_mac = {0x00, 0x02, 0x03, 0x04, 0x06, 0x07};
sal_memset(&netif, 0, sizeof(netif));
netif.port = 0;
netif.max_frame_size = 9000;
sal_memcpy(netif.name, "arc-mac-lrn-3", 20);
netif.mac_addr = sys_mac;
int unit = 0;
print bcm_pktio_netif_create(unit, &netif);

bcm_pktio_filter_t_init(&filter);
filter.type = BCM_PKTIO_FILTER_T_RX_PKT;
filter.priority = 1;
filter.dest_type = BCM_PKTIO_DEST_T_NETIF;
sal_memcpy(filter.desc, "arc-mac-lrn-3", 20);
filter.dest_id = netif.id;
filter.match_flags = BCM_PKTIO_FILTER_M_REASON;
BCM_PKTIO_REASON_SET(filter.m_flex_reason, BCM_PKTIO_RXPMD_FLEX_REASON_L2_SRC_STATIC_MOVE);
print bcm_pktio_filter_create(unit, &filter);


cint_reset();
uint32_t rxpmd_buf[30] = {0x009f5228, 0x00000000, 0x0000002a, 0x01310005, 0x80300000, 0x00020000, 0x00000000, 0x00000800 ,0x00000052 ,0x00000000 ,0x00000000 ,0x00000000 ,0x00520032 ,0x00000000 ,0x00000000 ,0x02020000 ,0x00000004 ,0x00000000};

bcm_pktio_pkt_t pkt;
pkt.unit = 0;
pkt.flags = 2;
//sal_memcpy(rxpmd_buf, pkt.pmd.data, 30);
int i =0;
while (i < 30) {
  pkt.pmd.data[i] = rxpmd_buf[i];
  i++;
}
print pkt;
uint32_t src_port = 0;
print bcm_pktio_pmd_field_get(0, &pkt, bcmPktioPmdTypeRx, 6, &src_port);
print src_port;
print pkt;


