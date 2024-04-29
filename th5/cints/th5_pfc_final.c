/*
 * BCMLT.0> lt traverse -l TM_DEVICE_INFO
 * Traverse table [TM_DEVICE_INFO]:
 *     NUM_REPL_Q=7
 *     NUM_REPL_CELLS=0x2c00(11264)
 *     NUM_BUFFER_POOL=2
 *     NUM_PIPE=0x20(32)
 *     NUM_CPU_Q=0x30(48)
 *     NUM_Q=0xc(12)
 *     NUM_SERVICE_POOL=4
 *     NUM_PORT_PRI_GRP=8
 *     NUM_CELLS=0x53458(341080)
 *     CELL_SIZE=0xfe(254)
 *     DEFAULT_MTU=0x600(1536)
 *     JUMBO_PKT_SIZE=0x2400(9216)
 *     PKT_HDR_SIZE=0x40(64)
 *     MAX_PKT_SIZE=0x24c8(9416)
 *     1 entry traversed.
 * BCMLT.0>
 */


int g_pg_to_pools_map_profile_id = 0;
int g_rx_pfc_to_voqs_profile_id = 0;
int g_ing_pri_to_pg_map_profile_id = 0;


bcm_error_t
bcmsdk_pfc_port_pg_dump (int unit,
												 bcm_port_t port,
												 int pg)
{
  int value;
  bcm_error_t rv;
  bcm_port_priority_group_config_t cfg;

  rv = bcm_cosq_port_priority_group_property_get(unit, port, pg,
                                                 bcmCosqPriorityGroupLossless,
                                                 &value);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to get lossless property - %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

  printf("bcmCosqPriorityGroupLossless: %d \n", value);

  rv = bcm_cosq_port_priority_group_property_get(unit, port, pg,
                                                 bcmCosqPauseEnable, &value);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to get pauseenable property - %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

  printf("bcmCosqPauseEnable: %d\n", value);

  bcm_port_priority_group_config_t_init(&cfg);
  rv = bcm_port_priority_group_config_get(unit, port, pg, &cfg);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to get group config - %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

  printf("PFC config on port:%d, pg:%d", port, pg);
  printf(" | cfg.shared_pool_xoff_enable:%d\n", cfg.shared_pool_xoff_enable);
  printf(" | cfg.shared_pool_discard_enable:%d\n", cfg.shared_pool_discard_enable);
  printf(" | cfg.pfc_transmit_enable:%d\n", cfg.pfc_transmit_enable);
  printf(" | cfg.priority_enable_vector_mask:%x\n", cfg.priority_enable_vector_mask);
  return rv;
}

bcm_error_t
bcmsdk_mmu_port_pg_control_set (int unit, int port_gport, int pg,
                                bcm_cosq_control_t type, int value)
{
  int rv;
  bcm_cosq_dynamic_setting_type_t dynamic;

  rv = bcm_cosq_control_dynamic_get(unit,
                                    type,
                                    &dynamic);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to get dynamic for type %d - %s\n",
           __FUNCTION__, port, pg, type, bcm_errmsg(rv));
    return rv;
  }

  switch (dynamic) {
    case bcmCosqDynamicTypeFixed:
      /*
       * Mapping cannot be changed during run time as disable MMU
       * traffic routine cannot guarantee flush based on Source port
       */
      rv = BCM_E_INTERNAL;
      break;
    case bcmCosqDynamicTypeConditional:
      /*
       * Mapping can be changed as long as the port has no traffic in MMU
       */
      rv = bcm_port_control_set(unit, port,
                                bcmPortControlMmuTrafficEnable, 0);
      if (rv != BCM_E_NONE) {
        printf("%s: port-%d:Failed to disable mmu traffic in port pg control %s\n",
               __FUNCTION__, port, bcm_errmsg(rv));
        return rv;
      }
      rv = bcm_cosq_control_set(unit, port_gport, pg,
                                type, value);
      if (rv != BCM_E_NONE) {
        printf("%s: port-%d:Failed in port pg control type %d id %d-%s\n",
               __FUNCTION__,
               port, profile_type, profile_id, bcm_errmsg(rv));
        return rv;
      }
      rv = bcm_port_control_set(unit, port,
                                bcmPortControlMmuTrafficEnable, 1);
      if (rv != BCM_E_NONE) {
        printf("%s: port-%d: in pg port control failed to enable mmu traffic-%s\n",
               __FUNCTION__,
               port, bcm_errmsg(rv));
        return rv;
      }
      break;
    case bcmCosqDynamicTypeFlexible:
      rv = bcm_cosq_control_set(unit, port_gport, pg,
                                type, value);
      if (rv != BCM_E_NONE) {
        printf("%s: port-%d: in pg port control failed to set profile type %d id %d-%s\n",
               __FUNCTION__,
               port, profile_type, profile_id, bcm_errmsg(rv));
        return rv;
      }
      break;
  }
  return BCM_E_NONE;
}

bcm_error_t
bcmsdk_setup_mmu_port_pg_attrs (int unit, int port, int pg,
                                int pg_min, int pg_headroom, int pg_shared)
{
  bcm_error_t rv;
  bcm_gport_t port_gport;

  rv = bcm_port_gport_get(unit, port, &port_gport);
  if (rv != BCM_E_NONE) {
    printf("%s: port - %d:failed to get gport- %s",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }

  /* Ingress Port PG Min limit setting.
     bcmCosqControlIngressPortPGMinLimitBytes
  */
  rv = bcmsdk_mmu_port_pg_control_set(unit, port_gport, pg,
                                      bcmCosqControlIngressPortPGMinLimitBytes,
                                      pg_min);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d: pg %d: failed to set pgmin - %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }
  /* Ingress Port PG Headroom limit setting
     bcmCosqControlIngressPortPGHeadroomLimitBytes
   */
  rv = bcmsdk_mmu_port_pg_control_set(unit, port_gport, pg,
                                      bcmCosqControlIngressPortPGHeadroomLimitBytes,
                                      pg_headroom);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d: pg %d: failed to set PGhdrm - %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

  /* Ingress Port PG Shared limit setting.
    bcmCosqControlIngressPortPGSharedLimitBytes
  */
  rv = bcmsdk_mmu_port_pg_control_set(unit, port_gport, pg,
                                      bcmCosqControlIngressPortPGSharedLimitBytes,
                                      pg_shared);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d: pg %d: failed to set PGShared - %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t
bcmsdk_setup_mmu_hdrm_pool_buffers (int unit)
{
  bcm_cosq_pkt_size_dist_t pkt_dist_array[1]; // max 3
  int pkt_size_array_count = 1;
  int hdrm_pool_id = 0;
  int max_num_lossless_classes = 8;

  pkt_dist_array[0].pkt_size = 4096; // 4k bytes.
  pkt_dist_array[0].dist_perc = 100; // 100%
/**
 * BCMLT.0> lt list TM_ING_THD_HEADROOM_POOL
 * TM_ING_THD_HEADROOM_POOL
 * 5 fields (2 key-type fields):
 * OPERATIONAL_STATE
 * LIMIT_CELLS_OPER
 * LIMIT_CELLS
 * BUFFER_POOL (key)
 * TM_HEADROOM_POOL_ID (key)
 * BCMLT.0> lt traverse -l TM_ING_THD_HEADROOM_POOL
 * Traverse table [TM_ING_THD_HEADROOM_POOL]:
 * OPERATIONAL_STATE=VALID
 * LIMIT_CELLS_OPER=0xa(10)
 * LIMIT_CELLS=0xa(10)
 * BUFFER_POOL=0
 * TM_HEADROOM_POOL_ID=0
 *
 * OPERATIONAL_STATE=VALID
 * LIMIT_CELLS_OPER=0
 * LIMIT_CELLS=0
 * BUFFER_POOL=0
 * TM_HEADROOM_POOL_ID=1
 *
 * OPERATIONAL_STATE=VALID
 * LIMIT_CELLS_OPER=0
 * LIMIT_CELLS=0
 * BUFFER_POOL=0
 * TM_HEADROOM_POOL_ID=2
 *
 * OPERATIONAL_STATE=VALID
 * LIMIT_CELLS_OPER=0
 * LIMIT_CELLS=0
 * BUFFER_POOL=0
 * TM_HEADROOM_POOL_ID=3
 *
 * OPERATIONAL_STATE=VALID
 * LIMIT_CELLS_OPER=0
 * LIMIT_CELLS=0
 * BUFFER_POOL=1
 * TM_HEADROOM_POOL_ID=0
 *
 * OPERATIONAL_STATE=VALID
 * LIMIT_CELLS_OPER=0
 * LIMIT_CELLS=0
 * BUFFER_POOL=1
 * TM_HEADROOM_POOL_ID=1
 *
 * OPERATIONAL_STATE=VALID
 * LIMIT_CELLS_OPER=0
 * LIMIT_CELLS=0
 * BUFFER_POOL=1
 * TM_HEADROOM_POOL_ID=2
 *
 * OPERATIONAL_STATE=VALID
 * LIMIT_CELLS_OPER=0
 * LIMIT_CELLS=0
 * BUFFER_POOL=1
 * TM_HEADROOM_POOL_ID=3
 * 8 entries traversed.
 * BCMLT.0>
 */
  rv = bcm_cosq_hdrm_pool_limit_set(unit, hdrm_pool_id,
                                    max_num_lossless_classes,
                                    pkt_size_array_count,
                                    &pkt_dist_array);
  if (rv != BCM_E_NONE) {
    printf("%s:Failed to reserve head room pool buffers %s\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t
bcmsdk_setup_mmu_thdi_attrs (int unit)
{
  bcm_error_t rv;

  rv = bcmsdk_setup_mmu_hdrm_pool_buffers(unit);
  if (rv != BCM_E_NONE) {
    printf("%s: failed to set mmu headroom buffers %s\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t
bcmsdk_setup_pg_to_tx_pfc_pri (int unit, int profile_id)
{
  // int pg_array[8] = {0, 7, 7, 7, 0, 0, 0, 0}; // PG array, indexed by PFC priority.
  // if PG7 is congested 1,2,3 priorities are set.
  // But does it mean that if PG0 is constrained priority 0, 4, 5, 6 and 7 are set ?
  int pg_array[8] = {0, 1, 2, 3, 4, 5, 6, 7}; // PG array, indexed by PFC priority.
  /* Set PFC-tx priority to PG mapping 1:1 mapping*/


/*
BCM.0>
BCM.0> bsh -c 'lt list @TM_PFC_PRI_TO_PRI_GRP_MAP'
bsh -c 'lt list @TM_PFC_PRI_TO_PRI_GRP_MAP'
TM_PFC_PRI_TO_PRI_GRP_MAP
  3 fields (2 key-type fields):
    TM_PRI_GRP_ID
    PFC_PRI (key)
    TM_PFC_PRI_TO_PRI_GRP_MAP_ID (key)
BCM.0>
*/

  rv = bcm_cosq_priority_group_pfc_priority_mapping_profile_set(unit, profile_id,
                                                                8, pg_array);
  if (rv != BCM_E_NONE) {
    printf("%s: profile set failed:%s.\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

bcm_error_t
bcmsdk_setup_pg_to_sp_hp_mapping (int unit, int profile_id)
{
	bcm_cosq_priority_group_mapping_profile_type_t prof_type;
	int pg_count;
	int sp_array[8]= {0, 0, 0, 0, 0, 0, 0, 0};
	int hp_array[8]= {0, 0, 0, 0, 0, 0, 0, 0};

	pg_count = 8;

  /**
   * Table Name: TM_PRI_GRP_POOL_MAP
   * Field Name: TM_ING_SERVICE_POOL_ID.
BCM.0> bsh -c 'lt list @TM_PRI_GRP_POOL_MAP'
bsh -c 'lt list @TM_PRI_GRP_POOL_MAP'
TM_PRI_GRP_POOL_MAP
  4 fields (2 key-type fields):
    TM_HEADROOM_POOL_ID
    TM_ING_SERVICE_POOL_ID
    TM_PRI_GRP_ID (key)
    TM_PRI_GRP_POOL_MAP_ID (key)
BCM.0>
    */
	// setup PG to SP mapping.
	prof_type = bcmCosqPriorityGroupServicePoolMapping;
	rv = bcm_cosq_priority_group_mapping_profile_set(unit, profile_id,
																									 prof_type,
																									 pg_count, sp_array);
	if (rv != BCM_E_NONE) {
		printf("%s: SP map failed:%s.\n",
           __FUNCTION__, bcm_errmsg(rv));
		return rv;
	}

  /**
   * Table Name: TM_PRI_GRP_POOL_MAP
   * Field Name: TM_HEADROOM_POOL_ID
BCM.0> bsh -c 'lt list @TM_PRI_GRP_POOL_MAP'
bsh -c 'lt list @TM_PRI_GRP_POOL_MAP'
TM_PRI_GRP_POOL_MAP
  4 fields (2 key-type fields):
    TM_HEADROOM_POOL_ID
    TM_ING_SERVICE_POOL_ID
    TM_PRI_GRP_ID (key)
    TM_PRI_GRP_POOL_MAP_ID (key)
BCM.0>
*/
	// setup PG to HP mapping.
	prof_type = bcmCosqPriorityGroupHeadroomPoolMapping;
	rv = bcm_cosq_priority_group_mapping_profile_set(unit, profile_id,
																									 prof_type,
																									 pg_count, hp_array);
	if (rv != BCM_E_NONE) {
		printf("%s: HP map failed:%s.\n",
           __FUNCTION__, bcm_errmsg(rv));
		return rv;
	}

  rv = bcmsdk_setup_pg_to_tx_pfc_pri(unit, profile_id);
  if (rv != BCM_E_NONE) {
    printf("%s: PG to tx prio failed  %s\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }

  return BCM_E_NONE;
}

bcm_error_t
bcmsdk_setup_ing_local_tc_to_pg (int unit, int profile_id)
{
  int array_count = 16;
  bcm_cosq_priority_group_mapping_profile_type_t prof_type;
  int set_pg_uc[16] = {0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7};
  int set_pg_mc[16] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};



  /*
   * TableName: TM_ING_UC_ING_PRI_MAP
   * fieldName: TM_PRI_GRP_ID
BCM.0> bsh -c 'lt list @TM_ING_UC_ING_PRI_MAP'
bsh -c 'lt list @TM_ING_UC_ING_PRI_MAP'
TM_ING_UC_ING_PRI_MAP
  3 fields (2 key-type fields):
    TM_PRI_GRP_ID
    ING_PRI (key)
    TM_ING_UC_ING_PRI_MAP_ID (key)
BCM.0>
   */
  prof_type = bcmCosqInputPriPriorityGroupUcMapping;
  rv = bcm_cosq_priority_group_mapping_profile_set(unit,
                                                   profile_id,
                                                   prof_type,
                                                   array_count, set_pg_uc);
  if (rv != BCM_E_NONE) {
    printf("%s: UC prio to pg map falied:%s.\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }

  /*
   * TableName: TM_ING_NONUC_ING_PRI_MAP
   * fieldName: TM_PRI_GRP_ID
BCM.0> bsh -c 'lt list @TM_ING_NONUC_ING_PRI_MAP'
bsh -c 'lt list @TM_ING_NONUC_ING_PRI_MAP'
TM_ING_NONUC_ING_PRI_MAP
  3 fields (2 key-type fields):
    TM_PRI_GRP_ID
    ING_PRI (key)
    TM_ING_NONUC_ING_PRI_MAP_ID (key)
BCM.0>
*/
  prof_type = bcmCosqInputPriPriorityGroupMcMapping;
  rv = bcm_cosq_priority_group_mapping_profile_set(unit, profile_id,
                                                   prof_type,
                                                   array_count, set_pg_mc);
  if (rv != BCM_E_NONE) {
    printf("%s: MC prio to pg map falied:%s.\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t
bcmsdk_setup_lossy_pgs (int unit, int port)
{
  // Mark all PGs as lossy
  int lossless_pg_array[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  int class_count = 8;

  for (int i = 0; i < class_count; i++) {
   /*
     * tableName: TM_ING_PORT_PRI_GRP
     * fieldName: LOSSLESS.
     BCM.0> bsh -c 'lt list @TM_ING_PORT_PRI_GRP'
     bsh -c 'lt list @TM_ING_PORT_PRI_GRP'
     TM_ING_PORT_PRI_GRP
     7 fields (2 key-type fields):
     OPERATIONAL_STATE
     IGNORE_PFC_XOFF
     PFC
     LOSSLESS
     ING_MIN_MODE
     TM_PRI_GRP_ID (key)
     PORT_ID (key)
     BCM.0>
     */
    rv = bcm_cosq_port_priority_group_property_set(unit,
                                                   port,
                                                   lossless_pg_array[i],
                                                   bcmCosqPriorityGroupLossless,
                                                   false); // false
    if (rv != BCM_E_NONE) {
      printf("%s: port-%d class %d failed to mark lossy - %s\n",
             __FUNCTION__, port, i, bcm_errmsg(rv));
      return rv;
    }
    rv = bcm_cosq_port_priority_group_property_set(unit,
                                                   port,
                                                   lossless_pg_array[i],
                                                   bcmCosqPauseEnable,
                                                   false); // false
 /*
     * tableName: TM_ING_PORT
     * fieldName: PAUSE
     BCM.0> bsh -c 'lt list @TM_ING_PORT'
     bsh -c 'lt list @TM_ING_PORT'
     TM_ING_PORT
     5 fields (1 key-type field):
     OPERATIONAL_STATE
     PAUSE
     PRI_GRP_MAP_ID
     ING_PRI_MAP_ID
     PORT_ID (key)
     BCM.0>
     */
    if (rv != BCM_E_NONE) {
      printf("%s: port-%d class %d failed to enable pause frames - %s \n",
             __FUNCTION__,
             port, i, bcm_errmsg(rv));
      return rv;
    }
  }
  return BCM_E_NONE;
}

bcm_error_t
bcmsk_setup_rx_pfc_to_voqs (int unit,
                            int profile_id)
{
	int pfc_class;
	int rv = 0;
	int profile_id = 0;

  /* Array index implicitly indicate PFC priority */
	bcm_cosq_pfc_class_map_config_t config_array[8];

  for (pfc_class = 0; pfc_class < 8; pfc_class++) {
    bcm_cosq_pfc_class_map_config_t_init(&config_array[pfc_class]);
    config_array[pfc_class].pfc_enable = 1;
    config_array[pfc_class].pfc_optimized = 0;
    // list of cosq voqs to pause when priorit 0 is received.
    //config_array[0].cos_list_bmp = (1U <<1) | (1U <<2) | (1U <<3);
    config_array[pfc_class] = pfc_class; // 1:1 traffic class to queue;
    config_array[pfc_class].pfc_flags = BCM_COSQ_PFC_MAP_TO_QUEUES;
  }

  /*
   * BCM.0> bsh -c 'lt list @TM_PFC_PRI_PROFILE'
   * bsh -c 'lt list @TM_PFC_PRI_PROFILE'
   * TM_PFC_PRI_PROFILE
   *   5 fields (2 key-type fields):
   *       PFC_MAP_TO_QUEUES
   *       COS_LIST
   *       PFC
   *       PFC_PRI (key)
   *       TM_PFC_PRI_PROFILE_ID (key)
   *BCM.0>
   */

  rv = bcm_cosq_pfc_class_config_profile_set(unit, profile_id,
                                             8, &config_array);
  if (rv != BCM_E_NONE) {
    printf("%s:failed to set rx pfc voq maps - %s\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

/**
 * pending to reset port pg mmu attributes.
 */
bcm_error_t
bcmsdk_pfc_port_pg_disable (int unit,
                            bcm_port_t port,
                            int pg)
{
	int rv = BCM_E_NONE;
	bcm_port_priority_group_config_t cfg;

/*
PC_PFC
  11 fields (1 key-type field):
    OPERATIONAL_STATE
    PFC_PASS
    XOFF_TIMER
    OPCODE
    ETH_TYPE
    DEST_ADDR
    REFRESH_TIMER
    ENABLE_STATS
    ENABLE_TX
    ENABLE_RX
    PORT_ID (key)
*/
	rv = bcm_port_control_set(unit, port, bcmPortControlPFCTransmit, false);
  if (rv != BCM_E_NONE) {
    printf("%s:port %d pg %d failed to set PFC TX off - %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

	/*
	 * tableName: TM_ING_PORT_PRI_GRP
	 * fieldName: LOSSLESS.
	 BCM.0> bsh -c 'lt list @TM_ING_PORT_PRI_GRP'
	 bsh -c 'lt list @TM_ING_PORT_PRI_GRP'
	 TM_ING_PORT_PRI_GRP
	 7 fields (2 key-type fields):
	 OPERATIONAL_STATE
	 IGNORE_PFC_XOFF
	 PFC
	 LOSSLESS
	 ING_MIN_MODE
	 TM_PRI_GRP_ID (key)
	 PORT_ID (key)
	 BCM.0>
	 */
	rv = bcm_cosq_port_priority_group_property_set(unit, port, pg,
																								 bcmCosqPriorityGroupLossless,
                                                 false);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to lossy pg %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

    /*
     * tableName: TM_ING_PORT
     * fieldName: PAUSE
     BCM.0> bsh -c 'lt list @TM_ING_PORT'
     bsh -c 'lt list @TM_ING_PORT'
     TM_ING_PORT
     5 fields (1 key-type field):
     OPERATIONAL_STATE
     PAUSE
     PRI_GRP_MAP_ID
     ING_PRI_MAP_ID
     PORT_ID (key)
     BCM.0>
     */

	rv = bcm_cosq_port_priority_group_property_set(unit, port, pg,
																								 bcmCosqPauseEnable, false);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to pause %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

	bcm_port_priority_group_config_t_init(&cfg);
	rv = bcm_port_priority_group_config_get(unit, port, pg, &cfg);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to get pg config %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

/*
BCMLT.0> lt list TM_ING_PORT_PRI_GRP
TM_ING_PORT_PRI_GRP
  7 fields (2 key-type fields):
    OPERATIONAL_STATE
    IGNORE_PFC_XOFF
    PFC // pfc_transmit_enable
    LOSSLESS
    ING_MIN_MODE
    TM_PRI_GRP_ID (key)
    PORT_ID (key)
BCMLT.0>
*/
	cfg.pfc_transmit_enable = false;
	rv = bcm_port_priority_group_config_set(unit, port, pg, &cfg);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to set pg config %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

// pending LT
  rv = bcm_port_pause_set(unit, port, true, true);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d failed to enable ethernet pause frames %s\n",
           __FUNCTION__, port, bcm_errmsg(rv));
    return rv;
  }

	return BCM_E_NONE;
}

/* Enable PFC along with other attributes.
 * Here Xoff thresholds are a combination of pg_min and pg_shared.
 * Pending is to configure Xon threshold
 */
bcm_error_t
bcmsdk_pfc_port_pg_enable (int unit, bcm_port_t port, int pg,
                           int pg_min, int pg_headroom, int pg_shared)
{
  int rv = 0;
  bcm_port_priority_group_config_t cfg;
  bcm_mac_t pause_sys_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x25};

  // Disable normal ethernet pause
// pending LT
  rv = bcm_port_pause_set(unit, port, false, false);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d failed to disable ethernet pause frames %s\n",
           __FUNCTION__, port, bcm_errmsg(rv));
    return rv;
  }

  rv = bcmsdk_setup_mmu_port_pg_attrs(unit, port, pg,
                                      pg_min, pg_headroom, pg_shared);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to set port pg attrs %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

/*
BCMLT.0> lt list PC_MAC_CONTROL
PC_MAC_CONTROL
  59 fields (1 key-type field):
    OPERATIONAL_STATE
    PAD_THRESHOLD
    PAD_ENABLE
    LINK_DOWN_TRANSMIT
    CRC_MODE_OPER
    CRC_MODE
    STRIP_CRC_OPER
    STRIP_CRC
    TX_ON_LOOPBACK
    PURGE_WRONG_SA
    PURGE_STACK_VLAN_FRAMES
    PURGE_UNSUPPORTED_PAUSE_PFC_DA
    PURGE_RX_CODE_ERROR_FRAMES
    PURGE_CRC_ERROR_FRAMES
    PURGE_LENGTH_CHECK_FAIL_FRAMES
    PURGE_TRUNCATED_FRAMES
    PURGE_GOOD_FRAMES
    PURGE_MULTICAST_FRAMES
    PURGE_BROADCAST_FRAMES
    PURGE_PROMISCUOUS_FRAMES
    PURGE_CONTROL_FRAMES
    PURGE_PAUSE_FRAMES
    PURGE_BAD_OPCODE_FRAMES
    PURGE_VLAN_TAGGED_FRAMES
    PURGE_UNICAST_FRAMES
    RX_FIFO_FULL
    PURGE_RUNT_FRAMES
    PURGE_PFC_FRAMES
    PURGE_SCH_CRC_ERROR
    PURGE_MACSEC_FRAMES
    PURGE_DRIBBLE_NIBBLE_ERROR_FRAMES
    STALL_TX_OPER
    STALL_TX
    PAUSE_TX_OPER
    PAUSE_RX_OPER
    RUNT_THRESHOLD_OPER
    INTER_FRAME_GAP_OPER
    TX_ENABLE_OPER
    RX_ENABLE_OPER
    MAC_ECC_INTR_ENABLE
    RUNT_THRESHOLD
    RUNT_THRESHOLD_AUTO
    CONTROL_PASS
*/
  rv = bcm_port_pause_addr_set(unit, port, pause_sys_mac);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to setup PFC src addr %s\n",
           __FUNCTION__, port, pg, bcm_errmsg(rv));
    return rv;
  }

/*
PC_PFC
  11 fields (1 key-type field):
    OPERATIONAL_STATE
    PFC_PASS
    XOFF_TIMER
    OPCODE
    ETH_TYPE
    DEST_ADDR
    REFRESH_TIMER
    ENABLE_STATS
    ENABLE_TX
    ENABLE_RX
    PORT_ID (key)
*/
  rv = bcm_port_control_set(unit, port, bcmPortControlPFCTransmit, true);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d failed to enable pfctx %s\n",
           __FUNCTION__, port, bcm_errmsg(rv));
    return rv;
  }

    /*
     * tableName: TM_ING_PORT_PRI_GRP
     * fieldName: LOSSLESS.
     BCM.0> bsh -c 'lt list @TM_ING_PORT_PRI_GRP'
     bsh -c 'lt list @TM_ING_PORT_PRI_GRP'
     TM_ING_PORT_PRI_GRP
     7 fields (2 key-type fields):
     OPERATIONAL_STATE
     IGNORE_PFC_XOFF
     PFC
     LOSSLESS
     ING_MIN_MODE
     TM_PRI_GRP_ID (key)
     PORT_ID (key)
     BCM.0>
     */
  rv = bcm_cosq_port_priority_group_property_set(unit, port, pg,
                                                 bcmCosqPriorityGroupLossless,
                                                 true);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to set lossless %s\n",
           __FUNCTION__, port, pg , bcm_errmsg(rv));
    return rv;
  }

    /*
     * tableName: TM_ING_PORT
     * fieldName: PAUSE
     BCM.0> bsh -c 'lt list @TM_ING_PORT'
     bsh -c 'lt list @TM_ING_PORT'
     TM_ING_PORT
     5 fields (1 key-type field):
     OPERATIONAL_STATE
     PAUSE
     PRI_GRP_MAP_ID
     ING_PRI_MAP_ID
     PORT_ID (key)
     BCM.0>
     */
  rv = bcm_cosq_port_priority_group_property_set(unit, port, pg,
                                                 bcmCosqPauseEnable, true);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to enable pause frames %s\n",
           __FUNCTION__, port, pg , bcm_errmsg(rv));
    return rv;
  }

  bcm_port_priority_group_config_t_init(&cfg);
  rv = bcm_port_priority_group_config_get(unit, port, pg, &cfg);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to get PG config %s\n",
           __FUNCTION__, port, pg , bcm_errmsg(rv));
    return rv;
  }

/*
BCMLT.0> lt list TM_ING_PORT_PRI_GRP
TM_ING_PORT_PRI_GRP
  7 fields (2 key-type fields):
    OPERATIONAL_STATE
    IGNORE_PFC_XOFF
    PFC // pfc_transmit_enable
    LOSSLESS
    ING_MIN_MODE
    TM_PRI_GRP_ID (key)
    PORT_ID (key)
BCMLT.0>
*/
  cfg.pfc_transmit_enable = true;
  rv = bcm_port_priority_group_config_set(unit, port, pg, &cfg);
  if (rv != BCM_E_NONE) {
    printf("%s: port %d pg %d failed to set PG config %s\n",
           __FUNCTION__, port, pg , bcm_errmsg(rv));
    return rv;
  }

  return rv;
}

/**
 * As part of the initialization routines perform the following.
 * Carve out HP0 memory for max MTU size.
 * Create maps localTC to PGs. 1:1
 * Create maps for PGs to SPs.
 * Create mapsfor LocalTC to VOQs, 1:1
 */
bcm_error_t
bcmsdk_init_mmu_profiles (int unit)
{
  int rv = 0;

  rv = bcmsdk_setup_pg_to_sp_hp_mapping(unit, g_pg_to_pools_map_profile_id);
  if (rv != BCM_E_NONE) {
    printf("%s: Failed to setup pg to sp hp mapping - %s\n", __FUNCTION__,
           bcm_errmsg(rv));
    return rv;
  }

  rv = bcmsdk_setup_ing_local_tc_to_pg(unit,
                                g_ing_pri_to_pg_map_profile_id);
  if (rv != BCM_E_NONE) {
    printf("%s: failed to setup local tc to pg - %s.\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }

  rv = bcmsdk_setup_rx_pfc_to_voqs(unit, g_rx_pfc_to_voqs_profile_id);
  if (rv != BCM_E_NONE) {
    printf("%s: Failed to setup pfc priority to voqs - %s\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t
bcmsdk_mmu_port_profile_set (int unit, bcm_port_t port,
                             bcm_cosq_profile_type_t profile_type,
                             int profile_id)
{
  bcm_error_t rv;
  bcm_cosq_dynamic_setting_type_t dynamic;

  rv = bcm_cosq_profile_property_dynamic_get(unit,
                                             profile_type, &dynamic);

  if (rv != BCM_E_NONE) {
    printf("%s: port-%d: failed to get dynamic for type %d - %s\n",
           __FUNCTION__,
           port, profile_type, bcm_errmsg(rv));
    return rv;
  }

  switch (dynamic) {
    case bcmCosqDynamicTypeFixed:
      /*
       * Mapping cannot be changed during run time as disable MMU
       * traffic routine cannot guarantee flush based on Source port
       */
      rv = BCM_E_INTERNAL;
      break;
    case bcmCosqDynamicTypeConditional:
      /*
       * Mapping can be changed as long as the port has no traffic in MMU
       */
      rv = bcm_port_control_set(unit, port,
                                bcmPortControlMmuTrafficEnable, 0);
      if (rv != BCM_E_NONE) {
        printf("%s: port-%d:Failed to disable mmu traffic %s\n",
               __FUNCTION__, port, bcm_errmsg(rv));
        return rv;
      }
      rv = bcm_cosq_port_profile_set(unit, port,
                                     profile_type, profile_id);
      if (rv != BCM_E_NONE) {
        printf("%s: port-%d:Failed to profile type %d id %d-%s\n",
               __FUNCTION__, port, profile_type, profile_id, bcm_errmsg(rv));
        return rv;
      }
      rv = bcm_port_control_set(unit, port,
                                bcmPortControlMmuTrafficEnable, 1);
      if (rv != BCM_E_NONE) {
        printf("%s: port-%d: failed to enable mmu traffic-%s\n",
               __FUNCTION__, port, bcm_errmsg(rv));
        return rv;
      }
      break;
    case bcmCosqDynamicTypeFlexible:
      bcm_cosq_port_profile_set(unit, port,
                                profile_type, profile_id);
      if (rv != BCM_E_NONE) {
        printf("%s: port-%d: failed to set profile type %d id %d-%s\n",
               __FUNCTION__, port, profile_type, profile_id, bcm_errmsg(rv));
        return rv;
      }
      break;
  }
  return rv;
}

bcm_error_t
bcmsdk_mmu_initialize_port (int unit,
                            bcm_port_t port)
{
  bcm_error_t rv;

  rv = bcmsdk_setup_lossy_pgs(unit, port);
  if (rv != BCM_E_NONE) {
    printf("%s: port-%d: Failed to setup lossless pgs - %s\n",
           __FUNCTION__, port, bcm_errmsg(rv));
    return rv;
  }

  rv = bcmsdk_mmu_port_profile_set(unit, port,
                                   bcmCosqProfilePGProperties,
                                   g_pg_to_pools_map_profile_id);
  if (rv != BCM_E_NONE) {
    printf("%s: port-%d: Failed to set profilePG properties - %s\n",
           __FUNCTION__, port, bcm_errmsg(rv));
    return rv;
  }

  rv = bcmsdk_mmu_port_profile_set(unit, port,
                                   bcmCosqProfileIntPriToPGMap,
                                   g_ing_pri_to_pg_map_profile_id);
  if (rv != BCM_E_NONE) {
    printf("%s: port-%d: Failed to set PriToPGMap profile - %s\n",
           __FUNCTION__, port, bcm_errmsg(rv));
    return rv;
  }

  rv = bcmsdk_mmu_port_profile_set(unit, port,
                                   bcmCosqProfilePfc,
                                   g_rx_pfc_to_voqs_profile_id);
  if (rv != BCM_E_NONE) {
    printf("%s: port-%d: Failed to set pfc profile - %s\n",
           __FUNCTION__, port, bcm_errmsg(rv));
    return rv;
  }

  /*
   * Keep PFC Rx enabled on all ports.
   */
/*
PC_PFC
  11 fields (1 key-type field):
    OPERATIONAL_STATE
    PFC_PASS
    XOFF_TIMER
    OPCODE
    ETH_TYPE
    DEST_ADDR
    REFRESH_TIMER
    ENABLE_STATS
    ENABLE_TX
    ENABLE_RX
    PORT_ID (key)
*/
  rv = bcm_port_control_set(unit, port, bcmPortControlPFCReceive, true);
  if (rv != BCM_E_NONE) {
    printf("%s: port-%d: Failed to enable pfc rx - %s\n",
           __FUNCTION__, port, bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

bcm_error_t
bcmsdk_mmu_initialize_all_ports (int unit)
{
  bcm_port_t port;
  bcm_error_t rv;
  bcm_port_config_t configP;

  /* Get current port configuration */
  if (BCM_FAILURE(rv = bcm_port_config_get(unit, &configP))) {
    printf("%s: Error in bcm_port_config_get: %s\n",
           __FUNCTION__, bcm_errmsg(rv)); return rv;
  }
  BCM_PBMP_ITER(configP.d3c, port) {
    // Initialize 800G ports.
    rv = bcmsdk_mmu_initialize_port(unit, port);
    if (rv != BCM_E_NONE) {
      printf("%s: Failed to initialize port %d - %s\n",
             __FUNCTION__, bcm_errmsg(rv));
      return rv;
    }
  }
  return BCM_E_NONE;
}

bcm_error_t
bcmsdk_mmu_init (int unit)
{
  bcm_error_t rv;
  rv = bcmsdk_setup_mmu_thdi_attrs(unit);

  if (rv != BCM_E_NONE) {
    printf("%s: failed to mmu thdi attrs %s\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }

  rv = bcmsdk_init_mmu_profiles(unit);
  if (rv != BCM_E_NONE) {
    printf("%s: failed to init mmu profiles %s\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }
  rv = bcmsdk_mmu_initialize_all_ports(unit);
  if (rv != BCM_E_NONE) {
    printf("%s: failed to initiliaze ports %s\n",
           __FUNCTION__, bcm_errmsg(rv));
    return rv;
  }
  printf("%s: Unit %d Success\n", __FUNCTION__, unit);
  return rv;
}
bcmsdk_mmu_init(0);

/* unit = 0, port = 11, pg = 3, pgmin = 10*254, pgshared = 100*254, hdrm = 20*254
 * print bcmsdk_pfc_port_pg_enable(unit, port, pg, pgmin, hdrm, pgshared);
 * print bcmsdk_pfc_port_pg_disable(unit, port, pg);
 */
