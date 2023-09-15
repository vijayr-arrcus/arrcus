cint_reset();

bcm_field_group_t groupid=1;
int unit =0;
bcm_field_entry_t fp_entry_id;
bcm_ip_t type = 0x0806;
//uint32_t flexctr_action_id = 0x10000007;
//uint32_t flexctr_action_id = 268435463;
uint32_t flexctr_action_id = 268435465;
uint32_t hw_stats_offset = 100;
bcm_field_flexctr_config_t flexctr_cfg;


printf("\n Create fp entry");
print bcm_field_entry_create(unit, groupid, &fp_entry_id);
printf("\n fp entry id  %d ", fp_entry_id);
print bcm_field_qualify_EtherType(unit, fp_entry_id, type, 0xFFFF);
printf("\n fp ethernet type quall added");
printf("\n fp priority set ");
print bcm_field_action_add(unit, fp_entry_id, bcmFieldActionCopyToCpu, 0,0);

flexctr_cfg.flexctr_action_id = flexctr_action_id;
flexctr_cfg.counter_index = hw_stats_offset;
flexctr_cfg.color = 0;

print bcm_field_entry_flexctr_attach(unit, fp_entry_id, &flexctr_cfg);
printf("\n fp counter attached");

print bcm_field_entry_install(unit, fp_entry_id);
printf("\n fp entry installed");


bcm_field_entry_destroy(unit, fp_entry_id);
