/* Enable per port sampler for egress port */
cint_reset();
void td4_egress_sampler (int unit, int port1, int rate)
{
  bcm_gport_t gport;
	print bcm_port_control_set(unit, port1, bcmPortControlSampleEgressEnable, TRUE);
	print bcm_port_control_set(unit, port1, bcmPortControlSampleEgressRate, 0x1);

	/* Set Egress sampler RNG seed value*/
	print bcm_switch_control_set(unit, bcmSwitchSampleEgressRandomSeed, 0xFFFF);


	/* Enable COPY_TO_CPU action */
	bcm_switch_trace_event_mon_t trace;
	bcm_switch_trace_event_mon_t_init(&trace);
	trace.trace_event = bcmPktTraceEventIngressMirrorSamplerEgrSampled;
	trace.actions = BCM_SWITCH_MON_ACTION_COPY_TO_CPU;
	print bcm_switch_trace_event_mon_set(unit, &trace);

	/* Configure mirror dest. with port3 as destination port */
	bcm_port_gport_get(unit, 0, &gport);
	bcm_mirror_destination_t mtp;
	bcm_mirror_destination_t_init(&mtp);
	mtp.gport=gport;
	/* Create Mirror destination with trace event as a source*/
	print bcm_mirror_destination_create(unit,&mtp);

	bcm_mirror_source_t source;
	bcm_mirror_source_t_init(&source);
	source.type = bcmMirrorSourceTypePktTraceEvent;
	source.trace_event = bcmPktTraceEventIngressMirrorSamplerEgrSampled;
	/* Add a mirror destination to a source sample event */
	print bcm_mirror_source_dest_add(unit, &source, mtp.mirror_dest_id);
}

[bcmPWN.0]Packet[15]:
Packet[15]:   Ethernet: dst<e8:c5:7a:8f:91:3f> src<b4:a9:fc:59:71:9c> Tagged Packet ProtID<0x8100> Ctrl<0x0f81> Internet Protocol (IP)
Packet[15]:   IP: V(4) src<1.1.1.1> dst<2.2.2.1> hl<5> service-type<0> tl<84> id<56544> frg-off<16384> ttl<64> > chk-sum<0x57c4>
Packet[15]:   ICMP: Type-8 (Echo [RFC792]) Code-0
Packet[15]:   Generic: Code <54784> Checksum <0x7463> hun <00 00 00 00>
[bcmPWN.0]
[bcmPWN.0]Packet[16]: data[0000]: {e8c57a8f913f} {b4a9fc59719c} 8100 0f81
[bcmPWN.0]Packet[16]: data[0010]: 0800 4500 0054 dce0 4000 4001 57c4 0101
[bcmPWN.0]Packet[16]: data[0020]: 0101 0202 0201 0800 228a 0008 36a6 63d6
[bcmPWN.0]Packet[16]: data[0030]: 7463 0000 0000 05bb 0200 0000 0000 1011
[bcmPWN.0]Packet[16]: data[0040]: 1213 1415 1617 1819 1a1b 1c1d 1e1f 2021
[bcmPWN.0]Packet[16]: data[0050]: 2223 2425 2627 2829 2a2b 2c2d 2e2f 3031
[bcmPWN.0]Packet[16]: data[0060]: 3233 3435 3637
[bcmPWN.0]

[RX metadata information]
        QUEUE_NUM=3
        PKT_LENGTH=0x6a
        SRC_PORT_NUM=0x28
        IP_ROUTED=1
        SWITCH=1
        MATCH_ID_LO=0x400a02
        MPB_FLEX_DATA_TYPE=5
        EGR_ZONE_REMAP_CTRL=1
        COPY_TO_CPU=1
[FLEX fields]
        EFFECTIVE_TTL_7_0=0x40
        ENTROPY_LABEL_LOW_15_0=0x4527
        EVENT_TRACE_VECTOR_31_0=0x2000000
        INGRESS_PP_PORT_7_0=0x28
        L2_OIF_10_0=0x78
        L3_IIF_13_0=5
        L3_OIF_1_13_0=3
        MPLS_LABEL_DECAP_COUNT_3_0=8
        NHOP_INDEX_1_15_0=6
        PARSER_VHLEN_0_15_0=0x8a
        PKT_MISC_CTRL_0_3_0=1
        SYSTEM_DESTINATION_15_0=0x78
        SYSTEM_OPCODE_3_0=1
        SYSTEM_SOURCE_15_0=0x28
        VFI_15_0=0xf81
        VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0=2
[RX reasons]
        MIRROR_SAMPLER_EGR_SAMPLED
[Decode match id]
        INGRESS_PKT_OUTER_L2_HDR_L2
        INGRESS_PKT_OUTER_L3_L4_HDR_ETHERTYPE
        INGRESS_PKT_OUTER_L3_L4_HDR_ICMP
        INGRESS_PKT_OUTER_L3_L4_HDR_IPV4

