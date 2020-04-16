/*
 * vlan_translations.c
 *
 *  Created on: Nov 20, 2018
 *      Author: Csaba
 *  cint version of the EVE commands, based on bcm_sdk_ave_vlan.c
 *  Target: Jerico 2
 */
/*
 * tag format class IDs to be defined for a VLAN port
 * a packet on a port may resolve to one of these
 * tag format class as defined, aka, derived from
 * the packet based on port TPID profile
 */
/* TODO: Must be created globally. Looks like appl_dnx_vlan_llvp_init_egress()
 * from src/appl/reference/dnx/appl_ref_vlan_init.c does it.
 */
const int TAG_FORMAT_CLASS_ID_NO_TAG = 0; //DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE, present by default
const int TAG_FORMAT_CLASS_ID_SINGLE_TAG = 2; //same as J+ or use 4 after ref init,
const int TAG_FORMAT_CLASS_ID_SINGLE_CTAG = 3;
//TODO find out what is run from ref init, contor it, e.g., TPIDs are not needed

/*
 * Egress VLAN edit class that is resolved from the out-LIF
 */
const int EGRESS_EDIT_CLASS_UNTAGGED = 8;
const int EGRESS_EDIT_CLASS_TAGGED = 10;
const int INGRESS_EDIT_CLASS_TAGGED = 11;
const int INGRESS_EDIT_CLASS_UNTAGGED = 12;
/*
 * EVE actions for tagged and untagged ports.
 * same IDs also used as Egress Vlan Edit Class IDs
 * for command resolution
 */

const int EVE_SINGLE_TAG_TO_NO_TAG = 18;
const int EVE_SINGLE_TAG_NOOP = 19;
const int EVE_NO_TAG_TO_SINGLE_TAG = 20;
const int EVE_NO_TAG_NOOP = 21;
const int IVE_SINGLE_TAG_NOOP = 22;
const int IVE_SINGLE_TAG_NO_TAG = 23;
const int IVE_NOOP = IVE_SINGLE_TAG_NOOP;
/*
 * setup Egress VLAN Edit profile for EGRESS_EDIT_CLASS_UNTAGGED
 *
 * used for egress via port default out-LIF for untagged ports.
 *
 * Two command actions are defined for this edit class:
 *
 * key(EDIT_CLASS_UNTAGGED, TAG_FORMAT_UNTAGGED) => action-id(EVE_NO_TAG_NOOP)
 * key(EDIT_CLASS_UNTAGGED, TAG_FORMAT_TAGGED) => action-id(EVE_SINGLE_TAG_TO_NO_TAG)
 */
ret_code_e
bcm_dnx_sdk_eve_untagged_init (int unit)
{
  int                               rv;
  int                               flags = 0;
  int                               action_id = 0;
  bcm_vlan_action_set_t             action;
  bcm_vlan_translate_action_class_t action_class;

  /*
   * action-id EVE_NO_TAG_NOOP
   * allocate action ID and set actions ID commands (EVE commands table)
   */
  flags = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
  action_id = EVE_NO_TAG_NOOP;
  rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to create eve-untagged-action-id-none : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  flags = BCM_VLAN_ACTION_SET_EGRESS;
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionNone;
  action.flags |= BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED;
  rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set eve-untagged-action-id-none : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  /*
   * action-id EVE_SINGLE_TAG_TO_NO_TAG
   * allocate action ID and set actions ID commands (EVE commands table)
   */
  flags = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
  action_id = EVE_SINGLE_TAG_TO_NO_TAG;
  rv =  bcm_vlan_translate_action_id_create(unit, flags, &action_id);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to create eve-untagged-action-id-del : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  flags = BCM_VLAN_ACTION_SET_EGRESS;
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionDelete;
  action.outer_tpid = 0x8100; //NOTE it shoud not make much difference
  action.flags |= BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED;
  rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set eve-untagged-action-id-del : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  /*
   * setup command resolution for this edit class / profile to above actions:
   *
   * command resolution key = edit_class (from outLIF) + tpid_class (from post-IVE packet parser)
   * command reslution result = action-id
   *
   * (EDIT_CLASS_UNTAGGED, TAG_FORMAT_UNTAGGED) => (EVE_NO_TAG_NOOP)
   * (EDIT_CLASS_UNTAGGED, TAG_FORMAT_TAGGED) => (EVE_SINGLE_TAG_TO_NO_TAG)
   *
   * out-LIF for untagged ports is associated with this edit class for command resoltion
   */

  bcm_vlan_translate_action_class_t_init(&action_class);
  action_class.vlan_edit_class_id = EGRESS_EDIT_CLASS_UNTAGGED;
  action_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_NO_TAG;
  action_class.vlan_translation_action_id = EVE_NO_TAG_NOOP;
  action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
  rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set eve-untagged-action-class-none : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  bcm_vlan_translate_action_class_t_init(&action_class);
  action_class.vlan_edit_class_id = EGRESS_EDIT_CLASS_UNTAGGED;
  action_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_TAG;
  action_class.vlan_translation_action_id = EVE_SINGLE_TAG_TO_NO_TAG;
  action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
  rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set eve-untagged-action-class-del : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  bcm_vlan_translate_action_class_t_init(&action_class);
  action_class.vlan_edit_class_id = EGRESS_EDIT_CLASS_UNTAGGED;
  action_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_CTAG;
  action_class.vlan_translation_action_id = EVE_SINGLE_TAG_TO_NO_TAG;
  action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
  rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set eve-untagged-action-class-del(SYM) : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
  LOG_ERR_MSG("DNX-SDK", "EVE class setup done for untagged ports, unit=%d\n", "");

  bcm_vlan_translate_action_class_t_init(&action_class);
  action_class.vlan_edit_class_id = INGRESS_EDIT_CLASS_UNTAGGED;
  action_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_NO_TAG;
  action_class.vlan_translation_action_id = IVE_NOOP;
  action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
  rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set ive-untagged-action-class-nop(SYM) : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
  LOG_ERR_MSG("DNX-SDK", "EVE class setup done for untagged ports, unit=%d\n", "");

  return (RET_CODE_SUCCESS);
}

/*
 * setup Egress VLAN Edit profile for EGRESS_EDIT_CLASS_TAGGED
 *
 * used for egress via port default out-LIF for tagged ports.
 *
 * Two command actions are defined for this edit class:
 *
 * key(EDIT_CLASS_TAGGED, TAG_FORMAT_TAGGED) => action-id(EVE_SINGLE_TAG_NOOP)
 * key(EDIT_CLASS_TAGGED, TAG_FORMAT_UNTAGGED) => action-id(EVE_NO_TAG_TO_SINGLE_TAG)
 */
ret_code_e
bcm_dnx_sdk_eve_tagged_init (int unit)
{
  int                               rv;
  int                               flags = 0;
  int                               action_id = 0;
  bcm_vlan_action_set_t             action;
  bcm_vlan_translate_action_class_t action_class;

  /*
   * action-id EVE_SINGLE_TAG_NOOP
   * allocate action ID and set actions ID commands (EVE commands table)
   */
  flags = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
  action_id = EVE_SINGLE_TAG_NOOP;
  rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to create eve-tagged-action-id-none : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
  //dnx_vlan_translate_action_id_set_verify() hints about used fields, dt_...
  flags = BCM_VLAN_ACTION_SET_EGRESS;
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionNone;
  action.outer_tpid = 0x8100;
  //NOTE force to 8100, not needed when only 8100 TPID is allowed
  //action.outer_tpid_action = bcmVlanTpidActionModify;
  rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set eve-tagged-action-id-none : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
  /*
   * action-id IVE_SINGLE_TAG_TO_NO_TAG
   * allocate action ID and set actions ID commands (EVE commands table)
   */
  flags = BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
  action_id = IVE_SINGLE_TAG_NO_TAG;
  rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to create ive-tagged-action-id-strip : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
  //dnx_vlan_translate_action_id_set_verify() hints about used fields, dt_...
  flags = BCM_VLAN_ACTION_SET_INGRESS;
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionDelete;
  action.outer_tpid = 0x8100;
  //NOTE force to 8100, not needed when only 8100 TPID is allowed
  //action.outer_tpid_action = bcmVlanTpidActionModify;
  rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set ive-tagged-action-id-strip : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
  /*
   * action-id IVE_SINGLE_TAG_NOOP
   * allocate action ID and set actions ID commands (EVE commands table)
   */
  flags = BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
  action_id = IVE_SINGLE_TAG_NOOP;
  rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to create ive-tagged-action-id-none : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
  //dnx_vlan_translate_action_id_set_verify() hints about used fields, dt_...
  flags = BCM_VLAN_ACTION_SET_INGRESS;
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionNone;
  action.outer_tpid = 0x8100;
  //NOTE force to 8100, not needed when only 8100 TPID is allowed
  //action.outer_tpid_action = bcmVlanTpidActionModify;
  rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set ive-tagged-action-id-none : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
  /*
   * action-id EVE_NO_TAG_TO_SINGLE_TAG
   * allocate action ID and set actions ID commands (EVE commands table)
   */
  flags = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
  action_id = EVE_NO_TAG_TO_SINGLE_TAG;
  rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to create eve-tagged-action-id-add : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  flags = BCM_VLAN_ACTION_SET_EGRESS;
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionAdd;
  action.outer_tpid = 0x8100;
  //NOTE propagate(?) PCP and not pick up garbage
  //TODO figure out how to propagate DSCP(?) dnx_vlan_translate_pcp_dei_action_to_dnx()
  action.dt_outer_pkt_prio = bcmVlanActionAdd;
  rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set eve-tagged-action-id-add : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  /*
   * setup command resolution for this edit class / profile to above actions:
   *
   * command resolution key = edit_class (from outLIF) + tpid_class (from post-IVE packet parser)
   * command reslution result = action-id
   *
   * (EDIT_CLASS_TAGGED, TAG_FORMAT_TAGGED) => (EVE_SINGLE_TAG_NOOP)
   * (EDIT_CLASS_TAGGED, TAG_FORMAT_UNTAGGED) => (EVE_NO_TAG_TO_SINGLE_TAG)
   *
   * out-LIF for untagged ports is associated with this edit class for command resoltion
   */

  bcm_vlan_translate_action_class_t_init(&action_class);
  action_class.vlan_edit_class_id = EGRESS_EDIT_CLASS_TAGGED;
  action_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_TAG;
  action_class.vlan_translation_action_id = EVE_SINGLE_TAG_NOOP;
  action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
  rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set eve-tagged-action-class-none : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
//  bcm_vlan_translate_action_class_t_init(&action_class);
//  action_class.vlan_edit_class_id = INGRESS_EDIT_CLASS_TAGGED;
//  action_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_TAG;
//  action_class.vlan_translation_action_id = IVE_SINGLE_TAG_NOOP;
//  action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
//  rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
//  if (BCM_E_NONE != rv) {
//    LOG_ERR_MSG("DNX-SDK", "Failed to set ive-tagged-action-class-none : %s\n", bcm_errmsg(rv));
//    return (RET_CODE_FAILURE);
//  }
//  bcm_vlan_translate_action_class_t_init(&action_class);
//  action_class.vlan_edit_class_id = INGRESS_EDIT_CLASS_TAGGED;
//  action_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_TAG;
//  action_class.vlan_translation_action_id = IVE_SINGLE_TAG_NO_TAG;
//  action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
//  rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
//  if (BCM_E_NONE != rv) {
//    LOG_ERR_MSG("DNX-SDK", "Failed to set ive-tagged-action-class-none : %s\n", bcm_errmsg(rv));
//    return (RET_CODE_FAILURE);
//  }
  bcm_vlan_translate_action_class_t_init(&action_class);
  action_class.vlan_edit_class_id = EGRESS_EDIT_CLASS_TAGGED;
  action_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_CTAG;
  action_class.vlan_translation_action_id = EVE_SINGLE_TAG_NOOP;
  action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
  rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set eve-tagged-action-class-none(SYM) : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
  bcm_vlan_translate_action_class_t_init(&action_class);
  action_class.vlan_edit_class_id = EGRESS_EDIT_CLASS_TAGGED;
  action_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_NO_TAG;
  action_class.vlan_translation_action_id = EVE_NO_TAG_TO_SINGLE_TAG;
  action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
  rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set eve-tagged-action-class-add : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  LOG_ERR_MSG("DNX-SDK", "EVE class setup done for tagged ports, unit=%d\n", "");

  bcm_vlan_translate_action_class_t_init(&action_class);
  action_class.vlan_edit_class_id = INGRESS_EDIT_CLASS_TAGGED;
  action_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_CTAG;
  action_class.vlan_translation_action_id = IVE_NOOP;
  action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
  rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to set ive-untagged-action-class-nop(SYM) : %s\n", bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }

  return (0);
}

ret_code_e
bcm_dnx_sdk_global_tpid_init(int unit)
{
  int rv;
  bcm_port_tag_format_class_t tag_format_class_id;
  rv = bcm_stk_module_enable(unit,0,-1,0);
  if (rv != BCM_E_NONE) {
      printf("Error, in bcm_stk_module_enable disable \n");
      return RET_CODE_FAILURE;
  }
  rv = bcm_switch_tpid_delete_all(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, in bcm_switch_tpid_delete_all\n");
      return RET_CODE_FAILURE;
  }
  bcm_switch_tpid_info_t tpid_info;
  tpid_info.tpid_value = 0x8100;
  rv = bcm_switch_tpid_add(unit, 0, &tpid_info);
  if (rv != BCM_E_NONE) {
      printf("Error, in bcm_switch_tpid_add(tpid_value=0x%04x)\n", tpid_info.tpid_value);
      return RET_CODE_FAILURE;
  }
  tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_TAG;
  //NOTE does not work with bcmTagStructTypeCTag
  rv = bcm_port_tpid_class_create(unit, BCM_PORT_TPID_CLASS_WITH_ID,
                                 bcmTagStructTypeSTag, &tag_format_class_id);
  if (BCM_E_NONE != rv) {
    rv = bcm_port_tpid_class_destroy(unit, BCM_PORT_TPID_CLASS_WITH_ID,
                                     tag_format_class_id);
    if (BCM_E_NONE != rv) {
      printf("Failed bcm_port_tpid_class_destroy for S-tag : %s\n",
             bcm_errmsg(rv));
      /* retry to capture the real error */
    }
    rv = bcm_port_tpid_class_create(unit, BCM_PORT_TPID_CLASS_WITH_ID,
                                   bcmTagStructTypeSTag, &tag_format_class_id);
    if (BCM_E_NONE != rv) {
      printf("Failed bcm_port_tpid_class_create for S-tag : %s\n", bcm_errmsg(rv));
      return RET_CODE_FAILURE;
    }
  }
  tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_CTAG;
  rv = bcm_port_tpid_class_destroy(unit, BCM_PORT_TPID_CLASS_WITH_ID,
                                   tag_format_class_id);
  if (BCM_E_NONE != rv && BCM_E_NOT_FOUND != rv) {
    printf("Failed bcm_port_tpid_class_destroy for SYM : %s\n",
           bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }
  rv = bcm_port_tpid_class_create(unit, BCM_PORT_TPID_CLASS_WITH_ID,
                                 bcmTagStructTypeCTag, &tag_format_class_id);
  if (BCM_E_NONE != rv) {
    printf("Failed bcm_port_tpid_class_create for C-tag(SYM) : %s\n", bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }
  rv = bcm_stk_module_enable(unit,0,-1,1);
  if (rv != BCM_E_NONE) {
      printf("Error, in bcm_stk_module_enable enable \n");
      return RET_CODE_FAILURE;
  }
  return RET_CODE_SUCCESS;
}
/*
 * setup Egress VLAN Edit profiles at init
 */
ret_code_e
bcm_dnx_sdk_eve_init(int unit)
{
  int rv;
  rv = bcm_dnx_sdk_global_tpid_init(unit);
  if (RET_CODE_SUCCESS != rv) {
    printf("Failed tpid init\n");
    return (rv);
  }
  rv = bcm_dnx_sdk_eve_untagged_init(unit);
  if (RET_CODE_SUCCESS != rv) {
    printf("Failed eve-untagged-init\n");
    return (rv);
  }

  rv = bcm_dnx_sdk_eve_tagged_init(unit);
  if (RET_CODE_SUCCESS != rv) {
    printf("Failed eve-tagged-init\n");
    return (rv);
  }

  return (rv);
}
int tpids_assign(int unit) {
    int rv;
   rv = bcm_stk_module_enable(unit,0,-1,0);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_stk_module_enable disable \n");
       return rv;
   }
   rv = bcm_switch_tpid_delete_all(unit);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_switch_tpid_delete_all\n");
       return rv;
   }

   bcm_switch_tpid_info_t tpid_info;

   /* TPID0 */
   tpid_info.tpid_value = 0x8100;
   rv = bcm_switch_tpid_add(unit, 0, &tpid_info);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_switch_tpid_add(tpid_value=0x%04x)\n", tpid_info.tpid_value);
       return rv;
   }
   rv = bcm_stk_module_enable(unit,0,-1,1);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_stk_module_enable enable \n");
       return rv;
   }
}
ret_code_e
bcm_dnx_sdk_ave_vlan_init(int unit)
{
  int rv;
  bcm_vlan_t sdk_default_vid = 1;

  /* Sdk creates vlan 1 and adds all ports as members, undo this */
  rv = bcm_vlan_destroy(unit, sdk_default_vid);
  if (BCM_E_NONE != rv) {
    LOG_ERR_MSG("DNX-SDK", "Failed to destroy SDK created default vlan 1\n",
                "");
    return RET_CODE_FAILURE;
  }
  switch (use_case) {
  case 2:
    break;
  default:
    /*
     * setup egress vlan editing commands
     */
    rv = bcm_dnx_sdk_eve_init(unit);
    if (RET_CODE_SUCCESS != rv) {
      LOG_ERR_MSG("DNX-SDK", "Failed to init EVE commands\n", "");
      return (RET_CODE_FAILURE);
    }
  }
  return (RET_CODE_SUCCESS);
}
