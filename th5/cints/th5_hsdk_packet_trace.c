cint_reset();

int unit = 0;
bcm_error_t rv = BCM_E_NONE;
bcm_port_t ingress_port;

/*
 * Configures the port in loopback mode
 */
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_EDB));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

  /* Enable IPv4 on ingress port */
  print bcm_port_control_set(unit, ingress_port, bcmPortControlIP4, TRUE);
  return BCM_E_NONE;
}

struct pkttrace_enum_map_t {
  char *name;
  int val;
};

pkttrace_enum_map_t field_names[] =
{
  {"MY_STATION_HIT", BCM_PKTIO_TRACE_MY_STATION_HIT},
    {"FWD_VLAN", BCM_PKTIO_TRACE_FWD_VLAN},
    {"FP_AND_PBM_ACTION_VALID", BCM_PKTIO_TRACE_FP_AND_PBM_ACTION_VALID},
    {"FP_REPLACE_PBM_ACTION_VALID", BCM_PKTIO_TRACE_FP_REPLACE_PBM_ACTION_VALID},
    {"FP_OR_PBM_ACTION_VALID", BCM_PKTIO_TRACE_FP_OR_PBM_ACTION_VALID},
    {"INNER_PAYLOAD_TAG_STATUS", BCM_PKTIO_TRACE_INNER_PAYLOAD_TAG_STATUS},
    {"POST_VXLT_TAG_STATUS", BCM_PKTIO_TRACE_POST_VXLT_TAG_STATUS},
    {"INCOMING_TAG_STATUS", BCM_PKTIO_TRACE_INCOMING_TAG_STATUS},
    {"FORWARDING_FIELD", BCM_PKTIO_TRACE_FORWARDING_FIELD},
    {"FORWARDING_TYPE", BCM_PKTIO_TRACE_FORWARDING_TYPE},
    {"EGRESS_MASK_TABLE_INDEX", BCM_PKTIO_TRACE_EGRESS_MASK_TABLE_INDEX},
    {"EGRESS_BLOCK_MASK_INDEX", BCM_PKTIO_TRACE_EGRESS_BLOCK_MASK_INDEX},
    {"VLAN_MASK_INDEX", BCM_PKTIO_TRACE_VLAN_MASK_INDEX},
    {"MAC_BLOCK_INDEX", BCM_PKTIO_TRACE_MAC_BLOCK_INDEX},
    {"NONUC_TRUNK_BLOCK_MASK_INDEX", BCM_PKTIO_TRACE_NONUC_TRUNK_BLOCK_MASK_INDEX},
    {"HASH_VALUE_HG", BCM_PKTIO_TRACE_HASH_VALUE_HG},
    {"HG_OFFSET", BCM_PKTIO_TRACE_HG_OFFSET},
    {"HG_PORT", BCM_PKTIO_TRACE_HG_PORT},
    {"HG_TRUNK_ID", BCM_PKTIO_TRACE_HG_TRUNK_ID},
    {"HG_TRUNK_RESOLUTION_DONE", BCM_PKTIO_TRACE_HG_TRUNK_RESOLUTION_DONE},
    {"DGPP", BCM_PKTIO_TRACE_DGPP},
    {"NHOP", BCM_PKTIO_TRACE_NHOP},
    {"LAG_OFFSET", BCM_PKTIO_TRACE_LAG_OFFSET},
    {"HASH_VALUE_LAG", BCM_PKTIO_TRACE_HASH_VALUE_LAG},
    {"LAG_ID", BCM_PKTIO_TRACE_LAG_ID},
    {"LAG_RESOLUTION_DONE", BCM_PKTIO_TRACE_LAG_RESOLUTION_DONE},
    {"RESOLVED_NETWORK_DVP", BCM_PKTIO_TRACE_RESOLVED_NETWORK_DVP},
    {"NETWORK_VP_LAG_VALID", BCM_PKTIO_TRACE_NETWORK_VP_LAG_VALID},
    {"ECMP_HASH_VALUE2", BCM_PKTIO_TRACE_ECMP_HASH_VALUE2},
    {"ECMP_OFFSET2", BCM_PKTIO_TRACE_ECMP_OFFSET2},
    {"ECMP_GROUP2", BCM_PKTIO_TRACE_ECMP_GROUP2},
    {"ECMP_RESOLUTION_DONE2", BCM_PKTIO_TRACE_ECMP_RESOLUTION_DONE2},
    {"ECMP_HASH_VALUE1", BCM_PKTIO_TRACE_ECMP_HASH_VALUE1},
    {"ECMP_OFFSET1", BCM_PKTIO_TRACE_ECMP_OFFSET1},
    {"ECMP_GROUP1", BCM_PKTIO_TRACE_ECMP_GROUP1},
    {"ECMP_RESOLUTION_DONE1", BCM_PKTIO_TRACE_ECMP_RESOLUTION_DONE1},
    {"FWD_DESTINATION_FIELD", BCM_PKTIO_TRACE_FWD_DESTINATION_FIELD},
    {"FWD_DESTINATION_TYPE", BCM_PKTIO_TRACE_FWD_DESTINATION_TYPE},
    {"PKT_RESOLUTION_VECTOR", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR},
    {"VXLT_HIT", BCM_PKTIO_TRACE_VXLT_HIT},
    {"VALID_VLAN_ID", BCM_PKTIO_TRACE_VALID_VLAN_ID},
    {"INGRESS_STG_STATE", BCM_PKTIO_TRACE_INGRESS_STG_STATE},
    {"L2_SRC_HIT", BCM_PKTIO_TRACE_L2_SRC_HIT},
    {"L2_SRC_STATIC", BCM_PKTIO_TRACE_L2_SRC_STATIC},
    {"L2_DST_HIT", BCM_PKTIO_TRACE_L2_DST_HIT},
    {"L2_USER_ENTRY_HIT", BCM_PKTIO_TRACE_L2_USER_ENTRY_HIT},
    {"L3_ENTRY_SOURCE_HIT", BCM_PKTIO_TRACE_L3_ENTRY_SOURCE_HIT},
    {"L3_ENTRY_DESTINATION_HIT", BCM_PKTIO_TRACE_L3_ENTRY_DESTINATION_HIT},
    {"LPM_HIT", BCM_PKTIO_TRACE_LPM_HIT},
    {"UNRESOLVED_SA", BCM_PKTIO_TRACE_UNRESOLVED_SA},
    {"DOS_ATTACK", BCM_PKTIO_TRACE_DOS_ATTACK},
    {"L3_TUNNEL_HIT", BCM_PKTIO_TRACE_L3_TUNNEL_HIT},
    {"MPLS_ENTRY_TABLE_LOOKUP_0_HIT", BCM_PKTIO_TRACE_MPLS_ENTRY_TABLE_LOOKUP_0_HIT},
    {"MPLS_ENTRY_TABLE_LOOKUP_1_HIT", BCM_PKTIO_TRACE_MPLS_ENTRY_TABLE_LOOKUP_1_HIT},
    {"MPLS_BOS_TERMINATED", BCM_PKTIO_TRACE_MPLS_BOS_TERMINATED},
    {"SRC_TBL_HALF_ENTRY_NUM", BCM_PKTIO_TRACE_SRC_TBL_HALF_ENTRY_NUM},
    {"SRC_TBL_INDEX", BCM_PKTIO_TRACE_SRC_TBL_INDEX},
    {"TBL_HALF_ENTRY_NUM", BCM_PKTIO_TRACE_TBL_HALF_ENTRY_NUM},
    {"TBL_INDEX", BCM_PKTIO_TRACE_TBL_INDEX},
    {"DEF64_SRC_PUBLIC_HIT", BCM_PKTIO_TRACE_DEF64_SRC_PUBLIC_HIT},
    {"DEF64_SRC_PRIVATE_HIT", BCM_PKTIO_TRACE_DEF64_SRC_PRIVATE_HIT},
    {"DEF64_PUBLIC_HIT", BCM_PKTIO_TRACE_DEF64_PUBLIC_HIT},
    {"DEF64_PRIVATE_HIT", BCM_PKTIO_TRACE_DEF64_PRIVATE_HIT},
    {"DEF128_SRC_PUBLIC_HIT", BCM_PKTIO_TRACE_DEF128_SRC_PUBLIC_HIT},
    {"DEF128_SRC_PRIVATE_HIT", BCM_PKTIO_TRACE_DEF128_SRC_PRIVATE_HIT},
    {"DEF128_PUBLIC_HIT", BCM_PKTIO_TRACE_DEF128_PUBLIC_HIT},
    {"DEF128_PRIVATE_HIT", BCM_PKTIO_TRACE_DEF128_PRIVATE_HIT},
    {"L3_LKUP2_KEY_TYPE", BCM_PKTIO_TRACE_L3_LKUP2_KEY_TYPE},
    {"L3_LKUP1_KEY_TYPE", BCM_PKTIO_TRACE_L3_LKUP1_KEY_TYPE},
    {"SRC_L3_ENTRY_BITMAP", BCM_PKTIO_TRACE_SRC_L3_ENTRY_BITMAP},
    {"SRC_L3_BUCKET_INDEX", BCM_PKTIO_TRACE_SRC_L3_BUCKET_INDEX},
    {"SRC_PRIVATE_HIT", BCM_PKTIO_TRACE_SRC_PRIVATE_HIT},
    {"SRC_PUBLIC_HIT", BCM_PKTIO_TRACE_SRC_PUBLIC_HIT},
    {"DST_L3_ENTRY_BITMAP", BCM_PKTIO_TRACE_DST_L3_ENTRY_BITMAP},
    {"DST_L3_BUCKET_INDEX", BCM_PKTIO_TRACE_DST_L3_BUCKET_INDEX},
    {"DST_PRIVATE_HIT", BCM_PKTIO_TRACE_DST_PRIVATE_HIT},
    {"DST_PUBLIC_HIT", BCM_PKTIO_TRACE_DST_PUBLIC_HIT},
    {"L2LU_SRC_INDEX", BCM_PKTIO_TRACE_L2LU_SRC_INDEX},
    {"L2LU_SRC_KEY_TYPE", BCM_PKTIO_TRACE_L2LU_SRC_KEY_TYPE},
    {"L2LU_SRC_HIT", BCM_PKTIO_TRACE_L2LU_SRC_HIT},
    {"L2LU_DST_IDX", BCM_PKTIO_TRACE_L2LU_DST_IDX},
    {"L2LU_DST_KEY_TYPE", BCM_PKTIO_TRACE_L2LU_DST_KEY_TYPE},
    {"L2LU_DST_HIT", BCM_PKTIO_TRACE_L2LU_DST_HIT},
    {"MPLS_ENTRY_INDEX_2", BCM_PKTIO_TRACE_MPLS_ENTRY_INDEX_2},
    {"MPLS_ENTRY_KEY_TYPE_2", BCM_PKTIO_TRACE_MPLS_ENTRY_KEY_TYPE_2},
    {"MPLS_ENTRY_INDEX_1", BCM_PKTIO_TRACE_MPLS_ENTRY_INDEX_1},
    {"MPLS_ENTRY_KEY_TYPE_1", BCM_PKTIO_TRACE_MPLS_ENTRY_KEY_TYPE_1},
    {"VT_INDEX_2", BCM_PKTIO_TRACE_VT_INDEX_2},
    {"VT_INDEX_1", BCM_PKTIO_TRACE_VT_INDEX_1},
    {"MPLS_ENTRY_HIT_1", BCM_PKTIO_TRACE_MPLS_ENTRY_HIT_1},
    {"MPLS_ENTRY_HIT_2", BCM_PKTIO_TRACE_MPLS_ENTRY_HIT_2},
    {"VT_KEY_TYPE_2", BCM_PKTIO_TRACE_VT_KEY_TYPE_2},
    {"VT_HIT_2", BCM_PKTIO_TRACE_VT_HIT_2},
    {"VT_KEY_TYPE_1", BCM_PKTIO_TRACE_VT_KEY_TYPE_1},
    {"VT_HIT_1", BCM_PKTIO_TRACE_VT_HIT_1},
    {"MY_STATION_INDEX", BCM_PKTIO_TRACE_MY_STATION_INDEX},
    {"ENTROPY", BCM_PKTIO_TRACE_ENTROPY},
    {"HIT_BIT_INDEX", BCM_PKTIO_TRACE_HIT_BIT_INDEX},
    {"HIT_BIT_TABLE", BCM_PKTIO_TRACE_HIT_BIT_TABLE},
    {"SUB_DB_PRIORITY", BCM_PKTIO_TRACE_SUB_DB_PRIORITY},
    {"L3_ENTRY_DST_HIT", BCM_PKTIO_TRACE_L3_ENTRY_DST_HIT},
    {"L3_ENTRY_SRC_HIT", BCM_PKTIO_TRACE_L3_ENTRY_SRC_HIT},
    {"L3_TUNNEL_HIT_1", BCM_PKTIO_TRACE_L3_TUNNEL_HIT_1},
    {"L3_TUNNEL_INDEX_1", BCM_PKTIO_TRACE_L3_TUNNEL_INDEX_1},
    {"L3_TUNNEL_KEY_TYPE_1", BCM_PKTIO_TRACE_L3_TUNNEL_KEY_TYPE_1},
    {"COMP_DST_HIT", BCM_PKTIO_TRACE_COMP_DST_HIT},
    {"COMP_DST_HIT_INDEX", BCM_PKTIO_TRACE_COMP_DST_HIT_INDEX},
    {"COMP_DST_HIT_TABLE", BCM_PKTIO_TRACE_COMP_DST_HIT_TABLE},
    {"COMP_DST_SUB_DB_PRIORITY", BCM_PKTIO_TRACE_COMP_DST_SUB_DB_PRIORITY},
    {"COMP_SRC_HIT", BCM_PKTIO_TRACE_COMP_SRC_HIT},
    {"COMP_SRC_HIT_INDEX", BCM_PKTIO_TRACE_COMP_SRC_HIT_INDEX},
    {"COMP_SRC_HIT_TABLE", BCM_PKTIO_TRACE_COMP_SRC_HIT_TABLE},
    {"COMP_SRC_SUB_DB_PRIORITY", BCM_PKTIO_TRACE_COMP_SRC_SUB_DB_PRIORITY},
    {"LPM_DST_HIT", BCM_PKTIO_TRACE_LPM_DST_HIT},
    {"LPM_DST_HIT_INDEX", BCM_PKTIO_TRACE_LPM_DST_HIT_INDEX},
    {"LPM_DST_HIT_TABLE", BCM_PKTIO_TRACE_LPM_DST_HIT_TABLE},
    {"LPM_DST_SUB_DB_PRIORITY", BCM_PKTIO_TRACE_LPM_DST_SUB_DB_PRIORITY},
    {"LPM_SRC_HIT", BCM_PKTIO_TRACE_LPM_SRC_HIT},
    {"LPM_SRC_HIT_INDEX", BCM_PKTIO_TRACE_LPM_SRC_HIT_INDEX},
    {"LPM_SRC_HIT_TABLE", BCM_PKTIO_TRACE_LPM_SRC_HIT_TABLE},
    {"LPM_SRC_SUB_DB_PRIORITY", BCM_PKTIO_TRACE_LPM_SRC_SUB_DB_PRIORITY},
    {"L3_TUNNEL_TCAM_HIT", BCM_PKTIO_TRACE_L3_TUNNEL_TCAM_HIT},
    {"L3_TUNNEL_TCAM_INDEX", BCM_PKTIO_TRACE_L3_TUNNEL_TCAM_INDEX},
    {"DVP", BCM_PKTIO_TRACE_DVP},
    {"SVP", BCM_PKTIO_TRACE_SVP},
    {"INNER_L2_OUTER_TAGGED", BCM_PKTIO_TRACE_INNER_L2_OUTER_TAGGED},
    {"O_NEXT_HOP", BCM_PKTIO_TRACE_O_NEXT_HOP},
    {"VFI", BCM_PKTIO_TRACE_VFI},
    {"MY_STATION_2_HIT", BCM_PKTIO_TRACE_MY_STATION_2_HIT},
    {"MY_STATION_2_INDEX", BCM_PKTIO_TRACE_MY_STATION_2_INDEX},
    {"LAG_INDICATOR", BCM_PKTIO_TRACE_LAG_INDICATOR},
    {"MY_PREFIX_TCAM_HIT", BCM_PKTIO_TRACE_MY_PREFIX_TCAM_HIT},
    {"L3_TUNNEL_TCAM_KEY_TYPE", BCM_PKTIO_TRACE_L3_TUNNEL_TCAM_KEY_TYPE},
    {"L3_TUNNEL_HASH_KEY_TYPE_1", BCM_PKTIO_TRACE_L3_TUNNEL_HASH_KEY_TYPE_1},
    {"fid count", BCM_PKTIO_TRACE_FID_COUNT}
};

pkttrace_enum_map_t fwd_type_names[] =
{
  {"VID", BCM_PKTIO_TRACE_FORWARDING_T_VID},
    {"FID", BCM_PKTIO_TRACE_FORWARDING_T_FID},
    {"RESERVED_COUNTER", 2},
    {"RESERVED_COUNTER", 3},
    {"L3_MPLS", BCM_PKTIO_TRACE_FORWARDING_T_L3_MPLS},
    {"VRF", BCM_PKTIO_TRACE_FORWARDING_T_VRF},
    {"RESERVED_COUNTER", 6},
    {"MPLS", BCM_PKTIO_TRACE_FORWARDING_T_MPLS},
};

pkttrace_enum_map_t fwd_dest_type_names[] =
{
  {"DGLP", BCM_PKTIO_TRACE_FWD_DESTINATION_T_DGLP},
    {"NHI", BCM_PKTIO_TRACE_FWD_DESTINATION_T_NHI},
    {"ECMP", BCM_PKTIO_TRACE_FWD_DESTINATION_T_ECMP},
    {"ECMP_MEMBER", BCM_PKTIO_TRACE_FWD_DESTINATION_T_ECMP_MEMBER},
    {"IPMC", BCM_PKTIO_TRACE_FWD_DESTINATION_T_IPMC},
    {"L2MC", BCM_PKTIO_TRACE_FWD_DESTINATION_T_L2MC},
    {"VLAN_FLOOD", BCM_PKTIO_TRACE_FWD_DESTINATION_T_VLAN_FLOOD},
};

pkttrace_enum_map_t pkt_resolution_names[] =
{
  {"UNKNOWN_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_PKT},
    {"CONTROL_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_CONTROL_PKT},
    {"OAM_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_OAM_PKT},
    {"BFD_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_BFD_PKT},
    {"BPDU_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_BPDU_PKT},
    {"RESERVED_COUNTER", 5},
    {"PKT_IS_1588", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_PKT_IS_1588},
    {"RESERVED_COUNTER", 7},
    {"KNOWN_L2UC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2UC_PKT},
    {"UNKNOWN_L2UC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L2UC_PKT},
    {"KNOWN_L2MC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2MC_PKT},
    {"UNKNOWN_L2MC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L2MC_PKT},
    {"L2BC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_L2BC_PKT},
    {"RESERVED_COUNTER", 13},
    {"RESERVED_COUNTER", 14},
    {"RESERVED_COUNTER", 15},
    {"KNOWN_L3UC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L3UC_PKT},
    {"UNKNOWN_L3UC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L3UC_PKT},
    {"KNOWN_IPMC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_IPMC_PKT},
    {"UNKNOWN_IPMC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_IPMC_PKT},
    {"RESERVED_COUNTER", 20},
    {"RESERVED_COUNTER", 21},
    {"RESERVED_COUNTER", 22},
    {"RESERVED_COUNTER", 23},
    {"KNOWN_MPLS_L2_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_L2_PKT},
    {"UNKNOWN_MPLS_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_MPLS_PKT},
    {"KNOWN_MPLS_L3_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_L3_PKT},
    {"RESERVED_COUNTER", 27},
    {"KNOWN_MPLS_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_PKT},
    {"RESERVED_COUNTER", 29},
    {"RESERVED_COUNTER", 30},
    {"RESERVED_COUNTER", 31},
    {"KNOWN_MIM_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MIM_PKT},
    {"UNKNOWN_MIM_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_MIM_PKT},
    {"RESERVED_COUNTER", 34},
    {"RESERVED_COUNTER", 35},
    {"RESERVED_COUNTER", 36},
    {"RESERVED_COUNTER", 37},
    {"RESERVED_COUNTER", 38},
    {"RESERVED_COUNTER", 39},
    {"KNOWN_TRILL_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_TRILL_PKT},
    {"UNKNOWN_TRILL_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_TRILL_PKT},
    {"RESERVED_COUNTER", 42},
    {"RESERVED_COUNTER", 43},
    {"RESERVED_COUNTER", 44},
    {"RESERVED_COUNTER", 45},
    {"RESERVED_COUNTER", 46},
    {"RESERVED_COUNTER", 47},
    {"KNOWN_NIV_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_NIV_PKT},
    {"UNKNOWN_NIV_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_NIV_PKT},
    {"KNOWN_L2GRE_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2GRE_PKT},
    {"KNOWN_VXLAN_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_VXLAN_PKT},
};

pkttrace_enum_map_t drop_reason_names[] =
{
  {"BFD_TERMINATED_DROP", BCM_PKTIO_TRACE_DROP_REASON_BFD_TERMINATED_DROP},
    {"BPDU", BCM_PKTIO_TRACE_DROP_REASON_BPDU},
    {"CFI_OR_L3DISABLE", BCM_PKTIO_TRACE_DROP_REASON_CFI_OR_L3DISABLE},
    {"CML", BCM_PKTIO_TRACE_DROP_REASON_CML},
    {"COMPOSITE_ERROR", BCM_PKTIO_TRACE_DROP_REASON_COMPOSITE_ERROR},
    {"CONTROL_FRAME", BCM_PKTIO_TRACE_DROP_REASON_CONTROL_FRAME},
    {"IPV4_PROTOCOL_ERROR", BCM_PKTIO_TRACE_DROP_REASON_IPV4_PROTOCOL_ERROR},
    {"IPV6_PROTOCOL_ERROR", BCM_PKTIO_TRACE_DROP_REASON_IPV6_PROTOCOL_ERROR},
    {"L3_DOS_ATTACK", BCM_PKTIO_TRACE_DROP_REASON_L3_DOS_ATTACK},
    {"L4_DOS_ATTACK", BCM_PKTIO_TRACE_DROP_REASON_L4_DOS_ATTACK},
    {"LAG_FAIL_LOOPBACK", BCM_PKTIO_TRACE_DROP_REASON_LAG_FAIL_LOOPBACK},
    {"MACSA_EQUALS_DA", BCM_PKTIO_TRACE_DROP_REASON_MACSA_EQUALS_DA},
    {"IPMC_PROC", BCM_PKTIO_TRACE_DROP_REASON_IPMC_PROC},
    {"L2DST_DISCARD", BCM_PKTIO_TRACE_DROP_REASON_L2DST_DISCARD},
    {"L2SRC_DISCARD", BCM_PKTIO_TRACE_DROP_REASON_L2SRC_DISCARD},
    {"L2SRC_STATIC_MOVE", BCM_PKTIO_TRACE_DROP_REASON_L2SRC_STATIC_MOVE},
    {"L3DST_DISCARD", BCM_PKTIO_TRACE_DROP_REASON_L3DST_DISCARD},
    {"L3SRC_DISCARD", BCM_PKTIO_TRACE_DROP_REASON_L3SRC_DISCARD},
    {"L3_DST_LOOKUP_MISS", BCM_PKTIO_TRACE_DROP_REASON_L3_DST_LOOKUP_MISS},
    {"L3_HDR_ERROR", BCM_PKTIO_TRACE_DROP_REASON_L3_HDR_ERROR},
    {"L3_TTL_ERROR", BCM_PKTIO_TRACE_DROP_REASON_L3_TTL_ERROR},
    {"MACSA0", BCM_PKTIO_TRACE_DROP_REASON_MACSA0},
    {"MPLS_STAGE", BCM_PKTIO_TRACE_DROP_REASON_MPLS_STAGE},
    {"MULTICAST_INDEX_ERROR", BCM_PKTIO_TRACE_DROP_REASON_MULTICAST_INDEX_ERROR},
    {"MY_STATION", BCM_PKTIO_TRACE_DROP_REASON_MY_STATION},
    {"PFM", BCM_PKTIO_TRACE_DROP_REASON_PFM},
    {"PROTCOL_PKT", BCM_PKTIO_TRACE_DROP_REASON_PROTCOL_PKT},
    {"PVLAN_VP_EFILTER", BCM_PKTIO_TRACE_DROP_REASON_PVLAN_VP_EFILTER},
    {"ECMP_RESOLUTION_ERROR", BCM_PKTIO_TRACE_DROP_REASON_ECMP_RESOLUTION_ERROR},
    {"FP_CHANGE_L2_FIELDS_NO_REDIRECT_DROP", BCM_PKTIO_TRACE_DROP_REASON_FP_CHANGE_L2_FIELDS_NO_REDIRECT_DROP},
    {"IFP_DROP", BCM_PKTIO_TRACE_DROP_REASON_IFP_DROP},
    {"NEXT_HOP_DROP", BCM_PKTIO_TRACE_DROP_REASON_NEXT_HOP_DROP},
    {"PROTECTION_DATA_DROP", BCM_PKTIO_TRACE_DROP_REASON_PROTECTION_DATA_DROP},
    {"TUNNEL_ERROR", BCM_PKTIO_TRACE_DROP_REASON_TUNNEL_ERROR},
    {"SPANNING_TREE_STATE", BCM_PKTIO_TRACE_DROP_REASON_SPANNING_TREE_STATE},
    {"SRC_ROUTE", BCM_PKTIO_TRACE_DROP_REASON_SRC_ROUTE},
    {"TAG_UNTAG_DISCARD", BCM_PKTIO_TRACE_DROP_REASON_TAG_UNTAG_DISCARD},
    {"TIME_SYNC_PKT", BCM_PKTIO_TRACE_DROP_REASON_TIME_SYNC_PKT},
    {"TUNNEL_DECAP_ECN", BCM_PKTIO_TRACE_DROP_REASON_TUNNEL_DECAP_ECN},
    {"MPLS_GAL_LABEL", BCM_PKTIO_TRACE_DROP_REASON_MPLS_GAL_LABEL},
    {"MPLS_INVALID_ACTION", BCM_PKTIO_TRACE_DROP_REASON_MPLS_INVALID_ACTION},
    {"MPLS_INVALID_CW", BCM_PKTIO_TRACE_DROP_REASON_MPLS_INVALID_CW},
    {"MPLS_INVALID_PAYLOAD", BCM_PKTIO_TRACE_DROP_REASON_MPLS_INVALID_PAYLOAD},
    {"MPLS_LABEL_MISS", BCM_PKTIO_TRACE_DROP_REASON_MPLS_LABEL_MISS},
    {"MPLS_TTL_CHECK_FAIL", BCM_PKTIO_TRACE_DROP_REASON_MPLS_TTL_CHECK_FAIL},
    {"VFP", BCM_PKTIO_TRACE_DROP_REASON_VFP},
    {"VLAN_CC_OR_PBT", BCM_PKTIO_TRACE_DROP_REASON_VLAN_CC_OR_PBT},
    {"ENIFILTER", BCM_PKTIO_TRACE_DROP_REASON_ENIFILTER},
    {"INVALID_TPID", BCM_PKTIO_TRACE_DROP_REASON_INVALID_TPID},
    {"INVALID_VLAN", BCM_PKTIO_TRACE_DROP_REASON_INVALID_VLAN},
    {"PVLAN_VID_MISMATCH", BCM_PKTIO_TRACE_DROP_REASON_PVLAN_VID_MISMATCH},
    {"VXLT_MISS", BCM_PKTIO_TRACE_DROP_REASON_VXLT_MISS},
    {"HIGIG_MH_TYPE1", BCM_PKTIO_TRACE_DROP_REASON_HIGIG_MH_TYPE1},
    {"DISC_STAGE", BCM_PKTIO_TRACE_DROP_REASON_DISC_STAGE},
    {"SW1_INVALID_VLAN", BCM_PKTIO_TRACE_DROP_REASON_SW1_INVALID_VLAN},
    {"HIGIG_HDR_ERROR", BCM_PKTIO_TRACE_DROP_REASON_HIGIG_HDR_ERROR},
    {"LAG_FAILOVER", BCM_PKTIO_TRACE_DROP_REASON_LAG_FAILOVER},
    {"CLASS_BASED_SM", BCM_PKTIO_TRACE_DROP_REASON_CLASS_BASED_SM},
    {"BAD_UDP_CHECKSUM", BCM_PKTIO_TRACE_DROP_REASON_BAD_UDP_CHECKSUM},
    {"NIV_FORWARDING", BCM_PKTIO_TRACE_DROP_REASON_NIV_FORWARDING},
    {"NIV_RPF_CHECK_FAIL", BCM_PKTIO_TRACE_DROP_REASON_NIV_RPF_CHECK_FAIL},
    {"TRILL_HEADER_VERSION_NONZERO", BCM_PKTIO_TRACE_DROP_REASON_TRILL_HEADER_VERSION_NONZERO},
    {"TRILL_ADJACENCY_CHECK_FAIL", BCM_PKTIO_TRACE_DROP_REASON_TRILL_ADJACENCY_CHECK_FAIL},
    {"TRILL_RBRIDGE_LOOKUP_MISS", BCM_PKTIO_TRACE_DROP_REASON_TRILL_RBRIDGE_LOOKUP_MISS},
    {"TRILL_UC_HDR_MC_MACDA", BCM_PKTIO_TRACE_DROP_REASON_TRILL_UC_HDR_MC_MACDA},
    {"TRILL_SLOWPATH", BCM_PKTIO_TRACE_DROP_REASON_TRILL_SLOWPATH},
    {"CORE_IS_IS_PKT", BCM_PKTIO_TRACE_DROP_REASON_CORE_IS_IS_PKT},
    {"TRILL_RPF_CHECK_FAIL", BCM_PKTIO_TRACE_DROP_REASON_TRILL_RPF_CHECK_FAIL},
    {"TRILL_PKT_WITH_SNAP_ENCAP", BCM_PKTIO_TRACE_DROP_REASON_TRILL_PKT_WITH_SNAP_ENCAP},
    {"TRILL_ING_RBRIDGE_EQ_EGR_RBRIDGE", BCM_PKTIO_TRACE_DROP_REASON_TRILL_ING_RBRIDGE_EQ_EGR_RBRIDGE},
    {"TRILL_HOPCOUNT_CHECK_FAIL", BCM_PKTIO_TRACE_DROP_REASON_TRILL_HOPCOUNT_CHECK_FAIL},
    {"INT_DATAPLANE_PROBE", BCM_PKTIO_TRACE_DROP_REASON_INT_DATAPLANE_PROBE},
    {"INT_ERROR", BCM_PKTIO_TRACE_DROP_REASON_INT_ERROR},
    {"INVALID_GSH_DROP", BCM_PKTIO_TRACE_DROP_REASON_INVALID_GSH_DROP},
    {"INVALID_NON_GSH_DROP", BCM_PKTIO_TRACE_DROP_REASON_INVALID_NON_GSH_DROP},
    {"SER_DROP", BCM_PKTIO_TRACE_DROP_REASON_SER_DROP},
    {"SRV6_PROC_DROP", BCM_PKTIO_TRACE_DROP_REASON_SRV6_PROC_DROP},
    {"SRV6_VALIDATION_DROP", BCM_PKTIO_TRACE_DROP_REASON_SRV6_VALIDATION_DROP},
    {"VXLAN_TUNNEL_ERROR_DROP", BCM_PKTIO_TRACE_DROP_REASON_VXLAN_TUNNEL_ERROR_DROP},
    {"VXLAN_VN_ID_LOOKUP_MISS", BCM_PKTIO_TRACE_DROP_REASON_VXLAN_VN_ID_LOOKUP_MISS},
    {"UNKNOWN_RH_WITH_NONZERO_SL_DROP", BCM_PKTIO_TRACE_DROP_REASON_UNKNOWN_RH_WITH_NONZERO_SL_DROP},
    {"trace drop reason count", BCM_PKTIO_TRACE_DROP_REASON_COUNT}
};

pkttrace_enum_map_t counter_names[] =
{
  {"RIPD4", BCM_PKTIO_TRACE_COUNTER_RIPD4},
    {"RIPC4", BCM_PKTIO_TRACE_COUNTER_RIPC4},
    {"RIPHE4", BCM_PKTIO_TRACE_COUNTER_RIPHE4},
    {"IMRP4", BCM_PKTIO_TRACE_COUNTER_IMRP4},
    {"RIPD6", BCM_PKTIO_TRACE_COUNTER_RIPD6},
    {"RIPC6", BCM_PKTIO_TRACE_COUNTER_RIPC6},
    {"RIPHE6", BCM_PKTIO_TRACE_COUNTER_RIPHE6},
    {"IMRP6", BCM_PKTIO_TRACE_COUNTER_IMRP6},
    {"BFD_UNKNOWN_ACH_ERROR", BCM_PKTIO_TRACE_COUNTER_BFD_UNKNOWN_ACH_ERROR},
    {"BFD_UNKNOWN_CONTROL_PACKET", BCM_PKTIO_TRACE_COUNTER_BFD_UNKNOWN_CONTROL_PACKET},
    {"BFD_UNKNOWN_VER_OR_DISCR", BCM_PKTIO_TRACE_COUNTER_BFD_UNKNOWN_VER_OR_DISCR},
    {"BLOCK_MASK_DROP", BCM_PKTIO_TRACE_COUNTER_BLOCK_MASK_DROP},
    {"DSFRAG", BCM_PKTIO_TRACE_COUNTER_DSFRAG},
    {"DSICMP", BCM_PKTIO_TRACE_COUNTER_DSICMP},
    {"DSL2HE", BCM_PKTIO_TRACE_COUNTER_DSL2HE},
    {"IMBP", BCM_PKTIO_TRACE_COUNTER_IMBP},
    {"LAGLUP", BCM_PKTIO_TRACE_COUNTER_LAGLUP},
    {"LAGLUPD", BCM_PKTIO_TRACE_COUNTER_LAGLUPD},
    {"MTU_CHECK_FAIL", BCM_PKTIO_TRACE_COUNTER_MTU_CHECK_FAIL},
    {"PARITYD", BCM_PKTIO_TRACE_COUNTER_PARITYD},
    {"PDISC", BCM_PKTIO_TRACE_COUNTER_PDISC},
    {"RDROP", BCM_PKTIO_TRACE_COUNTER_RDROP},
    {"RIMDR", BCM_PKTIO_TRACE_COUNTER_RIMDR},
    {"RPORTD", BCM_PKTIO_TRACE_COUNTER_RPORTD},
    {"RTUN", BCM_PKTIO_TRACE_COUNTER_RTUN},
    {"RTUNE", BCM_PKTIO_TRACE_COUNTER_RTUNE},
    {"RUC", BCM_PKTIO_TRACE_COUNTER_RUC},
    {"SRC_PORT_KNOCKOUT_DROP", BCM_PKTIO_TRACE_COUNTER_SRC_PORT_KNOCKOUT_DROP},
    {"RDISC", BCM_PKTIO_TRACE_COUNTER_RDISC},
    {"RFILDR", BCM_PKTIO_TRACE_COUNTER_RFILDR},
    {"IRPSE", BCM_PKTIO_TRACE_COUNTER_IRPSE},
    {"IRHOL", BCM_PKTIO_TRACE_COUNTER_IRHOL},
    {"IRIBP", BCM_PKTIO_TRACE_COUNTER_IRIBP},
    {"DSL3HE", BCM_PKTIO_TRACE_COUNTER_DSL3HE},
    {"IUNKHDR", BCM_PKTIO_TRACE_COUNTER_IUNKHDR},
    {"DSL4HE", BCM_PKTIO_TRACE_COUNTER_DSL4HE},
    {"IMIRROR", BCM_PKTIO_TRACE_COUNTER_IMIRROR},
    {"MTUERR", BCM_PKTIO_TRACE_COUNTER_MTUERR},
    {"VLANDR", BCM_PKTIO_TRACE_COUNTER_VLANDR},
    {"HGHDRE", BCM_PKTIO_TRACE_COUNTER_HGHDRE},
    {"MCIDXE", BCM_PKTIO_TRACE_COUNTER_MCIDXE},
    {"RHGUC", BCM_PKTIO_TRACE_COUNTER_RHGUC},
    {"RHGMC", BCM_PKTIO_TRACE_COUNTER_RHGMC},
    {"URPF", BCM_PKTIO_TRACE_COUNTER_URPF},
    {"VFPDR", BCM_PKTIO_TRACE_COUNTER_VFPDR},
    {"DSTDISC", BCM_PKTIO_TRACE_COUNTER_DSTDISC},
    {"CBLDROP", BCM_PKTIO_TRACE_COUNTER_CBLDROP},
    {"MACLMT_NODROP", BCM_PKTIO_TRACE_COUNTER_MACLMT_NODROP},
    {"MACLMT_DROP", BCM_PKTIO_TRACE_COUNTER_MACLMT_DROP},
    {"VNTAG_ERROR", BCM_PKTIO_TRACE_COUNTER_VNTAG_ERROR},
    {"NIV_FORWARDING_DROP", BCM_PKTIO_TRACE_COUNTER_NIV_FORWARDING_DROP},
    {"OFFSET_NONTRILL_FRAME_ON_NW_PORT_DROP", BCM_PKTIO_TRACE_COUNTER_OFFSET_NONTRILL_FRAME_ON_NW_PORT_DROP},
    {"OFFSET_TRILL_FRAME_ON_ACCESS_PORT_DROP", BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_FRAME_ON_ACCESS_PORT_DROP},
    {"OFFSET_TRILL_ERRORS_DROP", BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_ERRORS_DROP},
    {"OFFSET_TRILL_RBRIDGE_LOOKUP_MISS_DROP", BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_RBRIDGE_LOOKUP_MISS_DROP},
    {"OFFSET_TRILL_HOPCOUNT_CHECK_FAIL", BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_HOPCOUNT_CHECK_FAIL},
    {"OFFSET_TRILL_RPF_CHECK_FAIL_DROP", BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_RPF_CHECK_FAIL_DROP},
    {"IRVOQFC", BCM_PKTIO_TRACE_COUNTER_IRVOQFC},
    {"ECMP_NOT_RESOLVED", BCM_PKTIO_TRACE_COUNTER_ECMP_NOT_RESOLVED},
    {"trace counter count", BCM_PKTIO_TRACE_COUNTER_COUNT}
};

pkttrace_enum_map_t tag_status_names[] =
{
  {"UNTAGGED", BCM_PKTIO_TRACE_INCOMING_TAG_STATUS_UNTAGGED},
    {"TAGGED", BCM_PKTIO_TRACE_INCOMING_TAG_STATUS_TAGGED},
};

/* Parsing string given by user as packet payload */
static uint8 *
parse_data_packet_payload(int unit, char *packet_data, int *length)
{
  uint8 *p;
  char tmp, data_iter;
  int data_len, i, j, pkt_len, data_base;

  /* If string data starts with 0x or 0X, skip it */
  if ((packet_data[0] == '0')
      && (packet_data[1] == 'x' || packet_data[1] == 'X')) {
    data_base = 2;
  } else {
    data_base = 0;
  }

  data_len = sal_strlen(packet_data) - data_base;
  pkt_len = (data_len + 1) / 2;
  if (pkt_len < 64) {
    pkt_len = 64;
  }

  p = (auto)sal_alloc(pkt_len, "tx_string_packet");
  if (p == NULL) {
    return NULL;
  }
  sal_memset(p, 0, pkt_len);
  /* Convert char to value */
  i = j = 0;
  while (j < data_len) {
    data_iter = packet_data[data_base + j];
    if (('0' <= data_iter) && (data_iter <= '9')) {
      tmp = data_iter - '0';
    } else if (('a' <= data_iter) && (data_iter <= 'f')) {
      tmp = data_iter - 'a' + 10;
    } else if (('A' <= data_iter) && (data_iter <= 'F')) {
      tmp = data_iter - 'A' + 10;
    } else if (data_iter == ' ') {
      ++j;
      continue;
    } else {
      sal_free((void *)p);
      return NULL;
    }

    /* String input is in 4b unit. Below we're filling in 8b:
       offset is /2, and we shift by 4b if the input char is odd */
    p[i / 2] |= tmp << (((i + 1) % 2) * 4);
    ++i;
    ++j;
  }

  *length = i / 2;
  return p;
}

bcm_pktio_trace_data_t trace_data = {0};
bcm_error_t
config_pkt_trace(int unit, int port, char *pkt)
{
  bcm_error_t rv = BCM_E_NONE;
  char data[512];
  bcm_pktio_trace_pkt_t trace_pkt;
  uint8 *pkt_data = NULL;
  int pkt_len = 0;

// snprintf(data, 512,
// "0xb46ad4a3fc7f5c0758a4e00308004500001c00010000401152b80a0a0a030a0a0a02066e12b50008bea2");

  sal_memcpy(data, pkt, sal_strlen(pkt)+1);
  pkt_data = parse_data_packet_payload(unit, &data, &pkt_len);
  if (pkt_data == NULL) {
    rv = BCM_E_FAIL;
  }

  trace_pkt.port = port;
  trace_pkt.pkt  = pkt_data;
  trace_pkt.len  = pkt_len;
  rv = bcm_pktio_trace_data_collect(unit, &trace_pkt, &trace_data);
  if (BCM_FAILURE(rv)) {
    printf("bcm_pktio_trace_data_collect() FAILED: %s\n", bcm_errmsg(rv));
    return rv;
  }

  return rv;
}

bcm_error_t
decode_packet_trace_data_fields(int unit, bcm_pktio_trace_data_t *trace_data)
{
  int i=0;
  int rv=0;
  unsigned int val;
  bcm_pktio_bitmap_t drop_bitmap;
  bcm_pktio_bitmap_t counter_bitmap;

  printf("\nFIELDS:\n");
  for (i = 0; i < BCM_PKTIO_TRACE_FID_COUNT; i++) {
    rv = bcm_pktio_trace_field_get(unit, trace_data, i, &val);
    if ( BCM_E_UNAVAIL == rv) continue;
    switch (i) {
      case BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR:
        printf("\t%-32s:%s\n", field_names[i].name,
               pkt_resolution_names[val].name);
        break;
      case BCM_PKTIO_TRACE_INCOMING_TAG_STATUS:
        printf("\t%-32s:%s\n", field_names[i].name,
               tag_status_names[val].name);
        break;
      case BCM_PKTIO_TRACE_FORWARDING_TYPE:
        printf("\t%-32s:%s\n", field_names[i].name,
               fwd_type_names[val].name);
        break;
      case BCM_PKTIO_TRACE_FWD_DESTINATION_TYPE:
        printf("\t%-32s:%s\n", field_names[i].name,
               fwd_dest_type_names[val].name);
        break;
      default:
        printf("\t%-32s:%d\n", field_names[i].name, val);
    }
  }

  bcm_pktio_trace_drop_reasons_get(unit, trace_data, &drop_bitmap);
  printf("\nDROP_REASON:\n");
  for (i = 0; i < BCM_PKTIO_TRACE_DROP_REASON_COUNT; i++) {
    if (SHR_BITGET((auto)drop_bitmap.pbits, i)) {
      printf("\t%-32s\n", drop_reason_names[i].name);
    }
  }

  bcm_pktio_trace_counter_get(unit, trace_data, &counter_bitmap);
  printf("\nCOUNTER:\n");
  for (i = 0; i < BCM_PKTIO_TRACE_COUNTER_COUNT; i++) {
    if (SHR_BITGET((auto)counter_bitmap.pbits, i)) {
      printf("\t%-32s\n", counter_names[i].name);
    }
  }

  return BCM_E_NONE;
}

bcm_error_t
decode_pkt_trace_data(int unit, bcm_pktio_trace_data_t *trace_data)
{
  bcm_error_t rv = BCM_E_NONE;
  int idx;

  if ((!trace_data->len) || (NULL == trace_data->buf)){
    printf("Invalid trace data\n");
  }

  rv = decode_packet_trace_data_fields(unit, trace_data);
  if (BCM_FAILURE(rv)) {
    printf("decode_packet_trace_data_fields() FAILED: %s\n", bcm_errmsg(rv));
    return rv;
  }

  printf("\nDATA:\n");
  for (idx = 0; idx < trace_data->len; idx++) {
    if ((idx & 0xf) == 0) {
      printf("[%04x]: ", idx);
    }
    if ((idx & 0xf) == 8) {
      printf("- ");
    }
    printf("%02x ", trace_data->buf[idx]);

    if ((idx & 0xf) == 0xf) {
      printf("\n");
    }
  }
  printf("\n");

  return rv;
}

void run_test (int unit, int port, char *pkt)
{
  ingress_port = port;
  print ingress_port_setup(unit, ingress_port);
  print config_pkt_trace(unit, port, pkt);
  print decode_pkt_trace_data(unit, &trace_data);
}
