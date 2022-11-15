/* Enable per port sampler for egress port */
cint_reset();
void td4_egress_sampler (int unit, int port1, int rate)
{
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
