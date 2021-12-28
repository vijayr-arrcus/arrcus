cint_reset();
bcm_field_group_config_t fp_group_config;

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
