cint_reset();
struct l3_ingress_intf
{
    int intf_id;
    int vrf;
    uint32 flags;
    int qos_map;
};
int srv6_term_tunnel_id;

int intf_ingress_rif_set(int unit, l3_ingress_intf * ingress_itf)
{
    bcm_l3_ingress_t l3_ing_if;
    int rc;

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = ingress_itf->flags | BCM_L3_INGRESS_WITH_ID;      /* must, as we update exist RIF */
    l3_ing_if.vrf = ingress_itf->vrf;
    l3_ing_if.qos_map_id = ingress_itf->qos_map;

    rc = bcm_l3_ingress_create(unit, l3_ing_if, ingress_itf->intf_id);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rc;
    }

    return rc;
}

void
l3_ingress_intf_init(l3_ingress_intf * ingr_intf)
{
    ingr_intf->vrf = 0;
    ingr_intf->intf_id = 0;
    ingr_intf->flags = 0;
    ingr_intf->qos_map = 0;
}
int create_subinterface (bcm_gport_t gport, int vlan_id,
                         int vsi_id, int lif_id, int my_mac)
{
 uint32_t v4_enable = 0, v6_enable = 0, stnid = 0;
 bcm_l3_intf_t l3_intf;
 bcm_l3_ingress_t l3ing;
 ret_code_e rv;
 int bcm_rv;
 char *if_name;
 ret_code_e rc;
 bcm_if_t l3a_intf_id;
 int i=0;
 uint8_t mac_addr_bytes[IEEE_MACLEN];
 for(  i=0;i<6;i++ ){
   mac_addr_bytes[i]=0;
 }
 mac_addr_bytes[4] = my_mac;  // self mac address used for ip termination

 printf( " Adding gport to subinterface\n" );
 bcm_vswitch_create_with_id(unit, vsi_id);
 // AMIT where do we do this in case of arrcus code ??
 bcm_vlan_create(unit, vlan_id);
 set_tpid( gport);
 add_gport_to_vlan( gport, vlan_id, vsi_id, 1, lif_id );      // 1 says its a tagged port

 BCM_L3_ITF_SET(l3a_intf_id, BCM_L3_ITF_TYPE_RIF,  vsi_id);
 bcm_l3_intf_t_init(&l3_intf);
 l3_intf.l3a_intf_id = l3a_intf_id;
 l3_intf.l3a_vid = l3_intf.l3a_intf_id ;
 l3_intf.l3a_flags = BCM_L3_WITH_ID;
 l3_intf.l3a_mtu = 1524;  // MTU value
 l3_intf.l3a_vrf = vrf;     // Vrf value
 memcpy(l3_intf.l3a_mac_addr,mac_addr_bytes , IEEE_MACLEN); // Add the my_mac address here
 bcm_rv = bcm_l3_intf_create(unit, &l3_intf);

 bcm_l3_ingress_t_init(&l3ing);
 l3ing.vrf = vrf;
 l3ing.flags = BCM_L3_INGRESS_WITH_ID  ;
 l3ing.urpf_mode = bcmL3IngressUrpfDisable;;
 bcm_rv = bcm_l3_ingress_create(unit, &l3ing, &l3_intf.l3a_intf_id);
 printf("created ingress interface = 0x%08x, on vlan = %d in unit %d, vrf = %d\n",vsi_id,vlan_id,unit,vrf);

 return l3_intf.l3a_intf_id;;

}
// returns fec id for the created nh, that is used for adding a route pointing to that fec
int create_subinterface_and_nh (bcm_gport_t gport, int vlan_id,
                                int vsi_id, int lif_id,
                                int my_mac, int nhmac )
{
 // First create an L3 sub interface
 int l3if  = create_subinterface(gport, vlan_id, vsi_id, lif_id, my_mac );

 // Create egress nexhop for packet going out of this subinterface
 bcm_mac_t nh_mac = {0x0, 0x0, 0x0, 0x0, 0x0, nhmac};
 bcm_if_t dummy;
 bcm_l3_egress_t l3eg;
 bcm_if_t encap_id;

 bcm_l3_egress_t_init(&l3eg);
 sal_memcpy(&l3eg.mac_addr, nh_mac, 6);
 print bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &dummy);
 encap_id = l3eg.encap_id;
 printf("Created l3 egr nh %d\n", encap_id);


 bcm_if_t fec_id;
 bcm_l3_egress_t_init(&l3eg);
 BCM_L3_ITF_SET(l3eg.intf, BCM_L3_ITF_TYPE_RIF, l3if);
 l3eg.port = gport;
 l3eg.encap_id = encap_id;
 print bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec_id);
 printf("Created ing nh %d\n", fec_id);

 return fec_id;

}

bcm_ip6_t ip6_mask =         {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};
bcm_ip6_t ip6_dip_64 =       {0x20,0x01,0xab,0xcd,0xca,0xfe,0x20,0x00,0x81,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
bcm_ip6_t ip6_mask_64_loc =  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
bcm_ip6_t ip6_mask_64_func = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};
bcm_tunnel_terminator_t tunnel_term_set;
l3_ingress_intf ingress_rif;
int rv;
bcm_gport_t default_tunnel;
int fec;
int unit=0;
bcm_vswitch_cross_connect_t xc_gports;

fec = 

/*
* 1. Create 64 locator tunnel VRF is 10
*/
bcm_tunnel_terminator_t_init(&tunnel_term_set);
tunnel_term_set.flags = BCM_TUNNEL_TERM_UP_TO_64_LOCATOR_SEGMENT_ID;
tunnel_term_set.type = bcmTunnelTypeSR6;
sal_memcpy(tunnel_term_set.dip6, ip6_dip_64, 16);
sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_64_loc, 16);
tunnel_term_set.vrf = 10; // This is the vrf on which the pkt should be matched for location match
tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
printf("locator tunnel id = %d \n", tunnel_term_set.tunnel_id);

srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
l3_ingress_intf_init(&ingress_rif);
ingress_rif.vrf = 1; // This is the vrf that is yielded once location is a match. This is the vrf where the punt route is installed.
BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
rv = intf_ingress_rif_set(unit, &ingress_rif);
printf("intf_ingress_rif_set. rv = %d tunnel ID %d \n", rv, tunnel_term_set.tunnel_id);
default_tunnel = tunnel_term_set.tunnel_id;


bcm_tunnel_terminator_t_init(&tunnel_term_set);
tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
tunnel_term_set.flags  = BCM_TUNNEL_TERM_CROSS_CONNECT;
tunnel_term_set.default_tunnel_id = default_tunnel;
sal_memcpy(tunnel_term_set.dip6, ip6_dip_64, 16);
sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_64_func, 16);
tunnel_term_set.vrf = 1; // This is the vrf on which the pkt should be matched for function
tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
printf("function tunnel id = %d \n", tunnel_term_set.tunnel_id);


srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
l3_ingress_intf_init(&ingress_rif);
ingress_rif.vrf = 20; // This is the vrf that is yielded once the function is a match. This is the costomers vrf
BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
rv = intf_ingress_rif_set(unit, &ingress_rif);
printf("intf_ingress_rif_set. rv = %d \n", rv);

bcm_vswitch_cross_connect_t gports;
bcm_vswitch_cross_connect_t_init(&gports);
gports.port1 = tunnel_term_set.tunnel_id;
gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
BCM_GPORT_FORWARD_PORT_SET(gports.port2, fec);
rv = bcm_vswitch_cross_connect_add(unit, &gports);
