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
bcm_l3_host_t host_t;
int i=0;
for (i = 0; i < 65535; i++) {
  bcm_l3_host_t_init(&host_t);
  int unit=0;
  int rv = 0;
  host_t.l3a_ip_addr = 0xd9010426 + i;
  host_t.l3a_vrf = 0;
  host_t.l3a_intf = 100001;
  rv = bcm_l3_host_add(unit, &host_t);
  if (rv != 0) {
    printf("Failed to create host entry %d \n", rv);
    break;
  }
}
print i;

print sal_config_set("l3_mem_entries", "16384");
char *value = NULL;
value = sal_config_get("l3_mem_entries");
printf("%s \n ", value);


soc_cm_config_str_get(0, "l3_mem_entries");






