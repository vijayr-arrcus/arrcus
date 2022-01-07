cint_reset();

bcm_policer_config_t   pol_cfg;
bcm_field_entry_t      eid = 1;
bcm_field_group_t      gid;
bcm_field_entry_t      veid;
bcm_field_group_t      vgid;
bcm_policer_t          pol_id;
const uint8            priority = 111;
int                    rv;
int                    unit = 0;
int                    rx_count = 0;   /* Global received packet count */
int ing_port = 120;
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

/* Register callback function for received packets */
bcm_error_t
registerPktioRxCallback(int unit)
{
    const uint32        flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

bcm_error_t
unregisterPktioRxCallback(int unit)
{
    return bcm_pktio_rx_unregister(unit, pktioRxCallback, priority);
}

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int               *count = (auto) cookie;

    (*count)++; /* Bump received packet count */

    printf("pktioRxCallback: rx_count = 0x%x\n", *count);

    return BCM_PKTIO_RX_HANDLED;
}

int testSetup(int unit)
{
    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

int CreatePolicer ()
{
    int ckbits_sec = 1;
    int ckbits_burst = 2;

	bcm_policer_config_t_init(&pol_cfg);
	pol_cfg.mode = bcmPolicerModeCommitted;
	pol_cfg.ckbits_sec = ckbits_sec;
	pol_cfg.ckbits_burst = ckbits_burst;
	pol_cfg.flags |= BCM_POLICER_MODE_PACKETS;
	printf("Doing policer create...\n");
	BCM_IF_ERROR_RETURN( bcm_policer_create(unit, &pol_cfg, &pol_id) );
	print pol_id;

	bcm_policer_config_t_init(&pol_cfg);
    print bcm_policer_get(0, pol_id, &pol_cfg);
    // print pol_cfg;

    return BCM_E_NONE;
}

int ifp_entry_install(int unit, int gid)
{
    int       eid;
	int       level = 0;
    int       rv;
    uint32                          ctr_action_id;
    bcm_gport_t dst_gport;
    bcm_port_t  cpu_port = 0;

    rv = bcm_field_entry_create(unit, gid, &eid);
    if (BCM_FAILURE(rv)) {
        printf("Entry create with id:%d failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }


    //BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, eid, bcmFieldActionCopyToCpu, 0, 0));
    printf("Doing flexctr create to group 0x%08X. StatID: %d..\n",
            gid, statid_used+1);
    BCM_IF_ERROR_RETURN(flexctr_fp_stat_create(unit,
                                               gid,
                                               bcmFieldStageIngress,
                                               &ctr_action_id));
    flexctr_cfg[statid_used].flexctr_action_id = ctr_action_id;
    flexctr_cfg[statid_used].counter_index = statid_used;
    printf("Doing flexctr stat attach to entry 0x%08X...\n", eid);
    BCM_IF_ERROR_RETURN(bcm_field_entry_flexctr_attach(unit, eid,
                                                       &flexctr_cfg[statid_used]) );
    statid_used++;

    printf("Doing Entry %d install...\n", eid);
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, eid));

    return BCM_E_NONE;
}


int ifp_group_create(int unit, bcm_field_group_t *gid, int prio)
{
    bcm_field_group_config_t    group;

	bcm_field_hintid_t          hint_id;
	bcm_field_entry_t           entry;
	bcm_field_hint_t            hint;


    print bcm_field_group_config_t_init(group);

    group.mode = bcmFieldGroupModeAuto;
    group.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group.priority = prio;
    group.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;


    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageIngress);
	BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyDstGport);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionGpCopyToCpu);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionRpDrop);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionPolicerGroup);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionStatGroup);

    /* Create hint  */
    print bcm_field_hints_create(unit, &hint_id);
    printf("Hint create done..\n");

    /* Configuring hint type, and opaque object to be used */
    bcm_field_hint_t_init(&hint);
    printf("Hint init done..\n");
    hint.hint_type = bcmFieldHintTypeFlexCtrOpaqueObjectSel;
    hint.value = bcmFlexctrObjectIngIfpOpaqueObj0;
    /* Associating the above configured hints to hint id */
    BCM_IF_ERROR_RETURN(bcm_field_hints_add(unit, hint_id, &hint));
    group.hintid = hint_id;
    rv = bcm_field_group_config_create(unit, &group);
    if (BCM_FAILURE(rv)) {
        printf("Group create failed: [%s]\n", bcm_errmsg(rv));
        return rv;
    }
    *gid = group.group;

    return BCM_E_NONE;
}

int
configure_dfp(int unit)
{
    bcm_error_t rv;
    bcm_field_destination_match_t dest_fp_match;
    bcm_field_destination_action_t dest_fp_action;
    bcm_port_t  cpu_port = 0;
    bcm_gport_t dst_gport;

    /* Match Criteria */
    rv = bcm_port_gport_get(unit, cpu_port, &dst_gport);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_gport_get failed: [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_field_destination_match_t_init(&dest_fp_match);
    dest_fp_match.gport = dst_gport;
    dest_fp_match.gport_mask = 0xFFFF;
    dest_fp_match.priority = 100;

    /* Action to perform in DFP */
    bcm_field_destination_action_t_init(&dest_fp_action);
    dest_fp_action.drop = TRUE;

    BCM_IF_ERROR_RETURN(bcm_field_destination_entry_add(unit, 0, &dest_fp_match, &dest_fp_action));

    return BCM_E_NONE;
}

bcm_error_t execute(int unit)
{
  bcm_error_t rv;
  int trace_event[] = {
  bcmPktTraceEventIngressL2SrcStaticMove ,
  bcmPktTraceEventIngressPktIntegrityCheckFailed ,
  bcmPktTraceEventIngressProtocolPkt,
  bcmPktTraceEventIngressL2DstLookupMiss,
  bcmPktTraceEventIngressL2DstLookup,
  bcmPktTraceEventIngressL3DstLookupMiss,
  bcmPktTraceEventIngressL3DstLookup,
  bcmPktTraceEventIngressL3HdrError,
  bcmPktTraceEventIngressL3TtlError
  };
  int i = 0;

    bcm_switch_trace_event_mon_t trace_event_mon;

  rv = CreatePolicer ();
  if (BCM_FAILURE(rv)) {
      printf("Policer create failed [%s]\n", bcm_errmsg(rv));
      return rv;
  }

  rv = ifp_group_create(unit, &gid, priority);
  if (BCM_FAILURE(rv)) {
      printf("ifp_entry_group_create failed [%s]\n", bcm_errmsg(rv));
      return rv;
  }

  rv = ifp_entry_install(unit, gid);
  if (BCM_FAILURE(rv)) {
      printf("ifp_entry_install failed [%s]\n", bcm_errmsg(rv));
      return rv;
  }

  rv = configure_dfp(unit);
  if (BCM_FAILURE(rv)) {
      printf("ifp_entry_install failed [%s]\n", bcm_errmsg(rv));
      return rv;
  }

  printf("Disable the specific trace event and copy to CPU.\n");
  for (i = 0; i < 9; i++) {
    bcm_switch_trace_event_mon_t_init(&trace_event_mon);
    trace_event_mon.trace_event = trace_event[i];
    trace_event_mon.actions    = BCM_SWITCH_MON_ACTION_NONE;
    rv = (bcm_switch_trace_event_mon_set(unit, &trace_event_mon));
    if (BCM_FAILURE(rv)) {
      printf("bcm_switch_trace_event_mon_set failed [%s]\n", bcm_errmsg(rv));
    }
  }

  printf("Completed test setup successfully.\n");

  return BCM_E_NONE;
}
print execute(unit);
