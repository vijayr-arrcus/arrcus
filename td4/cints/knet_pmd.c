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
filter.match_flags = BCM_PKTIO_FILTER_M_INGPORT;
filter.m_ingport = 82;
print bcm_pktio_filter_create(unit, &filter);
