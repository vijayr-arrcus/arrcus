void
create_knet_interface (int unit, bcm_port_t port,
                       bcm_vlan_t vlan, char *interface_name,
                       bcm_mac_t local_mac)
{
  bcm_error_t rv;
  bcm_pktio_filter_t filter;
  bcm_pktio_netif_t netif;

  sal_memset(&netif, 0, sizeof(netif));
  netif.flags |= BCM_PKTIO_NETIF_F_BIND_TX_PORT;
  netif.vlan = vlan;
  netif.port = port;

  sal_strcpy(netif.name, interface_name);
  netif.mac_addr = local_mac;
  rv = bcm_pktio_netif_create(unit, &netif);

  sal_memset(&filter, 0, sizeof(filter));
  sal_strcpy(filter.desc, "Broadcast Drop");
  filter.type = BCM_PKTIO_FILTER_T_RX_PKT;
  filter.priority = 250;    /* Low but not lowest */
  /* Drop packet */
  filter.dest_type = BCM_PKTIO_DEST_T_NULL;
  filter.dest_id = netif.id;
  /* Set up raw match on first 6 bytes of the packet */
  filter.match_flags = BCM_PKTIO_FILTER_M_RAW;
  filter.raw_size = 6;
  for (idx = 0; idx < filter.raw_size; idx++) {
      filter.m_raw_data[idx] = 0xFF;        /* Raw data to match. */
      filter.m_raw_mask[idx] = 0xFF;        /* Raw data mask to match. */
  }
  rv = bcm_pktio_filter_create(unit, &filter);
}

bcm_port_config_t configP;
bcm_pbmp_t ports_pbmp;
int rv=0;
rv=bcm_port_config_get(0, &configP);
print configP.e;
bcm_pbmp_t port_bitmap;
bcm_port_t port;
char netdev_name[10];
int unit = 0;

BCM_PBMP_CLEAR(port_bitmap);
BCM_PBMP_ASSIGN(port_bitmap, configP.e);

BCM_PBMP_ITER(port_bitmap, port) {
  snprintf(netdev_name, 10, "swp%d", port);
  ifindex = 1000+port;
  bcmsdk_create_netdev(unit, port, netdev_name, &ifindex);
}
