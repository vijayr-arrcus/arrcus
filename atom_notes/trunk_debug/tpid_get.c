extern int bcm_port_tpid_class_get(
    int unit,
    bcm_port_tpid_class_t *tpid_class);

int rv = 0;
int unit = 0;
int port = 7;
bcm_port_tpid_class_t tpid_class;
bcm_port_tpid_class_t_init(&tpid_class);
tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
tpid_class.port = port;
tpid_class.tpid1 = 0x8100;
tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
rv = bcm_port_tpid_class_get(unit, &tpid_class);


cint_reset();
int rv = 0;
int unit = 0;
int port = 7;
bcm_port_tpid_class_t tpid_class;
bcm_port_tpid_class_t_init(&tpid_class);
tpid_class.port = port;
tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY|BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
rv = bcm_port_tpid_class_get(unit, &tpid_class);
