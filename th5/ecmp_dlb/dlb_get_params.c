cint_reset();
int unit = 0;
int32 seed = 0;
int use_flow_based_hash_ecmp = 0;
int block_n_offset_select = 0;
int hash_control;
int hash_computation = 0;
int pre_process_enable = 0;
int ipv4_select_fields = 0;
int dlb_ecmp_sampling_rate = 0;
int dlb_ecmp_random_seed = 0;
int dlb_ecmp_egress_bytes_exponent = 0;
int dlb_ecmp_queued_bytes_exponent = 0;
int dlb_ecmp_physical_queued_bytes_exponent = 0;
int dlb_ecmp_egress_bytes_decrease_reset = 0;
int dlb_ecmp_queued_bytes_decrease_reset = 0;
int dlb_ecmp_physical_queued_decrease_reset = 0;
int dlb_ecmp_egress_bytes_min_threshold = 0;
int dlb_ecmp_egress_bytes_max_threshold = 0;
int dlb_ecmp_queued_bytes_min_threshold = 0;
int dlb_ecmp_queued_bytes_max_threshold = 0;
int dlb_ecmp_physical_queued_bytes_min_threshold = 0;
int dlb_ecmp_physical_queued_bytes_max_threshold = 0;

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashSeed0, &seed));
printf("bcmSwitchHashSeed0 = 0x%X\n", seed);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashField0Config, &hash_computation));
printf("bcmSwitchHashField0Config = %d\n", hash_computation);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashField1Config, &hash_computation));
printf("bcmSwitchHashField1Config = %d\n", hash_computation);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashField0Config1, &hash_computation));
printf("bcmSwitchHashField0Config1 = %d\n", hash_computation);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashField1Config1, &hash_computation));
printf("bcmSwitchHashField1Config1 = %d\n", hash_computation);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashIP4TcpUdpField0, &ipv4_select_fields));
printf("bcmSwitchHashIP4TcpUdpField0 = %d\n", ipv4_select_fields);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0, &ipv4_select_fields));
printf("bcmSwitchHashIP4TcpUdpPortsEqualField0 = %d\n", ipv4_select_fields);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashIP4Field0, &ipv4_select_fields));
printf("bcmSwitchHashIP4Field0 = %d\n", ipv4_select_fields);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashField0PreProcessEnable, &pre_process_enable));
printf("bcmSwitchHashField0PreProcessEnable = %d\n", pre_process_enable);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashUseFlowSelEcmp, &use_flow_based_hash_ecmp));
printf("bcmSwitchHashUseFlowSelEcmp = %d\n", use_flow_based_hash_ecmp);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashUseFlowSelEcmpDynamic, &use_flow_based_hash_ecmp));
printf("bcmSwitchHashUseFlowSelEcmpDynamic = %d\n", use_flow_based_hash_ecmp);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchECMPHashSet0Offset, &block_n_offset_select));
printf("bcmSwitchECMPHashSet0Offset = %d\n", block_n_offset_select);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicHashOffset, &block_n_offset_select));
printf("bcmSwitchEcmpDynamicHashOffset = %d\n", block_n_offset_select);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashControl, &hash_control));
printf("bcmSwitchHashControl = %d\n", hash_control);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicSampleRate, &dlb_ecmp_sampling_rate));
printf("bcmSwitchEcmpDynamicSampleRate = %d\n", dlb_ecmp_sampling_rate);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicEgressBytesExponent, &dlb_ecmp_egress_bytes_exponent));
printf("bcmSwitchEcmpDynamicEgressBytesExponent = %d\n", dlb_ecmp_egress_bytes_exponent);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicEgressBytesDecreaseReset, &dlb_ecmp_egress_bytes_decrease_reset));
printf("bcmSwitchEcmpDynamicEgressBytesDecreaseReset = %d\n", dlb_ecmp_egress_bytes_decrease_reset);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicQueuedBytesExponent, &dlb_ecmp_queued_bytes_exponent));
printf("bcmSwitchEcmpDynamicQueuedBytesExponent = %d\n", dlb_ecmp_queued_bytes_exponent);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicQueuedBytesDecreaseReset, &dlb_ecmp_queued_bytes_decrease_reset));
printf("bcmSwitchEcmpDynamicQueuedBytesDecreaseReset = %d\n",dlb_ecmp_queued_bytes_decrease_reset);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent, &dlb_ecmp_physical_queued_bytes_exponent));
printf("bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent = %d\n", dlb_ecmp_physical_queued_bytes_exponent);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset, &dlb_ecmp_physical_queued_decrease_reset));
printf("bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset = %d\n", dlb_ecmp_physical_queued_decrease_reset);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicEgressBytesMinThreshold, &dlb_ecmp_egress_bytes_min_threshold));
printf("bcmSwitchEcmpDynamicEgressBytesMinThreshold = 0x%X\n", dlb_ecmp_egress_bytes_min_threshold);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicEgressBytesMaxThreshold, &dlb_ecmp_egress_bytes_max_threshold));
printf("bcmSwitchEcmpDynamicEgressBytesMaxThreshold = 0x%X\n", dlb_ecmp_egress_bytes_max_threshold);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicQueuedBytesMinThreshold, &dlb_ecmp_queued_bytes_min_threshold));
printf("bcmSwitchEcmpDynamicQueuedBytesMinThreshold = 0x%X\n", dlb_ecmp_queued_bytes_min_threshold);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicQueuedBytesMaxThreshold, &dlb_ecmp_queued_bytes_max_threshold));
printf("bcmSwitchEcmpDynamicQueuedBytesMaxThreshold = 0x%X\n", dlb_ecmp_queued_bytes_max_threshold);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold, &dlb_ecmp_physical_queued_bytes_min_threshold));
printf("bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold = 0x%X\n", dlb_ecmp_physical_queued_bytes_min_threshold);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold, &dlb_ecmp_physical_queued_bytes_max_threshold));
printf("bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold = 0x%X\n", dlb_ecmp_physical_queued_bytes_max_threshold);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchEcmpDynamicRandomSeed, &dlb_ecmp_random_seed));
printf("bcmSwitchEcmpDynamicRandomSeed = 0x%X\n", dlb_ecmp_random_seed);

int value = 0;
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashIP4Field1, &value));
printf("bcmSwitchHashIP4Field1 = %d \n", value);

BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashIP4Field0, &value));
printf("bcmSwitchHashIP4Field1 = %d \n", value);


BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashIP6TcpUdpPortsEqualField0, &value));
printf("bcmSwitchHashIP4Field0 = %d \n", value);
BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashIP6TcpUdpPortsEqualField1, &value));
printf("bcmSwitchHashIP4Field1 = %d \n", value);
int config_dlb_egr_port_attributes_get (int unit, bcm_port_t egr_port)
{
  bcm_l3_ecmp_dlb_port_quality_attr_t quality_attr;
  bcm_l3_ecmp_dlb_port_quality_attr_t_init(&quality_attr);
  BCM_IF_ERROR_RETURN(bcm_l3_ecmp_dlb_port_quality_attr_get(unit, egr_port, &quality_attr));
  printf("scaling_factor = %d\n", quality_attr.scaling_factor);
  printf("load_weight = %d\n", quality_attr.load_weight);
  printf("queue_size_weight = %d\n", quality_attr.queue_size_weight);
}

config_dlb_egr_port_attributes_get(0,  343);
config_dlb_egr_port_attributes_get(0, 2);
config_dlb_egr_port_attributes_get(0, 121);

int print_port_quality (int unit, bcm_port_t egr_port)
{
  bcm_l3_ecmp_dlb_port_quality_t port_quality;
  port_quality.buffer_id = 0;
  port_quality.port = egr_port;
  port_quality.quality = 0;

  printf("port %d \n", egr_port);
  BCM_IF_ERROR_RETURN(bcm_l3_ecmp_dlb_port_quality_get(unit, &port_quality));
  printf("\t- quality(0) - %d\n", port_quality.quality);

  port_quality.buffer_id = 1;
  port_quality.port = egr_port;
  port_quality.quality = 0;

  BCM_IF_ERROR_RETURN(bcm_l3_ecmp_dlb_port_quality_get(unit, &port_quality));
  printf("\t- quality(1) - %d\n", port_quality.quality);
  return 0;
}


print_port_quality(0, 343);
print_port_quality(0, 2);
print_port_quality(0, 121);

swp8 -- 121
swp47s1 -- 2
swp48s2 -- 343

pt reset DLB_ECMP_FLOWSET_INST0m
pt reset DLB_ECMP_FLOWSET_INST1m

pt dump -d DLB_ECMP_FLOWSET_INST0m
pt dump -d DLB_ECMP_FLOWSET_INST1m

bcm_l3_egress_ecmp_t ecmp_info;
bcm_l3_ecmp_member_t ecmp_member_array[10];
int ecmp_member_count = 0;
bcm_l3_egress_t egr;
int i = 0;
int unit = 0;
int rv = 0;

bcm_l3_egress_ecmp_t_init(&ecmp_info);
ecmp_info.ecmp_intf = 203970;

rv = bcm_l3_ecmp_get(unit, &ecmp_info, 10,
                                          ecmp_member_array, &ecmp_member_count);
print ecmp_info;
printf("\n\necmp member count %d\n", ecmp_member_count);

bcm_l3_ecmp_dlb_port_quality_t port_quality;
port_quality.buffer_id = 0;
port_quality.port = 343;

bcm_l3_ecmp_dlb_port_quality_get(0, &port_quality);
print port_quality;


port_quality.buffer_id = 0;
port_quality.port = 2;

bcm_l3_ecmp_dlb_port_quality_get(0, &port_quality);
print port_quality;

port_quality.buffer_id = 0;
port_quality.port = 121;

bcm_l3_ecmp_dlb_port_quality_get(0, &port_quality);
print port_quality;
