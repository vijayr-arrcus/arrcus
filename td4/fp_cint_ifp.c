cint_reset();

int opt_TestWithPresel = 1;
bcm_field_stage_t opt_TestFpStage = bcmFieldStageIngress;
bcm_field_qualify_t opt_TestFpQualStage = bcmFieldQualifyStageIngress;
int opt_IsVfp = 0;
int opt_IsIfp = 1;
int opt_IsEfp = 0;

bcm_error_t
TestFpPresel()
{
  bcm_field_entry_t entry;
  bcm_field_group_config_t group_config;
  int entry1_prio = 100;
  bcm_field_presel_t presel_id1 = 1;
  bcm_field_presel_group_config_t gcfg_pre;

  int fp_id_base;

  fp_id_base = BCM_FIELD_IFP_ID_BASE;

  bcm_field_presel_config_t presel_config;

  bcm_field_presel_group_config_t_init(&gcfg_pre);

  BCM_FIELD_QSET_INIT(gcfg_pre.qset);

  BCM_FIELD_QSET_ADD(gcfg_pre.qset, bcmFieldQualifyL2MacInfo);
  BCM_FIELD_QSET_ADD(gcfg_pre.qset, bcmFieldQualifyStageIngress);

  printf("Doing presel group config create...\n");
  BCM_IF_ERROR_RETURN(bcm_field_presel_group_config_create(unit, &gcfg_pre));

  bcm_field_presel_config_t_init(&presel_config);
  presel_config.stage = opt_TestFpStage;
  presel_config.flags = BCM_FIELD_PRESEL_CREATE_WITH_ID;
  presel_config.presel_id = presel_id1 | fp_id_base;

  printf("Doing presel config create...\n");
  BCM_IF_ERROR_RETURN(bcm_field_presel_config_create(unit, &presel_config));

  presel_id1 = presel_config.presel_id;
  printf("Doing presel Qualify1. preselid=0x%08X...\n", presel_id1);
  BCM_IF_ERROR_RETURN bcm_field_qualify_L2MacInfo(unit,
                        BCM_FIELD_QUALIFY_PRESEL | presel_id1, BCM_FIELD_L2MAC_SMAC_LT_DMAC,
                        BCM_FIELD_L2MAC_SMAC_LT_DMAC));
  presel_id1 = presel_id1 ^ fp_id_base;

  /* IFP Group Configuration and Creation */
  bcm_field_group_config_t_init(&group_config);
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESEL_GROUP;
  BCM_FIELD_PRESEL_ADD(group_config.preselset, presel_id1);
  group_config.presel_group= gcfg_pre.presel_group;
  group_config.priority = 64;
  group_config.mode = bcmFieldGroupModeAuto;
  BCM_FIELD_QSET_INIT(group_config.qset);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcPort);
  BCM_FIELD_QSET_ADD(group_config.qset, opt_TestFpQualStage);

  BCM_FIELD_ASET_INIT(group_config.aset);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
  if (opt_IsVfp || opt_IsIfp) {
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
  }
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);


  printf("Doing group config create...\n");
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

  printf("Doing entry create to group 0x%08X...\n", group_config.group);
  BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group_config.group, &entry) );

  printf("Doing SrcPort qualify...\n");
  BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcPort(unit, entry, module_id, 0xFF, ing_port1, 0xFF) );

  printf("Doing actions add...\n");
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
  if (opt_IsVfp || opt_IsIfp)
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 1, 0xCC));

  BCM_IF_ERROR_RETURN(bcm_field_entry_prio_set(unit, entry, entry1_prio));

  if (1)
  {
    uint32 ctr_action_id;
    printf("Doing flexctr create to group 0x%08X. StatID: %d..\n",
           group_config.group, statid_used+1);
    BCM_IF_ERROR_RETURN( flexctr_fp_stat_create(unit,
                         group_config.group, opt_TestFpStage, &ctr_action_id) );
    flexctr_cfg[statid_used].flexctr_action_id = ctr_action_id;
    flexctr_cfg[statid_used].counter_index = statid_used;
    printf("Doing flexctr stat attach to entry 0x%08X...\n", entry);
    BCM_IF_ERROR_RETURN( bcm_field_entry_flexctr_attach(unit, entry,
                         &flexctr_cfg[statid_used]) );
    statid_used++;
  }

  /* Installing FP Entry to FP TCAM */
  printf("Doing entry install...\n");
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry) );

  return BCM_E_NONE;
}
