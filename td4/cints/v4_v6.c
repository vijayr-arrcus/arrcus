bcm_field_group_config_t group_config;

void
bcm_create_fp_group (int unit)
{
	bcm_field_qualify_t qset;
	bcm_field_aset_t aset;
	bcm_field_group_config_t fp_group_config;

	bcm_field_group_config_t_init(&fp_group_config);
	BCM_FIELD_QSET_INIT(group_config.qset);

	memset(&qset, 0, sizeof(bcm_field_qset_t));
	memset(aset, 0, sizeof(bcm_field_aset_t));
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
	priotity = 17; // from fp show.
	fp_group_config.priority = priority;
	memcpy(&fp_group_config.qset, &qset, sizeof(bcm_field_qset_t));
	memcpy(&fp_group_config.aset, &aset, sizeof(bcm_field_aset_t));
	rv = bcm_field_group_config_create(unit, &fp_group_config);
	print fp_group_config.group;
}
bcm_create_fp_group(0);
