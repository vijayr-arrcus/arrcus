bcm_error_t
cosq_control_get(int unit, bcm_port_t ing_port, bcm_cos_t int_pri)
{
    int             limit_dynamic;
    int             limit_shared;
    bcm_gport_t     gport;
    bcm_cos_queue_t cosq;

    BCM_IF_ERROR_RETURN(bcm_cosq_gport_mapping_get
                        (unit, ing_port, int_pri,
                         BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &gport, &cosq));

    BCM_IF_ERROR_RETURN(bcm_cosq_control_get
                        (unit, gport, 0,
                         bcmCosqControlEgressUCSharedDynamicEnable,
                         &limit_dynamic));

    if (limit_dynamic) {
        BCM_IF_ERROR_RETURN(bcm_cosq_control_get
                            (unit, gport, 0, bcmCosqControlDropLimitAlpha,
                             &limit_shared));
    } else {
        BCM_IF_ERROR_RETURN(bcm_cosq_control_get
                            (unit, gport, 0,
                             bcmCosqControlEgressPoolSharedLimitBytes,
                             &limit_shared));
    }
    printf("%s : %d\n",
           limit_dynamic ? "bcmCosqControlDropLimitAlpha" :
           "bcmCosqControlEgressPoolSharedLimitBytes", limit_shared);

    return BCM_E_NONE;
}

bcm_error_t
cosq_control_set(int unit, bcm_port_t ing_port, bcm_cos_t int_pri,
                 int limit_shared)
{
    int             limit_dynamic;
    bcm_gport_t     gport;
    bcm_cos_queue_t cosq;
    int             temp;

    BCM_IF_ERROR_RETURN(bcm_cosq_gport_mapping_get
                        (unit, ing_port, int_pri,
                         BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &gport, &cosq));

    BCM_IF_ERROR_RETURN(bcm_cosq_control_set
                        (unit, gport, 0,
                         bcmCosqControlEgressUCSharedDynamicEnable, 1));

    temp = (auto) limit_shared;
    printf
      ("Setting bcmCosqControlDropLimitAlpha for port %d/int pri %d to %d\n",
       ing_port, int_pri, temp);
    BCM_IF_ERROR_RETURN(bcm_cosq_control_set
                        (unit, gport, 0, bcmCosqControlDropLimitAlpha,
                         limit_shared));

    return BCM_E_NONE;
}

bcm_error_t
testcase(int unit)
{
    BCM_IF_ERROR_RETURN(cosq_control_get(unit, 1, 0));
    bshell(unit,
           "bsh -c 'lt TM_THD_UC_Q traverse -l PORT_ID==1 TM_UC_Q_ID==0'");
    BCM_IF_ERROR_RETURN(cosq_control_set
                        (unit, 1, 0, bcmCosqControlDropLimitAlpha_1));
    BCM_IF_ERROR_RETURN(cosq_control_get(unit, 1, 0));
    bshell(unit,
           "bsh -c 'lt TM_THD_UC_Q traverse -l PORT_ID==1 TM_UC_Q_ID==0'");
    return BCM_E_NONE;
}

bcm_error_t     rval;
if (BCM_FAILURE(rval = testcase(0))) {
    printf("Failed: %s\n", bcm_errmsg(rval));
}
