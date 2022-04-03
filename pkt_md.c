cint_reset();
bcm_pktio_netif_t netif;
bcm_pktio_filter_t filter;
int unit = 0;
bcm_pktio_filter_t_init(&filter);
filter.type = BCM_PKTIO_FILTER_T_RX_PKT;
filter.priority = 1;
filter.dest_type = BCM_PKTIO_DEST_T_BCM_RX_API;
filter.dest_type = BCM_PKTIO_DEST_T_NETIF;
sal_memcpy(filter.desc, "Filter bcm", 20);
filter.dest_id = 1;
filter.match_flags = BCM_PKTIO_FILTER_M_INGPORT;
filter.m_ingport = 45;
print bcm_pktio_filter_create(unit, &filter);

pw stop
pw start report +raw +pmd +decode
