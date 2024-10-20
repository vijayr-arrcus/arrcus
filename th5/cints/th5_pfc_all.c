/*
*	BCMLT.0> lt traverse -l TM_DEVICE_INFO
*	Traverse table [TM_DEVICE_INFO]:
*			NUM_REPL_Q=7
*			NUM_REPL_CELLS=0x2c00(11264)
*			NUM_BUFFER_POOL=2
*			NUM_PIPE=0x20(32)
*			NUM_CPU_Q=0x30(48)
*			NUM_Q=0xc(12)
*			NUM_SERVICE_POOL=4
*			NUM_PORT_PRI_GRP=8
*			NUM_CELLS=0x53458(341080)
*			CELL_SIZE=0xfe(254)
*			DEFAULT_MTU=0x600(1536)
*			JUMBO_PKT_SIZE=0x2400(9216)
*			PKT_HDR_SIZE=0x40(64)
*			MAX_PKT_SIZE=0x24c8(9416)
*			1 entry traversed.
*	BCMLT.0>
*/

    bcmCosqControlIngressPool = 100,    /* Associate an Ingress Cos to a
                                           specific service pool */


/*
 * Setup Port PriorityGroup headroom value based on cable length and
 * packet distribution.
 */
extern int bcm_cosq_port_prigrp_hdrm_set(
    int unit,
    bcm_port_t port,
    uint32 pri_bmp,
    int cable_len,
    int arr_size,
    bcm_cosq_pkt_size_dist_t *pkt_dist_array);

/*
 * Calculate and set Headroom Pool limit based on number of lossless
 * classes and
 * packet distribution.
 */
extern int bcm_cosq_hdrm_pool_limit_set(
    int unit,
    int hdrm_pool,
    int num_lossless_class,
    int arr_size,
    bcm_cosq_pkt_size_dist_t *pkt_dist_array);




bcm_error_t setup_thdi_attrs (int unit)
{
  bcm_gport_t port_gport;
  int pg_headroom = 10*254;
  int pg_min = 45*254;
  int PriorityGrp = 7;
  int cosq=0; /* PG for THDI */
  int arg;

  print bcm_port_gport_get(0, port, &port_gport);

  /* Ingress headroom pool limit settings.
     bcmCosqControlIngressHeadroomPoolLimitBytes
BCMLT.0> lt list TM_ING_THD_HEADROOM_POOL
TM_ING_THD_HEADROOM_POOL
  5 fields (2 key-type fields):
    OPERATIONAL_STATE
    LIMIT_CELLS_OPER
    LIMIT_CELLS
    BUFFER_POOL (key)
    TM_HEADROOM_POOL_ID (key)
BCMLT.0> lt traverse -l TM_ING_THD_HEADROOM_POOL
Traverse table [TM_ING_THD_HEADROOM_POOL]:
    OPERATIONAL_STATE=VALID
    LIMIT_CELLS_OPER=0xa(10)
    LIMIT_CELLS=0xa(10)
    BUFFER_POOL=0
    TM_HEADROOM_POOL_ID=0

    OPERATIONAL_STATE=VALID
    LIMIT_CELLS_OPER=0
    LIMIT_CELLS=0
    BUFFER_POOL=0
    TM_HEADROOM_POOL_ID=1

    OPERATIONAL_STATE=VALID
    LIMIT_CELLS_OPER=0
    LIMIT_CELLS=0
    BUFFER_POOL=0
    TM_HEADROOM_POOL_ID=2

    OPERATIONAL_STATE=VALID
    LIMIT_CELLS_OPER=0
    LIMIT_CELLS=0
    BUFFER_POOL=0
    TM_HEADROOM_POOL_ID=3

    OPERATIONAL_STATE=VALID
    LIMIT_CELLS_OPER=0
    LIMIT_CELLS=0
    BUFFER_POOL=1
    TM_HEADROOM_POOL_ID=0

    OPERATIONAL_STATE=VALID
    LIMIT_CELLS_OPER=0
    LIMIT_CELLS=0
    BUFFER_POOL=1
    TM_HEADROOM_POOL_ID=1

    OPERATIONAL_STATE=VALID
    LIMIT_CELLS_OPER=0
    LIMIT_CELLS=0
    BUFFER_POOL=1
    TM_HEADROOM_POOL_ID=2

    OPERATIONAL_STATE=VALID
    LIMIT_CELLS_OPER=0
    LIMIT_CELLS=0
    BUFFER_POOL=1
    TM_HEADROOM_POOL_ID=3
  8 entries traversed.
BCMLT.0>
   */
  bcm_cosq_control_dynamic_get(unit, bcmCosqControlIngressHeadroomPoolLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressHeadroomPoolLimitBytes\n");
      print bcm_cosq_control_set(0, port_gport, cosq,
																 bcmCosqControlIngressHeadroomPoolLimitBytes,
																 pg_headroom);
      print bcm_cosq_control_get(0, port_gport, cosq,
																 bcmCosqControlIngressHeadroomPoolLimitBytes,
																 &arg);
      print arg;
  }


  /* Ingress Port PG Min limit setting.
     bcmCosqControlIngressPortPGMinLimitBytes
BCMLT.0> lt list TM_ING_THD_PORT_PRI_GRP
TM_ING_THD_PORT_PRI_GRP
  18 fields (2 key-type fields):
    EARLY_PFC_FLOOR_CELLS
    EARLY_PFC_XON_OFFSET_CELLS
    EARLY_PFC_XOFF_OFFSET_CELLS
    TM_ING_BST_THD_PRI_GRP_HEADROOM_PROFILE_ID
    OPERATIONAL_STATE
    TM_ING_BST_THD_PRI_GRP_PROFILE_ID
    HEADROOM_LIMIT_CELLS_OPER
    HEADROOM_LIMIT_CELLS
    HEADROOM_LIMIT_AUTO
    RESUME_FLOOR_CELLS
    RESUME_OFFSET_CELLS
    SHARED_LIMIT_DYNAMIC
    SHARED_LIMIT_CELLS_STATIC
    DYNAMIC_SHARED_LIMITS
    MIN_GUARANTEE_CELLS_OPER
    MIN_GUARANTEE_CELLS
    TM_PRI_GRP_ID (key)
    PORT_ID (key)
BCMLT.0>
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPGMinLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPGMinLimitBytes\n");
      print bcm_cosq_control_set (unit, port_gport, PriorityGrp, bcmCosqControlIngressPortPGMinLimitBytes,pg_min);
      print bcm_cosq_control_get (unit, port_gport, PriorityGrp, bcmCosqControlIngressPortPGMinLimitBytes,&arg);
      print arg;
  }

  /* Ingress Port PG Headroom limit setting
     bcmCosqControlIngressPortPGHeadroomLimitBytes
BCMLT.0> lt list TM_ING_THD_PORT_PRI_GRP
TM_ING_THD_PORT_PRI_GRP
  18 fields (2 key-type fields):
    EARLY_PFC_FLOOR_CELLS
    EARLY_PFC_XON_OFFSET_CELLS
    EARLY_PFC_XOFF_OFFSET_CELLS
    TM_ING_BST_THD_PRI_GRP_HEADROOM_PROFILE_ID
    OPERATIONAL_STATE
    TM_ING_BST_THD_PRI_GRP_PROFILE_ID
    HEADROOM_LIMIT_CELLS_OPER
    HEADROOM_LIMIT_CELLS
    HEADROOM_LIMIT_AUTO
    RESUME_FLOOR_CELLS
    RESUME_OFFSET_CELLS
    SHARED_LIMIT_DYNAMIC
    SHARED_LIMIT_CELLS_STATIC
    DYNAMIC_SHARED_LIMITS
    MIN_GUARANTEE_CELLS_OPER
    MIN_GUARANTEE_CELLS
    TM_PRI_GRP_ID (key)
    PORT_ID (key)
   */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPGHeadroomLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPGHeadroomLimitBytes\n");
      print bcm_cosq_control_set (unit, port_gport, PriorityGrp, bcmCosqControlIngressPortPGHeadroomLimitBytes,pg_headroom);
      print bcm_cosq_control_get (unit, port_gport, PriorityGrp, bcmCosqControlIngressPortPGHeadroomLimitBytes,&arg);
      print arg;
  }

  /* Ingress Port PG Shared limit setting.
    bcmCosqControlIngressPortPGSharedLimitBytes
BCMLT.0> lt list TM_ING_THD_PORT_PRI_GRP
TM_ING_THD_PORT_PRI_GRP
  18 fields (2 key-type fields):
    EARLY_PFC_FLOOR_CELLS
    EARLY_PFC_XON_OFFSET_CELLS
    EARLY_PFC_XOFF_OFFSET_CELLS
    TM_ING_BST_THD_PRI_GRP_HEADROOM_PROFILE_ID
    OPERATIONAL_STATE
    TM_ING_BST_THD_PRI_GRP_PROFILE_ID
    HEADROOM_LIMIT_CELLS_OPER
    HEADROOM_LIMIT_CELLS
    HEADROOM_LIMIT_AUTO
    RESUME_FLOOR_CELLS
    RESUME_OFFSET_CELLS
    SHARED_LIMIT_DYNAMIC
    SHARED_LIMIT_CELLS_STATIC
    DYNAMIC_SHARED_LIMITS
    MIN_GUARANTEE_CELLS_OPER
    MIN_GUARANTEE_CELLS
    TM_PRI_GRP_ID (key)
    PORT_ID (key)
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPGSharedLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPGSharedLimitBytes\n");
      print bcm_cosq_control_set(unit, port_gport, 2, bcmCosqControlIngressPortPGSharedLimitBytes, (200*254));
      print bcm_cosq_control_get(unit, port_gport, 2, bcmCosqControlIngressPortPGSharedLimitBytes, &arg);
      print arg;
  }
  return BCM_E_NONE;
}

int setup_pg_to_tx_pfc_pri (int unit, int profile_id)
{
	int pg_array[8] = {0, 7, 7, 7, 0, 0, 0, 0}; // PG array, indexed by PFC priority.
  //if PG7 is congested 1,2,3 priorities are set.
  // But does it mean that if PG0 is constrained priority 0, 4, 5, 6 and 7 are set ?
	int pg_array_2[8] = {0, 1, 2, 3, 4, 5, 6, 7}; // PG array, indexed by PFC priority.
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
		printf("bcm_cosq_priority_group_mapping_profile_set() failed:%s.\n",
           bcm_errmsg(rv));
		return rv;
	}
  return rv;
}

int
setup_pg_to_sp_hp_mapping (int unit, int profile_id)
{
	bcm_cosq_priority_group_mapping_profile_type_t prof_type;
	int arr_count;
  // TM_PRI_GRP_POOL_MAP
	int sp_array[8]= {0, 0, 0, 0, 0, 0, 0, 0};
	int hp_array[8]= {0, 0, 0, 0, 0, 0, 0, 0};

	arr_count=8;

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
																									 arr_count, sp_array);
	if (rv != BCM_E_NONE) {
		printf("bcm_cosq_priority_group_mapping_profile_set() failed:%s.\n",
					 bcm_errmsg(rv));
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
	prof_type=bcmCosqPriorityGroupHeadroomPoolMapping;
	rv = bcm_cosq_priority_group_mapping_profile_set(unit, profile_id,
																									 prof_type,
																									 arr_count, hp_array);
	if (rv != BCM_E_NONE) {
		printf("bcm_cosq_priority_group_mapping_profile_set() failed:%s.\n",
					 bcm_errmsg(rv));
		return rv;
	}

  rv = setup_pg_to_tx_pfc_pri(unit, profile_id);

  return BCM_E_NONE;
}

int
setup_ing_local_tc_to_pg (int unit, int profile_id)
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
    printf("bcm_cosq_priority_group_mapping_profile_set falied():%s.\n",
           bcm_errmsg(rv));
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
    printf("bcm_cosq_priority_group_mapping_profile_set falied():%s.\n",
           bcm_errmsg(rv));
    return rv;
  }
  return BCM_E_NONE;
}

int setup_lossless_pgs (int unit, int port)
{
  int lossless_pg_array[] = {7};
  int array_count = 1;

	for (int i = 0; i < array_count; i++) {
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
		print
		 bcm_cosq_port_priority_group_property_set(unit,
																							 port,
																							 lossless_pg_array[i],
																							 bcmCosqPriorityGroupLossless,
																							 1);
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
		print bcm_cosq_port_priority_group_property_set(unit,
																										port,
																										lossless_pg_array[i],
																										bcmCosqPauseEnable, 1);
	}
}

int setup_rx_pfc_to_voqs (int unit,
                          int profile_id)
{
	int pfc_class;
	int rv = 0;
	int profile_id = 0;
	bcm_cosq_pfc_class_map_config_t config_array[8];   /* Array index implicitly indicate PFC priority */

	for (pfc_class = 0; pfc_class < 8; pfc_class++) {
		bcm_cosq_pfc_class_map_config_t_init(&config_array[pfc_class]);
	}

	config_array[0].pfc_enable = 1;
	config_array[0].pfc_optimized = 0;
	config_array[0].cos_list_bmp = (1U <<1) | (1U <<2) | (1U <<3); // list of cosq voqs to pause when priorit 0 is received.
	config_array[0].pfc_flags = BCM_COSQ_PFC_MAP_TO_QUEUES;
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
	print bcm_cosq_pfc_class_config_profile_set(unit, profile_id, 8, &config_array);

}

/* Enable/Disable PFC-Tx or PFC-Rx based on pfc_txrx */
bcm_error_t
bcmsdk_pfc_port_enable (int unit,
                        bcm_port_t port,
                        int pfc_txrx,
                        int pfc_enable)
{
  int rv = 0;
  int disable = 0;
  int enable = 1;
  bcm_port_control_t pfc_type; // bcmPortControlPFCTransmit, bcmPortControlPFCReceive

// #define PFC_TX 0
// #define PFC_RX 1
  static int pfc_tx = 0;
  static int pfc_rx = 1;
bcm_mac_t pause_mac = {0x00, 0x00, 0x11, 0x11, 0x22, 0x25};


  rv = bcm_port_pause_addr_set(unit, port, pause_smac);
  if (rv != BCM_E_NONE) {
    printf("ERR: Pause MAC failed", bcm_errmsg(rv));
  } else {
    printf("PFC Source MAC Set (pause_smac): port:%d smac:%02x:%02x:%02x:%02x:%02x:%02x\n", port,
      pause_smac[0],pause_smac[1],pause_smac[2],pause_smac[3],pause_smac[4],pause_smac[5]);
  }

  if (pfc_tx == pfc_txrx) {
    pfc_type = bcmPortControlPFCTransmit;
    printf("bcmsdk_pfc_port_enable: PFC-Tx\n");
  } else if (pfc_rx == pfc_txrx) {
    pfc_type = bcmPortControlPFCReceive;
    printf("bcmsdk_pfc_port_enable: PFC-Rx\n");
  } else {
    printf("PFC-PORT-ENABLE-ERR: Wrong pfc_txrx. pfc_txrx:%d Port:%d\n", pfc_txrx, port);
  }

  if (pfc_enable) {
    /* 1. Disable normal PAUSE on the port.
     * This might not be required on XGS device.
     */
    rv = bcm_port_pause_set(unit, port, disable, disable);
    if (rv != BCM_E_NONE) {
      printf("port:%d PFC-PORT-ENABLE-ERR: Disable normal PAUSE failed.\n", port);
      printf("bcm_errmsg: %s", bcm_errmsg(rv));
    } else {
      printf("port:%d Normal PAUSE disabled.\n", port);
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
    /* Enable PFC on port */
    rv = bcm_port_control_set(unit, port, pfc_type, enable);
    if (rv != BCM_E_NONE) {
      printf("port:%d PFC-PORT-ENABLE-ERR: Enable PFC failed.\n", port);
      printf("bcm_errmsg: %s", bcm_errmsg(rv));
    } else {
      printf("port:%d PFC Enabled.\n", port);
    }
  } else {
    /* Disable PFC on port */
    rv = bcm_port_control_set(unit, port, pfc_type, disable);
    if (rv != BCM_E_NONE) {
      printf("port:%d PFC-PORT-ENABLE-ERR: Disable PFC failed.\n", port);
      printf("bcm_errmsg: %s", bcm_errmsg(rv));
    } else {
      printf("port:%d PFC Disabled.\n", port);
    }
  }

  return rv;
}

int test_start (int unit)
{
  int rv = 0;
  int pri_grp_pool_map_profile_id = 0;
  int rx_pfc_to_voqs_profile_id = 0;
  int ing_pri_pg_map_profile_id = 0;
  int port_id = 1;

  /* enable Rx PFC */
  print bcm_port_control_set(unit, port, bcmPortControlPFCReceive, 1); 

  setup_pg_to_sp_hp_mapping(unit, pri_grp_pool_map_profile_id);

/**
  * Table Name: TM_ING_PORT.
  * Field Name: PRI_GRP_MAP_ID.
  *     OPERATIONAL_STATE=PORT_INFO_UNAVAILABLE
    PAUSE=0
    PRI_GRP_MAP_ID=0
    ING_PRI_MAP_ID=0
    PORT_ID=0x15d(349)
  341 entries traversed.
BCM.0>
BCM.0> bsh -c 'lt list @TM_ING_PORT'
TM_ING_PORT
  5 fields (1 key-type field):
    OPERATIONAL_STATE
    PAUSE
    PRI_GRP_MAP_ID
    ING_PRI_MAP_ID
    PORT_ID (key)
BCM.0>

  */
  rv= bcm_cosq_port_profile_set(unit, port,
                                bcmCosqProfilePGProperties,
                                pri_grp_pool_map_profile_id);

  setup_ing_local_tc_to_pg(unit, ing_pri_pg_map_profile_id);
/**
  * Table Name: TM_ING_PORT.
  * Field Name: ING_PRI_MAP_ID.
  *     OPERATIONAL_STATE=PORT_INFO_UNAVAILABLE
    PAUSE=0
    PRI_GRP_MAP_ID=0
    ING_PRI_MAP_ID=0
    PORT_ID=0x15d(349)
  341 entries traversed.
BCM.0>
BCM.0> bsh -c 'lt list @TM_ING_PORT'
TM_ING_PORT
  5 fields (1 key-type field):
    OPERATIONAL_STATE
    PAUSE
    PRI_GRP_MAP_ID
    ING_PRI_MAP_ID
    PORT_ID (key)
BCM.0>
*/
  rv = bcm_cosq_port_profile_set(unit, port,
                                 bcmCosqProfileIntPriToPGMap,
                                 ing_pri_pg_map_profile_id);
  if (rv != BCM_E_NONE) {
    printf("bcm_cosq_port_profile_set falied():%s.\n", bcm_errmsg(rv));
    return rv;
  }

  setup_lossless_pgs(unit, port);

  setup_rx_pfc_to_voqs(unit, rx_pfc_to_voqs_profile_id);
	/* associate port with PFC-to-COS profile id */
  /* BCM.0>
   * BCM.0> bsh -c 'lt list @TM_PFC_EGR'
   * bsh -c 'lt list @TM_PFC_EGR'
   * TM_PFC_EGR
   *   4 fields (1 key-type field):
   *   OPERATIONAL_STATE
   *   TM_PFC_PRI_PROFILE_ID
   *   BLOCK_PFC_QUEUE_UPDATES
   *   PORT_ID (key)
   * BCM.0>
   */

   bcm_cosq_profile_type_t profile_type = bcmCosqProfilePfc;
    bcm_cosq_dynamic_setting_type_t dynamic;
    BCM_IF_ERROR_RETURN(bcm_cosq_profile_property_dynamic_get(unit, profile_type, &dynamic));

    if (dynamic == bcmCosqDynamicTypeFixed) {
        /*
         * Mapping cannot be changed during run time as disable MMU
         * traffic routine cannot guarantee flush based on Source port
         * Example: bcmCosqProfilePriGroupProperties
         */
        printf("The profile %d can not be modified(bcmCosqDynamicTypeFixed)\n", rx_pfc_to_voqs_profile_id);
        return -1;
    } else if (dynamic == bcmCosqDynamicTypeConditional) {
        /*
         * Mapping can be changed as long as the port has no traffic in MMU
         * Example: bcmCosqProfileQueueHierarchy, bcmCosqProfilePFCClass,
         *          bcmCosqProfilePFCAndQueueHierarchy
         */
        printf("The profile %d is bcmCosqDynamicTypeConditional\n", profile_id);
        BCM_IF_ERROR_RETURN(bcm_port_control_set (unit, port, bcmPortControlMmuTrafficEnable, 0));

        /* Attach sched profile to specified port */
        rv = bcm_cosq_port_profile_set(unit, port, bcmCosqProfilePfc, rx_pfc_to_voqs_profile_id);
        if (rv != BCM_E_NONE) {
             printf("bcm_cosq_port_profile_set failed: %s.\n", bcm_errmsg(rv));
             return rv;
        }

        BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port, bcmPortControlMmuTrafficEnable, 1));
    } else if (dynamic == bcmCosqDynamicTypeFlexible) {
        /* Mapping can be changed at run time. Example : bcmCosqProfileIntPriToPGMap */
        printf("The profile %d is bcmCosqDynamicTypeFlexible\n", profile_id);

        /* Attach sched profile to specified port */
        rv = bcm_cosq_port_profile_set(unit, port, bcmCosqProfilePfc, rx_pfc_to_voqs_profile_id);
        if (rv != BCM_E_NONE) {
             printf("bcm_cosq_port_profile_set failed: %s.\n", bcm_errmsg(rv));
             return rv;
        }
    }

  return BCM_E_NONE;
}
