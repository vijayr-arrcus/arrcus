cint_reset();
bcm_field_flexctr_config_t flexctr_cfg[10+3];/* 3 extra for test setup */
int                        statid_used = 0;
bcm_error_t
flexctr_fp_stat_create(int unit, int group_id, bcm_field_stage_t FpStage,
                       uint32 *stat_action_id /*OUT*/)
{
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = &action.index_operation;
    bcm_flexctr_value_operation_t *value_a_op = &action.operation_a;
    bcm_flexctr_value_operation_t *value_b_op = &action.operation_b;
    int options = 0;

    bcm_flexctr_action_t_init(&action);

    action.mode = bcmFlexctrCounterModeNormal;
    action.hint = group_id;
    action.drop_count_mode = bcmFlexctrDropCountModeAll;
    action.index_num = 8192;

    if( FpStage == bcmFieldStageLookup ) {
        action.source = bcmFlexctrSourceVfp;
        index_op->object[0] = bcmFlexctrObjectIngVfpOpaqueObj0;
    } else if( FpStage == bcmFieldStageIngress ) {
        action.source = bcmFlexctrSourceIfp;
        index_op->object[0] = bcmFlexctrObjectIngIfpOpaqueObj0;
    } else if( FpStage == bcmFieldStageEgress ) {
        action.source = bcmFlexctrSourceEfp;
        index_op->object[0] = bcmFlexctrObjectEgrEfpOpaqueObj1;
    } else if( FpStage == bcmFlexctrSourceExactMatch ) {
        action.source = bcmFlexctrSourceExactMatch;
        index_op->object[0] = bcmFlexctrObjectIngEmFtOpaqueObj0;
    }

    //index_op->object[0] = bcmFlexctrObjectConstZero;
    index_op->mask_size[0] = 13;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;    // alway = 0
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;
    /* Increase counter per packet. */
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne; // Increment by '1'
    value_a_op->mask_size[0] = 16;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;  // NOOP
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;
    /* Increase counter per packet bytes. */
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;
    //print action;
    BCM_IF_ERROR_RETURN( bcm_flexctr_action_create(unit, options, &action,
                                                    stat_action_id) );
    return BCM_E_NONE;
}
int ing_group_create( int presel_id, int prio) {
  int unit = 0, rv = 0;
  bcm_field_entry_t entry;
  bcm_field_group_config_t group_config;
  bcm_port_t ing_port = 4;
  bcm_pbmp_t pbmp;
  bcm_pbmp_t pbmp_mask;
  bcm_field_hint_t hint;
  bcm_field_hintid_t hint_id;
  uint32 ctr_action_id;
  /* create hint first */
    print bcm_field_hints_create(unit, &hint_id);
    /* configuring hint type, and opaque object to be used */
    bcm_field_hint_t_init(&hint);
    hint.hint_type = bcmFieldHintTypeFlexCtrOpaqueObjectSel;
    hint.value = bcmFlexctrObjectIngIfpOpaqueObj0;
    /* Associating the above configured hints to hint id */
    rv = bcm_field_hints_add(unit, hint_id, &hint);
    if (BCM_FAILURE(rv)) {
        printf("Failed to create hints for opq_obj:%d, error:%s\n", hint.value, bcm_errmsg(rv));
        return rv;
    }
  BCM_PBMP_CLEAR(pbmp_mask);  // Reset
  BCM_PBMP_NEGATE(pbmp_mask, pbmp_mask);
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, ing_port);  // Adds an ingress port
  bcm_field_group_config_t_init(&group_config);
  group_config.mode = bcmFieldGroupModeAuto;
  group_config.priority = prio;

  BCM_FIELD_QSET_INIT(group_config.qset);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPorts);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcIp);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDstIp);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcIp6High);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDstIp6High);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyRangeCheck);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyL4SrcPort);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyL4DstPort);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIpProtocol);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDSCP);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyTtl);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyTcpControl);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIpInfo);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIcmpTypeCode);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOuterVlanId);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIpExtensionHdrNum);

  BCM_FIELD_ASET_INIT(group_config.aset);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDropCancel);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCosMapNew);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpuCancel);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionMirrorIngress);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionL3Switch);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionIngressDstTypeSelect);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionRedirectEgrNextHop);
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);

  group_config.hintid = hint_id;

  printf("Doing group config create...\n");
  rv = bcm_field_group_config_create(unit, &group_config);
  if(BCM_FAILURE(rv))
      printf("ERROR : bcm_field_group_config_create failed : %s\n",bcm_errmsg(rv));
  BCM_IF_ERROR_RETURN(rv);

  printf("Doing entry create to group 0x%08X...\n", group_config.group);
  BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group_config.group, &entry) );
  printf("Entry ID is %d\n", entry);

  printf("Doing qualifier add \n");
  rv = bcm_field_qualify_OuterVlanId(unit, entry, 201, 0xFFFF);
  if (BCM_FAILURE(rv)) {
       printf("Error qualify OuterVlanId\n",
                                   bcm_errmsg(rv));
       return false;
  }

  printf("Doing SrcIp6High qualifier add \n");
  rv = bcm_field_qualify_SrcIp6High(unit, entry, 0x01010101, 0xFFFFFF);
  if (BCM_FAILURE(rv)) {
       printf("Error qualify DstIp\n",
                                   bcm_errmsg(rv));
       return false;
  }

  printf("Doing DstIp6High qualifier add \n");
  rv = bcm_field_qualify_DstIp6High(unit, entry, 0x01010101, 0xFFFFFF);
  if (BCM_FAILURE(rv)) {
       printf("Error qualify DstIp\n",
                                   bcm_errmsg(rv));
       return false;
  }


  printf("Doing actions add...\n");
  rv = (bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
  if(BCM_FAILURE(rv)) {
      printf("ERROR : bcm_field_action_add failed for bcmFieldActionDrop : %s\n",bcm_errmsg(rv));
      BCM_IF_ERROR_RETURN(rv);
 }
  printf("Doing flexctr create to group 0x%08X. StatID: %d..\n",
          group_config.group, statid_used+1);
  BCM_IF_ERROR_RETURN(flexctr_fp_stat_create(unit,
                                             group_config.group,
                                             bcmFieldStageIngress,
                                             &ctr_action_id));
  flexctr_cfg[statid_used].flexctr_action_id = ctr_action_id;
  flexctr_cfg[statid_used].counter_index = statid_used;
  flexctr_cfg[statid_used].color = BCM_FIELD_COUNT_ALL;
  printf("Doing flexctr stat attach to entry 0x%08X...\n", entry);
  rv = bcm_field_entry_flexctr_attach(unit, entry,
                                 &flexctr_cfg[statid_used]);
  if(BCM_FAILURE(rv)) {
      printf("ERROR : bcm_field_action_add failed for bcmFieldActionDrop : %s\n",bcm_errmsg(rv));
                          BCM_IF_ERROR_RETURN(rv);
      return rv;
  }
  statid_used++;

  printf("installing  Field Entry\n");
  rv = bcm_field_entry_install(unit, entry);
  if (BCM_FAILURE(rv)) {
    printf("Error: installing  Field Entry rv:%s\n",
                bcm_errmsg(rv));

  }
}

ing_group_create (0, 200);
