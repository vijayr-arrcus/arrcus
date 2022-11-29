cint_reset();

bcm_pktio_netif_t netif;
bcm_pktio_filter_t filter;
int
sflow_create (int unit)
{
  int rv;

  sal_memset(&netif, 0, sizeof(netif));
  netif.port = 0;
  bcm_mac_t sys_mac = {0x00, 0x02, 0x03, 0x04, 0x06, 0x07};
  netif.mac_addr = sys_mac;
  netif.flags |= BCM_PKTIO_NETIF_F_RCPU_ENCAP;
  sal_memcpy(netif.name, "sflow-2", 20);


  rv = bcm_pktio_netif_create(unit, &netif);
  if (BCM_E_NONE != rv) {
    printf("Failed to create netif %s\n",
           bcm_errmsg(rv));
    return -1;
  }

  bcm_pktio_filter_t_init(&filter);
  filter.type = BCM_PKTIO_FILTER_T_RX_PKT;
  filter.priority = 1;
  filter.dest_type = BCM_PKTIO_DEST_T_NETIF;
  sal_memcpy(filter.desc, "Ingress sflow-2", 20);
  filter.match_flags = BCM_PKTIO_FILTER_M_REASON;
  filter.dest_id = netif.id;
  BCM_PKTIO_REASON_SET(filter.m_flex_reason,
                       BCM_PKTIO_RXPMD_FLEX_REASON_MIRROR_SAMPLER_SAMPLED);
``
  rv = bcm_pktio_filter_create(unit, &filter);
  if (BCM_E_NONE != rv) {
    printf("Failed to create filer %s\n", bcm_errmsg(rv));
    return -1;
  }
  printf("netif %d \n filter id %d \n", netif.id, filter.id);
  return 1;
}

print sflow_create(0);
