cint_reset();
bcm_port_t port;
port = 41;
uint32 kbits_sec, kbits_burst;
void
storm_control_disable (bcm_port_t port)
{
  print bcm_rate_bandwidth_set(0, port, BCM_RATE_BCAST,
                               BCM_RATE_DISABLE, BCM_RATE_DISABLE);
  print bcm_rate_bandwidth_set(0, port, BCM_RATE_MCAST,
                               BCM_RATE_DISABLE, BCM_RATE_DISABLE);
  print bcm_rate_bandwidth_set(0, port, BCM_RATE_DLF,
                               BCM_RATE_DISABLE, BCM_RATE_DISABLE);

}

storm_control_disable(port);

print bcm_rate_bandwidth_set(0, port, BCM_RATE_BCAST, 1800000, 5500);
print bcm_rate_bandwidth_get(0, port, BCM_RATE_BCAST, &kbits_sec, &kbits_burst);
print kbits_sec;
print kbits_burst;

print bcm_rate_bandwidth_set(0, port, BCM_RATE_MCAST, 1800000, 5500);
print bcm_rate_bandwidth_get(0, port, BCM_RATE_MCAST, &kbits_sec, &kbits_burst);
print kbits_sec;
print kbits_burst;

print bcm_rate_bandwidth_set(0, port, BCM_RATE_DLF, 1800000, 5500);
print bcm_rate_bandwidth_get(0, port, BCM_RATE_DLF, &kbits_sec, &kbits_burst);
print kbits_sec;
print kbits_burst;
