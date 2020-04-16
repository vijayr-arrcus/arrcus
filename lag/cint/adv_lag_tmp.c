 /*
 * adv_vlan_with_trunk.c
 *
 *      Author: Amit
 *  VLAN programming, cint version
 *  Target: Jerico 2
 *  Depends: vlan_translations.c, global.c
 *
 *
 *

cd ../../../../../cint/advvlan/
cint globals.c
cint vlan_translations.c

cint adv_lag.c

cint
print bcm_dnx_sdk_l2_init(0);
print bcm_dnx_sdk_ave_vlan_init(0);

// Check vlan show -- Previously no output was seen but now it will be seen.

// create trunk
main_trunk( 13,14,100, 500 );
// main_trunk(mem1, mem2, tid, Vlan_domain);
// VAR Is there any LAG group table.
// VAR Check the vlan domain (dump chg IPPE_PP_PORT_TO_VD)
// VAR we are adding only the trunk to the Vlan Domain ?
// The member links will inherit this property but what about the port 15, 16
// VAR if we dont add to the vlan domain (sig get name=vlan_domain)


// create vlan and vsi
create_vlan_vsi( 50,1050 );
// create_vlan_vsi(vlan, vsi);
// VAR is this just global VLAN/VSI creation and no real show commands exists ?

add_trunk_to_vlan( 100, 50, 1050, 1, 600);
//add_trunk_to_vlan( int tid, int vlan_id, int vsi_id, int tagged, int lif_id )


add_port_to_vlan( 15, 50, 1050, 1, 601);
add_port_to_vlan( 16, 50, 1050, 1, 602);


// Add mac address for the ports
add_mac_trunk( 100, 1050, 100 );
add_mac_port( 15, 1050, 15 );
add_mac_port( 16, 1050, 16 );
// Above is all desintation mac address, suggesting that the particular MAC address is connected/learnt on this port.;

exit;

// Otput of following tables shows the added interfaces as well
DBaL TaBLe dump TaBLe=ESEM_FORWARD_DOMAIN_MAPPING_DB
dbal taBLe duMP taBLe=IN_AC_S_VLAN
dbal taBLe duMP taBLe=EEDB_OUT_AC
vlan show
l2 show

// Show commands to see packet in action.
// sig, vsi
Now lets send some packets

* tagged packet from 14( trunk ) to 15 ( egresses on 15 )
tx 1 psrc=14 data=0x00000000000f00000000000781000032080045000046000100004011f66021212103212121010d050cfb00326447616161616161616161616161616161616161616161616161616161616161616161616161616161616161

* untagged packet from 14( trunk ) to 15 ( gets dropped because of vlan membership )
tx 1 psrc=15 data=0x00000000000f000000000007080045000046000100004011f66021212103212121010d050cfb003264476161616161616161616161616161616161616161616161616161616161616161616161616161616161

* tagged packet from 15 to trunk ( tagged packet goes out of the trunk )
tx 1 psrc=15 data=0x00000000006400000000000781000032080045000046000100004011f66021212103212121010d050cfb00326447616161616161616161616161616161616161616161616161616161616161616161616161616161616161

* untagged packet from 15 to trunk ( gets dropped because of vlan membership )
tx 1 psrc=15 data=0x00000000000f000000000007080045000046000100004011f66021212103212121010d050cfb003264476161616161616161616161616161616161616161616161616161616161616161616161616161616161


field ent info grp=8 ent=0x50


// TODO Amit
// 1. Add acls for trunk port in l2
// 1. Handle split horizon for lag, and test it
//

 */

int unit = 0;
int core = 0;  // TODO_AMIT we need to handle multiple cores
bcm_field_context_t ingress_context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
bcm_field_context_t egress_context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;

int trunk_member_add(bcm_trunk_t tid, int port)
{
  bcm_port_resource_t resource;
  bcm_trunk_member_t  member;
  bcm_trunk_info_t    trunk_info;
  int rv;
  bcm_trunk_member_t_init(&member);
  /* Prepare local ports to gports */
  BCM_GPORT_SYSTEM_PORT_ID_SET(member.gport, port);
  BCM_TRUNK_ID_SET(tid, 0, tid);

  /* Adds members (in member_array) to trunk and activates trunk mechanism */
  rv = bcm_trunk_member_add(unit, tid, &member);
  bcm_port_interface_info_t pi;
  bcm_port_mapping_info_t pm;
  uint32_t flags;
  int tgport;
  BCM_GPORT_TRUNK_SET(tgport, tid);

  pm.core = 1;
  rv = bcm_port_get(unit, tgport, &flags, &pi, &pm);
  printf("trunk set add pm.core %d\n", pm.core);
  bcm_port_resource_get(unit, tgport, &resource);
  printf("\n speed = %d \n", resource.speed);
  bcm_port_resource_get(unit, port, &resource);
  printf("\n speed = %d \n", resource.speed);
}

int trunk_member_del (bcm_trunk_t tid, int port)
{
  bcm_port_resource_t resource;
  bcm_trunk_member_t  member;
  bcm_trunk_info_t    trunk_info;
  int rv;
  /* Prepare local ports to gports */
  bcm_trunk_member_t_init(&member);
  BCM_TRUNK_ID_SET(tid, 0, tid);
  BCM_GPORT_SYSTEM_PORT_ID_SET(member.gport, port);

  bcm_trunk_member_delete(unit, tid, &member);

  bcm_port_interface_info_t pi;
  bcm_port_mapping_info_t pm;
  uint32_t flags;
  int tgport;
  BCM_GPORT_TRUNK_SET(tgport, tid);

  pm.core = 1;
  printf("\n getting port details for trunk");
  rv = bcm_port_get(unit, tgport, &flags, &pi, &pm);
  printf("trunk set add pm.core %d\n", pm.core);
  bcm_port_resource_get(unit, tgport, &resource);
  printf("\n speed = %d \n", resource.speed);
}

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
  // TODO_AMIT
  BCM_TRUNK_ID_SET(tid, 0, tid);

  /* Creates #tid trunk (no memory allocation and no members inside) */
  rv =  bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);

  BCM_GPORT_TRUNK_SET(tgport, tid);
  printf("trunk gport is  create: gport=%d\n", tgport);
  key.index=0;
  key.type=bcmSwitchPortHeaderType;
  value.value=BCM_SWITCH_PORT_HEADER_TYPE_ETH;
  rv = bcm_switch_control_indexed_port_set(0, tgport, key, value);
  // VAR we are going to set the attributes of the LIFs
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
  /*
  // IN case you want to check the pp port values for the port

  bcm_trunk_pp_port_allocation_info_t trunk_pp_port_aloc_info_get;
  bcm_trunk_pp_port_allocation_info_t_init(&trunk_pp_port_aloc_info_get);
  bcm_trunk_pp_port_allocation_get(unit,tid, 0,
				   &trunk_pp_port_aloc_info_get);

  printf("Allocated pp_port1 is %d and pp_port2 is %d. \n",
	 trunk_pp_port_aloc_info_get.pp_port_per_core_array[0],
	 trunk_pp_port_aloc_info_get.pp_port_per_core_array[1]);
  */
  bcm_port_interface_info_t pi;
  bcm_port_mapping_info_t pm;
  uint32_t flags;

  pm.core = 1;
  rv = bcm_port_get(unit, tgport, &flags, &pi, &pm);
  printf("trunk set pm.core %d\n", pm.core);
  return tgport;
}

int
set_tpid( bcm_gport_t gport ){
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
//  tpid_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_TAG;
  tpid_class.vlan_translation_action_id = 0;
  rv = bcm_port_tpid_class_set(unit, &tpid_class);
  // VAR Any show commands to check the TPID configuration for a port 

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

  // VAR not adding the physical ports to the vlan domain ?
  // We are adding them to the same VSI though
  return tgport;
}


// This does what is being done inside
// bcm_dnx_sdk_extract_and_add_port_to_vlan()
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
  // VAR symmetric but flag = 0 ? // This is how arcos does it and so believe that 0 means symmetric or most likely default configuration.

  // set the untagged property for trunk
  if (!tagged) {
    //Note: ingress VLAN miss is drop by default (PG 21.5.2)
    // This sets untagged packets default vlan for the incoming port
    rv = bcm_port_untagged_vlan_set(unit, gport, vlan_id);
    // Init AC and Drop AC
    //
  }


  /*
   * create (p,v) outLIF, for an entry in ESEM_FORWARD_DOMAIN_MAPPING_DB
   */
  // TODO_AMIT somehow we donot need any id for this guy ???
  // Pick up anything egressing on a port x VSI independently of VID.
  bcm_vlan_port_t_init(&vp);
  vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
  vp.port = gport;
  vp.vsi = vsi_id; // VAR VSI Not to be set. need atleast the encap ? since this is egress LIF
  // VAR may be the translation is actually the encap ?
  // this is VLAN translate AC so still required a MAC rewrite ?
  vp.flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
  vp.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
  rv = bcm_vlan_port_create(unit, &vp);
  out_gp = vp.vlan_port_id;
  printf("1-vlan_port create ID %d\n", vp.vlan_port_id);
  // VAR show for egress LIF
  // The inLIF and oLIF are part of the same VSI, it should be fine

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
  // VAR for subinterface there will be another LIF and vlan
  BCM_GPORT_VLAN_PORT_ID_SET(vp.vlan_port_id, vp.vlan_port_id);
  vp.flags = BCM_VLAN_PORT_WITH_ID;
  vp.egress_vlan = vp.match_vlan = vlan_id;
  // Create an LIF for this port vlan
  rv = bcm_vlan_port_create(unit, &vp);
  in_gp = vp.vlan_port_id;
  printf("2-vlan_port create ID %d\n", vp.vlan_port_id);

  // Adds the LIFs to VSI
  rv = bcm_vswitch_port_add(unit, vsi_id, vp.vlan_port_id);
  // VAR check with Csaba
  printf("3-vlan_port create ID %d\n", vp.vlan_port_id);

  /*
   * associate Egress VLAN edit class and VLAN edit
   * parameters to be used for VLAN editing on outLIF for
   * tagged / untagged ports. command key is derived from
   * this edit class plus the packet tag format.
   */
  printf( "setting egress vlan trans\n" );
  // VAR we are going to set the attributes of the LIFs
  bcm_vlan_port_translation_t_init(&vp_trans);
  vp_trans.new_outer_vlan = vlan_id; // VAR Why are we setting out vlan ID as the vlan ID again for in LIF.
  vp_trans.gport = in_gp; // VAR in_gp is symmetric
  if (tagged) {
    vp_trans.vlan_edit_class_id = EGRESS_EDIT_CLASS_TAGGED;
  } else {
    vp_trans.vlan_edit_class_id = EGRESS_EDIT_CLASS_UNTAGGED;
  }
  vp_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;//VAR what does this mean ? The configuration is applicable for the Egress side.
  rv = bcm_vlan_port_translation_set(unit, &vp_trans);

  // Do for out gport as well
  vp_trans.gport = out_gp;
  rv = bcm_vlan_port_translation_set(unit, &vp_trans); // VAR this makes sense to me.


  /* /\* learn enable on port*\/ */
  /* unsigned int f; */
  /* rv = bcm_port_learn_get(unit, gport, &f); */
  /* //NOTE physical AND virtual ports are both need learn enable, BTW it is the default for physical ports */
  /* rv = bcm_port_learn_set(unit, gport, */
  /* 			  BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD); */

  return 0;
}

int
add_mac_trunk( int tid,  uint32_t vsi, char endmac )
{
  bcm_l2_addr_t l2_addr;
  bcm_mac_t mac = {0,0,0,0,0,0}; //dst_mac
  int rv;
  bcm_gport_t tgport;
  mac[5] = endmac;
  bcm_l2_addr_t_init(&l2_addr, mac, vsi); // VAR if its mac learning who provides the VSI
  l2_addr.flags = BCM_L2_STATIC;      /* static entry */ // DYNAMIC ENTRY as above

  // Convert td to trunk gport
  BCM_TRUNK_ID_SET( tid, 0, tid );
  BCM_GPORT_TRUNK_SET(tgport, tid);
  l2_addr.port = tgport;
  rv = bcm_l2_addr_add(unit, &l2_addr);
  return rv;
}


int
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

int
add_port_to_vlan( int port, int vlan_id, int vsi_id, int tagged, int lif_id )
{
  bcm_gport_t gport;
  BCM_GPORT_SYSTEM_PORT_ID_SET( gport, port );
  set_tpid( gport);
  add_gport_to_vlan( gport, vlan_id, vsi_id, tagged, lif_id );
  return 0;
}
int
create_vlan_vsi( int vlan_id, int vsi_id )
{
  // create VSI
  bcm_vswitch_create_with_id(unit, vsi_id);
  //create VLAN
  bcm_vlan_create(unit, vlan_id);
  return 0;
}

// Acl Rule to drop packets coming on trunk port on a specifc VSI
int cint_field_trunk_qual_tc_rule(
    bcm_core_t core,
    bcm_field_stage_t stage, // VAR which stage we are in, PMF1, PMF2, PMF3, EPMF
    bcm_field_context_t context_id, // VAR ingress or egress.
    bcm_field_qualify_t qual_type, // in port or dest port
    uint32 qual_mask, // VAR for the qualifier -- which has defined width -- mask
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
    entry_info.entry_qual[0].value[0] = trunk_gport;
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
