cint_reset();
bcm_field_group_config_t fp_group_config;

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

  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTtl);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp6HopLimit);
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
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2StationMove);

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
bcm_field_entry_t v4_any_dst_fp_entry_id;
bcm_field_entry_t v4_any_src_fp_entry_id;
bcm_field_entry_t v4_opt_dst_fp_entry_id;
bcm_field_entry_t v4_opt_src_fp_entry_id;

int v4_any_dst_fp_stats_id;
int v4_any_src_fp_stats_id;
int v4_opt_dst_fp_stats_id;
int v4_opt_src_fp_stats_id;

bcm_policer_t v4_any_dst_policer_id;
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


 */
void
bcm_ipv4_entry_create_dst_any (int unit)
{
  bcm_policer_config_t pol_cfg;
  bcm_field_stat_t stats[2] = {bcmFieldStatPackets, bcmFieldStatBytes};
  print bcm_field_entry_create(unit, fp_group_config.group, &v4_any_dst_fp_entry_id);
  print v4_any_dst_fp_entry_id;
  print bcm_field_qualify_clear(unit, v4_any_dst_fp_entry_id);
  print bcm_field_qualify_IpType(unit, v4_any_dst_fp_entry_id, bcmFieldIpTypeIpv4Any);
  print bcm_field_qualify_L4DstPort(unit, v4_any_dst_fp_entry_id,v4_port_dst, 0xFFFF);
  print bcm_field_qualify_DstPort(unit, v4_any_dst_fp_entry_id, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
  print bcm_field_entry_prio_set(unit, v4_any_dst_fp_entry_id, (1000-23));

  print bcm_field_stat_create(unit, fp_group_config.group,
                               2,stats , &v4_any_dst_fp_stats_id);
  print bcm_field_entry_stat_attach(unit, v4_any_dst_fp_entry_id, v4_any_dst_fp_stats_id);
  print bcm_field_action_add(unit, v4_any_dst_fp_entry_id, bcmFieldActionCosQCpuNew, local_tc, local_tc++);
  print bcm_field_action_add(unit, v4_any_dst_fp_entry_id, bcmFieldActionRpDrop, 0, 0);
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
