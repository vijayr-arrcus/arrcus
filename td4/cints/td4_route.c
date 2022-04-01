#include <stdlib.h>
cint_reset();
int unit = 0;
bcm_l3_egress_t egr_obj;
bcm_if_t egr_obj_if;
int rv = BCM_E_NONE;
bcm_l3_egress_t_init(&egr_obj);
egr_obj.intf = -1;
egr_obj.port = 0;
egr_obj.flags = BCM_L3_L2TOCPU;
rv = bcm_l3_egress_create(unit, 0, &egr_obj, &egr_obj_if);
bcm_l3_route_t route_t;
bcm_l3_route_t_init(&route_t);
route_t.l3a_subnet = 0x0A010208;
route_t.l3a_ip_mask = 0xffffff00;
route_t.l3a_intf = egr_obj_if;
route_t.l3a_vrf = 0;
route_t.l3a_opaque_ctrl_id = 3;
route_t.l3a_lookup_class = 0;
rv = bcm_l3_route_add(unit, &route_t);
if (BCM_FAILURE(rv)) {
    printf(“Error in bcm_l3_route_add: %s\n”, bcm_errmsg(rv));
}
bcm_l3_host_t host_t;
bcm_l3_host_t_init(&host_t);
host_t.l3a_ip_addr = 0x0A010308;
host_t.l3a_intf = egr_obj_if;
host_t.l3a_vrf = 0;
host_t.l3a_opaque_ctrl_id = 2;
host_t.l3a_lookup_class = 6;
rv = bcm_l3_host_add(unit, &host_t);
if (BCM_FAILURE(rv)) {
    printf(“Error in bcm_l3_route_add: %s\n”, bcm_errmsg(rv));
}





lt L3_IPV4_UC_ROUTE_VRF traverse -l

DATA_TYPE=WIDE_MODE
    MISC_CTRL_1=0
    MISC_CTRL_0=7
    DESTINATION_TYPE=NHOP
    DESTINATION=9
    VRF_ID=0
    IPV4_MASK=0xffffff00(4294967040)
    IPV4=0xa010200(167838208)


lt L3_IPV4_UNICAST_TABLE traverse -l

  MISC_CTRL_1=0
    MISC_CTRL_0=5
    ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX=0
    NHOP_2_OR_ECMP_GROUP_INDEX_1=0
    CLASS_ID=6
    DESTINATION_TYPE=NHOP
    DESTINATION=9
    VRF=0
    IPV4_ADDR=0xa010308(167838472)
