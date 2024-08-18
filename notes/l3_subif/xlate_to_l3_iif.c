cint_reset();

bcm_gport_t  gport1;
bcm_gport_t  gport2;
bcm_vlan_t   vlan = 100;
bcm_pbmp_t   pbmp; 
bcm_mac_t    mac_address1 = {0x02, 0x0, 0x00, 0x00, 0x00, 0x01}; 
bcm_mac_t    mac_address2 = {0x02, 0x0, 0x00, 0x00, 0x00, 0x02}; 
int          unit = 0;
int          cpu = 0;
int          vrf1 = 2;
int          vrf2 = 3;
int          port1 = 1; /* xe0 */
int          port2 = 2; /* xe1 */

BCM_GPORT_MODPORT_SET(gport1, 0, port1);
BCM_GPORT_MODPORT_SET(gport2, 0, port2);

print bcm_switch_control_set(unit, bcmSwitchL3IngressMode, 1);
print bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);
print bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, 1);

/* Remove all filters for CPU0 */
print bcm_port_vlan_member_set(unit, cpu, 0);

/* VLAN create */
/*
print bcm_vlan_create(unit, vlan);
BCM_PBMP_CLEAR(pbmp);
BCM_PBMP_PORT_ADD(pbmp, port1);
BCM_PBMP_PORT_ADD(pbmp, port2);
print bcm_vlan_port_add(unit, vlan, pbmp, pbmp);
*/

/* ****** L3 ****** */
bcm_l3_ingress_t l3_in_if1, l3_in_if2;
bcm_l3_intf_t l3_iface1, l3_iface2;
bcm_l3_egress_t l3_egress1, l3_egress2; 
bcm_if_t l3_egress_id1, l3_egress_id2;
bcm_l3_host_t l3host1, l3host2;

/* L3 interface 1 / L3_IIF 1 */
bcm_l3_intf_t_init(&l3_iface1);
l3_iface1.l3a_flags = BCM_L3_ADD_TO_ARL;
l3_iface1.l3a_vid = vlan;
l3_iface1.l3a_vrf = vrf1;
l3_iface1.l3a_mac_addr = mac_address1;
print bcm_l3_intf_create(unit, &l3_iface1);
/* Use the same ID to allocate the ingress interface (L3_IIF) */
bcm_l3_ingress_t_init(&l3_in_if1);
l3_in_if1.vrf = vrf1;
l3_in_if1.flags = BCM_L3_INGRESS_REPLACE;
print bcm_l3_ingress_create(unit, &l3_in_if1, &l3_iface1.l3a_intf_id);

/* L3 interface 2 / L3_IIF 2 */
bcm_l3_intf_t_init(&l3_iface2);
l3_iface2.l3a_flags = BCM_L3_ADD_TO_ARL;
l3_iface2.l3a_vid = vlan;
l3_iface2.l3a_vrf = vrf2;
l3_iface2.l3a_mac_addr = mac_address2;
print bcm_l3_intf_create(unit, &l3_iface2);
/* Use the same ID to allocate the ingress interface (L3_IIF) */
bcm_l3_ingress_t_init(&l3_in_if2);
l3_in_if2.vrf = vrf2;
l3_in_if2.flags = BCM_L3_INGRESS_REPLACE;
print bcm_l3_ingress_create(unit, &l3_in_if2, &l3_iface2.l3a_intf_id);

/* Egress side next hop object */

/* L3 egress to CPU [L3_IIF 1] */
bcm_l3_egress_t_init(&l3_egress1); 
l3_egress1.flags = BCM_L3_KEEP_SRCMAC | BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN | BCM_L3_KEEP_TTL; 
l3_egress1.flags |= BCM_L3_EGRESS_ONLY | BCM_L3_L2TOCPU; 
l3_egress1.port = cpu; 
l3_egress1.intf = l3_iface1.l3a_intf_id; 
print bcm_l3_egress_create(unit, 0, &l3_egress1, &l3_egress_id1);

/* L3 egress to CPU [L3_IIF 2] */
bcm_l3_egress_t_init(&l3_egress2); 
l3_egress2.flags = BCM_L3_KEEP_SRCMAC | BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN | BCM_L3_KEEP_TTL; 
l3_egress.flags |= BCM_L3_EGRESS_ONLY | BCM_L3_L2TOCPU; 
l3_egress.port = cpu; 
l3_egress.intf = l3_iface2.l3a_intf_id; 
print bcm_l3_egress_create(unit, 0, &l3_egress, &l3_egress_id);

/* Create Host entry 1 */
bcm_l3_host_t_init(&l3host1);
l3host1.l3a_ip_addr = 0x0A000001;  /* 10.0.0.1 */
l3host1.l3a_vrf = vrf1; 
l3host1.l3a_intf = l3_egress_id1; 
print bcm_l3_host_add(unit, &l3host1); 

/* Create Host entry 2 */
bcm_l3_host_t_init(&l3host2);
l3host2.l3a_ip_addr = 0x0B000001; /* 11.0.0.1 */
l3host2.l3a_vrf = vrf2; 
l3host2.l3a_intf = l3_egress_id2;
print bcm_l3_host_add(unit, &l3host2);

/* ****** VLAN_XLATE ****** */

print bcm_vlan_control_port_set(unit, port1, bcmVlanTranslateIngressEnable, 1);
print bcm_vlan_control_port_set(unit, port1, bcmVlanPortTranslateKeyFirst, bcmVlanTranslateKeyPortOuter);

print bcm_vlan_control_port_set(unit, port2, bcmVlanTranslateIngressEnable, 1);
print bcm_vlan_control_port_set(unit, port2, bcmVlanPortTranslateKeyFirst, bcmVlanTranslateKeyPortOuter);

bcm_vlan_action_set_t action;
bcm_vlan_action_set_t_init(&action);
action.ingress_if = l3_iface1.l3a_intf_id;
print bcm_vlan_translate_action_add(unit, gport1, bcmVlanTranslateKeyPortOuter, vlan, 0, &action);

bcm_vlan_action_set_t_init(&action);
action.ingress_if = l3_iface2.l3a_intf_id;
print bcm_vlan_translate_action_add(unit, gport2, bcmVlanTranslateKeyPortOuter, vlan, 0, &action);