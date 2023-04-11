cint_reset();

int unit = 0;
uint32_t egress_if = 400005;
uint32_t vp = 2952790018;

void
flex_stat_setup_custom_egress_stats (int unit, uint32_t egress_if,
                              uint32_t vp, uint32_t value,
                              char *value_str,
                              uint32_t *egr_stat_id)
{
  int rv = 0;
	uint32 mode_id = 0;
	uint32 entry_in = 0;
  uint32_t stat_cnt_id;
	uint32 ret_cnt_id = 0;
	uint32 num_attr_sel = 1;
	uint32_t flow_handle = 0;
	uint32 total_counters = 1;
	int object = bcmStatObjectEgrVxlan;
	int stat_flag = BCM_STAT_GROUP_MODE_EGRESS;
	bcm_stat_group_mode_attr_selector_t attr_selectors[1];

	bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[0]);
	attr_selectors[0].counter_offset = 0;
	attr_selectors[0].attr = bcmStatGroupModeAttrPktType;
	attr_selectors[0].attr_value = value;

  printf("-------------------------------------------------\n");
	printf("group mode id create value %s\n", value_str);
	rv = bcm_stat_group_mode_id_create(unit, // 0
																			stat_flag, // BCM_STAT_GROUP_MODE_EGRESS
																			total_counters, // 1
																			num_attr_sel, // 1
																			attr_selectors, // array of counter selectors
																			&mode_id);

	printf("mode_id %d rv = %d/ %s\n", mode_id, rv, bcm_errmsg(rv));
	printf("custom group create \n");
	rv = bcm_stat_custom_group_create(unit, // 0
																		 mode_id,
																		 object, //bcmStatObjectEgrVxlan
																		 &stat_cnt_id,
																		 &entry_in);

	printf("FlexStat counter id is %d rv = %d/ %s\n", stat_cnt_id, rv,
         bcm_errmsg(rv));

	printf("FlexStat entry id is %d\n",entry_in);

	print bcm_flow_handle_get(unit, "CLASSIC_VXLAN", &flow_handle);

	printf("flow handle is %d\n", flow_handle);


	bcm_flow_stat_info_t stat_info;
	bcm_flow_stat_info_t_init(&stat_info);

	stat_info.flow_handle = flow_handle;
	stat_info.function_type = bcmFlowFuncTypeEgressObj;
	stat_info.valid_elements = BCM_FLOW_STAT_EGRESS_IF_VALID;

	stat_info.egress_if = egress_if;

	rv = bcm_flow_stat_attach(unit,&stat_info,0,NULL,stat_cnt_id);
  printf ("stat attach rv %d/ %s \n", rv, bcm_errmsg(rv));

	rv = bcm_flow_stat_id_get(unit,&stat_info,0,NULL,object,&ret_cnt_id);
  printf ("stat get rv %d/ %s \n", rv, bcm_errmsg(rv));

	printf("stat cnt id %d retcnt id %d \n", stat_cnt_id, ret_cnt_id);
  *egr_stat_id = stat_cnt_id;
  if (stat_cnt_id != ret_cnt_id) {
    *egr_stat_id = ret_cnt_id;
  }
}

void
flex_stat_get_custom_egress_counts (int unit, uint32 stat_id, char *str)
{
	int num_entries = 1;
	uint32_t cnt_idx[1] = {0};
	bcm_stat_flex_stat_t stat = bcmStatFlexStatPackets;
  bcm_stat_value_t stat_values[1];
  int rv = 0;

  rv = bcm_stat_flex_counter_sync_get(unit, stat_id, stat,
                                       num_entries, cnt_idx, stat_values);
  printf("FlexStat stat_id %d packet counter - %s- %d\n",stat_id, str, stat_values[0].packets);
  printf("-------------------------------------------------\n");
}
uint32_t egress_stat_id_l2_ukuc = 0;
uint32_t egress_stat_id_l2_kuc = 0;
uint32_t egress_stat_id_l2_bc = 0;
uint32_t egress_stat_id_l2_mc = 0;
uint32_t egress_stat_id_l2_uk_mc = 0;
uint32_t egress_stat_id_l3_uc = 0;
uint32_t egress_stat_id_l3_uk_uc = 0;
uint32_t egress_stat_id_all = 0;
void flex_stat_setup_custom_stats_egress (int unit, uint32_t egress_if, uint32_t vp)
{
  flex_stat_setup_custom_egress_stats(unit, egress_if, vp, bcmStatGroupModeAttrPktTypeUnknownL2UC,
                               "UnknownL2UC", &egress_stat_id_l2_ukuc);
  flex_stat_get_custom_egress_counts(unit, egress_stat_id_l2_ukuc, "UnknownL2UC");

  flex_stat_setup_custom_egress_stats(unit, egress_if, vp,
                               bcmStatGroupModeAttrPktTypeKnownL2UC, "KnownL2UC",
                               &egress_stat_id_l2_kuc);
  flex_stat_get_custom_egress_counts(unit, egress_stat_id_l2_kuc, "UnknownL2UC");

  flex_stat_setup_custom_egress_stats(unit, egress_if, vp,
                               bcmStatGroupModeAttrPktTypeL2BC, "L2BC",
                               &egress_stat_id_l2_bc);
  flex_stat_get_custom_egress_counts(unit, egress_stat_id_l2_bc, "L2BC");

  flex_stat_setup_custom_egress_stats(unit, egress_if, vp,
                               bcmStatGroupModeAttrPktTypeKnownL2MC, "L2MC",
                               &egress_stat_id_l2_mc);
  flex_stat_get_custom_egress_counts(unit, egress_stat_id_l2_mc, "L2MC");

  flex_stat_setup_custom_egress_stats(unit, egress_if, vp,
                               bcmStatGroupModeAttrPktTypeUnknownL2MC, "L2UKMC",
                               &egress_stat_id_l2_uk_mc);
  flex_stat_get_custom_egress_counts(unit, egress_stat_id_l2_uk_mc, "L2UKMC");

  flex_stat_setup_custom_egress_stats(unit, egress_if, vp,
                               bcmStatGroupModeAttrPktTypeKnownL3UC, "L3UC",
                               &egress_stat_id_l3_uc);
  flex_stat_get_custom_egress_counts(unit, egress_stat_id_l3_uc, "L3UC");

  flex_stat_setup_custom_egress_stats(unit, egress_if, vp,
                               bcmStatGroupModeAttrPktTypeUnknownL3UC, "L3UKUC",
                               &egress_stat_id_l3_uk_uc);
  flex_stat_get_custom_egress_counts(unit, egress_stat_id_l3_uk_uc, "L3UKUC");

  flex_stat_setup_custom_egress_stats(unit, egress_if, vp,
                               bcmStatGroupModeAttrPktTypeAll, "All",
                               &egress_stat_id_all);
  flex_stat_get_custom_egress_counts(unit, egress_stat_id_all, "All");
}

flex_stat_setup_custom_stats_egress(unit, egress_if, vp);
