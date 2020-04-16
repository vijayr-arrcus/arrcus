
int mpls_vrf = 1;
int create_l3if(int unit, bcm_port_t port, bcm_vlan_t vlan, bcm_mac_t mac)
{
  int rv;
  bcm_pbmp_t u, t;
  bcm_l3_intf_t l3if;
  bcm_l3_ingress_t l3ing;

  bcm_vlan_create(unit, vlan);
  if (!BCM_TRUNK_ID_IS_VALID(port)) {
	  BCM_PBMP_CLEAR(u);
	  BCM_PBMP_CLEAR(t);
	  BCM_PBMP_PORT_SET(t, port);
	  bcm_vlan_port_add(unit, vlan, t, u);
	  bcm_port_untagged_vlan_set(unit, port, vlan);
  } else {
	  bcm_gport_t gport;
      int tagged = 1;
	  BCM_GPORT_TRUNK_SET(gport, port);

	  rv = bcm_vlan_gport_add(unit, vlan, gport,
			  tagged ? 0 : BCM_VLAN_GPORT_ADD_UNTAGGED);

	  rv = bcm_port_untagged_vlan_set(unit, gport, vlan);
	  // set the untagged property for trunk
	  if (!tagged) {
		  //Note: ingress VLAN miss is drop by default (PG 21.5.2)
		  // This sets untagged packets default vlan for the incoming port
		  rv = bcm_port_untagged_vlan_set(unit, gport, vlan);
	  }
      bcm_vlan_control_port_set(unit, gport, bcmVlanTranslateIngressMissDrop, 0);
  }

  bcm_l3_intf_t_init(&l3if);
  l3if.l3a_vid = vlan;
  l3if.l3a_mtu = 8974;
  sal_memcpy(&l3if.l3a_mac_addr, mac, 6);
  bcm_l3_intf_create(unit, &l3if);
  printf("Created l3intf id %d\n", l3if.l3a_intf_id);

  bcm_l3_ingress_t_init(&l3ing);
  l3ing.flags = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_GLOBAL_ROUTE;
  l3ing.urpf_mode = bcmL3IngressUrpfDisable;;
  l3ing.vrf = mpls_vrf; 
   bcm_l3_ingress_create(unit, &l3ing, &l3if.l3a_intf_id);
  return l3if.l3a_intf_id;
}

int create_egr_nh(int unit, bcm_if_t tnl_if, bcm_if_t vsi, bcm_mac_t mac, bcm_if_t *encap_id)
{
  bcm_if_t dummy;
  bcm_l3_egress_t l3eg;

  bcm_l3_egress_t_init(&l3eg);
  sal_memcpy(&l3eg.mac_addr, mac, 6);
  //Linking tunnel and arp encap
  l3eg.intf = tnl_if;
  //For MPLS in J2, arp points to vsi (rif)
  l3eg.vlan = vsi;

   bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &dummy);
  *encap_id = l3eg.encap_id;
  printf("Created l3 egr nh %d\n", l3eg.encap_id);
  return 0;
}

int create_ing_nh(int unit, bcm_if_t tnl_if, bcm_gport_t port, bcm_if_t *id)
{
  bcm_if_t fec_id;
  bcm_l3_egress_t l3eg;

  bcm_l3_egress_t_init(&l3eg);
  l3eg.intf = tnl_if;
  l3eg.port = port;

   bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec_id);
  *id = fec_id;
  printf("Created ing nh %d\n", fec_id);
  return 0;
}

int create_v4route(int unit, uint32 pfx, uint32 mask, bcm_if_t fec)
{
  bcm_l3_route_t l3rt;

  bcm_l3_route_t_init(&l3rt);
  l3rt.l3a_subnet = pfx;
  l3rt.l3a_ip_mask = mask;
  l3rt.l3a_intf = fec;
  l3rt.l3a_vrf = mpls_vrf;
   bcm_l3_route_add(unit, &l3rt);
  return 0;
}

int create_mpls_tnl_init(int unit, bcm_if_t oif, bcm_mpls_label_t lbl, bcm_if_t *tnl_if)
{
  bcm_mpls_egress_label_t l;

  bcm_mpls_egress_label_t_init(&l);
  l.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
  l.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
  l.ttl = 255;
  l.label = lbl;
  l.l3_intf_id = oif;

   bcm_mpls_tunnel_initiator_create(unit, 0, 1/* num lbls */, &l);
  printf("Create tunnel init %d\n", l.tunnel_id);
  *tnl_if = l.tunnel_id;
  return 0;
}

int create_mpls_tnl_swap(int unit, bcm_if_t oif, bcm_mpls_label_t lbl, bcm_if_t *tnl_if)
{
  bcm_mpls_egress_label_t l;

  bcm_mpls_egress_label_t_init(&l);
  l.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
  l.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
  l.label = lbl;
  l.l3_intf_id = oif;
  bcm_mpls_tunnel_initiator_create(unit, 0, 1/* num lbls */, &l);
  print("Create tunnel init %d", l.tunnel_id);
  *tnl_if = l.tunnel_id;
  return 0;
}

int create_ilm_swap(int unit, bcm_mpls_label_t in_lbl, bcm_if_t fec)
{
  bcm_mpls_tunnel_switch_t ent;

  bcm_mpls_tunnel_switch_t_init(&ent);
  ent.action = BCM_MPLS_SWITCH_ACTION_NOP;
  ent.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
  ent.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
  ent.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
  ent.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
  ent.label = in_lbl;
  ent.egress_if = fec;
  print  bcm_mpls_tunnel_switch_create(unit, &ent);
  printf("Created swap entry:%d\n",ent.tunnel_id);
  return 0;
}

int create_ilm_pop(int unit, bcm_mpls_label_t lbl)
{
  bcm_mpls_tunnel_switch_t ent;

  bcm_mpls_tunnel_switch_t_init(&ent);
  ent.action = BCM_MPLS_SWITCH_ACTION_POP;
  ent.label = lbl;
  ent.vpn = mpls_vrf;
  //ent.flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
  bcm_mpls_tunnel_switch_create(unit, &ent);
  printf("Created pop entry:%d\n",ent.tunnel_id);

  return 0;
}


/* Test cases
 * tx 1 psrc=1 data=00010203040500000000000181000bb98847001011644500002c00010000400050bb0101010114141401417272637573204e6574776f726b20446966666572656e74
 * expected data: a82bb513131300010203041381000bc58847013131634500002c00010000400050bb0101010114141401417272637573204e6574776f726b20446966666572656e74
 */
/* Assumes packet ingr with label 257(0x0101) swapped to 4883(0x1313) */
int mpls_lsr(bcm_port_t port, bcm_vlan_t vlan)
{
  int unit = 0, mod;
  bcm_gport_t gp;
  bcm_if_t tnl_if, encap, fec;
  bcm_mac_t sysmac = {0x0, 0x1, 0x2, 0x3, 0x4, 0x13};
  bcm_mac_t m5712 = {0xa8, 0x2b, 0xb5, 0x13, 0x13, 0x13};
  int l3if;

  bcm_stk_modid_get(unit, &mod);

  /* Egress l3 intf */
  l3if = create_l3if(unit, port, vlan, sysmac);
  /* Create mpls encap */
  create_mpls_tnl_swap(unit, 0/*RIF will be programmed in arp - egress_only*/, 4883/* lbl */, &tnl_if);
  /* Create egr nh */
  create_egr_nh(unit, tnl_if, l3if, m5712, &encap);
  /* Create ing nh */
  //BCM_GPORT_MODPORT_SET(gp, mod, 14);
  if (!BCM_TRUNK_ID_IS_VALID(port)) {
      BCM_GPORT_LOCAL_SET(gp, port);
  } else {
	  BCM_GPORT_TRUNK_SET(gp, port);
  }
  create_ing_nh(unit, tnl_if, gp, &fec);

  /* Add route entry->mpls tnl */
  create_ilm_swap(unit, 257, fec);
  printf("LSR fec:%d\n", fec);

  //ToDo: Do we need this??
  //create_v4route(unit, 0x13131300, 0xffffff00, fec);
  return 0;
}

/* Test cases
 * tx 1 psrc=1 data=00010203040500000000000181000bb98847001111644500002c00010000400050bb0101010114141401417272637573204e6574776f726b20446966666572656e74
 * expected data: a82bb514141400010203041481000bc608004500002c000100003f0051bb0101010114141401417272637573204e6574776f726b20446966666572656e74
 */
int mpls_term(bcm_port_t port, bcm_vlan_t vlan)
{
  int unit = 0, mod;
  bcm_gport_t gp;
  bcm_if_t tnl_if, encap, fec;
  bcm_mac_t sysmac = {0x0, 0x1, 0x2, 0x3, 0x4, 0x14};
  bcm_mac_t m5712 = {0xa8, 0x2b, 0xb5, 0x14, 0x14, 0x14};
  int l3if;

  bcm_stk_modid_get(unit, &mod);

  /* Egress l3 intf */
  l3if = create_l3if(unit, port, vlan, sysmac);
  /* Create egr nh */
  create_egr_nh(unit, 0/*No tunnel encap so linking not required*/, l3if, m5712, &encap);

  /* Create ing nh */
  //BCM_GPORT_MODPORT_SET(gp, mod, port);
  if (!BCM_TRUNK_ID_IS_VALID(port))
      BCM_GPORT_LOCAL_SET(gp, port);
  else 
      BCM_GPORT_TRUNK_SET(gp, port);
  create_ing_nh(unit, encap, gp, &fec);

  /* Add tnl term entry */
  create_ilm_pop(unit, 273);

  /* Create routing table entry */
  create_v4route(unit, 0x14141400, 0xffffff00, fec);
  return 0;
}

int mpls_push_tnl(bcm_port_t port, bcm_vlan_t vlan, int lbl)
{
  int unit = 0, mod;
  bcm_gport_t gp;
  bcm_if_t tnl_if, encap, fec;
  bcm_mac_t sysmac = {0x0, 0x1, 0x2, 0x3, 0x4, 0x15};
  bcm_mac_t m5712 = {0xa8, 0x2b, 0xb5, 0x15, 0x15, 0x15};
  int l3if;

  bcm_stk_modid_get(unit, &mod);

  /* Egress l3 intf */
  l3if = create_l3if(unit, port, vlan, sysmac);

  /* Create mpls encap */
  create_mpls_tnl_init(unit, 0, lbl, &tnl_if);

  /* Create egr nh */
  create_egr_nh(unit, tnl_if, l3if, m5712, &encap);

  /* Create ing nh */
  //BCM_GPORT_MODPORT_SET(gp, mod, port);
  BCM_GPORT_LOCAL_SET(gp, port);
  create_ing_nh(unit, tnl_if, gp, &fec);

  return fec;
}

int mpls_push_chain_tnl(bcm_port_t port, bcm_vlan_t vlan, int vpn_lbl, int igp_lbl)
{
  int unit = 0, mod;
  bcm_gport_t gp;
  bcm_if_t tnl_if1, tnl_if2, encap, fec;
  bcm_mac_t sysmac = {0x0, 0x1, 0x2, 0x3, 0x4, 0x16};
  bcm_mac_t m5712 = {0xa8, 0x2b, 0xb5, 0x16, 0x16, 0x16};
  int l3if;

  //bcm_stk_modid_get(unit, &mod);
  /* Egress l3 intf */
  l3if = create_l3if(unit, port, vlan, sysmac);

  //ToDo: check the linking part
  /* Create IGP mpls encap */
  create_mpls_tnl_init(unit, 0/*Rif will be programmed as part of egress_only*/,
                       igp_lbl, &tnl_if1/*igp encap*/);
  /* Create BGP mpls encap and link to igp encap*/
  create_mpls_tnl_init(unit, tnl_if1/*igp encap to be linked to vpn encap*/,
                       vpn_lbl, &tnl_if2/*vpn encap*/);
  /* Create egr arp encap and link it to igp encap */
  create_egr_nh(unit, tnl_if1/*igp encap*/ , l3if, m5712, &encap/*arp encap*/);

  printf("Debug::::\n");
  print tnl_if1;
  print tnl_if2;
  print encap;

  /* Create ing nh */
  //BCM_GPORT_MODPORT_SET(gp, mod, port);
  BCM_GPORT_LOCAL_SET(gp, port);
  create_ing_nh(unit, tnl_if2/*vpn encap will be programmed in ingress*/, gp, &fec);
  printf("Created fec:%d\n", fec);

  return fec;
}

/* Test cases
 * For pushing single label:
 * tx  1 psrc=1 data=00010203040500000000000181000bb908004500002c000100004000f35c5756060915151501417272637573204e6574776f726b20446966666572656e74
 * expected data: a82bb515151500010203041581000bc78847015151ff4500002c000100003f00f45c5756060915151501417272637573204e6574776f726b20446966666572656e74
 *
 */
int mpls_push_single_label(bcm_port_t port, bcm_vlan_t vlan)
{
  int unit = 0;
  bcm_if_t fec;

  fec = mpls_push_tnl(port, vlan, 5397);
  /* Add route entry->mpls tnl */
  create_v4route(unit, 0x15151500, 0xffffff00, fec);
}

int mpls_push_double_label(bcm_port_t port, bcm_vlan_t vlan)
{
  int unit = 0;
  bcm_if_t fec;

  fec = mpls_push_chain_tnl(port, vlan, 22, 5654);
  /* Add route entry->mpls tnl */
  create_v4route(unit, 0x16161600, 0xffffff00, fec);
}

int create_cscd_ing_nh(int unit, bcm_if_t l3if, bcm_gport_t port, bcm_if_t *id)
{
  bcm_if_t fec_id;
  bcm_l3_egress_t l3eg;

  bcm_l3_egress_t_init(&l3eg);
  l3eg.intf = l3if;
  //l3eg.vlan = l3if;
  l3eg.port = port;
  l3eg.flags = BCM_L3_CASCADED;

   bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec_id);
  *id = fec_id;
  printf("Created ing nh %d\n", fec_id);
  return 0;
}

int mpls_cascaded(bcm_port_t port, bcm_vlan_t vlan)
{
  int unit = 0, mod, gp1;
  bcm_gport_t gp;
  bcm_if_t tnl_if, tnl_if1, encap, fec, fec1;
  bcm_mac_t sysmac = {0x0, 0x1, 0x2, 0x3, 0x4, 0x17};
  bcm_mac_t m5712 = {0xa8, 0x2b, 0xb5, 0x17, 0x17, 0x17};

  bcm_stk_modid_get(unit, &mod);

  /* Egress l3 intf */
  create_l3if(unit, port, vlan, sysmac);

  /* Create mpls encap */
  create_mpls_tnl_init(unit, 0, 5911/* igb lbl */, &tnl_if);
  /* Create egr nh */
  create_egr_nh(unit, tnl_if, vlan, m5712, &encap);
  /* Create ing nh */
  //BCM_GPORT_MODPORT_SET(gp, mod, port);
  BCM_GPORT_LOCAL_SET(gp, port);
  create_cscd_ing_nh(unit, tnl_if, gp, &fec);

  //BCM_L3_ITF_SET(tnl_if, BCM_L3_ITF_TYPE_LIF, 0);
  create_mpls_tnl_init(unit, 0, 23/* vpn lbl */, &tnl_if1);

  BCM_GPORT_FORWARD_PORT_SET(gp1, fec);
  create_ing_nh(unit, tnl_if1, gp1, &fec1);
  create_v4route(unit, 0x17171700, 0xffffff00, fec1);
}

int mpls(int src_port, int swap_port, int term_port, int single_label_port, int double_label_port, int cascaded_port)
{
  int unit = 0;
  bcm_vlan_t vlan = 3001;
  bcm_mac_t sysmac = {0x0, 0x1, 0x2, 0x3, 0x4, 0x05};

  /* Ingress l3 intf */
  create_l3if(unit, src_port, vlan, sysmac);

  if (swap_port) {
    mpls_lsr(swap_port, 3013);
  }
  if (term_port) {
    mpls_term(term_port, 3014);
  }
  if (single_label_port) {
    mpls_push_single_label(single_label_port, 3015);
  }
  if (double_label_port) {
    mpls_push_double_label(double_label_port, 3016);
  }
  if (cascaded_port) {
    mpls_cascaded(cascaded_port, 3017);
  }
}


/*** LSR Tests ***/
//mpls(1, 13, 14, 15, 16, 17);
// in physical port, out lag port
int mpls_lag_1 (int trunk_id) {
    int tid = trunk_id;
    bcm_gport_t tgport;
    int vlan_domain = 100;
    main_trunk(13, 14, tid,vlan_domain);
    BCM_TRUNK_ID_SET(tid, 0, tid);
    BCM_GPORT_TRUNK_SET(tgport, tid);
    set_tpid(tgport);
    mpls(1, tid, 0, 0,0,0);
}

// in lag port, out physical
int mpls_lag_2 (int trunk_id) {
    int tid = trunk_id;
    int tgport;
    int vlan_domain = 1;
    main_trunk(1, 13, tid,vlan_domain);
    BCM_TRUNK_ID_SET(tid, 0, tid);
    BCM_GPORT_TRUNK_SET(tgport, tid);
    set_tpid(tgport);
    mpls(tid, 14, 0, 0,0,0);
}

// in and out both lag.
int mpls_lag_3 (int trunk_id_1, int trunk_id_2) {
    int tid_1 = trunk_id_1;
    int tid_2 = trunk_id_2;
    int tgport;
    int vlan_domain = 1;

    main_trunk(1, 13, tid_1,vlan_domain);
    BCM_TRUNK_ID_SET(tid_1, 0, tid_1);
    BCM_GPORT_TRUNK_SET(tgport, tid_1);
    set_tpid(tgport);

    main_trunk(14, 15, tid_2,vlan_domain);
    BCM_TRUNK_ID_SET(tid_2, 0, tid_2);
    BCM_GPORT_TRUNK_SET(tgport, tid_2);
    set_tpid(tgport);

    mpls(tid_1, tid_2, 0, 0,0,0);
}

/** LER tests **/
// in physical out lag port
int mpls_lag_4 (int trunk_id) {
    int tid = trunk_id;
    bcm_gport_t tgport;
    int vlan_domain = 100;
    main_trunk(13, 14, tid,vlan_domain);
    BCM_TRUNK_ID_SET(tid, 0, tid);
    BCM_GPORT_TRUNK_SET(tgport, tid);
    set_tpid(tgport);
    mpls(1, 0, tid, 0,0,0);
}

// in lag out physical
int mpls_lag_5 (int trunk_id) {
    int tid = trunk_id;
    bcm_gport_t tgport;
    int vlan_domain = 100;
    main_trunk(1, 13, tid,vlan_domain);
    BCM_TRUNK_ID_SET(tid, 0, tid);
    BCM_GPORT_TRUNK_SET(tgport, tid);
    set_tpid(tgport);
    mpls(tid, 0, 14, 0,0,0);
}
