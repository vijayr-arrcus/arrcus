int
get_resource(int unit, int port)
{
  bcm_port_resource_t port_info;
  bcm_port_resource_t_init(&port_info);
  print  bcm_port_resource_get(unit, port, &port_info);
  print port_info;
}

int 
set_link_training (int unit, int port, int enable) 
{
  bcm_port_resource_t port_info;
  bcm_port_resource_t_init(&port_info);
  bcm_port_enable_set(unit, port, 0);
  print  bcm_port_resource_get(unit, port, &port_info);
  print port_info;
  port_info.link_training = enable;
  print bcm_port_resource_set(unit, port, &port_info);
  bcm_port_resource_t_init(&port_info);
  print  bcm_port_resource_get(unit, port, &port_info);
  print port_info;
  bcm_port_enable_set(unit, port, 1);
  return 1;
}

print set_link_training(0, 102, 1);
print set_link_training(0, 104, 1);
print set_link_training(0, 105, 1);
print set_link_training(0, 106, 1);





