cint_reset();

bcm_policer_config_t   pol_cfg;
bcm_field_entry_t      eid = 1;
bcm_field_group_t      gid;
bcm_field_entry_t      veid;
bcm_field_group_t      vgid;
bcm_policer_t          pol_id;
const uint8            priority = 112;
int                    rv;
int                    unit = 0;
int                    rx_count = 0;   /* Global received packet count */
int ing_port = 120;
bcm_field_flexctr_config_t flexctr_cfg[10+3];/* 3 extra for test setup */
int                        statid_used = 0;
bcm_field_DstType_t dst_type = 0;

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
	//pol_id = 10 | BCM_FIELD_IFP_ID_BASE;
	//pol_cfg.flags |= BCM_POLICER_WITH_ID;
	pol_cfg.flags |= BCM_POLICER_MODE_PACKETS;
	printf("Doing policer create...\n");
	BCM_IF_ERROR_RETURN( bcm_policer_create(unit, &pol_cfg, &pol_id) );
	print pol_id;

	bcm_policer_config_t_init(&pol_cfg);
    print bcm_policer_get(0, pol_id, &pol_cfg);
    // print pol_cfg;

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

    BCM_FIELD_QSET_ADD(group.qset,bcmFieldQualifyDstMac);
     BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyRangeCheck );
     BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyL4SrcPort );
     BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyL4DstPort );
     BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyEtherType );
     BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyIpProtocol );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyTtl );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyTcpControl );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyDstClassL3 );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyIpInfo );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyL2SrcHit );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyIcmpTypeCode );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyIpExtensionHdrNum );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyPacketStatus );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyForwardingStatus );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyL2InterfaceMatch );
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageIngress);
	 BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyCopyToCpuReasonLow);
	BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyDstGport);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyDstType);

    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionGpCopyToCpu);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionRpDrop);
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

int ifp_entry_install(int unit, int gid, int cpu_reason_code)
{
    int       eid;
	int       level = 0;
    int       rv;
    uint32                          ctr_action_id;
    bcm_gport_t dst_gport;
  bcm_port_t  cpu_port = 0;
  int         modid = 0;

    rv = bcm_field_entry_create(unit, gid, &eid);
    if (BCM_FAILURE(rv)) {
        printf("Entry create with id:%d failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, eid, ing_port, -1);
    if (BCM_FAILURE(rv)) {
        print ("Qualifier InPort failed: [%s]\n", bcm_errmsg(rv));
    }
    if(cpu_reason_code > 0) {
      rv = bcm_field_qualify_CopyToCpuReasonLow(unit, eid,
                                              (cpu_reason_code),
                                              (cpu_reason_code));
      if (BCM_FAILURE(rv)) {
          print ("Qualifier copy to cpu reason failed: [%s]\n", bcm_errmsg(rv));
      }
    } else {
      print bcm_port_gport_get(unit, cpu_port, &dst_gport);
      rv = bcm_field_qualify_DstGport(unit, eid, dst_gport);
      if (BCM_FAILURE(rv)) {
          print ("Qualifier DstGport failed: [%s]\n", bcm_errmsg(rv));
      }

      rv = bcm_field_qualify_DstType_get(unit, eid, &dst_type);
      if (BCM_FAILURE(rv)) {
          print ("Qualifier DstType Get failed: [%s]\n", bcm_errmsg(rv));
      }
      rv = bcm_field_qualify_DstType(unit, eid, bcmFieldDstTypeL2Mcast);
      if (BCM_FAILURE(rv)) {
          print ("Qualifier DstType failed: [%s]\n", bcm_errmsg(rv));
      }
    }

    printf("Doing policer attach to entry...%d\n", eid);
    BCM_IF_ERROR_RETURN(bcm_field_entry_policer_attach(unit, eid, level, pol_id) );

    printf("Doing action : RpDrop...%d\n", eid);
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, eid, bcmFieldActionRpDrop, 0, 0));

    printf("Doing action : GpCopyToCpu...%d\n", eid);
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, eid, bcmFieldActionGpCopyToCpu, 0, 0));

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


bcm_error_t testCpuReasons(int unit)
{
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

    rv = ifp_entry_install(unit, gid,BCM_FIELD_COPY_TO_CPU_REASON_PROTOCOL_PKT);
    if (BCM_FAILURE(rv)) {
        printf("ifp_entry_install failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifp_entry_install(unit, gid,BCM_FIELD_COPY_TO_CPU_REASON_L2_DST_LOOKUP_MISS);
    if (BCM_FAILURE(rv)) {
        printf("ifp_entry_install failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifp_entry_install(unit, gid,BCM_FIELD_COPY_TO_CPU_REASON_L2_DST_LOOKUP);
    if (BCM_FAILURE(rv)) {
        printf("ifp_entry_install failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifp_entry_install(unit, gid,BCM_FIELD_COPY_TO_CPU_REASON_L3_DST_LOOKUP_MISS);
    if (BCM_FAILURE(rv)) {
        printf("ifp_entry_install failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifp_entry_install(unit, gid,BCM_FIELD_COPY_TO_CPU_REASON_L3_DST_LOOKUP);
    if (BCM_FAILURE(rv)) {
        printf("ifp_entry_install failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifp_entry_install(unit, gid,BCM_FIELD_COPY_TO_CPU_REASON_L3_TTL_ERROR);
    if (BCM_FAILURE(rv)) {
        printf("ifp_entry_install failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifp_entry_install(unit, gid,0);
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

    return BCM_E_NONE;
}

bcm_error_t testVerify(int unit)
{
  char str[512];

    bshell (unit, "l2 add mac=0x1 vl=10 port=cd0 static=1");
    bshell (unit, "l2 add mac=0x2 vl=10 port=cd1 static=1");
    bshell (unit, "l2 add mac=0x3 vl=10 port=cd2 static=1");
    bshell (unit, "vlan create 10 pbm=cd0,cd1,cd2");
    bshell (unit, "port cd0 en=1 lb=mac");
    bshell (unit, "port cd1 en=0 lb=mac");
    bshell (unit, "port cd2 en=0 lb=mac");

    /* L2_SRC_STATIC_MOVE */
    bshell (unit, "tx 10 pbm=cd0 vl=10 sm=0x3 dm=0x2");
    bshell (unit, "sleep 1");

    /* PROTOCOL_PKT */
    bshell (unit, "tx 6 pbm=cd0 vl=10 sm=0xBB dm=0x180c2000020");
    bshell (unit, "sleep 1");

    printf("IPv4 without fragment - flags= 0x0 and frag offset = 0x0 \n");
  	snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000020000000000018100000508004502002e0000000040ff3d94141414140a0a0a0a0000000000000000000000000000000000000000000000000000f3b42e69; sleep quiet 1", ing_port);
  	bshell(unit, str);

    return BCM_E_NONE;
}

bcm_error_t execute(int unit)
{
  bcm_error_t rv;

  rv = testSetup(unit);
  if (BCM_FAILURE(rv)) {
    printf("testSetup() failed, %s.\n", bcm_errmsg(rv));
    return -1;
  }
  printf("Completed test setup successfully.\n");

  rv = testCpuReasons(unit);
  if (BCM_FAILURE(rv)) {
    printf("testCpuReasons() failed\n");
    return -1;
  }
  printf("Completed test CpuReasons successfully.\n");

  rv = testVerify(unit);
  if (BCM_FAILURE(rv)) {
    printf("testVerify() failed\n");
    return -1;
  }
  printf("Completed test verify successfully.\n");

  return BCM_E_NONE;
}
print execute(unit);
