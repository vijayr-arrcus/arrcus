cint_reset();
int unit = 0;
int station_id = 0;
bcm_mac_t mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
int rv = BCM_E_NONE;
bcm_l2_station_t l2_station;
bcm_l2_station_t_init(&l2_station);
l2_station.flags |= (BCM_L2_STATION_UNDERLAY | BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6 | BCM_L2_STATION_MPLS | BCM_L2_STATION_ARP_RARP);
sal_memcpy(l2_station.dst_mac, mac_in, 6);
sal_memset(l2_station.dst_mac_mask, 0xff, 6);
rv = bcm_l2_station_add(unit, &station_id, &l2_station);
print rv;
printf("%s", bcm_errmsg(rv));


