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
l3_ingress_intf_init(
    l3_ingress_intf * ingr_intf)
{
    ingr_intf->vrf = 0;
    ingr_intf->intf_id = 0;
    ingr_intf->flags = 0;
    ingr_intf->qos_map = 0;
}

bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
bcm_ip6_t ip6_mask_96_loc = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00};
bcm_ip6_t ip6_mask_96_func = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00};
bcm_ip6_t ip6_dip_96 = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
bcm_ip6_t ip6_dip_next_endpoint = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0,0,0xFF,0x13};
bcm_tunnel_terminator_t tunnel_term_set;
l3_ingress_intf ingress_rif;
int rv;
bcm_gport_t default_tunnel;
int fec;
int unit=0;

fec += 0x500;

/*
* 1. Create 96 locator tunnel VRF is 10
*/
bcm_tunnel_terminator_t_init(&tunnel_term_set);
tunnel_term_set.flags = BCM_TUNNEL_TERM_UP_TO_96_LOCATOR_SEGMENT_ID;
tunnel_term_set.type = bcmTunnelTypeSR6;
sal_memcpy(tunnel_term_set.dip6, ip6_dip_96, 16);
sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_96_loc, 16);
tunnel_term_set.vrf = 1;
tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
printf("locator tunnel id = %d \n", tunnel_term_set.tunnel_id);

srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
l3_ingress_intf_init(&ingress_rif);
ingress_rif.vrf = 10;
BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
rv = intf_ingress_rif_set(unit, &ingress_rif);
printf("intf_ingress_rif_set. rv = %d tunnel ID %d \n", rv, tunnel_term_set.tunnel_id);
default_tunnel = tunnel_term_set.tunnel_id;


bcm_tunnel_terminator_t_init(&tunnel_term_set);
tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
tunnel_term_set.default_tunnel_id = default_tunnel;
sal_memcpy(tunnel_term_set.dip6, ip6_dip_96, 16);
sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_96_func, 16);
tunnel_term_set.vrf = 1;
tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
printf("function tunnel id = %d \n", tunnel_term_set.tunnel_id);


srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
l3_ingress_intf_init(&ingress_rif);
ingress_rif.vrf = 20;
BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
rv = intf_ingress_rif_set(unit, &ingress_rif);
printf("intf_ingress_rif_set. rv = %d \n", rv);
