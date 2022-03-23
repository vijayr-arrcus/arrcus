cint_reset();
int unit = 0;
bcm_port_t port = 24;
bcm_port_resource_t port_info;
bcm_port_resource_t_init(&port_info);
print bcm_port_resource_get(unit, port, &port_info);
print port_info;

cint_reset();
int unit = 0;
bcm_port_t port = 62;
bcm_port_resource_t port_info;
bcm_port_resource_t_init(&port_info);
print bcm_port_resource_get(unit, port, &port_info);
print port_info;
