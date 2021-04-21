cint_reset();

bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
int core;
bcm_field_group_info_t fg_info;
bcm_field_group_attach_info_t attach_info;
bcm_field_entry_info_t entry_info;

bcm_field_group_t field_grp_handle;
bcm_field_entry_t field_entry_handle;

bcm_field_group_info_t_init(&fg_info);
bcm_field_group_attach_info_t_init(&attach_info);
bcm_field_entry_info_t_init(&entry_info);


fg_info.fg_type = bcmFieldGroupTypeTcam;
fg_info.stage = stage;

sal_strncpy_s(fg_info.name, "ICMP SRV6", sizeof(fg_info.name));
fg_info.nof_quals = 2;
fg_info.nof_actions = 1;

attach_info.key_info.nof_quals = fg_info.nof_quals;
attach_info.payload_info.nof_actions = fg_info.nof_actions;
entry_info.nof_entry_quals = fg_info.nof_quals;
entry_info.nof_entry_actions = fg_info.nof_actions;
entry_info.core = core;

fg_info.qual_types[0] = bcmFieldQualifyIcmpTypeCode;
attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
attach_info.key_info.qual_info[0].input_arg = 0;
attach_info.key_info.qual_info[0].offset = 0;
entry_info.entry_qual[0].type = fg_info.qual_types[0];
entry_info.entry_qual[0].value[0] = 128;
