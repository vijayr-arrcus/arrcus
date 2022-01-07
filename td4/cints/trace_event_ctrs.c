cint_reset();
uint64 val;
int rv = 0;
bcm_pkt_trace_event_t trace_type;

for (trace_type = 0; trace_type < bcmPktTraceEventCount; trace_type = trace_type + 1) {
  rv = bcm_stat_trace_event_counter_get(0, trace_type, &val);
  if (rv == BCM_E_NONE) {
    printf("Counter incremented for type %d  -- val: %d", trace_type, val);
  } else {
    printf(" rv %s", bcm_errmsg(rv));
  }
}
