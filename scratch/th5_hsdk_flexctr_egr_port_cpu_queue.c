/*  Feature  : Flex Counters
 *
 *  Usage    : BCM.0> cint th5_hsdk_flexctr_egr_port_queue.c
 *
 *  Config   : bcm78900_b0-generic-128x400.config.yml
 *
 *  Log file : th5_hsdk_flexctr_egr_port_queue_log.txt
 *
 *  Test Topology :
 *
 *                   +-----------------------+
 *                   |                       |
 *                   |                       |
 *                   |                       |
 *       +-----------+        TH5            +------------+
 *                   |                       |
 *                   |                       |
 *                   |                       |
 *                   +-----------------------+
 *
 *
 *  Summary:
 *  ========
 *    This CINT script demonstrates how to configure and use flex counters
 *    to count packets and bytes on egress port queues.
 *
 *    Prerequistes:
 *    =============
 *      Build HSDK, the Flex Counter feature is built by default.
 *
 *    =====================================================
 *    Detailed operations performed by the CINT script:
 *
 *    1) Step1 - Test Setup: The function test_setup() is used to gather
 *       test ports and set up per queue counters for each test port.
 *      a) Verify that attached device is supported by this test.
 *      b) Get list of test ports using SDK API.
 *      c) Initialize queue counter structure
 *      d) Configure queue counters for all test ports.
 *    2) Step2 - Configuration: There is no separate configuration
 *       step. Configuration operations take place during the verification step.
 *    3) Step3 - Verification (Done in verify())
 *      a) Transmit unicast packets on each test port with port in
 *         loopback. The number of packets and size of each packet is a function
 *         of the port.
 *      b) For each port, get the packet and byte counts for all queues. Verify
 *         the actual packet and byte counts against expected values.
 *
 *      Expected Result:
 *      Per queue packet and byte counts must match what was transmitted. If they
 *      don't, an error code is returned.
 */

cint_reset();

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
 *
 * SUPPORT FUNCTIONS FOR PER PORT, PER QUEUE EGRESS COUNTERS
 *
 * Use the functions defined here for applications that require per queue
 * counters.
 *
 * The TH5 does not have dedicated per port, per queue counters
 * (typically called PERQ_PKT and PERQ_BYTE).  In place of this functionality,
 * the TH5 supports flex counters that provide equivalent functionality
 * (see bcmFlexctrObjectStaticEgrQueueNum and bcmFlexctrObjectStaticEgrEgressPort ).
 */

/* Define CONSTANTS */

/* Even though there are only 12 queues, flex counters allocate 16 */
const uint32        RESERVE_QUEUES_PER_PORT = 16;

/*
 * The TH5 supports a total of 12 queues, the default configuration
 * is 8 unicast queues (0..7) and 4 multicast queues (8..11).
 */
const uint32        EGR_QUEUES_PER_PORT = 12;

/* Need to reserve flex counter resources across logical port space */
const int           MAX_LOGICAL_PORT = 352;

/* Define DATA STRUCTURES */

/*
 * Define a data structure to hold the counter ID and attached ports.
 */
typedef struct queue_counter_t {
  /* Keep track of which ports have been attached to the flex counter */
  bcm_pbmp_t          attached_ports;
  /* One flex counter for all ports */
  uint32              stat_counter_id;
};
uint32 flex_cpu_ctr_id;
bcm_port_t cpu_port = 0;

int cpu_q_max = 48;    // Size of one section is 128 in counter pool. Although queues for CPU are 48.
int cpu_q_max_bits = 6;

int q_per_port = 16;
int q_per_port_bits = 4;


/* Pass packet and byte counts for each port in this structure. */
typedef struct per_queue_counts_t {
  bcm_flexctr_counter_value_t counter_values[EGR_QUEUES_PER_PORT];
};

/* DEFINE PER PORT/PER QUEUE SPECIFIC FUNCTIONS */

/*******************************************************************************
 * Function: queue_counter_t_init
 *
 * Helper function used to initialize a queue_counter_t structure.
 *
 * Parameters:
 *   queue_counter: Pointer to queue count structure.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
void
queue_counter_t_init(queue_counter_t * queue_counter)
{
  sal_memset(queue_counter, 0, sizeof(*queue_counter));
}

/*******************************************************************************
 * Function: egr_port_and_queue_num
 *
 * Helper function used to configure a single flex counter for
 * bcmFlexctrObjectStaticEgrQueueNum and bcmFlexctrObjectStaticEgrEgressPort.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   stat_counter_id: Pointer to receive the new counter ID.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
egr_port_and_queue_num(int unit, uint32 * stat_counter_id)
{
  const int           options = 0;

  bcm_flexctr_action_t action;
  bcm_flexctr_index_operation_t *index_op;
  bcm_flexctr_value_operation_t *value_a_op;
  bcm_flexctr_value_operation_t *value_b_op;

  bcm_flexctr_action_t_init(&action);
  action.flags = 0;
  action.source = bcmFlexctrSourceEgrPort;
  action.mode = bcmFlexctrCounterModeNormal;
  /* reserve resources */
  action.index_num = (MAX_LOGICAL_PORT + 1) * RESERVE_QUEUES_PER_PORT;

  index_op = &action.index_operation;
  index_op->object[0] = bcmFlexctrObjectStaticEgrQueueNum;
  index_op->mask_size[0] = 4;
  index_op->shift[0] = 0;
  index_op->object[1] = bcmFlexctrObjectStaticEgrEgressPort;
  index_op->mask_size[1] = 9;
  index_op->shift[1] = 4;

  /* Increase counter per packet. */
  value_a_op = &action.operation_a;
  value_a_op->select = bcmFlexctrValueSelectCounterValue;
  value_a_op->object[0] = bcmFlexctrObjectConstOne;
  value_a_op->mask_size[0] = 1;
  value_a_op->shift[0] = 0;
  value_a_op->object[1] = bcmFlexctrObjectConstZero;
  value_a_op->mask_size[1] = 1;
  value_a_op->shift[1] = 0;
  value_a_op->type = bcmFlexctrValueOperationTypeInc;

  /* Increase counter per packet bytes. */
  value_b_op = &action.operation_b;
  value_b_op->select = bcmFlexctrValueSelectPacketLength;
  value_b_op->type = bcmFlexctrValueOperationTypeInc;

  /* Create an ingress action with a time-based trigger */
  BCM_IF_ERROR_RETURN(bcm_flexctr_action_create
                      (unit, options, &action, stat_counter_id));

  printf("Created Flex Counter Action: ID=0x%X\n", *stat_counter_id);
  return BCM_E_NONE;
}

/*******************************************************************************
 * Function: create_queue_counter_for_port
 *
 * Configure the per queue counters for specified port.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   logical_port: Logical port to configure
 *   queue_counter: Counter struct that the flex counter stat ID.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
create_queue_counter_for_port(int unit, bcm_port_t logical_port,
                              queue_counter_t * queue_counter)
{
  /*
   * Create th flex counter if this is the first port being added to the
   * flex counter.
   * */
  if (queue_counter->stat_counter_id == 0) {
    BCM_IF_ERROR_RETURN(egr_port_and_queue_num
                        (unit, &queue_counter->stat_counter_id));
  }
  /* Attach logical port to flex counter */
  BCM_IF_ERROR_RETURN(bcm_port_stat_attach
                      (unit, logical_port, queue_counter->stat_counter_id));
  /* Track attached ports */
  BCM_PBMP_PORT_ADD(queue_counter->attached_ports, logical_port);
  return BCM_E_NONE;
}

/*******************************************************************************
 * Function: get_port_queue_counts
 *
 * Get packet and byte counts for all queues on specified port.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   logical_port: Logical port to configure
 *   queue_counter: Counter struct that holds the counter stat ID.
 *   per_queue_counters: Pointer to receive packet and byte counts for
 *   all queues.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
get_port_queue_counts(int unit, bcm_port_t logical_port,
                      queue_counter_t * queue_counter,
                      per_queue_counts_t * per_queue_counts)
{
  int                 queue;
  uint32              counter_indexes[EGR_QUEUES_PER_PORT];
  bcm_error_t         rv;

  /* Compute list of indices for all queues on this logical port */
  for (queue = 0; queue < EGR_QUEUES_PER_PORT; queue++) {
    counter_indexes[queue] = (logical_port * RESERVE_QUEUES_PER_PORT) + queue;
  }

  /* Fetch packet and byte counts for all queues */
  if (BCM_FAILURE(rv = bcm_flexctr_stat_sync_get(unit,
                                                 queue_counter->stat_counter_id,
                                                 EGR_QUEUES_PER_PORT, counter_indexes,
                                                 per_queue_counts->counter_values))) {
    printf("bcm_flexctr_stat_sync_get() FAILED %s\n", bcm_errmsg(rv));
    return rv;
  }
  return BCM_E_NONE;
}

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
 *
 * TEST FUNCTIONS:
 * The following functions are used to validate the per queue counter function
 * defined above.
 */

/*******************************************************************************
 * Function: portNumbersGet
 *
 * Get a list of 100G ports for use in this test.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   port_list: Pointer to receive list of test ports
 *   num_ports: Number of ports to get (and size of port_list)
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int *num_ports)
{
  bcm_port_t          port;
  bcm_error_t         rv;
  bcm_port_config_t   configP;
  int                 port_count;

  /* Get current port configuration */
  if (BCM_FAILURE(rv = bcm_port_config_get(unit, &configP))) {
    printf("Error in bcm_port_config_get: %s\n\n", bcm_errmsg(rv));
    return rv;
  }

  /* Collect requested number of "cd" ports */
  port_count = 0;
  printf("Ports: ");
  BCM_PBMP_ITER(configP.all, port) {
    if (port == 0) {
      continue;
    }
    port_list[port_count] = port;
    printf("%3d(d3c%d) ", port, port_count);
    if ((port_count % 10) == 9) {
      printf("\n   ");
    }
    port_count++;
  }
  printf("\n");
  printf("portNumbersGet() ports total %d \n",
         port_count);
  *num_ports = port_count;

  return BCM_E_NONE;
}

/*******************************************************************************
 * Function: do_tx
 *
 * Using diagnostic shell commands, send packets on a single test port. The
 * number of packets and size of each packet is a function of the "port_num"
 * parameters.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   port_num: A sequential port number, "0" for "cd0" etc.
 *
 * Returns:
 * Nothing
 */
void
do_tx(int unit, int port_num)
{
  /* Packet priority, packet count and byte count are a function of port number */
  const int           tx_count = port_num + 1;
  const int           tx_len = 100 + (port_num * 4);
  const int           tx_prio = port_num % 8;

  char                command_str[185];

  /* Set port to MAC loopback */
  sprintf(command_str, "port d3c%d lb=edb", port_num);
  bshell(unit, command_str);

  /* Create an L2 entry so that we're sending unicast packets */
  sprintf(command_str,
          "l2 add Port=d3c%d vlan=1 mac=00:00:00:00:00:%02X static=true",
          port_num, port_num + 1);
  bshell(unit, command_str);
  printf(">> %s\n", command_str);

  /* Send packet using built in "tx" command */
  sprintf(command_str,
          "tx %d Length=%d VlanPrio=%d dm=00:00:00:00:00:%02X sm=80:80:80:80:80:%02X",
          tx_count, tx_len, tx_prio, port_num + 1, port_num + 64);
  bshell(unit, command_str);
  printf(">> %s [%d bytes]\n", command_str, tx_count * tx_len);

  /* Set port back to no loopback */
  sprintf(command_str, "port d3c%d lb=none", port_num);
  bshell(unit, command_str);
}

/*******************************************************************************
 * Function: get_device_type
 *
 * Get current device type as listed on PCI bus. Used to determine if current
 * device is supported byt this test.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   dev_type: Pointer to receive device type code.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
get_device_type(int unit, uint32 * dev_type)
{
  bcm_info_t          info;

  BCM_IF_ERROR_RETURN(bcm_info_get(unit, &info));

  *dev_type = info.device & 0x0000FFF0;       /* Mask off lowest hex digit to get dev group */

  return BCM_E_NONE;
}

/*******************************************************************************
 * Function: test_setup
 *
 * Main set up routine. Calls other setup sub-functions defined above.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   port_list: List of ports to configure
 *   num_ports: Number of ports in port_list
 *   queue_counter: Pointer to queue_counter struct, this function
 *     initializes and configures this object.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
test_setup(int unit, bcm_port_t * port_list, int num_ports,
           queue_counter_t * queue_counter)
{
  int                 idx;
  uint32              dev_type;

  /* Test test only supported for Tomahawk5 */
  BCM_IF_ERROR_RETURN(get_device_type(unit, &dev_type));
  if (dev_type != 0xF900) {
    printf("Unsupported device type/configuration\n");
    return BCM_E_FAIL;
  }

  /* Get list of ports for this configuration */
  BCM_IF_ERROR_RETURN(portNumbersGet(unit, port_list, &num_ports));

  /* Initialize queue counter structure */
  queue_counter_t_init(queue_counter);

  /* Create per queue counters for all ports in the port list */
  for (idx = 0; idx < num_ports; idx++) {
    BCM_IF_ERROR_RETURN(create_queue_counter_for_port
                        (unit, port_list[idx], queue_counter));
  }
  printf("Setup of counters for FP ports done \n");
  return BCM_E_NONE;
}

void PrintStatCpuQueue(int unit)
{
  int opt_ClearCounterOnRead = 0;
  int rv;
  uint32 cid[cpu_q_max];
  bcm_flexctr_counter_value_t     counter_value[cpu_q_max];
  int q_num;
  int pipe = 0;
  bcm_pbmp_t pbm;
  bcm_port_t port=cpu_port;
  char *desc;

  printf("\n-----  CPU QUEUE - PIPE %d  -------\n", pipe);

  for (q_num=0; q_num < cpu_q_max; ++q_num) {
    cid[q_num] = q_num;
  }

  rv = bcm_flexctr_stat_sync_get(unit, flex_cpu_ctr_id,
                                 cpu_q_max, &cid, &counter_value);
  if (BCM_E_NONE != rv) {
    printf("Error in reading cpu queue stat %d %d\n", rv, cpu_q_max); return;
  }
  for (q_num = 0; q_num < cpu_q_max; ++q_num) {
    //        if ((COMPILER_64_HI(counter_value[q_num].value[0]) != 0) ||
    //            (COMPILER_64_LO(counter_value[q_num].value[0]) != 0) ) {
    printf("CPU QUEUE %d Ctr:0x%08X-0x%08X port:%d -     0x%08d    0x%08d  (val0_low / val1_low)\n",
           q_num, flex_cpu_ctr_id, cid[q_num], port,
           COMPILER_64_LO(counter_value[q_num].value[0]),
           COMPILER_64_LO(counter_value[q_num].value[1]));
    //        }
  }
  // Clear counter

//  if (opt_ClearCounterOnRead) {
//    sal_memset(counter_value, 0x00, cpu_q_max * sizeof(counter_value[0]));
//    bcm_flexctr_stat_set(unit, flex_cpu_ctr_id, cpu_q_max,
//                         &cid, &counter_value);
//  }

  printf("~~~~~~~~~~~~~~~~~~~~~~~\n");
  return;
}


/*******************************************************************************
 * Function: verify
 *
 * Main verification routine. Sends traffic on all test ports and verifies
 * that per queue counters match expected values.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   num_ports: Number of ports in port_list
 *   port_list: List of ports to configure
 *   queue_counter: Pointer to queue_counter struct,
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
verify(int unit, int *port_list, int num_ports, queue_counter_t * queue_counter)
{
  bcm_port_t          port_idx;
  int                 test_failed = 0;

  sal_sleep(20);
  bshell(unit, "show c");
  PrintStatCpuQueue(unit);
  return BCM_E_NONE;
}

bcm_error_t
setup_cpu_counter (int unit, int cpu_port, uint32 *flex_cntr_id)
{
  const int           options = 0;
  printf("setup cpu counters \n");
  uint32_t stat_counter_id;
  bcm_flexctr_action_t action;
  bcm_flexctr_index_operation_t *index_op;
  bcm_flexctr_value_operation_t *value_a_op;
  bcm_flexctr_value_operation_t *value_b_op;

  bcm_flexctr_action_t_init(&action);
  action.flags = 0;
  action.source = bcmFlexctrSourceEgrPort;
  action.mode = bcmFlexctrCounterModeNormal;
  action.drop_count_mode = bcmFlexctrDropCountModeAll;
  action.index_num = cpu_q_max; // Max CPU queues.

  index_op = &action.index_operation;
  index_op->object[0] = bcmFlexctrObjectStaticEgrQueueNum;
  index_op->mask_size[0] = cpu_q_max_bits;
  index_op->shift[0] = 0;
  index_op->object[1] = bcmFlexctrObjectConstZero;
  index_op->mask_size[1] = 1;     // For CPU port, port is known =0 so dont allocate any bits for the port.
  index_op->shift[1] = 0;

  /* Increase counter per packet. */
  value_a_op = &action.operation_a;
  value_a_op->select = bcmFlexctrValueSelectCounterValue;
  value_a_op->type = bcmFlexctrValueOperationTypeInc;
  value_a_op->object[0] = bcmFlexctrObjectConstOne;
  value_a_op->mask_size[0] = 1;
  value_a_op->shift[0] = 0;
  value_a_op->object[1] = bcmFlexctrObjectConstZero;
  value_a_op->mask_size[1] = 1;
  value_a_op->shift[1] = 0;

  /* packet bytes */
  value_b_op = &action.operation_b;
  value_b_op->select = bcmFlexctrValueSelectPacketLength;
  value_b_op->type = bcmFlexctrValueOperationTypeInc;

  printf("Doing flex ctr action create for cpu queue...\n");
  BCM_IF_ERROR_RETURN(bcm_flexctr_action_create(unit, options,
                                                &action, &stat_counter_id));
  print stat_counter_id;

  BCM_IF_ERROR_RETURN(bcm_port_stat_attach(unit, cpu_port, stat_counter_id));

  *flex_cntr_id = stat_counter_id;
}

/*******************************************************************************
 * Function: execute
 *
 * Test entry point. Test paramters defined here, calls "setup" and "verify"
 * functions.
 *
 * Parameters:
 *   None, all test parameters are defined by this function.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
execute()
{
  const int           unit = 0;
  int                 num_ports = 64;

  bcm_port_t          port_list[num_ports];
  queue_counter_t     queue_counter;

  /* Get test port list and configure per queue counters */
//  BCM_IF_ERROR_RETURN(test_setup(unit, port_list, num_ports, &queue_counter));

  BCM_IF_ERROR_RETURN(setup_cpu_counter(unit, 0, &flex_cpu_ctr_id));

  /* Send packets on specified queues and verify counts */
  return verify(unit, port_list, num_ports, &queue_counter);
}

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
 * Entry point
 */
const char         *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  if (BCM_FAILURE(execute())) {
    printf("Test Failed\n");
  }
}
