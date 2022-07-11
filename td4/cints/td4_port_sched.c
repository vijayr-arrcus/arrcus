int port_sched = 0;
int ucast_group = 0;
int mcast_group = 0;
int g_index = 0;
int mod_index = 0;
bcm_error_t cpu_cosq_gport_traverse_cb (int unit,
                                        bcm_gport_t port,
                                        int numq,
                                        uint32 flags,
                                        bcm_gport_t gport,
                                        void *user_data)
{
  int local_port;
  local_port = BCM_GPORT_MODPORT_PORT_GET(port);


  if (local_port != 1) {
    return BCM_E_NONE;
  }


  printf("\n local port is %d flags %d mod index %d g index %d\n",
         local_port, flags, mod_index, g_index);
  mod_index = (g_index + 1) % 8;
  g_index++;

  if (flags & BCM_COSQ_GPORT_SCHEDULER) {
    printf("\n UC queue index %d and port %d \n ", numq, gport);
    port_sched++;
  }
  if (flags & BCM_COSQ_GPORT_UCAST_QUEUE_GROUP) {
    printf("\n UC queue group index %d and port %d \n ", numq, gport);
    ucast_group++;
  }
  if (flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) {
    printf("\n MC queue index %d and port %d \n", numq, gport);
    mcast_group++;
  }

  return BCM_E_NONE;
}

int rv = 0;
int unit = 0;
rv = bcm_cosq_gport_traverse(unit, cpu_cosq_gport_traverse_cb, NULL);
printf("\n port sched %d ucast sched %d mcast sched %d \n ",
       port_sched, ucast_group, mcast_group);
