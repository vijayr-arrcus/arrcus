/*
 * expand macros with
gcc -E acl.c -o - | grep -v "^#" > cint_acl.c

FGs:

TAIL:
I: ingress_fg_qualify
  VLAN: vlan_fg_qualify
  port: in_port_fg_qualify
E: egress_fg_qualify 
  VLAN,port: out_vlan_fg_qualify,out_port_fg_qualify

BCM_DNX_FG_TUPLE v4
tuple_fg_qualify
 I: in_tuple_fg_qualify,TAIL
 E: ud_ether[0],TAIL
new:
tuple_fg_qualify
 I: in_tuple_fg_qualify,TAIL
 E: out_tuple_fg_qualify,TAIL

BCM_DNX_FG_TUPLE6 v6
 I: in_tuple6_fg_qualify,in_tuple_fg_qualify,ud_icmp[0],TAIL
 E: out_tuple6_fg_qualify,ud_ether[0],TAIL
new:
 I: in_tuple6_fg_qualify,in_tuple_fg_qualify,TAIL
 E: out_tuple6_fg_qualify,out_tuple_fg_qualify,TAIL

BCM_DNX_FG_L2
l2_fg_qualify
 I: in_l2_fg_qualify,TAIL
 E: ud_ether[0],TAIL
new:
l2_fg_qualify
 I: in_l2_fg_qualify,TAIL
 E: out_l2_fg_qualify,TAIL

BCM_DNX_FG_IPV4 v4
l3_fg_qualify,L4_fg_qualify,v4_fg_qualify
 I: mpls_fg_qualify,in_l3_fg_qualify,ud_icmp[0],TAIL 
 E: out_l3_fg_qualify,TAIL
new:
l3_fg_qualify,L4_fg_qualify,v4_fg_qualify
 I: mpls_fg_qualify,in_l3_fg_qualify,TAIL
 E: out_l3_fg_qualify,TAIL
 
BCM_DNX_FG_IPV6_PRE v6
 I: pre_v6_fg_qualify,ud_icmp[0]
 E:
new:
 I: pre_v6_fg_qualify
 E:

cascaded from BCM_DNX_FG_IPV6_PRE
BCM_DNX_FG_IPV6 v6
v6_fg_qualify //from l3_*
 I: L4_fg_qualify,ud_icmp[0],in_v6_fg_qualify,TAIL
 E: out_v6_fg_qualify,TAIL
new:
 I: L4_fg_qualify,in_v6_fg_qualify,TAIL
 E: out_v6_fg_qualify,TAIL

BCM_DNX_FG_USER_COPP v4
l3_fg_qualify,copp_fg_qualify,user_copp_fg_qualify
 I: in_copp_fg_qualify,in_default_copp_fg_qualify,L4_fg_qualify,ud_icmp[0],TAIL
 E: TAIL
new:
l3_fg_qualify,copp_fg_qualify,user_copp_fg_qualify
 I: in_copp_fg_qualify,in_default_copp_fg_qualify,L4_fg_qualify,TAIL
 E: TAIL

BCM_DNX_FG_DEFAULT_COPP v4
l3_fg_qualify,copp_fg_qualify,default_copp_fg_qualify
 I: in_copp_fg_qualify,in_default_copp_fg_qualify,L4_fg_qualify,ud_icmp[0],TAIL
 E: TAIL
new:
l3_fg_qualify,copp_fg_qualify,default_copp_fg_qualify
 I: in_copp_fg_qualify,in_default_copp_fg_qualify,L4_fg_qualify,TAIL
 E: TAIL

(type, direction, target) identifies the context and FG?
gtuple is more to pass parameters, local to bcm_dnx_find_update_filter_grp() in xlate
gtuple.ctxt is device global
fp_grp_tuple_t *fp_grp holds the FG_ID?

bcm_dnx_sdk_fg_create() FG + context(/program) with actions (bcm_dnx_sdk_fg_create(dpal_plugin_get_dev_id(dh), acl_type, direction, port_type, &fg_tuple))
  bcm_dnx_sdk_get_qset() to add qualifiers into the FG
bcm_dnx_create_fp_entries() adds FG(/key) entries
bcm_dnx_create_fp_entries (void *ctxt, dpal_obj_t *obj, fp_grp_tuple_t *fp_grp,
                           uint32_t *entry_cnt, uint32_t direction,
                           bool fe_install)
                           uint32_t tgt_type, uint64_t tgt_id,
                           uint32_t vlanid, bool install_action, uint32_t statsid,


presel_ingress[BCM_DNX_FG_L2] = BCM_DNX_ING_PRESEL_3, bcmFieldForwardingTypeAny

presel_ingress[BCM_DNX_FG_IPV4==1] = BCM_DNX_ING_PRESEL_4, bcmFieldForwardingTypeIp4Ucast, bcmFieldHeaderFormatIp4AnyL2L3

presel_ingress[BCM_DNX_FG_IPV6==2] = BCM_DNX_ING_PRESEL_5, bcmFieldForwardingTypeIp6Ucast, bcmFieldHeaderFormatIp6AnyL2L3

presel_ingress[BCM_DNX_FG_DEFAULT_COPP] = BCM_DNX_ING_PRESEL_2 in_ports:
      BCM_PBMP_ASSIGN(port_bitmap, port_config.e);
      BCM_PBMP_REMOVE(port_bitmap, port_config.cpu);
      BCM_PBMP_REMOVE(port_bitmap, port_config.sfi);

presel_ingress_g = BCM_DNX_ING_PRESEL_1, bcmFieldForwardingTypeAny

presel_egress[BCM_DNX_FG_IPV4==1] = BCM_DNX_EGR_PRESEL_2, bcmFieldForwardingTypeIp4Ucast

presel_egress[BCM_DNX_FG_IPV6==1] = BCM_DNX_EGR_PRESEL_3, bcmFieldForwardingTypeIp6Ucast

presel_egress_g = BCM_DNX_EGR_PRESEL_1, bcmFieldForwardingTypeAny

    case BCM_DNX_FG_L2:
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_ingress[BCM_DNX_FG_L2]);
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_ingress[BCM_DNX_FG_IPV4]);
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_ingress[BCM_DNX_FG_IPV6]);
      break;

    case BCM_DNX_FG_TUPLE:
    case BCM_DNX_FG_IPV4:
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_ingress[BCM_DNX_FG_IPV4]);
      break;

    case BCM_DNX_FG_TUPLE6:
    case BCM_DNX_FG_IPV6:
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_ingress[BCM_DNX_FG_IPV6]);
      break;

    case BCM_DNX_FG_USER_COPP:
    case BCM_DNX_FG_DEFAULT_COPP:
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_ingress[BCM_DNX_FG_L2]);
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_ingress[BCM_DNX_FG_IPV4]);
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_ingress[BCM_DNX_FG_IPV6]);
      break;

    default:
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_ingress_g);
      break;



    case BCM_DNX_FG_TUPLE:
    case BCM_DNX_FG_IPV4:
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_egress[BCM_DNX_FG_IPV4]);
      break;

    case BCM_DNX_FG_TUPLE6:
    case BCM_DNX_FG_IPV6:
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_egress[BCM_DNX_FG_IPV6]);
      break;

    default:
      BCM_FIELD_PRESEL_ADD(*presel_set, ctxt->presel_egress_g);
      break;

Unused... ingress only
bcm_field_group_cycle_t fg_cycle[type] = {
  bcmFieldGroupCycle1,
  bcmFieldGroupCycle1, BCM_DNX_FG_IPV4
  bcmFieldGroupCycle0, BCM_DNX_FG_IPV6
  bcmFieldGroupCycle1, BCM_DNX_FG_L2
  bcmFieldGroupCycle1, BCM_DNX_FG_MIX
  bcmFieldGroupCycle0, BCM_DNX_FG_DEFAULT_COPP
  bcmFieldGroupCycle0, BCM_DNX_FG_USER_COPP
  bcmFieldGroupCycle1, BCM_DNX_FG_TUPLE
  bcmFieldGroupCycle0, BCM_DNX_FG_IPV6_PRE
  bcmFieldGroupCycle1 BCM_DNX_FG_TUPLE6 // BCM_DNX_FG_MAX
seems to be off
*/

//cint_reset();
//#define p13 2
//#define p14 3

#define BCM_DNX_INVALID_ID 0xFFFFFFF
#define BCM_DNX_SDK_GROUP_PRIORITY_ANY 0xffff
typedef enum bcm_dnx_presel_id_e {
  BCM_DNX_BFD_PRESEL_0 = 0, // Predefined
  BCM_DNX_BFD_PRESEL_1 = 3,
  BCM_DNX_BFD_PRESEL_2,
  BCM_DNX_BFD_PRESEL_5,
  BCM_DNX_ING_COPP_1,
  BCM_DNX_ING_COPP_2,
  BCM_DNX_ING_COPP_3,
  BCM_DNX_ING_COPP_4,
  BCM_DNX_ING_COPP_5,
  BCM_DNX_ING_COPP_6,
  BCM_DNX_BFD_PRESEL_6,
  BCM_DNX_BFD_PRESEL_7,
  BCM_DNX_ING_PRESEL_1, // User configured
  BCM_DNX_ING_PRESEL_2,
  BCM_DNX_ING_PRESEL_3,
  BCM_DNX_ING_PRESEL_4,
  BCM_DNX_ING_PRESEL_5,
  BCM_DNX_ING_PRESEL_6,
  BCM_DNX_ING_PRESEL_7,
  BCM_DNX_ING_PRESEL_8,
  BCM_DNX_ING_PRESEL_9,
  BCM_DNX_ING_PRESEL_10,
  BCM_DNX_ING_PRESEL_11,
  BCM_DNX_ING_PRESEL_12,
  BCM_DNX_BFD_PRESEL_3,
  BCM_DNX_BFD_PRESEL_4,
  BCM_DNX_EGR_PRESEL_0 = 0, // User configured
  BCM_DNX_EGR_PRESEL_1,
  BCM_DNX_EGR_PRESEL_2,
  BCM_DNX_EGR_PRESEL_3,
  BCM_DNX_EGR_PRESEL_4
} /*bcm_dnx_presel_t*/;
typedef enum bcm_dnx_range_id_t {
  BCM_DNX_SP_RANGE = 0,
  BCM_DNX_DP_RANGE,
  BCM_DNX_RANGE_MAX
} /*bcm_dnx_range_id_t*/;
typedef enum bcm_dnx_fg_direction_t {
  BCM_DNX_PMF_NONE = 0,
  BCM_DNX_PMF_INGRESS,
  BCM_DNX_PMF_EGRESS,
  BCM_DNX_PMF_MAX
} /*bcm_dnx_fg_direction_t*/;
char *bcm_dnx_fg_direction_s[] = {
    "",
    "I",
    "E"
};

typedef enum bcm_dnx_fg_type_t {
  BCM_DNX_FG_NONE = 0,
  BCM_DNX_FG_IPV4,
  BCM_DNX_FG_IPV6,
  BCM_DNX_FG_L2,
  BCM_DNX_FG_QOS_V4,
  BCM_DNX_FG_QOS_V6,
  BCM_DNX_FG_QOS_L2,
  BCM_DNX_FG_MIX,
  BCM_DNX_FG_DEFAULT_COPP,
  BCM_DNX_FG_USER_COPP,
  BCM_DNX_FG_TUPLE,
  BCM_DNX_FG_IPV6_PRE,
  BCM_DNX_FG_TUPLE6,
  BCM_DNX_FG_MAX,
  BCM_DNX_FG_PRE_COPP,
  BCM_DNX_FG_COPP_HDR,
  BCM_DNX_FG_MAX_J2
} /*bcm_dnx_fg_type_t*/;

/* Note down which FG types could be allocated from the same TCAM bank.
 * Part of it could be calculated but the mutual exclusion rules are
 * coming from application logic. Let's start with hard-coded values.
 */
typedef enum bcm_dnx_tcam_alloc_group_e {
  BCM_DNX_AG_DONT_CARE = 0,
  BCM_DNX_AG_V4V6,
  BCM_DNX_AG_L2,
  BCM_DNX_AG_COPP,
  BCM_DNX_AG_SFLOW,
  BCM_DNX_AG_FREE,
  BCM_DNX_AG_MAX
} /*bcm_dnx_tcam_alloc_group_e*/;
/* No easy function to get this info, unit is 80 bits */
typedef enum bcm_dnx_tcam_fg_key_size_e {
  BCM_DNX_FG_KEY_DONT_CARE = 0,
  BCM_DNX_FG_KEY_80 = 1,
  BCM_DNX_FG_KEY_160 = 2,
  BCM_DNX_FG_KEY_320 = 4
} /*bcm_dnx_tcam_fg_key_size_t*/;

#define BCM_DNX_FG_KEY_SIZE_BASE_BITS 80
#define BCM_DNX_FG_ACTION_SIZE_BASE_BITS 32

typedef struct bcm_dnx_tcam_fg_info_t {
  bcm_field_group_tcam_info_t fg_tcam_state;
  int nof_entries;
  bcm_dnx_tcam_alloc_group_e alloc_group;
  bcm_dnx_tcam_fg_key_size_e fg_key_size;
} /*bcm_dnx_tcam_fg_info_t*/;
typedef struct bcm_dnx_tcam_bank_info_t {
  bcm_dnx_tcam_alloc_group_e alloc_group;
  int refcnt;
} /*bcm_dnx_tcam_bank_info_t*/;

#define BCM_DNX_MAX_FG_ID 32
#define BCM_DNX_LARGE_TCAMS 12

typedef struct bcm_dnx_tcam_info_t {
  bcm_dnx_tcam_fg_info_t fgs[BCM_DNX_MAX_FG_ID];
  bcm_dnx_tcam_bank_info_t banks[BCM_FIELD_NUMBER_OF_TCAM_BANKS];
  int entries_per_alloc_group[2 * BCM_DNX_AG_MAX];
  int keys_per_alloc_group[2 * BCM_DNX_AG_MAX];
  int total_keys_per_alloc_group[2 * BCM_DNX_AG_MAX];
} /*bcm_dnx_tcam_info_t*/;

static bcm_dnx_tcam_info_t g_bcm_dnx_tcam_info;

typedef struct bcm_dnx_sdk_fg_t {
  const char *fg_type_name;
  bcm_field_stage_t fg_stage;
  bcm_dnx_tcam_alloc_group_e tcam_alloc_group;
  bcm_dnx_tcam_fg_key_size_e fg_key_size;
  bcm_field_group_tcam_info_t tcam_info;
} /*bcm_dnx_tcam_info_t*/;

static bcm_dnx_sdk_fg_t bcm_dnx_sdk_fgs[] = {
    {"NONE",},
    {"IPV4", bcmFieldStageIngressPMF1, BCM_DNX_AG_V4V6, BCM_DNX_FG_KEY_160},
    {"IPV6", bcmFieldStageIngressPMF1, BCM_DNX_AG_V4V6, BCM_DNX_FG_KEY_320},
    {"L2", bcmFieldStageIngressPMF1, BCM_DNX_AG_L2, BCM_DNX_FG_KEY_160},
    {"QoSV4", bcmFieldStageIngressPMF2, BCM_DNX_AG_COPP, BCM_DNX_FG_KEY_160},
    {"QoSV6", bcmFieldStageIngressPMF2, BCM_DNX_AG_COPP, BCM_DNX_FG_KEY_320},
    {"QoSL2", bcmFieldStageIngressPMF2, BCM_DNX_AG_COPP, BCM_DNX_FG_KEY_160},
    {"MIX",},
    {"DEF_COPP", bcmFieldStageIngressPMF2, BCM_DNX_AG_DONT_CARE, BCM_DNX_FG_KEY_320,
      {1, {12,}, bcmFieldTcamBankAllocationModeSelect}},
    {"USR_COPP", bcmFieldStageIngressPMF2, BCM_DNX_AG_COPP, BCM_DNX_FG_KEY_320,},
    {"TUPLE", bcmFieldStageIngressPMF1, BCM_DNX_AG_V4V6, BCM_DNX_FG_KEY_320,},
    {"IPV6_PRE",},
    {"TUPLE6", bcmFieldStageIngressPMF1, BCM_DNX_AG_V4V6, BCM_DNX_FG_KEY_320,},
    {"MAX",},
    {"PRE_COPP", bcmFieldStageIngressPMF1, BCM_DNX_AG_DONT_CARE, BCM_DNX_FG_KEY_160,
      {1, {14,}, bcmFieldTcamBankAllocationModeSelect}},
    {"COPP_HDR", bcmFieldStageIngressPMF3, BCM_DNX_AG_SFLOW, BCM_DNX_FG_KEY_80,
      {1, {15,}, bcmFieldTcamBankAllocationModeSelect}},
};

typedef enum bcm_dnx_target_type_t {
  BCM_DNX_TARGET_NONE = 0,
  BCM_DNX_TARGET_PORT,
  BCM_DNX_TARGET_PV,
  BCM_DNX_TARGET_VLAN,
  BCM_DNX_TARGET_MAX
} /*bcm_dnx_target_type_t*/;
char *bcm_dnx_target_type_s[] = {
    "",
    "PORT",
    "PV",
    "VLAN"
};
//BCM_DNX_PMF_MAX  * BCM_DNX_FG_MAX * BCM_DNX_TARGET_MAX
//default is default context (0)
//static bcm_field_context_t fg_to_context_map[3][10][4];

typedef enum bcm_dnx_sdk_context_id_e {
  BCM_DNX_CONTEXT_INVALID = 0,
  BCM_DNX_CONTEXT_INGRESS_V4_TAG,
  BCM_DNX_CONTEXT_INGRESS_V4,
  BCM_DNX_CONTEXT_INGRESS_V6_TAG,
  BCM_DNX_CONTEXT_INGRESS_V6,
  BCM_DNX_CONTEXT_INGRESS_L2_TAG,
  BCM_DNX_CONTEXT_INGRESS_L2,
  BCM_DNX_CONTEXT_EGRESS_V4,
  BCM_DNX_CONTEXT_EGRESS_V6,
//  BCM_DNX_CONTEXT_COPP,
//  BCM_DNX_CONTEXT_COPP3,
  BCM_DNX_CONTEXT_COPP_V4_TAG,
  BCM_DNX_CONTEXT_COPP_V4,
  BCM_DNX_CONTEXT_COPP_V6_TAG,
  BCM_DNX_CONTEXT_COPP_V6,
  BCM_DNX_CONTEXT_COPP_L2_TAG,
  BCM_DNX_CONTEXT_COPP_L2,
  BCM_DNX_CONTEXT_MAX, // = 15,
  BCM_DNX_CONTEXT_INGRESS_V6_2 //cascaded for IPv6
};

typedef struct bcm_dnx_sdk_presel_t {
  bcm_field_presel_t presel_id; //lower has higher priority
  bcm_field_presel_qualify_data_t quals[3];
};

typedef struct bcm_dnx_sdk_context_t {
  bcm_field_stage_t stage;
  char *name;
  bcm_dnx_sdk_presel_t presels[6];
  bcm_dnx_sdk_context_id_e cascaded_from;
  uint8 hashing_enabled;
  bcm_field_context_t context_id;
} /*bcm_dnx_sdk_context_t*/;

static bcm_field_presel_qualify_data_t bcm_dnx_single_tagged =
//#define bcm_dnx_single_tagged
  {bcmFieldQualifyVlanFormat, 0, BCM_FIELD_VLAN_FORMAT_INNER_TAGGED, 0x1F };
//  {bcmFieldQualifyVlanFormat, 0, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED, 0x1F};
//0x0 seems to be the default class
#define BCM_DNX_COPP_CLASS 0x1
#define BCM_DNX_COPP_MATCH 0x8
static bcm_dnx_sdk_context_t bcm_dnx_sdk_contexts[] = {
    {bcmFieldStageCount}, //INVALID
    {
      bcmFieldStageIngressPMF1,
      "INGRESS_V4_TAG",
      {
        {
          BCM_DNX_ING_PRESEL_1,
          {
            bcm_dnx_single_tagged,
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp4, 0x1F},
          },
        },
      },
      BCM_DNX_CONTEXT_INVALID,
      1,
    },
    {
      bcmFieldStageIngressPMF1,
      "INGRESS_V4",
      {
        {
          BCM_DNX_ING_PRESEL_2,
          {
            {bcmFieldQualifyForwardingType, 0, bcmFieldLayerTypeIp4, 0x1F},
          },
        },
        {
          BCM_DNX_ING_PRESEL_3,
          {
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp4, 0x1F},
          },
        },
      },
      BCM_DNX_CONTEXT_INVALID,
      1,
    },
    {
      bcmFieldStageIngressPMF1,
      "INGRESS_V6_TAG",
      {
        {
          BCM_DNX_ING_PRESEL_4,
          {
            bcm_dnx_single_tagged,
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp6, 0x1F},
          },
        },
      },
      BCM_DNX_CONTEXT_INVALID,
      1,
    },
    {
      bcmFieldStageIngressPMF1,
      "INGRESS_V6",
      {
        {
          BCM_DNX_ING_PRESEL_5,
          {
            {bcmFieldQualifyForwardingType, 0, bcmFieldLayerTypeIp6, 0x1F},
          },
        },
        {
          BCM_DNX_ING_PRESEL_6,
          {
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp6, 0x1F},
          },
        },
      },
      BCM_DNX_CONTEXT_INVALID,
      1,
    },
    {
      bcmFieldStageIngressPMF1,
      "INGRESS_L2_TAG",
      {
        {
          BCM_DNX_ING_PRESEL_7,
          {
            bcm_dnx_single_tagged,
            {bcmFieldQualifyForwardingType, 0, bcmFieldLayerTypeEth, 0x1F},
          },
        },
      },
      BCM_DNX_CONTEXT_INVALID,
      1,
    },
    {
      bcmFieldStageIngressPMF1,
      "INGRESS_L2",
      {
        {
          BCM_DNX_ING_PRESEL_8,
          {
            {bcmFieldQualifyForwardingType, 0, bcmFieldLayerTypeEth, 0x1F},
          },
        },
      },
      BCM_DNX_CONTEXT_INVALID,
      1,
    },
    {
      bcmFieldStageEgress,
      "EGRESS_V4",
      {
        {
            BCM_DNX_EGR_PRESEL_1,
          {
            {bcmFieldQualifyForwardingType, 0, bcmFieldLayerTypeIp4, 0x1F},
          },
        },
        {
            BCM_DNX_EGR_PRESEL_2,
          {
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp4, 0x1F},
          },
        },
      },
    },
    {
      bcmFieldStageEgress,
      "EGRESS_V6",
      {
        {
            BCM_DNX_EGR_PRESEL_3,
          {
            {bcmFieldQualifyForwardingType, 0, bcmFieldLayerTypeIp6, 0x1F},
          },
        },
        {
            BCM_DNX_EGR_PRESEL_4,
          {
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp6, 0x1F},
          },
        },
      },
    },
    {
      bcmFieldStageIngressPMF2,
      "COPP_V4_TAG",
      {
        {
            BCM_DNX_ING_COPP_1,
        },
      },
      BCM_DNX_CONTEXT_INGRESS_V4_TAG,
    },
    {
      bcmFieldStageIngressPMF2,
      "COPP_V4",
      {
        {
            BCM_DNX_ING_COPP_2,
        },
      },
      BCM_DNX_CONTEXT_INGRESS_V4,
    },
    {
      bcmFieldStageIngressPMF2,
      "COPP_V6_TAG",
      {
        {
            BCM_DNX_ING_COPP_3,
        },
      },
      BCM_DNX_CONTEXT_INGRESS_V6_TAG,
    },
    {
      bcmFieldStageIngressPMF2,
      "COPP_V6",
      {
        {
            BCM_DNX_ING_COPP_4,
        },
      },
      BCM_DNX_CONTEXT_INGRESS_V6,
    },
    {
      bcmFieldStageIngressPMF2,
      "COPP_L2_TAG",
      {
        {
            BCM_DNX_ING_COPP_5,
        },
      },
      BCM_DNX_CONTEXT_INGRESS_L2_TAG,
    },
    {
      bcmFieldStageIngressPMF2,
      "COPP_L2",
      {
        {
            BCM_DNX_ING_COPP_6,
        },
      },
      BCM_DNX_CONTEXT_INGRESS_L2,
    },
#if 0
    {
        //TODO bcm_dnx_sdk_pre_copp_qual_fec dstport bcmPortClassFieldIngressPMF1TrafficManagementPortCs, bcmFieldQualifyPortClassTrafficManagement
        //TODO bcm_dnx_sdk_pre_copp_qual_cpu dstport
        //TODO bcm_dnx_sdk_pre_copp_qual_inport RCY_PORT srcport bcmPortClassFieldIngressPMF1PacketProcessingPortCs, bcmFieldQualifyPortClassPacketProcessing
        //TODO bcm_dnx_sdk_pre_copp_qual_lldp 88cc
      bcmFieldStageIngressPMF1, //we need 2 cascaded contexts to catch COPP both early on and from stage1
      "INGRESS_COPP",
      {
        {//bcm_dnx_sdk_pre_copp_qual_trap
          BCM_DNX_ING_COPP_1,
          {
              {bcmFieldQualifyRxTrapCode, 0, 0xf7, 0xff},
          },
        },
        {
          BCM_DNX_ING_COPP_2,
          {
              {bcmFieldQualifyPortClassTrafficManagement, 0, BCM_DNX_COPP_CLASS, 0xff},
          },
        },
        {
          BCM_DNX_ING_COPP_3,
          {
              {bcmFieldQualifyPortClassPacketProcessing, 0, BCM_DNX_COPP_CLASS, 0xff},
          },
        },
        {
          BCM_DNX_ING_COPP_4,
          {
              {bcmFieldQualifyForwardingType, 0, bcmFieldLayerTypeArp, 0x1F},
          },
        },
        { //bcm_dnx_sdk_pre_copp_qual_arp
          BCM_DNX_ING_COPP_5,
          {
              {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeArp, 0x1F},
          },
        },
      },
    },
    {
      bcmFieldStageIngressPMF3,
      "INGRESS_COPP3",
      {
        {
          BCM_DNX_ING_COPP_2,
          {
            {bcmFieldQualifyPortClassTrafficManagement, 0, BCM_DNX_COPP_CLASS, 0xff},
          },
        },
      },
    },
    {
        bcmFieldStageIngressPMF1,
        "INGRESS_V4_OUT",
        {
            {bcmFieldQualifyVlanFormat, 0, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED, 0x1F },
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp4, 0x1F},
        },
    },
    {
        bcmFieldStageIngressPMF1,
        4,
        "INGRESS_V4_OUT_8100",
        {
            {bcmFieldQualifyVlanFormat, 0x8100, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED, 0x1F },
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp4, 0x1F},
        },
    },
    {
        bcmFieldStageIngressPMF1,
        5,
        "INGRESS_V4_IN_8100",
        {
            {bcmFieldQualifyVlanFormat, 0x8100, BCM_FIELD_VLAN_FORMAT_INNER_TAGGED, 0x1F },
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp4, 0x1F},
        },
    },
    {
        bcmFieldStageIngressPMF1,
        6,
        "INGRESS_V4_IN_1",
        {
            {bcmFieldQualifyVlanFormat, 1, BCM_FIELD_VLAN_FORMAT_INNER_TAGGED, 0x1F },
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp4, 0x1F},
        },
    },
    {
        bcmFieldStageIngressPMF1,
        7,
        "INGRESS_V4_OUT_1",
        {
            {bcmFieldQualifyVlanFormat, 1, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED, 0x1f },
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp4, 0x1F},
        },
    },
    {
        bcmFieldStageIngressPMF1,
        8,
        "INGRESS_V4_0_1_1",
        {
            {bcmFieldQualifyVlanFormat, 0, 1,1 },
            {bcmFieldQualifyForwardingType, 1, bcmFieldLayerTypeIp4, 0x1F},
        },
    },
#endif
};


static int
bcm_dnx_pmf_hash_create (int unit, bcm_field_context_t context_id, bcm_field_context_hash_action_key_t action_key)
{
  bcm_field_context_hash_info_t hash_info;
  bcm_field_context_hash_info_t_init(&hash_info);
  hash_info.hash_function = bcmFieldContextHashFunctionCrc16Bisync;
  hash_info.order = TRUE;
  hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
  //hash_info.hash_config.function_select = bcmFieldContextHashActionValueAugmentCrc;
  hash_info.hash_config.action_key = action_key;
  hash_info.key_info.nof_quals = 2;
  hash_info.key_info.qual_types[0] = bcmFieldQualifyInPort;
  hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
  hash_info.key_info.qual_info[0].input_arg = 1;
  hash_info.key_info.qual_info[0].offset = 0;
  hash_info.key_info.qual_types[1] = bcmFieldQualifyEcmpLoadBalanceKey0;
  hash_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
  hash_info.key_info.qual_info[1].input_arg = 1;
  hash_info.key_info.qual_info[1].offset = 0;
  return bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1,context_id,&hash_info );
}

static int
bcm_dnx_pmf_context_create (int unit, bcm_dnx_sdk_context_t *context)
{
  int rv;
  bcm_field_context_info_t context_info;
  bcm_field_context_info_t_init(&context_info);
  context_info.hashing_enabled = context->hashing_enabled;
  if (BCM_DNX_CONTEXT_INVALID != context->cascaded_from) {
    context_info.cascaded_from =
        bcm_dnx_sdk_contexts[context->cascaded_from].context_id;
  }
  void *dest_char = &(context_info.name[0]);
  sal_strncpy_s(dest_char, context->name,
                sizeof(context_info.name));
  rv = bcm_field_context_create(unit, 0, context->stage,
                                &context_info,
                                &context->context_id);
  if (rv != BCM_E_NONE) {
    printf("Error (%d), in bcm_field_context_create name:%s\n", rv,
           context->name);
    return rv;
  }
  if (context->hashing_enabled) {
    bcm_field_context_hash_action_key_t key =
        bcmFieldContextHashActionKeyEcmpLbKey0;
    rv = bcm_dnx_pmf_hash_create(unit, context->context_id, key);
    if (rv != BCM_E_NONE) {
      printf("Error (%d), in bcm_dnx_pmf_hash_create name:%s ID:%d key:%d\n",
             rv, context->name, context->context_id, key);
      return rv;
    }
  }
  return BCM_E_NONE;
}
static int bcm_dnx_pmf_presel_set (int unit, bcm_dnx_sdk_context_t *context, int destroy)
{
  int rv, j, p = 0;
  bcm_field_presel_entry_data_t p_data;
  bcm_field_presel_entry_id_t p_id;
  bcm_field_presel_entry_id_info_init(&p_id);
  bcm_field_presel_entry_data_info_init(&p_data);
  p_id.stage = context->stage;
  p_data.context_id = context->context_id;
  if (destroy) {
    p_data.entry_valid = FALSE;
    j = 1;
  } else {
    p_data.entry_valid = TRUE;
  }
  //qual_type 0 is (hopefully) not a valid context preselector
  while (context->presels[p].quals[0].qual_type) {
    p_id.presel_id = context->presels[p].presel_id;
    p_data.nof_qualifiers = j = 0;
    if (!destroy) {
      while (context->presels[p].quals[j].qual_type) {
        p_data.qual_data[p_data.nof_qualifiers++] =
            context->presels[p].quals[j++];
      }
    }
    if (j || destroy) {
      rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
      if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_field_presel_set%s id:%d\n", rv,
               destroy ? "(destroy)" : "", p_id.presel_id);
        if (!destroy) {
          return rv;
        }
      }
    }
    p ++;
  }
  return BCM_E_NONE;
}

static ret_code_e
bcm_dnx_sdk_copp_presel_set(int unit, bcm_dnx_sdk_context_t *context, bcm_field_group_t id, int destroy)
{
  int rv;
  context->presels[0].quals[0].qual_type = bcmFieldQualifyCascadedKeyValue;
  context->presels[0].quals[0].qual_arg = id;
  context->presels[0].quals[0].qual_value = BCM_DNX_COPP_MATCH;
  context->presels[0].quals[0].qual_mask = 0xf;
  rv = bcm_dnx_pmf_presel_set(unit, context, destroy);
  if (destroy && (BCM_E_NONE == rv)) {
    context->presels[0].quals[0].qual_type = 0;
  }
  return BCM_E_NONE == rv ? RET_CODE_SUCCESS : RET_CODE_FAILURE;
}

static int
bcm_dnx_pmf_contexts_setup (int unit, int pv)
{
  int i, rv;
  for (i = BCM_DNX_CONTEXT_INGRESS_V4_TAG; i < BCM_DNX_CONTEXT_MAX; i++) {
    if (pv
        && ((i == BCM_DNX_CONTEXT_INGRESS_V4_TAG)
            || (i == BCM_DNX_CONTEXT_INGRESS_V6_TAG)
            || (i == BCM_DNX_CONTEXT_INGRESS_L2_TAG)
            || (i == BCM_DNX_CONTEXT_COPP_V4_TAG)
            || (i == BCM_DNX_CONTEXT_COPP_V6_TAG)
            || (i == BCM_DNX_CONTEXT_COPP_L2_TAG)))
      continue;
    rv = bcm_dnx_pmf_context_create(unit, bcm_dnx_sdk_contexts + i);
    if (rv != BCM_E_NONE) {
      return rv;
    }
    rv = bcm_dnx_pmf_presel_set(unit, bcm_dnx_sdk_contexts + i, 0);
    if (rv != BCM_E_NONE) {
      return rv;
    }
  }
  return 0;
}

#define DIM(n) sizeof(n) / sizeof((n)[0])

static int
bcm_dnx_pmf_contexts_destroy(int unit)
{
  int i, rv;
  for (i = BCM_DNX_CONTEXT_MAX - 1; i > BCM_DNX_CONTEXT_INVALID;  i--) {
    if (bcm_dnx_sdk_contexts[i].context_id) {
      bcm_dnx_pmf_presel_set(unit, bcm_dnx_sdk_contexts + i, 1);
      if (bcm_dnx_sdk_contexts[i].hashing_enabled) {
        rv = bcm_field_context_hash_destroy(unit, bcm_dnx_sdk_contexts[i].stage,
                                            bcm_dnx_sdk_contexts[i].context_id);
        if (rv != BCM_E_NONE) {
          printf(
              "Error (%d), in bcm_field_context_hash_destroy name:%s id:%d\n",
              rv, bcm_dnx_sdk_contexts[i].name,
              bcm_dnx_sdk_contexts[i].context_id);
          //return rv;
        }
      }
      rv = bcm_field_context_destroy(unit, bcm_dnx_sdk_contexts[i].stage,
                                     bcm_dnx_sdk_contexts[i].context_id);
      if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_field_context_destroy name:%s id:%d\n", rv,
               bcm_dnx_sdk_contexts[i].name,
               bcm_dnx_sdk_contexts[i].context_id);
        //return rv;
      } else {
        bcm_dnx_sdk_contexts[i].context_id = 0;
      }
    }
  }
}
//const int fg_id = 100, context_id = 0, fe_id = 0, drop_aid = 2072, drop_aid_2 = 2073, drop_aid_3 = 2074, copp_aid = 2075;
//const int snoop_aid = 2076;
const int udq_dscp = 2100, udq_ports = 2103, udq_frag = 2104;
const int udq_options = 2105, udq_ttl_proto = 2106;
const int udq_tcpflags = 2107, udq_vsi = 2108, udq_pr_v4 = 2109;
const int udq_pr_v6 = 2110;

uint32 g_tuple_flag = 0;

//typedef enum bcm_dnx_sdk_udq_t {
//  udq_dscp = 2084,
//  udq_ttl,
//  udq_proto,
//  udq_ports,
//  udq_frag,
//  udq_options
//};

typedef struct bcm_dnx_sdk_udq_def_t {
  int /*bcm_dnx_sdk_udq_t*/ udq;
  uint32 size;
  char *name;
};
#define BCM_DNX_SDK_VSI_BITS 13
/* Port range matcher */
#define BCM_DNX_J2_PR_BITS 24
#define BCM_DNX_J2_PR_V4_BITS 10
#define BCM_DNX_J2_PR_V6_BITS 23

static bcm_dnx_sdk_udq_def_t bcm_dnx_sdk_udq_defs[] = {
  {udq_dscp, 6, "DSCP",},
  {udq_ports, 32, "L4SrcDst",},
  {udq_frag, 1, "Fragmented",},
  {udq_options, 1, "IPOptions",},
  {udq_ttl_proto, 16, "TTLProto",},
  /* at the moment it is just the standard 6 bits */
  {udq_tcpflags, 6, "TCPFlags",},
  {udq_vsi, BCM_DNX_SDK_VSI_BITS, "VSI",},
  {udq_pr_v4, BCM_DNX_J2_PR_V4_BITS, "L4v4Range",},
  {udq_pr_v6, BCM_DNX_J2_PR_V6_BITS, "L4v6Range",}
};

typedef struct bcm_dnx_field_db_t {
  int unit;
  bcm_field_qualify_t ud_icmp[2];
  bcm_field_qualify_t ud_ether[1];
  bcm_field_qualify_t ud_trap;
  bcm_field_presel_t presel_ingress_g;
  bcm_field_presel_t presel_ingress[10];
  bcm_field_presel_t presel_egress_g;
  bcm_field_presel_t presel_egress[10];
  bcm_gport_t fwd_trap_id;
  int ff_trap_id;
  //bcm_dnx_range_entry_t *range_list[BCM_DNX_RANGE_MAX];
  bcm_gport_t host_snoop_id;
  bcm_field_group_t pre_copp_group_id;
  bcm_field_group_t egr_group_id;
  bcm_field_group_t egrl2_group_id;
  bcm_field_group_t egr6_group_id;
  bcm_field_group_t ing6_group_id;
  uint32_t egr_group_id_ref;
  int cascade_id;
} /*bcm_dnx_field_db_t*/;

typedef struct bcm_dnx_group_tuple_t {
  bcm_dnx_fg_type_t type;
  bcm_dnx_fg_direction_t direction;
  bcm_dnx_target_type_t target_type;
  bcm_field_group_t group_id;
  bcm_field_presel_t presel_id;
  int priority;
  bcm_dnx_field_db_t *ctxt;
  //bcm_dnx_sdk_counter_source_t source;
  uint32 tuple_flag;
} /*bcm_dnx_group_tuple_t*/;

typedef struct bcm_dnx_entry_tuple_t {
  bcm_field_entry_t entry_id;
  bcm_field_entry_t entry_id__0;
  bcm_field_entry_t entry_id__1;
  //FE under construction
  bcm_field_entry_info_t *ent_info;
  int flags;
  bcm_dnx_fg_type_t type;
  bcm_dnx_fg_direction_t direction;
  bcm_field_range_t range_id[2];
  bcm_policer_t policer_id;
  bcm_policer_t policer_id__0;
  int priority;
  bcm_dnx_field_db_t *ctxt;
//  bcm_dnx_sdk_counter_t *counter_ctxt;
//  bcm_dnx_sdk_counter_source_t source;
  unsigned int counter_id;
  unsigned int policer_counter_id;
//  bool tuple_flag;
} /*bcm_dnx_entry_tuple_t*/;

typedef struct bcm_dnx_sdk_field_t {
  bcm_field_qualify_t qual_type;
  bcm_field_qualify_attach_info_t attach_info;
  bcm_field_qualify_t base_qual_id;
  int offset;
} /*bcm_dnx_sdk_field_t*/;

typedef struct bcm_dnx_sdk_action_t {
  bcm_field_action_t action;
  int supress_valid_bit;
};
static bcm_field_qualify_attach_info_t
  MetaData = {bcmFieldInputTypeMetaData},
  LayerAbsoluteIp = {bcmFieldInputTypeLayerAbsolute, 1},
  LayerAbsoluteL4 = {bcmFieldInputTypeLayerAbsolute, 2},
  LayerAbsoluteL2 = {bcmFieldInputTypeLayerAbsolute},
  LayerAbsolute1q = {bcmFieldInputTypeLayerAbsolute, 0, 96},
  RecordsAbsoluteIp = {bcmFieldInputTypeLayerRecordsAbsolute, 1, 0};

void bcmsdk_trace_acl_gen_error (const char *app_err, const char *sdk_err)
{
  printf("ACL app : %s, sdk : %s\n", app_err, sdk_err);
  return;
}
void bcmsdk_trace_acl_fp_grp_error(const char *app_err, const char *sdk_err,
                                   int dir, int tgt_type, int acl_type,
                                   uint32_t grp_id, uint32_t unit, uint32_t rc)
{
  printf("unit %d app : %s  grp_id %u \n",
                               unit, app_err,  grp_id);
  print dir;
  print tgt_type;
  print acl_type;
  print rc;
  return;
}

static int init_test_run = 0;

//#define vrf_aid (aid_first + 5)
#define aid_first 2072
#define drop_aid (aid_first + 0)
#define redir_aid_2 (aid_first + 1)
#define copp_aid (aid_first + 2)
#define snoop_aid (aid_first + 3)
#define redir_aid (aid_first + 4)
#define mirror_aid (aid_first + 5)

typedef struct bcm_dnx_sdk_uda_def_t {
  bcm_field_action_t uda_id;
  const char *name;
  bcm_field_action_t base_action_type;
  int prefix_size;
  int size;
  bcm_field_stage_t stage;
  int prefix_value;
}/*bcm_dnx_sdk_uda_def_t*/;

static bcm_dnx_sdk_uda_def_t bcm_dnx_sdk_uda_defs[] = {
    {drop_aid, "action_drop_1", bcmFieldActionForward, 31, 1, bcmFieldStageIngressPMF1, 0x000FFFFF},
    {redir_aid_2, "action_redir_2", bcmFieldActionForward, 11, 21, bcmFieldStageIngressPMF2, 0x0},
    {copp_aid, "action_void", bcmFieldActionVoid, 31, 1, bcmFieldStageIngressPMF1, 0x0},
    {snoop_aid, "action_snoop", bcmFieldActionSnoop, 19, 1, bcmFieldStageIngressPMF1,/* will be updated */},
    {redir_aid, "action_redir", bcmFieldActionForward, 11, 21, bcmFieldStageIngressPMF1, 0x0},
    {mirror_aid, "action_mirror", bcmFieldActionMirrorIngress, 13, 4, bcmFieldStageIngressPMF1, 0x1E},
};

static int
bcm_dnx_pmf_setup_action (int unit, bcm_field_action_t id,
    bcm_field_action_info_t *action_info, const char *name)
{
  int rv;
  bcm_field_action_t new_id = id;
  void *dest_char;
  dest_char = &(action_info->name[0]);
  sal_strncpy_s(dest_char, name, sizeof(action_info->name));
  rv = bcm_field_action_create(unit, BCM_FIELD_FLAG_WITH_ID, action_info, &new_id);
  if(rv != BCM_E_NONE || new_id != id)
  {
      printf("bcm_field_action_create for with id:is failed, new id:\n");
      return rv;
  }
  printf(" was created with id:() \n");
  return rv;
}

int
bcm_dnx_pmf_setup_actions (int unit)
{
  int i, rv;
  bcm_dnx_sdk_uda_def_t *d = bcm_dnx_sdk_uda_defs;
  bcm_field_action_info_t action_info;
  for (i = 0; i < DIM(bcm_dnx_sdk_uda_defs); i ++, d ++) {
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = d->base_action_type;
    action_info.prefix_size = d->prefix_size;
    action_info.size = d->size;
    action_info.stage = d->stage;
    action_info.prefix_value = d->prefix_value;
    rv = bcm_dnx_pmf_setup_action(unit, d->uda_id, &action_info, d->name);
    if (rv) {
      return rv;
    }
  }
  return 0;
}

int cint_field_action_drop_main_trap(int unit ,bcm_field_stage_t stage,bcm_field_action_t *action_drop_id)
{
    bcm_field_action_info_t action_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    bcm_gport_t trap_gport;
    int id;
    rv = bcm_rx_trap_type_get(unit,0, bcmRxTrapDfltDroppedPacket, &id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_type_get\n", rv);
        return rv;
    }
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionTrap;
    action_info.prefix_size = 31; //24;
    /* lsb 1 comes from entry payload */
    BCM_GPORT_TRAP_SET(trap_gport, id, 0, 0);
    print trap_gport;
    action_info.prefix_value = trap_gport >> 1;
    action_info.size = 1; //8;
    action_info.stage = stage;
    dest_char = &(action_info.name[0]);
    char name[BCM_FIELD_MAX_SHORT_NAME_LEN];
    snprintf(name, BCM_FIELD_MAX_SHORT_NAME_LEN, "action_drop_%d", *action_drop_id == drop_aid ? 1 : 2);
    sal_strncpy_s(dest_char, name, sizeof(action_info.name));
    rv = bcm_field_action_create(unit, BCM_FIELD_FLAG_WITH_ID, &action_info, action_drop_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_create (%s)\n", name);
        return rv;
    }
    int int_id = *action_drop_id;
    printf("%s was created with id:(%d) \n", name, int_id);
    return 0;

}

static int
bcm_dnx_pmf_udh_action(int unit, bcm_field_action_t *id)
{
  bcm_field_action_info_t action_info;
  void *dest_char;
  bcm_field_action_info_t_init(&action_info);
  action_info.action_type = bcmFieldActionUDHData0;
  action_info.prefix_size = 8;
  action_info.prefix_value = 0x0;
  action_info.size = 24;
  action_info.stage = bcmFieldStageIngressPMF2;
  dest_char = &(action_info.name[0]);
  char name[BCM_FIELD_MAX_SHORT_NAME_LEN];
  snprintf(name, BCM_FIELD_MAX_SHORT_NAME_LEN, "action_udh");
  sal_strncpy_s(dest_char, name, sizeof(action_info.name));
  int rv = bcm_field_action_create(unit, BCM_FIELD_FLAG_WITH_ID, &action_info, id);
  if(rv != BCM_E_NONE)
  {
      printf("Error in bcm_field_action_create (%s)\n", name);
      return rv;
  }
  int int_id = *id;
  printf("%s was created with id:(%d) \n", name, int_id);
  return 0;
}



static void
bcm_dnx_pmf_setup_action_snoop(int unit, bcm_gport_t snoop_gport_id)
{
  bcm_dnx_sdk_uda_defs[snoop_aid - aid_first].prefix_value =
      ((snoop_gport_id & 0x1ff)
      | ((BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(snoop_gport_id) & 0xf) << 9)) >> 1;
}

static int
bcm_dnx_pmf_ud_qual(int unit, uint32 size, bcm_field_qualify_t qual_id, char *qname)
{
  bcm_field_qualifier_info_create_t qual_info;
  uint32 qual_print;
  void *dest_char;
  int rv = BCM_E_NONE;

  bcm_field_qualifier_info_create_t_init(&qual_info);
  qual_info.size = size;
  dest_char = &(qual_info.name[0]);
  char name[BCM_FIELD_MAX_SHORT_NAME_LEN];
  snprintf(name, BCM_FIELD_MAX_SHORT_NAME_LEN, "udq_%d_%s", size, qname);
  sal_strncpy_s(dest_char, name, sizeof(qual_info.name));

  rv = bcm_field_qualifier_create(unit, BCM_FIELD_FLAG_WITH_ID, &qual_info, &qual_id);
  if (rv != BCM_E_NONE)
  {
      printf("Error (%d), in bcm_field_qualifier_create(%s)\n", rv, name);
      return rv;
  }
  qual_print = qual_id;
  printf("Created Qualifier (0x%x)  \n",qual_print);
  return 0;
}

static int
bcm_dnx_pmf_ud_quals(int unit)
{
  int i, rv;
  bcm_dnx_sdk_udq_def_t *d = bcm_dnx_sdk_udq_defs;
  for (i = 0; i < DIM(bcm_dnx_sdk_udq_defs); i ++, d ++) {
    rv = bcm_dnx_pmf_ud_qual(unit, d->size, d->udq, d->name);
    if (rv) {
      return rv;
    }
  }
  return 0;
}

#define BC(b, v, msg) \
  if ((b) < (v)) { \
    printf("%s (can't go for %d when max is %d)", msg, v, b); \
    return -1; \
}

int
bcm_dnx_fset_offset (int unit, bcm_field_qualify_attach_info_t *qual_info,
    bcm_field_group_info_t *fg_info, bcm_dnx_sdk_field_t *field)
{
  int rv;
  bcm_field_qualifier_info_get_t qual_info_get;
  bcm_field_qualifier_info_get_t_init(&qual_info_get);
  if (field->qual_type < udq_dscp || !field->base_qual_id) {
    return 0;
  }
  rv = bcm_field_qualifier_info_get(unit, field->base_qual_id, fg_info->stage, &qual_info_get);
  if(rv != BCM_E_NONE)
  {
      printf("Error (%d) in bcm_field_qualifier_info_get\n", rv);
      return rv;
  }
  qual_info->offset = qual_info_get.offset + field->offset;
  return 0;
}

int
bcm_dnx_fset_add (int unit, bcm_field_group_info_t *fg_info,
    bcm_field_group_attach_info_t *attach_info, bcm_dnx_sdk_field_t *field,
    int num)
{
  int rv;
  BC(DIM(fg_info->qual_types), fg_info->nof_quals + num, "FG is full");
  BC(DIM(attach_info->key_info.qual_types), fg_info->nof_quals + num,
     "Context is full");
  while (num--) {
    fg_info->qual_types[fg_info->nof_quals++] = field->qual_type;
    attach_info->key_info.qual_types[attach_info->key_info.nof_quals] =
        field->qual_type;
    attach_info->key_info.qual_info[attach_info->key_info.nof_quals] =
        field->attach_info;
    rv = bcm_dnx_fset_offset(
        unit, attach_info->key_info.qual_info + attach_info->key_info.nof_quals,
        fg_info, field);
    if(rv != BCM_E_NONE)
    {
        printf("bcm_dnx_fset_offset failed with %d\n", rv);
        return rv;
    }
    field++;
    attach_info->key_info.nof_quals++;
  }
  return 0;
}

ret_code_e
bcm_dnx_fset_patch (int unit, bcm_field_group_info_t *fg_info,
    bcm_field_group_attach_info_t *attach_info, bcm_dnx_sdk_field_t *patch,
    int num)
{
  int rv;
  int ent;
  attach_info->key_info.nof_quals;
  for (ent = 0; ent < attach_info->key_info.nof_quals && num; ent++) {
    if (patch->qual_type == attach_info->key_info.qual_types[ent]) {
      attach_info->key_info.qual_info[ent] = patch->attach_info;
      bcm_dnx_fset_offset(unit, attach_info->key_info.qual_info + ent, fg_info,
                          patch);
      if(rv != BCM_E_NONE)
      {
          return RET_CODE_FAILURE;
      }
      patch++;
      num--;
    }
  }
  return RET_CODE_SUCCESS;
}

int
bcm_dnx_aset_add (bcm_field_group_info_t *fg_info,
    bcm_field_group_attach_info_t *attach_info, bcm_dnx_sdk_action_t *action,
    int num, int prio)
{
  BC(DIM(fg_info->action_types), fg_info->nof_actions + num,
     "FG action set is full");
  BC(DIM(attach_info->payload_info.action_types),
     attach_info->payload_info.nof_actions + num, "Context action set is full");
  while (num--) {
    if (action->supress_valid_bit) {
      fg_info->action_with_valid_bit[fg_info->nof_actions] = FALSE;
    }
    fg_info->action_types[fg_info->nof_actions++] = action->action;
    attach_info->payload_info.action_info[attach_info->payload_info.nof_actions].priority =
        ((prio == BCM_DNX_SDK_GROUP_PRIORITY_ANY) || action->supress_valid_bit) ?
            BCM_FIELD_ACTION_DONT_CARE : BCM_FIELD_ACTION_PRIORITY(0, prio);
    attach_info->payload_info.action_types[attach_info->payload_info.nof_actions++] =
        action->action;
    action++;
  }
  return 0;
}
#define BCV(e)                                                                \
    if (sizeof(ent_info->e[i].value) < bytes) {                               \
        printf("Entry %s can't hold %d bytes)\n", #e, bytes);                 \
        return -1;                                                            \
      }
int
bcm_dnx_fe_qual_add (bcm_field_entry_info_t *ent_info,
    bcm_field_qualify_t type, uint32 *val, uint32 *mask, int bytes)
{
  BC(DIM(ent_info->entry_qual), ent_info->nof_entry_quals + 1,
       "FE is full");
  int i = ent_info->nof_entry_quals;
  BCV(entry_qual);
  ent_info->entry_qual[i].type = type;
  sal_memcpy(ent_info->entry_qual[i].value, val, bytes);
  sal_memcpy(ent_info->entry_qual[i].mask, mask, bytes);
  ent_info->nof_entry_quals ++;
  return 0;
}

int
bcm_dnx_fe_qual_add32 (bcm_field_entry_info_t *ent_info,
    bcm_field_qualify_t type, uint32 val, uint32 mask)
{
  BC(DIM(ent_info->entry_qual), ent_info->nof_entry_quals + 1,
       "FE is full");
  int i = ent_info->nof_entry_quals;
  ent_info->entry_qual[i].type = type;
  ent_info->entry_qual[i].value[0] = val;
  ent_info->entry_qual[i].mask[0] = mask;
  ent_info->nof_entry_quals ++;
  return 0;
}

int
bcm_dnx_fe_action_add (bcm_field_entry_info_t *ent_info,
    bcm_field_action_t type, uint32 *val, int bytes)
{
  BC(DIM(ent_info->entry_action), ent_info->nof_entry_actions + 1,
       "FE action set is full");
  int i = ent_info->nof_entry_actions;
  BCV(entry_action);
  ent_info->entry_action[i].type = type;
  sal_memcpy(ent_info->entry_action[i].value, val, bytes);
  ent_info->nof_entry_actions ++;
  return 0;
}
int
bcm_dnx_fe_action_add32 (bcm_field_entry_info_t *ent_info,
    bcm_field_action_t type, uint32 val)
{
  BC(DIM(ent_info->entry_action), ent_info->nof_entry_actions + 1,
       "FE action set is full");
  int i = ent_info->nof_entry_actions;
  ent_info->entry_action[i].type = type;
  ent_info->entry_action[i].value[0]= val;
  ent_info->nof_entry_actions ++;
  return 0;
}

static bcm_dnx_sdk_field_t l2_fg_qualify[] = {
  {bcmFieldQualifySrcMac, LayerAbsoluteL2},
  {bcmFieldQualifyDstMac, LayerAbsoluteL2},
  {bcmFieldQualifyLayerRecordType, {bcmFieldInputTypeLayerRecordsAbsolute, 1, 0}},
};

static bcm_dnx_sdk_field_t in_l2_fg_qualify[] = {
  {bcmFieldQualifyEtherTypeUntagged, LayerAbsoluteL2}
};

static bcm_dnx_sdk_field_t out_l2_fg_qualify[] = {
  {bcmFieldQualifyEtherTypeUntagged, LayerAbsoluteL2}
};
//ipv4 only
//NOTE the fact of ipv4 packet should come from the context
static bcm_dnx_sdk_field_t l3_fg_qualify[] = {
  {udq_ttl_proto, LayerAbsoluteIp, bcmFieldQualifyIp4Ttl},
  {udq_dscp, LayerAbsoluteIp, bcmFieldQualifyIp4Tos},
  {udq_options, RecordsAbsoluteIp, bcmFieldQualifyIpHasOptions},
};

static bcm_dnx_sdk_field_t v4_ipproto_qualify[] = {
  {udq_ttl_proto, LayerAbsoluteIp, bcmFieldQualifyIp4Protocol},
};

////TODO adopt bcm_dnx_sdk_fe_set_IpType() bcmFieldQualifyIpType user
////bcmFieldQualifyIpProtocolCommon is metadata used to pick ICMP
////TODO adopt bcm_dnx_sdk_fe_set_IcmpTypeCode()
////ipv4 only
//static bcm_dnx_sdk_field_t in_l3_fg_qualify[] = {
//  {udq_ttl, LayerAbsoluteIp,bcmFieldQualifyIp4Ttl},
//  //bcmFieldQualifyIpProtocolCommon,
//};
//ipv6 only
static bcm_dnx_sdk_field_t v6_fg_qualify[] = {
  {udq_ttl_proto, LayerAbsoluteIp, bcmFieldQualifyIp6NextHeader}, //HL second byte
  {udq_dscp, LayerAbsoluteIp, bcmFieldQualifyDSCP},
  {udq_options, RecordsAbsoluteIp, bcmFieldQualifyIp6FirstAdditionalHeaderExist},
};

//static bcm_dnx_sdk_field_t v6_ipproto_qualify[] = {
//  {udq_proto, LayerAbsoluteIp, bcmFieldQualifyIpProtocol},
//};

//ipv4 only
//static bcm_dnx_sdk_field_t out_l3_fg_qualify[] = {
//  {udq_ttl, LayerAbsoluteIp,bcmFieldQualifyIp4Ttl},
//};

static bcm_dnx_sdk_field_t in_port_fg_qualify[] = {
  {bcmFieldQualifyInPort, MetaData}
};

static bcm_dnx_sdk_field_t out_port_fg_qualify[] = {
  {bcmFieldQualifyDstPort, MetaData}
};

static bcm_dnx_sdk_field_t vlan_fg_qualify[] = {
  {udq_vsi, MetaData, bcmFieldQualifyInVPort0Raw,}
};

static bcm_dnx_sdk_field_t out_vlan_fg_qualify[] = {
  {udq_vsi, MetaData, bcmFieldQualifyOutVPort0Raw,}
};
//TODO
//static bcm_dnx_sdk_field_t mpls_fg_qualify[] = {
//  bcmFieldQualifyMplsForwardingLabelId,
//};
//ipv4 only
static bcm_dnx_sdk_field_t v4_fg_qualify[] = {
  {bcmFieldQualifySrcIp, LayerAbsoluteIp},
  {bcmFieldQualifyDstIp, LayerAbsoluteIp},
  {udq_frag, RecordsAbsoluteIp, bcmFieldQualifyIpFrag},
};

static bcm_dnx_sdk_field_t v4_src_qualify[] = {
  {bcmFieldQualifySrcIp, LayerAbsoluteIp},
};

static bcm_dnx_sdk_field_t v4_dst_qualify[] = {
  {bcmFieldQualifyDstIp, LayerAbsoluteIp},
};

static bcm_dnx_sdk_field_t L4_fg_qualify[] = {
  {udq_ports, LayerAbsoluteL4, bcmFieldQualifyL4SrcPort},
  {udq_tcpflags, LayerAbsoluteL4, bcmFieldQualifyTcpControl},
};

static bcm_dnx_sdk_field_t L4_fg_limited_qualify[] = {
  {udq_ports, LayerAbsoluteL4, bcmFieldQualifyL4SrcPort},
};
//ipv4 only
static bcm_dnx_sdk_field_t tuple_fg_qualify[] = {
  {udq_options, RecordsAbsoluteIp, bcmFieldQualifyIpHasOptions},
  {bcmFieldQualifySrcMac, LayerAbsoluteL2},
  {bcmFieldQualifyDstMac, LayerAbsoluteL2},
  {bcmFieldQualifySrcIp, LayerAbsoluteIp},
  {bcmFieldQualifyDstIp, LayerAbsoluteIp},
};
//ipv6 only
static bcm_dnx_sdk_field_t in_tuple6_fg_qualify[] = {
  {udq_options, RecordsAbsoluteIp, bcmFieldQualifyIp6FirstAdditionalHeaderExist},
  {bcmFieldQualifySrcMac, LayerAbsoluteL2},
  {bcmFieldQualifyDstMac, LayerAbsoluteL2},
  {bcmFieldQualifySrcIp6High, LayerAbsoluteIp},
  {bcmFieldQualifyDstIp6High, LayerAbsoluteIp},
  //try to get ICMP {type,code} from L4 srcPort
  {udq_ttl_proto, LayerAbsoluteIp, bcmFieldQualifyIpProtocol},
  {bcmFieldQualifyL4SrcPort, LayerAbsoluteL4},
};
//ipv6 only
static bcm_dnx_sdk_field_t out_tuple6_fg_qualify[] = {
  {udq_options, RecordsAbsoluteIp, bcmFieldQualifyIp6FirstAdditionalHeaderExist},
  {bcmFieldQualifySrcMac, LayerAbsoluteL2},
  {bcmFieldQualifyDstMac, LayerAbsoluteL2},
};

static bcm_dnx_sdk_field_t in_tuple_fg_qualify[] = {
  {bcmFieldQualifyEtherTypeUntagged, LayerAbsoluteL2},
};

static bcm_dnx_sdk_field_t out_tuple_fg_qualify[] = {
  {bcmFieldQualifyEtherTypeUntagged, LayerAbsoluteL2},
};
//ipv6 only
static bcm_dnx_sdk_field_t in_v6_fg_qualify[] = {
  //bcmFieldQualifyIpProtocolCommon,
  {bcmFieldQualifyIp6FlowLabel, LayerAbsoluteIp},
  {bcmFieldQualifySrcIp6High, LayerAbsoluteIp},
  {bcmFieldQualifyDstIp6, LayerAbsoluteIp},
};

static bcm_dnx_sdk_field_t v6_src_qualify[] = {
  {bcmFieldQualifySrcIp6High, LayerAbsoluteIp},
};

static bcm_dnx_sdk_field_t v6_dst_qualify[] = {
  {bcmFieldQualifyDstIp6, LayerAbsoluteIp},
};
/* v6 implicit rules need these qualifiers */
static bcm_dnx_sdk_field_t v6_limited_qualify[] = {
    {udq_ttl_proto, LayerAbsoluteIp, bcmFieldQualifyIp6NextHeader},
    {udq_ports, LayerAbsoluteL4, bcmFieldQualifyL4SrcPort},
};
/* this is what remains from L4 ports for v6 limited */
static bcm_dnx_sdk_field_t v6_limited_L4[] = {
    {bcmFieldQualifyL4DstPort, LayerAbsoluteL4},
};
//ipv6 only
static bcm_dnx_sdk_field_t pre_v6_fg_qualify[] = {
  //bcmFieldQualifyIpProtocolCommon,
  {bcmFieldQualifySrcIp6High, LayerAbsoluteIp},
  {bcmFieldQualifyDstIp6, LayerAbsoluteIp},
  //try to get ICMP {type,code} from L4 srcPort
  {udq_ttl_proto, LayerAbsoluteIp, bcmFieldQualifyIpProtocol},
  {bcmFieldQualifyL4SrcPort, LayerAbsoluteL4}
};
//ipv6 only
static bcm_dnx_sdk_field_t out_v6_fg_qualify[] = {
  {bcmFieldQualifyIp6FlowLabel, LayerAbsoluteIp},
  //bcmFieldQualifyDstClassField, //TODO cascade between I and E
  {bcmFieldQualifyIp6HopLimit, LayerAbsoluteIp},
};
//ipv4 only
//static bcm_dnx_sdk_field_t copp_l3_fg_qualify[] = {
//  {bcmFieldQualifyIp4Protocol, LayerAbsoluteIp},
//  {bcmFieldQualifyIp4Tos, LayerAbsoluteIp},
//  {bcmFieldQualifyIp4Ttl, LayerAbsoluteIp},
//  {bcmFieldQualifyIpHasOptions, RecordsAbsoluteIp},
//  {bcmFieldQualifyIpFrag, RecordsAbsoluteIp},
////TODO: SDK bug? in the lack of this we get "Error allocating bank: Prefix of handler 18 is already taken on bank 12"
//  {bcmFieldQualifyLayerRecordType, {bcmFieldInputTypeLayerRecordsAbsolute, 1, 0}},
//  {bcmFieldQualifyInPort, MetaData},
//  {bcmFieldQualifyDstPort, MetaData}
//};
//
//{udq_proto, {bcmFieldInputTypeLayerAbsolute, 1, 72},},
//{udq_dscp, {bcmFieldInputTypeLayerAbsolute, 1, 8},},
//{udq_ttl, {bcmFieldInputTypeLayerAbsolute, 1, 64},},
//
static bcm_dnx_sdk_field_t copp_l3_fg_qualify_v4[] = {
  {udq_ttl_proto, LayerAbsoluteIp, bcmFieldQualifyIp4Ttl}, //proto second byte
  {udq_dscp, LayerAbsoluteIp, bcmFieldQualifyIp4Tos},
  {udq_options, RecordsAbsoluteIp, bcmFieldQualifyIpHasOptions},
  {udq_frag, RecordsAbsoluteIp, bcmFieldQualifyIpFrag},
};

//static bcm_dnx_sdk_field_t copp_l3_fg_qualify_v6_patch[] = {
//  {udq_proto, LayerAbsoluteIp, bcmFieldQualifyIpProtocol},
//  {udq_dscp, LayerAbsoluteIp, bcmFieldQualifyDSCP},
//  {udq_options, RecordsAbsoluteIp, bcmFieldQualifyIp6FirstAdditionalHeaderExist},
//  {udq_ttl, LayerAbsoluteIp, bcmFieldQualifyIp6HopLimit},
////  {udq_frag, RecordsAbsoluteIp, bcmFieldQualifyIpFrag},
//};

//static bcm_dnx_sdk_field_t copp_l3_fg_qualify_v6[] = {
//  {bcmFieldQualifyIpProtocol, LayerAbsoluteIp},
//  {bcmFieldQualifyDSCP, LayerAbsoluteIp},
//  {bcmFieldQualifyIp6HopLimit, LayerAbsoluteIp},
//  {bcmFieldQualifyIp6FirstAdditionalHeaderExist, RecordsAbsoluteIp},
//};

static bcm_dnx_sdk_field_t pre_copp_fg_qualify[] = {
  {bcmFieldQualifyInPort, MetaData},
  {bcmFieldQualifyLayerRecordType, {bcmFieldInputTypeLayerRecordsAbsolute, 1, 0}},
  {bcmFieldQualifyEtherTypeUntagged, LayerAbsoluteL2},
  {bcmFieldQualifyDstPort, MetaData}
};
//ipv4 only
static bcm_dnx_sdk_field_t copp_fg_qualify[] = {
    {bcmFieldQualifyDstMac, LayerAbsoluteL2},
    {bcmFieldQualifyRangeCheck, MetaData},
//  {bcmFieldQualifyIp4Ttl, LayerAbsoluteIp},
//  {bcmFieldQualifyDstPort, MetaData},
  //TODO bcm_dnx_sdk_fe_set_PacketRes() only in catch_all, BTW WTF
  //bcmFieldQualifyPacketRes,
};

static bcm_dnx_sdk_field_t default_copp_fg_qualify[] = {
  {bcmFieldQualifyDstMac, LayerAbsoluteL2},
};
//ipv4 only
static bcm_dnx_sdk_field_t user_copp_fg_qualify[] = {
  {bcmFieldQualifySrcIp, LayerAbsoluteIp},
  {bcmFieldQualifyDstIp, LayerAbsoluteIp},
};
//ipv4 only
static bcm_dnx_sdk_field_t in_copp_fg_qualify[] = {
  //bcmFieldQualifyCascadedKeyValue, //TODO cascade
  //TODO bcm_dnx_sdk_fe_set_L2DestHit() unused?, BTW WTF
  //bcmFieldQualifyL2DestHit,
  //TODO adopt bcm_dnx_sdk_fe_set_IpFrag(), frag/no-frag
  {bcmFieldQualifyIpFrag, RecordsAbsoluteIp},
  //bcmFieldQualifyIpProtocolCommon,
  {bcmFieldQualifyEtherTypeUntagged, LayerAbsoluteL2},
  {bcmFieldQualifyRxTrapCode, MetaData}
};

static bcm_dnx_sdk_field_t in_default_copp_fg_qualify[] = {
  {bcmFieldQualifyRangeCheck, MetaData}
};

static bcm_dnx_sdk_field_t copp_hdr_fg_qualify[] = {
  {bcmFieldQualifyDstPort, MetaData},
  {bcmFieldQualifyLayerRecordType, {bcmFieldInputTypeLayerRecordsAbsolute, 1, 0}}
};

static bcm_dnx_sdk_field_t flowspec_extra_qualify_v4[] = {
/* For packet_header_size ranges, qualifier is the value of the first range ID
 * to which the packet_header_size fits. */
  {bcmFieldQualifyPacketLengthRangeCheck, MetaData},
  {udq_pr_v4, MetaData, bcmFieldQualifyRangeCheck},
};

static bcm_dnx_sdk_field_t flowspec_extra_qualify_v6[] = {
/* For packet_header_size ranges, qualifier is the value of the first range ID
 * to which the packet_header_size fits. */
  {bcmFieldQualifyPacketLengthRangeCheck, MetaData},
  {udq_pr_v6, MetaData, bcmFieldQualifyRangeCheck},
};

#if 0
static bcm_dnx_sdk_field_t
  l2_fg_qualify[] = {
    {bcmFieldQualifySrcMac, LayerAbsoluteL2},
    {bcmFieldQualifyDstMac, LayerAbsoluteL2}
  },
  in_l2_fg_qualify[] = {
    {bcmFieldQualifyEtherTypeUntagged, LayerAbsoluteL2}
  },
  out_l2_fg_qualify[] = {
    {bcmFieldQualifyEtherTypeUntagged, LayerAbsoluteL2}
  },
  l3_fg_qualify[] = { //ipv4 only
    //bcmFieldQualifyIpProtocol,
    {bcmFieldQualifyIp4Tos, LayerAbsoluteIp},
    //bcmFieldQualifyIpType
    //NOTE the fact of ipv4 packet should come from the context
    {bcmFieldQualifyIpHasOptions, RecordsAbsoluteIp}
  },
  v6_fg_qualify[] = {
    //bcmFieldQualifyIpProtocol,
    {bcmFieldQualifyDSCP, LayerAbsoluteIp},
    //bcmFieldQualifyIpType
    //NOTE the fact of ipv6 packet should come from the context
    {bcmFieldQualifyIp6FirstAdditionalHeaderExist, RecordsAbsoluteIp}
  },
  vlan_fg_qualify[] = {
    {bcmFieldQualifyVlanId, LayerAbsolute1q}
//    {bcmFieldQualifyTranslatedOuterVlanId, MetaData}
  },
  in_port_fg_qualify[] = {
    {bcmFieldQualifyInPort, MetaData}
  },
  out_port_fg_qualify[] = {
    {bcmFieldQualifyDstPort, MetaData}
  },
  in_tuple_fg_qualify[] = {
    {bcmFieldQualifyEtherTypeUntagged, LayerAbsoluteL2}
  },
  out_tuple_fg_qualify[] = {
    {bcmFieldQualifyEtherTypeUntagged, LayerAbsoluteL2}
  },
  out_vlan_fg_qualify[] = {
    {bcmFieldQualifyVrf, MetaData}
  },
  tuple_fg_qualify[] = {
    {bcmFieldQualifyIpHasOptions, RecordsAbsoluteIp},
    //bcmFieldQualifyIpType,
    {bcmFieldQualifySrcMac, LayerAbsoluteL2},
    {bcmFieldQualifyDstMac, LayerAbsoluteL2},
    {bcmFieldQualifySrcIp, LayerAbsoluteIp},
    {bcmFieldQualifyDstIp, LayerAbsoluteIp}
  },
  in_l3_fg_qualify[] = {
    {bcmFieldQualifyIp4Ttl, LayerAbsoluteIp},
    {bcmFieldQualifyIp4Protocol, LayerAbsoluteIp}
  },
  out_l3_fg_qualify[] = {
    {bcmFieldQualifyIP4Ttl, LayerAbsoluteIp}
  },
  v4_fg_qualify[] = {
    {bcmFieldQualifySrcIp, LayerAbsoluteIp},
    {bcmFieldQualifyDstIp, LayerAbsoluteIp}
  },
  L4_fg_qualify[] = {
    {bcmFieldQualifyL4DstPort, LayerAbsoluteL4},
    {bcmFieldQualifyL4SrcPort, LayerAbsoluteL4}
  },
  out_tuple6_fg_qualify[] = {
    //bcmFieldQualifyIpType,
    {bcmFieldQualifyIp6FirstAdditionalHeaderExist, RecordsAbsoluteIp},
    {bcmFieldQualifySrcMac, LayerAbsoluteL2},
    {bcmFieldQualifyDstMac, LayerAbsoluteL2}
  },
  in_tuple6_fg_qualify[] = {
    //bcmFieldQualifyIpType,
    {bcmFieldQualifyIp6FirstAdditionalHeaderExist, RecordsAbsoluteIp},
    {bcmFieldQualifySrcMac, LayerAbsoluteL2},
    {bcmFieldQualifyDstMac, LayerAbsoluteL2},
    {bcmFieldQualifySrcIp6High, LayerAbsoluteIp},
    {bcmFieldQualifyDstIp6High, LayerAbsoluteIp}
  },
  in_v6_fg_qualify[] = {
    {bcmFieldQualifyIpProtocol, LayerAbsoluteIp},
    bcmFieldQualifyIp6HopLimit,
    bcmFieldQualifyIp6FlowLabel,
    bcmFieldQualifySrcIp6High,
    {bcmFieldQualifyDstIp6, LayerAbsoluteIp},
  };









bcm_field_qualify_t mpls_fg_qualify[] = {
  bcmFieldQualifyMplsForwardingLabelId,
};









bcm_field_qualify_t pre_v6_fg_qualify[] = {
  bcmFieldQualifyIpProtocolCommon,
  bcmFieldQualifySrcIp6High,
  bcmFieldQualifyDstIp6,
};

bcm_field_qualify_t out_v6_fg_qualify[] = {
  bcmFieldQualifyIp6FlowLabel,
  bcmFieldQualifyDstClassField,
  bcmFieldQualifyIp6HopLimit,
};

bcm_field_qualify_t pre_copp_fg_qualify[] = {
  bcmFieldQualifyInPort,
  bcmFieldQualifyRxTrapCode,
  bcmFieldQualifyEtherType,
  bcmFieldQualifyDstPort,
};

bcm_field_qualify_t copp_fg_qualify[] = {
  bcmFieldQualifyTtl,
  bcmFieldQualifyDstPort,
  bcmFieldQualifyPacketRes,
};

bcm_field_qualify_t default_copp_fg_qualify[] = {
  bcmFieldQualifyDstMac,
};

bcm_field_qualify_t user_copp_fg_qualify[] = {
  bcmFieldQualifySrcIp,
  bcmFieldQualifyDstIp,
  bcmFieldQualifyDstMac,
};

bcm_field_qualify_t in_copp_fg_qualify[] = {
  bcmFieldQualifyCascadedKeyValue,
  bcmFieldQualifyL2DestHit,
  bcmFieldQualifyIpFrag,
  bcmFieldQualifyIpProtocolCommon,
  bcmFieldQualifyEtherType,
  bcmFieldQualifyRxTrapCode
};

bcm_field_qualify_t in_default_copp_fg_qualify[] = {
  bcmFieldQualifyRangeCheck,
};
#endif

#define BCM_DNX_FE_ING_STAT_CMD 1

static bcm_dnx_sdk_action_t drop_action[] = {
    {drop_aid,},
};

/* no redirect and mirror actions in case of v4 limited qualifiers */
static bcm_dnx_sdk_action_t redir_action[] = {
    {redir_aid,},
    {mirror_aid,},
};

static bcm_dnx_sdk_action_t action[] = {
  {bcmFieldActionStatId0 + BCM_DNX_FE_ING_STAT_CMD,},
  {bcmFieldActionStatProfile0 + BCM_DNX_FE_ING_STAT_CMD,},
  {snoop_aid,},
};
// share COPP policers
static bcm_dnx_sdk_action_t qos_action[] = {
    {bcmFieldActionStatId0,},
    {bcmFieldActionStatProfile0,},
    {bcmFieldActionPrioIntNew,},
};

static bcm_dnx_sdk_action_t in_v6_pre_action[] = {
  {bcmFieldActionClassDestSet,},
};
//NOTE: need to bump to 64 bits action
static bcm_dnx_sdk_action_t pre_copp_action[] = {
  {bcmFieldActionUDHData1,},
  {copp_aid, 1},
};

static bcm_dnx_sdk_action_t egress_action[] = {
//  bcmFieldActionStat,
  {bcmFieldActionDrop,},
//  bcmFieldActionDropCancel,
  {bcmFieldActionStatId0,},
  {bcmFieldActionStatProfile0,},
  {bcmFieldActionMirrorEgress,},
};

static bcm_dnx_sdk_action_t copp_action[] = {
  //bcmFieldActionDrop,
  {redir_aid_2,},
  {bcmFieldActionStatId0,},
  {bcmFieldActionStatProfile0,},
  {bcmFieldActionPrioIntNew,},
  //{bcmFieldActionPolicerLevel0,},
};

static bcm_dnx_sdk_action_t copp_hdr_action[] = {
  {bcmFieldActionForwardingLayerIndex,},
  {bcmFieldActionEgressForwardingIndex,},
  {bcmFieldActionStatSampling,},
};

//static bcm_dnx_sdk_action_t sflow_in_action[] = {
//  {bcmFieldActionStatSampling,},
//};

//static bcm_dnx_sdk_action_t sflow_eg_action[] = {
//  {bcmFieldActionMirrorEgress,},
//};

#define QSET_ADD(_, n) \
    rv = bcm_dnx_fset_add(unit, fg_info, attach_info, n,\
                          sizeof(n)/sizeof(n[0]));\
    if (rv) { \
      printf("Error (%d), bcm_dnx_fset_add(%s)\n", rv, #n); \
      return rv; \
    }

#define QSET_PATCH(_, n) \
    if (!detach) { \
      rv = bcm_dnx_fset_patch(unit, fg_info, attach_info, n,\
                            sizeof(n)/sizeof(n[0]));\
      if (rv) { \
        printf("Error bcm_dnx_fset_patch(%s)\n", #n); \
        return rv; \
      } \
    }

#define ingress_finish \
    fg_info->stage = bcm_dnx_sdk_fgs[acl_type].fg_stage; \
    return RET_CODE_SUCCESS;

#define  QUALIFIER_IP_SRC_ADDR      0x01
#define  QUALIFIER_IP_DST_ADDR      0x02
#define  QUALIFIER_IP_PROTO         0x04
#define  QUALIFIER_IP_L4_PORTS      0x08
#define  QUALIFIER_IP_RANGE         0x10
#define  ACTION_DROP_ONLY           0x20

#define QSET_LIMITED_ADD(f,q) if (tuple_flag & (f)) { QSET_ADD(f, q) }

/* returns RET_CODE_NOT_FOUND when normal qsets need to be applied */
static ret_code_e
bcm_dnx_sdk_get_limited_qset(int unit,
                         bcm_dnx_fg_type_t acl_type,
                         bcm_dnx_fg_direction_t direction,
                         bcm_field_group_info_t *fg_info,
                         bcm_field_group_attach_info_t *attach_info,
                         uint32 tuple_flag)
{
  int rv;
  if (!tuple_flag) {
    return RET_CODE_NOT_FOUND;
  }
  if (BCM_DNX_PMF_INGRESS != direction) {
    return RET_CODE_NOT_FOUND;
  }
  switch (acl_type) {
  case BCM_DNX_FG_IPV4:
    QSET_LIMITED_ADD(QUALIFIER_IP_SRC_ADDR, v4_src_qualify);
    QSET_LIMITED_ADD(QUALIFIER_IP_DST_ADDR, v4_dst_qualify);
    QSET_LIMITED_ADD(QUALIFIER_IP_PROTO, v4_ipproto_qualify);
    QSET_LIMITED_ADD(QUALIFIER_IP_L4_PORTS, L4_fg_limited_qualify);
    break;
  case BCM_DNX_FG_IPV6:
    /* IP_PROTO and L4_PORTS are always included
     * as they are needed by implicit rules */
    QSET_ADD(f, v6_limited_qualify);
    QSET_LIMITED_ADD(QUALIFIER_IP_SRC_ADDR, v6_src_qualify);
    QSET_LIMITED_ADD(QUALIFIER_IP_DST_ADDR, v6_dst_qualify);
    break;
  default:
    return RET_CODE_NOT_FOUND;
    break;
  }
  return RET_CODE_SUCCESS;
}

ret_code_e
bcm_dnx_sdk_get_qset(int unit,
                     bcm_dnx_group_tuple_t *gtuple,
                     bcm_dnx_fg_type_t acl_type,
                     bcm_dnx_fg_direction_t direction,
                     bcm_dnx_target_type_t target_type,
                     bcm_field_group_info_t *fg_info,
                     bcm_field_group_attach_info_t *attach_info,
                     uint32 tuple_flag)
{
  int rv;
  ret_code_e ret;
  //if (gtuple == NULL) return RET_CODE_ARG_INVALID;
  //if (gtuple->ctxt == NULL) return RET_CODE_ARG_INVALID;

  if (acl_type == BCM_DNX_FG_NONE || acl_type >= BCM_DNX_FG_MAX_J2)
    return RET_CODE_ARG_INVALID;
  if (direction == BCM_DNX_PMF_NONE || direction >= BCM_DNX_PMF_MAX)
    return RET_CODE_ARG_INVALID;
  if (target_type == BCM_DNX_TARGET_NONE || target_type >= BCM_DNX_TARGET_MAX)
    return RET_CODE_ARG_INVALID;
  if (BCM_DNX_PMF_INGRESS == direction) {
    fg_info->stage = bcm_dnx_sdk_fgs[acl_type].fg_stage;
  } else {
    fg_info->stage = bcmFieldStageEgress;
  }
  switch(acl_type) {
  case BCM_DNX_FG_TUPLE:
    QSET_ADD((*fqset), tuple_fg_qualify);
    switch(direction) {
    case BCM_DNX_PMF_INGRESS:
      QSET_ADD((*fqset), in_tuple_fg_qualify);
      break;
    default:
      QSET_ADD((*fqset), out_tuple_fg_qualify);
//      rv = bcm_field_qset_data_qualifier_add(unit, fqset, gtuple->ctxt->ud_ether[0]);
//      if (rv != BCM_E_NONE) {
//        printf("bcm_field_qset_data_qualifier_add fg_tuple grp failed",
//                                   bcm_errmsg(rv));
//        return RET_CODE_FAILURE;
//      }
      break;
    }
    break;

  case BCM_DNX_FG_TUPLE6:
    switch(direction) {
    case BCM_DNX_PMF_INGRESS:
      QSET_ADD((*fqset), in_tuple6_fg_qualify);
      QSET_ADD((*fqset), in_tuple_fg_qualify);
//      rv = bcm_field_qset_data_qualifier_add(unit, fqset, gtuple->ctxt->ud_icmp[0]);
//      if (rv != BCM_E_NONE) {
//        bcmsdk_trace_acl_gen_error("bcm_field_qset_data_qualifier_add ipv6 grp failed",
//                                   bcm_errmsg(rv));
//        return RET_CODE_FAILURE;
//      }
      break;
    default:
      QSET_ADD((*fqset), out_tuple6_fg_qualify);
      QSET_ADD((*fqset), out_tuple_fg_qualify);
//      rv = bcm_field_qset_data_qualifier_add(unit, fqset, gtuple->ctxt->ud_ether[0]);
//      if (rv != BCM_E_NONE) {
//        bcmsdk_trace_acl_gen_error("bcm_field_qset_data_qualifier_add fg_tuple grp failed",
//                                   bcm_errmsg(rv));
//        return RET_CODE_FAILURE;
//      }
      break;
    }
    break;

  case BCM_DNX_FG_L2:
    QSET_ADD((*fqset), l2_fg_qualify);

    switch(direction) {
    case BCM_DNX_PMF_INGRESS:
      QSET_ADD((*fqset), in_l2_fg_qualify);
      break;
    default:
      QSET_ADD((*fqset), out_l2_fg_qualify);
//      rv = bcm_field_qset_data_qualifier_add(unit, fqset, gtuple->ctxt->ud_ether[0]);
//      if (rv != BCM_E_NONE) {
//        bcmsdk_trace_acl_gen_error("bcm_field_qset_data_qualifier_add l2 grp failed",
//                                   bcm_errmsg(rv));
//        return RET_CODE_FAILURE;
//      }
      break;
    }
    break;

    case BCM_DNX_FG_QOS_L2:
      QSET_ADD((*fqset), l2_fg_qualify);

      switch(direction) {
      case BCM_DNX_PMF_INGRESS:
        QSET_ADD((*fqset), in_l2_fg_qualify);
        break;
      default:
        return RET_CODE_UNSUPPORTED;
        break;
      }
      break;

  case BCM_DNX_FG_IPV4:
    ret = bcm_dnx_sdk_get_limited_qset(unit, acl_type, direction, fg_info,
                                       attach_info, tuple_flag);
    if (RET_CODE_NOT_FOUND == ret) {
      QSET_ADD((*fqset), l3_fg_qualify);

      switch (direction) {
      case BCM_DNX_PMF_INGRESS:
        QSET_ADD((*fqset), flowspec_extra_qualify_v4);
#ifdef MPLS
        QSET_ADD((*fqset), mpls_fg_qualify);
#endif
        break;
      default:
        break;
      }

      QSET_ADD((*fqset), L4_fg_qualify);
      QSET_ADD((*fqset), v4_fg_qualify);
    } else if (RET_CODE_SUCCESS != ret) {
      return ret;
    }
    break;

  case BCM_DNX_FG_QOS_V4:
    ret = bcm_dnx_sdk_get_limited_qset(unit, acl_type, direction, fg_info,
                                       attach_info, tuple_flag);
    if (RET_CODE_NOT_FOUND == ret) {
      QSET_ADD((*fqset), l3_fg_qualify);

      switch (direction) {
      case BCM_DNX_PMF_INGRESS:
#ifdef MPLS
        QSET_ADD((*fqset), mpls_fg_qualify);
#endif
        break;
      default:
        return RET_CODE_UNSUPPORTED;
        break;
      }

      QSET_ADD((*fqset), L4_fg_qualify);
      QSET_ADD((*fqset), v4_fg_qualify);
    } else if (RET_CODE_SUCCESS != ret) {
      return ret;
    }
    break;

  case BCM_DNX_FG_IPV6_PRE:
//    switch(direction) {
//    case BCM_DNX_PMF_INGRESS:
//      QSET_ADD((*fqset), pre_v6_fg_qualify);
//      //try to get ICMP {type,code} from L4 srcPort
////      rv = bcm_field_qset_data_qualifier_add(unit, fqset, gtuple->ctxt->ud_icmp[0]);
////      if (rv != BCM_E_NONE) {
////        bcmsdk_trace_acl_gen_error("bcm_field_qset_data_qualifier_add ipv6_pre grp failed",
////                                   bcm_errmsg(rv));
////        return RET_CODE_FAILURE;
////      }
//      break;
//
//    default:
//      break;
//    }
    break;

  case BCM_DNX_FG_IPV6:
    ret = bcm_dnx_sdk_get_limited_qset(unit, acl_type, direction, fg_info,
                                       attach_info, tuple_flag);
    if (RET_CODE_NOT_FOUND == ret) {
      QSET_ADD((*fqset), v6_fg_qualify);

    switch(direction) {
    case BCM_DNX_PMF_INGRESS:
      QSET_ADD((*fqset), flowspec_extra_qualify_v6);
      break;
    default:
      break;
    }
      QSET_ADD((*fqset), L4_fg_qualify);

      //try to get ICMP {type,code} from L4 srcPort
//      rv = bcm_field_qset_data_qualifier_add(unit, fqset, gtuple->ctxt->ud_icmp[0]);
//      if (rv != BCM_E_NONE) {
//        bcmsdk_trace_acl_gen_error("bcm_field_qset_data_qualifier_add ipv6 grp failed",
//                                   bcm_errmsg(rv));
//        return RET_CODE_FAILURE;
//      }
      QSET_ADD((*fqset), in_v6_fg_qualify);
//      break;
//    default:
//      QSET_ADD((*fqset), out_v6_fg_qualify);
//      break;
//    }
    } else if (RET_CODE_SUCCESS != ret) {
      return ret;
    }
    break;

  case BCM_DNX_FG_QOS_V6:
    ret = bcm_dnx_sdk_get_limited_qset(unit, acl_type, direction, fg_info,
                                       attach_info, tuple_flag);
    if (RET_CODE_NOT_FOUND == ret) {
      QSET_ADD((*fqset), v6_fg_qualify);
      if (BCM_DNX_PMF_INGRESS != direction) {
        return RET_CODE_UNSUPPORTED;
      }
      QSET_ADD((*fqset), L4_fg_qualify);
      QSET_ADD((*fqset), in_v6_fg_qualify);
    } else if (RET_CODE_SUCCESS != ret) {
      return ret;
    }
    break;

  case BCM_DNX_FG_USER_COPP:
    QSET_ADD((*fqset), copp_l3_fg_qualify_v4);
    QSET_ADD((*fqset), L4_fg_qualify);
    QSET_ADD((*fqset), copp_fg_qualify);
    QSET_ADD((*fqset), pre_copp_fg_qualify);
    QSET_ADD((*fqset), user_copp_fg_qualify);
    return RET_CODE_SUCCESS;
    break;

  case BCM_DNX_FG_DEFAULT_COPP:
    QSET_ADD((*fqset), copp_l3_fg_qualify_v4);
    QSET_ADD((*fqset), L4_fg_qualify);
    QSET_ADD((*fqset), copp_fg_qualify);
    QSET_ADD((*fqset), pre_copp_fg_qualify);

    return RET_CODE_SUCCESS;
    break;

  case BCM_DNX_FG_PRE_COPP:
    QSET_ADD((*fqset), pre_copp_fg_qualify);
    return RET_CODE_SUCCESS;
    break;

  case BCM_DNX_FG_COPP_HDR:
    QSET_ADD((*fqset), copp_hdr_fg_qualify);
    QSET_ADD((*fqset), in_port_fg_qualify);
    return RET_CODE_SUCCESS;
    break;
  default:
    return RET_CODE_UNSUPPORTED;
    break;
  }

  switch(direction) {
  case BCM_DNX_PMF_INGRESS:
    switch(target_type) {
    case BCM_DNX_TARGET_VLAN:
      if (acl_type != BCM_DNX_FG_IPV6_PRE) {
        QSET_ADD((*fqset), vlan_fg_qualify);
      }
      break;

    case BCM_DNX_TARGET_PORT:
      if (acl_type != BCM_DNX_FG_IPV6_PRE) {
        QSET_ADD((*fqset), in_port_fg_qualify);
      }
      break;

    case BCM_DNX_TARGET_PV:
      if (acl_type != BCM_DNX_FG_IPV6_PRE) {
        QSET_ADD((*fqset), in_port_fg_qualify);
        QSET_ADD((*fqset), vlan_fg_qualify);
      }
      break;
    default:
      break;
    }
    break;

  case BCM_DNX_PMF_EGRESS:
    switch(target_type) {
    case BCM_DNX_TARGET_VLAN:
      QSET_ADD((*fqset), out_vlan_fg_qualify);
      QSET_ADD((*fqset), out_port_fg_qualify);
      break;
    case BCM_DNX_TARGET_PORT:
      QSET_ADD((*fqset), out_vlan_fg_qualify);
      QSET_ADD((*fqset), out_port_fg_qualify);
      break;
    case BCM_DNX_TARGET_PV:
      QSET_ADD((*fqset), out_vlan_fg_qualify);
      QSET_ADD((*fqset), out_port_fg_qualify);
      break;
    default:
      break;
    }
    break;
  default:
    return RET_CODE_ARG_INVALID;
  }
  return RET_CODE_SUCCESS;
}

#define ASET_ADD(_, action) \
    rv = bcm_dnx_aset_add(&fg_info, &attach_info, action,\
           DIM(action), gtuple->priority); \
    if (rv) { \
      printf("Error (%d), bcm_dnx_aset_add(action)\n", rv); \
      return rv; \
    }

static ret_code_e
bcm_dnx_sdk_attach_fg_to_context (int unit,
    bcm_dnx_sdk_context_id_e context, bcm_field_group_t id,
    bcm_field_group_attach_info_t *attach_info, int detach)
{
  bcm_field_context_t context_id = bcm_dnx_sdk_contexts[context].context_id;
  int rv =
      detach ?
          bcm_field_group_context_detach(unit, id, context_id) :
          bcm_field_group_context_attach(unit, 0, id, context_id, attach_info);
  if (rv != BCM_E_NONE) {
    printf("Error (%d), in bcm_field_group_context_%s FG:%d Context ID:%d\n",
           rv, detach ? "detach" : "attach", id, context_id);
    return RET_CODE_FAILURE;
  }
  printf("%s TCAM field group %d %s context %d\n",
         detach ? "Detached" : "Attached", id, detach ? "from" : "to",
         context_id);
  return RET_CODE_SUCCESS;
}

#define ATTACH(c) \
    rv = bcm_dnx_sdk_attach_fg_to_context(unit, c, id, attach_info, detach); \
    if (RET_CODE_SUCCESS != rv && !detach) return rv

#define TAG_ATTACH(c, a) \
  switch (target_type) { \
  case BCM_DNX_TARGET_VLAN: \
    a(c ## _TAG); \
    break; \
  default: \
    a(c); \
    break; \
  }

#define ATTACH_T(c) TAG_ATTACH(c, ATTACH)

#define COPP_ATTACH_CASCADE(typ) \
if (!detach) { ATTACH(BCM_DNX_CONTEXT_INGRESS_##typ); }\
rv = bcm_dnx_sdk_copp_presel_set(unit, bcm_dnx_sdk_contexts + BCM_DNX_CONTEXT_COPP_##typ, id, detach); \
if (RET_CODE_SUCCESS != rv && !detach) return rv; \
if (detach) { ATTACH(BCM_DNX_CONTEXT_INGRESS_##typ); }


#define COPP_ATTACH_CASCADE_T(c) TAG_ATTACH(c, COPP_ATTACH_CASCADE)

static ret_code_e
bcm_dnx_sdk_attach_WA_FEC_dest_to_context (int unit,
    bcm_dnx_sdk_context_id_e context, int detach)
{
  bcm_field_group_info_t fg_info;
  bcm_field_group_attach_info_t attach_info;
  bcm_field_group_t fg_id = 127;
  int rv;
  if (!detach) {
    bcm_field_group_info_t_init(&fg_info);
    rv = bcm_field_group_info_get(unit, fg_id, &fg_info);
    if (rv != BCM_E_NONE) {
      printf("Error (%d), in bcm_field_group_info_get FG:%d\n", rv, fg_id);
      return RET_CODE_FAILURE;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    /**Destination is taken from meta data*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
  }
  return bcm_dnx_sdk_attach_fg_to_context(unit, context, fg_id, &attach_info,
                                          detach);
}

static int
bcm_dnx_sdk_get_key_size (int bits, int base)
{
  int size;
  if (bits <= 0) {
    return BCM_DNX_FG_KEY_DONT_CARE;
  }
  size = (bits - 1) / base + 1;
  if (size > BCM_DNX_FG_KEY_320) {
    /* we are in trouble here */
    return BCM_DNX_FG_KEY_DONT_CARE;
  }
  if (3 == size) {
    size = BCM_DNX_FG_KEY_320;
  }
  return size;
}

static ret_code_e
bcm_dnx_sdk_fg_get_key_size_bits (int unit, bcm_field_group_info_t *fg_info, int *key_size)
{
  int size_key, i, rv;
  bcm_field_qualifier_info_get_t qual_info_get;
  for (size_key = 0, i = 0; i < fg_info->nof_quals; i++) {
    bcm_field_qualifier_info_get_t_init(&qual_info_get);
    rv = bcm_field_qualifier_info_get(unit, fg_info->qual_types[i],
                                      fg_info->stage, &qual_info_get);
    if (rv != BCM_E_NONE) {
      printf("bcm_field_qualifier_info_get failed for: %d stage: %d\n",
             fg_info->qual_types[i], fg_info->stage);
      return RET_CODE_FAILURE;
    }
    size_key += qual_info_get.size;
  }
  *key_size = size_key;
  return RET_CODE_SUCCESS;
}

static ret_code_e
bcm_dnx_sdk_fg_get_key_size (int unit, bcm_field_group_t fg_id,
    bcm_dnx_tcam_fg_key_size_e *size)
{
  bcm_field_group_info_t fg_info;
  bcm_field_action_info_t action_info;
  int rv, size_key, action_offset, last_action, key_size, action_size;
  ret_code_e ret;
  bcm_field_group_info_t_init(&fg_info);
  rv = bcm_field_group_info_get(unit, fg_id, &fg_info);
  if (rv != BCM_E_NONE) {
    printf("Error (%d), in bcm_field_group_info_get FG:%d\n", rv, fg_id);
    return RET_CODE_FAILURE;
  }
  ret = bcm_dnx_sdk_fg_get_key_size_bits(unit, &fg_info, &size_key);
  if (RET_CODE_SUCCESS != ret) {
    return ret;
  }
  last_action = fg_info.nof_actions - 1;
  rv = bcm_field_group_action_offset_get(unit, 0, fg_id,
                                         fg_info.action_types[last_action],
                                         &action_offset);
  if (rv != BCM_E_NONE) {
    printf("Action offset get failed: %d\n", rv);
    return RET_CODE_FAILURE;
  }
  if (fg_info.action_with_valid_bit[last_action]) {
    action_offset++;
  }
  bcm_field_action_info_t_init(&action_info);
  rv = bcm_field_action_info_get(unit, fg_info.action_types[last_action],
                                 fg_info.stage, &action_info);
  if (rv != BCM_E_NONE) {
    printf("Action info get failed: %d\n", rv);
    return RET_CODE_FAILURE;
  }
  action_offset += action_info.size;
  action_size = bcm_dnx_sdk_get_key_size(action_offset,
                                         BCM_DNX_FG_ACTION_SIZE_BASE_BITS);
  key_size = bcm_dnx_sdk_get_key_size(size_key, BCM_DNX_FG_KEY_SIZE_BASE_BITS);
  printf("FG ID:%d action_size:%d(%d) key_size:%d(%d)\n", fg_id, action_offset,
         action_size, size_key, key_size);
  *size = action_size > key_size ? action_size : key_size;
  return RET_CODE_SUCCESS;
}

static ret_code_e
bcm_dnx_sdk_attach_fg_to_contexts (int unit,
    bcm_dnx_fg_type_t type, bcm_dnx_fg_direction_t direction,
    bcm_dnx_target_type_t target_type, bcm_field_group_t id,
    bcm_field_group_attach_info_t *attach_info, bcm_field_group_info_t *fg_info,
    int detach)
{
  ret_code_e rv;
  switch (direction) {
  case BCM_DNX_PMF_INGRESS:
    switch (type) {
    case BCM_DNX_FG_L2:
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V4);
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V6);
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_L2);
      break;
    case BCM_DNX_FG_QOS_L2:
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V4);
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V6);
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_L2);
      break;
    case BCM_DNX_FG_IPV4:
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V4);
      break;
    case BCM_DNX_FG_QOS_V4:
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V4);
      break;
    case BCM_DNX_FG_TUPLE:
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V4);
      break;
    case BCM_DNX_FG_IPV6_PRE:
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V6);
      break;
    case BCM_DNX_FG_IPV6:
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V6);
      break;
    case BCM_DNX_FG_QOS_V6:
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V6);
      break;
    case BCM_DNX_FG_TUPLE6:
      ATTACH_T(BCM_DNX_CONTEXT_INGRESS_V6);
      break;
    case BCM_DNX_FG_USER_COPP:
      ATTACH_T(BCM_DNX_CONTEXT_COPP_V4);
      QSET_PATCH(x, v6_fg_qualify);
      ATTACH_T(BCM_DNX_CONTEXT_COPP_V6);
      ATTACH_T(BCM_DNX_CONTEXT_COPP_L2);
      break;
    case BCM_DNX_FG_DEFAULT_COPP:
      ATTACH_T(BCM_DNX_CONTEXT_COPP_V4);
      QSET_PATCH(x, v6_fg_qualify);
      ATTACH_T(BCM_DNX_CONTEXT_COPP_V6);
      ATTACH_T(BCM_DNX_CONTEXT_COPP_L2);
      break;
    case BCM_DNX_FG_PRE_COPP:
      COPP_ATTACH_CASCADE_T(V4);
      COPP_ATTACH_CASCADE_T(V6);
      COPP_ATTACH_CASCADE_T(L2);
      break;
    default:
      //By default the context_id here is 0 which is the default context
      ATTACH(BCM_DNX_CONTEXT_INVALID);
      break;
    }
    break;
  default:
    switch (type) {
    case BCM_DNX_FG_IPV4:
      ATTACH(BCM_DNX_CONTEXT_EGRESS_V4);
      break;
    case BCM_DNX_FG_TUPLE:
      ATTACH(BCM_DNX_CONTEXT_EGRESS_V4);
      break;
    case BCM_DNX_FG_IPV6:
      ATTACH(BCM_DNX_CONTEXT_EGRESS_V6);
      break;
    case BCM_DNX_FG_TUPLE6:
      ATTACH(BCM_DNX_CONTEXT_EGRESS_V6);
      break;
    case BCM_DNX_FG_L2:
      ATTACH(BCM_DNX_CONTEXT_EGRESS_V4);
      ATTACH(BCM_DNX_CONTEXT_EGRESS_V6);
      ATTACH(BCM_DNX_CONTEXT_INVALID);
      break;
    default:
      ATTACH(BCM_DNX_CONTEXT_INVALID);
      break;
    }
  }
  return RET_CODE_SUCCESS;
}
ret_code_e
bcm_dnx_sdk_fg_create (int unit,
    bcm_dnx_fg_type_t type,
    bcm_dnx_fg_direction_t direction,
    bcm_dnx_target_type_t target_type,
    bcm_dnx_group_tuple_t *gtuple)
{
  int rv;
  if (type == BCM_DNX_FG_NONE || type >= BCM_DNX_FG_MAX_J2)
    return RET_CODE_ARG_INVALID;
  if (direction == BCM_DNX_PMF_NONE || direction >= BCM_DNX_PMF_MAX)
    return RET_CODE_ARG_INVALID;
  if (target_type == BCM_DNX_TARGET_NONE || target_type >= BCM_DNX_TARGET_MAX)
    return RET_CODE_ARG_INVALID;
  if (direction == BCM_DNX_PMF_EGRESS && type == BCM_DNX_FG_IPV6_PRE)
    return RET_CODE_SUCCESS;
  bcm_field_group_t id;
  //TODO resolution of context
  bcm_field_context_t context = 0;
  bcm_field_group_info_t fg_info;
  bcm_field_group_attach_info_t attach_info;
  bcm_field_group_info_t_init(&fg_info);
  fg_info.fg_type = bcmFieldGroupTypeTcam;
  char name[BCM_FIELD_MAX_SHORT_NAME_LEN];
  snprintf(name, BCM_FIELD_MAX_SHORT_NAME_LEN, "%s/%s/%s",
           bcm_dnx_fg_direction_s[direction], bcm_dnx_sdk_fgs[type].fg_type_name,
           bcm_dnx_target_type_s[target_type]);
  void *dstname = fg_info.name;
  sal_strncpy(dstname, name, BCM_FIELD_MAX_SHORT_NAME_LEN);
  bcm_field_group_attach_info_t_init(&attach_info);
  rv = bcm_dnx_sdk_get_qset(unit, gtuple, type, direction,
                               target_type, &fg_info, &attach_info,
                               g_tuple_flag);
  if (rv != RET_CODE_SUCCESS) {
    printf("Error (%d), bcm_dnx_sdk_get_qset\n", rv);
    return rv;
  }
  if (!fg_info.nof_quals) {
    printf("Skipping empty FG\n");
    return RET_CODE_SUCCESS;
  }
  if (direction == BCM_DNX_PMF_INGRESS) {
    switch(type) {
    case BCM_DNX_FG_DEFAULT_COPP:
      ASET_ADD(faset, copp_action);
      break;
    case BCM_DNX_FG_USER_COPP:
      ASET_ADD(faset, copp_action);
      break;
    case BCM_DNX_FG_QOS_V4:
      ASET_ADD(faset, qos_action);
      break;
    case BCM_DNX_FG_QOS_V6:
      ASET_ADD(faset, qos_action);
      break;
    case BCM_DNX_FG_QOS_L2:
      ASET_ADD(faset, qos_action);
      break;
    case BCM_DNX_FG_PRE_COPP:
      ASET_ADD(faset, pre_copp_action);
      break;
    case BCM_DNX_FG_COPP_HDR:
      ASET_ADD(faset, copp_hdr_action);
      break;
    default:
      if (g_tuple_flag & ACTION_DROP_ONLY) {
        ASET_ADD(faset, drop_action);
      } else {
        ASET_ADD(faset, redir_action);
      }
      ASET_ADD(faset, action);
      break;
    }
  } else {
    ASET_ADD(faset, egress_action);
  }
  if (bcm_dnx_sdk_fgs[type].tcam_info.nof_tcam_banks) {
    fg_info.tcam_info = bcm_dnx_sdk_fgs[type].tcam_info;
  }
  rv = bcm_field_group_add(
      unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info,
      &id);
  if (rv != BCM_E_NONE) {
    printf("Error (%d), in bcm_field_group_add\n", rv);
    return RET_CODE_FAILURE;
  }
  printf("Created TCAM field group (%d) \n", id);
  gtuple->group_id = id;
  gtuple->type = type;
  gtuple->direction = direction;
  gtuple->target_type = target_type;
  rv = bcm_dnx_sdk_attach_fg_to_contexts(unit, type, direction, target_type, id, &attach_info, fg_info, 0);
  if (RET_CODE_SUCCESS != rv) return rv;
  return bcm_dnx_pmf_tcam_fg_info_init(unit, &g_bcm_dnx_tcam_info, id, direction, type);
}

ret_code_e
bcm_dnx_sdk_fg_destroy (int unit, bcm_dnx_group_tuple_t *gtuple)
{
  int rv;
  //best effort
  bcm_dnx_sdk_attach_fg_to_contexts(unit, gtuple->type, gtuple->direction,
                                    gtuple->target_type, gtuple->group_id, NULL, NULL, 1);

  rv = bcm_field_group_delete(unit, gtuple->group_id);
  if (rv != BCM_E_NONE) {
    printf("Error (%d), in bcm_field_group_delete\n", rv);
    return rv;
  }
  bcm_dnx_pmf_tcam_fg_info_destroy(&g_bcm_dnx_tcam_info, gtuple->group_id);
  return 0;
}

static bcm_field_entry_qual_t my_e[20][5] = {
    {
        {bcmFieldQualifySrcMac, {0x1122}, {0xffff}},
        {bcmFieldQualifyDstMac, {0x0002}, {0xffff}}
    },
    {
        {bcmFieldQualifySrcMac, {0x1123}, {0xffff}},
//        {bcmFieldQualifyTranslatedOuterVlanId, {0x32}, {0x0fff}}
        {bcmFieldQualifyVlanId, {0x32}, {0x0fff}}
    },
    {
        {bcmFieldQualifySrcMac, {0x1122}, {0xffff}},
        {bcmFieldQualifyDstMac, {0x0002}, {0xffff}},
//        {bcmFieldQualifyTranslatedOuterVlanId, {0x32}, {0x0fff}}
        {bcmFieldQualifyVlanId, {0x32}, {0xfff}}
    },
    {
        {bcmFieldQualifyEtherTypeUntagged, {0x801}, {0xffff}}
    },
};

ret_code_e
bcm_dnx_sdk_fe_create (int unit, int id, int act, int prio, int fg)
{
  bcm_field_entry_info_t ent_info;
  int rv, i;
  bcm_field_entry_info_t_init(&ent_info);
  ent_info.priority = prio;
  ent_info.nof_entry_quals = 0;
  ent_info.nof_entry_actions = 0;
  for (i = 0; i < 5; i++) {
    if (!my_e[id][i].type) continue;
    print my_e[id][i];
    rv = bcm_dnx_fe_qual_add(&ent_info, my_e[id][i].type,
                             my_e[id][i].value, my_e[id][i].mask,
                             6);
    if (rv) {
      printf("Error (%d), bcm_dnx_fe_qual_add(%d)\n", rv,
             my_e[id][i].type);
      return rv;
    }
  }
  if (act) {
    rv = bcm_dnx_fe_action_add32(&ent_info, act, 1 /*BCM_FIELD_COLOR_RED*/);
    if (rv) {
      printf("Error (%d), bcm_dnx_fe_action_add(drop_aid)\n", rv);
      return rv;
    }
  }
  rv = bcm_field_entry_add(unit, BCM_FIELD_FLAG_WITH_ID, fg, &ent_info, &id);
  if (rv != BCM_E_NONE) {
    printf("Error (%d), in bcm_field_entry_add\n", rv);
    return rv;
  }
  printf("Entry add: id:(0x%x)\n", id);
  return 0;
}
#define TRY(test) \
  rv = test; \
  if (rv) { \
    printf("%s failed rv:%d\n", #test, rv); \
    return rv; \
  }

#define TRY2(test) \
  rv = test; \
  if (rv) { \
    printf("%s failed rv:%d\n", #test, rv); \
    printf("c: %d e: %d\n", c, e); \
    return RET_CODE_FAILURE; \
  }
int
bcm_dnx_sdk_blank_fe_action_create (int unit, int *id, int act_id, int act_val, int prio, int fg)
{
  bcm_field_entry_info_t ent_info;
  int rv;
  bcm_field_entry_info_t_init(&ent_info);
  ent_info.priority = prio;
  ent_info.nof_entry_quals = 0;
  ent_info.nof_entry_actions = 0;
  TRY(bcm_dnx_fe_action_add32(&ent_info, act_id, act_val));
  TRY(bcm_field_entry_add(unit, 0, fg, &ent_info, id));
  return 0;
}

int
bcm_dnx_sdk_fe_destroy (int unit, int id, int fg)
{
  int rv = BCM_E_NONE;
  rv = bcm_field_entry_delete(unit, fg, NULL, id);
  if (rv != BCM_E_NONE) {
    printf("Error (%d), in bcm_field_entry_delete\n", rv);
  }
  return rv;
}

ret_code_e
bcm_dnx_sdk_fe_set_IcmpTypeCode(int unit, bcm_dnx_entry_tuple_t *etuple,
                                uint8 type, uint8 code)
{
  ret_code_e ret;
  //There is one hton
  uint32 icmptypecode = code | (type << 8);
  uint32 icmpmask = 0xffff;
  bcm_dnx_field_db_t *ctxt;
  bcm_field_entry_info_t *entry;
  int rv;

  if (etuple == NULL) return RET_CODE_ARG_INVALID;
//  if (etuple->ctxt == NULL) return RET_CODE_ARG_INVALID;
//
//  ctxt = (bcm_dnx_field_db_t *) etuple->ctxt;

//  entry = ((etuple->direction == BCM_DNX_PMF_EGRESS) ?
//           etuple->entry_id__0 : etuple->entry_id);
  entry = etuple->ent_info;

//  rv = bcm_field_qualify_IpProtocolCommon(unit, entry,
//                                          (etuple->type == BCM_DNX_FG_IPV6 ?
//                                           bcmFieldIpProtocolCommonIp6Icmp :
//                                           bcmFieldIpProtocolCommonIcmp));
  rv = bcm_dnx_fe_qual_add32(
      entry,
      //TODO: broken
      udq_ttl_proto,
      etuple->type == BCM_DNX_FG_IPV6 ? 58 : 1, 0xff);
  if (rv != BCM_E_NONE) {
    bcmsdk_trace_acl_gen_error("qualify_Ip4Protocol failed",
                               bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }

//  rv = bcm_field_qualify_data(unit, entry, ctxt->ud_icmp[0],
//                              icmptypecode, icmpmask, 2);
  rv = bcm_dnx_fe_qual_add32(entry, bcmFieldQualifyL4SrcPort, &icmptypecode, &icmpmask, 2);
  if (rv != BCM_E_NONE) {
    bcmsdk_trace_acl_gen_error("qualify_data icmptypecode failed",
                               bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }

// Ignore other unit
//  if (etuple->direction == BCM_DNX_PMF_EGRESS) {
//    entry = etuple->entry_id__1;
//    rv = bcm_field_qualify_IpProtocolCommon((unit ? 0 : 1), entry,
//                                            (etuple->type == BCM_DNX_FG_IPV6 ?
//                                             bcmFieldIpProtocolCommonIp6Icmp :
//                                             bcmFieldIpProtocolCommonIcmp));
//    if (rv != BCM_E_NONE) {
//      bcmsdk_trace_acl_gen_error("qualify_IpProtocolCommon ingress failed",
//                                 bcm_errmsg(rv));
//      return RET_CODE_FAILURE;
//    }
//
//    rv = bcm_field_qualify_data((unit ? 0 : 1), entry, ctxt->ud_icmp[0],
//                                icmptypecode, icmpmask, 2);
//    if (rv != BCM_E_NONE) {
//      bcmsdk_trace_acl_gen_error("qualify_data for icmptypecode ingress failed",
//                                 bcm_errmsg(rv));
//      return RET_CODE_FAILURE;
//    }
//    etuple->flags = 1;
//  }

  return RET_CODE_SUCCESS;
}
static int
bcm_dnx_sdk_fe_replace(int unit, bcm_field_group_t fg,
    bcm_field_entry_info_t *ent_info,
    bcm_field_entry_t *id)
{
  uint32 flags = 0;
  int rv;
  if (BCM_DNX_INVALID_ID != *id) {
    /* update */
    flags |= BCM_FIELD_FLAG_WITH_ID;
    rv = bcm_field_entry_delete(unit, fg, NULL, *id);
    if (rv != BCM_E_NONE) {
      printf("bcm_field_entry_delete fg:%d entry:%d during update",
              fg, *id);
      return RET_CODE_FAILURE;
    }
  }
  rv = bcm_field_entry_add(unit, flags, fg, ent_info, id);
  if (rv != BCM_E_NONE) {
    printf("bcm_field_entry_add fg:%d id:%d flags:%x", fg, *id, flags);
    return rv;
  }
  return rv;
}

static ret_code_e
bcm_sdk_test_with_etuple(int unit, int act,
    int prio, int fg, bcm_field_entry_t *id)
{
  bcm_dnx_entry_tuple_t etuple;
  bcm_field_entry_info_t ent_info;
  etuple.ent_info = &ent_info;
  etuple.entry_id = *id;
  int rv, i;
  bcm_field_entry_info_t_init(&ent_info);
  ent_info.priority = prio;
  ent_info.nof_entry_quals = 0;
  ent_info.nof_entry_actions = 0;
  rv = bcm_dnx_sdk_fe_set_IcmpTypeCode(unit, &etuple, 8, 0);
  if (rv) {
    printf("Error (%d), bcm_dnx_sdk_fe_set_IcmpTypeCode\n", rv);
    return rv;
  }

  rv = bcm_dnx_fe_action_add32(&ent_info, act, 1);
  if (rv) {
    printf("Error (%d), bcm_dnx_fe_action_add(drop_aid)\n", rv);
    return rv;
  }
  rv = bcm_dnx_sdk_fe_replace(unit, fg, &ent_info, &etuple.entry_id);
  if (rv != BCM_E_NONE) {
    printf("Error (%d), in bcm_field_entry_add\n", rv);
    return rv;
  }
  printf("Etuple entry add: id:(0x%x)\n", etuple.entry_id);
  *id = etuple.entry_id;
  return RET_CODE_SUCCESS;
}

ret_code_e
bcm_dnx_pmf_test_run (int unit /*, bcm_dnx_device_ctxt_t *dev_ctxt*/)
{
  bcm_dnx_group_tuple_t gtuple;
  bcm_dnx_fg_type_t type = BCM_DNX_FG_NONE + 1;
  bcm_dnx_field_db_t *ctxt = NULL;
  ret_code_e ret;
  bcm_error_t rv;

//  if (dev_ctxt == NULL) return RET_CODE_ARG_INVALID;
//  ctxt = (bcm_dnx_field_db_t *) dev_ctxt->pmf_ctxt;
//  if (ctxt == NULL) return RET_CODE_ARG_INVALID;
//  if (ctxt->unit != unit) return RET_CODE_ARG_INVALID;

  init_test_run = 1;
  while(type < BCM_DNX_FG_MAX) {
    bcm_dnx_fg_direction_t direction = BCM_DNX_PMF_NONE + 1;

    while (direction < BCM_DNX_PMF_MAX) {
      bcm_dnx_target_type_t target = BCM_DNX_TARGET_NONE + 1;

      if (direction == BCM_DNX_PMF_EGRESS &&
          (type == BCM_DNX_FG_IPV6_PRE ||
              type == BCM_DNX_FG_DEFAULT_COPP ||
              type == BCM_DNX_FG_USER_COPP)) {
        direction+=1;
        continue;
      }

      while (target < BCM_DNX_TARGET_MAX) {
        gtuple.priority  = BCM_DNX_SDK_GROUP_PRIORITY_ANY;
        gtuple.group_id  = BCM_DNX_INVALID_ID;
        gtuple.presel_id = BCM_DNX_INVALID_ID;
        gtuple.ctxt      = ctxt;
//        gtuple.source    = ((direction == BCM_DNX_PMF_EGRESS) ?
//                            BCM_DNX_SDK_COUNTER_EGRESS_PMF :
//                            BCM_DNX_SDK_COUNTER_INGRESS_PMF);

        // gtuple.tuple_flag = false;
        ret = bcm_dnx_sdk_fg_create(unit, type, direction, target, &gtuple);
        if (ret != RET_CODE_SUCCESS) {
          bcmsdk_trace_acl_fp_grp_error("Test FP GRP Create Failed", NULL, direction, target,
                                        type, 0XFFFFFFFF, unit, ret);
          init_test_run = 0;
          return ret;
        }
        if (gtuple.group_id  != BCM_DNX_INVALID_ID) {
          bshell(0,"field group info group=10");
          ret = bcm_dnx_sdk_fg_destroy(unit, &gtuple);
          if (ret != RET_CODE_SUCCESS) {
            bcmsdk_trace_acl_fp_grp_error("Test FP GRP destroy Failed", NULL, direction, target,
                                          type, gtuple.group_id, unit, ret);
            init_test_run = 0;
            return ret;
          }
        }
        target+=1;
      }
      direction+=1;
    }
    type+=1;
  }
  init_test_run = 0;

  return RET_CODE_SUCCESS;
}

bcm_dnx_group_tuple_t g_gtuple[100];
int g_gtuple_size = 0;

ret_code_e
bcm_dnx_pmf_test_fgs_destroy_int(int unit, bcm_dnx_group_tuple_t *gtuple, int num)
{
  while (num) {
    if (gtuple->presel_id) {
      bcm_dnx_sdk_fe_destroy(unit, gtuple->presel_id, gtuple->group_id);
      gtuple->presel_id = 0;
      int rv = bcm_dnx_pmf_tcam_fg_info_update(unit, &g_bcm_dnx_tcam_info, gtuple->group_id, -1);
      if (rv) {
        printf("fg_info_update failed during FG destroy, FG ID:%d\n", gtuple->group_id);
      }
      //return RET_CODE_SUCCESS; //remove
    }
    ret_code_e ret = bcm_dnx_sdk_fg_destroy(unit, gtuple);
    if (ret != RET_CODE_SUCCESS) {
      bcmsdk_trace_acl_fp_grp_error("FG destroy Failed", NULL,
                                    gtuple->direction, gtuple->target_type,
                                    gtuple->type, gtuple->group_id, unit, ret);
      //return ret;
    } else {
      gtuple->group_id  = BCM_DNX_INVALID_ID;
    }
    gtuple ++;
    num --;
  }
  return RET_CODE_SUCCESS;
}

ret_code_e
bcm_dnx_pmf_test_fgs_destroy (int unit)
{
  bcm_field_entry_delete_all(unit);
  bcm_dnx_pmf_test_fgs_destroy_int(unit, g_gtuple, g_gtuple_size);
  g_gtuple_size = 0;
  return RET_CODE_SUCCESS;
}

/* try to enumerate all supported combinations */
static bcm_dnx_group_tuple_t fgs_port_vlan_ing[] = {
    {BCM_DNX_FG_PRE_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_PRE_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
    {BCM_DNX_FG_COPP_HDR, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_IPV4, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_IPV4, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
    {BCM_DNX_FG_IPV6, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_IPV6, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
    {BCM_DNX_FG_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
    {BCM_DNX_FG_IPV4, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PORT,}, //target irrelevant
};

static bcm_dnx_group_tuple_t fgs_pv_ing[] = {
    {BCM_DNX_FG_PRE_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_COPP_HDR, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_IPV4, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_IPV6, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
//    {BCM_DNX_FG_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_QOS_V4, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_IPV4, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PV,},
};

static bcm_dnx_group_tuple_t fgs_copp_ing[] = {
    {BCM_DNX_FG_DEFAULT_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_DEFAULT_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
    {BCM_DNX_FG_USER_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_USER_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
};
static bcm_dnx_group_tuple_t fgs_copp_pv_ing[] = {
    {BCM_DNX_FG_DEFAULT_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_USER_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
};
static bcm_dnx_group_tuple_t fgs_egr[] = {
    {BCM_DNX_FG_IPV6, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_L2, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PORT,},
};

static bcm_dnx_group_tuple_t fgs_pv_egr[] = {
    {BCM_DNX_FG_IPV6, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_L2, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PV,},
};
static bcm_dnx_group_tuple_t fgs_pv_qos[] = {
    {BCM_DNX_FG_QOS_V6, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_QOS_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
};
static bcm_dnx_group_tuple_t fgs_pv_tuple[] = {
    {BCM_DNX_FG_PRE_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_COPP_HDR, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_TUPLE, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_TUPLE6, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_TUPLE, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PV,},
};
static bcm_dnx_group_tuple_t fgs_pv_egr_tuple[] = {
    {BCM_DNX_FG_TUPLE6, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PV,},
};

static int
bcm_dnx_pmf_test_get_valid_port(int unit, int index)
{
  bcm_port_config_t port_config;
  bcm_port_t port;
  bcm_port_config_t_init(&port_config);
  bcm_port_config_get(unit, &port_config);
  BCM_PBMP_ITER(port_config.nif, port) {
    if (0 == index) {
      BCM_GPORT_LOCAL_SET(port, port);
      return port;
    }
    index --;
  }
  return 1;
}

int
bcm_dnx_pmf_test_fg_populate(int unit, bcm_dnx_group_tuple_t *gtuple, int core)
{
  bcm_field_group_t group_id = gtuple->group_id;
  bcm_field_group_info_t fg_info;
  bcm_field_entry_info_t ent_info;
  int rv, i;
  TRY(bcm_field_group_info_get(unit, group_id, &fg_info));
  bcm_field_entry_info_t_init(&ent_info);
  ent_info.priority = 1;
  ent_info.nof_entry_quals = 0;
  ent_info.nof_entry_actions = 0;
  if (bcmFieldActionStatSampling != fg_info.action_types[0] &&
      bcmFieldActionMirrorEgress != fg_info.action_types[0]) {
    TRY(bcm_dnx_fe_action_add32(&ent_info, fg_info.action_types[0], 1));
  }
  for (i = 0; i < fg_info.nof_quals; i++) {
    int val = i;
    if (bcmFieldQualifyInPort == fg_info.qual_types[i])
      val = bcm_dnx_pmf_test_get_valid_port(unit, 0);
    if (bcmFieldQualifyDstGport == fg_info.qual_types[i]) {
      fg_info.qual_types[i] = bcmFieldQualifyDstPort;
    }
    if (bcmFieldQualifyDstPort == fg_info.qual_types[i]) {
      if (BCM_DNX_PMF_EGRESS == gtuple->direction)
        BCM_GPORT_SYSTEM_PORT_ID_SET(val, val);
      else {
//        val |= 0x0c0000;
//        print *gtuple;
//        print val;
      }
    }
//    if (bcmFieldQualifyIpHasOptions == fg_info.qual_types[i])
//      val = 1;
//    if (bcmFieldQualifyIp6FirstAdditionalHeaderExist == fg_info.qual_types[i])
//      val = 1;
//    if (bcmFieldQualifyIpFrag == fg_info.qual_types[i])
//      val = 1;
    if (udq_frag == fg_info.qual_types[i])
      val = 1;
    if (udq_options == fg_info.qual_types[i])
      val = 1;
    if (bcmFieldQualifyPacketLengthRangeCheck == fg_info.qual_types[i])
      val = 1;
    TRY(bcm_dnx_fe_qual_add32(&ent_info, fg_info.qual_types[i], val,
                              1 | (val << 1)));
  }
  TRY(bcm_field_entry_add(unit, 0, group_id, &ent_info, &gtuple->presel_id));
  TRY(bcm_dnx_pmf_tcam_fg_info_update(unit, &g_bcm_dnx_tcam_info, group_id, 1));
  return rv;
}

ret_code_e
bcm_dnx_pmf_test_fg_set(int unit, bcm_dnx_group_tuple_t *gtuple, int num)
{
  ret_code_e ret;
  int rv;
  while (num--) {
    gtuple->priority = gtuple->type * BCM_DNX_TARGET_MAX + gtuple->target_type;
    gtuple->group_id = BCM_DNX_INVALID_ID;
    ret = bcm_dnx_sdk_fg_create(unit, gtuple->type, gtuple->direction,
                                gtuple->target_type, gtuple);
    if (ret != RET_CODE_SUCCESS) {
      bcmsdk_trace_acl_fp_grp_error("Test FP GRP Create Failed", NULL,
                                    gtuple->direction, gtuple->target_type,
                                    gtuple->type, 0XFFFFFFFF, unit, ret);
      return ret;
    }
    TRY(bcm_dnx_pmf_test_fg_populate(unit, gtuple, -1));
    if (verbose) {
      char cmd[128];
      snprintf(cmd, 128, "field group info group=%d", gtuple->group_id);
      bshell(0, "field  context info context=5 stage=ipmf1");
      //bshell(0, cmd);
    }
    gtuple++;
  }
  return RET_CODE_SUCCESS;
}

bcm_field_group_t copp_id;

ret_code_e
bcm_dnx_pmf_fgs_test_all(int unit, bcm_dnx_group_tuple_t *ing, int num_i,
    bcm_dnx_group_tuple_t *copp, int num_c,
    bcm_dnx_group_tuple_t *egr, int num_e,
    bcm_dnx_group_tuple_t *qos, int num_q, int pv)
{
  int rv;
  int c = -1, e = -1, q = -1;
  TRY2(bcm_dnx_pmf_contexts_setup(unit, pv));
  TRY2(bcm_dnx_pmf_test_fg_set(unit, ing, num_i));
  for (c = 0; c < num_c; c ++) {
    TRY2(bcm_dnx_pmf_test_fg_set(unit, copp + c, 1));
    for (e = 0; e < num_e; e ++) {
      TRY2(bcm_dnx_pmf_test_fg_set(unit, egr + e, 1));
      for (q = 0; q < num_q; q ++) {
        TRY2(bcm_dnx_pmf_test_fg_set(unit, qos + q, 1));
        bcm_dnx_pmf_test_fgs_destroy_int(unit, qos + q, 1);
      }
      bcm_dnx_pmf_test_fgs_destroy_int(unit, egr + e, 1);
    }
    bcm_dnx_pmf_test_fgs_destroy_int(unit, copp + c, 1);
  }
  bcm_dnx_pmf_test_fgs_destroy_int(unit, ing, num_i);
  bcm_dnx_pmf_contexts_destroy(unit);
  return RET_CODE_SUCCESS;
}

void
bcm_dnx_pmf_fgs_test_clear(int unit, bcm_dnx_group_tuple_t *ing, int num_i,
    bcm_dnx_group_tuple_t *copp, int num_c,
    bcm_dnx_group_tuple_t *egr, int num_e,
    bcm_dnx_group_tuple_t *qos, int num_q, int pv)
{
  bcm_dnx_pmf_test_fgs_destroy_int(unit, ing, num_i);
  bcm_dnx_pmf_test_fgs_destroy_int(unit, copp, num_c);
  bcm_dnx_pmf_test_fgs_destroy_int(unit, egr, num_e);
  bcm_dnx_pmf_test_fgs_destroy_int(unit, qos, num_q);
  bcm_dnx_pmf_contexts_destroy(unit);
}

#define FG_TEST(tc, ing, copp, egr, qos, pv) \
ret_code_e \
fgs_##tc(int unit) \
{ \
  return bcm_dnx_pmf_fgs_test_all(unit, (ing), DIM(ing), copp, DIM(copp), egr, DIM(egr), qos, DIM(qos), pv); \
} \
void \
fgs_##tc##_c(int unit) \
{ \
  return bcm_dnx_pmf_fgs_test_clear(unit, (ing), DIM(ing), copp, DIM(copp), egr, DIM(egr), qos, DIM(qos), pv); \
}

//FG_TEST(1, fgs_port_vlan_ing, fgs_copp_ing, fgs_egr, 0)
FG_TEST(2, fgs_pv_ing, fgs_copp_pv_ing, fgs_pv_egr, fgs_pv_qos, 1)
FG_TEST(3, fgs_pv_tuple, fgs_copp_pv_ing, fgs_pv_egr_tuple, fgs_pv_qos, 1)

ret_code_e
bcm_dnx_pmf_fgs_wc(int unit, bcm_dnx_group_tuple_t *fgs, int num, int pv)
{
  int rv;
  TRY(bcm_dnx_pmf_contexts_setup(unit, pv));
  return bcm_dnx_pmf_test_fg_set(unit, fgs, num);
}
void
bcm_dnx_pmf_fgs_wc_clear(int unit, bcm_dnx_group_tuple_t *fgs, int num, int pv)
{
  bcm_dnx_pmf_test_fgs_destroy_int(unit, fgs, num);
  bcm_dnx_pmf_contexts_destroy(unit);
}

int
bcm_dnx_pmf_fgs_fe_scale_up(int unit, bcm_dnx_group_tuple_t *gtuple, int num, int *ids, int core)
{
  int i, rv;
  int save_presel_id = gtuple->presel_id;
  for (i = 0; i < num; i ++) {
    rv = bcm_dnx_pmf_test_fg_populate(unit, gtuple, core);
    ids[i] = gtuple->presel_id;
    if (rv) {
      gtuple->presel_id = save_presel_id;
      return i;
    }
  }
  gtuple->presel_id = save_presel_id;
  return num;
}

void
bcm_dnx_pmf_fgs_fe_scale_down(int unit, bcm_dnx_group_tuple_t *gtuple, int num, int *ids)
{
  int i, rv;
  for (i = 0; i < num; i++) {
    bcm_dnx_sdk_fe_destroy(unit, ids[i], gtuple->group_id);
    int rv = bcm_dnx_pmf_tcam_fg_info_update(unit, &g_bcm_dnx_tcam_info,
                                             gtuple->group_id, -1);
    if (rv) {
      printf("fg_info_update failed during scale down, FG ID:%d\n",
             gtuple->group_id);
    }
  }
}

#define FG_WC(tc, ing, pv) \
ret_code_e \
fg_wc_##tc(int unit) \
{ \
  return bcm_dnx_pmf_fgs_wc(unit, (ing), DIM(ing), pv); \
} \
void \
fg_wc_##tc##_c(int unit) \
{ \
  return bcm_dnx_pmf_fgs_wc_clear(unit, (ing), DIM(ing), pv); \
}


static bcm_dnx_group_tuple_t fgs_wc_port_vlan[] = {
    {BCM_DNX_FG_PRE_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_PRE_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
    {BCM_DNX_FG_COPP_HDR, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_IPV4, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_IPV4, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
    {BCM_DNX_FG_IPV6, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_IPV6, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
    {BCM_DNX_FG_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
    {BCM_DNX_FG_USER_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PORT,},
    {BCM_DNX_FG_USER_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_VLAN,},
    {BCM_DNX_FG_IPV6, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PORT,},
};
//NOTE: programming order makes some difference in free FFCs for initial keys?
static bcm_dnx_group_tuple_t fgs_wc_pv[] = {
    {BCM_DNX_FG_PRE_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_COPP_HDR, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    //{BCM_DNX_FG_QOS_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_IPV4, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_IPV6, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_QOS_V4, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_QOS_V6, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    //{BCM_DNX_FG_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_USER_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_IPV6, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_IPV4, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PV,},
};

static bcm_dnx_group_tuple_t fgs_copp_pv[] = {
    {BCM_DNX_FG_PRE_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
    {BCM_DNX_FG_USER_COPP, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,},
};

FG_WC(1, fgs_wc_port_vlan, 0)
FG_WC(2, fgs_wc_pv, 1)

ret_code_e
bcm_dnx_pmf_test_fgs (int unit, int tuple, int usr_copp, int pv, bcm_dnx_fg_direction_t direction)
{
  bcm_dnx_group_tuple_t *gtuple = g_gtuple;
  //g_gtuple_size = 0;
  bcm_dnx_fg_type_t type = BCM_DNX_FG_NONE + 1;
  bcm_dnx_field_db_t *ctxt = NULL;
  ret_code_e ret;

  while(type < BCM_DNX_FG_MAX) {
    // only one type is supported on egress
    if (direction == BCM_DNX_PMF_EGRESS) {
      bshell(0, "field system info stage=epmf");
      bcm_dnx_pmf_test_fgs_destroy(unit);
      gtuple = g_gtuple;
    }
    // mutual exclusion rules, plus unused types
    if ((tuple && (BCM_DNX_FG_IPV4 == type || BCM_DNX_FG_IPV6 == type || BCM_DNX_FG_L2 == type)) ||
        (!tuple && (BCM_DNX_FG_TUPLE == type || BCM_DNX_FG_TUPLE6 == type)) ||
        (usr_copp && (BCM_DNX_FG_DEFAULT_COPP == type)) ||
        (!usr_copp && (BCM_DNX_FG_USER_COPP == type)) ||
        (BCM_DNX_FG_MIX == type)) {
      //skip type
      type += 1;
      continue;
    }
    bcm_dnx_target_type_t target = pv ? BCM_DNX_TARGET_PV : BCM_DNX_TARGET_NONE + 1;

    if (direction == BCM_DNX_PMF_EGRESS &&
        (type == BCM_DNX_FG_IPV6_PRE ||
            type == BCM_DNX_FG_DEFAULT_COPP ||
            type == BCM_DNX_FG_USER_COPP)) {
      type += 1;
      continue;
    }

    while (target < (pv ? BCM_DNX_TARGET_PV + 1 : BCM_DNX_TARGET_MAX)) {
      if (BCM_DNX_TARGET_PV == target && !pv) {
        target += 1;
        continue;
      }
      if (direction == BCM_DNX_PMF_EGRESS && BCM_DNX_TARGET_VLAN == target) {
        //PORT and VLAN targets are identical
        target += 1;
        continue;
      }
      // unique priorities are needed
      gtuple->priority  = type * BCM_DNX_TARGET_MAX + target;
      gtuple->group_id  = BCM_DNX_INVALID_ID;
//        gtuple.presel_id = BCM_DNX_INVALID_ID;
      gtuple->ctxt      = ctxt;
//        gtuple.source    = ((direction == BCM_DNX_PMF_EGRESS) ?
//                            BCM_DNX_SDK_COUNTER_EGRESS_PMF :
//                            BCM_DNX_SDK_COUNTER_INGRESS_PMF);

      // gtuple.tuple_flag = false;
      ret = bcm_dnx_sdk_fg_create(unit, type, direction, target, gtuple);
      if (BCM_DNX_INVALID_ID != gtuple->group_id) g_gtuple_size ++;
      if (ret != RET_CODE_SUCCESS) {
        bcmsdk_trace_acl_fp_grp_error("Test FP GRP Create Failed", NULL, direction, target,
                                      type, 0XFFFFFFFF, unit, ret);
        return ret;
      }
      gtuple ++;
      target+=1;
    }
    type+=1;
  }
  if (direction == BCM_DNX_PMF_INGRESS) {
    bshell(0, "field system info stage=ipmf1");
    bshell(0, "field system info stage=ipmf2");
  }
  return RET_CODE_SUCCESS;
}


#if 0
bcm_field_qualify_t egress_fg_qualify[] = {
  bcmFieldQualifyStageEgress
};

bcm_field_qualify_t ingress_fg_qualify[] = {
  bcmFieldQualifyStageIngress
};
#endif


char *p_t_v4_udp=
//DMAC         SMAC         TPID VLAN ETYP VL DC LEN  IPID F FR  TL PR CKSM SIP      DIP      L4SP L4DP
 "000000000002 000000000001 8100 0032 0800 45 00 0035 0000 0 000 80 11 26c4 0a000005 0a000001 0011 0203 0405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20";
char *p_t_v4_udp_flood=
//DMAC         SMAC         TPID VLAN ETYP VL DC LEN  IPID F FR  TL PR CKSM SIP      DIP      L4SP L4DP
 "000000000022 000000000001 8100 0032 0800 45 00 0035 0000 0 000 80 11 26c4 0a000005 0a000001 0011 0203 0405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20";
char *p_t_v4_tcp=
//DMAC         SMAC         TPID VLAN ETYP VL DC LEN  IPID F FR  TL PR CKSM SIP      DIP      L4SP L4DP SEQ      ACK      HR FL
 "000000000003 000000001123 8100 0033 0800 45 01 0035 0000 0 000 81 06 26c4 0a000006 0a000002 0012 0204 04050607 08090a0b 0c 01 0e0f101112131415161718191a1b1c1d1e1f20";
char *p_v4_tcp=
//DMAC         SMAC         ETYP VL DC LEN  IPID F FR  TL PR CKSM SIP      DIP      L4SP L4DP
 "000000000001 000000001124 0800 45 02 0035 0000 0 000 82 06 26c4 0a000007 0a000003 0013 0205 0405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20";
char *p_v4_tcpo=
//DMAC         SMAC         ETYP VL DC LEN  IPID F FR  TL PR CKSM SIP      DIP      OPTIONS  L4SP L4DP
 "000000000001 000000001124 0800 46 06 0035 0000 0 000 82 06 26c4 0a000007 0a000003 00000000 0013 0205 0405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20";
//print str(Ether(dst="00:00:00:00:00:02", src="00:00:00:00:11:22")/Dot1Q(vlan=50)/IPv6(src="2006::1", dst="2007::1")/"abcdefghijklm").encode("HEX")
char *p_t_v6_fake_v4tcp=
//DMAC         SMAC         TPID VLAN ETYP VL DC LEN  IPID F FR  TL PR CKSM SIP      DIP      L4SP L4DP
 "000000000002 000000001122 8100 0032 86dd 60 00 0000 000d 3 b40 20 06 0000 00000000 00000000 0000 0001 200700000000000000000000000000016162636465666768696a6b6c6d";

#define PKTSZ 256
#define PKTS 5
static char pkts[PKTS][PKTSZ];


typedef struct bcm_dnx_sdk_test_qual_t {
  bcm_field_entry_qual_t q;
  bcm_field_action_t action;
  int match_pkt;
} /*bcm_dnx_sdk_test_qual_t*/;
typedef struct bcm_dnx_sdk_test_pkt_t {
  char *pkt_def;
  int port;
} /*bcm_dnx_sdk_test_pkt_t*/;
typedef struct bcm_dnx_sdk_test_t {
  bcm_dnx_fg_type_t type;
  bcm_dnx_fg_direction_t direction;
  bcm_dnx_target_type_t target_type;
  bcm_dnx_sdk_test_pkt_t pkts[PKTS];
  char *verify_cmd;
  bcm_dnx_sdk_test_qual_t quals[32];
} /*bcm_dnx_sdk_test_t*/;

static void
bcm_dnx_create_pkt(int id, char *def, int port)
{
  char data[PKTSZ];
  data[0] = 0;
  char *d = data;
  while (*def) {
    if (*def != ' ') *d++ = *def;
    def ++;
  }
  snprintf(pkts[id], PKTSZ, "tx 1 psrc=%d DATA=0x%s", port, data);
}

static ret_code_e
bcm_dnx_sdk_test_add_fe (int unit, bcm_dnx_group_tuple_t *gtuple,
    bcm_field_entry_qual_t *qual, bcm_field_action_t action,
    bcm_field_entry_t *entry_handle)
{
  bcm_field_entry_info_t ent_info;
  int rv;
  bcm_field_entry_info_t_init(&ent_info);
  ent_info.priority = 1;
  ent_info.nof_entry_quals = 1;
  ent_info.nof_entry_actions = 1;
  ent_info.entry_qual[0] = *qual;
  ent_info.entry_action[0].type = action;
  ent_info.entry_action[0].value[0] = redir_aid == action ? 0x1FFFFF : 1;
  TRY(bcm_field_entry_add(unit, 0 & BCM_FIELD_FLAG_WITH_ID, gtuple->group_id, &ent_info, entry_handle));
  return RET_CODE_SUCCESS;
}
/*
 * L2 ingress (I/L2/PV)
=========================================================================================
| Key & Payload Info                                                                    |
=========================================================================================
| Qualifier | Qual | Qual |   | Action        | Base Action           | Action | Action |
|           | Size | Lsb  |   |               |                       | Size   | Lsb    |
=========================================================================================
| MAC_SRC   | 48   | 0    |   | action_drop_1 | DST_DATA (UDA ID: 24) | 1      | 6      |
-----------------------------------------------------------------------------------------
| MAC_DST   | 48   | 48   |   | DP            | N/A                   | 2      | 8      |
-----------------------------------------------------------------------------------------
| ETHERTYPE | 16   | 96   |   | FWD_DATA      | N/A                   | 32     | 11     |
-----------------------------------------------------------------------------------------
| IN_PORT   | 9    | 112  |   | SNOOP_DATA    | N/A                   | 20     | 44     |
-----------------------------------------------------------------------------------------
| VLAN_ID   | 12   | 121  |   |               |                       |        |        |
=========================================================================================
 */
static bcm_dnx_sdk_test_t l2_i = {
    BCM_DNX_FG_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,
    {
        {p_t_v4_udp, 1},
        {p_t_v4_tcp, p13},
        {p_v4_tcp, p16}, /* untagged go to 3rd VSI */
    },
    "sig get name=fwd_action_d from=ipmf3",
    {
    {{bcmFieldQualifySrcMac, {0x1}, {0}, {0xffff}}, redir_aid, 0},
    {{bcmFieldQualifyDstMac, {0x3}, {0}, {0xffff}}, redir_aid, 1},
    {{udq_vsi, {0x13ba}, {0}, {0x1fff}}, redir_aid, 0},
    //BCM_GPORT_LOCAL_SET(in_port_gport, in_port=13); //13
    {{bcmFieldQualifyInPort, {0x4000000 | p13}, {0}, {0x1ff}}, redir_aid, 1},
    {{bcmFieldQualifyEtherTypeUntagged, {0x0800}, {0}, {0xffff}}, redir_aid, 2},
    {{0}, 0, -1},
    }
};

/*
 * IPV4 ingress (I/IPV4/PV)
===============================================================================================
| Key & Payload Info                                                                          |
===============================================================================================
| Qualifier       | Qual | Qual |   | Action        | Base Action           | Action | Action |
|                 | Size | Lsb  |   |               |                       | Size   | Lsb    |
===============================================================================================
| IPV4_PROTOCOL   | 8    | 0    |   | action_drop_1 | DST_DATA (UDA ID: 24) | 1      | 6      |
-----------------------------------------------------------------------------------------------
| IPV4_TOS        | 8    | 8    |   | DP            | N/A                   | 2      | 8      |
-----------------------------------------------------------------------------------------------
| IPV4_HAS_OPTION | 1    | 16   |   | FWD_DATA      | N/A                   | 32     | 11     |
-----------------------------------------------------------------------------------------------
| IPV4_TTL        | 8    | 17   |   | SNOOP_DATA    | N/A                   | 20     | 44     |
-----------------------------------------------------------------------------------------------
| L4_DST_PORT     | 16   | 25   |   |               |                       |        |        |
-----------------------------------------------------------------------------------------------
| L4_SRC_PORT     | 16   | 41   |   |               |                       |        |        |
-----------------------------------------------------------------------------------------------
| IPV4_SIP        | 32   | 57   |   |               |                       |        |        |
-----------------------------------------------------------------------------------------------
| IPV4_DIP        | 32   | 89   |   |               |                       |        |        |
-----------------------------------------------------------------------------------------------
| IN_PORT         | 9    | 121  |   |               |                       |        |        |
-----------------------------------------------------------------------------------------------
| VLAN_ID         | 12   | 130  |   |               |                       |        |        |
===============================================================================================
 */
static bcm_dnx_sdk_test_t v4_i = {
    BCM_DNX_FG_IPV4, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,
    {
        {p_t_v4_udp, 1},
        {p_t_v4_tcp, p13},
        {p_v4_tcpo, p14},
    },
    "sig get name=fwd_action_d from=ipmf3",
    {
    {{udq_ttl_proto, {0x11}, {0}, {0xff}}, redir_aid, 0},
    {{udq_dscp, {0x02}, {0}, {0xff}}, redir_aid, 2},
    {{udq_dscp, {0x00}, {0}, {0xff}}, redir_aid, 0},
    {{udq_options, {0x1}, {0}, {0x1}}, redir_aid, 2},
    {{udq_ttl_proto, {0x8100}, {0}, {0xff00}}, redir_aid, 1},
//    {{bcmFieldQualifyL4DstPort, {0x0205}, {0xffff}}, redir_aid, 2}, //IPv4 with options missing L4 header start
    {{udq_ports, {0x0203}, {0}, {0xff}}, redir_aid, 0},
    {{udq_ports, {0x00110000}, {0}, {0xff0000}}, redir_aid, 0},
    {{bcmFieldQualifySrcIp, {0x0a000006}, {0}, {0xffffffff}}, redir_aid, 1},
    {{bcmFieldQualifyDstIp, {0x01}, {0}, {0xff}}, redir_aid, 0},
    {{udq_vsi, {0x13bb}, {0}, {0x1fff}}, redir_aid, 1},
    //BCM_GPORT_LOCAL_SET(in_port_gport, in_port=14);
    {{bcmFieldQualifyInPort, {0x4000000 | p14}, {0}, {0x1ff}}, redir_aid, 2},
    {{udq_tcpflags, {0x01}, {0}, {0x3f}}, redir_aid, 1},
    {{0}, 0, -1},
    }
};

static bcm_dnx_sdk_test_t v4_i_v6pkt = {
    BCM_DNX_FG_IPV4, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,
    {
        {p_t_v6_fake_v4tcp, 1},
    },
    "sig get name=fwd_action_d from=ipmf3",
    {
    {{udq_ttl_proto, {0x06}, {0}, {0xff}}, redir_aid, -3}, //it should not match
    {{0}, 0, -1},
    }
};


/*
 * V4 Tuple ingress (I/TUPLE/PV) DOES NOT WORK in Cmodel but works on HW
===============================================================================================
| Key & Payload Info                                                                          |
===============================================================================================
| Qualifier       | Qual | Qual |   | Action        | Base Action           | Action | Action |
|                 | Size | Lsb  |   |               |                       | Size   | Lsb    |
===============================================================================================
| IPV4_HAS_OPTION | 1    | 0    |   | action_drop_1 | DST_DATA (UDA ID: 24) | 1      | 70     |
-----------------------------------------------------------------------------------------------
| MAC_SRC         | 48   | 1    |   | DP            | N/A                   | 2      | 72     |
-----------------------------------------------------------------------------------------------
| MAC_DST         | 48   | 49   |   | FWD_DATA      | N/A                   | 32     | 75     |
-----------------------------------------------------------------------------------------------
| IPV4_SIP        | 32   | 97   |   | SNOOP_DATA    | N/A                   | 20     | 108    |
-----------------------------------------------------------------------------------------------
| IPV4_DIP        | 32   | 129  |   |               |                       |        |        |
-----------------------------------------------------------------------------------------------
| ETHERTYPE       | 16   | 161  |   |               |                       |        |        |
-----------------------------------------------------------------------------------------------
| IN_PORT         | 9    | 177  |   |               |                       |        |        |
-----------------------------------------------------------------------------------------------
| VLAN_ID         | 12   | 186  |   |               |                       |        |        |
===============================================================================================
 */
static bcm_dnx_sdk_test_t tuple_i = {
    BCM_DNX_FG_TUPLE, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV,
    {
        {p_t_v4_udp, 1},
        {p_t_v4_tcp, p13},
        {p_v4_tcp, p14},
    },
    "sig get name=fwd_action_d from=ipmf3",
    {
    {{udq_options, {0x0}, {0}, {0x1}}, redir_aid, -2}, //all should match
    {{bcmFieldQualifySrcMac, {0x1124}, {0}, {0xffff}}, redir_aid, 2},
    {{bcmFieldQualifyDstMac, {0x3}, {0}, {0xffff}}, redir_aid, 1},
    {{bcmFieldQualifySrcIp, {0x0a000007}, {0}, {0xffffffff}}, redir_aid, 2},
    {{bcmFieldQualifyDstIp, {0x01}, {0}, {0xff}}, redir_aid, 0},
    {{bcmFieldQualifyEtherTypeUntagged, {0x0800}, {0}, {0xffff}}, redir_aid, 2},
    {{udq_vsi, {0x13bb}, {0}, {0x1fff}}, redir_aid, 1},
    //BCM_GPORT_LOCAL_SET(in_port_gport, in_port=1);
    {{bcmFieldQualifyInPort, {0x4000001}, {0}, {0xf}}, redir_aid, 0},
    {{0}, 0, -1},
    }
};

/*
 * IPV4 Egress (E/IPV4/PV)
==========================================================================
| IPV4_PROTOCOL   | 8    | 0    |   | DISCARD | N/A    | 1      | 31     |
--------------------------------------------------------------------------
| IPV4_TOS        | 8    | 8    |   |         |        |        |        |
--------------------------------------------------------------------------
| IPV4_HAS_OPTION | 1    | 16   |   |         |        |        |        |
--------------------------------------------------------------------------
| IPV4_TTL        | 8    | 17   |   |         |        |        |        |
--------------------------------------------------------------------------
| L4_DST_PORT     | 16   | 25   |   |         |        |        |        |
--------------------------------------------------------------------------
| L4_SRC_PORT     | 16   | 41   |   |         |        |        |        |
--------------------------------------------------------------------------
| IPV4_SIP        | 32   | 57   |   |         |        |        |        |
--------------------------------------------------------------------------
| IPV4_DIP        | 32   | 89   |   |         |        |        |        |
--------------------------------------------------------------------------
| PPH_FWD_DOMAIN  | 18   | 121  |   |         |        |        |        |
--------------------------------------------------------------------------
| DST_SYS_PORT    | 16   | 139  |   |         |        |        |        |
==========================================================================
 */
static bcm_dnx_sdk_test_t v4_e = {
    BCM_DNX_FG_IPV4, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PV,
    {
        {p_t_v4_udp, 1},
        {p_t_v4_tcp, p13},
        {p_v4_tcpo, p14},
    },
    "sig get name=discard from=epmf",
    {
    {{udq_ttl_proto, {0x11}, {0}, {0xff}}, bcmFieldActionDrop, 0},
    {{udq_dscp, {0x02}, {0}, {0xff}}, bcmFieldActionDrop, 2},
    {{udq_dscp, {0x00}, {0}, {0xff}}, bcmFieldActionDrop, 0},
    {{udq_options, {0x1}, {0}, {0x1}}, bcmFieldActionDrop, 2},
    {{udq_ttl_proto, {0x8100}, {0}, {0xff00}}, bcmFieldActionDrop, 1},
//    {{bcmFieldQualifyL4DstPort, {0x0205}, {0xffff}}, bcmFieldActionDrop, 2}, //IPv4 with options missing L4 header start
    {{udq_ports, {0x0203}, {0}, {0xff}}, bcmFieldActionDrop, 0},
    {{udq_ports, {0x00110000}, {0}, {0xff0000}}, bcmFieldActionDrop, 0},
    {{bcmFieldQualifySrcIp, {0x0a000006}, {0}, {0xffffffff}}, bcmFieldActionDrop, 1},
    {{bcmFieldQualifyDstIp, {0x01}, {0}, {0xff}}, bcmFieldActionDrop, 0},
    {{udq_vsi, {0x13bb}, {0}, {0x1fff}}, bcmFieldActionDrop, 1},
    //BCM_GPORT_SYSTEM_PORT_ID_SET(sysport, 1);
    {{bcmFieldQualifyDstPort, {0x6c000001}, {0}, {0xff}}, bcmFieldActionDrop, 2},
    {{udq_tcpflags, {0x01}, {0}, {0x3f}}, bcmFieldActionDrop, 1},
    {{0}, 0, -1},
    }
};

/*
 * L2 egress (E/L2/PV)
=========================================================================
| MAC_SRC        | 48   | 0    |   | DISCARD | N/A    | 1      | 31     |
-------------------------------------------------------------------------
| MAC_DST        | 48   | 48   |   |         |        |        |        |
-------------------------------------------------------------------------
| ETHERTYPE      | 16   | 96   |   |         |        |        |        |
-------------------------------------------------------------------------
| PPH_FWD_DOMAIN | 18   | 112  |   |         |        |        |        |
-------------------------------------------------------------------------
| DST_SYS_PORT   | 16   | 130  |   |         |        |        |        |
=========================================================================
 */
static bcm_dnx_sdk_test_t l2_e = {
    BCM_DNX_FG_L2, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PV,
    {
        {p_t_v4_udp, 1},
        {p_t_v4_tcp, p13},
        {p_v4_tcp, p14},
    },
    "sig get name=discard from=epmf",
    {
    {{bcmFieldQualifySrcMac, {0x1}, {0}, {0xffff}}, bcmFieldActionDrop, 0},
    {{bcmFieldQualifyDstMac, {0x3}, {0}, {0xffff}}, bcmFieldActionDrop, 1},
    {{udq_vsi, {0x13bb}, {0}, {0xffff}}, bcmFieldActionDrop, 1},
    //BCM_GPORT_SYSTEM_PORT_ID_SET(sysport, 13);
    {{bcmFieldQualifyDstPort, {0x6c000000 | p13}, {0}, {0xff}}, bcmFieldActionDrop, 0},
    {{bcmFieldQualifyEtherTypeUntagged, {0x0800}, {0}, {0xffff}}, bcmFieldActionDrop, 2},
    {{0}, 0, -1},
    }
};

//p14=0 -> go to CPU
static bcm_dnx_sdk_test_t l2_copp = {
    BCM_DNX_FG_L2, BCM_DNX_PMF_EGRESS, BCM_DNX_TARGET_PV,
    {
        {p_t_v4_udp_flood, 1},
        {p_t_v4_tcp, p13},
    },
    "sig get name=discard from=epmf",
    {
    {{bcmFieldQualifyDstPort, {0x6c000000 | p14}, {0xff}}, bcmFieldActionDrop, 0}, //CPU flooded packet should be dropped
    {{bcmFieldQualifySrcMac, {0x1}, {0xffff}}, bcmFieldActionDrop, 0}, //all flooded must be dropped
    {{bcmFieldQualifySrcMac, {0x1123}, {0xffff}}, bcmFieldActionDrop, 1},
    {{0}, 0, -1},
    }
};

//NOTE new qualifier struct layout
static bcm_dnx_sdk_test_t copp = {
    BCM_DNX_FG_L2, BCM_DNX_PMF_INGRESS, BCM_DNX_TARGET_PV, //ignored
    {
        {p_t_v4_udp_flood, 1},
        {p_t_v6_fake_v4tcp, 1},
        {p_t_v4_tcp, p13},
    },
    "sig get name=fwd_action_d from=ipmf3",
    {
    //BCM_GPORT_LOCAL_SET(in_port_gport, in_port=1);
    {{bcmFieldQualifyInPort, {0x4000000 | 1}, {0x1ff}, {0x1ff}}, copp_aid, 0}, //PRE_COPP entry, match on inport=1
    {{bcmFieldQualifyDstMac, {0x22}, {0x1ff}, {0xffff}}, redir_aid_2, 0},
    {{bcmFieldQualifyDstMac, {0x3}, {0x1ff}, {0xffff}}, redir_aid_2, -3}, //none should match
    {{udq_ttl_proto, {0x06}, {0x1ff}, {0xffff}}, redir_aid_2, -3}, //none should match
    {{udq_ttl_proto, {0x11}, {0x1ff}, {0xffff}}, redir_aid_2, 0},
    {{udq_ttl_proto, {0x3b}, {0x1ff}, {0xffff}}, redir_aid_2, 1},
    {{bcmFieldQualifyRangeCheck, {0x1}, {0x1ff}, {0x1}}, redir_aid_2, 0},
    {{bcmFieldQualifyRangeCheck, {0x2}, {0x1ff}, {0x2}}, redir_aid_2, 0},
    {{0}, 0, -1},
    }
};
/*
 * IPV6 ingress
========================================================
| ID | Name      | Type | Stage       | Key  | Payload |
|    |           |      |             | Size | Size    |
========================================================
| 9  | I/IPV6/PV | TCAM | IngressPMF2 | 320  | 128     |
========================================================

=========================================================================================================
| Qualifier                 | Qual | Qual |   | Action        | Base Action           | Action | Action |
|                           | Size | Lsb  |   |               |                       | Size   | Lsb    |
=========================================================================================================
| IPV6_NEXT_HEADER          | 8    | 0    |   | action_drop_2 | DST_DATA (UDA ID: 25) | 1      | 103    |
---------------------------------------------------------------------------------------------------------
| IPV6_TC                   | 8    | 8    |   | DP            | N/A                   | 2      | 105    |
---------------------------------------------------------------------------------------------------------
| IPV6_1ST_ADD_HEADER_EXIST | 1    | 16   |   | SNOOP_DATA    | N/A                   | 20     | 108    |
---------------------------------------------------------------------------------------------------------
| L4_DST_PORT               | 16   | 17   |   |               |                       |        |        |
---------------------------------------------------------------------------------------------------------
| L4_SRC_PORT               | 16   | 33   |   |               |                       |        |        |
---------------------------------------------------------------------------------------------------------
| IPV6_HOP_LIMIT            | 8    | 49   |   |               |                       |        |        |
---------------------------------------------------------------------------------------------------------
| IPV6_FLOW_LABEL           | 20   | 57   |   |               |                       |        |        |
---------------------------------------------------------------------------------------------------------
| IPV6_SIP_HIGH             | 64   | 77   |   |               |                       |        |        |
---------------------------------------------------------------------------------------------------------
| IPV6_DIP                  | 128  | 141  |   |               |                       |        |        |
---------------------------------------------------------------------------------------------------------
| IN_PORT                   | 9    | 269  |   |               |                       |        |        |
---------------------------------------------------------------------------------------------------------
| VLAN_ID                   | 12   | 278  |   |               |                       |        |        |
=========================================================================================================
 */

static ret_code_e
run_acl_qual_tests(int unit, bcm_dnx_sdk_test_qual_t *q, bcm_dnx_group_tuple_t *gtuple, int i, char *verify_cmd)
{
  int rv;
  uint8 bmp;
  while (-1 != q->match_pkt) {
    bcm_field_entry_t e = 1;
    int p;
    TRY(bcm_dnx_sdk_test_add_fe(unit, gtuple, q->q, q->action, &e));
    //return RET_CODE_SUCCESS;
    for (p = 0; p < i; p++) {
      printf("%s\n", pkts[p]);
      if (p == q->match_pkt || -2 == q->match_pkt) {
        printf("!!!!!!!!!!!!!!It should be dropped!!!!!!!!!!!!!!!!!!!!!!\n");
      }
      bcm_field_entry_hit_flush(unit, BCM_FIELD_ENTRY_HIT_FLUSH_ALL, e);
      bshell(0, pkts[p]);
      if (on_hw) {
        bcm_field_entry_hit_get(unit, 0, e, &bmp);
        if (!!(p == q->match_pkt || -2 == q->match_pkt) ^ !!bmp) {
          print * q;
          printf("invalid match or match failed at packet #%d bmp:%x\n", p,
                 bmp);
          return RET_CODE_FAILURE;
        }
      }
      bshell(0, verify_cmd);
      printf("==========================end============================ %x\n",
             bmp);
    }
    TRY(bcm_dnx_sdk_fe_destroy(unit, e, gtuple->group_id));
    q++;
  }
  return RET_CODE_SUCCESS;
}

static ret_code_e
bcm_dnx_sdk_test_quals(int unit, bcm_dnx_sdk_test_t *test)
{
  int rv;
  bcm_dnx_pmf_test_fgs_destroy(0);
  int i = 0;
  while (i < PKTS && test->pkts[i].pkt_def) {
    bcm_dnx_create_pkt(i, test->pkts[i].pkt_def, test->pkts[i].port);
    i ++;
  }
  bcm_dnx_group_tuple_t *gtuple = g_gtuple;
  g_gtuple_size ++;
  gtuple->priority  = 1;
  TRY(bcm_dnx_sdk_fg_create(unit, test->type, test->direction, test->target_type, gtuple));
  bshell(0,"field group info group=7");
  TRY(run_acl_qual_tests(unit, test->quals, gtuple, i, test->verify_cmd));
  return RET_CODE_SUCCESS;
}

FG_WC(COPP, fgs_copp_pv, 1)

bcm_field_entry_t g_copp_e;
ret_code_e
bcm_dnx_sdk_test_copp(int unit, bcm_dnx_sdk_test_t *test)
{
  int rv;
  TRY(fg_wc_COPP(unit));
  TRY(bcm_dnx_sdk_fe_write_range(unit, bcmFieldRangeTypeL4SrcPort, 0x10, 0x11,
                                 bcmFieldStageIngressPMF1, 0));
  TRY(bcm_dnx_sdk_fe_write_range(unit, bcmFieldRangeTypeL4DstPort, 0x203, 0x303,
                                 bcmFieldStageIngressPMF1, 1));
  int i = 0;
  while (i < PKTS && test->pkts[i].pkt_def) {
    bcm_dnx_create_pkt(i, test->pkts[i].pkt_def, test->pkts[i].port);
    i++;
  }
  bcm_dnx_sdk_test_qual_t *q = test->quals;
  TRY(bcm_dnx_sdk_test_add_fe(unit, fgs_copp_pv, q->q, q->action, &g_copp_e));
  q ++;
  TRY(run_acl_qual_tests(unit, q, fgs_copp_pv + 1, i, test->verify_cmd));
  return RET_CODE_SUCCESS;
}

void
bcm_dnx_sdk_test_copp_c(int unit)
{
  bcm_dnx_sdk_fe_destroy(unit, g_copp_e, fgs_copp_pv->group_id);
  fg_wc_COPP_c(unit);
}

static bcm_field_range_type_t
bcm_dnx_get_range_type(bcm_dnx_range_id_t id)
{
  if (id == BCM_DNX_SP_RANGE) return bcmFieldRangeTypeL4SrcPort;
  else return bcmFieldRangeTypeL4DstPort;
}

static int
bcm_dnx_sdk_fe_write_range(int unit,
    bcm_field_range_type_t type,
    uint16_t min, uint16_t max,
    bcm_field_stage_t stage,
    bcm_field_range_t range_id)
{
  int rv;
  bcm_field_range_info_t range_info;
  bcm_field_range_info_t_init(&range_info);
  range_info.range_type = type;
  range_info.min_val = min;
  range_info.max_val = max;
  rv = bcm_field_range_set(unit, 0, stage, range_id, &range_info);

  if (rv != BCM_E_NONE) {
    printf("bcm_field_range_set id:%d, min:%d max:%d: rv:%d",
            range_id, min, max, rv);
  }
  return rv;
}

static ret_code_e
bcm_dnx_sdk_fe_find_range(int unit,
    bcm_field_range_type_t type,
    uint16_t min, uint16_t max,
    bcm_field_stage_t stage,
    bcm_field_range_t *range_id)
{
  bcm_field_range_t i, free = 0xffff;
  int rv;
  bcm_field_range_info_t range_info;
  for (i = 0; i < 24; i++) {
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = type;
    rv = bcm_field_range_info_get(unit, stage, &i, &range_info);
    if (rv != BCM_E_NONE) {
      printf("bcm_field_range_info_get id:%d, rv:%d", i, rv);
      return RET_CODE_FAILURE;
    }
    if (range_info.min_val == min && range_info.max_val == max) {
      *range_id = i;
      return RET_CODE_SUCCESS;
    }
  }
  return RET_CODE_FAILURE;
}

ret_code_e
bcm_dnx_sdk_set_ff_trap(int unit, int src_port, int dst_oort)
{
  bcm_rx_trap_config_t trap_config;
  int rv;
  int trap_id;
  int sysport;
  int egr_port;

  rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
  if (rv != BCM_E_NONE) {
    printf("bcm_rx_trap_type_create failed with error :%s \n",
                bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }

  bcm_rx_trap_config_t_init(&trap_config);
  trap_config.flags             = (BCM_RX_TRAP_UPDATE_DEST |
                                   BCM_RX_TRAP_LEARN_DISABLE |
                                   BCM_RX_TRAP_BYPASS_FILTERS);
  // | BCM_RX_TRAP_REPLACE);
  trap_config.dest_port         = dst_oort;
//  trap_config.trap_strength     = 7;
//  trap_config.snoop_strength    = 3;
  rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
  if (rv != BCM_E_NONE) {
    printf("bcm_rx_trap_set ailed with error :%s \n",
                bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }

  BCM_GPORT_LOCAL_SET(sysport, src_port);
  BCM_GPORT_TRAP_SET(egr_port, trap_id, 7, 3);
  rv = bcm_port_force_forward_set(unit, sysport, egr_port, 1);
  if (rv != BCM_E_NONE) {
    printf("bcm_port_force_forward_set failed with error :%s \n",
                bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }

  return rv;
}

ret_code_e
bcm_dnx_sdk_mirror_dest_create(int unit, bcm_gport_t dst_port,
                               bcm_gport_t *mirror_gport_id)
{
  bcm_mirror_destination_t mirror_dest;
  bcm_error_t rv = 0;

  if (mirror_gport_id == NULL) return RET_CODE_ARG_INVALID;

  bcm_mirror_destination_t_init(&mirror_dest);

  mirror_dest.gport = dst_port;
  rv = bcm_mirror_destination_create(unit, &mirror_dest);
  if (rv != BCM_E_NONE) {
    printf(
        "bcm_mirror_destination_create failed with error :%s \n",
        bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }

  *mirror_gport_id = mirror_dest.mirror_dest_id;

  return RET_CODE_SUCCESS;
}

#define BCM_DNX_LOG_SYSLOG_ACTION_RULE_ID   0x10
ret_code_e
bcm_dnx_sdk_snoop_dest_create(int unit, bcm_gport_t dst_port,
                              bcm_gport_t *snoop_gport_id)
{
  bcm_rx_snoop_config_t snoop_config;
  bcm_rx_trap_config_t trap_config;
  bcm_gport_t local_gport_id;
  bcm_error_t rv = 0;
  int snoop_command;
  int trap_id, flags = 0;
  bcm_mirror_destination_t mirror_dest;

  if (snoop_gport_id == NULL)
    return RET_CODE_ARG_INVALID;

//  rv = bcm_rx_snoop_create(unit, flags, &snoop_command);
//  if (rv != BCM_E_NONE) {
//    bcmsdk_trace_infra_hw_error("bcm_rx_snoop_create failed with error :%s \n",
//                                bcm_errmsg(rv));
//    return RET_CODE_FAILURE;
//  }
//
//  bcm_rx_snoop_config_t_init(&snoop_config);
//  snoop_config.flags |= BCM_RX_SNOOP_UPDATE_DEST;
//
//  BCM_GPORT_LOCAL_SET(local_gport_id, 0);
//  snoop_config.dest_port = local_gport_id;
//  snoop_config.size = -1; /* Full packet */
//  snoop_config.probability = 100000; /* 100000 is 100.000% */
//
//  rv = bcm_rx_snoop_set(unit, snoop_command, &snoop_config);
//  if (rv != BCM_E_NONE) {
//    bcmsdk_trace_infra_hw_error("bcm_rx_snoop_set failed with error :%s \n",
//                                bcm_errmsg(rv));
//    return RET_CODE_FAILURE;
//  }
  BCM_GPORT_LOCAL_SET(local_gport_id, 0);

  bcm_mirror_destination_t_init(&mirror_dest);
  mirror_dest.gport = local_gport_id;
  mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP;

  rv = bcm_mirror_destination_create(unit, &mirror_dest);
  if (rv != BCM_E_NONE) {
    printf("Error in bcm_mirror_destination_create.\n");
    return RET_CODE_FAILURE;
  }

  /* Check if snoop was created */
  if (!BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest.mirror_dest_id))
  {
      printf("Error. Could not create snoop.\n");
      return RET_CODE_FAILURE;
  }

//  trap_id = BCM_DNX_LOG_SYSLOG_ACTION_RULE_ID;
  rv = bcm_rx_trap_type_create(unit, /*BCM_RX_TRAP_WITH_ID*/0, bcmRxTrapUserDefine,
                               &trap_id);
  if (rv != BCM_E_NONE) {
    printf(
        "bcm_rx_trap_type_create failed with error :%s \n", bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }

  bcm_rx_trap_config_t_init(&trap_config);
//  trap_config.flags |= BCM_RX_TRAP_REPLACE;
//  trap_config.trap_strength = 0;
  trap_config.snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);
//  trap_config.snoop_strength = 3;

  rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
  if (rv != BCM_E_NONE) {
    printf("bcm_rx_trap_set failed with error :%s \n",
                                bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }

  BCM_GPORT_TRAP_SET(*snoop_gport_id, trap_id, 7, 3);

  return RET_CODE_SUCCESS;
}


static int
bcm_dnx_sdk_pre_copp_fg_create(int unit, bcm_field_group_t *id_p, int pv, int detach)
{
  bcm_field_group_info_t fg_info;
  bcm_field_group_attach_info_t attach_info;
  bcm_field_qualifier_info_create_t qual_info;
  bcm_field_action_info_t action_info;
  bcm_dnx_group_tuple_t _gtuple;
  bcm_dnx_group_tuple_t *gtuple = &_gtuple;
  bcm_field_group_t id = *id_p;
  gtuple->priority = BCM_DNX_SDK_GROUP_PRIORITY_ANY;
  int rv;
  if (!detach) {
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 0;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_actions = 0;
    bcm_field_group_attach_info_t_init(&attach_info);
    QSET_ADD((*fqset), pre_copp_fg_qualify);
    ASET_ADD(faset, pre_copp_action);
    void *dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "PRE_COPP", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info,
                             &id);
    if (rv != BCM_E_NONE) {
      printf("Error (%d) in bcm_field_group_add\n", rv);
      return rv;
    }
    *id_p = id;
  }
  COPP_ATTACH_CASCADE(V4);
  COPP_ATTACH_CASCADE(V6);
  COPP_ATTACH_CASCADE(L2);
  if (!pv) {
    COPP_ATTACH_CASCADE(V4_TAG);
    COPP_ATTACH_CASCADE(V6_TAG);
    COPP_ATTACH_CASCADE(L2_TAG);
  }
  if (detach) {
    rv = bcm_field_group_delete(unit, *id_p);
    if (rv != BCM_E_NONE) {
      printf("Error (%d) in bcm_field_group_remove\n", rv);
      return rv;
    }
  }
  return rv;
}

int
bcm_dnx_sdk_pre_copp_fg(int unit, int detach)
{
  bcm_field_group_info_t fg_info;
  bcm_field_group_attach_info_t attach_info;
  bcm_field_qualifier_info_create_t qual_info;
  bcm_field_action_info_t action_info;
  bcm_field_group_t id;
  bcm_dnx_group_tuple_t _gtuple;
  bcm_dnx_group_tuple_t *gtuple = &_gtuple;
  gtuple->priority = BCM_DNX_SDK_GROUP_PRIORITY_ANY;
  int rv;
  bcm_field_group_info_t_init(&fg_info);
  fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
  fg_info.nof_quals = 0;
  fg_info.stage = bcmFieldStageIngressPMF2;
  fg_info.nof_actions = 0;
  bcm_field_group_attach_info_t_init(&attach_info);
  QSET_ADD((*fqset), in_default_copp_fg_qualify);
  ASET_ADD(faset, pre_copp_action);
  void *dest_char = &(fg_info.name[0]);
  sal_strncpy_s(dest_char, "PRE_COPP", sizeof(fg_info.name));

  rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &id);
  if(rv != BCM_E_NONE)
  {
      printf("Error (%d) in bcm_field_group_add\n", rv);
      return rv;
  }

  ATTACH(BCM_DNX_CONTEXT_INGRESS_V4);
  ATTACH(BCM_DNX_CONTEXT_INGRESS_V6);
  ATTACH(BCM_DNX_CONTEXT_INGRESS_L2);
  return rv;
}

#define assert(c) if (!(c)) {print #c; return -1;}

/* in BCM_DNX_FG_KEY_80 */
static int
bcm_dnx_pmf_tcam_keys_per_bank (int bank_id)
{
  /* OAM_FWD_IDX_FG picks up 80 keys from bank 0 */
  return bank_id ? (BCM_DNX_LARGE_TCAMS > bank_id ? 4096 : 512) : 4016;
}

static int
bcm_dnx_pmf_tcam_bank_add_to_ag (bcm_dnx_tcam_info_t *tcam_info,
    int bank_id, bcm_dnx_tcam_alloc_group_e alloc_group)
{
  int old, ng;
  if (alloc_group == tcam_info->banks[bank_id].alloc_group) {
    tcam_info->banks[bank_id].refcnt++;
    return 0;
  }
  if (BCM_DNX_AG_FREE != tcam_info->banks[bank_id].alloc_group) {
    old = tcam_info->banks[bank_id].alloc_group;
    ng = alloc_group;
    printf("Bank %d added to 2 AGs, old:%d new:%d\n", bank_id, old, ng);
    return -1;
  }
  tcam_info->banks[bank_id].refcnt++;
  tcam_info->banks[bank_id].alloc_group = alloc_group;
  tcam_info->total_keys_per_alloc_group[BCM_DNX_AG_FREE] -=
      bcm_dnx_pmf_tcam_keys_per_bank(bank_id);
  tcam_info->total_keys_per_alloc_group[alloc_group] +=
      bcm_dnx_pmf_tcam_keys_per_bank(bank_id);
  return 0;
}

static int
bcm_dnx_pmf_tcam_bank_remove_from_ag (
    bcm_dnx_tcam_info_t *tcam_info, int bank_id,
    bcm_dnx_tcam_alloc_group_e alloc_group)
{
  assert(0 < tcam_info->banks[bank_id].refcnt);
  assert(alloc_group == tcam_info->banks[bank_id].alloc_group);
  tcam_info->banks[bank_id].refcnt--;
  if (tcam_info->banks[bank_id].refcnt) {
    return 0;
  }
  tcam_info->banks[bank_id].alloc_group = BCM_DNX_AG_FREE;
  tcam_info->total_keys_per_alloc_group[BCM_DNX_AG_FREE] +=
      bcm_dnx_pmf_tcam_keys_per_bank(bank_id);
  tcam_info->total_keys_per_alloc_group[alloc_group] -=
      bcm_dnx_pmf_tcam_keys_per_bank(bank_id);
  return 0;
}

static bcm_dnx_tcam_fg_info_t*
bcm_dnx_pmf_tcam_fg_info_lookup (bcm_dnx_tcam_info_t *tcam_info,
    bcm_field_group_t group_id)
{
  assert(BCM_DNX_MAX_FG_ID > group_id);
  return tcam_info->fgs + group_id;
}

#define memset sal_memset
#define memcpy sal_memcpy

static ret_code_e
bcm_dnx_pmf_tcam_fg_info_init (int unit,
    bcm_dnx_tcam_info_t *tcam_info, bcm_field_group_t group_id,
    bcm_dnx_fg_direction_t direction, bcm_dnx_fg_type_t type)
{
  bcm_dnx_tcam_fg_info_t *tcam_fg_info = bcm_dnx_pmf_tcam_fg_info_lookup(
      tcam_info, group_id);
  memset((void*) tcam_fg_info, 0, sizeof(*tcam_fg_info));
  tcam_fg_info->alloc_group = bcm_dnx_sdk_fgs[type].tcam_alloc_group
      + (BCM_DNX_PMF_EGRESS == direction ? BCM_DNX_AG_MAX : 0);
  return bcm_dnx_sdk_fg_get_key_size(unit, group_id, &tcam_fg_info->fg_key_size);
}

static void
bcm_dnx_pmf_tcam_fg_info_destroy (bcm_dnx_tcam_info_t *tcam_info,
    bcm_field_group_t group_id)
{
  bcm_dnx_tcam_fg_info_t *tcam_fg_info =
      bcm_dnx_pmf_tcam_fg_info_lookup(tcam_info, group_id);
  int old, old_id;
  bcm_field_group_tcam_info_t *fg_tcam_state = &tcam_fg_info->fg_tcam_state;
  bcm_dnx_tcam_alloc_group_e alloc_group = tcam_fg_info->alloc_group;
  for (old = 0; old < fg_tcam_state->nof_tcam_banks; old++) {
    old_id = fg_tcam_state->tcam_bank_ids[old];
    bcm_dnx_pmf_tcam_bank_remove_from_ag(tcam_info, old_id, alloc_group);
  }
//  tcam_fg_info->fg_key_size = BCM_DNX_FG_KEY_DONT_CARE;
//  tcam_fg_info->fg_tcam_state.nof_tcam_banks = 0;
  memset((void*)tcam_fg_info, 0, sizeof(*tcam_fg_info));
}

static int
bcm_dnx_pmf_tcam_fg_info_update (int unit,
    bcm_dnx_tcam_info_t *tcam_info, bcm_field_group_t group_id, int delta_entries)
{
  //return 0;
  bcm_field_group_t fg_id;
  bcm_field_group_info_t fg_info;
  bcm_dnx_tcam_fg_info_t *tcam_fg_info =
      bcm_dnx_pmf_tcam_fg_info_lookup(tcam_info, group_id);
  if (BCM_DNX_FG_KEY_DONT_CARE == tcam_fg_info->fg_key_size) {
    printf("FG ID: %d uninitialized during fg_info_update (%d)\n", group_id, delta_entries);
    return -1;
  }
  bcm_dnx_tcam_alloc_group_e alloc_group = tcam_fg_info->alloc_group;
  int rv;
  bcm_field_group_info_t_init(&fg_info);
  TRY(bcm_field_group_info_get(unit, group_id, &fg_info));
  /* assume sorted list */
  int old, new;
  int old_id, new_id;
  bcm_field_group_tcam_info_t *fg_tcam_state = &tcam_fg_info->fg_tcam_state;
  for (old = 0, new = 0;
      old < fg_tcam_state->nof_tcam_banks
          && new < fg_info.tcam_info.nof_tcam_banks;) {
    old_id = fg_tcam_state->tcam_bank_ids[old];
    new_id = fg_info.tcam_info.tcam_bank_ids[new];
    if (old_id == new_id) {
      old++;
      new++;
    }
    if (old_id < new_id) {
      TRY(bcm_dnx_pmf_tcam_bank_remove_from_ag(tcam_info, old_id, alloc_group));
      old++;
    }
    if (old_id > new_id) {
      TRY(bcm_dnx_pmf_tcam_bank_add_to_ag(tcam_info, new_id, alloc_group));
      new++;
    }
  }
  bcm_dnx_tcam_alloc_group_e alloc_group1= alloc_group;
  for (; old < fg_tcam_state->nof_tcam_banks; old++) {
    old_id = fg_tcam_state->tcam_bank_ids[old];
    TRY(bcm_dnx_pmf_tcam_bank_remove_from_ag(tcam_info, old_id, alloc_group1));
  }
  for (; new < fg_info.tcam_info.nof_tcam_banks; new++) {
    new_id = fg_info.tcam_info.tcam_bank_ids[new];
    TRY(bcm_dnx_pmf_tcam_bank_add_to_ag(tcam_info, new_id, alloc_group1));
  }
  tcam_info->entries_per_alloc_group[alloc_group] += delta_entries;
  tcam_info->keys_per_alloc_group[alloc_group] += delta_entries
      * tcam_fg_info->fg_key_size;
  tcam_fg_info->nof_entries += delta_entries;
  memcpy(fg_tcam_state, &fg_info.tcam_info, sizeof(fg_info.tcam_info));
  return 0;
}

static int
bcm_dnx_pmf_tcam_fg_info_adjust_320 (bcm_dnx_tcam_info_t *tcam_info,
    bcm_dnx_tcam_alloc_group_e alloc_group)
{
  int i;
  int adjust = 0;
  bcm_dnx_tcam_alloc_group_e alloc0, alloc1;
  for (i = 0; i < BCM_FIELD_NUMBER_OF_TCAM_BANKS - 1; i += 2) {
    alloc0 = tcam_info->banks[i].alloc_group;
    alloc1 = tcam_info->banks[i + 1].alloc_group;
    if (alloc0 == alloc1) {
      continue;
    }
    if (BCM_DNX_AG_FREE == alloc0) {
      if (alloc_group != alloc1) {
        /* ith bank is not usable in this AG for wide key,
         *  was counted as free
         */
        adjust +=
            bcm_dnx_pmf_tcam_keys_per_bank(i) / BCM_DNX_FG_KEY_320;
      }
      continue;
    }
    if (BCM_DNX_AG_FREE == alloc1) {
      if (alloc_group != alloc0) {
        /* i+1th bank is not usable in this AG for wide key,
         *  was counted as free
         */
        adjust +=
            bcm_dnx_pmf_tcam_keys_per_bank(i+1) / BCM_DNX_FG_KEY_320;
      }
      continue;
    }
    /* i and i+1 are not free, and not useful for wide keys,
     * bank0 is not fully available, take care. */
    if (alloc_group == alloc0) {
      adjust +=
          bcm_dnx_pmf_tcam_keys_per_bank(i) / BCM_DNX_FG_KEY_320;
    }
    if (alloc_group == alloc1) {
      adjust +=
          bcm_dnx_pmf_tcam_keys_per_bank(i+1) / BCM_DNX_FG_KEY_320;
    }
  }
  return adjust;
}

static int
bcm_dnx_pmf_tcam_fg_info_get_max (bcm_dnx_tcam_info_t *tcam_info,
    bcm_field_group_t group_id, uint32 *entries, uint32 *guaranteed_free,
    uint32 *max_free)
{
  int free_entries_in_free_banks;
  int guaranteed_free_entries_in_used_banks;
  int max_free_entries_in_used_banks;
  int adjust = 0;
  bcm_dnx_tcam_fg_info_t *tcam_fg_info =
      bcm_dnx_pmf_tcam_fg_info_lookup(tcam_info, group_id);
  bcm_dnx_tcam_alloc_group_e alloc_group = tcam_fg_info->alloc_group;
  if (BCM_DNX_FG_KEY_DONT_CARE == tcam_fg_info->fg_key_size) {
    printf("Uninitialized FG ID: %d\n", group_id);
    return RET_CODE_NOT_FOUND;
  }
  free_entries_in_free_banks =
      tcam_info->total_keys_per_alloc_group[BCM_DNX_AG_FREE]
          / tcam_fg_info->fg_key_size;
  guaranteed_free_entries_in_used_banks =
      tcam_info->entries_per_alloc_group[alloc_group]
          * tcam_fg_info->fg_key_size;
  if (tcam_info->keys_per_alloc_group[alloc_group]
      > guaranteed_free_entries_in_used_banks) {
    guaranteed_free_entries_in_used_banks =
        tcam_info->keys_per_alloc_group[alloc_group];
  }
  guaranteed_free_entries_in_used_banks =
      tcam_info->total_keys_per_alloc_group[alloc_group]
          - guaranteed_free_entries_in_used_banks;
  if (0 < guaranteed_free_entries_in_used_banks) {
    guaranteed_free_entries_in_used_banks /= tcam_fg_info->fg_key_size;
  } else {
    guaranteed_free_entries_in_used_banks = 0;
  }
  max_free_entries_in_used_banks =
      (tcam_info->total_keys_per_alloc_group[alloc_group]
          - tcam_info->keys_per_alloc_group[alloc_group])
          / tcam_fg_info->fg_key_size;
  if (BCM_DNX_FG_KEY_320 == tcam_fg_info->fg_key_size) {
    adjust = bcm_dnx_pmf_tcam_fg_info_adjust_320(tcam_info, alloc_group);
  }
  *entries = tcam_fg_info->nof_entries;
  free_entries_in_free_banks -= adjust;
  if (0 < free_entries_in_free_banks + guaranteed_free_entries_in_used_banks) {
    *guaranteed_free =
        free_entries_in_free_banks + guaranteed_free_entries_in_used_banks;
  } else {
    *guaranteed_free = 0;
  }
  if (0 < free_entries_in_free_banks + max_free_entries_in_used_banks) {
    *max_free = free_entries_in_free_banks + max_free_entries_in_used_banks;
  } else {
    *max_free = 0;
  }
  return RET_CODE_SUCCESS;
}

static int
bcm_dnx_pmf_tcam_fg_info_get (bcm_dnx_tcam_info_t *tcam_info,
    bcm_field_group_t group_id, uint32 *entries, uint32 *guaranteed_free,
    uint32 *total)
{
  int free_entries_in_free_banks;
  int guaranteed_free_entries_in_used_banks;
  int adjust = 0;
  bcm_dnx_tcam_fg_info_t *tcam_fg_info =
      bcm_dnx_pmf_tcam_fg_info_lookup(tcam_info, group_id);
  bcm_dnx_tcam_alloc_group_e alloc_group = tcam_fg_info->alloc_group;
  if (BCM_DNX_FG_KEY_DONT_CARE == tcam_fg_info->fg_key_size) {
    printf("Uninitialized FG ID: %d\n", group_id);
    return RET_CODE_NOT_FOUND;
  }
  free_entries_in_free_banks =
      tcam_info->total_keys_per_alloc_group[BCM_DNX_AG_FREE]
          / tcam_fg_info->fg_key_size;
  guaranteed_free_entries_in_used_banks =
      tcam_info->entries_per_alloc_group[alloc_group]
          * tcam_fg_info->fg_key_size;
  if (tcam_info->keys_per_alloc_group[alloc_group]
      > guaranteed_free_entries_in_used_banks) {
    guaranteed_free_entries_in_used_banks =
        tcam_info->keys_per_alloc_group[alloc_group];
  }
  guaranteed_free_entries_in_used_banks =
      tcam_info->total_keys_per_alloc_group[alloc_group]
          - guaranteed_free_entries_in_used_banks;
  if (0 < guaranteed_free_entries_in_used_banks) {
    guaranteed_free_entries_in_used_banks /= tcam_fg_info->fg_key_size;
  } else {
    guaranteed_free_entries_in_used_banks = 0;
  }
  if (BCM_DNX_FG_KEY_320 == tcam_fg_info->fg_key_size) {
    adjust = bcm_dnx_pmf_tcam_fg_info_adjust_320(tcam_info, alloc_group);
  }
  *entries = tcam_fg_info->nof_entries;
  free_entries_in_free_banks -= adjust;
  if (0 < free_entries_in_free_banks + guaranteed_free_entries_in_used_banks) {
    *guaranteed_free =
        free_entries_in_free_banks + guaranteed_free_entries_in_used_banks;
  } else {
    *guaranteed_free = 0;
  }
  *total =
      (tcam_info->total_keys_per_alloc_group[alloc_group]
          + tcam_info->total_keys_per_alloc_group[BCM_DNX_AG_FREE])
       / tcam_fg_info->fg_key_size
       - adjust;
  return RET_CODE_SUCCESS;
}

static void
bcm_dnx_pmf_tcam_info_init (bcm_dnx_tcam_info_t *tcam_info)
{
  int i;
  memset(tcam_info, 0 , sizeof(*tcam_info));
  for (i = 0; i < BCM_FIELD_NUMBER_OF_TCAM_BANKS; i ++) {
    tcam_info->banks[i].alloc_group = BCM_DNX_AG_FREE;
    tcam_info->total_keys_per_alloc_group[BCM_DNX_AG_FREE] +=
          bcm_dnx_pmf_tcam_keys_per_bank(i);
  }
 /* SDK is using TCAM bank 1 for FG 'WA_OAM__stat_lm' in stage IPMF3
  * No more true as we need that key space in IPMF3 default context
  */
//  bcm_dnx_pmf_tcam_bank_add_to_ag(tcam_info, 1, BCM_DNX_AG_DONT_CARE);
}
