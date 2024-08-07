/*
 * Feature  : Dynamic Load Balancing (DLB) over ECMP group test with Traffic
 *
 * Usage    : BCM.0> cint th5_hsdk_l3_ecmp_dlb_traffic_test.c
 *
 * config   : bcm78900_b0-generic-320x100-l3.config.yml
 *
 * Log file : th5_hsdk_l3_ecmp_dlb_traffic_test_log.txt
 *
 * -----------
 * Description
 * -----------
 *  This cint script demonstrates the configuration of ECMP group in Dynamic
 * Load Balancing (DLB) mode. It also demonstrates a traffic test to show the
 * working of Dynamic Load Balancing of incoming IPv4 traffic streams across the
 * ECMP member ports. It shows how Quality on egress member ports varies according
 * to existing incoming traffic stream load (rate), which is used as metric to
 * assign new incoming streams. Further, it also shows that other unrelated L2
 * traffic on one of the ECMP egress member ports is also considered in quality
 * calculations so that the right flow assignment and traffic distribution
 * happens.
 *
 * -------------------------------
 * Test Topology and traffic setup
 * -------------------------------
 *                  +-----------------------------------------------+
 *   +--->          |                                               |
 *  L2 Stream    +-->ce3(4)     L2 Entry                            |
 *  Rate = 20%      |           ---------                           |
 *                  |           mac=0x123                           |
 *                  |           vlan=400                            |
 *                  |           port=ce2(3)                         |
 *                  |                                               |
 *                  |                          ++Egr Obj            |
 *   +--->       +-->ce4(5)     L3 Route       | 100001  --   ce0(1)+--->
 *  L3 Stream 1     |           --------       |                    |
 *  Rate = 50%      |         IP=10.10.0.0/16  |                    |
 *                  |         Dest=ECMP group +-+Egr Obj            |
 *                  |              203968      | 100002  --   ce1(2)+--->
 *   +--->       +-->ce5(6)                    |                    |
 *  L3 Stream 2     |                          |                    |
 *  Rate = 70%      |                          ++Egr Obj            |
 *                  |                            100003  --   ce2(3)+--->
 *                  |                                               |
 *                  |               BCM78900 (TH5)                  |
 *                  +-----------------------------------------------+
 *
 * ---------------
 * Stream details:
 * ---------------
 * L2 Stream: (Packet Length=100 Bytes | 20% line rate |ingress port ce3(4))
 *      DMAC = 00:00:00:00:01:23
 *      SMAC = 00:00:00:00:04:56
 *      <L2 Payload>
 *
 * L3 Stream 1: (Packet Length=100 Bytes | 50% line rate |ingress port ce4(5))
 *      DMAC = 00:00:00:00:00:01
 *      SMAC = 00:00:00:00:00:0A
 *          IPv4 SIP = 20.20.20.20
 *          IPv4 DIP = 10.10.10.10
 *          Protocol = UDP (17)
 *              UDP L4 Src Port = 5555
 *              UDP L4 Dst Port = 25
 *              <UDP Payload>
 *
 * L3 Stream 2: (Packet Length=100 Bytes | 70% line rate |ingress port ce5(6))
 *      DMAC = 00:00:00:00:00:01
 *      SMAC = 00:00:00:00:00:0B
 *          IPv4 SIP = 20.20.20.30
 *          IPv4 DIP = 10.10.10.10
 *          Protocol = UDP (17)
 *              UDP L4 Src Port = 5556
 *              UDP L4 Dst Port = 25
 *              <UDP Payload>
 *
 * -----------------------------------
 * Expected final traffic distribution
 * -----------------------------------
 *                  +-------------------+
 *   +--->          |                   |
 *  L2 Stream    +-->ce3(4)             |
 *  Rate = 20%      |                   |
 *                  |             ce0(1)+---> L3 Stream 2 (70%)
 *                  |                   |     = 70%
 *                  |                   |  QUALITY=3
 *                  |                   |
 *                  |                   |
 *   +--->       +-->ce4(5)             |
 *  L3 Stream 1     |             ce1(2)+---> L3 Stream 1 (50%)
 *  Rate = 50%      |                   |     = 50%
 *                  |                   |  QUALITY=4
 *                  |                   |
 *   +--->       +-->ce5(6)             |
 *  L3 Stream 2     |                   |
 *  Rate = 70%      |             ce2(3)+---> L2 Stream (20%)
 *                  |                   |     = 20%
 *                  |                   |  QUALITY=5
 *                  |   BCM78900(TH5)   |
 *                  +-------------------+
 *
 * ---------------------------------------------
 * Detailed steps performed and expected results
 * ---------------------------------------------
 * STEP-1: Port setup (done in test_setup())
 *  - Three ingress ports ce3(4), ce4(5) and ce5(6) are configured to be working
 *    in MAC loopback mode.
 *  - Egress ports ce0(1), ce1(2) and ce2(3) are configured to be working
 *    in PHY loopback mode. IFP rule is configured to drop loop backed packets.
 *
 * STEP-2: DLB ECMP configuration (done in configure_dlb_ecmp())
 *  - Configure RTAG7 for DLB ECMP distribution to work
 *  - Configure DLB global parameters for DLB hardware calculations to work
 *  - Create VLAN, my station tcam and L3 ingress and egress interfaces
 *  - Create 3 L3 egress objects and set per port DLB attributes on egress ports
 *  - Create DLB ECMP group.
 *  - Create L3 route with the above DLB ECMP group as a destination.
 *  - Create egress port based flex counters on each ECMP member port.
 *
 * STEP-3: L2 Unicast path configuration (done in config_l2_flow_path())
 *  - To showcase that DLB hardware calculations take other non-ECMP traffic load
 *    too on an ECMP member egress port into account, create an L2 unicast entry
 *    destined to ce2(3) which is part of DLB ECMP group created above.
 *
 * STEPS 4-7: Traffic test and verification (done in test_verify())
 * NOTE:
 *     For performing traffic test, reserved ports and reserved VLANs are used
 * to first generate loopback based flooding traffic without any learning. The
 * packet of interest is looped back. Each ingress port is made part of a
 * specific reserved vlan. Then, to start traffic, the "port vlan" on ingress port
 * is set to actual VLAN of interest so that all the traffic goes as per the
 * forwarding logic setup.
 *
 * STEP-4: Start L2 traffic stream @20% line rate ingressing ce3(4) port
 *  - Collect egress flex counter stats two times with a time interval to check
 *    which egress ports are sending out traffic.
 *    Also, Get the final DLB Quality on each egress port using
 *    bcm_l3_ecmp_dlb_port_quality_get API.
 *  - Expected result:
 *      Packets should be egressing ce2(3) port at 20% line rate.
 *      Total one egress port should be sending out as 1 stream is ingressing.
 *      Quality on ce2(3) port should reduce to 5 from maximum of 7.
 *      Final qualities on (1, 2, 3) should be (7, 7, 5) respectively.
 *
 * STEP-5: Start L3 traffic stream-1 @50% line rate ingressing ce4(5) port
 *  - Collect egress flex counter stats two times with a time interval to check
 *    which egress ports are sending out traffic.
 *    Also, Get the final DLB Quality on each egress port using
 *    bcm_l3_ecmp_dlb_port_quality_get API.
 *  - Expected result:
 *      New stream should be egressing ce0(1) port at 50% line rate.
 *      Total two egress ports should be sending out as 2 streams are ingressing.
 *      Quality on ce1(2) port should reduce to 4 from maximum of 7.
 *      Final qualities on (1, 2, 3) should be (7, 4, 5) respectively.
 *      Note: This stream can take either ce0(1) or ce1(2) randomly as both are of
 *      same quality 7 at this point. ce0(1) is chosen as demo example.
 *
 * STEP-6: Start L3 traffic stream-2 @70% line rate ingressing ce5(6) port
 *  - Collect egress flex counter stats two times with a time interval to check
 *    which egress ports are sending out traffic.
 *    Also, Get the final DLB Quality on each egress port using
 *    bcm_l3_ecmp_dlb_port_quality_get API.
 *  - Expected result:
 *      New stream should be egressing ce1(2) port at 70% line rate.
 *      Total three egress ports should be sending out as 3 streams are ingressing.
 *      Quality on ce0(1) port should reduce to 3 from maximum of 7.
 *      Final qualities on (1, 2, 3) should be (3, 4, 5) respectively.
 *
 *******************************************************************************
 */
cint_reset();
/************************ Configuration Parameters  ***************************/
/* Modify these values as per the traffic setup requirements.                 */

int const NO_OF_ECMP_MEMBERS = 3; /* No of members in the DLB ECMP group */
bcm_port_t egress_port[NO_OF_ECMP_MEMBERS] = {1, 2, 3};
                        /* Egress ports that form members of DLB ECMP group */

/** Traffic simulation test related Parameters **/
bcm_port_t ingress_port[] = {4, 5, 6};

/* Reserved ports and VLANs are used in loopback based traffic generation */
bcm_port_t traffic_rsvd_ports_1[] = {7, 8};
bcm_port_t traffic_rsvd_ports_2[] = {9, 10};
bcm_port_t traffic_rsvd_ports_3[] = {11, 12};

bcm_vlan_t traffic_rsvd_vlan[] = {4004, 4005, 4006};

/*** RTAG7 Parameters ***/
uint32 seed = 0x11111111;
                        /* Seed used in RTAG7 hashing */
int hash_computation = BCM_HASH_FIELD_CONFIG_CRC32HI;
                        /* Hash function to be used in RTAG7 */
int block_n_offset_select = 0;
                        /* RTAG7 hash block and offset selection */
                        /* 0 = Use A_0 hash with offset of 0 */
int pre_process_enable = 1;
                        /* Enable/disable pre-processing of bins */
                        /* 1 = Enable */
int use_flow_based_hash_ecmp = 0;
                        /* Use flow-based or port-based hashing */
                        /* 0 = Use port based hashing,
                                i.e., the block and offset to be used
                                in member selection is fixed per ingress port */
int ipv4_select_fields = BCM_HASH_EXT_FIELD_IP4_ADDRESS |
                         BCM_HASH_EXT_FIELD_L4_DST_PORT |
                         BCM_HASH_EXT_FIELD_L4_SRC_PORT;
                        /* IPv4 packet fields to be used in hashing.
                            Same setting is applied for both TCP/UDP as well as
                            plain IPv4 packets, only relevant flags will be
                            considered by corresponding switch control */

/*** DLB Parameters ***/
/* DLB Hardware engine uses several parameters in determining the final member
   selection in a given DLB ECMP group. These knobs can be configured to tune
   the DLB calculations in user desired expectation */

/* dlb global parameters */
int dlb_ecmp_sampling_rate = 200000;
                    /* the rate at which the dlb calculations happen in hardware
                        a high rate means a low sampling period, which in turn
                        means the changes in load on ecmp members are quickly
                        accounted for in member quality calculations. */
int dlb_ecmp_random_seed = 0x5555;
                    /* IF all DLB eligible ECMP members have equal quality
                        (generally, before start of any traffic), a new incoming
                        flow will be assigned randomly. This seed is used in
                        that Random number generation */

/* DLB engine performs Exponentially Weighted Moving Average (EWMA)
   calculations at sampling rate to determine the load on a member port.
   The following parameters are used to tune that moving average */
int dlb_ecmp_egress_bytes_exponent = 2;
                    /* Weight value used in Port Loading EWMA calculation
                        Higher value indicates more weightage given to
                        average value compared to instantaneous value. */
int dlb_ecmp_queued_bytes_exponent = 2;
                    /* Weight value used in Total port queue size EWMA calculation
                        Higher value indicates more weightage given to
                        average value compared to instantaneous value. */
int dlb_ecmp_physical_queued_bytes_exponent = 2;
                    /* Weight value used in ITM port queue size EWMA calculation
                        Higher value indicates more weightage given to
                        average value compared to instantaneous value. */

int dlb_ecmp_egress_bytes_decrease_reset = 0;
                    /* Used in logic of EMWA calculation. It's a boolean (0 or 1)
                       If set, the historical member load is reset to the
                       instantaneous value if the latter is smaller.
                       Always preferably set it to 0 */
int dlb_ecmp_queued_bytes_decrease_reset = 0;
                    /* Used in logic of EMWA calculation. It's a boolean (0 or 1)
                       If set, the historical queued bytes is reset to the
                       instantaneous value if the latter is smaller.
                       Always preferably set it to 0 */
int dlb_ecmp_physical_queued_decrease_reset = 0;
                    /* Used in logic of EMWA calculation. It's a boolean (0 or 1)
                       If set, the historical ITM queued bytes is reset to the
                       instantaneous value if the latter is smaller.
                       Always preferably set it to 0 */

/* The load determined by EWMA calculations is quantized to one of 8 bands of
   Quality on each egress member port. The following parameters are used to
   tune the Quantization range*/
int dlb_ecmp_egress_bytes_min_threshold = 500;
int dlb_ecmp_egress_bytes_max_threshold = 9000;
                    /* The min and max values of Average Port loading used to
                        determine the Average Port loading Quality band.
                        Unit is Mbps.
                        Example: Min = 500 Mbps, Max = 9000 Mbps means,
                        for a 10G port, Port loading quantized average Quality
                        is 7 if egress traffic rate is below 500 Mbps.
                        Quality is 0 if rate is above 9 Gbps.

                        NOTE: These values should be set considering an egress
                        port speed of 10G. For other speeds, DLB hardware itself
                        will use the Scaling factor to upscale the value.
                        For example, Min=500 Mbps means for 100G port, the min
                        is considered by hardware as 5000 Mbps */

int dlb_ecmp_queued_bytes_min_threshold = 0x6632;
int dlb_ecmp_queued_bytes_max_threshold = 0x22E9A;
                    /* The min and max values of Average Total Port Queue size
                        used to determining the Total Queue size Quality band.
                        Unit is Bytes.
                        Ideally, it should be set in terms of
                        (no_of_cells * per_cell_size) where per_cell_size depends
                        on a given chip's MMU architecture. (On TH5, its 254 bytes)
                        Example: Min = 0x6632 (103 cells * 254 bytes)
                                 Max = 0x22E9A (563 cells * 254 bytes)
                        If Average queue size is less than 103 cells, total port
                        size average Quality is 7. If it exceeds 563 cells,
                        Quality is 0 */

int dlb_ecmp_physical_queued_bytes_min_threshold = 0xCC64;
int dlb_ecmp_physical_queued_bytes_max_threshold = 0x116CE;
                    /* The min and max values of Average ITM Port Queue size
                        used to determining the ITM Queue size Quality band.
                        Unit is Bytes.
                        Same explanation as of Total Port Queue size apply. */


int ecmp_dlb_eligible_ethertypes[] = {0x0800, 0x86DD};
                    /* Ethertypes that are eligible for DLB can be configured
                        at a system level. Only those packets egressing DLB
                        ECMP groups with these eligible Ethertypes will undergo
                        DLB. Other ethertypes use static hashing. */

/* DLB per-ECMP member Parameters */
int dlb_scaling_factor[NO_OF_ECMP_MEMBERS] = {100, 100, 100};
                    /* Determines scaling factor to be used on each egress member
                       port.
                       Value should be equal to egress port speed.
                       e.g,, 10G port -> Scaling factor = 10
                             100G port -> Scaling factor = 100
                             400G port -> Scaling factor = 400
                       NOTE: Actual hardware scaling factor value mapping will
                       be done by SDK */

/* The Quantized average
   (Port loading + Total port Queue size  + ITM Port Queue size) Qualities will
   be used in determining final Quality of a given egress port.
   The weightage given to each of the above 3 factors in determining final
   quality is configurable as below */
int dlb_port_loading_weight[NO_OF_ECMP_MEMBERS] = {70, 70, 70};
                    /* Weightage for Average port loading Quality on each egress
                       member port in determining final quality.
                       Unit is percentage.
                       Example, 70 -> 70% weightage for Port loading */
int dlb_queue_size_weight[NO_OF_ECMP_MEMBERS] = {20, 20, 20};
                    /* Weightage for Total port queue size Quality on each egress
                       member port in determining final quality.
                       Unit is percentage.
                       Example, 20 -> 20% weightage for Total port queue size */

/* NOTE: There is no separate knob for ITM port queue size weightage.
   The ITM port queue size weight
        = (100 - port loading weight percent - total queue size weight percent)
   Example, Port loading avg quality weight = 70%,
            Total queue size ave quality weight = 20%
            implies
            ITM port queue size avg quality weight = 10% */

/* DLB per-ECMP group Parameters */
int dlb_ecmp_dynamic_size = 1024;
                    /* Flowset table size. Unit is No of entries. */
int dlb_ecmp_dynamic_age = 256;
                    /* Inactivity duration. Unit is usec.
                       Indicates the duration for which an incoming macro flow
                       should be idle to consider it to be reassigned to another
                       ECMP member port */
int dlb_ecmp_max_paths = 64;
                    /* ECMP maximum paths. */


/*** L3 Route setup parameters ***/
bcm_vlan_t ingress_vlan[] = {400, 500, 600};
bcm_vlan_t egress_vlan[NO_OF_ECMP_MEMBERS] = {10, 20, 30};

bcm_vrf_t vrf = 1;
bcm_mac_t station_mac = "00:00:00:00:00:01";

bcm_mac_t src_mac[NO_OF_ECMP_MEMBERS] = {
    "00:00:00:00:00:01",
    "00:00:00:00:00:02",
    "00:00:00:00:00:03",
};

bcm_mac_t dst_mac[NO_OF_ECMP_MEMBERS] = {
    "00:00:00:00:00:11",
    "00:00:00:00:00:22",
    "00:00:00:00:00:33",
};

bcm_ip_t dst_ip_route = 0x0a0a0000; /* L3 route network address*/
bcm_ip_t dst_ip_mask  = 0xffff0000; /* Subnet mask */

/*** L2 Stream flow setup parameters ***/
bcm_vlan_t l2_flow_vlan = ingress_vlan[0];
bcm_port_t l2_flow_egress_port = egress_port[NO_OF_ECMP_MEMBERS - 1];
bcm_mac_t l2_flow_dst_mac = "00:00:00:00:01:23";

uint32_t egr_port_stat_counter_id; /*Flex counter action ID for egress ports */

/*** Traffic streams to be used in the test as hex strings ***/
char *l2_stream = "0000000001230000000004560B04000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

char *ipv4_stream_1 = "00000000000100000000000A08004500004E0000000040113E64141414140A0A0A0A15B30019003AAD720000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

char *ipv4_stream_2 = "00000000000100000000000B08004500004E0000000040113E5A1414141E0A0A0A0A15B40019003AAD670000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

/******************************************************************************/
/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
int test_setup(int unit)
{
	bcm_error_t rv;
	int i=0;

    for(i = 0; i < sizeof(ingress_port)/sizeof(ingress_port[0]);i++) {
        rv = ingress_port_setup(unit, ingress_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("ingress_port_setup() failed.\n");
            return rv;
        }
    }

    rv = egress_port_multi_setup(unit, egress_port, NO_OF_ECMP_MEMBERS);
    if (BCM_FAILURE(rv)) {
        printf("egress_port_multi_setup() failed.\n");
        return rv;
    }
	return rv;
}

int ingress_port_setup(int unit, bcm_port_t ing_port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port, BCM_PORT_LOOPBACK_PHY));
    return BCM_E_NONE;
}
/* Configures the port in loopback mode and installs an IFP rule to drop the 
 * packets on that port. This is to avoid continuous loopback of the packet 
 * from the egress port.
 *
 * NOTE: For DLB test, egress ports MUST BE in PHY loopback mode, not MAC loopback.
 */
int egress_port_multi_setup(int unit, bcm_port_t *egr_port, int no_of_egr_ports)
{
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    int i = 0;

    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    for(i = 0; i < no_of_egr_ports; i++) {
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port[i], BCM_PORT_LOOPBACK_PHY));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, egr_port[i], BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}


/* Configure RTAG7 settings for DLB ECMP hashing to work */
int config_rtag7_dlb_ecmp(int unit)
{
    int hash_control;

    /*** RTAG7 Parameters ***/
    // uint32 seed = 0x11111111;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed0, seed));
    /* Seed used in RTAG7 hashing */
    // int hash_computation = BCM_HASH_FIELD_CONFIG_CRC32HI;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config,
                                                        hash_computation));
    /* int ipv4_select_fields = BCM_HASH_EXT_FIELD_IP4_ADDRESS |
                         BCM_HASH_EXT_FIELD_L4_DST_PORT |
                         BCM_HASH_EXT_FIELD_L4_SRC_PORT; */
                        /* IPv4 packet fields to be used in hashing.
                            Same setting is applied for both TCP/UDP as well as
                            plain IPv4 packets, only relevant flags will be
                            considered by corresponding switch control */
    
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0,
                                                        ipv4_select_fields));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0,
                                                        ipv4_select_fields));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4Field0,
                                                        ipv4_select_fields));
/* int pre_process_enable = 1; */
                        /* Enable/disable pre-processing of bins */
                        /* 1 = Enable */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable,
                                                        pre_process_enable));
    /* 
int use_flow_based_hash_ecmp = 0;
    */
                        /* Use flow-based or port-based hashing */
                        /* 0 = Use port based hashing,
                                i.e., the block and offset to be used
                                in member selection is fixed per ingress port */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp,
                                                        use_flow_based_hash_ecmp));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmpDynamic,
                                                        use_flow_based_hash_ecmp));
// int block_n_offset_select = 0;
                        /* RTAG7 hash block and offset selection */
                        /* 0 = Use A_0 hash with offset of 0 */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchECMPHashSet0Offset,
                                                        block_n_offset_select));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchEcmpDynamicHashOffset,
                                                        block_n_offset_select));

    /*Enable RTAG7 for ECMP */
    BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashControl,
                                                        &hash_control));
    hash_control  |= BCM_HASH_CONTROL_ECMP_ENHANCE;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashControl,
                                                        hash_control));

    return BCM_E_NONE;
}

/* Configure Global DLB parameters used in DLB calculations */
int config_global_dlb_ecmp(int unit) {
    /* Sample Rate */
/* dlb global parameters */
// int dlb_ecmp_sampling_rate = 200000;
                    /* the rate at which the dlb calculations happen in hardware
                        a high rate means a low sampling period, which in turn
                        means the changes in load on ecmp members are quickly
                        accounted for in member quality calculations. */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit, bcmSwitchEcmpDynamicSampleRate,
                                        dlb_ecmp_sampling_rate));

/* DLB engine performs Exponentially Weighted Moving Average (EWMA)
   calculations at sampling rate to determine the load on a member port.
   The following parameters are used to tune that moving average */
// int dlb_ecmp_egress_bytes_exponent = 2;
    /* DLB Quantization parameters */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesExponent,
                                        dlb_ecmp_egress_bytes_exponent));
//int dlb_ecmp_egress_bytes_decrease_reset = 0;
                    /* Used in logic of EMWA calculation. It's a boolean (0 or 1)
                       If set, the historical member load is reset to the
                       instantaneous value if the latter is smaller.
                       Always preferably set it to 0 */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesDecreaseReset,
                                        dlb_ecmp_egress_bytes_decrease_reset));
// int dlb_ecmp_queued_bytes_exponent = 2;
                    /* Weight value used in Total port queue size EWMA calculation
                        Higher value indicates more weightage given to
                        average value compared to instantaneous value. */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesExponent,
                                        dlb_ecmp_queued_bytes_exponent));
// int dlb_ecmp_queued_bytes_decrease_reset = 0;
                    /* Used in logic of EMWA calculation. It's a boolean (0 or 1)
                       If set, the historical queued bytes is reset to the
                       instantaneous value if the latter is smaller.
                       Always preferably set it to 0 */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesDecreaseReset,
                                        dlb_ecmp_queued_bytes_decrease_reset));
// int dlb_ecmp_physical_queued_bytes_exponent = 2;
                    /* Weight value used in ITM port queue size EWMA calculation
                        Higher value indicates more weightage given to
                        average value compared to instantaneous value. */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent,
                                        dlb_ecmp_physical_queued_bytes_exponent));
// int dlb_ecmp_physical_queued_decrease_reset = 0;
                    /* Used in logic of EMWA calculation. It's a boolean (0 or 1)
                       If set, the historical ITM queued bytes is reset to the
                       instantaneous value if the latter is smaller.
                       Always preferably set it to 0 */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset,
                                        dlb_ecmp_physical_queued_decrease_reset));

/* The load determined by EWMA calculations is quantized to one of 8 bands of
   Quality on each egress member port. The following parameters are used to
   tune the Quantization range*/
// int dlb_ecmp_egress_bytes_min_threshold = 500;
// int dlb_ecmp_egress_bytes_max_threshold = 9000;
                    /* The min and max values of Average Port loading used to
                        determine the Average Port loading Quality band.
                        Unit is Mbps.
                        Example: Min = 500 Mbps, Max = 9000 Mbps means,
                        for a 10G port, Port loading quantized average Quality
                        is 7 if egress traffic rate is below 500 Mbps.
                        Quality is 0 if rate is above 9 Gbps.

                        NOTE: These values should be set considering an egress
                        port speed of 10G. For other speeds, DLB hardware itself
                        will use the Scaling factor to upscale the value.
                        For example, Min=500 Mbps means for 100G port, the min
                        is considered by hardware as 5000 Mbps */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesMinThreshold,
                                        dlb_ecmp_egress_bytes_min_threshold));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesMaxThreshold,
                                        dlb_ecmp_egress_bytes_max_threshold));
// int dlb_ecmp_queued_bytes_min_threshold = 0x6632;
// int dlb_ecmp_queued_bytes_max_threshold = 0x22E9A;
                    /* The min and max values of Average Total Port Queue size
                        used to determining the Total Queue size Quality band.
                        Unit is Bytes.
                        Ideally, it should be set in terms of
                        (no_of_cells * per_cell_size) where per_cell_size depends
                        on a given chip's MMU architecture. (On TH5, its 254 bytes)
                        Example: Min = 0x6632 (103 cells * 254 bytes)
                                 Max = 0x22E9A (563 cells * 254 bytes)
                        If Average queue size is less than 103 cells, total port
                        size average Quality is 7. If it exceeds 563 cells,
                        Quality is 0 */

    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesMinThreshold,
                                        dlb_ecmp_queued_bytes_min_threshold));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesMaxThreshold,
                                        dlb_ecmp_queued_bytes_max_threshold));
// int dlb_ecmp_physical_queued_bytes_min_threshold = 0xCC64;
// int dlb_ecmp_physical_queued_bytes_max_threshold = 0x116CE;
                    /* The min and max values of Average ITM Port Queue size
                        used to determining the ITM Queue size Quality band.
                        Unit is Bytes.
                        Same explanation as of Total Port Queue size apply. */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold,
                                        dlb_ecmp_physical_queued_bytes_min_threshold));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold,
                                        dlb_ecmp_physical_queued_bytes_max_threshold));

// int dlb_ecmp_random_seed = 0x5555;
                    /* IF all DLB eligible ECMP members have equal quality
                        (generally, before start of any traffic), a new incoming
                        flow will be assigned randomly. This seed is used in
                        that Random number generation */
    /* Set the random seed */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit, bcmSwitchEcmpDynamicRandomSeed,
                                                    dlb_ecmp_random_seed));

// int ecmp_dlb_eligible_ethertypes[] = {0x0800, 0x86DD};
                    /* Ethertypes that are eligible for DLB can be configured
                        at a system level. Only those packets egressing DLB
                        ECMP groups with these eligible Ethertypes will undergo
                        DLB. Other ethertypes use static hashing. */
    /* Eligibility based on EtherType */
    BCM_IF_ERROR_RETURN(
        bcm_l3_egress_ecmp_ethertype_set(unit,
                                            BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE,
                                            (sizeof(ecmp_dlb_eligible_ethertypes)/sizeof(ecmp_dlb_eligible_ethertypes[0])),
                                            ecmp_dlb_eligible_ethertypes));

    return BCM_E_NONE;
}

/* Create vlan and add port to vlan */
int create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in creating vlan %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);

    return rv;
}

/* Create vlan and add port to vlan and untagged bitmap */
int create_vlan_add_port_also_untagged(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_PBMP_PORT_ADD(ubmp, port);
    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in creating vlan %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);

    return rv;
}
/* Configure my station tcam to enable L3 route lookup */
int
config_my_station_tcam(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_port_t src_port)
{
    bcm_error_t rv;
    bcm_mac_t mac_mask={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_l2_station_t l2_station;
    int station_id;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    sal_memcpy(l2_station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    l2_station.src_port = src_port;
    l2_station.src_port_mask = src_port;
    l2_station.vlan = vlan;
    l2_station.vlan_mask = vlan;
    l2_station.flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;

    rv = bcm_l2_station_add(unit, &station_id, &l2_station);
    return rv;
}

/* Create L3 Ingress Interface */
int create_l3_ing_interface(int unit, bcm_vrf_t vrf, bcm_if_t ingress_if)
{
    bcm_error_t rv;
    bcm_l3_ingress_t l3_ingress;

    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ingress.vrf   = vrf;

    rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);

    return rv;
}

/* Config vlan_id to l3_iif mapping */
int config_l3_iif_binding(int unit, bcm_vlan_t vlan, bcm_if_t ingress_if)
{
    bcm_error_t rv;
    bcm_vlan_control_vlan_t vlan_ctrl;

    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vlan, &vlan_ctrl));
    vlan_ctrl.ingress_if = ingress_if;

    rv = bcm_vlan_control_vlan_set(unit, vlan, vlan_ctrl);

    return rv;
}

/* Create L3 interface */
int
create_l3_egr_interface(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t *l3_intf_id)
{
    bcm_error_t rv;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));

    rv = bcm_l3_intf_create(unit, &l3_intf);
    *l3_intf_id = l3_intf.l3a_intf_id;

    return rv;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport,
               bcm_if_t *egr_if)
{
    bcm_error_t rv;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));

    rv = bcm_l3_egress_create(unit, 0, &l3_egr, egr_if);

    return rv;
}

/* Configure DLB attributes on a member egress port */
int config_dlb_egr_port_attributes(int unit, bcm_port_t egr_port,
                                    int scaling_factor,
                                    int load_weight,
                                    int queue_size_weight)
{
    bcm_error_t rv;

    bcm_l3_ecmp_dlb_port_quality_attr_t quality_attr;
    bcm_l3_ecmp_dlb_port_quality_attr_t_init(&quality_attr);
    quality_attr.scaling_factor = scaling_factor;
    quality_attr.load_weight = load_weight;
    quality_attr.queue_size_weight = queue_size_weight;
    rv = bcm_l3_ecmp_dlb_port_quality_attr_set(unit, egr_port, &quality_attr);

    return rv;
}


/* Create ECMP group in DLB mode */
int create_dlb_ecmp_group(int unit, bcm_if_t *egr_obj_id, int no_of_members,
                            uint32 dynamic_mode, uint32 dynamic_size,
                            uint32 dynamic_age, uint32 max_paths,
                            bcm_if_t *ecmp_group_id) {
    bcm_error_t rv;
    int i = 0;
    /* Level 2 ECMP group */
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    ecmp_info.dynamic_mode = dynamic_mode;
    ecmp_info.dynamic_size = dynamic_size;
    ecmp_info.dynamic_age = dynamic_age;

    uint32 flags = 0;
    bcm_l3_ecmp_member_t ecmp_member_array[no_of_members];
    for(i = 0; i < no_of_members; i++) {
        bcm_l3_ecmp_member_t_init(&ecmp_member_array[i]);
        ecmp_member_array[i].egress_if = egr_obj_id[i];
    }
    rv = bcm_l3_ecmp_create(unit, flags, &ecmp_info, no_of_members, ecmp_member_array);
    *ecmp_group_id = ecmp_info.ecmp_intf;

    return rv;
}

/* Set DLB egress member status */
/* Determines if a given egress member (port) should be considered as link "UP"
 * or "DOWN" in DLB calculations.
 * BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_DOWN
 *      = Consider link status as "Down" and don't use the egress port in
 *        DLB reassignments.
 * BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_UP
 *      = Consider link status as "UP" and use the egress port in
 *        DLB reassignments.
 * BCM_L3_ECMP_DYNAMIC_MEMBER_HW
 *      = Let hardware link status determine the eligibility.
 *        If port is "UP", use it in DLB. If port is "Down", don't use.
 * 
 * NOTE: Even though API takes egress object as parameter, the final setting is
 * done on corresponding egress port basis.
 */
int set_dlb_member_status(int unit, bcm_if_t egr_obj_id, int status) {
    return bcm_l3_egress_ecmp_member_status_set(unit, egr_obj_id, status);
}

/* Create L3 route entry */
int
create_l3_route(int unit, bcm_ip_t subnet_addr, bcm_ip_t subnet_mask,
                            bcm_vrf_t vrf, bcm_if_t ecmp_group_id)
{
    bcm_error_t  rv;
    bcm_l3_route_t route;

    bcm_l3_route_t_init(&route);
    route.l3a_flags = BCM_L3_MULTIPATH;
    route.l3a_subnet  = subnet_addr;
    route.l3a_ip_mask = subnet_mask;
    route.l3a_intf = ecmp_group_id;
    route.l3a_vrf = vrf;
    rv = bcm_l3_route_add(unit, &route);

    return rv;
}

/* Create L2 table entry */
int create_l2_entry(int  unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac, vlan);

    l2addr.port = port;
    l2addr.flags |= BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, l2addr);

    return rv;
}

/* Main function - Configure DLB ECMP functionality on the system */
bcm_error_t
configure_dlb_ecmp(int unit)
{
	bcm_error_t rv;
	int flags, i;
    bcm_if_t ingress_if = 0x1b2;
    bcm_if_t l3_intf_id[NO_OF_ECMP_MEMBERS];
    bcm_if_t egr_obj_id[NO_OF_ECMP_MEMBERS];

    bcm_if_t ecmp_group_id;

    rv = config_rtag7_dlb_ecmp(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_rtag7_dlb_ecmp(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Global DLB Configuraton */
    rv = config_global_dlb_ecmp(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_global_dlb_ecmp(): %s.\n", bcm_errmsg(rv));
        return rv;
	}

    printf("\n VLAN configuration...");
    for(i = 0; i < sizeof(ingress_vlan)/sizeof(ingress_vlan[0]); i++) {
        rv = create_vlan_add_port(unit, ingress_vlan[i], ingress_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("\nError in bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    for(i = 0; i < NO_OF_ECMP_MEMBERS; i++) {
        /* Create and add ingress port to ingress VLAN */
        rv = create_vlan_add_port(unit, egress_vlan[i], egress_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    for(i = 0; i < sizeof(ingress_port)/sizeof(ingress_port[0]); i++) {
        rv = config_my_station_tcam(unit, station_mac, ingress_vlan[i], ingress_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("\nError in config_my_station_tcam(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Create L3 ingress interface */
    rv = create_l3_ing_interface(unit, vrf, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_ing_interface(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* VLAN to L3_IIF mapping */
    for(i = 0; i < sizeof(ingress_vlan)/sizeof(ingress_vlan[0]); i++) {
        rv = config_l3_iif_binding(unit, ingress_vlan[i], ingress_if);
        if (BCM_FAILURE(rv)) {
            printf("\nError in config_l3_iif_binding(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    printf("\n L3 egress configuration...");
    for(i = 0; i < NO_OF_ECMP_MEMBERS; i++) {
        /* Create egress L3 interface */
        rv = create_l3_egr_interface(unit, src_mac[i], egress_vlan[i], &l3_intf_id[i]);
        if (BCM_FAILURE(rv)) {
            printf("\nError in create_l3_interface(): %s.\n", bcm_errmsg(rv));
            return rv;
        }

        /* Create L3 egress object */
        rv = create_egr_obj(unit, l3_intf_id[i], dst_mac[i], egress_port[i], &egr_obj_id[i]);
        if (BCM_FAILURE(rv)) {
            printf("\nError in create_egr_obj(): %s.\n", bcm_errmsg(rv));
            return rv;
        }

/* The Quantized average
   (Port loading + Total port Queue size  + ITM Port Queue size) Qualities will
   be used in determining final Quality of a given egress port.
   The weightage given to each of the above 3 factors in determining final
   quality is configurable as below */
// int dlb_port_loading_weight[NO_OF_ECMP_MEMBERS] = {70, 70, 70};
                    /* Weightage for Average port loading Quality on each egress
                       member port in determining final quality.
                       Unit is percentage.
                       Example, 70 -> 70% weightage for Port loading */
// int dlb_queue_size_weight[NO_OF_ECMP_MEMBERS] = {20, 20, 20};
                    /* Weightage for Total port queue size Quality on each egress
                       member port in determining final quality.
                       Unit is percentage.
                       Example, 20 -> 20% weightage for Total port queue size */
        rv = config_dlb_egr_port_attributes(unit, egress_port[i],
                                                dlb_scaling_factor[i],
                                                dlb_port_loading_weight[i],
                                                dlb_queue_size_weight[i]);
        if (BCM_FAILURE(rv)) {
            printf("\nError in config_dlb_egr_port_attributes(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
        printf("\n L3 intf = [%d] Egress Object ID = [%d]", l3_intf_id[i], egr_obj_id[i]);
    }

/* NOTE: There is no separate knob for ITM port queue size weightage.
   The ITM port queue size weight
        = (100 - port loading weight percent - total queue size weight percent)
   Example, Port loading avg quality weight = 70%,
            Total queue size ave quality weight = 20%
            implies
            ITM port queue size avg quality weight = 10% */

/* DLB per-ECMP group Parameters */
// int dlb_ecmp_dynamic_size = 1024;
                    /* Flowset table size. Unit is No of entries. */
// int dlb_ecmp_dynamic_age = 256;
                    /* Inactivity duration. Unit is usec.
                       Indicates the duration for which an incoming macro flow
                       should be idle to consider it to be reassigned to another
                       ECMP member port */
// int dlb_ecmp_max_paths = 64;
                    /* ECMP maximum paths. */

    printf("\n L3 ECMP group configuration...");
    rv = create_dlb_ecmp_group(unit, egr_obj_id, NO_OF_ECMP_MEMBERS,
                                BCM_L3_ECMP_DYNAMIC_MODE_NORMAL,
                                dlb_ecmp_dynamic_size, dlb_ecmp_dynamic_age,
                                dlb_ecmp_max_paths, &ecmp_group_id);
    if (BCM_FAILURE(rv)) {
            printf("\nError in create_dlb_ecmp_group(): %s.\n", bcm_errmsg(rv));
            return rv;
    }

    for(i = 0; i < NO_OF_ECMP_MEMBERS; i++) {
        rv = set_dlb_member_status(unit, egr_obj_id[i], BCM_L3_ECMP_DYNAMIC_MEMBER_HW);
        if (BCM_FAILURE(rv)) {
            printf("\nError in set_dlb_member_status(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }
    printf("\n L3 ECMP group ID = [%d]", ecmp_group_id);
    printf("\n L3 Route configuration...\n");
    rv = create_l3_route(unit, dst_ip_route, dst_ip_mask, vrf, ecmp_group_id);
    if (BCM_FAILURE(rv)) {
        printf("Error executing configure_route(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* create egress flex counters and attach to Egress ports */
	rv = config_egr_port_flexctr(unit, NO_OF_ECMP_MEMBERS, egress_port, &egr_port_stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("ERROR: config_egr_port_flexctr() : %s.\n", bcm_errmsg(rv));
       return rv;
    }
	return rv;
}

/* Configure Traffic Generator*/
int config_traffic_generator(int unit, bcm_vlan_t rsvd_vlan, bcm_port_t ing_port, bcm_port_t *rsvd_port, int no_of_rsvd_ports) {
    bcm_error_t rv;
    int i = 0;
    printf("\n Ingress port add to reserved VLAN...\n");
    printf("Ingress port = %d\n", ing_port);
    rv = create_vlan_add_port_also_untagged(unit, rsvd_vlan, ing_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_untagged_vlan_set(unit, ing_port, rsvd_vlan);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_untagged_vlan_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_learn_set(unit, ing_port, BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_learn_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n Reserved ports add to reserved VLAN...\n");
    for(i = 0; i < no_of_rsvd_ports; i++) {
        printf("Rsvd port %d = %d\n", i + 1, rsvd_port[i]);
        rv = ingress_port_setup(unit, rsvd_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("ingress_port_setup() failed.\n");
            return rv;
        }
        rv = create_vlan_add_port_also_untagged(unit, rsvd_vlan, rsvd_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
		rv = bcm_port_untagged_vlan_set(unit, rsvd_port[i], rsvd_vlan);
        if (BCM_FAILURE(rv)) {
            printf("\nError in bcm_port_untagged_vlan_set(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
		rv = bcm_port_learn_set(unit, rsvd_port[i], BCM_PORT_LEARN_FWD);
        if (BCM_FAILURE(rv)) {
            printf("\nError in bcm_port_learn_set(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }
    bshell(unit, "sleep quiet 5");
    return rv;
}

/* Configure L2 entry for L2 unicast flow */
int config_l2_flow_path(int unit) {
    bcm_error_t rv;
    printf("\n L2 flow path configuration...\n");

    rv = create_vlan_add_port(unit, l2_flow_vlan, l2_flow_egress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = create_l2_entry(unit, l2_flow_dst_mac, l2_flow_vlan, l2_flow_egress_port);
    if (BCM_FAILURE(rv)) {
        printf("Error executing create_l2_entry(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int config_egr_port_flexctr(int unit, int port_num, bcm_gport_t *egr_port, uint32 *stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;
	int i = 0;

    action.index_num = 20;
    action.source = bcmFlexctrSourceEgrPort;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;

    /* Counter index is always zero. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrEgressPort;
    index_op->mask_size[0] = 9;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 16;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    rv = bcm_flexctr_action_create(unit, options, &action, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Created flexctr stat_counter_id = 0x%x.\n", *stat_counter_id);

	for(i=0;i<port_num;i++){
    rv = bcm_port_stat_attach(unit, egr_port[i], *stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_port_stat_attach() -  %s.\n", bcm_errmsg(rv));
    }
    printf("Flexctr stat_counter_id:0x%x attached to egress port %d\n", *stat_counter_id, egr_port[i]);
	}

    return rv;
}

/* Get traffic metrics and DLB Quality metrics when traffic is running */
int get_and_check_traffic_stats(int unit, int no_of_streams_running) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_flexctr_counter_value_t egr_port_counter_val_run_1[20], egr_port_counter_val_run_2[20];
    uint32 counter_index[20];
    int i = 0;
    bcm_l3_ecmp_dlb_port_quality_t port_quality[NO_OF_ECMP_MEMBERS];

    for (i = 0; i < 20; i++){
        counter_index[i] = i;
    }

    /* Send traffic streams as first run and capture the egress port stats */
    sal_memset(egr_port_counter_val_run_1, 0, sizeof(egr_port_counter_val_run_1));
    rv = bcm_flexctr_stat_get(unit,
                              egr_port_stat_counter_id,
                              20,
                              counter_index,
                              egr_port_counter_val_run_1);
    if (BCM_FAILURE(rv)) {
        printf("Egress bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    bshell(unit, "sleep quiet 3");
    rv = bcm_flexctr_stat_get(unit,
                              egr_port_stat_counter_id,
                              20,
                              counter_index,
                              egr_port_counter_val_run_2);
    if (BCM_FAILURE(rv)) {
        printf("Egress bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Compare the stats captured in both runs
     *  DLB -> Stats should be different
     *  Static hashing -> Stats should be the same
     */
    printf("\n\nNo. of packets egressing on each egress port:");
    printf("\n------------------------------");
    printf("\nport       Run-1         Run-2");
    printf("\n------------------------------");
    for(i = 0; i < NO_OF_ECMP_MEMBERS; i++) {
        printf("\n%3d %12u  %12u",egress_port[i],
                    COMPILER_64_LO(egr_port_counter_val_run_1[egress_port[i]].value[0]),
                    COMPILER_64_LO(egr_port_counter_val_run_2[egress_port[i]].value[0]));
    }
    printf("\n");

    printf("\nDLB Quality on egress ports");
    printf("\n Port   Quality");
    printf("\n --------------");
    for(i = 0; i < NO_OF_ECMP_MEMBERS; i++) {
        port_quality[i].port = egress_port[i];
        port_quality[i].buffer_id = 0;
        rv = bcm_l3_ecmp_dlb_port_quality_get(unit, &port_quality[i]);
        if (BCM_FAILURE(rv)) {
            printf("\nError in bcm_l3_ecmp_dlb_port_quality_get(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
        printf("\n%3d%7d", egress_port[i], port_quality[i].quality);
                /* Check the quality on each Egress member port */
    }
    printf("\n");

    /* Verification */
    int no_of_egr_ports_sending_traffic = 0;
    for(i = 0; i < NO_OF_ECMP_MEMBERS; i++) {
        if(COMPILER_64_LO(egr_port_counter_val_run_1[egress_port[i]].value[0]) !=
            COMPILER_64_LO(egr_port_counter_val_run_2[egress_port[i]].value[0])) {
                no_of_egr_ports_sending_traffic++;
                    /* Packet counter incrementing - port sending traffic */
        }
    }

    if(no_of_streams_running > no_of_egr_ports_sending_traffic) {
        printf("\nNot Working: %d streams occupy %d egress ports\n",
                                                no_of_streams_running,
                                                no_of_egr_ports_sending_traffic);
        return -1;
    }
    else {
        printf("\nWorking: %d streams occupy %d egress ports\n",
                                                no_of_streams_running,
                                                no_of_egr_ports_sending_traffic);
    }

    // printf("\n ~~~ Traffic on Ingress ports ~~~\n");
    // for(i = 0; i < sizeof(ingress_port)/sizeof(ingress_port[0]); i++) {
    //     snprintf(str, 1000, "show c c s r %d", ingress_port[i]);
    //     bshell(unit, str);
    // }
    // printf("\n ~~~ Traffic on Egress ECMP Member ports ~~~\n");
    // for(i = 0; i < NO_OF_ECMP_MEMBERS; i++) {
    //     snprintf(str, 1000, "show c c s r %d", egress_port[i]);
    //     bshell(unit, str);
    // }
    return rv;
}

/* Start the traffic by starting loopback traffic to actual ingress port and vlan */
int start_traffic_generator(int unit, bcm_port_t traffic_port, bcm_vlan_t vlan,
                                  uint32 rate_kbps, uint32 burst_kb,
                                  char *traffic_stream) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;


    char str[512];
    snprintf(str, 512, "tx 1 pbm=%d data=%s", traffic_port, traffic_stream);
    printf("%s\n", str);
    bshell(unit, str);

    rv = bcm_port_rate_egress_set(unit, traffic_port, rate_kbps, burst_kb);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_rate_egress_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

	rv = bcm_port_untagged_vlan_set(unit, traffic_port, vlan);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_untagged_vlan_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Port VLAN configuration */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);

    BCM_PBMP_PORT_ADD(pbmp, traffic_port);
    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Main verify function to perform the traffic test */
int test_verify(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int no_of_streams_running = 0;

    bshell(unit, "sleep quiet 2");
    /* Configuration show */
    printf("\n -------------------");
    printf("\n Configuration show:");
    printf("\n -------------------");
    bshell(unit, "e; e ~~~; e ps ce0-ce12; e ~~~; ps ce0-ce12");
    bshell(unit, "e; e ~~~; e vlan show; e ~~~; vlan show");
    bshell(unit, "e; e ~~~; e l2 show; e ~~~; l2 show");
    bshell(unit, "e; e ~~~; e l3 ing_intf show; e ~~~; l3 ing_intf show");
    bshell(unit, "e; e ~~~; e l3 intf show; e ~~~; l3 intf show");
    bshell(unit, "e; e ~~~; e l3 egress show; e ~~~; l3 egress show");
    bshell(unit, "e; e ~~~; e l3 ecmp show; e ~~~; l3 ecmp show");
    bshell(unit, "e; e ~~~; e l3 route show; e ~~~; l3 route show");


    printf("\n -------------");
    printf("\n Traffic Test:");
    printf("\n -------------");

    /* L2 stream */
    printf("\n Configure traffic generator for L2 Stream:\n");
    rv = config_traffic_generator(unit, traffic_rsvd_vlan[0], ingress_port[0], traffic_rsvd_ports_1, 2);
    if (BCM_FAILURE(rv)) {
        printf("Error executing config_traffic_generator(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("\n Start the traffic generator with L2 traffic:\n");
    printf("Traffic gen port   : %d\n", ingress_port[0]);
    printf("Ingress VLAN       : %d\n", l2_flow_vlan);
    printf("Rate of traffic    : 20 %% of Line rate\n");
    printf("Packet data    :\n%s\n", l2_stream);
    rv = start_traffic_generator(unit, ingress_port[0],
                                        l2_flow_vlan,
                                        20000000,
                                        1000,
                                        l2_stream);
    if (BCM_FAILURE(rv)) {
        printf("Error executing start_traffic_generator(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    no_of_streams_running++;
    printf("\nGetting traffic information:\n");
    rv = get_and_check_traffic_stats(unit, no_of_streams_running);
    if (BCM_FAILURE(rv)) {
        printf("Error executing get_and_check_traffic_stats(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* IPv4 stream-1 destined to DLB ECMP group  */
    printf("\n Configure traffic generator for IPv4 stream 1:\n");
    rv = config_traffic_generator(unit, traffic_rsvd_vlan[1], ingress_port[1], traffic_rsvd_ports_2, 2);
    if (BCM_FAILURE(rv)) {
        printf("Error executing config_traffic_generator(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("\n Start the traffic generator with IPv4 stream 1:\n");
    printf("Traffic gen port   : %d\n", ingress_port[1]);
    printf("Ingress VLAN       : %d\n", ingress_vlan[1]);
    printf("Rate of traffic    : 50 %% of Line rate\n");
    printf("Packet data    :\n%s\n", ipv4_stream_1);
    rv = start_traffic_generator(unit, ingress_port[1],
                                        ingress_vlan[1],
                                        50000000,
                                        1000,
                                        ipv4_stream_1);
    if (BCM_FAILURE(rv)) {
        printf("Error executing start_traffic_generator(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    no_of_streams_running++;

    printf("\nGetting traffic information:\n");
    rv = get_and_check_traffic_stats(unit, no_of_streams_running);
    if (BCM_FAILURE(rv)) {
        printf("Error executing get_and_check_traffic_stats(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* IPv4 stream-2 destined to DLB ECMP group  */
    printf("\n Configure traffic generator for IPv4 stream 2:\n");
    rv = config_traffic_generator(unit, traffic_rsvd_vlan[2], ingress_port[2], traffic_rsvd_ports_3, 2);
    if (BCM_FAILURE(rv)) {
        printf("Error executing config_traffic_generator(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("\n Start the traffic generator with IPv4 stream 2:\n");
    printf("Traffic gen port   : %d\n", ingress_port[2]);
    printf("Ingress VLAN       : %d\n", ingress_vlan[2]);
    printf("Rate of traffic    : 70 %% of Line rate\n");
    printf("Packet data    :\n%s\n", ipv4_stream_2);
    rv = start_traffic_generator(unit, ingress_port[2],
                                        ingress_vlan[2],
                                        70000000,
                                        1000,
                                        ipv4_stream_2);
    if (BCM_FAILURE(rv)) {
        printf("Error executing start_traffic_generator(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    no_of_streams_running++;
    printf("\nGetting traffic information:\n");
    rv = get_and_check_traffic_stats(unit, no_of_streams_running);
    if (BCM_FAILURE(rv)) {
        printf("Error executing get_and_check_traffic_stats(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) demonstrates the deployment for L3 route to DLB ECMP group with RTAG7 configure sequence
 *     (Done in configure_dlb_ecmp())
 *  c) demonstrates the functionality(done in test_verify()).
 */
bcm_error_t
execute(void)
{
    int unit = 0;
    bcm_error_t rv;

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2a) configure_dlb_ecmp(): ** start **";
    if (BCM_FAILURE((rv = configure_dlb_ecmp(unit)))) {
        printf("configure_dlb_ecmp() failed.\n");
        return rv;
    }
    print "~~~ #2a) configure_dlb_ecmp(): ** end **";

    print "~~~ #2b) config_l2_flow_path(): ** start **";
    if (BCM_FAILURE((rv = config_l2_flow_path(unit)))) {
        printf("config_l2_flow_path() failed.\n");
        return rv;
    }
    print "~~~ #2b) config_l2_flow_path(): ** end **";
    print "~~~ #3) test_verify(): ** start **";
    if (BCM_FAILURE((rv = test_verify(unit)))) {
        printf("test_verify() failed.\n");
        return rv;
    }
    print "~~~ #3) test_verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print "execute(): Start";
  print execute();
  print "execute(): End";
}
