/*
 * Feature  : DLB ECMP configuration and DLB parameter value recommendations on
 *            core switch in 'ML clusters data exchange' application.
 *
 * Usage    : BCM.0> cint th5_hsdk_l3_ecmp_dlb_customer_ml_cluster.c
 *
 * config   : bcm78900_b0-generic-320x100-l3.config.yml
 *
 * Log file : th5_hsdk_l3_ecmp_dlb_customer_ml_cluster_log.txt
 *
 * -----------
 * Description
 * -----------
 *  This cint script demonstrates the configuration of ECMP group in Dynamic
 * Load Balancing (DLB) mode on an XGS device that's part of core switch in
 * ML (Machine Learning) cluster topology.
 * It's a standard DLB ECMP configuration but specifies the DLB global
 * and per ECMP group parameter value ranges to be used for customer-specific
 * ML cluster application.
 *
 *  - In this ML cluster topology, a large number of  servers exchange 'learning'
 * data via the network in which the XGS device is part of.
 *  - These are connected via ECMP group with large number of members
 * (e.g., 16 members).
 *  - The data exchange happens in form of sudden bursts of large data at line
 * rates for a short duration.
 *  - it's N:N communication, i.e., from XGS device point of view, if ECMP has 8
 * members of 100G speeds, the ingress traffic will be from 8 100G ingress ports.
 *  - The Entropy in the packet data streams, i.e., the variation in packet fields
 * used for hashing and distribution is generally very less.
 * For example, L2 fields, SIP, DIP may be same among data streams.
 * Only L4 Src port may be varying.
 *  - Because of the above reasons, static hashing may not provide efficient
 * distribution of incoming streams of 8 x 100G rate among 8 x 100G ECMP members.
 *  - DLB 'tries' to provide a better distribution by assigning the new incoming
 * stream bursts to different ECMP members.
 *  - As all incoming streams start almost at the same time, short-lived, are
 * bursty, and low entropy, the DLB hardware calculation-related parameters
 * should be tuned in a specific way to provide the optimal performance.
 *
 * NOTE:
 * (1) A traffic generator is necessary to be connected to the device to run
 *     the traffic test.
 * (2) This script demos a minimal test where there's only one ingress port with
 *     8 incoming streams.
 *     An ideal test would need multiple ingress ports sending several streams of
 *     bursty traffic which start ingressing simultaneously.
 * (3) The DLB parameter values are just for reference. User may need to tune
 *     them according to their system requirements and expected performance during
 *     deployment.
 *
 * -------------------------------
 * Test Topology and traffic setup
 * -------------------------------
 *                         +----------------------------------------------+
 * +--->                   |                                              |
 * Ingress                 |                         ++Egr Obj            |
 * 8 Streams of            |                         | 100001  --   ce0(1)+---->
 * traffic                 |                         |                    |
 * @12.5% Linerate         |                         |                    |
 *  each                   |                         ++Egr Obj            |
 *                         |                         | 100002  --   ce1(2)+---->
 * Total = 100%            |                         |                    |
 *         linerate        |                         |                    |
 * Each packet             |                         ++Egr Obj            |
 *       = 1500 bytes      |                         | 100003  --   ce2(3)+---->
 *                         |                         |                    |
 *                         |          L3 Route       |                    |
 * Packet Details       +-->ce8(9 )   +------+       ++Egr Obj            |
 * --------------          |        IP=10.10.0.0/16  | 100004  --   ce3(4)+---->
 * DMAC=00:00:00:00:00:01  |        Dest=ECMP group ++                    |
 * SMAC=00:00:00:00:00:0A  |             203968      |                    |
 * VLAN=2                  |                         ++Egr Obj            |
 *                         |                         | 100005  --   ce4(5)+---->
 * SIP=20.20.20.20         |                         |                    |
 * DIP=10.10.10.10         |                         |                    |
 *                         |                         ++Egr Obj            |
 * L4 Dst Port=25          |                         | 100006  --   ce5(6)+---->
 * L4 Src Port=            |                         |                    |
 *   5555(0x15B3),         |                         |                    |
 *   5556(0x15B4),         |                         ++Egr Obj            |
 *   5557(0x15B5),         |                         | 100007  --   ce6(7)+---->
 *   5558(0x15B6),         |                         |                    |
 *   5559(0x15B7),         |                         |                    |
 *   5560(0x15B8),         |                         ++Egr Obj            |
 *   5561(0x15B9),         |                           100008  --   ce7(8)+---->
 *   5562(0x15BA)          |                                              |
 * (values in each of      |                                              |
 *     8 streams )         |              BCM78900 (TH5)                  |
 *                         +----------------------------------------------+
 * <PAYLOAD>
 *
 * ---------------
 * Stream details:
 * ---------------
 * Total 8 streams:
 * (Packet Length=1500 Bytes | 12.5% line rate each |ingress port ce8(9) )
 *      DMAC = 00:00:00:00:00:01
 *      SMAC = 00:00:00:00:00:0A
 *      VLAN = 2
 *          IPv4 SIP = 20.20.20.20
 *          IPv4 DIP = 10.10.10.10
 *          Protocol = UDP (17)
 *              UDP L4 Src Port = 5555, 5556, 5557, 5558
 *                                5559, 5560, 5561, 5562
 *                                (Only varying field among streams)
 *              UDP L4 Dst Port = 25
 *              <UDP Payload>
 *
 * -----------------------------------
 * Expected final traffic distribution
 * -----------------------------------
 *
 *                                                  One possible
 *                      +-------------------+       Distribution
 *                      |                   |       ------------
 *                      |                   |
 *                      |             ce0(1)+---->  +--->
 *                      |                   |       25 % line rate
 *                      |                   |
 *                      |                   |
 *                      |             ce1(2)+---->  +--->
 *                      |                   |       12.5% line rate
 *   +--->              |                   |
 *   Ingress            |                   |
 *   8 Streams of       |             ce2(3)+---->
 *   traffic            |                   |
 *   @12.5% Linerate    |                   |
 *    each           +-->ce8(9)             |
 *                      |             ce3(4)+---->  +--->
 *   Total = 100%       |                   |       12.5% line rate
 *           linerate   |                   |
 *   Each packet        |                   |
 *         = 1500 bytes |             ce4(5)+---->  +--->
 *                      |                   |       12.5% line rate
 *   All streams        |                   |
 *   started at once    |                   |
 *                      |             ce5(6)+---->  +--->
 *                      |                   |       12.5% line rate
 *                      |                   |
 *                      |                   |
 *                      |             ce6(7)+---->  +--->
 *                      |                   |       12.5% line rate
 *                      |                   |
 *                      |                   |
 *                      |             ce7(8)+---->  +--->
 *                      |                   |       12.5% line rate
 *                      |                   |
 *                      |   BCM78900 (TH5)  |
 *                      +-------------------+
 *
 * NOTE: The above is just an example. Because it's DLB, the final distribution
 * in each run may vary.
 * ---------------------------------------------
 * Detailed steps performed and expected results
 * ---------------------------------------------
 * STEP-1: Port setup (done in test_setup())
 *  - One ingress port ce8(9) should be connected to Traffic generator.
 *  - Egress ports ce0(1)-ce7(8) are configured to be working
 *    in PHY loopback mode. IFP rule is configured to drop loop backed packets.
 *
 * STEP-2: DLB ECMP configuration (done in configure_dlb_ecmp())
 *  - Configure RTAG7 for DLB ECMP distribution to work
 *  - Configure DLB global parameters for DLB hardware calculations to work
 *    The values are configured to suit ML cluster application.
 *  - Create VLAN, my station tcam and L3 ingress and egress interfaces
 *  - Create 3 L3 egress objects and set per port DLB attributes on egress ports
 *    The values are configured to suit ML cluster application.
 *  - Create DLB ECMP group.
 *    The ECMP group parameters are configured to suit ML cluster application.
 *  - Create L3 route with the above DLB ECMP group as a destination.
 *
 * STEP 3: Traffic test and verification (done in test_verify())
 * NOTE:
 *     After completing the configuration part, the cint script pauses
 *     for user to start the traffic stream and waits. User need to
 *     start the stream, and press 'C' to continue running the script, or press
 *     'Q' to stop the traffic test.
 *
 *  - Start 8 streams @ 12.5% line rate each (total ingress = 100% line rate)
 *    simultaneously.
 *  - Observe the 'show c r' output.
 *  -  Get the final DLB Quality on each egress port using
 *    bcm_l3_ecmp_dlb_port_quality_get API.
 *
 *  - Expected result:
 *      - 'show c r' should show that all egress ports are egressing traffic as a
 *      best possible scenario. Another possible scenario may be that, one or two
 *      egress ports may be egressing packets at a higher rate, while one or two
 *      ECMP member ports may not be egressing any traffic at all.
 *
 *      - Quality value may be reduced on egress ports based on traffic rate
 *      it is egressing out. (may become 6 or 5 or 4 reduced from maximum of 7)
 *
 *******************************************************************************
 */
cint_reset();
/************************ Configuration Parameters  ***************************/
/* Modify these values as per the traffic setup requirements.                 */

int const NO_OF_ECMP_MEMBERS = 8; /* No of members in the DLB ECMP group */
bcm_port_t ingress_port= 9; /* Ingress port where L3 traffic ingresses */
bcm_port_t egress_port[NO_OF_ECMP_MEMBERS] = {1, 2, 3, 4, 5, 6, 7, 8};
                        /* Egress ports that form members of DLB ECMP group */

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

/* DLB Global Parameters */
int dlb_ecmp_sampling_rate = 200000;
                    /* The rate at which the DLB calculations happen in hardware
                        A high rate means a low sampling period, which in turn
                        means the changes in load on ECMP members are quickly
                        accounted for in member quality calculations. */
                    /* For ML clusters application, this value should be as low
                       as possible. 200000 (equal to 5 us sampling period) is
                       recommended. */
int dlb_ecmp_random_seed = 0x5555;
                    /* IF all DLB eligible ECMP members have equal quality
                        (generally, before start of any traffic), a new incoming
                        flow will be assigned randomly. This seed is used in
                        that Random number generation */

/* DLB engine performs Exponentially Weighted Moving Average (EWMA)
   calculations at sampling rate to determine the load on a member port.
   The following parameters are used to tune that moving average */
int dlb_ecmp_egress_bytes_exponent = 1;
                    /* Weight value used in Port Loading EWMA calculation
                        Higher value indicates more weightage given to
                        average value compared to instantaneous value. */

                    /* For ML clusters application, this value should be as low
                       as possible. 0 or 1 is recommended. This ensures DLB
                       calculations quickly take instantaneous load into
                       account leading to quicker Quality changes and better
                       stream assignment */
int dlb_ecmp_queued_bytes_exponent = 1;
                    /* Weight value used in Total port queue size EWMA calculation
                        Higher value indicates more weightage given to
                        average value compared to instantaneous value. */
/* For ML clusters application, this value should be as low as possible. 0 or 1 is recommended. This ensures DLB calculations quickly take instantaneous load into account leading to quicker Quality changes and better stream assignment */
int dlb_ecmp_physical_queued_bytes_exponent = 1;
                    /* Weight value used in ITM port queue size EWMA calculation
                        Higher value indicates more weightage given to
                        average value compared to instantaneous value. */
/* For ML clusters application, this value should be as low as possible. 0 or 1 is recommended. This ensures DLB calculations quickly take instantaneous load into account leading to quicker Quality changes and better stream assignment */

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
int dlb_ecmp_egress_bytes_min_threshold = 100;
int dlb_ecmp_egress_bytes_max_threshold = 5000;
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
/* For ML clusters application, the min and max should be on the lower side: min = 1% and max = 50% of line rate.  This ensures a quick quality change on egress port when a stream starts ingressing and subsequent streams gets assigned to other available member ports noticing this quality change. */
int dlb_ecmp_queued_bytes_min_threshold = 0x6632;
int dlb_ecmp_queued_bytes_max_threshold = 0x22E9A;
                    /* The min and max values of Average Total Port Queue size
                        used to determine the Total Queue size Quality band.
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
                        used to determine the ITM Queue size Quality band.
                        Unit is Bytes.
                        Same explanation as of Total Port Queue size apply. */


int ecmp_dlb_eligible_ethertypes[] = {0x0800, 0x86DD};
                    /* Ethertypes that are eligible for DLB can be configured
                        at a system level. Only those packets egressing DLB
                        ECMP groups with these eligible Ethertypes will undergo
                        DLB. Other ethertypes use static hashing. */

/* DLB per-ECMP member Parameters */
int dlb_scaling_factor[NO_OF_ECMP_MEMBERS] = {10, 10, 10, 10, 10, 10, 10, 10};
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
int dlb_port_loading_weight[NO_OF_ECMP_MEMBERS] = {50, 50, 50, 50, 50, 50, 50, 50};
                    /* Weightage for Average port loading Quality on each egress
                       member port in determining final quality.
                       Unit is percentage.
                       Example, 70 -> 70% weightage for Port loading */

int dlb_queue_size_weight[NO_OF_ECMP_MEMBERS] = {25, 25, 25, 25, 25, 25, 25, 25};
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

                    /* For ML clusters application, it's recommended to give
                       equal weight for both Port loading and Port Queue size*/
/* DLB per-ECMP group Parameters */
int dlb_ecmp_dynamic_size = 16384;
                    /* Flowset table size. Unit is No of entries. */
/* For ML clusters application, this value should be as large as possible, as the entropy is already less in incoming packets. The ingress streams should occupy as many different flowset entries as possible, instead of multiple streams mapping to the same flowset entry */
int dlb_ecmp_dynamic_age = 33;
                    /* Inactivity duration. Unit is usec.
                       Indicates the duration for which an incoming macro flow
                       should be idle to consider it to be reassigned to another
                       ECMP member port */
                    /* For ML clusters application, this value should be as low
                       as possible, but also should consider the overall system's
                       traffic design and set to a value not to cause packet
                       reordering. (if it's too low, it leads to packet reordering) */

int dlb_ecmp_max_paths = 64;
                    /* ECMP maximum paths. */


/*** L3 Route setup parameters ***/
bcm_vlan_t ingress_vlan = 2;
bcm_vlan_t egress_vlan[NO_OF_ECMP_MEMBERS] = {10, 20, 30, 40, 50, 60, 70, 80};

bcm_vrf_t vrf = 1;
bcm_mac_t station_mac = "00:00:00:00:00:01";

bcm_mac_t src_mac[NO_OF_ECMP_MEMBERS] = {
    "00:00:00:00:00:22",
    "00:00:00:00:00:33",
    "00:00:00:00:00:44",
    "00:00:00:00:00:55",
    "00:00:00:00:00:66",
    "00:00:00:00:00:77",
    "00:00:00:00:00:88",
    "00:00:00:00:00:99",
};

bcm_mac_t dst_mac[NO_OF_ECMP_MEMBERS] = {
    "00:00:00:00:00:02",
    "00:00:00:00:00:03",
    "00:00:00:00:00:04",
    "00:00:00:00:00:05",
    "00:00:00:00:00:06",
    "00:00:00:00:00:07",
    "00:00:00:00:00:08",
    "00:00:00:00:00:09",
};

bcm_ip_t dst_ip_route = 0x0a0a0000; /* L3 route network address*/
bcm_ip_t dst_ip_mask  = 0xffff0000; /* Subnet mask */

/*** Traffic streams to be used in the test as hex strings ***/
char *ipv4_stream_1 = "00000000000100000000000A810000020800450005C600000000401138EC141414140A0A0A0A15B3001905B2A28200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

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

    rv = egress_port_multi_setup(unit, egress_port, NO_OF_ECMP_MEMBERS);
    if (BCM_FAILURE(rv)) {
        printf("egress_port_multi_setup() failed.\n");
        return rv;
    }
	return rv;
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

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed0, seed));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config,
                                                        hash_computation));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0,
                                                        ipv4_select_fields));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0,
                                                        ipv4_select_fields));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4Field0,
                                                        ipv4_select_fields));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable,
                                                        pre_process_enable));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp,
                                                        use_flow_based_hash_ecmp));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmpDynamic,
                                                        use_flow_based_hash_ecmp));
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
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit, bcmSwitchEcmpDynamicSampleRate,
                                        dlb_ecmp_sampling_rate));

    /* DLB Quantization parameters */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesExponent,
                                        dlb_ecmp_egress_bytes_exponent));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesDecreaseReset,
                                        dlb_ecmp_egress_bytes_decrease_reset));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesExponent,
                                        dlb_ecmp_queued_bytes_exponent));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesDecreaseReset,
                                        dlb_ecmp_queued_bytes_decrease_reset));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent,
                                        dlb_ecmp_physical_queued_bytes_exponent));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset,
                                        dlb_ecmp_physical_queued_decrease_reset));

    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesMinThreshold,
                                        dlb_ecmp_egress_bytes_min_threshold));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesMaxThreshold,
                                        dlb_ecmp_egress_bytes_max_threshold));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesMinThreshold,
                                        dlb_ecmp_queued_bytes_min_threshold));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesMaxThreshold,
                                        dlb_ecmp_queued_bytes_max_threshold));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold,
                                        dlb_ecmp_physical_queued_bytes_min_threshold));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold,
                                        dlb_ecmp_physical_queued_bytes_max_threshold));

    /* Set the random seed */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit, bcmSwitchEcmpDynamicRandomSeed,
                                                    dlb_ecmp_random_seed));

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

/* Configure my station tcam to enable L3 route lookup */
int
config_my_station_tcam(int unit, bcm_mac_t mac)
{
    bcm_error_t rv;
    bcm_mac_t mac_mask={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_l2_station_t l2_station;
    int station_id;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    sal_memcpy(l2_station.dst_mac_mask, mac_mask, sizeof(mac_mask));
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
    rv = create_vlan_add_port(unit, ingress_vlan, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    for(i = 0; i < NO_OF_ECMP_MEMBERS; i++) {
        /* Create and add ingress port to ingress VLAN */
        rv = create_vlan_add_port(unit, egress_vlan[i], egress_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

	/* Enable L3 Lookup for incoming l3 packet */
    rv = config_my_station_tcam(unit, station_mac);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_my_station_tcam(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 ingress interface */
    rv = create_l3_ing_interface(unit, vrf, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_ing_interface(): %s.\n",bcm_errmsg(rv));
        return rv;
    }


    /* VLAN to L3_IIF mapping */
    rv = config_l3_iif_binding(unit, ingress_vlan, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in config_l3_iif_binding %s.\n", bcm_errmsg(rv));
        return rv;
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

	return rv;
}

/* Get traffic metrics and DLB Quality metrics when traffic is running */
int get_traffic_stats(int unit) {
    bcm_error_t rv = BCM_E_NONE;
    int i = 0;
    bcm_l3_ecmp_dlb_port_quality_t port_quality[NO_OF_ECMP_MEMBERS];
    char str[1000];
    bshell(unit,"sleep quiet 5");
    bshell(unit, "e; e ~~~; e show c r; e ~~~; show c r");
    // printf("\n ~~~ Flowset entries taken by ingress streams ~~~");
    // bshell(unit, "e; e ~~~; e bsh -c 'pt dump -d DLB_ECMP_FLOWSET_INST0m|grep -B2 -A2 OBSERVATION_TIMESTAMP'; e ~~~");
    // bshell(unit, "bsh -c 'pt dump -d DLB_ECMP_FLOWSET_INST0m|grep -B2 -A2 OBSERVATION_TIMESTAMP'");
    // bshell(unit, "e; e ~~~; e bsh -c 'pt dump -d DLB_ECMP_FLOWSET_INST1m|grep -B2 -A2 OBSERVATION_TIMESTAMP'; e ~~~");
    // bshell(unit, "bsh -c 'pt dump -d DLB_ECMP_FLOWSET_INST1m|grep -B2 -A2 OBSERVATION_TIMESTAMP'");

    printf("\n ~~~ DLB Quality on egress ports ~~~");
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

    return BCM_E_NONE;
}

/* Main verify function to perform the traffic test */
int test_verify(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    /* Configuration show */
    printf("\n -------------------");
    printf("\n Configuration show:");
    printf("\n -------------------");
    bshell(unit, "e; e ~~~; e ps ce0-ce9; e ~~~; ps ce0-ce9");
    bshell(unit, "e; e ~~~; e vlan show; e ~~~; vlan show");
    bshell(unit, "e; e ~~~; e l3 ing_intf show; e ~~~; l3 ing_intf show");
    bshell(unit, "e; e ~~~; e l3 intf show; e ~~~; l3 intf show");
    bshell(unit, "e; e ~~~; e l3 egress show; e ~~~; l3 egress show");
    bshell(unit, "e; e ~~~; e l3 ecmp show; e ~~~; l3 ecmp show");
    bshell(unit, "e; e ~~~; e l3 route show; e ~~~; l3 route show");


    printf("\n -------------");
    printf("\n Traffic Test:");
    printf("\n -------------");
    /* IPv4 streams (8 No.s) destined to DLB ECMP group  */
    printf("\n Start the 8 IPv4 streams destined to DLB ECMP group from traffic generator at once:\n");
    printf("Ingress port                : %d\n", ingress_port);
    printf("Rate of traffic             : 100 %% of Line rate\n");
    printf("Rate of each 8 streams      : 12.5 %% of Line rate\n");
    printf("Packet data in first stream :\n%s\n", ipv4_stream_1);
    printf("NOTE: Rest of 7 streams will have same content, but L4 UDP Src port changed.\n");
    // printf("\n Press C after starting traffic streams to continue.");
    // printf("\n Press Q to quit the traffic test.\n");
    // if(bshell(unit, "pause -C")) {
    //     printf("\n Exiting Traffic test.\n");
    //     return;
    // }

    printf("\nGetting traffic information:\n");
    rv = get_traffic_stats(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error executing get_traffic_stats(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n ----------------------------------");
    printf("\n Final traffic stream distribution:");
    printf("\n ----------------------------------");
    bshell(unit,"sleep quiet 5");
    bshell(unit, "e; e ~~~; e show c c s r; e ~~~; show c c s r");

    return BCM_E_NONE;
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
