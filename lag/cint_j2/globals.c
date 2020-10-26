/*
 * globals.c
 *
 *  Created on: Nov 20, 2018
 *      Author: Csaba
 *  cint version of needed global definitions and logging
 */
cint_reset();
typedef enum ret_code_e {
  RET_CODE_AVL_ERR = -8,
  RET_CODE_UNSUPPORTED = -7,
  RET_CODE_NOT_FOUND = -6,
  RET_CODE_ARG_INVALID = -5,
  RET_CODE_CONNECT_FAILURE = -4,
  RET_CODE_BIND_FAILURE = -3,
  RET_CODE_MALLOC_FAILURE = -2,
  RET_CODE_FAILURE = -1,
  RET_CODE_SUCCESS = 0,
  RET_CODE_MAX
};
//const int p13=2, p14=3, p15=4, p16=5, p17=6;
const int p13=13, p14=14, p15=15, p16=16, p17=17;
int verbose = 0;
int on_hw = 0;

void
LOG_ERR_MSG(char *pre, char *fmt, char *msg)
{
  printf("%s:%s\n",fmt, msg);
}
void
bcmsdk_trace_device_error(const char *app_err,  const char *sdk_err,
                          uint32_t unit)
{
  printf("Unit %u: app : %s, sdk : %s, \n",
              unit, app_err, sdk_err);
  return;
}

const int BCM_DNX_MAX_CORES = 2;
const int BCM_DNX_MAX_PORTS = 256;
const int BCM_SDK_DNX_DEF_VLAN = 4095;
const int BCM_DNX_OLP_RSVD_MCID = 4096;
const int IEEE_MACLEN = 6;

int use_tpid_class  = 1;
int use_inlif = 1; //needed for vlan->vsi mapping
int use_outlif = 1;
int use_case = 1;
int use_vlan_trans = 1;
int use_esem = 1;

typedef struct vsi_info_t {
  uint32_t in_gp[BCM_DNX_MAX_PORTS];
  uint32_t out_gp[BCM_DNX_MAX_PORTS];
};

ret_code_e
bcm_dnx_get_sys_mod_port(int unit, int port, bcm_gport_t *modport,
                         bcm_gport_t *sysport)
{
  bcm_port_interface_info_t pi;
  bcm_port_mapping_info_t pm;
  uint32_t flags;
  bcm_error_t rv;

  *modport = 0;
  *sysport = 0;

  rv = bcm_port_get(unit, port, &flags, &pi, &pm);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("Failure getting  port during get sysmod port", bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }

  BCM_GPORT_MODPORT_SET(*modport, ((unit * BCM_DNX_MAX_CORES) + pm.core), port);
  BCM_GPORT_SYSTEM_PORT_ID_SET(*sysport, port);

  return RET_CODE_SUCCESS;
}

int
bcm_dnx_sdk_manage_mcast_group (int unit, uint32_t port, int mcid, int add, bcm_gport_t enc_gp)
{
  bcm_multicast_replication_t rep;
  int rv;
  bcm_multicast_replication_t_init(&rep);
  rep.port = port;
  rep.encap1 = enc_gp & 0xfffff;
  if (add) {
    rv = bcm_multicast_add(unit, mcid, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, 1,
                       &rep);
  } else {
    rv = bcm_multicast_delete(unit, mcid, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, 1,
                           &rep);
  }
  if (rv != BCM_E_NONE){
        printf("Error, bcm_multicast_%s port: %x mcid: %d enc: %x: %s\n",
               add ? "add" : "delete", port, mcid, enc_gp, bcm_errmsg(rv));;
  }
  return rv;
}
ret_code_e
bcm_dnx_sdk_mac_flush_by_port (int unit, uint32_t port)
{
  int rv;

  rv = bcm_l2_addr_delete_by_port(unit, -1, port, BCM_L2_DELETE_STATIC);
  if (rv != BCM_E_NONE) {
    printf("mac-flush failed for port %d unit %d. %s\n",
                port, unit, bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  return (RET_CODE_SUCCESS);
}

ret_code_e
olp_port_get(int unit, bcm_gport_t *port)
{
  int count, rv = BCM_E_NONE;
  bcm_gport_t olp[BCM_DNX_MAX_CORES];

  rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OLP,
                             BCM_DNX_MAX_CORES, olp, &count);
  if ((rv != BCM_E_NONE) || (count < 1)) {
    bcmsdk_trace_device_error("Port internal get failure",
                               bcm_errmsg(rv), unit);
    return RET_CODE_FAILURE;
  }
  *port = olp[0];
  return RET_CODE_SUCCESS;
}

void
memcpy(uint8_t *dst, uint8_t *src, int len)
{
  while(0 < len) {
    *dst++ = *src++;
    len --;
  }
}

ret_code_e
bcm_dnx_sdk_l2_init (int lcl_unit)
{
  int rv;
  //NOTE appl_dnx_olp_init() should do this
//  rv = bcm_switch_control_set(lcl_unit, bcmSwitchL2LearnMode, BCM_L2_INGRESS_DIST);
//  if (rv != BCM_E_NONE) {
//    bcmsdk_trace_device_error(" bcm l2 learn mode set failure ",
//                             bcm_errmsg(rv), lcl_unit);
//    return RET_CODE_FAILURE;
//  }
  if (1) {
    uint16_t idx;
    ret_code_e rc;
    bcm_gport_t olp_gp = 0;
    int mcid = BCM_DNX_OLP_RSVD_MCID;
    bcm_l2_learn_msgs_config_t learn_msgs;
    uint8_t _src_mac_address[IEEE_MACLEN] = {0x00, 0x00, 0x00, 0x00, 0x12, 0x55};
    uint8_t _dest_mac_address[IEEE_MACLEN] = {0x00, 0x00, 0x00, 0x00, 0x44, 0x88};

    rv = bcm_multicast_create(lcl_unit,
                              BCM_MULTICAST_INGRESS_GROUP|BCM_MULTICAST_WITH_ID,
                              &mcid);
    if (BCM_E_NONE != rv) {
      bcmsdk_trace_device_error(" Multicast group create failure",
                               bcm_errmsg(rv), lcl_unit);
  //    return RET_CODE_FAILURE;
    }

    rc = olp_port_get(lcl_unit, &olp_gp);
    if (RET_CODE_SUCCESS != rc) {
      bcmsdk_trace_device_error(" olp port get  failure",
                               bcm_errmsg(rv), lcl_unit);
      return rc;
    }

    //TODO appl_ref_l2_init() might do the same
    rv = bcm_dnx_sdk_manage_mcast_group(lcl_unit, olp_gp, mcid, 1, 0);
    if (BCM_E_NONE != rv) {
      bcmsdk_trace_device_error(" bcm multicast ingress get failure ",
                               bcm_errmsg(rv), lcl_unit);
      //return RET_CODE_FAILURE;
    }

    /* set distribution of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);

    /* the following setting affects learning messages */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_LEARNING | BCM_L2_LEARN_MSG_DEST_MULTICAST;
    learn_msgs.dest_group = mcid;
    //learn_msgs.flags |= BCM_L2_LEARN_MSG_LEARNING;
    //learn_msgs.dest_port = 240; //or 79 or 240 (olp)?
    /* follow attributes set the encapsulation of the learning messages*/
    /* learning message encapsulated with ethernet header*/
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0x120;
    memcpy(learn_msgs.src_mac_addr, _src_mac_address, 6);
    memcpy(learn_msgs.dst_mac_addr, _dest_mac_address, 6);
    /* default value of learn_msgs.vlan indicates packet is not tagged */
    /*learn_msgs.vlan/tpid/vlan_prio: not relevant as packet is not tagged*/
    rv = bcm_l2_learn_msgs_config_set(lcl_unit,&learn_msgs);
    if (rv != BCM_E_NONE) {
      bcmsdk_trace_device_error(" bcm l2 learn msgs config set failure ",
                               bcm_errmsg(rv), lcl_unit);
      return RET_CODE_FAILURE;
    }
  }
  /* change learning mode, to be egress distributed, packet will be learned at egress
  note that if packet dropped in ingress then it will not be learned
  */
  //NOTE egress learning in not supported on J2 (AN 3.3.3)

  return RET_CODE_SUCCESS;
}

