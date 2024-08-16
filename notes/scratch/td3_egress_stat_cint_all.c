uint32_t egr_stat_id = 0;
int unit = 0;
uint32_t egress_if = 400005;
uint32_t vp = 2952790018;


void
flex_stat_setup_egress_stats (int unit, uint32_t egress_if, uint32_t vp)
{
  int rv = 0;
	uint32 mode_id = 0;
	uint32 entry_in = 0;
  uint32_t stat_cnt_id;
	uint32 ret_cnt_id = 0;
	uint32 num_attr_sel = 2;
	uint32_t flow_handle = 0;
	uint32 total_counters = 2;
	int object = bcmStatObjectEgrVxlan;
	int stat_flag = BCM_STAT_GROUP_MODE_EGRESS;
	bcm_stat_group_mode_attr_selector_t attr_selectors[7];


	bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[0]);
	attr_selectors[0].counter_offset = 0;
	attr_selectors[0].attr = bcmStatGroupModeAttrPktType;
	attr_selectors[0].attr_value = bcmStatGroupModeAttrPktTypeKnownL3UC;

  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[1]);
  attr_selectors[1].counter_offset = 1;
  attr_selectors[1].attr = bcmStatGroupModeAttrPktType;
  attr_selectors[1].attr_value = bcmStatGroupModeAttrPktTypeUnknownL3UC;

  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[2]);
  attr_selectors[2].counter_offset = 2;
  attr_selectors[2].attr = bcmStatGroupModeAttrPktType;
  attr_selectors[2].attr_value = bcmStatGroupModeAttrPktTypeL2BC;

  num_attr_sel = 3;
  total_counters = 3;

	printf("group mode id create L3KUC, L3UKC, L2BC\n");
	rv = bcm_stat_group_mode_id_create(unit, // 0
																			stat_flag, // BCM_STAT_GROUP_MODE_EGRESS
																			total_counters, // 7
																			num_attr_sel, // 7
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
  egr_stat_id = stat_cnt_id;
}

void
flex_stat_get_egress_counts (int unit, uint32 stat_id)
{
	int num_entries = 7;
	uint32_t cnt_idx[7] = {0, 1, 2, 3, 4, 5, 6};
	bcm_stat_flex_stat_t stat = bcmStatFlexStatPackets;
  bcm_stat_value_t stat_values[7];

  print bcm_stat_flex_counter_sync_get(unit, stat_id, stat,
                                       num_entries, cnt_idx, stat_values);
  printf("\nFlexStat packet counter - bcmStatGroupModeAttrPktTypeKnownL2UC - %d\n",stat_values[0].packets);
  printf("\nFlexStat packet counter - bcmStatGroupModeAttrPktTypeUnknownL2UC - %d\n",stat_values[1].packets);
  printf("\nFlexStat packet counter - bcmStatGroupModeAttrPktTypeL2BC - %d\n",stat_values[2].packets);
  printf("\nFlexStat packet counter - bcmStatGroupModeAttrPktTypeKnownL2MC - %d\n",stat_values[3].packets);
  printf("\nFlexStat packet counter - bcmStatGroupModeAttrPktTypeUnknownL2MC - %d\n",stat_values[4].packets);
  printf("\nFlexStat packet counter - bcmStatGroupModeAttrPktTypeKnownL3UC - %d\n",stat_values[5].packets);
  printf("\nFlexStat packet counter - bcmStatGroupModeAttrPktTypeUnknownL3UC - %d\n",stat_values[6].packets);
}

flex_stat_setup_egress_stats(0, egress_if, vp);
