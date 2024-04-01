cint_reset();
bcm_field_group_config_t fp_group_config;
uint32_t stat_counter_id;


/*
 *
BCM.0>
BCM.0> fp show group 2
fp show group 2
GID          2: gid=0x2, instance=0 mode=Double, stage=Ingress lookup=Enabled, ActionResId={-1}, pbmp={0x000000000000000000000000000000008878787f87878089fe1e0203e1e1e023}
         qset={DstMac, RangeCheck, L4SrcPort, L4DstPort, EtherType, IpProtocol, Ttl, DstPort, TcpControl, PacketRes, DstClassL3, IpType, Stage, StageIngress, IpFrag, L2SrcHit, L2SrcStatic, L2StationMove, IcmpTypeCode},
         aset={CosQCpuNew, CopyToCpu, CopyToCpuCancel, Drop, DropCancel, MirrorIngress, L3Switch, RpDrop, RedirectEgrNextHop, PolicerGroup, StatGroup},
         Qualifiers in Part-0: {DstMac->RangeCheck->L4DstPort->IpProtocol->DstPort->IpType->L2StationMove},
         Qualifiers in Part-1: {RangeCheck->L4SrcPort->EtherType->Ttl->TcpControl->PacketRes->DstClassL3->IpType->IpFrag->L2SrcHit->L2SrcStatic->IcmpTypeCode},

         group_priority= 17
         slice_primary =  {slice_number=3, Entry count=768(0x300), Entry free=768(0x300)},
         slice_secondary = {slice_number=4}
         group_status={prio_min=-2147483647, prio_max=2147483647, entries_total=2304, entries_free=2304,
                       counters_total=83968, counters_free=82432, meters_total=6144, meters_free=5376}
BCM.0>
 */
/* Create IFP flex counter action */
bcm_error_t
flexCounterSetup(int unit, int group)
{
  int options = 0;
  bcm_error_t  rv;
  bcm_flexctr_action_t action;
  bcm_flexctr_index_operation_t *index_op = NULL;
  bcm_flexctr_value_operation_t *value_a_op = NULL;
  bcm_flexctr_value_operation_t *value_b_op = NULL;

  print bcm_flexctr_action_t_init(&action);
  action.flags = 0;
  /* Group ID passed as hint and IFP as source */
  action.hint = group;
  action.hint_type = bcmFlexctrHintFieldGroupId;
  action.source = bcmFlexctrSourceIfp;
  action.mode = bcmFlexctrCounterModeNormal;
  action.index_num = 4098;
  action.drop_count_mode=bcmFlexctrDropCountModeAll;

  /* Counter index is IFP. */
  index_op = &action.index_operation;
  index_op->object[0] = bcmFlexctrObjectStaticIngFieldStageIngress;
  index_op->object_id[0] = 0;
  index_op->mask_size[0] = 15;
  index_op->shift[0] = 0;
  index_op->object[1] = bcmFlexctrObjectConstZero;
  index_op->mask_size[1] = 1;
  index_op->shift[1] = 0;

  /* Increase counter per packet. */
  value_a_op = &action.operation_a;
  value_a_op->select = bcmFlexctrValueSelectCounterValue;
  value_a_op->object[0] = bcmFlexctrObjectConstOne;
  value_a_op->mask_size[0] = 15;
  value_a_op->shift[0] = 0;
  value_a_op->object[1] = bcmFlexctrObjectConstZero;
  value_a_op->mask_size[1] = 1;
  value_a_op->shift[1] = 0;
  value_a_op->type = bcmFlexctrValueOperationTypeInc;

  /* Increase counter per packet bytes. */
  value_b_op = &action.operation_b;
  value_b_op->select = bcmFlexctrValueSelectPacketLength;
  value_b_op->type = bcmFlexctrValueOperationTypeInc;
  /* Create an ingress action */
  rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
  if (BCM_FAILURE(rv)) {
    printf("bcm_flexctr_action_create() FAILED: %s\n", bcm_errmsg(rv));
    return rv;
  }
  printf("stat_counter_id == %d \r\n", stat_counter_id);

  return BCM_E_NONE;
}

uint32_t stat_index = 0;

int
flex_counter_attach (int unit, int eid, uint32_t stat_counter_id, uint32_t *entry_index)
{
	bcm_field_flexctr_config_t flexctr_cfg;
	int rv = 0;

	flexctr_cfg.flexctr_action_id = stat_counter_id;
  flexctr_cfg.counter_index = stat_index * 2; // give space for red and green
  ++stat_index;
  entry_index = flexctr_cfg.counter_index;
	rv = bcm_field_entry_flexctr_attach(unit, eid, &flexctr_cfg);
	if (BCM_FAILURE(rv)) {
		printf("bcm_field_entry_flexctr_attach() FAILED: %s\n", bcm_errmsg(rv));
		return rv;
	}
	return rv;
}
void
_my_bcm_create_fp_group (int unit)
{
  int priority;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;

  bcm_field_group_config_t_init(&fp_group_config);
  BCM_FIELD_QSET_INIT(fp_group_config.qset);
  BCM_FIELD_ASET_INIT(fp_group_config.aset);

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_ASET_INIT(aset);

  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTtl);
//  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp6HopLimit);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4SrcPort);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIcmpTypeCode);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTcpControl);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyPacketRes);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstClassL3);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyRangeCheck);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpFrag);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2SrcStatic);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2SrcHit);
 // BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2StationMove);

  /*
   * Create ASET
   */
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropCancel);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionCopyToCpu);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionCopyToCpuCancel);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionL3Switch);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionRpDrop);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionCosQCpuNew);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionMirrorIngress);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionStatGroup);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirectEgrNextHop);

  fp_group_config.flags |= BCM_FIELD_GROUP_CREATE_SINGLE;
  priority = 3; // Group priority.
  priority = 17; // Group priority.
  fp_group_config.priority = priority;
  fp_group_config.qset = qset;
  fp_group_config.aset = aset;
//  sal_memcpy(&fp_group_config.qset, &qset, sizeof(bcm_field_qset_t));
//  sal_memcpy(&fp_group_config.aset, &aset, sizeof(bcm_field_aset_t));
  print bcm_field_group_config_create(unit, &fp_group_config);
  print fp_group_config;
}

void
my_bcm_create_fp_group (int unit)
{
  int priority;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;

  bcm_field_group_config_t_init(&fp_group_config);
  BCM_FIELD_QSET_INIT(fp_group_config.qset);
  BCM_FIELD_ASET_INIT(fp_group_config.aset);

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_ASET_INIT(aset);

  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);
 // BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2StationMove);

  /*
   * Create ASET
   */
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropCancel);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionCopyToCpu);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionCopyToCpuCancel);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionL3Switch);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionRpDrop);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionCosQCpuNew);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionMirrorIngress);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionGpStatGroup);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionRpStatGroup);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirectEgrNextHop);

  fp_group_config.flags |= BCM_FIELD_GROUP_CREATE_SINGLE;
  priority = 3; // Group priority.
  priority = 17; // Group priority.
  fp_group_config.priority = priority;
  fp_group_config.qset = qset;
  fp_group_config.aset = aset;
//  sal_memcpy(&fp_group_config.qset, &qset, sizeof(bcm_field_qset_t));
//  sal_memcpy(&fp_group_config.aset, &aset, sizeof(bcm_field_aset_t));
  print bcm_field_group_config_create(unit, &fp_group_config);
  print fp_group_config.group;
  print flexCounterSetup(unit, fp_group_config.group);
}
bcm_field_entry_t v4_any_dst_fp_entry_id;
bcm_field_entry_t v4_any_dst_range_fp_entry_id;
bcm_field_entry_t v4_any_src_fp_entry_id;
bcm_field_entry_t v4_opt_dst_fp_entry_id;
bcm_field_entry_t v4_opt_src_fp_entry_id;

int v4_any_dst_fp_stats_id;
int v4_any_dst_range_fp_stats_id;
int v4_any_src_fp_stats_id;
int v4_opt_dst_fp_stats_id;
int v4_opt_src_fp_stats_id;

bcm_policer_t v4_any_dst_policer_id;
bcm_policer_t v4_any_dst_range_policer_id;
bcm_policer_t v4_any_src_policer_id;
bcm_policer_t v4_opt_dst_policer_id;
bcm_policer_t v4_opt_src_policer_id;

bcm_l4_port_t v4_port_dst = 1212;
bcm_l4_port_t v4_port_src = 2323;
bcm_l4_port_t v4_port_dst_opt = 4545;
bcm_l4_port_t v4_port_src_opt = 5656;

bcm_field_entry_t v6_any_dst_fp_entry_id;
bcm_field_entry_t v6_any_src_fp_entry_id;
bcm_field_entry_t v6_opt_dst_fp_entry_id;
bcm_field_entry_t v6_opt_src_fp_entry_id;

int v6_any_dst_fp_stats_id;
int v6_any_src_fp_stats_id;
int v6_opt_dst_fp_stats_id;
int v6_opt_src_fp_stats_id;

bcm_policer_t v6_any_dst_policer_id;
bcm_policer_t v6_any_src_policer_id;
bcm_policer_t v6_opt_dst_policer_id;
bcm_policer_t v6_opt_src_policer_id;

bcm_l4_port_t v6_port_dst = 1212;
bcm_l4_port_t v6_port_src = 2323;
bcm_l4_port_t v6_port_dst_opt = 4545;
bcm_l4_port_t v6_port_src_opt = 5656;

int local_tc = 1;

/*
 * v4_any_dst_fp_entry_id = 319 (0x13f)

BCM.0>
BCM.0> fp show entry 319
fp show entry 319
EID 0x0000013f: gid=0x2,
 slice=3, slice_idx=0, part=0, prio=0x3d1, flags=0x10602, Installed, Enabled, color_indep=1
 slice=4, slice_idx=0, part=1, prio=0x3d1, flags=0x10604, Installed, Enabled, color_indep=1

 L4DstPort
    Part:0 Offset0: 16 Width0:   4
    Part:0 Offset1: 36 Width1:   4
    Part:0 Offset2: 40 Width2:   4
    Part:0 Offset3: 44 Width3:   4
    DATA=0x000004bc
    MASK=0x0000ffff
 DstPort
    Part:0 Offset0: 48 Width0:  16
    Part:0 Offset1: 152 Width1:   1
    DATA=0x00000000
    MASK=0x0001ffff
 IpType
    Part:0 Offset0: 104 Width0:   4
    Part:1 Offset1: 158 Width1:   1
    DATA=0x00000000
    MASK=0x0000000e
         action={act=RpDrop, param0=0(0), param1=0(0), param2=0(0), param3=0(0)}
         action={act=CosQCpuNew, param0=1(0x1), param1=1(0x1), param2=0(0), param3=0(0)}
         policer={pid=0x26, level=0, peak_kbits_sec=0, peak_kbits_burst=0, commit_kbits_sec=0x3e8, commit_kbits_burst=0xc8, PacketBased=0x1, mode=0, entries=2, Clean}
         statistics={stat id 125  slice = 9 idx=3 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>

BCM.0> fctr stat show statid=268435457 0-10
Counter value: Action Id=268435457
        index 0
                A:                     0,  B:
   0
        index 1
                A:                     0,  B:
   0
        index 2
                A:                     0,  B:
   0
        index 3
                A:                     0,  B:
   0
        index 4
                A:                     0,  B:
   0
        index 5
                A:                     0,  B:
   0
        index 6
                A:                     0,  B:
   0
        index 7
                A:                     0,  B:
   0
        index 8
                A:                     0,  B:
   0
        index 9
                A:                     0,  B:
   0
        index 10
                A:                     0,  B:
   0
BCM.0>

 */
void
bcm_ipv4_entry_create_dst_any (int unit)
{
  uint32_t entry_stats_id = 0;
  bcm_policer_config_t pol_cfg;
  print bcm_field_entry_create(unit, fp_group_config.group, &v4_any_dst_fp_entry_id);
  print v4_any_dst_fp_entry_id;
  print bcm_field_qualify_clear(unit, v4_any_dst_fp_entry_id);
  print bcm_field_qualify_IpType(unit, v4_any_dst_fp_entry_id, bcmFieldIpTypeIpv4Any);
  print bcm_field_qualify_L4DstPort(unit, v4_any_dst_fp_entry_id,v4_port_dst, 0xFFFF);
  print bcm_field_qualify_DstPort(unit, v4_any_dst_fp_entry_id, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
  print bcm_field_entry_prio_set(unit, v4_any_dst_fp_entry_id, (1000-23));

  print flex_counter_attach(unit, v4_any_dst_fp_entry_id, stat_counter_id, &entry_stats_id);
  printf("%s: stats id attached to entry %d is %d\n",
         __FUNCTION__,  v4_any_dst_fp_entry_id, entry_stats_id);

  print bcm_field_action_add(unit, v4_any_dst_fp_entry_id, bcmFieldActionCosQCpuNew, local_tc, local_tc++);
  print bcm_field_action_add(unit, v4_any_dst_fp_entry_id, bcmFieldActionRpDrop, 0, 0);
  print bcm_field_action_add(unit, entry, bcmFieldActionGpStatGroup, 0, 0);
  print bcm_field_action_add(unit, entry, bcmFieldActionRpStatGroup, 1, 0);
  bcm_policer_config_t_init(&pol_cfg);
  pol_cfg.mode = bcmPolicerModeSrTcm;
  pol_cfg.ckbits_sec = 1000;
  pol_cfg.ckbits_burst = 200;
  pol_cfg.flags = BCM_POLICER_MODE_PACKETS;
  pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
  print bcm_policer_create(unit, &pol_cfg, &v4_any_dst_policer_id);

  print bcm_field_entry_policer_attach(unit, v4_any_dst_fp_entry_id,
                                       0, v4_any_dst_policer_id);
  print bcm_field_entry_install(unit, v4_any_dst_fp_entry_id);
}

/*
 *
  int min = 0, max = 5;
  With min = 0 and max = 5 we always get data =  0x00000040;
  with min = 0 and max = 6 data = 0x00000080;
BCM.0>
BCM.0> fp show entry 116
fp show entry 116
EID 0x00000074: gid=0x2,
 slice=3, slice_idx=0, part=0, prio=0x3d1, flags=0x10602, Installed, Enabled, color_indep=1
 slice=4, slice_idx=0, part=1, prio=0x3d1, flags=0x10604, Installed, Enabled, color_indep=1

 RangeCheck
    Part:0 Offset1: 64 Width1:  16
    Part:1 Offset0:  0 Width0:  16
    DATA=0x00000040
    MASK=0x00000040
 DstPort
    Part:0 Offset0: 48 Width0:  16
    Part:0 Offset1: 152 Width1:   1
    DATA=0x00000000
    MASK=0x0001ffff
 IpType
    Part:0 Offset0: 104 Width0:   4
    Part:1 Offset1: 158 Width1:   1
    DATA=0x00000000
    MASK=0x0000000e
         action={act=RpDrop, param0=0(0), param1=0(0), param2=0(0), param3=0(0)}
         action={act=CosQCpuNew, param0=1(0x1), param1=1(0x1), param2=0(0), param3=0(0)}
         policer={pid=0x26, level=0, peak_kbits_sec=0, peak_kbits_burst=0, commit_kbits_sec=0x3e8, commit_kbits_burst=0xc8, PacketBased=0x1, mode=0, entries=2, Clean}
         statistics={stat id 37  slice = 9 idx=3 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>
 */

/*
 * cint>
 * cint> print min;
 * print min;
 * int min = 33434 (0x829a)
 * cint>
 * cint>
 * cint> print max;
 * print max;
 * int max = 33534 (0x82fe)
 * cint>
 * BCM.0>
 BCM.0>
BCM.0> fp show entry 123
fp show entry 123
EID 0x0000007b: gid=0x2,
 slice=3, slice_idx=0x6, part=0, prio=0x3d1, flags=0x10602, Installed, Enabled, color_indep=1
 slice=4, slice_idx=0x6, part=1, prio=0x3d1, flags=0x10604, Installed, Enabled, color_indep=1

 RangeCheck
    Part:0 Offset1: 64 Width1:  16
    Part:1 Offset0:  0 Width0:  16
    DATA=0x00000002
    MASK=0x00000002
 DstPort
    Part:0 Offset0: 48 Width0:  16
    Part:0 Offset1: 152 Width1:   1
    DATA=0x00000000
    MASK=0x0001ffff
 IpType
    Part:0 Offset0: 104 Width0:   4
    Part:1 Offset1: 158 Width1:   1
    DATA=0x00000000
    MASK=0x0000000e
         action={act=RpDrop, param0=0(0), param1=0(0), param2=0(0), param3=0(0)}
         action={act=CosQCpuNew, param0=1(0x1), param1=1(0x1), param2=0(0), param3=0(0)}
         policer={pid=0x2d, level=0, peak_kbits_sec=0, peak_kbits_burst=0, commit_kbits_sec=0x3e8, commit_kbits_burst=0xc8, PacketBased=0x1, mode=0, entries=2, Clean}
         statistics={stat id 44  slice = 9 idx=15 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>*
 */

void
bcm_ipv4_entry_create_dst_any_range (int unit)
{
  int flags = BCM_FIELD_RANGE_DSTPORT;
  bcm_range_config_t range_config;
  bcm_field_range_t range_id;
  bcm_policer_config_t pol_cfg;
  bcm_field_stat_t stats[2] = {bcmFieldStatPackets, bcmFieldStatBytes};
  bcm_pbmp_t              pbmp_mask;
  int min = 0, max = 5;

  print bcm_field_entry_create(unit, fp_group_config.group, &v4_any_dst_range_fp_entry_id);
  print v4_any_dst_range_fp_entry_id;
  print bcm_field_range_create(unit, &range_id, flags, min, max);
  print range_id;
 // print bcm_field_group_ports_get(unit, fp_group_config.group, &pbmp_mask);
//  print bcm_range_config_t_init(&range_config);
 // range_config.rtype = bcmRangeTypeL4DstPort;
  //range_config.min = min;
//  range_config.max = max;
 // range_config.ports = pbmp_mask;
 // print bcm_range_create(unit, 0, &range_config);
//  print range_config;

  print bcm_field_qualify_clear(unit, v4_any_dst_range_fp_entry_id);
  print bcm_field_qualify_RangeCheck(unit, v4_any_dst_range_fp_entry_id, range_id, 0);
  print bcm_field_qualify_IpType(unit, v4_any_dst_range_fp_entry_id, bcmFieldIpTypeIpv4Any);
  print bcm_field_qualify_DstPort(unit, v4_any_dst_range_fp_entry_id, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
  print bcm_field_entry_prio_set(unit, v4_any_dst_range_fp_entry_id, (1000-23));

  print bcm_field_stat_create(unit, fp_group_config.group,
                               2,stats , &v4_any_dst_range_fp_stats_id);
  print bcm_field_entry_stat_attach(unit, v4_any_dst_range_fp_entry_id, v4_any_dst_range_fp_stats_id);
  print bcm_field_action_add(unit, v4_any_dst_range_fp_entry_id, bcmFieldActionCosQCpuNew, local_tc, local_tc);
  print bcm_field_action_add(unit, v4_any_dst_range_fp_entry_id, bcmFieldActionRpDrop, 0, 0);
  bcm_policer_config_t_init(&pol_cfg);
  pol_cfg.mode = bcmPolicerModeSrTcm;
  pol_cfg.ckbits_sec = 1000;
  pol_cfg.ckbits_burst = 200;
  pol_cfg.flags = BCM_POLICER_MODE_PACKETS;
  pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
  print bcm_policer_create(unit, &pol_cfg, &v4_any_dst_range_policer_id);

  print bcm_field_entry_policer_attach(unit, v4_any_dst_range_fp_entry_id,
                                       0, v4_any_dst_range_policer_id);
  print bcm_field_entry_install(unit, v4_any_dst_range_fp_entry_id);
}

/*
 *
int v4_any_src_fp_entry_id = 320 (0x140)

BCM.0>
BCM.0> fp show entry 320
fp show entry 320
EID 0x00000140: gid=0x2,
 slice=3, slice_idx=0, part=0, prio=0x3d2, flags=0x10602, Installed, Enabled, color_indep=1
 slice=4, slice_idx=0, part=1, prio=0x3d2, flags=0x10604, Installed, Enabled, color_indep=1

 L4SrcPort
    Part:1 Offset0: 20 Width0:  16
    DATA=0x00000913
    MASK=0x0000ffff
 DstPort
    Part:0 Offset0: 48 Width0:  16
    Part:0 Offset1: 152 Width1:   1
    DATA=0x00000000
    MASK=0x0001ffff
 IpType
    Part:0 Offset0: 104 Width0:   4
    Part:1 Offset1: 158 Width1:   1
    DATA=0x00000000
    MASK=0x0000000e
 IcmpTypeCode
    Part:1 Offset0: 20 Width0:  16
    DATA=0x00000913
    MASK=0x0000ffff
         action={act=RpDrop, param0=0(0), param1=0(0), param2=0(0), param3=0(0)}
         action={act=CosQCpuNew, param0=2(0x2), param1=2(0x2), param2=0(0), param3=0(0)}
         policer={pid=0x27, level=0, peak_kbits_sec=0, peak_kbits_burst=0, commit_kbits_sec=0x3e8, commit_kbits_burst=0xc8, PacketBased=0x1, mode=0, entries=2, Clean}
         statistics={stat id 126  slice = 9 idx=2 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>
 */
void
bcm_ipv4_entry_create_src_any (int unit)
{
  bcm_policer_config_t pol_cfg;
  bcm_field_stat_t stats[2] = {bcmFieldStatPackets, bcmFieldStatBytes};

  print bcm_field_entry_create(unit, fp_group_config.group, &v4_any_src_fp_entry_id);
  print v4_any_src_fp_entry_id;

  print bcm_field_qualify_clear(unit, v4_any_src_fp_entry_id);
  print bcm_field_qualify_IpType(unit, v4_any_src_fp_entry_id, bcmFieldIpTypeIpv4Any);
  print bcm_field_qualify_L4SrcPort(unit, v4_any_src_fp_entry_id,v4_port_src, 0xFFFF);
  print bcm_field_qualify_DstPort(unit, v4_any_src_fp_entry_id, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);

  print bcm_field_entry_prio_set(unit, v4_any_src_fp_entry_id, (1000-22));
  print bcm_field_stat_create(unit, fp_group_config.group,
                               2,stats , &v4_any_src_fp_stats_id);
  print bcm_field_entry_stat_attach(unit, v4_any_src_fp_entry_id, v4_any_src_fp_stats_id);

  print bcm_field_action_add(unit, v4_any_src_fp_entry_id, bcmFieldActionCosQCpuNew, local_tc, local_tc++);
  print bcm_field_action_add(unit, v4_any_src_fp_entry_id, bcmFieldActionRpDrop, 0, 0);

  bcm_policer_config_t_init(&pol_cfg);
  pol_cfg.mode = bcmPolicerModeSrTcm;
  pol_cfg.ckbits_sec = 1000;
  pol_cfg.ckbits_burst = 200;
  pol_cfg.flags = BCM_POLICER_MODE_PACKETS;
  pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
  print bcm_policer_create(unit, &pol_cfg, &v4_any_src_policer_id);

  print bcm_field_entry_policer_attach(unit, v4_any_src_fp_entry_id,
                                 0, v4_any_src_policer_id);

  print bcm_field_entry_install(unit, v4_any_src_fp_entry_id);
}

/*
 *
BCM.0>
BCM.0> fp show entry 321
fp show entry 321
EID 0x00000141: gid=0x2,
 slice=3, slice_idx=0, part=0, prio=0x3d3, flags=0x10602, Installed, Enabled, color_indep=1
 slice=4, slice_idx=0, part=1, prio=0x3d3, flags=0x10604, Installed, Enabled, color_indep=1

 L4DstPort
    Part:0 Offset0: 16 Width0:   4
    Part:0 Offset1: 36 Width1:   4
    Part:0 Offset2: 40 Width2:   4
    Part:0 Offset3: 44 Width3:   4
    DATA=0x000011c1
    MASK=0x0000ffff
 DstPort
    Part:0 Offset0: 48 Width0:  16
    Part:0 Offset1: 152 Width1:   1
    DATA=0x00000000
    MASK=0x0001ffff
 IpType
    Part:0 Offset0: 104 Width0:   4
    Part:1 Offset1: 158 Width1:   1
    DATA=0x00000001
    MASK=0x0000001f
         action={act=RpDrop, param0=0(0), param1=0(0), param2=0(0), param3=0(0)}
         action={act=CosQCpuNew, param0=3(0x3), param1=3(0x3), param2=0(0), param3=0(0)}
         policer={pid=0x28, level=0, peak_kbits_sec=0, peak_kbits_burst=0, commit_kbits_sec=0x3e8, commit_kbits_burst=0xc8, PacketBased=0x1, mode=0, entries=2, Clean}
         statistics={stat id 127  slice = 9 idx=7 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>
*/

void
bcm_ipv4_entry_create_dst_opt (int unit)
{
  bcm_policer_config_t pol_cfg;
  bcm_field_stat_t stats[2] = {bcmFieldStatPackets, bcmFieldStatBytes};
  print bcm_field_entry_create(unit, fp_group_config.group, &v4_opt_dst_fp_entry_id);
  print v4_opt_dst_fp_entry_id;
  print bcm_field_qualify_clear(unit, v4_opt_dst_fp_entry_id);
  print bcm_field_qualify_IpType(unit, v4_opt_dst_fp_entry_id, bcmFieldIpTypeIpv4Any);
  print bcm_field_qualify_IpType(unit, v4_opt_dst_fp_entry_id, bcmFieldIpTypeIpv4WithOpts);
  print bcm_field_qualify_L4DstPort(unit, v4_opt_dst_fp_entry_id,v4_port_dst_opt, 0xFFFF);
  print bcm_field_qualify_DstPort(unit, v4_opt_dst_fp_entry_id, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
  print bcm_field_entry_prio_set(unit, v4_opt_dst_fp_entry_id, (1000-21));
  print bcm_field_stat_create(unit, fp_group_config.group,
                               2,stats , &v4_opt_dst_fp_stats_id);
  print bcm_field_entry_stat_attach(unit, v4_opt_dst_fp_entry_id, v4_opt_dst_fp_stats_id);
  print bcm_field_action_add(unit, v4_opt_dst_fp_entry_id, bcmFieldActionCosQCpuNew, local_tc, local_tc++);
  print bcm_field_action_add(unit, v4_opt_dst_fp_entry_id, bcmFieldActionRpDrop, 0, 0);
  bcm_policer_config_t_init(&pol_cfg);
  pol_cfg.mode = bcmPolicerModeSrTcm;
  pol_cfg.ckbits_sec = 1000;
  pol_cfg.ckbits_burst = 200;
  pol_cfg.flags = BCM_POLICER_MODE_PACKETS;
  pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
  print bcm_policer_create(unit, &pol_cfg, &v4_opt_dst_policer_id);

  print bcm_field_entry_policer_attach(unit, v4_opt_dst_fp_entry_id,
                                 0, v4_opt_dst_policer_id);
  print bcm_field_entry_install(unit, v4_opt_dst_fp_entry_id);
}

/*
 * int v4_opt_src_fp_entry_id = 322 (0x142)
BCM.0>
BCM.0> fp show entry 322
fp show entry 322
EID 0x00000142: gid=0x2,
 slice=3, slice_idx=0, part=0, prio=0x3d4, flags=0x10602, Installed, Enabled, color_indep=1
 slice=4, slice_idx=0, part=1, prio=0x3d4, flags=0x10604, Installed, Enabled, color_indep=1

 L4SrcPort
    Part:1 Offset0: 20 Width0:  16
    DATA=0x00001618
    MASK=0x0000ffff
 DstPort
    Part:0 Offset0: 48 Width0:  16
    Part:0 Offset1: 152 Width1:   1
    DATA=0x00000000
    MASK=0x0001ffff
 IpType
    Part:0 Offset0: 104 Width0:   4
    Part:1 Offset1: 158 Width1:   1
    DATA=0x00000001
    MASK=0x0000001f
 IcmpTypeCode
    Part:1 Offset0: 20 Width0:  16
    DATA=0x00001618
    MASK=0x0000ffff
         action={act=RpDrop, param0=0(0), param1=0(0), param2=0(0), param3=0(0)}
         action={act=CosQCpuNew, param0=4(0x4), param1=4(0x4), param2=0(0), param3=0(0)}
         policer={pid=0x29, level=0, peak_kbits_sec=0, peak_kbits_burst=0, commit_kbits_sec=0x3e8, commit_kbits_burst=0xc8, PacketBased=0x1, mode=0, entries=2, Clean}
         statistics={stat id 128  slice = 9 idx=6 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>
 */
void
bcm_ipv4_entry_create_src_opt (int unit)
{
  bcm_policer_config_t pol_cfg;
  bcm_field_stat_t stats[2] = {bcmFieldStatPackets, bcmFieldStatBytes};
  print bcm_field_entry_create(unit, fp_group_config.group, &v4_opt_src_fp_entry_id);
  print v4_opt_src_fp_entry_id;
  print bcm_field_qualify_clear(unit, v4_opt_src_fp_entry_id);
  print bcm_field_qualify_IpType(unit, v4_opt_src_fp_entry_id, bcmFieldIpTypeIpv4Any);
  print bcm_field_qualify_IpType(unit, v4_opt_src_fp_entry_id, bcmFieldIpTypeIpv4WithOpts);
  print bcm_field_qualify_L4SrcPort(unit, v4_opt_src_fp_entry_id,v4_port_src_opt, 0xFFFF);
  print bcm_field_qualify_DstPort(unit, v4_opt_src_fp_entry_id, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
  print bcm_field_entry_prio_set(unit, v4_opt_src_fp_entry_id,
                                 (1000-20));
  print bcm_field_stat_create(unit, fp_group_config.group,
                               2,stats , &v4_opt_src_fp_stats_id);
  print bcm_field_entry_stat_attach(unit, v4_opt_src_fp_entry_id, v4_opt_src_fp_stats_id);
  print bcm_field_action_add(unit, v4_opt_src_fp_entry_id, bcmFieldActionCosQCpuNew, local_tc, local_tc++);
  print bcm_field_action_add(unit, v4_opt_src_fp_entry_id, bcmFieldActionRpDrop, 0, 0);
  bcm_policer_config_t_init(&pol_cfg);
  pol_cfg.mode = bcmPolicerModeSrTcm;
  pol_cfg.ckbits_sec = 1000;
  pol_cfg.ckbits_burst = 200;
  pol_cfg.flags = BCM_POLICER_MODE_PACKETS;
  pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
  print bcm_policer_create(unit, &pol_cfg, &v4_opt_src_policer_id);

  print bcm_field_entry_policer_attach(unit, v4_opt_src_fp_entry_id,
                                 0, v4_opt_src_policer_id);
  print bcm_field_entry_install(unit, v4_opt_src_fp_entry_id);
}

/*
 int v6_any_dst_fp_entry_id = 323 (0x143)

BCM.0>
BCM.0> fp show entry 323
fp show entry 323
EID 0x00000143: gid=0x2,
 slice=3, slice_idx=0x4, part=0, prio=0x3d1, flags=0x10602, Installed, Enabled, color_indep=1
 slice=4, slice_idx=0x4, part=1, prio=0x3d1, flags=0x10604, Installed, Enabled, color_indep=1

 L4DstPort
    Part:0 Offset0: 16 Width0:   4
    Part:0 Offset1: 36 Width1:   4
    Part:0 Offset2: 40 Width2:   4
    Part:0 Offset3: 44 Width3:   4
    DATA=0x000004bc
    MASK=0x0000ffff
 DstPort
    Part:0 Offset0: 48 Width0:  16
    Part:0 Offset1: 152 Width1:   1
    DATA=0x00000000
    MASK=0x0001ffff
 IpType
    Part:0 Offset0: 104 Width0:   4
    Part:1 Offset1: 158 Width1:   1
    DATA=0x00000004
    MASK=0x0000000c
         action={act=RpDrop, param0=0(0), param1=0(0), param2=0(0), param3=0(0)}
         action={act=CosQCpuNew, param0=4(0x4), param1=4(0x4), param2=0(0), param3=0(0)}
         policer={pid=0x2a, level=0, peak_kbits_sec=0, peak_kbits_burst=0, commit_kbits_sec=0x3e8, commit_kbits_burst=0xc8, PacketBased=0x1, mode=0, entries=2, Clean}
         statistics={stat id 129  slice = 9 idx=5 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>
*/

void
bcm_ipv6_entry_create_dst_any (int unit)
{
  bcm_policer_config_t pol_cfg;
  bcm_field_stat_t stats[2] = {bcmFieldStatPackets, bcmFieldStatBytes};
  print bcm_field_entry_create(unit, fp_group_config.group, &v6_any_dst_fp_entry_id);
  print v6_any_dst_fp_entry_id;
  print bcm_field_qualify_clear(unit, v6_any_dst_fp_entry_id);
  print bcm_field_qualify_IpType(unit, v6_any_dst_fp_entry_id, bcmFieldIpTypeIpv6);
  print bcm_field_qualify_L4DstPort(unit, v6_any_dst_fp_entry_id,v6_port_dst, 0xFFFF);
  print bcm_field_qualify_DstPort(unit, v6_any_dst_fp_entry_id, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
  print bcm_field_entry_prio_set(unit, v6_any_dst_fp_entry_id, (1000-23));

  print bcm_field_stat_create(unit, fp_group_config.group,
                               2,stats , &v6_any_dst_fp_stats_id);
  print bcm_field_entry_stat_attach(unit, v6_any_dst_fp_entry_id, v6_any_dst_fp_stats_id);
  print bcm_field_action_add(unit, v6_any_dst_fp_entry_id, bcmFieldActionCosQCpuNew, --local_tc, local_tc);
  print bcm_field_action_add(unit, v6_any_dst_fp_entry_id, bcmFieldActionRpDrop, 0, 0);
  bcm_policer_config_t_init(&pol_cfg);
  pol_cfg.mode = bcmPolicerModeSrTcm;
  pol_cfg.ckbits_sec = 1000;
  pol_cfg.ckbits_burst = 200;
  pol_cfg.flags = BCM_POLICER_MODE_PACKETS;
  pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
  print bcm_policer_create(unit, &pol_cfg, &v6_any_dst_policer_id);

  print bcm_field_entry_policer_attach(unit, v6_any_dst_fp_entry_id,
                                       0, v6_any_dst_policer_id);
  print bcm_field_entry_install(unit, v6_any_dst_fp_entry_id);
}

/*
 *
int v6_any_src_fp_entry_id = 324 (0x144)
BCM.0>
BCM.0> fp show entry 324
fp show entry 324
EID 0x00000144: gid=0x2,
 slice=3, slice_idx=0x3, part=0, prio=0x3d2, flags=0x10602, Installed, Enabled, color_indep=1
 slice=4, slice_idx=0x3, part=1, prio=0x3d2, flags=0x10604, Installed, Enabled, color_indep=1

 L4SrcPort
    Part:1 Offset0: 20 Width0:  16
    DATA=0x00000913
    MASK=0x0000ffff
 DstPort
    Part:0 Offset0: 48 Width0:  16
    Part:0 Offset1: 152 Width1:   1
    DATA=0x00000000
    MASK=0x0001ffff
 IpType
    Part:0 Offset0: 104 Width0:   4
    Part:1 Offset1: 158 Width1:   1
    DATA=0x00000004
    MASK=0x0000000c
 IcmpTypeCode
    Part:1 Offset0: 20 Width0:  16
    DATA=0x00000913
    MASK=0x0000ffff
         action={act=RpDrop, param0=0(0), param1=0(0), param2=0(0), param3=0(0)}
         action={act=CosQCpuNew, param0=3(0x3), param1=3(0x3), param2=0(0), param3=0(0)}
         policer={pid=0x2b, level=0, peak_kbits_sec=0, peak_kbits_burst=0, commit_kbits_sec=0x3e8, commit_kbits_burst=0xc8, PacketBased=0x1, mode=0, entries=2, Clean}
         statistics={stat id 130  slice = 9 idx=4 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>

 */
void
bcm_ipv6_entry_create_src_any (int unit)
{
  bcm_policer_config_t pol_cfg;
  bcm_field_stat_t stats[2] = {bcmFieldStatPackets, bcmFieldStatBytes};

  print bcm_field_entry_create(unit, fp_group_config.group, &v6_any_src_fp_entry_id);
  print v6_any_src_fp_entry_id;

  print bcm_field_qualify_clear(unit, v6_any_src_fp_entry_id);
  print bcm_field_qualify_IpType(unit, v6_any_src_fp_entry_id, bcmFieldIpTypeIpv6);
  print bcm_field_qualify_L4SrcPort(unit, v6_any_src_fp_entry_id,v6_port_src, 0xFFFF);
  print bcm_field_qualify_DstPort(unit, v6_any_src_fp_entry_id, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);

  print bcm_field_entry_prio_set(unit, v6_any_src_fp_entry_id, (1000-22));
  print bcm_field_stat_create(unit, fp_group_config.group,
                               2,stats , &v6_any_src_fp_stats_id);
  print bcm_field_entry_stat_attach(unit, v6_any_src_fp_entry_id, v6_any_src_fp_stats_id);

  print bcm_field_action_add(unit, v6_any_src_fp_entry_id, bcmFieldActionCosQCpuNew, --local_tc, local_tc);
  print bcm_field_action_add(unit, v6_any_src_fp_entry_id, bcmFieldActionRpDrop, 0, 0);

  bcm_policer_config_t_init(&pol_cfg);
  pol_cfg.mode = bcmPolicerModeSrTcm;
  pol_cfg.ckbits_sec = 1000;
  pol_cfg.ckbits_burst = 200;
  pol_cfg.flags = BCM_POLICER_MODE_PACKETS;
  pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
  print bcm_policer_create(unit, &pol_cfg, &v6_any_src_policer_id);

  print bcm_field_entry_policer_attach(unit, v6_any_src_fp_entry_id,
                                 0, v6_any_src_policer_id);

  print bcm_field_entry_install(unit, v6_any_src_fp_entry_id);
}

/*
int v6_opt_dst_fp_entry_id = 325 (0x145)
BCM.0>
BCM.0> fp show entry 325
fp show entry 325
EID 0x00000145: gid=0x2,
 slice=3, slice_idx=0x2, part=0, prio=0x3d3, flags=0x10602, Installed, Enabled, color_indep=1
 slice=4, slice_idx=0x2, part=1, prio=0x3d3, flags=0x10604, Installed, Enabled, color_indep=1

 L4DstPort
    Part:0 Offset0: 16 Width0:   4
    Part:0 Offset1: 36 Width1:   4
    Part:0 Offset2: 40 Width2:   4
    Part:0 Offset3: 44 Width3:   4
    DATA=0x000011c1
    MASK=0x0000ffff
 DstPort
    Part:0 Offset0: 48 Width0:  16
    Part:0 Offset1: 152 Width1:   1
    DATA=0x00000000
    MASK=0x0001ffff
 IpType
    Part:0 Offset0: 104 Width0:   4
    Part:1 Offset1: 158 Width1:   1
    DATA=0x00000005
    MASK=0x0000001f
         action={act=RpDrop, param0=0(0), param1=0(0), param2=0(0), param3=0(0)}
         action={act=CosQCpuNew, param0=3(0x3), param1=3(0x3), param2=0(0), param3=0(0)}
         policer={pid=0x2c, level=0, peak_kbits_sec=0, peak_kbits_burst=0, commit_kbits_sec=0x3e8, commit_kbits_burst=0xc8, PacketBased=0x1, mode=0, entries=2, Clean}
         statistics={stat id 131  slice = 9 idx=15 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>
*/
void
bcm_ipv6_entry_create_dst_opt (int unit)
{
  bcm_policer_config_t pol_cfg;
  bcm_field_stat_t stats[2] = {bcmFieldStatPackets, bcmFieldStatBytes};
  print bcm_field_entry_create(unit, fp_group_config.group, &v6_opt_dst_fp_entry_id);
  print v6_opt_dst_fp_entry_id;
  print bcm_field_qualify_clear(unit, v6_opt_dst_fp_entry_id);
  print bcm_field_qualify_IpType(unit, v6_opt_dst_fp_entry_id, bcmFieldIpTypeIpv6);
  print bcm_field_qualify_IpType(unit, v6_opt_dst_fp_entry_id, bcmFieldIpTypeIpv6OneExtHdr);
  print bcm_field_qualify_L4DstPort(unit, v6_opt_dst_fp_entry_id,v6_port_dst_opt, 0xFFFF);
  print bcm_field_qualify_DstPort(unit, v6_opt_dst_fp_entry_id, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
  print bcm_field_entry_prio_set(unit, v6_opt_dst_fp_entry_id, (1000-21));
  print bcm_field_stat_create(unit, fp_group_config.group,
                               2,stats , &v6_opt_dst_fp_stats_id);
  print bcm_field_entry_stat_attach(unit, v6_opt_dst_fp_entry_id, v6_opt_dst_fp_stats_id);
  print bcm_field_action_add(unit, v6_opt_dst_fp_entry_id, bcmFieldActionCosQCpuNew, local_tc, local_tc);
  print bcm_field_action_add(unit, v6_opt_dst_fp_entry_id, bcmFieldActionRpDrop, 0, 0);
  bcm_policer_config_t_init(&pol_cfg);
  pol_cfg.mode = bcmPolicerModeSrTcm;
  pol_cfg.ckbits_sec = 1000;
  pol_cfg.ckbits_burst = 200;
  pol_cfg.flags = BCM_POLICER_MODE_PACKETS;
  pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
  print bcm_policer_create(unit, &pol_cfg, &v6_opt_dst_policer_id);

  print bcm_field_entry_policer_attach(unit, v6_opt_dst_fp_entry_id,
                                 0, v6_opt_dst_policer_id);
  print bcm_field_entry_install(unit, v6_opt_dst_fp_entry_id);
}

/*
int v6_opt_src_fp_entry_id = 326 (0x146)
BCM.0>
BCM.0> fp show entry 326
fp show entry 326
EID 0x00000146: gid=0x2,
 slice=3, slice_idx=0x1, part=0, prio=0x3d4, flags=0x10602, Installed, Enabled, color_indep=1
 slice=4, slice_idx=0x1, part=1, prio=0x3d4, flags=0x10604, Installed, Enabled, color_indep=1

 L4SrcPort
    Part:1 Offset0: 20 Width0:  16
    DATA=0x00001618
    MASK=0x0000ffff
 DstPort
    Part:0 Offset0: 48 Width0:  16
    Part:0 Offset1: 152 Width1:   1
    DATA=0x00000000
    MASK=0x0001ffff
 IpType
    Part:0 Offset0: 104 Width0:   4
    Part:1 Offset1: 158 Width1:   1
    DATA=0x00000005
    MASK=0x0000001f
 IcmpTypeCode
    Part:1 Offset0: 20 Width0:  16
    DATA=0x00001618
    MASK=0x0000ffff
         action={act=RpDrop, param0=0(0), param1=0(0), param2=0(0), param3=0(0)}
         action={act=CosQCpuNew, param0=3(0x3), param1=3(0x3), param2=0(0), param3=0(0)}
         policer={pid=0x2d, level=0, peak_kbits_sec=0, peak_kbits_burst=0, commit_kbits_sec=0x3e8, commit_kbits_burst=0xc8, PacketBased=0x1, mode=0, entries=2, Clean}
         statistics={stat id 132  slice = 9 idx=14 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>

 */
void
bcm_ipv6_entry_create_src_opt (int unit)
{
  bcm_policer_config_t pol_cfg;
  bcm_field_stat_t stats[2] = {bcmFieldStatPackets, bcmFieldStatBytes};
  print bcm_field_entry_create(unit, fp_group_config.group, &v6_opt_src_fp_entry_id);
  print v6_opt_src_fp_entry_id;
  print bcm_field_qualify_clear(unit, v6_opt_src_fp_entry_id);
  print bcm_field_qualify_IpType(unit, v6_opt_src_fp_entry_id, bcmFieldIpTypeIpv6);
  print bcm_field_qualify_IpType(unit, v6_opt_src_fp_entry_id, bcmFieldIpTypeIpv6OneExtHdr);
  print bcm_field_qualify_L4SrcPort(unit, v6_opt_src_fp_entry_id,v6_port_src_opt, 0xFFFF);
  print bcm_field_qualify_DstPort(unit, v6_opt_src_fp_entry_id, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
  print bcm_field_entry_prio_set(unit, v6_opt_src_fp_entry_id,
                                 (1000-20));
  print bcm_field_stat_create(unit, fp_group_config.group,
                               2,stats , &v6_opt_src_fp_stats_id);
  print bcm_field_entry_stat_attach(unit, v6_opt_src_fp_entry_id, v6_opt_src_fp_stats_id);
  print bcm_field_action_add(unit, v6_opt_src_fp_entry_id, bcmFieldActionCosQCpuNew, local_tc, local_tc);
  print bcm_field_action_add(unit, v6_opt_src_fp_entry_id, bcmFieldActionRpDrop, 0, 0);
  bcm_policer_config_t_init(&pol_cfg);
  pol_cfg.mode = bcmPolicerModeSrTcm;
  pol_cfg.ckbits_sec = 1000;
  pol_cfg.ckbits_burst = 200;
  pol_cfg.flags = BCM_POLICER_MODE_PACKETS;
  pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
  print bcm_policer_create(unit, &pol_cfg, &v6_opt_src_policer_id);

  print bcm_field_entry_policer_attach(unit, v6_opt_src_fp_entry_id,
                                 0, v6_opt_src_policer_id);
  print bcm_field_entry_install(unit, v6_opt_src_fp_entry_id);
}

my_bcm_create_fp_group(0);
bcm_ipv4_entry_create_dst_any(0);
