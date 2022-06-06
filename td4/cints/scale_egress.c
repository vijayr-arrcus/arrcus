cint_reset();
int unit = 0;
bcm_l3_egress_t egr_obj;
bcm_if_t egr_obj_if;
int i =0;
for (i = 0; i < 65536;  i++)  {
  int rv = BCM_E_NONE;
  bcm_l3_egress_t_init(&egr_obj);
  egr_obj.intf = -1;
  egr_obj.port = 0;
  egr_obj.flags = BCM_L3_L2TOCPU;
  egr_obj.flags2 = BCM_L3_FLAGS2_UNDERLAY;
  rv = bcm_l3_egress_create(unit, 0, &egr_obj, &egr_obj_if);
  if (rv != 0) {
    printf("Scale broke at %d \n", rv);
    break;
  }
}
print i;

cint_reset();
int unit = 0;
bcm_l3_egress_t egr_obj;
bcm_if_t egr_obj_if;
int i =0;
for (i = 0; i < 65536;  i++)  {
  int rv = BCM_E_NONE;
  bcm_l3_egress_t_init(&egr_obj);
  egr_obj.intf = -1;
  egr_obj.port = 0;
  egr_obj.flags = BCM_L3_L2TOCPU;
  rv = bcm_l3_egress_create(unit, 0, &egr_obj, &egr_obj_if);
  if (rv != 0) {
    printf("Scale broke at %d \n", rv);
    break;
  }
}
print i;
