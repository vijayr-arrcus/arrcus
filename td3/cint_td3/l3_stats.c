/*
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *  ingress_port     |                              |  egress_port
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 */

cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port;
bcm_if_t g_intf_id_ingress;
bcm_if_t g_intf_id_egress;
bcm_if_t g_ingress_id;
bcm_if_t g_egr_obj_id_ingress;
bcm_if_t g_egr_obj_id_egress;
uint32_t g_l3_iif_mode_id;
uint32_t g_l3_iif_cntr_id;
uint32_t g_intf_egress_stat_id;
uint32_t g_egr_if_mode_id;
uint32_t g_egr_if_cntr_id;

/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
*/
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

  int i=0,port=0,rv=0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
    printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
    return rv;
  }

  ports_pbmp = configP.e;
  for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
    if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
      port_list[port]=i;
      port++;
    }
  }

  if (( port == 0 ) || ( port != num_ports )) {
      printf("portNumbersGet() failed \n");
      return -1;
  }

  return BCM_E_NONE;

}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
  return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU and drop the packets. This is
 * to avoid continuous loopback of the packet.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

  return BCM_E_NONE;
}
/*
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
  int port_list[2];

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  ingress_port = port_list[0];
  egress_port = port_list[1];

  if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", ingress_port);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
      printf("egress_port_setup() failed for port %d\n", egress_port);
      return -1;
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}
bcm_error_t
l3_intf_create (int unit,
                bcm_mac_t *mac,
                bcm_vlan_t vlan,
                bcm_vrf_t vrf,
                bcm_if_t *intf_id)
{
  bcm_error_t rv;
  bcm_l3_intf_t l3_intf;

  bcm_l3_intf_t_init(&l3_intf);
  l3_intf.l3a_intf_id = -1;
  sal_memcpy(l3_intf.l3a_mac_addr, mac, 6);
  l3_intf.l3a_vid = vlan;
  l3_intf.l3a_vrf = vrf;
  rv = bcm_l3_intf_create(unit, &l3_intf);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
  }
  *intf_id = l3_intf.l3a_intf_id;
  return rv;
}


bcm_error_t
l3_ingress_create (int unit,
                   bcm_vrf_t vrf,
                   bcm_if_t *ingress_id)
{
  bcm_error_t rv;
  bcm_l3_ingress_t l3_ingress;

  bcm_l3_ingress_t_init(&l3_ingress);
  l3_ingress.vrf = vrf;
  rv = bcm_l3_ingress_create(unit, &l3_ingress, ingress_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  return rv;
}

bcm_error_t
l3_egress_create (int unit, bcm_if_t intf_id,
                  bcm_mac_t *remote_mac, bcm_vlan_t vlan,
                  bcm_gport_t out_gport,
                  bcm_if_t *egr_obj_id)
{
  bcm_error_t rv;
  bcm_l3_egress_t l3_egr_obj;
/* Create L3 egress object for egress port */
  bcm_l3_egress_t_init(&l3_egr_obj);
  l3_egr_obj.flags = 0;
  l3_egr_obj.intf = intf_id;
  sal_memcpy(l3_egr_obj.mac_addr, remote_mac, 6);
  l3_egr_obj.vlan = vlan;
  l3_egr_obj.port = out_gport;
  rv = bcm_l3_egress_create(unit, 0, &l3_egr_obj, egr_obj_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  return rv;
}

bcm_error_t
route_create (int unit, bcm_ip_t dst_ip,
              bcm_vrf_t vrf,
              bcm_if_t egr_obj_id)
{
  bcm_error_t rv;
/* Add L3 Route Entry one direction */
  bcm_l3_route_t route;
  bcm_l3_route_t_init(&route);
  route.l3a_subnet = dst_ip;
  route.l3a_ip_mask = 0xFFFFFFFF;
  route.l3a_intf = egr_obj_id;
  route.l3a_vrf = vrf;
  rv = bcm_l3_route_add(unit, &route);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  return rv;
}


bcm_error_t config_l3 (int unit)
{
  bcm_error_t rv;

  bcm_vlan_control_vlan_t vlan_control;
  bcm_gport_t ingress_gport, egress_gport;
  bcm_ip_t l3_routing_dest_ip_1 = 0x01010101; /* 1.1.1.1 */
  bcm_ip_t l3_routing_dest_ip_2 = 0x02020202; /* 2.2.2.2 */
  bcm_mac_t local_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
  bcm_mac_t local_mac_2 = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
  bcm_mac_t remote_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
  bcm_mac_t remote_mac_2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
  bcm_vrf_t vrf = 0;
  bcm_vlan_t ingress_vlan = 21;
  bcm_vlan_t egress_vlan = 22;


  { // gport gets
    rv = bcm_port_gport_get(unit, ingress_port, &ingress_gport);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_port_gport_get(unit, egress_port, &egress_gport);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
    }
  }

  { // Port vlan association
    /* Create ingress_vlan */
    rv = bcm_vlan_create(unit, ingress_vlan);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_vlan_gport_add(unit, ingress_vlan, ingress_gport, 0);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Create egress_vlan */
    rv = bcm_vlan_create(unit, egress_vlan);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_vlan_gport_add(unit, egress_vlan, egress_gport, 0);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
    }
  }

  { // Setup ingress egress modes.
    /* Enable Egress Interface Mode */
    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(.., bcmSwitchL3EgressMode, ..): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Enable Ingress Interface Mode */
    rv = bcm_switch_control_set(unit, bcmSwitchL3IngressMode, TRUE);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(.., bcmSwitchL3IngressMode, ..): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, TRUE);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(.., bcmSwitchL3IngressInterfaceMapSet, ..): %s.\n", bcm_errmsg(rv));
      return rv;
    }
  }

  { // setup l3 ingress L3_IIF.

    rv = l3_ingress_create(unit, vrf, &g_ingress_id);

    if (BCM_FAILURE(rv)) {
      printf("L3 ingress object creation failed %s", bcm_errmsg(rv));
      return rv;
    }

    bcm_vlan_control_vlan_t_init(&vlan_control);
    rv = bcm_vlan_control_vlan_get(unit, ingress_vlan, &vlan_control);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_get(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    vlan_control.ingress_if = g_ingress_id;
    rv = bcm_vlan_control_vlan_set(unit, ingress_vlan, vlan_control);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }
  }

  { // setup l3 inf object for ingress port
    rv = l3_intf_create(unit,
                        local_mac_1,
                        ingress_vlan,
                        vrf,
                        &g_intf_id_ingress);
    if (BCM_FAILURE(rv)) {
      printf("Erro creating l3 intf object for ingress port %s \n", bcm_errmsg(rv));
      return rv;
    }
  }

  { // setup l3 inf object for egress port
    rv = l3_intf_create(unit,
                        local_mac_2,
                        egress_vlan,
                        vrf,
                        &g_intf_id_egress);
    if (BCM_FAILURE(rv)) {
      printf("Erro creating l3 intf object for egress port %s \n", bcm_errmsg(rv));
      return rv;
    }
  }

  { // setup egress object for NH through egress port
    rv = l3_egress_create(unit, g_intf_id_egress, remote_mac_2,
                          egress_vlan, egress_gport, &g_egr_obj_id_egress);
    if (BCM_FAILURE(rv)) {
      printf("Erro creating Egress object for egress port %s \n", bcm_errmsg(rv));
      return rv;
    }
  }

  {// setup route for a NH/egress object out of egress_port.
    rv = route_create(unit, l3_routing_dest_ip_2,
                      vrf, g_egr_obj_id_egress);
  }
  return BCM_E_NONE;
}

/* Get L3IIF flex cnt stats*/
bcm_error_t l3iif_flex_stat_get(int unit, bcm_if_t l3_iif_id)
{
  bcm_error_t rv;
  bcm_stat_value_t l3iif_flex_cnt_val[3];
  uint32 l3iif_flex_cnt_idx[3] = {0, 1, 2};
  uint32 num_counters = 3, i;

  rv = bcm_l3_ingress_stat_counter_sync_get(unit, l3_iif_id, bcmL3StatInPackets, num_counters, l3iif_flex_cnt_idx, l3iif_flex_cnt_val);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_stat_counter_sync_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  for (i = 0; i < num_counters; i++) {
    printf("The packet counter value at offset %d is 0x%x%x \n",
            i,
            COMPILER_64_HI(l3iif_flex_cnt_val[i].packets64),
            COMPILER_64_LO(l3iif_flex_cnt_val[i].packets64));
  }


  rv = bcm_l3_ingress_stat_counter_sync_get(unit, l3_iif_id, bcmL3StatInBytes, num_counters, l3iif_flex_cnt_idx, l3iif_flex_cnt_val);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_stat_counter_sync_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  for (i = 0; i < num_counters; i++) {
    printf("The byte counter value at offset %d is 0x%x%x \n",
            i,
            COMPILER_64_HI(l3iif_flex_cnt_val[i].bytes),
            COMPILER_64_LO(l3iif_flex_cnt_val[i].bytes));
  }

  return BCM_E_NONE;
}

bcm_error_t
l3_intf_stat_get (int unit, uint32_t l3intf_id)
{
  bcm_error_t rv;
  int count = 1;
  uint32 num_entries = 1;
  uint32_t counter_index = 0;
  bcm_flexctr_counter_value_t counter_value;

  /* Get counter value. */
  sal_memset(&counter_value, 0, sizeof(counter_value));
  rv = bcm_flexctr_stat_get(unit, l3intf_id, num_entries, &counter_index,
                            &counter_value);
  if (BCM_FAILURE(rv)) {
    printf("Error in bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* print counter value. */
  printf("Tunnel initiated Stats: %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0]),
         COMPILER_64_LO(counter_value.value[1]));
  if (COMPILER_64_LO(counter_value.value[0]) != count) {
    printf("Error in packet verification. Expected packet stat %d but got %d\n", count, COMPILER_64_LO(counter_value.value[0]));
  };
  return rv;
}

bcm_error_t
l3_egress_stat_get (int unit, uint32_t egr_id)
{
  bcm_error_t rv;
  bcm_stat_value_t l3egrif_flex_cnt_val[3];
  uint32 l3egrif_flex_cnt_idx[3] = {0, 1, 2};
  uint32 num_counters = 3, i;

  rv = bcm_l3_egress_stat_counter_sync_get(unit, egr_id, bcmL3StatOutPackets, num_counters, l3egrif_flex_cnt_idx, l3egrif_flex_cnt_val);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_stat_counter_sync_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  for (i = 0; i < num_counters; i++) {
    printf("The packet counter value at offset %d is 0x%x%x \n",
            i,
            COMPILER_64_HI(l3egrif_flex_cnt_val[i].packets64),
            COMPILER_64_LO(l3egrif_flex_cnt_val[i].packets64));
  }


  rv = bcm_l3_egress_stat_counter_sync_get(unit, egr_id, bcmL3StatOutBytes, num_counters, l3egrif_flex_cnt_idx, l3egrif_flex_cnt_val);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_stat_counter_sync_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  for (i = 0; i < num_counters; i++) {
    printf("The byte counter value at offset %d is 0x%x%x \n",
            i,
            COMPILER_64_HI(l3egrif_flex_cnt_val[i].bytes),
            COMPILER_64_LO(l3egrif_flex_cnt_val[i].bytes));
  }

  return BCM_E_NONE;
}

void verify (int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");

 /* Send 3 packets. KNOWN_L3_UNICAST
 *  Ethernet: dst<00:00:00:00:11:11> src<00:00:00:00:00:01> Tagged Packet ProtID<0x8100> Ctrl<0x0015> Internet Protocol (IP)
 *  IP: V(4) src<2.2.2.2> dst<1.1.1.1> hl<5> service-type<0> tl<46> id<0> frg-off<0> ttl<64> > chk-sum<0x73cc>
 */
  printf("Sending 3 KNOWN_L3_UNICAST packets..\n");
  snprintf(str, 512, "tx 3 pbm=%d data=0x0000000011110000000000018100001508004500002E0000000040FF73CC0202020201010101000000000000000000000000000000000000000000000000000058863ECA; sleep 1", ingress_port);
  bshell(unit, str);

  /*Retrieve L3IIF flex counters and print them.*/
  printf("\nDumping L3IIF counters after sending 3 KNOWN_L3_UNICAST packets..\n");
  l3iif_flex_stat_get(unit, g_ingress_id);
  l3_intf_stat_get(unit, g_intf_id_egress);
  l3_egress_stat_get(unit, g_egr_obj_id_egress);

  /* Send another 2 packets.. KNOWN_L3_UNICAST
 *  Ethernet: dst<00:00:00:00:11:11> src<00:00:00:00:00:01> Tagged Packet ProtID<0x8100> Ctrl<0x0015> Internet Protocol (IP)
 *  IP: V(4) src<2.2.2.2> dst<1.1.1.1> hl<5> service-type<0> tl<46> id<0> frg-off<0> ttl<64> > chk-sum<0x73cc>
 */

  printf("Sending another 2 KNOWN_L3_UNICAST packets..\n");
  snprintf(str, 512, "tx 2 pbm=%d data=0x0000000011110000000000018100001508004500002E0000000040FF73CC0202020201010101000000000000000000000000000000000000000000000000000058863ECA; sleep 1", ingress_port);
  bshell(unit, str);

  /*Retrieve L3IIF flex counters and print them.*/
  printf("\nDumping L3IIF counters after sending another 2 KNOWN_L3_UNICAST packets..\n");
  l3iif_flex_stat_get(unit, g_ingress_id);
  l3_intf_stat_get(unit, g_intf_id_egress);
  l3_egress_stat_get(unit, g_egr_obj_id_egress);


  /*Send another 2 packets.. UNKNOWN_L3_UNICAST
   * Ethernet: dst<00:00:00:00:11:11> src<00:00:00:00:00:01> Tagged Packet ProtID<0x8100> Ctrl<0x0015> Internet Protocol (IP)
   * IP: V(4) src<2.2.2.2> dst<1.1.1.2> hl<5> service-type<0> tl<46> id<0> frg-off<0> ttl<64> > chk-sum<0x73cb>
   */
  printf("Sending 2 UNKNOWN_L3_UNICAST packets..\n");
  snprintf(str, 512, "tx 2 pbm=%d data=0x0000000011110000000000018100001508004500002E0000000040FF73CB0202020201010102000102030405060708090A0B0C0D0E0F10111213141516171819A0F3086A; sleep 1", ingress_port);
  bshell(unit, str);

  /*Retrieve L3IIF flex counters and print them.*/
  printf("\nDumping L3IIF counters after sending  2 UNKNOWN L3 UNICAST packets..\n");
  l3iif_flex_stat_get(unit, g_ingress_id);
  l3_intf_stat_get(unit, g_intf_id_egress);
  l3_egress_stat_get(unit, g_egr_obj_id_egress);

  /*Send 2 L2 Broadcat packets..
   *Ethernet: dst<ff:ff:ff:ff:ff:ff> src<00:00:05:00:01:00> Tagged Packet ProtID<0x8100> Ctrl<0x0015> 802.3 Packet
   * IP: V(0) src<14.15.16.17> dst<18.19.20.21> hl<2> service-type<3> tl<1029> id<1543> frg-off<2057> ttl<10> > chk-sum<0x0c0d>
   */
  printf("Sending 2 L2 Broadcat packets..\n");
  snprintf(str, 512, "tx 2 pbm=%d data=0xFFFFFFFFFFFF00000500010081000015000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2FD75BBFBB; sleep 1", ingress_port);
  bshell(unit, str);

  /*Retrieve L3IIF flex counters and print them.*/
  printf("\nDumping L3IIF counters after sending 2 L2 Broadcast packets..\n");
  l3iif_flex_stat_get(unit, g_ingress_id);
  l3_intf_stat_get(unit, g_intf_id_egress);
  l3_egress_stat_get(unit, g_egr_obj_id_egress);
}

bcm_error_t
setup_l3_iif_stats (int unit, bcm_if_t ingress_if_id,
                    uint32_t *iif_mode_id,
                    uint32_t *cntr_id)
{
  bcm_error_t rv;
  uint32 l3iif_flex_cnt_id = 0;
  uint32 num_counters;
  uint32 total_counters = 3;
  int num_selectors=3;
  bcm_stat_group_mode_attr_selector_t attr_selectors[3];
  uint32 mode_id = 0;
  /* Selector0 for KNOWN_L3UC_PKT. Assigned to 1st counter. */
  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[0]);
  attr_selectors[0].counter_offset = 0;
  attr_selectors[0].attr = bcmStatGroupModeAttrPktType;
  attr_selectors[0].attr_value = bcmStatGroupModeAttrPktTypeKnownL3UC;

  /* Selector1 for UNKNOWN_L3UC_PKT Assigned to 2nd counter. */
  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[1]);
  attr_selectors[1].counter_offset = 1;
  attr_selectors[1].attr = bcmStatGroupModeAttrPktType;
  attr_selectors[1].attr_value = bcmStatGroupModeAttrPktTypeUnknownL3UC;

  /* Selector2 for L2_BC. Assigned to 3rd counter. */
  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[2]);
  attr_selectors[2].counter_offset = 2;
  attr_selectors[2].attr = bcmStatGroupModeAttrPktType;
  attr_selectors[2].attr_value = bcmStatGroupModeAttrPktTypeL2BC;


  /* Create customized stat mode */
  rv = bcm_stat_group_mode_id_create(unit, BCM_STAT_GROUP_MODE_INGRESS, total_counters, num_selectors, attr_selectors, &mode_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_stat_group_mode_id_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Attach stat to customized group mode */
  rv = bcm_stat_custom_group_create(unit, mode_id, bcmStatObjectIngL3Intf, &l3iif_flex_cnt_id, &num_counters);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_stat_custom_group_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_l3_ingress_stat_attach(unit, ingress_if_id, l3iif_flex_cnt_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_stat_attach(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  *cntr_id = l3iif_flex_cnt_id;
  *iif_mode_id = mode_id;
  return rv;
}

bcm_error_t
setup_l3_intf_stats (int unit, bcm_if_t intf_id,
                     uint32_t *l3_iif_cntr_id)
{
  uint32_t stat_cntr_id;
  int options = 0;
  bcm_error_t  rv;
  bcm_flexctr_action_t action;
  bcm_flexctr_index_operation_t *index_op = NULL;
  bcm_flexctr_value_operation_t *value_a_op = NULL;
  bcm_flexctr_value_operation_t *value_b_op = NULL;

  print bcm_flexctr_action_t_init(&action);
  action.flags = 0;
  action.source = bcmFlexctrSourceEgrL3Intf;
  action.mode = bcmFlexctrCounterModeNormal;
  action.index_num = 100; /* Number of required counters */

  index_op = &action.index_operation;
  index_op->object[0] = bcmFlexctrObjectConstZero;
  index_op->mask_size[0] = 7;
  index_op->shift[0] = 0;
  index_op->object[1] = bcmFlexctrObjectConstZero;
  index_op->mask_size[1] = 1;
  index_op->shift[1] = 0;

  /* Increase counter per packet. */
  value_a_op = &action.operation_a;
  value_a_op->select = bcmFlexctrValueSelectCounterValue;
  value_a_op->object[0] = bcmFlexctrObjectConstOne;
  value_a_op->mask_size[0] = 16;
  value_a_op->shift[0] = 0;
  value_a_op->object[1] = bcmFlexctrObjectConstZero;
  value_a_op->mask_size[1] = 1;
  value_a_op->shift[1] = 0;
  value_a_op->type = bcmFlexctrValueOperationTypeInc;

  /* Increase counter per packet bytes. */
  value_b_op = &action.operation_b;
  value_b_op->select = bcmFlexctrValueSelectPacketLength;
  value_b_op->type = bcmFlexctrValueOperationTypeInc;

  /* Create an ingress action */
  rv = bcm_flexctr_action_create(unit, options, &action, &stat_cntr_id);
  if (BCM_FAILURE(rv)) {
    printf("Error in bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
    return rv;
  }
  print stat_cntr_id;

  /* Attach counter action to route */
  rv = bcm_l3_intf_stat_attach(unit, intf_id, stat_cntr_id);
  if (BCM_FAILURE(rv)) {
    printf("Error in  bcm_tunnel_terminator_stat_attach: %s.\n", bcm_errmsg(rv));
  }

  *l3_iif_cntr_id = stat_cntr_id;

  return BCM_E_NONE;
}

bcm_error_t
setup_l3_egress_nh_stats (int unit,
                          bcm_if_t egr_if_id,
                          uint32_t *egr_mode_id,
                          uint32_t *l3_egress_if_stat_id)
{
  bcm_error_t rv;
  uint32 l3egrif_flex_cnt_id = 0;
  uint32 num_counters;
  uint32 total_counters = 3;
  int num_selectors=3;
  bcm_stat_group_mode_attr_selector_t attr_selectors[3];
  uint32 mode_id = 0;

  /* Selector0 for outer tag pri 0- Assigned to 1st counter. */
  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[0]);
  attr_selectors[0].counter_offset = 0;
  attr_selectors[0].attr = bcmStatGroupModeAttrOuterPri;
  attr_selectors[0].attr_value = 0;

  /* Selector1 for outer tag pri 1- Assigned to 2nd counter. */
  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[1]);
  attr_selectors[1].counter_offset = 1;
  attr_selectors[1].attr = bcmStatGroupModeAttrOuterPri;
  attr_selectors[1].attr_value = 1;

  /* Selector2 for outer tag pri 2- Assigned to 3rd counter. */
  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[2]);
  attr_selectors[2].counter_offset = 2;
  attr_selectors[2].attr = bcmStatGroupModeAttrOuterPri;
  attr_selectors[2].attr_value = 2;


  /* Create customized stat mode */
  rv = bcm_stat_group_mode_id_create(unit, BCM_STAT_GROUP_MODE_EGRESS, total_counters, num_selectors, attr_selectors, &mode_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stat_group_mode_id_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Attach stat to customized group mode */
  rv = bcm_stat_custom_group_create(unit, mode_id, bcmStatObjectEgrL3Intf, &l3egrif_flex_cnt_id, &num_counters);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stat_custom_group_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_l3_egress_stat_attach(unit, l3_egr_obj_id, l3egrif_flex_cnt_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_egress_stat_attach(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  *l3_egress_if_stat_id = l3egrif_flex_cnt_id;
  *egr_mode_id = mode_id;
  return rv;
}

bcm_error_t
setup_l3_stats (int unit)
{
  bcm_error_t rv;
  { // setup ingress stats
    rv = setup_l3_iif_stats(unit, g_ingress_id,
                            &g_l3_iif_mode_id, &g_l3_iif_cntr_id);
    if (BCM_FAILURE(rv)) {
      printf("l3 ingress stats setup failed %s \n", bcm_errmsg(rv));
      return rv;
    }
  }
  { // setup intf stats for egress port.
    rv = setup_l3_intf_stats(unit, g_intf_id_egress, &g_intf_egress_stat_id);
    if (BCM_FAILURE(rv)) {
      printf("l3 intf stats setup failed %s \n", bcm_errmsg(rv));
      return rv;
    }
  }
  { // setup egress NH stats.
    rv = setup_l3_egress_nh_stats(unit, g_egr_obj_id_egress,
                                  &g_egr_if_mode_id, &g_egr_if_cntr_id);
    if (BCM_FAILURE(rv)) {
      printf("l3 egress NH stats setup failed %s \n", bcm_errmsg(rv));
      return rv;
    }
  }
  return BCM_E_NONE;
}
/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in config_flex_cnt_l3_ingress())
 * c)demonstrates the functionality (Done in verify()).
 */
bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;
  bcm_if_t intf_id = 0;

  bshell(unit, "config show; a ; version ; cancun stat");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }
  printf("Completed test setup successfully.\n");

  if (BCM_FAILURE((rv = config_l3(unit)))) {
    printf("config_l3() failed.\n");
    return -1;
  }
  printf("Completed l3 config successfully\n");

  if (BCM_FAILURE((rv = setup_l3_stats(unit)))) {
    printf ("Failed l3 stats setup\n");
    return -1;
  }
  printf("Completed l3 stats setup successfully\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
