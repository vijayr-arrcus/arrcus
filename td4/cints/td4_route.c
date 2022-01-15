/*
 * =====================================================================================
 *
 *       Filename:  td4_route.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  01/12/2022 05:43:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
cint_reset();
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
route_t.l3a_subnet = 0x0A010100;
route_t.l3a_ip_mask = 0xffffff00;
route_t.l3a_intf = egr_obj_if;
route_t.l3a_vrf = 0;
route_t.l3a_opaque_ctrl_id = 6;
rv = bcm_l3_route_add(unit, &route_t);
if (BCM_FAILURE(rv)) {
    printf(“Error in bcm_l3_route_add: %s\n”, bcm_errmsg(rv));
}
bcm_l3_host_t host_t;
bcm_l3_host_t_init(&host_t);
host_t.l3a_subnet = 0x0A010101;
host_t.l3a_ip_mask = 0xffffffff;
host_t.l3a_intf = egr_obj_if;
host_t.l3a_vrf = 0;
host_t.l3a_opaque_ctrl_id = 6;
rv = bcm_l3_host_add(unit, &host_t);
if (BCM_FAILURE(rv)) {
    printf(“Error in bcm_l3_route_add: %s\n”, bcm_errmsg(rv));
}








