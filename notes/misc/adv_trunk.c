/*
* adv_trunk.c
*  ** This is only for J2 Cmodel as of now
*  trunk port prograaming along with L2 and L3( subinterface ) for trunk .
*  Also has Acl and add/delete of memberlinks
*  Target: Jerico 2
*  Depends: vlan_translations.c, global.c
*  For uses and experiment please look at trunk_notes
**/

int unit = 0;  // Works for a single unit as of now
int core = 0;  // TODO_AMIT we need to handle multiple cores
int vrf = 5;   // using single vrf as of now
bcm_field_context_t ingress_context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;  //  needed for acls
bcm_field_context_t egress_context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;   // needed for acls

// This api is used by trunk_main()
// Takes in tid, number of member ports and memberport array
// Returns the trunk gport
int trunk_create(bcm_trunk_t tid, int member_port_num, int *member_ports)
{
 int                 rv= BCM_E_NONE;
 int                 i;
 bcm_trunk_member_t  member_array[20];
 bcm_trunk_info_t    trunk_info;
 int tgport;
 bcm_switch_control_key_t key;
 bcm_switch_control_info_t value;


 sal_memset(member_array, 0, sizeof(member_array));
 sal_memset(&trunk_info, 0, sizeof(trunk_info));

 // This is necessary for J2
 BCM_TRUNK_ID_SET(tid, 0, tid);

 /* Creates #tid trunk (no memory allocation and no members inside) */
 rv =  bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);

 BCM_GPORT_TRUNK_SET(tgport, tid);
 printf("trunk gport is  create: gport=%d\n", tgport);
 key.index=0;
 key.type=bcmSwitchPortHeaderType;
 value.value=BCM_SWITCH_PORT_HEADER_TYPE_ETH;
 rv = bcm_switch_control_indexed_port_set(0, tgport, key, value);
 for (i=0; i < member_port_num; i++) {
   rv = bcm_switch_control_indexed_port_set(0, member_ports[i], key, value);
 }
 /* Prepare local ports to gports */
 for (i=0; i<member_port_num; i++)
   {
     BCM_GPORT_SYSTEM_PORT_ID_SET(member_array[i].gport, member_ports[i]);
   }
 trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW;

 /* Adds members (in member_array) to trunk and activates trunk mechanism */
 rv = bcm_trunk_set(unit, tid, &trunk_info, member_port_num, member_array);

 // IN case you want to check the pp port values for the port
 /*
 bcm_trunk_pp_port_allocation_info_t trunk_pp_port_aloc_info_get;
 bcm_trunk_pp_port_allocation_info_t_init(&trunk_pp_port_aloc_info_get);
 bcm_trunk_pp_port_allocation_get(unit,tid, 0,
          &trunk_pp_port_aloc_info_get);

 printf("Allocated pp_port1 is %d and pp_port2 is %d. \n",
  trunk_pp_port_aloc_info_get.pp_port_per_core_array[0],
  trunk_pp_port_aloc_info_get.pp_port_per_core_array[1]);
 */
 printf("trunk set\n");
 return tgport;
}

int
set_tpid( bcm_gport_t gport )
{
 printf( "setting tpid\n" );

 bcm_port_tpid_class_t tpid_class;
 int rv;
 /*
  * define tag format class ID 2 for single tagged packets
  * on this port - this is output from parser and input to
  * resolve IVE / EVE command class ID to be applied to the
  * packet.
  */
 bcm_port_tpid_class_t_init(&tpid_class);
 tpid_class.port = gport;   // using the gport
 tpid_class.tpid1 = 0x8100;
 tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
 tpid_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_TAG;
 tpid_class.vlan_translation_action_id = 0;
 rv = bcm_port_tpid_class_set(unit, &tpid_class);

 /*
  * define tag format class ID 0 for untagged packets
  * egress to this port - this is output from parser and input to
  * resolve IVE / EVE command class ID to be applied to the
  * packet.
  */
 bcm_port_tpid_class_t_init(&tpid_class);
 tpid_class.port = gport;
 tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
 tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
 tpid_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_NO_TAG;
 tpid_class.vlan_translation_action_id = 0;
 rv = bcm_port_tpid_class_set(unit, &tpid_class);
 return rv;
}

// Creating a trunk port, this is what we use from outside
// Returns the trunk gport values based on the TID
// This API only uses two ports per trunk
int
main_trunk( int port_0, int port_1,  bcm_trunk_t tid, int vlan_domain)
{
 int rv = BCM_E_NONE;
 int member_ports[20];
 int member_port_num = 2;

 member_ports[0] = port_0;
 member_ports[1] = port_1;

 bcm_gport_t tgport = trunk_create(tid, member_port_num, member_ports);
 printf("Trunk #%d created with %d ports\n", tid, member_port_num);

 // Add the vlan domain for the trunk pp ports
 rv = bcm_port_class_set(unit, tgport, bcmPortClassId, vlan_domain);
 return tgport;
}


// This does what is being done inside
// bcm_dnx_sdk_extract_and_add_port_to_vlan()
// Associates a port to vlan ( membership )
// Default vlan on port based on tagging
// Adds LIF for port+Vlan and then an VSI for the LIF
int
add_gport_to_vlan( bcm_gport_t gport, int vlan_id, int vsi_id,
      int tagged, int lif_id )
{
 int rv;
 bcm_vlan_port_t vp;
 bcm_vlan_port_translation_t vp_trans;
 uint32_t in_gp, out_gp;
 // Add gport to a vlan membership and define its
 // egress tag properties
 // Assumes symmetric configuration between incomming and outgoing
 // vlan port membership
 rv = bcm_vlan_gport_add(unit, vlan_id, gport,
       tagged ? 0 : BCM_VLAN_GPORT_ADD_UNTAGGED);

 // set the untagged property for trunk
 if (!tagged) {
   //Note: ingress VLAN miss is drop by default (PG 21.5.2)
   // This sets untagged packets default vlan for the incoming port
   rv = bcm_port_untagged_vlan_set(unit, gport, vlan_id);
 }


 /*
  * create (p,v) outLIF, for an entry in ESEM_FORWARD_DOMAIN_MAPPING_DB
  */
 // TODO_AMIT somehow we donot need any id for this guy ???
 // Pick up anything egressing on a port x VSI independently of VID.
 bcm_vlan_port_t_init(&vp);
 vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
 vp.port = gport;
 vp.vsi = vsi_id;
 vp.flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
 vp.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
 rv = bcm_vlan_port_create(unit, &vp);
 out_gp = vp.vlan_port_id;

 // Set up Symmetric Lifs for the port VLAN
 // This is what is being used for J2
 bcm_vlan_port_t_init(&vp);
 if (tagged) {
   vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
 } else {
   vp.criteria = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;
 }
 vp.port = gport;
 BCM_GPORT_SUB_TYPE_LIF_SET(vp.vlan_port_id, 0, lif_id + 0x1000);
 BCM_GPORT_VLAN_PORT_ID_SET(vp.vlan_port_id, vp.vlan_port_id);
 vp.flags = BCM_VLAN_PORT_WITH_ID;
 vp.egress_vlan = vp.match_vlan = vlan_id;
 // Create an LIF for this port vlan
 rv = bcm_vlan_port_create(unit, &vp);
 in_gp = vp.vlan_port_id;

 // Adds the LIFs to VSI
 rv = bcm_vswitch_port_add(unit, vsi_id, vp.vlan_port_id);

 /*
  * associate Egress VLAN edit class and VLAN edit
  * parameters to be used for VLAN editing on outLIF for
  * tagged / untagged ports. command key is derived from
  * this edit class plus the packet tag format.
  */
 printf( "setting egress vlan trans\n" );
 bcm_vlan_port_translation_t_init(&vp_trans);
 vp_trans.new_outer_vlan = vlan_id;
 vp_trans.gport = in_gp;
 if (tagged) {
   vp_trans.vlan_edit_class_id = EGRESS_EDIT_CLASS_TAGGED;
 } else {
   vp_trans.vlan_edit_class_id = EGRESS_EDIT_CLASS_UNTAGGED;
 }
 vp_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
 rv = bcm_vlan_port_translation_set(unit, &vp_trans);

 // Do for out gport as well
 vp_trans.gport = out_gp;
 rv = bcm_vlan_port_translation_set(unit, &vp_trans);

 /* /\* learn enable on port*\/ */
 /* unsigned int f; */
 /* rv = bcm_port_learn_get(unit, gport, &f); */
 /* //NOTE physical AND virtual ports are both need learn enable, BTW it is the default for physical ports */
 /* rv = bcm_port_learn_set(unit, gport, */
 /* 			  BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD); */

 return 0;
}

// Add a mac address for the trunk port + VSI combintion
// endmac = last octate of the mac address
// This is used for L2 testing cases
ret_code_e
add_mac_trunk( int tid,  uint32_t vsi, char endmac )
{
 bcm_l2_addr_t l2_addr;
 bcm_mac_t mac = {0,0,0,0,0,0};
 int rv;
 bcm_gport_t tgport;
 mac[5] = endmac;
 bcm_l2_addr_t_init(&l2_addr, mac, vsi);
 l2_addr.flags = BCM_L2_STATIC;      /* static entry */

 // Convert td to trunk gport
 BCM_TRUNK_ID_SET( tid, 0, tid );
 BCM_GPORT_TRUNK_SET(tgport, tid);
 l2_addr.port = tgport;
 rv = bcm_l2_addr_add(unit, &l2_addr);
 return rv;
}

// Add a mac address for the regular port + VSI combintion
// endmac = last octate of the mac address
// This is used for L2 testing cases
ret_code_e
add_mac_port(int port, uint32_t vsi, char endmac)
{
 bcm_l2_addr_t l2_addr;
 bcm_mac_t mac = {0,0,0,0,0,0};
 int rv;
 mac[5] = endmac;
 bcm_l2_addr_t_init(&l2_addr, mac, vsi);
 l2_addr.flags = BCM_L2_STATIC;      /* static entry */
 l2_addr.port = port;
 rv = bcm_l2_addr_add(unit, &l2_addr);
 return RET_CODE_SUCCESS;
}

// This is top level call for associating a trunk id with
// vlan, VSI and the LIF
int
add_trunk_to_vlan( int tid, int vlan_id, int vsi_id, int tagged, int lif_id )
{
 bcm_gport_t tgport;
 BCM_TRUNK_ID_SET(tid, 0, tid);
 BCM_GPORT_TRUNK_SET(tgport, tid);
 set_tpid( tgport );
 add_gport_to_vlan( tgport, vlan_id, vsi_id, tagged, lif_id );
 return 0;
}

// This is top level call for associating a reluar port  with
// vlan, VSI and the LIF
int
add_port_to_vlan( int port, int vlan_id, int vsi_id, int tagged, int lif_id )
{
 bcm_gport_t gport;
 BCM_GPORT_SYSTEM_PORT_ID_SET( gport, port );
 set_tpid( gport);
 add_gport_to_vlan( gport, vlan_id, vsi_id, tagged, lif_id );
 return 0;
}
// Create a vlan and VSI
int
create_vlan_vsi( int vlan_id, int vsi_id )
{
 // create VSI
 bcm_vswitch_create_with_id(unit, vsi_id);
 //create VLAN
 bcm_vlan_create(unit, vlan_id);
 return 0;
}

// Acl Rule to update TC value of packets coming in and out of trunk
// Accepts inport and dstport as qualifiers
// We have not added the VSI qual yet
int cint_field_trunk_qual_tc_rule(
   bcm_core_t core,
   bcm_field_stage_t stage,
   bcm_field_context_t context_id,
   bcm_field_qualify_t qual_type,
   uint32 qual_mask,
   bcm_gport_t trunk_gport,
   int vsi_id )
{
 // TODO amit add vsi qual as well
   bcm_field_group_info_t fg_info;
   bcm_field_group_attach_info_t attach_info;
   bcm_field_entry_info_t entry_info;
   void *dest_char;
   int rv = BCM_E_NONE;
   bcm_field_group_t Cint_field_trunk_port_quals_fg_id;
   bcm_field_entry_t Cint_field_trunk_port_quals_entry_handle;

   bcm_field_group_info_t_init(&fg_info);
   bcm_field_group_attach_info_t_init(&attach_info);
   bcm_field_entry_info_t_init(&entry_info);

   fg_info.fg_type = bcmFieldGroupTypeTcam;
   fg_info.stage = stage;
   dest_char = &(fg_info.name[0]);
   sal_strncpy_s(dest_char, "trunk_port_quals", sizeof(fg_info.name));
   fg_info.nof_quals = 1;
   fg_info.nof_actions = 1;
   attach_info.key_info.nof_quals = fg_info.nof_quals;
   attach_info.payload_info.nof_actions = fg_info.nof_actions;
   entry_info.nof_entry_quals = fg_info.nof_quals;
   entry_info.nof_entry_actions = fg_info.nof_actions;
   entry_info.core = core;

   fg_info.qual_types[0] = qual_type;
   attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
   attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
   attach_info.key_info.qual_info[0].input_arg = 0;
   attach_info.key_info.qual_info[0].offset = 0;
   entry_info.entry_qual[0].type = fg_info.qual_types[0];
   entry_info.entry_qual[0].value[0] = trunk_gport; // Ths basicall accepts trunk gport
   entry_info.entry_qual[0].mask[0] = qual_mask;

   fg_info.action_types[0] = bcmFieldActionPrioIntNew;
   attach_info.payload_info.action_types[0] = fg_info.action_types[0];
   entry_info.entry_action[0].type = fg_info.action_types[0];
   entry_info.entry_action[0].value[0] = 6; //Cint_field_trunk_port_quals_tc_action_value;

   /** Create the field group. */
   rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info,
          &Cint_field_trunk_port_quals_fg_id);
   printf("Field Group ID %d was created. \n", Cint_field_trunk_port_quals_fg_id);

   /** Attach the created field group to the context. */
   rv = bcm_field_group_context_attach(unit, 0, Cint_field_trunk_port_quals_fg_id,
         context_id, &attach_info);
   printf("Field Group ID %d was attached to Context ID %d. \n",
    Cint_field_trunk_port_quals_fg_id, context_id);

   /** Add an entry to the created field group. */
   rv = bcm_field_entry_add(unit, 0, Cint_field_trunk_port_quals_fg_id, &entry_info,
          &Cint_field_trunk_port_quals_entry_handle);
   printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n",
    Cint_field_trunk_port_quals_entry_handle,
          Cint_field_trunk_port_quals_entry_handle, Cint_field_trunk_port_quals_fg_id);

   return rv;
}

// Add ingress acl for trunk using inport qual
int add_trunk_inport_tc_rule( int tid, int vsi_id )
{
 int rv = BCM_E_NONE;
 bcm_gport_t tgport;
 BCM_TRUNK_ID_SET(tid, 0, tid);
 BCM_GPORT_TRUNK_SET(tgport, tid);

 printf( " gport passed to the acl rule is %d\n", tgport );
 rv = cint_field_trunk_qual_tc_rule(core,
           bcmFieldStageIngressPMF1,
           ingress_context_id, bcmFieldQualifyInPort,
           0x1FF, tgport, vsi_id);
 return rv;
}

// Add egress acl for trunk using dstport qual
// **IMPORTANT , we have to adapt the qual mask to take trunk port into account
int add_trunk_dstport_tc_rule( int tid, int vsi_id )
{
 int rv = BCM_E_NONE;
 bcm_gport_t tgport;
 BCM_TRUNK_ID_SET(tid, 0, tid);
 BCM_GPORT_TRUNK_SET(tgport, tid);

 printf( " gport passed to the acl rule is %d\n", tgport );
 rv = cint_field_trunk_qual_tc_rule(core,bcmFieldStageEgress,
            egress_context_id, bcmFieldQualifyDstPort,
            0xF0FF, tgport, vsi_id);
 return rv;
}

// API to add a new local port as part of trunk port
int
add_trunk_member( int tid, int local_port )
{

 int rv;
 bcm_trunk_member_t member;

 /*
  *  bcm_trunk_member_add adds member_array to an existing tid.
  *  - multiple instances of a same port is allowed (to change ballance load)
  *  - same port cannot be member of different tid's
    */
 BCM_GPORT_SYSTEM_PORT_ID_SET(member.gport, local_port);
 BCM_TRUNK_ID_SET(tid, 0, tid);
 rv =  bcm_trunk_member_add(unit,tid,&member);
 return rv;
}

// API to add a delete a member link from trunk
int
delete_trunk_member( int tid, int local_port )
{

 int rv;
 bcm_trunk_member_t member;

 BCM_GPORT_SYSTEM_PORT_ID_SET(member.gport, local_port);
 BCM_TRUNK_ID_SET(tid, 0, tid);
 rv =  bcm_trunk_member_delete(unit,tid,&member);
 return rv;
}

// ===========================================
//    L3 related Apis
// ===========================================


// Now lets look at L3 configureations that are needed
// This API create a L3 subinterface on the provided gport,
// called from create_trunk_subinterface_and_nh  and create_port_subinterface_and_nh
int create_subinterface( bcm_gport_t gport, int vlan_id, int vsi_id, int lif_id, int my_mac )
{
 uint32_t v4_enable = 0, v6_enable = 0, stnid = 0;
 bcm_l3_intf_t l3_intf;
 bcm_l3_ingress_t l3ing;
 ret_code_e rv;
 int bcm_rv;
 char *if_name;
 ret_code_e rc;
 bcm_if_t l3a_intf_id;
 int i=0;
 uint8_t mac_addr_bytes[IEEE_MACLEN];
 for(  i=0;i<6;i++ ){
   mac_addr_bytes[i]=0;
 }
 mac_addr_bytes[4] = my_mac;  // self mac address used for ip termination

 printf( " Adding gport to subinterface\n" );
 bcm_vswitch_create_with_id(unit, vsi_id);
 // AMIT where do we do this in case of arrcus code ??
 bcm_vlan_create(unit, vlan_id);
 set_tpid( gport);
 add_gport_to_vlan( gport, vlan_id, vsi_id, 1, lif_id );      // 1 says its a tagged port

 BCM_L3_ITF_SET(l3a_intf_id, BCM_L3_ITF_TYPE_RIF,  vsi_id);
 bcm_l3_intf_t_init(&l3_intf);
 l3_intf.l3a_intf_id = l3a_intf_id;
 l3_intf.l3a_vid = l3_intf.l3a_intf_id ;
 l3_intf.l3a_flags = BCM_L3_WITH_ID;
 l3_intf.l3a_mtu = 1524;  // MTU value
 l3_intf.l3a_vrf = vrf;     // Vrf value
 memcpy(l3_intf.l3a_mac_addr,mac_addr_bytes , IEEE_MACLEN); // Add the my_mac address here
 bcm_rv = bcm_l3_intf_create(unit, &l3_intf);

 bcm_l3_ingress_t_init(&l3ing);
 l3ing.vrf = vrf;
 l3ing.flags = BCM_L3_INGRESS_WITH_ID  ;
 l3ing.urpf_mode = bcmL3IngressUrpfDisable;;
 bcm_rv = bcm_l3_ingress_create(unit, &l3ing, &l3_intf.l3a_intf_id);
 printf("created ingress interface = 0x%08x, on vlan = %d in unit %d, vrf = %d\n",vsi_id,vlan_id,unit,vrf);

 return l3_intf.l3a_intf_id;;

}

// returns fec id for the created nh, that is used for adding a route pointing to that fec
int create_subinterface_and_nh( bcm_gport_t gport, int vlan_id, int vsi_id, int lif_id, int my_mac, int nhmac )
{
 // First create an L3 sub interface
 int l3if  = create_subinterface(gport, vlan_id, vsi_id, lif_id, my_mac );

 // Create egress nexhop for packet going out of this subinterface
 bcm_mac_t nh_mac = {0x0, 0x0, 0x0, 0x0, 0x0, nhmac};
 bcm_if_t dummy;
 bcm_l3_egress_t l3eg;
 bcm_if_t encap_id;

 bcm_l3_egress_t_init(&l3eg);
 sal_memcpy(&l3eg.mac_addr, nh_mac, 6);
 print bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &dummy);
 encap_id = l3eg.encap_id;
 printf("Created l3 egr nh %d\n", encap_id);


 bcm_if_t fec_id;
 bcm_l3_egress_t_init(&l3eg);
 BCM_L3_ITF_SET(l3eg.intf, BCM_L3_ITF_TYPE_RIF, l3if);
 l3eg.port = gport;
 l3eg.encap_id = encap_id;
 print bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec_id);
 printf("Created ing nh %d\n", fec_id);

 return fec_id;

}
// This is the api used to create a subinterface in a trunk ( tid ), We provide the
// vlan tag information, VSI and the LIF. along with the my_mac and a nexthop mac used by route
// using this subinterface
int create_trunk_subinterface_and_nh( int tid, int vlan_id, int vsi_id, int lif_id, int my_mac, int nh_mac )
{
 bcm_gport_t tgport;
 BCM_TRUNK_ID_SET(tid, 0, tid);
 BCM_GPORT_TRUNK_SET(tgport, tid);
 return create_subinterface_and_nh( tgport, vlan_id, vsi_id, lif_id, my_mac, nh_mac);
}
// Similar as above but is used or local ports
int create_port_subinterface_and_nh( int port, int vlan_id, int vsi_id, int lif_id, int my_mac, int nh_mac )
{
 bcm_gport_t gport;
 BCM_GPORT_SYSTEM_PORT_ID_SET( gport, port );
 return create_subinterface_and_nh( gport, vlan_id, vsi_id, lif_id, my_mac, nh_mac);
}

// Adds a route for a given fec
int add_ip_route( uint32 pfx, uint32 mask, bcm_if_t fec ){
 bcm_l3_route_t l3rt;
 bcm_l3_route_t_init(&l3rt);
 l3rt.l3a_subnet = pfx;
 l3rt.l3a_ip_mask = mask;
 l3rt.l3a_intf = fec;
 l3rt.l3a_vrf = vrf;
 print bcm_l3_route_add(unit, &l3rt);
 return 0;
}
/* end */
