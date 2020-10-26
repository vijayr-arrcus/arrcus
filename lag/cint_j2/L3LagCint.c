/*
 * print lag_hashing_routing_main(0,13,14,1,100);
 * tx 1 psrc=1 DATA=00002222222200000000110181000011080045000023000100004000F6762121212121212101333333333333333333333333333333
 * tx 1 psrc=1 DATA=00002222222200001111111181000011080045000023000100004000F6952121210221212101333333333333333333333333333333
 * Commands to check the hashing happening and the corresponding tx ports ( TM port )
 *
 * pp vis eklp
 * pp vis ppi
 * sig get to=ipmf1
 */
void
print_port_config (int unit, char *str)
{
    int cnt=0;
    bcm_port_config_t port_config;
    bcm_pbmp_t port_bitmap;
    bcm_port_t port;

    bcm_port_config_t_init(&port_config);

    bcm_port_config_get(unit, &port_config);
    printf("\n %s \n", str);
    printf("\n ====================");

    BCM_PBMP_CLEAR(port_bitmap);
    BCM_PBMP_ASSIGN(port_bitmap, port_config.all);
    BCM_PBMP_ITER(port_bitmap, port) {
//        printf("\n port %d", port);
        cnt++;
    }
    printf("\n count = %d ", cnt);
}
int trunk_create(int unit, bcm_trunk_t tid, int in_local_port, int out_local_port_num,
                 int *out_local_ports, int is_hash_mode)
{
    bcm_gport_t sysport=0,modport=0, modport_1=0, modport_2=0;
    int                 rv= BCM_E_NONE;
    int                 i;
    bcm_trunk_member_t  member_array[20];
    bcm_trunk_info_t    trunk_info;
    bcm_gport_t tgport, gport, tmp_gport;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    sal_memset(member_array, 0, sizeof(member_array));
    sal_memset(&trunk_info, 0, sizeof(trunk_info));

    /* On JER2, need to set trunk id before creating */
    if (is_device_or_above(unit, JERICHO2)){
        BCM_TRUNK_ID_SET(tid, 0, tid);
    }
    printf("Trunk ID set #%d create with %d ports\n",
           tid, out_local_port_num);
    print_port_config(unit, "before trunk create");

    /* Creates #tid trunk (no memory allocation and no members inside) */
    rv =  bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);
    printf("AFter trunk_create ID set #%d create with %d ports\n",
           tid, out_local_port_num);
    print_port_config(unit, "after trunk create");
    if(rv != BCM_E_NONE)
    {
        printf("Error: bcm_trunk_create(), rv=%d \n", rv);
        return rv;
    }
    printf("Before Trunk gport get =%d\n", tgport);
    BCM_GPORT_TRUNK_SET(tgport, tid);
    printf("After Trunk gport get =%d\n", tgport);
    BCM_GPORT_MODPORT_SET(modport_1, 0, out_local_ports[0]);
    BCM_GPORT_MODPORT_SET(modport_2, 0, out_local_ports[1]);
    BCM_GPORT_MODPORT_SET(modport, 0, tid);
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport, 0x1234);
    int my_modid;
    bcm_stk_my_modid_get(unit, &my_modid);
    printf("modid is %d\n", my_modid);
//    BCM_GPORT_MODPORT_SET(modport_gport, my_modid, )

    tmp_gport = 0;
    bcm_stk_sysport_gport_set(unit, sysport, tgport);
    bcm_stk_sysport_gport_get(unit, sysport, &tmp_gport);
    printf("sysport %d gport %d\n", sysport, tmp_gport);


    if (is_device_or_above(unit, JERICHO2)){
        key.index=0;
        key.type=bcmSwitchPortHeaderType;
        value.value=BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        rv = bcm_switch_control_indexed_port_set(0, tgport, key, value);
        if(rv != BCM_E_NONE)
        {
             printf("Error: bcm_switch_control_indexed_port_set() %d, tgport\n");
        }

        for (i=0; i < out_local_port_num; i++) {
            rv = bcm_switch_control_indexed_port_set(0, out_local_ports[i], key, value);
            if(rv != BCM_E_NONE)
            {
                printf("Error: bcm_switch_control_indexed_port_set() %d, out_local_ports[i]\n");
            }
        }
    }

    /* Prepare local ports to gports */
    for (i=0; i<out_local_port_num; i++)
    {
       BCM_GPORT_SYSTEM_PORT_ID_SET(member_array[i].gport, out_local_ports[i]);
    }

    /* two options are valid BCM_TRUNK_PSC_PORTFLOW or BCM_TRUNK_PSC_ROUND_ROBIN */
    if(is_hash_mode)
    {
        trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW;

        /* Set numbers of headers to take under consideration when creating the LB key for hashing */
        /* bcmSwitchTrunkHashPktHeaderCount isn't supported on JER2 yet */
        if (!is_device_or_above(unit, JERICHO2)){
            rv = bcm_switch_control_port_set(unit, in_local_port, bcmSwitchTrunkHashPktHeaderCount, 0x2);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_control_port_set(), rv=%d.\n", rv);
                return rv;
            }
        }
    }
    else
    {
        if (!is_device_or_above(unit, JERICHO2)){
            trunk_info.psc= BCM_TRUNK_PSC_ROUND_ROBIN;
        } else {
            trunk_info.psc= BCM_TRUNK_PSC_SMOOTH_DIVISION;
        }
    }


    /* Adds members (in member_array) to trunk and activates trunk mechanism */
    rv = bcm_trunk_set(unit, tid, &trunk_info, out_local_port_num, member_array);
    if(rv != BCM_E_NONE)
    {
        printf("Error: bcm_trunk_set(), rv=%d\n", rv);
        return rv;
    }
    printf("trunk set\n");
    print_port_config(unit, "after trunk set");
    return rv;

}

int lag_hashing_routing_main( int unit,  int port_0, int port_1, int in_local_port, bcm_trunk_t tid)
{
  int rv = BCM_E_NONE;
  int out_local_ports[20];
  int out_local_port_num = 2;

  out_local_ports[0] = port_0;
  out_local_ports[1] = port_1;

  printf("Trunk #%d create with %d ports\n", tid, out_local_port_num);
  rv = trunk_create(unit, tid, in_local_port, out_local_port_num, out_local_ports, 1);
  if (rv != BCM_E_NONE) {
    printf("Error, trunk_create(), rv=%d.\n", rv);
    return rv;
  }

  if (is_device_or_above(unit, JERICHO2)){
    BCM_TRUNK_ID_SET(tid, 0, tid);
  }
  int trunk_gport;
  int in_vlan = 17;
  int vrf = 5;
  int out_vlan=18;
  bcm_if_t egress_intf;
  int encap_id;
  int rc;
  int ingress_intfs[2];

  bcm_mac_t mac_address  = {0x00, 0x00, 0x22, 0x22, 0x22, 0x22};  /* my-MAC */
  bcm_mac_t next_mac_address  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */
  int host = 0x21212101; /* 33.33.33.1 */
  bcm_l3_host_t l3host;

  /* create in-RIF */
  rc = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
  if (rc != BCM_E_NONE) {
    printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
  }
  printf( " Adding in port to vlan\n " );
  rc = bcm_vlan_gport_add(unit, in_vlan, in_local_port, 0);
  if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
    printf("fail add port(0x%08x) to vlan(%d)\n", in_local_port, in_vlan);
    return rc;
  }
  create_l3_intf_s intf;
  intf.vsi = in_vlan;
  intf.my_global_mac = mac_address;
  intf.my_lsb_mac = mac_address;
  intf.vrf_valid = 1;
  intf.vrf = vrf;

  printf( " creating ingress vlan/ intf \n" );
  rc = l3__intf_rif__create(unit, &intf);
  ingress_intfs[0] = intf.rif;
  if (rc != BCM_E_NONE) {
    printf("Error, l3__intf_rif__create\n");
  }


  // create out-RIF
  rc = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
  if (rc != BCM_E_NONE) {
    printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
  }
  printf( " adding trunk port to vlan \n" );
  BCM_GPORT_TRUNK_SET(trunk_gport, tid); /* create a trunk gport and give this gport */
  rc = bcm_vlan_gport_add(unit, out_vlan, trunk_gport, 0);
  if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
    printf("fail add port(0x%08x) to vlan(%d)\n", trunk_gport, out_vlan);
    return rc;
  }

  intf.vsi = out_vlan;

  rc = l3__intf_rif__create(unit, &intf);
  ingress_intfs[1] = intf.rif;
  if (rc != BCM_E_NONE) {
    printf("Error, l3__intf_rif__create\n");
  }

  /* create FEC and send to LAG (instead of out-port) */
  BCM_GPORT_TRUNK_SET(trunk_gport, tid); /* create a trunk gport and give this gport to create_l3_egress() instead of the dest-port */
  printf( "adding the route\n");
  create_l3_egress_s l3eg2;
  sal_memcpy(l3eg2.next_hop_mac_addr, next_mac_address , 6);
  l3eg2.out_tunnel_or_rif = ingress_intfs[1];
  l3eg2.out_gport = trunk_gport;
  l3eg2.vlan = out_vlan;
  l3eg2.fec_id = egress_intf;
  l3eg2.arp_encap_id = encap_id;
  rc = l3__egress__create(unit,&l3eg2);
  if (rc != BCM_E_NONE) {
    printf("Error, l3__egress__create\n");
    return rc;
  }

  egress_intf = l3eg2.fec_id;
  encap_id = l3eg2.arp_encap_id;

  /* add host entry and point to the FEC pointing at LAG */
  bcm_l3_host_t_init(&l3host);
  l3host.l3a_ip_addr = host;
  l3host.l3a_vrf = vrf;
  l3host.l3a_intf = egress_intf; /* FEC */

  rc = bcm_l3_host_add(unit, &l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_add failed: %x \n", rc);
    return rc;
  }
    print_port_config(unit, "End of hash");
  return 0;
}
