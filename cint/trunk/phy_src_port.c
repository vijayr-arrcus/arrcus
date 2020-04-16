int port_set_sysport(int unit, int local_port, int sys_port)
{
    bcm_gport_t gport;
    bcm_gport_t modport;
    bcm_gport_t modport_gport;
    bcm_gport_t sys_port_final;
    int core;

    int my_modid;
    int modid;
    int tm_port;

    int rv;

    uint32_t flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;

    bcm_port_class_set(unit,local_port, bcmPortClassId, 100);

    rv = bcm_port_get(unit, local_port, &flags, &interface_info, &mapping_info);

    rv = bcm_stk_my_modid_get(unit, &my_modid);

    core = mapping_info.core;
    modid = my_modid+core;
    tm_port = mapping_info.tm_port;
    tm_port = local_port;



    // Reset current mapping
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, local_port);
    rv = bcm_stk_sysport_gport_set(unit, gport, -1);

    // Set the new mapping.
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port_final, sys_port);
    BCM_GPORT_MODPORT_SET(modport_gport, modid, tm_port);
    rv = bcm_stk_sysport_gport_set(unit,
                                   sys_port_final,
                                   modport_gport);
    bcm_port_class_set(unit,sys_port_final, bcmPortClassId, 101);
/*
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, local_port);
    rv = bcm_stk_sysport_gport_set(unit,
                                   gport,
                                   modport_gport);
                                   */
}
