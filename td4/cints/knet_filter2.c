bcm_pktio_filter_t filter;
int unit = 0;
int priority = 100;
int port = 61;
int netif_id = 31;

filter.type = BCM_PKTIO_FILTER_T_RX_PKT;
filter.priority = priority;
filter.dest_id = netif_id;
filter.dest_type = BCM_PKTIO_DEST_T_NETIF;
filter.match_flags = BCM_PKTIO_FILTER_M_INGPORT;
filter.m_ingport = port;
print bcm_pktio_filter_create(unit, &filter);

bcm_pktio_filter_destroy(0, 38);
