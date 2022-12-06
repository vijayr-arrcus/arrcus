cint_reset();

bcm_mirror_sample_profile_t profile;
bcm_mirror_destination_t mtp;
int profile_id;
void
set_ingress_sflow (int unit, int port, int rate)
{
	bcm_mirror_sample_type_t type;
	bcm_mirror_source_t source;
  int e_port = 0; // CPU port

	bcm_gport_t gport;
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

set_ingress_sflow(0, 42, 1);

01:21:31.737192 b4:a9:fc:59:71:9c (oui Unknown) > e8:c5:7a:b1:27:24 (oui Unknown), ethertype 802.1Q (0x8100), length 206:
        0x0000:  b880 1004 0000 0066 0000 4800 0000 0c2b  .......f..H....+
        0x0010:  6a00 0000 0000 020a 4000 0500 1201 0000  j.......@.......
        0x0020:  1081 0100 0200 0000 0400 2a00 4000 2b00  ..........*.@.+.
        0x0030:  7117 0000 0000 0000 0200 2a00 0000 7f0f  q.........*.....
        0x0040:  2b00 8a00 0000 0000 0000 0000 0000 0000  +...............
        0x0050:  0001 0000 0000 e8c5 7ab1 2724 b4a9 fc59  ........z.'$...Y
        0x0060:  719c 8100 0f7f 0800 4500 0054 f8b1 4000  q.......E..T..@.
        0x0070:  4001 e3c6 1717 1701 1818 1801 0800 3566  @.............5f
        0x0080:  0021 0002 8180 8763 0000 0000 f0bf 0a00  .!.....c........
        0x0090:  0000 0000 1011 1213 1415 1617 1819 1a1b  ................
				0x00a0:  1c1d 1e1f 2021 2223 2425 2627 2829 2a2b  .....!"#$%&'()*+
        0x00b0:  2c2d 2e2f 3031 3233 3435 3637            ,-./01234567

with xxx

01:23:05.561649 b4:a9:fc:59:71:9c (oui Unknown) > e8:c5:7a:b1:27:24 (oui Unknown), ethertype 802.1Q (0x8100), length 206:
        0x0000:  e8c5 7ab1 2724 b4a9 fc59 719c 8100 0001  ..z.'$...Yq.....
        0x0010:  de08 b880 1004 0000 0066 0000 4800 0000  .........f..H...


        0x0020:  0c2b 6a00 0000 0000 020a 4000 0500 1201  .+j.......@.....
        0x0030:  0000 1081 0100 0200 0000 0400 2a00 4000  ............*.@.
        0x0040:  2b00 7117 0000 0000 0000 0200 2a00 0000  +.q.........*...
        0x0050:  7f0f 2b00 8a00 0000 0000 0000 0000 0000  ..+.............
        0x0060:  0000 0001 0000 0000
																		e8c5 7ab1 2724 b4a9  ..........z.'$..
				0x0070:  fc59 719c 8100 0f7f 0800 4500 0054 5d47  .Yq.......E..T]G
        0x0080:  4000 4001 7f31 1717 1701 1818 1801 0800  @.@..1..........
        0x0090:  5914 0022 0002 df80 8763 0000 0000 7110  Y..".....c....q.
        0x00a0:  0800 0000 0000 1011 1213 1415 1617 1819  ................
        0x00b0:  1a1b 1c1d 1e1f 2021 2223 2425 2627 2829  .......!"#$%&'()
        0x00c0:  2a2b 2c2d 2e2f 3031 3233 3435 3637       *+,-./01234567

17:03:48.028473 IP 23.23.23.1 > 24.24.24.1: ICMP echo request, id 40, seq 1, length 64
        0x0000:  e8c5 7ab1 2724 b4a9 fc59 719c 0800 4500
        0x0010:  0054 c3f3 4000 4001 1885 1717 1701 1818
        0x0020:  1801 0800 a5a4 0028 0001 f48c 8763 0000
        0x0030:  0000 176f 0000 0000 0000 1011 1213 1415
        0x0040:  1617 1819 1a1b 1c1d 1e1f 2021 2223 2425
        0x0050:  2627 2829 2a2b 2c2d 2e2f 3031 3233 3435
        0x0060:  3637

[bcmPWN.0]Packet[3]:
Packet[3]:   Ethernet: dst<e8:c5:7a:b1:27:24> src<b4:a9:fc:59:71:9c> Tagged Packet ProtID<0x8100> Ctrl<0x0f7f> Internet Protocol (IP)
Packet[3]:   IP: V(4) src<23.23.23.1> dst<24.24.24.1> hl<5> service-type<0> tl<84> id<32958> frg-off<16384> ttl<64> > chk-sum<0x5bba>
Packet[3]:   ICMP: Type-8 (Echo [RFC792]) Code-0
Packet[3]:   Generic: Code <33280> Checksum <0x8763> hun <00 00 00 00>
[bcmPWN.0]
[bcmPWN.0]Packet[4]: data[0000]: {e8c57ab12724} {b4a9fc59719c} 8100 0f7f
[bcmPWN.0]Packet[4]: data[0010]: 0800 4500 0054 80be 4000 4001 5bba 1717
[bcmPWN.0]Packet[4]: data[0020]: 1701 1818 1801 0800 9d5d 0023 0002 5682
[bcmPWN.0]Packet[4]: data[0030]: 8763 0000 0000 b9c4 0400 0000 0000 1011
[bcmPWN.0]Packet[4]: data[0040]: 1213 1415 1617 1819 1a1b 1c1d 1e1f 2021
[bcmPWN.0]Packet[4]: data[0050]: 2223 2425 2627 2829 2a2b 2c2d 2e2f 3031
[bcmPWN.0]Packet[4]: data[0060]: 3233 3435 3637
[bcmPWN.0]

[RX metadata information]
        QUEUE_NUM=3
        PKT_LENGTH=0x6a
        SRC_PORT_NUM=0x2b
        MATCH_ID_LO=0x400a02
        MPB_FLEX_DATA_TYPE=5
        EGR_ZONE_REMAP_CTRL=1
        COPY_TO_CPU=1
[FLEX fields]
        EFFECTIVE_TTL_7_0=0x40
        ENTROPY_LABEL_LOW_15_0=0x1771
        EVENT_TRACE_VECTOR_31_0=0x1000000
        INGRESS_PP_PORT_7_0=0x2b
        L2_OIF_10_0=0x2a
        L3_IIF_13_0=2
        L3_OIF_1_13_0=1
        MPLS_LABEL_DECAP_COUNT_3_0=8
        NHOP_INDEX_1_15_0=4
        PARSER_VHLEN_0_15_0=0x8a
        PKT_MISC_CTRL_0_3_0=1
        SYSTEM_DESTINATION_15_0=0x2a
        SYSTEM_OPCODE_3_0=1
        SYSTEM_SOURCE_15_0=0x2b
        VFI_15_0=0xf7f
        VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0=2
[RX reasons]
        MIRROR_SAMPLER_SAMPLED
[Decode match id]
        INGRESS_PKT_OUTER_L2_HDR_L2
        INGRESS_PKT_OUTER_L3_L4_HDR_ETHERTYPE
        INGRESS_PKT_OUTER_L3_L4_HDR_ICMP
        INGRESS_PKT_OUTER_L3_L4_HDR_IPV4


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

04:59:42.624379 e8:c5:7a:b1:27:24 (oui Unknown) > Broadcast, ethertype 802.1Q (0x8100), length 168:
        0x0000:  ffff ffff ffff e8c5 7ab1 2724 8100 0001
        0x0010:  de08 b780 1004 0000 0040 c641 4800 0000
        0x0020:  0c3d 4400 0000 0000 2404 6000 0d00 5001
        0x0030:  0003 0800 0000 0200 0000 0000 6400 0000
        0x0040:  0000 0000 0000 0000 0000 0000 0000 3d00
        0x0050:  0208 0000 0000 3d00 0000 0000 0000 a82b
        0x0060:  0000 0000 0000 0002
																		ffff ffff ffff e8c5
        0x0070:  7ab1 2724 8100 0064 0806 0001 0800 0604
        0x0080:  0001 e8c5 7ab1 2724 0101 0102 0000 0000
        0x0090:  0000 0101 0103 0000 0000 0000 0000 0000
        0x00a0:  0000 0000 0000 0000

