cint_reset();

void
set_ingress_sflow (int unit, int port, int rate)
{
	int profile_id;
	bcm_mirror_sample_type_t type;
	bcm_mirror_sample_profile_t profile;
	bcm_mirror_source_t source;
  int e_port = 0; // CPU port

	bcm_gport_t gport;
	bcm_mirror_destination_t mtp;
	bcm_mirror_destination_t_init(&mtp);

	BCM_GPORT_MODPORT_SET(gport, unit, e_port);
	mtp.gport = gport;
  mtp.meta_data = 0xAECF;
  mtp.meta_data_type = bcmMirrorPsampFmt2HeaderUserMeta;

	printf("Create mirror destination\n");
	print bcm_mirror_destination_create(unit, &mtp);

	type = bcmMirrorSampleTypeIngress;
	printf("Create sample profile id\n");

  print bcm_mirror_sample_profile_create(unit, 0, type,  &profile_id);
  printf("Profile id %d \n", profile_id);

	bcm_mirror_sample_profile_t_init(&profile);
  profile.enable = TRUE;
  profile.rate = 0x1;
  profile.pool_count_enable = TRUE;
  profile.sample_count_enable = TRUE;
  profile.trace_enable = TRUE;
  profile.mirror_mode = bcmMirrorSampleMirrorModeBelowRate;

  printf("Create sample profile\n");

  print(bcm_mirror_sample_profile_set(unit, type, profile_id, &profile));

  bcm_mirror_source_t_init(&source);
  source.type = bcmMirrorSourceTypeSampleIngress;
  source.sample_profile_id = profile_id;

  printf("Source dest set \n");

  print bcm_mirror_source_dest_add(unit, &source, mtp.mirror_dest_id);

  print bcm_port_control_set(unit, port, bcmPortControlIngressSampleProfile, profile_id);
}

set_ingress_sflow(0, 40, 1);


[bcmPWN.0]
[bcmPWN.0]Packet[16]: data[0000]: {0180c200000e} {b4a9fc59719c} 8100 0f81
[bcmPWN.0]Packet[16]: data[0010]: 88cc 0207 04b4 a9fc 5971 9b04 0605 7377
[bcmPWN.0]Packet[16]: data[0020]: 7034 3906 0200 780a 096c 6f63 616c 686f
[bcmPWN.0]Packet[16]: data[0030]: 7374 0c1f 4172 7263 7573 204f 7065 7261
[bcmPWN.0]Packet[16]: data[0040]: 7469 6e67 2053 7973 7465 6d20 2841 7263
[bcmPWN.0]Packet[16]: data[0050]: 4f53 290e 0400 9c00 1010 0c05 010a 1ba8
[bcmPWN.0]Packet[16]: data[0060]: 7a02 0000 0006 0010 1811 0220 0105 5004
[bcmPWN.0]Packet[16]: data[0070]: 0800 01b6 a9fc fffe 5971 9b02 0000 0006
[bcmPWN.0]Packet[16]: data[0080]: 0008 0573 7770 3439 fe09 0012 0f03 0100
[bcmPWN.0]Packet[16]: data[0090]: 0000 00fe 0900 120f 0102 8000 004d 0000
[bcmPWN.0]

[RX metadata information]
        QUEUE_NUM=6
        PKT_LENGTH=0xa4
        SRC_PORT_NUM=0x28
        SWITCH=1
        MATCH_ID_LO=0x2202
        MPB_FLEX_DATA_TYPE=5
        EGR_ZONE_REMAP_CTRL=1
        MULTICAST=1
        COPY_TO_CPU=1
[FLEX fields]
        DROP_CODE_15_0=0x808
        ENTROPY_LABEL_LOW_15_0=0x2ba8
        EVENT_TRACE_VECTOR_31_0=0x1080040
        INGRESS_PP_PORT_7_0=0x28
        L3_IIF_13_0=5
        MPLS_LABEL_DECAP_COUNT_3_0=8
        PKT_MISC_CTRL_0_3_0=2
        SYSTEM_DESTINATION_15_0=0x807
        SYSTEM_OPCODE_3_0=3
        SYSTEM_SOURCE_15_0=0x28
        VFI_15_0=0xf81
        VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0=2
[RX reasons]
        PROTOCOL_PKT
        IFP_METER
        MIRROR_SAMPLER_SAMPLED
[Decode match id]
        INGRESS_PKT_OUTER_L2_HDR_L2
        INGRESS_PKT_OUTER_L3_L4_HDR_ETHERTYPE
        INGRESS_PKT_OUTER_L3_L4_HDR_UNKNOWN_L3

[bcmPWN.0]Packet[16]:
Packet[16]:   Ethernet: dst<01:80:c2:00:00:0e> src<b4:a9:fc:59:71:9c> Tagged Packet ProtID<0x8100> Ctrl<0x0f81> * Unknown/Experimental format * 01 80 c2 00 00 0e b4 a9 fc 59 71 9c 81 00 0f 81 88 cc 02 07 04 b4 a9 fc 59 71 9b 04 06 05 73 77 70 34 39 06 02 00 78 0a 09 6c 6f 63 61 6c 68 6f 73 74 0c 1f 41 72 72 63 75 73 20 4f 70 65 72 61
