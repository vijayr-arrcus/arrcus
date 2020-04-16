/*
 * adv_vlan.c
 *
 *  Created on: Nov 20, 2018
 *      Author: Csaba
 *  VLAN programming, cint version
 *  Target: Jerico 2
 *  Depends: vlan_translations.c, global.c
 *
cint /home/user/arrcus_sw/dpal/plugin/bcm/dnx/jericho_2/cint/globals.c
cint /home/user/arrcus_sw/dpal/plugin/bcm/dnx/jericho_2/cint/vlan_translations.c
cint /home/user/arrcus_sw/dpal/plugin/bcm/dnx/jericho_2/cint/adv_vlan.c
cint
print bcm_dnx_sdk_ave_vlan_init(0);
print add_vlan(1,50,5000,9000,0);

 *
 *
 */

void
do_simple_add_vlan_nok(int port, int vlan, int tagged, int unit)
{
  bcm_pbmp_t pbmp, upbmp;
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);
  if (tagged) {
    BCM_PBMP_PORT_ADD(pbmp, port);
  } else {
    BCM_PBMP_PORT_ADD(upbmp, port);
    print bcm_port_untagged_vlan_set(unit, port, vlan);
  }
  print bcm_vlan_port_add(unit, vlan, pbmp, upbmp);
  print bcm_dnx_sdk_manage_mcast_group(unit, port, vlan, 1, 0);
  print bcm_port_learn_set(unit, port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
}
void
do_simple_add_vlan(int port, int vlan, int tagged, int unit)
{
  bcm_pbmp_t pbmp, upbmp;
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);
  BCM_PBMP_PORT_ADD(pbmp, port);
  if (!tagged) {
    BCM_PBMP_PORT_ADD(upbmp, port);
  }
  print bcm_port_untagged_vlan_set(unit, port, tagged ? 4095 : vlan);
  print bcm_vlan_port_add(unit, vlan, pbmp, upbmp);
  print bcm_dnx_sdk_manage_mcast_group(unit, port, vlan, 1, 0);
  print bcm_port_learn_set(unit, port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
}
void
simple_add_vlan(int vlan, int untagged, int unit)
{
  print bcm_vlan_create(unit, vlan);
  int flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
  print bcm_multicast_create(unit, flags, &vlan);
  do_simple_add_vlan(1, vlan, 1, unit);
  do_simple_add_vlan(p13, vlan, 1, unit);
  do_simple_add_vlan(untagged, vlan, 0, unit);
  print add_mac(1, vlan, 1, unit);
  print add_mac(p13, vlan, 2, unit);
  print add_mac(untagged, vlan, 3, unit);
}

//NOTE does not make any difference, OLP distributer is the default
int
bcm_dnx_sdk_vsi_learn_distribution(int unit, uint32_t vlan)
{
  int rv;
  bcm_l2_addr_distribute_t distribution;
  bcm_l2_addr_distribute_t_init(&distribution);
  distribution.vid = vlan;

  distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT |
                           BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER;

  if (1 /*aging_profiles_bug*/)
  {
      distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER;

      rv = bcm_l2_addr_msg_distribute_set(unit, &distribution);
      if (rv != BCM_E_NONE) {
          print rv;
          printf("Error, bcm_l2_addr_msg_distribute_set \n");
          return rv;
      }

      distribution.vid = -1;
      distribution.flags = BCM_L2_ADDR_DIST_AGED_OUT_EVENT | BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER;
  }

  rv = bcm_l2_addr_msg_distribute_set(unit, &distribution);
  if (rv != BCM_E_NONE) {
      print rv;
      printf("Error, bcm_l2_addr_msg_distribute_set \n");
      return rv;
  }

  return BCM_E_NONE;
}

int l2_basic_bridge_run(int port, int lif_id,
    uint32_t vlan_id, uint32_t vsi_id,
    int mcid, int tagged,
    uint32_t *in_gp, uint32_t *out_gp,
    int unit)
{
  int rv;
  int i;
  /* Set vlan domain:*/
  rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_port_class_set(port = %d)\n", port);
    return rv;
  }

  /*
   * Set VLAN port membership
   */
  bcm_pbmp_t pbmp, untag_pbmp;

  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(untag_pbmp);
  if (!tagged) {
    BCM_PBMP_PORT_ADD(untag_pbmp, port);
  }
  BCM_PBMP_PORT_ADD(pbmp, port);

  rv = bcm_vlan_port_add(unit, vsi_id, pbmp, untag_pbmp);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_port_add\n");
    return rv;
  }
  rv = bcm_vlan_gport_add(unit, vlan_id, port, 0);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_gport_add\n");
    return rv;
  }
  if (!tagged) {
    rv = bcm_port_untagged_vlan_set(unit, port, vlan_id);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_port_untagged_vlan_set\n");
      return rv;
    }
  }

  /* Create Outlif in EEDB */
  bcm_gport_t outlif;
  outlif = (17 << 26 | 2 << 22 | (10000 + lif_id));

  bcm_vlan_port_t vlan_port;
  bcm_vlan_port_t_init(&vlan_port);

  vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
  vlan_port.vsi = 0;
  if (tagged) {
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
  } else {
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;
  }
  vlan_port.match_vlan = vlan_id;
  vlan_port.port = port;
  vlan_port.vlan_port_id = outlif;
  rv = bcm_vlan_port_create(unit, &vlan_port);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
    return rv;
  }

  *out_gp = vlan_port.vlan_port_id;
  printf("OutLif create: port=%d, vlan_port_id=0x%08x,\n", vlan_port.port,
         vlan_port.vlan_port_id);

  rv = bcm_vswitch_port_add(unit, vsi_id, *out_gp);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vswitch_port_add(port = %d)\n", *out_gp);
    return rv;
  }

  bcm_port_tpid_class_t port_tpid_class;
  bcm_port_tpid_class_t_init(&port_tpid_class);

  port_tpid_class.tpid1 = 0x8100;
  port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
  //port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_C;
  port_tpid_class.port = port;
  port_tpid_class.tag_format_class_id = 4;

  rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_port_tpid_class_set( oPort )\n");
    return rv;
  }

  rv = vlan_translate_ive_eve_translation_set_with_tpid_action(
                                                  unit,
                                                  *out_gp,                                   /* outlif                  */
                                                  0,                              /* outer_tpid              */
                                                  0,                              /* inner_tpid              */
                                                  bcmVlanTpidActionNone,    /* Outer tpid action modify*/
                                                  bcmVlanTpidActionNone,    /* Inner tpid action modify*/
                                                  bcmVlanActionDelete,         /* outer_action            */
                                                  bcmVlanActionDelete,         /* inner_action            */
                                                  0,                                           /* new_outer_vid           */
                                                  0,                                           /* new_inner_vid           */
                                                  5,    /* vlan_edit_profile       */
                                                  4,    /* tag_format              */
                                                  FALSE                                                       /* is_ive                  */
                                                  );
  if (rv != BCM_E_NONE) {
      printf("Error, in vlan_translate_ive_eve_translation_set_with_tpid_action(setting 0Port%d EVE action\n", g_l2_basic_bridge.out_port);
      return rv;
  }
  bcm_vlan_control_vlan_t control;

  control.flags2 = 0;
  control.forwarding_vlan = vsi_id;
  control.unknown_unicast_group = vsi_id;
  control.broadcast_group = vsi_id;
  control.unknown_multicast_group = vsi_id;
  rv = bcm_vlan_control_vlan_set(unit, vsi_id, control);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_control_vlan_set");
    return rv;
  }
  return rv;
}

ret_code_e
bcm_dnx_sdk_extract_and_add_port_to_vlan_sym (int port, int lif_id,
                              uint32_t vlan_id, uint32_t vsi_id,
                              int mcid, int tagged,
                              uint32_t *in_gp, uint32_t *out_gp,
                              int unit)
{
  int rv;
  uint32_t ut = unit;
  bcm_gport_t sp, gp;
  bcm_port_tpid_class_t tpid_class;
  bcm_vlan_port_translation_t vp_trans;
  bcm_port_match_info_t port_match;
  bcm_vlan_port_t vp;
  /*
   * Retrieve modport
   */
  rv = bcm_dnx_get_sys_mod_port(unit, port, &gp, &sp);
  if (BCM_E_NONE != rv) {
    printf("Failed to retrieve port mapping for unit %d:%s\n ", unit,
           bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }
  /*
   * local port class, TPID
   */
  if (ut == unit) {
    /*
     * set port class for VLAN domain assignment
     */
    rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
    if (BCM_E_NONE != rv) {
      printf("Failed to set port %d:%s\n", port, bcm_errmsg(rv));
      return RET_CODE_FAILURE;
    }
//    bcm_pbmp_t pbmp, upbmp;
//    BCM_PBMP_CLEAR(pbmp);
//    BCM_PBMP_CLEAR(upbmp);
//    if (tagged) {
//      BCM_PBMP_PORT_ADD(pbmp, port);
//    } else {
//      BCM_PBMP_PORT_ADD(upbmp, port);
//    }
//    rv = bcm_vlan_port_add(unit, vlan_id, pbmp, upbmp);
//    if (rv != BCM_E_NONE) {
//        printf("Error, in bcm_vlan_port_add\n");
//        return rv;
//    }
    rv = bcm_vlan_gport_add(unit, vlan_id, sp,
                            tagged ? 0 : BCM_VLAN_GPORT_ADD_UNTAGGED);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_vlan_gport_add gp:%x VLAN:%d\n", gp, vlan_id);
      return rv;
    }
    if (!tagged) {
      rv = bcm_port_untagged_vlan_set(unit, port, vlan_id);
      if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
      }
    }
    if (use_tpid_class) {
      /*
       * define tag format class ID 2 for single tagged packets
       * on this port - this is output from parser and input to
       * resolve IVE / EVE command class ID to be applied to the
       * packet.
       */
      bcm_port_tpid_class_t_init(&tpid_class);
      tpid_class.port = port;
      tpid_class.tpid1 = 0x8100;
      //TODO how to get rid of anything not 8100 single tagged, _INVALID is not enough
      //9100:VLAN,8100:anything is picked up here as VLAN and passed to untagged port as-is
      tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
      tpid_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_TAG;
      tpid_class.vlan_translation_action_id = 0;
      //NOTE filter unwanted traffic
      tpid_class.flags =
          BCM_PORT_TPID_CLASS_EGRESS_ONLY /*| BCM_PORT_TPID_CLASS_OUTER_C | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO*/;
      rv = bcm_port_tpid_class_set(unit, &tpid_class);
      if (BCM_E_NONE != rv) {
        printf("Failed to set egress tagged tpid-class for port %d: %s\n", port,
               bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
      //TODO a per port way to drop unwanted traffic,
      //higher level logic is needed for trunk ports which
      //might need to take tagged and untagged at the same port
      tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY /*| BCM_PORT_TPID_CLASS_OUTER_C*/;
      if (!tagged) {
        //NOTE otherwise tagged traffic on untagged port is considered as default VLAN
        tpid_class.flags |= BCM_PORT_TPID_CLASS_DISCARD;
      }
      rv = bcm_port_tpid_class_set(unit, &tpid_class);
      if (BCM_E_NONE != rv) {
        printf("Failed to set ingress tagged tpid-class for port %d: %s\n",
               port, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
      /*
       * define tag format class ID 0 for untagged packets
       * egress to this port - this is output from parser and input to
       * resolve IVE / EVE command class ID to be applied to the
       * packet.
       */
      bcm_port_tpid_class_t_init(&tpid_class);
      tpid_class.port = port;
      tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
      tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
      tpid_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_NO_TAG;
      tpid_class.vlan_translation_action_id = 0;
      //NOTE filter unwanted traffic
      tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
      rv = bcm_port_tpid_class_set(unit, &tpid_class);
      if (BCM_E_NONE != rv) {
        printf("Failed to set egress untagged tpid-class for port %d: %s\n",
               port, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
      //TODO a per port way to drop unwanted traffic...
      tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
      if (tagged) {
        tpid_class.flags |= BCM_PORT_TPID_CLASS_DISCARD;
      }
      rv = bcm_port_tpid_class_set(unit, &tpid_class);
      if (BCM_E_NONE != rv) {
        printf("Failed to set ingress untagged tpid-class for port %d: %s\n",
               port, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
    }
    /* Interface is local to this unit, set pvid + knet filter handling */
    if (!tagged) {
      rv = bcm_port_untagged_vlan_set(unit, port, vlan_id);
      //TODO: ensure this is always called for a port. By default untagged==1 (PG 21.5.2)
      //Note: ingress VLAN miss is drop by default (PG 21.5.2)
      //TODO seems to be punt to RP
      if (BCM_E_NONE != rv) {
        printf("Failed to set pvid for port %d:%s\n", port, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
      //TBD : Knet filter handling
    }
  }
  if (use_outlif) {
    /*
     * create (p,v) Symmetric AC LIF
     * InLIF will cause: Cannot commit entry - already exists in table IN_AC_S_VLAN_DB
     *
     */
    //NOTE this VLAN gport will be added to all MACTs
    bcm_vlan_port_t_init(&vp);
    if (tagged) {
      vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    } else {
      vp.criteria = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;
      //NOTE: Error, Unavailable criteria BCM_VLAN_PORT_MATCH_PORT (2) in egress
    }
    vp.port = sp; //TODO cleanup if port or gp or sp
    //NOTE do not add VSI here
    //NOTE Global LIF ID range [0x00001000:0x00100FFF]
    //outlif = (17 << 26 | 2 << 22 | 0 << 20 | ((0x1000 + lif_id) & 0xFFFFF));
    //vp.vlan_port_id = 0x44801000 + lif_id;
    vp.flags = BCM_VLAN_PORT_WITH_ID;
    BCM_GPORT_SUB_TYPE_LIF_SET(vp.vlan_port_id, 0, lif_id + 0x1000);
    BCM_GPORT_VLAN_PORT_ID_SET(vp.vlan_port_id, vp.vlan_port_id);
    vp.egress_vlan = vp.match_vlan = vlan_id;
    rv = bcm_vlan_port_create(unit, &vp);
    if (BCM_E_NONE != rv) {
      printf(
          "Failed to create symmetric LIF for port %d vlan %d LIF: %d VSI: %d:%s\n",
          port, vlan_id, lif_id, vsi_id, bcm_errmsg(rv));
      return RET_CODE_FAILURE;
    }
    /*
     * passed in global LIF ID is further updated by SDK.
     * this updated ID is stored in the bcm vlan object and is
     * needed for outLIF delete handling.
     */
    *out_gp = vp.vlan_port_id;
    printf("out_gp: %x\n", vp.vlan_port_id);
    //NOTE this adds LIFs to VSI
    rv = bcm_vswitch_port_add(unit, vsi_id, vp.vlan_port_id);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_vswitch_port_add(port = %d):%s\n", vp.vlan_port_id,
             bcm_errmsg(rv));
      return RET_CODE_FAILURE;
    }
    /*NOTE src/bcm/dnx/port/port_match.c[277]dnx_port_match_verify unit 0:
     * Error 'Invalid parameter' indicated, Egress unsupported match. match = 3,
     * only BCM_PORT_MATCH_PORT(2) is supported
    */
//    bcm_port_match_info_t_init(&port_match);
//     port_match.match = BCM_PORT_MATCH_PORT_VLAN;
//     port_match.port = port;
//     port_match.match_vlan = vlan_id;  //TODO looks odd VLAN==VSI
//     port_match.flags = BCM_PORT_MATCH_EGRESS_ONLY;
//     rv = bcm_port_match_add(unit, vp.vlan_port_id, &port_match);
//     if (BCM_E_NONE != rv) {
//       printf("Failed to add outLIF port match for port %d vlan %d. %s\n",
//                   port, vlan_id, bcm_errmsg(rv));
//       return RET_CODE_FAILURE;
//     }
    /*
     * associate Egress VLAN edit class and VLAN edit
     * parameters to be used for VLAN editing on outLIF for
     * tagged / untagged ports. command key is derived from
     * this edit class plus the packet tag format.
     */
    if (ut == unit && use_vlan_trans) {
      bcm_vlan_port_translation_t_init(&vp_trans);
      vp_trans.new_outer_vlan = vlan_id;
      vp_trans.gport = vp.vlan_port_id;
      if (tagged) {
        vp_trans.vlan_edit_class_id = EGRESS_EDIT_CLASS_TAGGED;
      } else {
        vp_trans.vlan_edit_class_id = EGRESS_EDIT_CLASS_UNTAGGED;
      }
      vp_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
      rv = bcm_vlan_port_translation_set(unit, &vp_trans);
      if (BCM_E_NONE != rv) {
        printf("Failed to set LIF EVE for port %d (gport %x) vlan %d: %s\n",
               port, vp_trans.gport, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
//      bcm_vlan_port_translation_t_init(&vp_trans);
//      vp_trans.gport = vp.vlan_port_id;
//      if (tagged) {
//        vp_trans.vlan_edit_class_id = INGRESS_EDIT_CLASS_TAGGED;
//      } else {
//        vp_trans.vlan_edit_class_id = INGRESS_EDIT_CLASS_UNTAGGED;
//      }
//      vp_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
//      rv = bcm_vlan_port_translation_set(unit, &vp_trans);
//      if (BCM_E_NONE != rv) {
//        printf("Failed to set LIF IVE for port %d (gport %x) vlan %d: %s\n",
//               port, vp_trans.gport, bcm_errmsg(rv));
//        return RET_CODE_FAILURE;
//      }
    }
  }
  if (ut == unit) {
    if (use_esem) {
      //NOTE Add an extra ESEM entry for L3
      /*
       * 88690-PG102: 22.8 L3 Egress Object: Egress-ARP (Next-hop)
       * "VLAN translation properties: Optional. It is optional to derive
       * VLAN-translation information directly from the ARP entry and not
       * from additional ESEM entry or ESEM default properties."
       */
      bcm_vlan_port_t_init(&vp);
      vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
      vp.port = sp;
      vp.vsi = vsi_id;
      //NOTE it will be a different lif type, reuse LIF ID
      vp.vlan_port_id = lif_id & 0xfff;
      BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SET(vp.vlan_port_id,
                                                  vp.vlan_port_id | 0x20000);
      BCM_GPORT_VLAN_PORT_ID_SET(vp.vlan_port_id, vp.vlan_port_id);
      vp.flags = BCM_VLAN_PORT_WITH_ID;
      vp.flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
      //NOTE to set VSI BCM_VLAN_PORT_VLAN_TRANSLATION flag is needed
      //This flag does not allow for bidirectional LIF creation
      vp.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
      //NOTE 'match_vlan' is and additional key 'C_VID' considered from 6.5.16
      //vp.egress_vlan =
      //vp.match_vlan = vlan_id;
      rv = bcm_vlan_port_create(unit, &vp);
      if (BCM_E_NONE != rv) {
        printf(
            "Failed to create ESEM LIF for port %d vlan %d LIF: %d VSI: %d:%s\n",
            port, vlan_id, lif_id, vsi_id, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
      if (use_vlan_trans) {
        bcm_vlan_port_translation_t_init(&vp_trans);
        vp_trans.new_outer_vlan = vlan_id;
        vp_trans.gport = vp.vlan_port_id;
        if (tagged) {
          vp_trans.vlan_edit_class_id = EGRESS_EDIT_CLASS_TAGGED;
        } else {
          vp_trans.vlan_edit_class_id = EGRESS_EDIT_CLASS_UNTAGGED;
        }
        vp_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_port_translation_set(unit, &vp_trans);
        if (BCM_E_NONE != rv) {
          printf(
              "Failed to set ESEM LIF EVE for port %d (gport %x) vlan %d: %s\n",
              port, vp_trans.gport, bcm_errmsg(rv));
          return RET_CODE_FAILURE;
        }
      }
      //TODO rename
      *in_gp = vp.vlan_port_id;
    }
    /* learn enable on port*/
    unsigned int f;
    rv = bcm_port_learn_get(unit, port, &f);
    printf("bcm_port_learn_get(%d) : %x\n", port, f);
    //NOTE physical AND virtual ports are both need learn enable, BTW it is the default for physical ports
    rv = bcm_port_learn_set(unit, port,
                            BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_learn_set\n");
      return RET_CODE_FAILURE;
    }
  }
  /*
   * Add port to multicast group
   */
  //NOTE new method
  return bcm_dnx_sdk_manage_mcast_group(unit, port, mcid, 1, *out_gp);
}

ret_code_e
bcm_dnx_sdk_extract_and_add_port_to_vlan (int port, int lif_id,
                              uint32_t vlan_id, uint32_t vsi_id,
                              int mcid, int tagged,
                              uint32_t *in_gp, uint32_t *out_gp,
                              int unit)
{
  int                         rv;
  uint32_t                    ut = unit;
  bcm_port_tpid_class_t       tpid_class;
  bcm_vlan_port_translation_t vp_trans;
  bcm_port_match_info_t       port_match;
  bcm_vlan_port_t             vp;
  bcm_gport_t                 sp;
  /*
   * local port class, TPID, and inLIF programming
   */
  BCM_GPORT_SYSTEM_PORT_ID_SET(sp, port);
  if (ut == unit) {
    /*
     * set port class for VLAN domain assignment
     */
    rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
    if (BCM_E_NONE != rv) {
      printf( "Failed to set port %d:%s\n",
                 port, bcm_errmsg(rv));
      return RET_CODE_FAILURE;
    }
//TODO:  bcm_port_tpid_add and bcm_port_tpid_delete_all are obsolete (AN 3.29)
    //TODO need to add port to VLAN (AN 3.10, no VSI membership mode?)
//    bcm_pbmp_t pbmp, upbmp;
//    BCM_PBMP_CLEAR(pbmp);
//    BCM_PBMP_CLEAR(upbmp);
//    if (tagged) {
//      BCM_PBMP_PORT_ADD(pbmp, port);
//    } else {
//      BCM_PBMP_PORT_ADD(upbmp, port);
//    }
//    rv = bcm_vlan_port_add(unit, vlan_id, pbmp, upbmp);
//    if (rv != BCM_E_NONE) {
//        printf("Error, in bcm_vlan_port_add\n");
//        return rv;
//    }
    //TODO these 2 seems to do the same thing
    rv = bcm_vlan_gport_add(unit, vlan_id, sp,
                            tagged ? 0 : BCM_VLAN_GPORT_ADD_UNTAGGED);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add gp:%x VLAN:%d\n", port, vlan_id);
        return rv;
    }
    if (use_tpid_class) {
      /*
       * define tag format class ID 2 for single tagged packets
       * on this port - this is output from parser and input to
       * resolve IVE / EVE command class ID to be applied to the
       * packet.
       */
      bcm_port_tpid_class_t_init(&tpid_class);
      tpid_class.port = sp;
      tpid_class.tpid1 = 0x8100;
      //TODO how to get rid of anything not 8100 single tagged, _INVALID is not enough
      //9100:VLAN,8100:anything is picked up here as VLAN and passed to untagged port as-is
      tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
      tpid_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_SINGLE_TAG;
      tpid_class.vlan_translation_action_id = 0;
      //NOTE filter unwanted traffic
      tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY /*| BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO*/;
      rv = bcm_port_tpid_class_set(unit, &tpid_class);
      if (BCM_E_NONE != rv) {
        printf( "Failed to set egress tagged tpid-class for port %d: %s\n",
                    port, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
      //TODO a per port way to drop unwanted traffic,
      //higher level logic is needed for trunk ports which
      //might need to take tagged and untagged at the same port
      tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
      if (!tagged) {
        //NOTE otherwise tagged traffic on untagged port is considered as default VLAN
        tpid_class.flags |= BCM_PORT_TPID_CLASS_DISCARD;
      }
      rv = bcm_port_tpid_class_set(unit, &tpid_class);
      if (BCM_E_NONE != rv) {
        printf( "Failed to set ingress tagged tpid-class for port %d: %s\n",
                    port, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
      /*
       * define tag format class ID 0 for untagged packets
       * egress to this port - this is output from parser and input to
       * resolve IVE / EVE command class ID to be applied to the
       * packet.
       */
      bcm_port_tpid_class_t_init(&tpid_class);
      tpid_class.port = sp;
      tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
      tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
      tpid_class.tag_format_class_id = TAG_FORMAT_CLASS_ID_NO_TAG;
      tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
      tpid_class.vlan_translation_action_id = 0;
      //NOTE filter unwanted traffic
      tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
      rv = bcm_port_tpid_class_set(unit, &tpid_class);
      if (BCM_E_NONE != rv) {
        printf( "Failed to set egress untagged tpid-class for port %d: %s\n",
                    port, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
      //TODO a per port way to drop unwanted traffic...
      tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
      if (tagged) {
        tpid_class.flags |= BCM_PORT_TPID_CLASS_DISCARD;
      }
      rv = bcm_port_tpid_class_set(unit, &tpid_class);
      if (BCM_E_NONE != rv) {
        printf( "Failed to set ingress untagged tpid-class for port %d: %s\n",
                    port, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
    }
    /* Interface is local to this unit, set pvid + knet filter handling */
    if (!tagged) {
      rv = bcm_port_untagged_vlan_set(unit, port, vlan_id);
      //TODO: ensure this is always called for a port. By default untagged==1 (PG 21.5.2)
      //Note: ingress VLAN miss is drop by default (PG 21.5.2)
      //TODO seems to be punt to RP
      if (BCM_E_NONE != rv) {
        printf( "Failed to set pvid for port %d:%s\n", port, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
      //TBD : Knet filter handling
    }
    if (use_inlif) {
      /*
       * create (p,v) inLIF
       */
      bcm_vlan_port_t_init(&vp);
      if (tagged) {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
      } else {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT;
  //TODO  (AN 3.27), use BCM_VLAN_PORT_MATCH_PORT; _UNTAGGED does not work
  //TODO (PG 21.8.2.1) use BCM_VLAN_PORT_MATCH_PORT to make this the port default inLIF
      }
      vp.port = sp; //TODO clean up if port or gp or sp
      vp.match_vlan = vlan_id;
      vp.vsi = vsi_id;
      /* Note: egress_vlan is no longer supported according to 9.1.3 */
      //TODO: find the SDK way of setting range [0x00001000:0x00100FFF] for dnx_vlan_port_create_verify()
      // low_global_lif_id_bound = dnx_data_l3.rif.nof_rifs_get(unit);
      // high_global_lif_id_bound =
      //   low_global_lif_id_bound + dnx_data_lif.global_lif.nof_global_l2_gport_lifs_get(unit);
      // starts at 0x44901000
      vp.vlan_port_id = lif_id &0xfff;
      BCM_GPORT_SUB_TYPE_LIF_SET(vp.vlan_port_id,
          BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,
      //    0,
          vp.vlan_port_id | 0x1000);
      BCM_GPORT_VLAN_PORT_ID_SET(vp.vlan_port_id, vp.vlan_port_id);
      vp.flags = BCM_VLAN_PORT_WITH_ID;
      vp.flags |= BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
      rv = bcm_vlan_port_create(unit, &vp);
      if (BCM_E_NONE != rv) {
        printf( "Failed to create inLIF for port %d vlan %d LIF: %d VSI: %d:%s\n",
                     port, vlan_id, lif_id, vsi_id, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }

      /*
       * passed in global LIF ID is further updated by SDK.
       * this updated ID is stored in the bcm vlan object and is
       * needed for inLIF delete handling.
       */
      *in_gp = vp.vlan_port_id;
      //NOTE normally the virtual LIF is hit before learning
      rv = bcm_port_learn_set(unit, vp.vlan_port_id, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
      if (rv != BCM_E_NONE)
      {
          printf("Error, bcm_port_learn_set %x: %s\n", vp.vlan_port_id, bcm_errmsg(rv));
          return RET_CODE_FAILURE;
      }
      printf("in_gp: %x\n", vp.vlan_port_id);
    }
  }
  if (use_outlif) {
    /*
     * create (p,v) outLIF
     *
     * for untagged / native VLAN, we dont necessarily need a (VSI, port)
     * ESEM entry for more granular EVE, as we are  simply stripping the
     * tag - however, still creating one for consistent behavior across
     * tagged and untagged ports as well as flexibililty in case
     * more granular control at outLIF is needed for future applications.
     *
     * alternatively, we could simply modify the default eve profile(0)
     * action-id for tagged tag_format to strip the tag on egress and not
     * create an out-LIF at all OR use port-dafault outLIF with a different
     * ID (cannot use same ID as inLIF, as api does not let you create
     * egress_only port default outLIF for VSI > 4096)
     */
    //TODO why add it to all units?
    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = sp; //TODO cleanup if port or gp or sp
    vp.vsi = vsi_id;
    //TODO (PG 21.8.2.2) port, vsi are irrelevant in case of BCM_VLAN_PORT_MATCH_NONE, changed
    //BCM_GPORT_SUB_TYPE_LIF_SET(vp.vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, lif_id);
    //BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(vp.vlan_port_id, lif_id);
    //TODO find the SDK way of setting range [0x00020000:0x00020FFF] for dnx_vlan_port_create_verify()
    //  SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_INLIF_2, &ing_table_capacity));
    //  SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_ESEM, &eg_table_capacity));
    //  low_global_lif_id_bound = ing_table_capacity;
    //  high_global_lif_id_bound = low_global_lif_id_bound + eg_table_capacity;

    vp.vlan_port_id = lif_id &0xfff;
//    BCM_GPORT_SUB_TYPE_LIF_SET(vp.vlan_port_id,
//        BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY,
//        vp.vlan_port_id | 0x2000);
    BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SET(vp.vlan_port_id, vp.vlan_port_id | 0x20000);
    BCM_GPORT_VLAN_PORT_ID_SET(vp.vlan_port_id, vp.vlan_port_id);
    vp.flags = BCM_VLAN_PORT_WITH_ID;
    vp.flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    //NOTE to set VSI BCM_VLAN_PORT_VLAN_TRANSLATION flag is needed
    //This flag does not allow for bidirectional LIF creation
    vp.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
    vp.egress_vlan = vp.match_vlan = vlan_id;
    rv = bcm_vlan_port_create(unit, &vp);
    if (BCM_E_NONE != rv) {
      printf( "Failed to create outLIF for port %d vlan %d LIF: %d VSI: %d:%s\n",
                         port, vlan_id, lif_id, vsi_id, bcm_errmsg(rv));
      return RET_CODE_FAILURE;
    }
    // TODO: delete explicitly removes this removed match

    /*
     * passed in global LIF ID is further updated by SDK.
     * this updated ID is stored in the bcm vlan object and is
     * needed for outLIF delete handling.
     */
    *out_gp = vp.vlan_port_id;
    printf("out_gp: %x\n", vp.vlan_port_id);
//    rv = bcm_vswitch_port_add(unit, vsi_id, vp.vlan_port_id);
//    if (rv != BCM_E_NONE) {
//        printf("Error, in bcm_vswitch_port_add(port = %d):%s\n", vp.vlan_port_id, bcm_errmsg(rv));
//        return RET_CODE_FAILURE;
//    }
    /*
     * associate Egress VLAN edit class and VLAN edit
     * parameters to be used for VLAN editing on outLIF for
     * tagged / untagged ports. command key is derived from
     * this edit class plus the packet tag format.
     */
    if (ut == unit && use_vlan_trans) {
      bcm_vlan_port_translation_t_init(&vp_trans);
      vp_trans.new_outer_vlan = vlan_id;
      vp_trans.gport = vp.vlan_port_id;
      if (tagged) {
        vp_trans.vlan_edit_class_id = EGRESS_EDIT_CLASS_TAGGED;
      } else {
        vp_trans.vlan_edit_class_id = EGRESS_EDIT_CLASS_UNTAGGED;
      }
      vp_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
      rv = bcm_vlan_port_translation_set(unit, &vp_trans);
      if (BCM_E_NONE != rv) {
        printf( "Failed to set outLIF EVE for port %d (gport %x) vlan %d: %s\n",
                    port, vp_trans.gport, bcm_errmsg(rv));
        return RET_CODE_FAILURE;
      }
    }
  }
//  bcm_dnx_sdk_vsi_learn_distribution(unit, vlan_id);
//  bcm_dnx_sdk_vsi_learn_distribution(unit, vsi_id);
  /* learn enable on port*/
  unsigned int f;
  rv = bcm_port_learn_get(unit,port,&f);
  printf("bcm_port_learn_get(%d) : %x\n", port, f);
  //NOTE physical AND virtual ports are both need learn enable, BTW it is the default for physical ports
  rv = bcm_port_learn_set(unit,port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
  if (rv != BCM_E_NONE)
  {
      printf("Error, bcm_port_learn_set\n");
      return RET_CODE_FAILURE;
  }

  /*
   * Add port to multicast group
   */
  //NOTE new method
  return bcm_dnx_sdk_manage_mcast_group(unit, sp, mcid, 1, 0);
}
ret_code_e
add_mac_ol(int port, uint32_t vsi, char endmac, int unit, int outlif)
{
  bcm_l2_addr_t l2_addr;
  bcm_mac_t mac = {0,0,0,0,0,0};
  int rv;
  mac[5] = endmac;
  bcm_l2_addr_t_init(&l2_addr, mac, vsi);
  l2_addr.flags = BCM_L2_STATIC;      /* static entry */
  l2_addr.port = port;
  if (2 <= use_case) {
    bcm_gport_t forward_encap_id;
    BCM_FORWARD_ENCAP_ID_VAL_SET(forward_encap_id,BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF,BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_VLAN_PORT,outlif & 0xffff);
    l2_addr.encap_id = forward_encap_id;
  }
  rv = bcm_l2_addr_add(unit, &l2_addr);

  if (rv != BCM_E_NONE)
  {
      printf("Error, bcm_l2_addr_add (%x): %s\n", endmac, bcm_errmsg(rv));
      return rv;
  }
  return RET_CODE_SUCCESS;
}
ret_code_e
add_mac(int port, uint32_t vsi, char endmac, int unit)
{
  return add_mac_ol(port, vsi, endmac, unit, 0);
}

ret_code_e
bcm_dnx_sdk_extract_and_del_port_from_vlan_sym (int port,
                              uint32_t vlan_id, uint32_t vsi,
                              int mcid, int tagged,
                              uint32_t in_gp, uint32_t out_gp,
                              int unit)
{
  int rv;
  bcm_vlan_t pvid;
  bcm_gport_t sp;
  BCM_GPORT_SYSTEM_PORT_ID_SET(sp, port);
  if (1 /*ut==unit*/) {

    rv = bcm_vlan_gport_delete(unit, vlan_id, sp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_delete\n");
        return rv;
    }
    /*
     * need to reset PVID, else mac learing for untagged packets on
     * on this port continues to happen on the last configured native
     * vlan
     */

    /* Interface is local to this unit, reset pvid + knet filter handling */
    if (!tagged) {
      rv = bcm_port_untagged_vlan_get(unit, port, &pvid);
      if (BCM_E_NONE != rv) {
        printf("Failed to get pvid for port %d: %s\n", port, bcm_errmsg(rv));
      }
      if (vlan_id == pvid) {
        /* PVID has not been reset by an earlier update */
        rv = bcm_port_untagged_vlan_set(unit, port, BCM_SDK_DNX_DEF_VLAN);
        if (BCM_E_NONE != rv) {
          printf("Failed to reset pvid for port %d:%s\n", port, bcm_errmsg(rv));
        }
        //TBD : KNET strip tag handling
      }
    }
  }

  /*
   * flush all mac entries learnt on this port to avoid traffic being
   * unicast switched to this port instead of being flooded on the
   * the bridge
   * NOTE: must use sp instead of gp
   */
  bcm_dnx_sdk_mac_flush_by_port(unit, port);
  if (use_outlif) {
    rv = bcm_vlan_port_destroy(unit, out_gp);
    if (BCM_E_NONE != rv) {
      printf("Failed to destroy outLIF %x for port %d vlan %d. %s\n", out_gp,
             port, vlan_id, bcm_errmsg(rv));
      return (RET_CODE_FAILURE);
    }
  }
  if (use_esem) {
    rv = bcm_vlan_port_destroy(unit, in_gp);
    if (BCM_E_NONE != rv) {
      printf("Failed to destroy ESEM LIF %x for port %d vlan %d. %s\n", in_gp,
             port, vlan_id, bcm_errmsg(rv));
      return (RET_CODE_FAILURE);
    }
  }
  /*
   * Del port from multicast group
   */
  return bcm_dnx_sdk_manage_mcast_group(unit, sp, mcid, 0, out_gp);
  /*
   * not resetting port TPID profile, as TPIDs are deleted before
   * re-provisioning the port
   * NM-TBD: no harm from leaving port class ID set or do we necessarily
   * need to reset?
   */
}
/* (ds_bcm_port_t *obj, uint32_t vlan_id,
                               uint32_t vsi, int mcid, bool tagged,
                               uint32_t in_gp, uint32_t out_gp,
                               dpal_switch_unit_t unit) */
ret_code_e
bcm_dnx_sdk_extract_and_del_port_from_vlan (int port,
                              uint32_t vlan_id, uint32_t vsi,
                              int mcid, int tagged,
                              uint32_t in_gp, uint32_t out_gp,
                              int unit)
{
  int rv;
  bcm_vlan_t pvid;
  uint32_t ut = unit;
  bcm_port_match_info_t port_match;

/*  int                         rv;
  uint32_t                    ut = unit;
  bcm_gport_t                 sp, gp;
  bcm_port_tpid_class_t       tpid_class;
  bcm_vlan_port_translation_t vp_trans;
  bcm_port_match_info_t       port_match;
  bcm_vlan_port_t             vp; */
  bcm_gport_t                 sp;
  /* Vlan membership + PVID setup only if port is local to this unit */
  BCM_GPORT_SYSTEM_PORT_ID_SET(sp, port);
  if (ut == unit) {
    if (use_inlif) {
      /*
       * delete inLIF from VSI
       */
      rv = bcm_vswitch_port_delete(unit, vsi, in_gp);
      if (BCM_E_NONE != rv) {
        printf("Failed to del inLIF %x for port %d vlan %d from vsi %d. %s\n",
               in_gp, port, vlan_id, vsi, bcm_errmsg(rv));
        return (RET_CODE_FAILURE);
      }
      /*
       * destroy inLIF
       */
      rv = bcm_vlan_port_destroy(unit, in_gp);
      if (BCM_E_NONE != rv) {
        printf("Failed to destroy inLIF %x for port %d vlan %d. %s\n",
               in_gp, port, vlan_id, bcm_errmsg(rv));
        return (RET_CODE_FAILURE);
      }
    }
    //TODO remove port from VLAN
    bcm_pbmp_t pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_remove(unit, vlan_id, pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_remove\n");
        return rv;
    }
    rv = bcm_vlan_gport_delete(unit, vlan_id, sp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_delete\n");
        return rv;
    }
    /*
     * need to reset PVID, else mac learing for untagged packets on
     * on this port continues to happen on the last configured native
     * vlan
     */

    /* Interface is local to this unit, reset pvid + knet filter handling */
    if (!tagged) {
      rv = bcm_port_untagged_vlan_get(unit, port, &pvid);
      if (BCM_E_NONE != rv) {
        printf("Failed to get pvid for port %d: %s\n", port, bcm_errmsg(rv));
      }
      if (vlan_id == pvid) {
        /* PVID has not been reset by an earlier update */
        rv = bcm_port_untagged_vlan_set(unit, port, BCM_SDK_DNX_DEF_VLAN);
        if (BCM_E_NONE != rv) {
          printf("Failed to reset pvid for port %d:%s\n", port, bcm_errmsg(rv));
        }
        //TBD : KNET strip tag handling
      }
    }
  }

  /*
   * flush all mac entries learnt on this port to avoid traffic being
   * unicast switched to this port instead of being flooded on the
   * the bridge
   * NOTE: must use sp instead of gp
   */
  bcm_dnx_sdk_mac_flush_by_port(unit, port);

  /*
   * destroy outLIF ESEM entry, NOTE: remove
   */
/*
  bcm_port_match_info_t_init(&port_match);
  port_match.match = BCM_PORT_MATCH_PORT_VLAN;
  port_match.port = gp;
  port_match.match_vlan = vsi;
  port_match.flags = BCM_PORT_MATCH_EGRESS_ONLY;
  rv = bcm_port_match_delete(unit, out_gp, &port_match);
  if (BCM_E_NONE != rv) {
    printf("Failed to delete outLIF port match for port %d vlan %d unit %d. %s\n",
                port, vlan_id, unit, bcm_errmsg(rv));
    return RET_CODE_FAILURE;
  }
*/
  if (use_outlif) {
    rv = bcm_vlan_port_destroy(unit, out_gp);
    if (BCM_E_NONE != rv) {
      printf("Failed to destroy outLIF %x for port %d vlan %d. %s\n",
             out_gp, port, vlan_id, bcm_errmsg(rv));
      return (RET_CODE_FAILURE);
    }
  }
  /*
   * Del port from multicast group
   */
  return bcm_dnx_sdk_manage_mcast_group(unit, sp, mcid, 0, 0);
  /*
   * not resetting port TPID profile, as TPIDs are deleted before
   * re-provisioning the port
   * NM-TBD: no harm from leaving port class ID set or do we necessarily
   * need to reset?
   */
}

ret_code_e
add_vlanp(uint32_t vlan_id, uint32_t vsi_id, int lif_base, int p1, int p2, int untagged, int unit, vsi_info_t *vsi_info)
{
  int            rv;
  int            flags = 0;
  /*
   * create VSI
   */

  /*
   * create multicast group for flooding
   */
  int mcid = vsi_id;
  //TODO do we need BCM_MULTICAST_TYPE_L2?
  flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
  rv = bcm_multicast_create(unit, flags, &mcid);
  if (BCM_E_NONE != rv) {
    printf( "Failed to create mc group for vlan %d. %s\n",
                vlan_id, bcm_errmsg(rv));
    return (RET_CODE_FAILURE);
  }
  switch (use_case) {
  case 2:
    /* Create VSI: say it is below 4k */
    rv = bcm_vlan_create(unit, vsi_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vis=%d)\n", vsi_id);
        return rv;
    }
    print
    l2_basic_bridge_run(p1, lif_base, vlan_id, vsi_id,
                                                 mcid, 1, vsi_info->in_gp,
                                                 vsi_info->out_gp, unit);
    print
    l2_basic_bridge_run(p2, lif_base + 1, vlan_id,
                                                 vsi_id, mcid, 1,
                                                 (vsi_info->in_gp) + 1,
                                                 (vsi_info->out_gp) + 1, unit);
    /* untagged */
    print
    l2_basic_bridge_run(untagged, lif_base + 2,
                                                 vlan_id, vsi_id, mcid, 0,
                                                 (vsi_info->in_gp) + 2,
                                                 (vsi_info->out_gp) + 2, unit);
    break;
  case 1:
    rv = bcm_vswitch_create_with_id(unit, vsi_id);
    if (BCM_E_NONE != rv) {
      printf( "Failed to create VSI %d: %s\n", vsi_id, bcm_errmsg(rv));
      return (RET_CODE_FAILURE);
    }
    //TODO create VLAN
    rv = bcm_vlan_create(unit, vlan_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vlan=%d)\n", vlan_id);
        //return rv;
    }
    print
    bcm_dnx_sdk_extract_and_add_port_to_vlan_sym(p1, lif_base, vlan_id, vsi_id,
                                             mcid, 1, vsi_info->in_gp,
                                             vsi_info->out_gp, unit);
    print
    bcm_dnx_sdk_extract_and_add_port_to_vlan_sym(p2, lif_base + 1, vlan_id, vsi_id,
                                             mcid, 1, (vsi_info->in_gp) + 1,
                                             (vsi_info->out_gp) + 1, unit);
    /* untagged */
    print
    bcm_dnx_sdk_extract_and_add_port_to_vlan_sym(untagged, lif_base + 2, vlan_id,
                                             vsi_id, mcid, 0,
                                             (vsi_info->in_gp) + 2,
                                             (vsi_info->out_gp) + 2, unit);
    break;
  default:
    rv = bcm_vswitch_create_with_id(unit, vsi_id);
    if (BCM_E_NONE != rv) {
      printf( "Failed to create VSI %d: %s\n", vsi_id, bcm_errmsg(rv));
      return (RET_CODE_FAILURE);
    }
    //TODO create VLAN
    rv = bcm_vlan_create(unit, vlan_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vlan=%d)\n", vlan_id);
        //return rv;
    }
    print
    bcm_dnx_sdk_extract_and_add_port_to_vlan(p1, lif_base, vlan_id, vsi_id,
                                             mcid, 1, vsi_info->in_gp,
                                             vsi_info->out_gp, unit);
    print
    bcm_dnx_sdk_extract_and_add_port_to_vlan(p2, lif_base + 1, vlan_id, vsi_id,
                                             mcid, 1, (vsi_info->in_gp) + 1,
                                             (vsi_info->out_gp) + 1, unit);
    /* untagged */
    print
    bcm_dnx_sdk_extract_and_add_port_to_vlan(untagged, lif_base + 2, vlan_id,
                                             vsi_id, mcid, 0,
                                             (vsi_info->in_gp) + 2,
                                             (vsi_info->out_gp) + 2, unit);
  }
  print add_mac_ol(p1, vsi_id, 1, unit, (vsi_info->out_gp)[0]);
  print add_mac_ol(p2, vsi_id, 2, unit, (vsi_info->out_gp)[1]);
  print add_mac_ol(untagged, vsi_id, 3, unit, (vsi_info->out_gp)[2]);

  return (RET_CODE_SUCCESS);
}

ret_code_e
del_vlanp(uint32_t vlan_id, uint32_t vsi_id, int lif_base, int p1, int p2, int untagged, int unit, vsi_info_t *vsi_info)
{
  int            rv;
  int            flags = 0;
  switch (use_case) {
  case 1:
    print
    bcm_dnx_sdk_extract_and_del_port_from_vlan_sym(p1, vlan_id, vsi_id, vsi_id, 1,
                                               vsi_info->in_gp[0],
                                               vsi_info->out_gp[0], unit);
    print
    bcm_dnx_sdk_extract_and_del_port_from_vlan_sym(p2, vlan_id, vsi_id, vsi_id, 1,
                                               vsi_info->in_gp[1],
                                               vsi_info->out_gp[1], unit);
    /* untagged */
    print
    bcm_dnx_sdk_extract_and_del_port_from_vlan_sym(untagged, vlan_id, vsi_id,
                                               vsi_id, 0, vsi_info->in_gp[2],
                                               vsi_info->out_gp[2], unit);
  break;
  default:
    print
    bcm_dnx_sdk_extract_and_del_port_from_vlan(p1, vlan_id, vsi_id, vsi_id, 1,
                                               vsi_info->in_gp[0],
                                               vsi_info->out_gp[0], unit);
    print
    bcm_dnx_sdk_extract_and_del_port_from_vlan(p2, vlan_id, vsi_id, vsi_id, 1,
                                               vsi_info->in_gp[1],
                                               vsi_info->out_gp[1], unit);
    /* untagged */
    print
    bcm_dnx_sdk_extract_and_del_port_from_vlan(untagged, vlan_id, vsi_id,
                                               vsi_id, 0, vsi_info->in_gp[2],
                                               vsi_info->out_gp[2], unit);
  }
  /*
   * delete vsi
   */
  rv = bcm_vswitch_destroy(unit, vsi_id);
  if (BCM_E_NONE != rv) {
    printf("Failed to destroy vsi %d for vlan %d: %s\n",
                vsi_id, vlan_id, bcm_errmsg(rv));
  }
  //TODO destroy VLAN
  rv = bcm_vlan_destroy(unit, vlan_id);
  if (rv != BCM_E_NONE) {
      printf("Error, in bcm_vlan_destroy(vlan=%d)\n", vlan_id);
      //return rv;
  }
  /*
   * delete vsi multicast group
   */
  int mcid = vsi_id;
  rv = bcm_multicast_destroy(unit, mcid);
  if (BCM_E_NONE != rv) {
    printf("Failed to destroy mc group for vlan %d. %s\n",
                vlan_id, bcm_errmsg(rv));
  }

return (RET_CODE_SUCCESS);
}

ret_code_e
add_vlan(uint32_t vlan_id, uint32_t vsi_id, int lif_base, int untagged, int unit, vsi_info_t *vsi_info)
{
  return add_vlanp(vlan_id, vsi_id, lif_base, 1, p13, untagged, unit, *vsi_info);
}
ret_code_e
del_vlan(uint32_t vlan_id, uint32_t vsi_id, int lif_base, int untagged, int unit, vsi_info_t *vsi_info)
{
  return del_vlanp(vlan_id, vsi_id, lif_base, 1, p13, untagged, unit, *vsi_info);
}

int
get_sysport_from_gport(int unit, bcm_gport_t gp, bcm_gport_t *sp)
{
  int rv;
  if (BCM_GPORT_IS_VLAN_PORT(gp)) {
    bcm_vlan_port_t vp;
    bcm_vlan_port_t_init(&vp);
    vp.vlan_port_id = gp;
    rv = bcm_vlan_port_find(unit, &vp);
    if (rv != BCM_E_NONE) {
      printf(
          "Error in bcm_vlan_port_find of gport(%x): %s\n",
          vp.vlan_port_id, bcm_errmsg(rv));
      return rv;
    }
    *sp = vp.port;
    if (!BCM_GPORT_IS_SYSTEM_PORT(*sp)) {
      printf("%x is not a sysport\n", *sp);
      return BCM_E_INTERNAL;
    }
  }
  return BCM_E_NONE;
}
