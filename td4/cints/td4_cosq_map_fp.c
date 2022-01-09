/*
 * =====================================================================================
 *
 *       Filename:  td4_cosq_map_fp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/09/2022 17:21:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
https://brcmsemiconductor-csm.wolkenservicedesk.com/wolken-support/article?articleId=19990


The bcmFieldActionCosQCpuNew is not supported by NPL, It is not possible to assign direct queue from FP instead use bcmFieldActionCosMapNew Action.[1]

The FP_ING_COS_Q_MAP table is used to assign the COS queue; however, the IFP provides a 4-bit action, which is concatenated with INT_PRI to index the IFP_COS_MAP Table. This is not possible to assign the CPU_COS which can have up to 48 queues.[2]

A new mode is introduced to provide 8-bit pointer to the FP_ING_COS_Q_MAP table. Therefore, A new IFP action, ifp_cos_map_lo_profile_ptr, is added for it. When the action is non-zero, the concatenation of (ifp_cos_map_profile_ptr[3:0], ifp_cos_map_lo_profile_ptr[3:0]) will be the index which pointers to the upper region of the FP_ING_COS_Q_MAP table to get the direct cos queue.

The below is an example scripts to demonstrate how the HSDK supports the new added action:

cint

int classifierId1 = 0;
int classifierId2 = 0;
int unit = 0;
bcm_cosq_classifier_t classifier;
bcm_cosq_classifier_t_init(&classifier);
classifier.flags |= BCM_COSQ_CLASSIFIER_FIELD | BCM_COSQ_CLASSIFIER_NO_INT_PRI;
print bcm_cosq_classifier_create(0, &classifier, &classifierId1);
print bcm_cosq_classifier_create(0, &classifier, &classifierId2);

int cosq_array1[4] = {1, 1, 1, 34}; // ucq, mcq, rqe, cpu_queue
int cosq_array2[4] = {1, 1, 1, 10}; // ucq, mcq, rqe, cpu_queue
print bcm_cosq_classifier_mapping_multi_set(unit, -1, classifierId1, -1, 4, NULL, cosq_array1);
print bcm_cosq_classifier_mapping_multi_set(unit, -1, classifierId2, -1, 4, NULL, cosq_array2);

bcm_field_group_config_t ifp_group_cfg;
bcm_field_entry_t ifp_entry;

print bcm_field_group_config_t_init(&ifp_group_cfg);
print BCM_FIELD_QSET_INIT(ifp_group_cfg.qset);
print BCM_FIELD_ASET_INIT(ifp_group_cfg.aset);
print BCM_FIELD_QSET_ADD(ifp_group_cfg.qset, bcmFieldQualifyStageIngress);
print BCM_FIELD_QSET_ADD(ifp_group_cfg.qset, bcmFieldQualifyOuterVlanPri);
print BCM_FIELD_ASET_ADD(ifp_group_cfg.aset, bcmFieldActionCopyToCpu);
//print BCM_FIELD_ASET_ADD(ifp_group_cfg.aset, bcmFieldActionCosQCpuNew); // no longer supported
print BCM_FIELD_ASET_ADD(ifp_group_cfg.aset, bcmFieldActionCosMapNew);

bcm_port_config_t port_cfg;
bcm_port_config_t_init(&port_cfg);
print bcm_port_config_get(unit, &port_cfg);

BCM_PBMP_ASSIGN(ifp_group_cfg.ports, port_cfg.per_pipe[0]);
BCM_PBMP_OR(ifp_group_cfg.ports, port_cfg.per_pipe[1]);
ifp_group_cfg.flags |= BCM_FIELD_GROUP_CREATE_WITH_PORT;
ifp_group_cfg.priority = 11;
print bcm_field_group_config_create(unit, &ifp_group_cfg);

bcm_field_entry_t entry_1, entry_2;
print bcm_field_entry_create(unit, ifp_group_cfg.group, &entry_1);
bcm_field_qualify_OuterVlanPri(unit, entry_1, 0, -1);
print bcm_field_action_add(unit, entry_1, bcmFieldActionCopyToCpu, 0, 0);
print bcm_field_action_add(unit, entry_1, bcmFieldActionCosMapNew, classifierId1, 0);
print bcm_field_entry_install(unit, entry_1);

print bcm_field_entry_create(unit, ifp_group_cfg.group, &entry_2);
bcm_field_qualify_OuterVlanPri(unit, entry_2, 1, -1);
print bcm_field_action_add(unit, entry_2, bcmFieldActionCopyToCpu, 0, 0);
print bcm_field_action_add(unit, entry_2, bcmFieldActionCosMapNew, classifierId2, 0);
print bcm_field_entry_install(unit, entry_2);

bcm_pbmp_t pbmp, ubmp, cpu_bmp;
bcm_vlan_t default_vlan = 1;
bcm_vlan_t vlan = 100;

bcm_port_t port_1 = 1;
bcm_port_t port_2 = 2;

BCM_PBMP_CLEAR(ubmp);
BCM_PBMP_CLEAR(pbmp);
BCM_PBMP_PORT_ADD(pbmp, port_1);
BCM_PBMP_PORT_ADD(pbmp, port_2);

BCM_PBMP_PORT_ADD(pbmp, 0);
BCM_PBMP_PORT_ADD(cpu_bmp, 0);

print bcm_vlan_create(unit, vlan);
print bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
print bcm_vlan_port_remove(unit, default_vlan, pbmp);
print bcm_vlan_port_remove(unit, default_vlan, cpu_bmp);

print bcm_port_loopback_set(unit, port_1, BCM_PORT_LOOPBACK_PHY);
print bcm_port_loopback_set(unit, port_2, BCM_PORT_LOOPBACK_PHY);

exit;

l2 add mac=0x1 vl=100 p=cd1
pw start
pw report +raw +pmd
tx 1 pbm=cd0 DestMac=0x1 vlan=100 VlanPrio=0
tx 1 pbm=cd0 DestMac=0x1 vlan=100 VlanPrio=1

 

Test Result:

BCM.0> tx 1 pbm=cd0 vlan=100 VlanPrio=0
Packet generate, length=64
[bcmPWN.0]
[bcmPWN.0]Packet[1]: data[0000]: {00bc10000000} {000102030405} 8100 0064
[bcmPWN.0]Packet[1]: data[0010]: 002e 7062 6163 6b5f 7365 4646 0000 4646
[bcmPWN.0]Packet[1]: data[0020]: 0000 4646 0000 4646 0000 4646 0000 4646
[bcmPWN.0]Packet[1]: data[0030]: 0000 4646 0000 4646 0000 4646 0000 4646
[bcmPWN.0]

[RX metadata information]
QUEUE_NUM=0x22    //  34
PKT_LENGTH=0x44
SRC_PORT_NUM=1
SWITCH=1
MATCH_ID_LO=0x54
MPB_FLEX_DATA_TYPE=0xd
EGR_ZONE_REMAP_CTRL=1
COPY_TO_CPU=1
[FLEX fields]
EVENT_TRACE_VECTOR_31_16=4
DROP_CODE_15_0=0x6666
SYSTEM_SOURCE_15_0=1
INGRESS_PP_PORT_7_0=1
VFI_15_0=0x64
VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0=2
SYSTEM_OPCODE_3_0=1
[RX reasons]
IFP
[Decode match id]
INGRESS_PKT_OUTER_L2_HDR_L2
INGRESS_PKT_OUTER_L2_HDR_OTAG
INGRESS_PKT_OUTER_L2_HDR_SNAP_OR_LLC
BCM.0> show c
MC_PERQ_PKT(34).cpu0 : 2 +2
MC_PERQ_BYTE(34).cpu0 : 280 +280
XLMIB_TBYT.cd0 : 68 +68
XLMIB_RBYT.cd0 : 68 +68
XLMIB_TPOK.cd0 : 1 +1
XLMIB_RPOK.cd0 : 1 +1
XLMIB_TVLN.cd0 : 1 +1
XLMIB_RVLN.cd0 : 1 +1
XLMIB_TUCA.cd0 : 1 +1
XLMIB_RUCA.cd0 : 1 +1
XLMIB_TPKT.cd0 : 1 +1
XLMIB_RPKT.cd0 : 1 +1
XLMIB_T127.cd0 : 1 +1
XLMIB_R127.cd0 : 1 +1
XLMIB_RPRM.cd0 : 1 +1
XLMIB_TBYT.cd1 : 68 +68
XLMIB_RBYT.cd1 : 68 +68
XLMIB_TPOK.cd1 : 1 +1
XLMIB_RPOK.cd1 : 1 +1
XLMIB_TVLN.cd1 : 1 +1
XLMIB_RVLN.cd1 : 1 +1
XLMIB_TUCA.cd1 : 1 +1
XLMIB_RUCA.cd1 : 1 +1
XLMIB_TPKT.cd1 : 1 +1
XLMIB_RPKT.cd1 : 1 +1
XLMIB_T127.cd1 : 1 +1
XLMIB_R127.cd1 : 1 +1
XLMIB_RPRM.cd1 : 1 +1
UC_PERQ_PKT(0).cd1 : 1 +1
UC_PERQ_BYTE(0).cd1 : 68 +68
BCM.0>

BCM.0> tx 1 pbm=cd0 vlan=100 VlanPrio=1
Packet generate, length=64
[bcmPWN.0]
[bcmPWN.0]Packet[2]: data[0000]: {00bc10000000} {000102030405} 8100 2064
[bcmPWN.0]Packet[2]: data[0010]: 002e 4554 5f41 4444 2869 6a6a 0000 6a6a
[bcmPWN.0]Packet[2]: data[0020]: 0000 6a6a 0000 6a6a 0000 6a6a 0000 6a6a
[bcmPWN.0]Packet[2]: data[0030]: 0000 6a6a 0000 6a6a 0000 6a6a 0000 6a6a
[bcmPWN.0]

[RX metadata information]
QUEUE_NUM=0xa // 10
PKT_LENGTH=0x44
SRC_PORT_NUM=1
SWITCH=1
MATCH_ID_LO=0x54
MPB_FLEX_DATA_TYPE=0xd
EGR_ZONE_REMAP_CTRL=1
COPY_TO_CPU=1
[FLEX fields]
EVENT_TRACE_VECTOR_31_16=4
DROP_CODE_15_0=0x6666
SYSTEM_SOURCE_15_0=1
INGRESS_PP_PORT_7_0=1
INGRESS_QOS_REMAP_VALUE_OR_IFP_OPAQUE_OBJ_15_0=0x200
VFI_15_0=0x64
VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0=2
INT_PRI_3_0=1
SYSTEM_OPCODE_3_0=1
[RX reasons]
IFP
[Decode match id]
INGRESS_PKT_OUTER_L2_HDR_L2
INGRESS_PKT_OUTER_L2_HDR_OTAG
INGRESS_PKT_OUTER_L2_HDR_SNAP_OR_LLC
BCM.0> show c
MC_PERQ_PKT(10).cpu0 : 2 +2
MC_PERQ_BYTE(10).cpu0 : 280 +280
XLMIB_TBYT.cd0 : 136 +68
XLMIB_RBYT.cd0 : 136 +68
XLMIB_TPOK.cd0 : 2 +1
XLMIB_RPOK.cd0 : 2 +1
XLMIB_TVLN.cd0 : 2 +1
XLMIB_RVLN.cd0 : 2 +1
XLMIB_TUCA.cd0 : 2 +1
XLMIB_RUCA.cd0 : 2 +1
XLMIB_TPKT.cd0 : 2 +1
XLMIB_RPKT.cd0 : 2 +1
XLMIB_T127.cd0 : 2 +1
XLMIB_R127.cd0 : 2 +1
XLMIB_RPRM.cd0 : 2 +1
XLMIB_TBYT.cd1 : 136 +68
XLMIB_RBYT.cd1 : 136 +68
XLMIB_TPOK.cd1 : 2 +1
XLMIB_RPOK.cd1 : 2 +1
XLMIB_TVLN.cd1 : 2 +1
XLMIB_RVLN.cd1 : 2 +1
XLMIB_TUCA.cd1 : 2 +1
XLMIB_RUCA.cd1 : 2 +1
XLMIB_TPKT.cd1 : 2 +1
XLMIB_RPKT.cd1 : 2 +1
XLMIB_T127.cd1 : 2 +1
XLMIB_R127.cd1 : 2 +1
XLMIB_RPRM.cd1 : 2 +1
UC_PERQ_PKT(1).cd1 : 1 +1
UC_PERQ_BYTE(1).cd1 : 68 +68
BCM.0>

 

Per the RX metadata, it reveals that the QUEUE_NUM of to-cpu packet is exactly the same as the IFP assigned queue number. In the "show counter", the PERQ counters are also updated on CPU port accordingly.

 

[1] SDK-206824
[2] TD4CUSTOMER-123
