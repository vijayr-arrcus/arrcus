cint_reset();
bcm_gport_t dst_gport;
bcm_port_t  cpu_port = 0; 
int         modid = 0;
int unit = 0;
print bcm_port_gport_get(unit, cpu_port, &dst_gport);

print dst_gport;
print BCM_GPORT_LOCAL_CPU;
