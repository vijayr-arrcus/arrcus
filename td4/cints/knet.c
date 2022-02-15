bcmpkt_rx_register
      pktio_rx_handler
           ltsw_pktio_pkt_data_dump
           bcmi_ltsw_pktio_rx_dump
                         bcmpkt_rxpmd_get

RXPMD | RXPMDFLEX | DMAC | SMAC | Rest of the packet Data | CRC
1. RXPMD refers to the fixed RX metadata.Length is 20 bytes
2. RXPMDFLEX refers to the RX metadata which can be influenced by NPL. For TD4, this is defined by DNA
   Length is 52 bytes.
3. There’s a special field in the TD4 RXPMD called “MATCH_ID” that includes flags for various information from the packet header. This includes information like “Is this an L2 packet”, “does this packet have an outer tag” etc..

Every RX packet has both RXPMD and RXPMDFLEX.

typedef struct bcm_pktio_fid_support_s {
  bcm_pktio_bitmap_t txpmd_support;   /* TXPMD field ID supported bit array. */
  bcm_pktio_bitmap_t rxpmd_support;   /* RXPMD field ID supported bit array. */
  bcm_pktio_bitmap_t gih_support;     /* GIH field ID supported bit array. */
  bcm_pktio_bitmap_t;  /* FLEXPMD field ID supported bit array. */
  bcm_pktio_bit map_t higig3_support;  /* Higig3 field ID supported bit array. */
} bcm_pktio_fid_support_t;

bcmpkt_packet_t;
  bcmpkt_pmd_t pmd;
        rxpmd;


Example for Retrieving metadata from received packet
{
  bcm_pktio_fid_support_t support;
  uint32_t src_port;
  uint32_t pkt_length;
  uint32_t outer_vid;
  uint32_t vfi;

  bcm_pktio_pmd_fid_support_get(unit, &support);
  if (BCM_PKTIO_FID_SUPPORT_GET(support.rxpmd_support, BCM_PKTIO_RXPMD_SRC_PORT_NUM)) {
    bcm_pktio_pmd_field_get(unit, packet, bcmPktioPmdTypeRx,BCM_PKTIO_RXPMD_SRC_PORT_NUM,
                            &src_port);
  }


  if (BCM_PKTIO_FID_SUPPORT_GET(support.rxpmd_support, BCM_PKTIO_RXPMD_PKT_LENGTH)) {
    bcm_pktio_pmd_field_get(unit, packet, bcmPktioPmdTypeRx, BCM_PKTIO_RXPMD_PKT_LENGTH,
                            &pkt_length);
  }

  if (BCM_PKTIO_FID_SUPPORT_GET(support.rxpmd_support, BCM_PKTIO_RXPMD_OUTER_VID)) {
    bcm_pktio_pmd_field_get(unit, packet, bcmPktioPmdTypeRx, BCM_PKTIO_RXPMD_OUTER_VID,
                            &outer_vid);
  }

  /* Reading VFI */
  if (BCM_PKTIO_FID_SUPPORT_GET(support.rxflex_support, BCM_PKTIO_RXPMD_FLEX_VFI_15_0)) {
    bcm_pktio_pmd_field_get(unit, packet, bcmPktioPmdTypeRxFlex,
                            BCM_PKTIO_RXPMD_FLEX_VFI_15_0, &vfi);
  }
}

bcm_error_t
header_info(int unit, bcm_pktio_pkt_t * pkt)
{
  bcm_pktio_fid_support_t support;
  bcm_pktio_bitmap_t pkt_format;
  uint32          match_id_id[2] = { 0, 0 };
  int             count;
  printf("\nL2/L2 Header Info\n");

  BCM_IF_ERROR_RETURN(bcm_pktio_pmd_fid_support_get(unit, &support));
  BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get(unit, pkt, bcmPktioPmdTypeRx,
                                              BCM_PKTIO_RXPMD_MATCH_ID_LO,
                                              &match_id_id[0]));
  BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get(unit, pkt, bcmPktioPmdTypeRx,
                                              BCM_PKTIO_RXPMD_MATCH_ID_HI,
                                              &match_id_id[1]));
  /* In rx callback, decode match id. */
  BCM_IF_ERROR_RETURN(bcm_pktio_pmd_pkt_format_get
                      (unit, 2, match_id_id, &count, &pkt_format));
  printf("  OUTER_L2_HDR_L2: %s\n",
         BCM_PKTIO_MATCH_ID_GET(&pkt_format,
                                BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_L2) ? "yes" : "no");
  printf("  OUTER_L2_HDR_ITAG: %s\n",
         BCM_PKTIO_MATCH_ID_GET(&pkt_format,
                                BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_ITAG) ? "yes" : "no");
  printf("  OUTER_L2_HDR_OTAG: %s\n",
         BCM_PKTIO_MATCH_ID_GET(&pkt_format,
                                BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_OTAG) ? "yes" : "no");
  printf("  OUTER_L2_HDR_SNAP_OR_LLC: %s\n",
         BCM_PKTIO_MATCH_ID_GET(&pkt_format,
                                BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_SNAP_OR_LLC) ? "yes" : "no");
  printf("  OUTER_L3_L4_HDR_ETHERTYPE: %s\n",
         BCM_PKTIO_MATCH_ID_GET(&pkt_format,
                                BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_ETHERTYPE) ? "yes" : "no");
  printf("  OUTER_L3_L4_HDR_IPV4: %s\n",
         BCM_PKTIO_MATCH_ID_GET(&pkt_format,
                                BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_IPV4) ? "yes" : "no");
  printf("  OUTER_L3_L4_HDR_UDP: %s\n",
         BCM_PKTIO_MATCH_ID_GET(&pkt_format,
                                BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_UDP) ? "yes" : "no");
  return BCM_E_NONE;
}


As stated in CS00012204995, kindly refer to KB0028394 [Trident4 - Working with the new HSDK Packet Metadata].
 
For reason code, look for EVENT_TRACE_VECTOR [48bit - trace_vector_0_15, trace_vector_16_31 & trace_vector_32_47]
 
Note :
RXPMD & RXPMDFLEX are not in network order [dmesg output]
KB0028394 is based on DNA version 4.5.5 & RXPMDFLEX field is listed reverse in the article
 
Example (dmesg Output):
  0000: 0180c200 00200000 00000003 81000001 de08b880 10040000 00400000 48000000

  0020: 00014400 00000000 2a000000 05001001 00001082 00000200 00000000 00000000

  0040: 01000000 00000000 00000000 00000000 0a000100 00000000 00000000 00000202

  0060: 44000a00 00000000 0180c200 00200000 00000003 8100000a 002eeb2a 00000000

  0080: b8082526 2728292a 2b2c2d2e 2f303132 33343536 3738393a 3b3c3d3e 3f404142

  00a0: 43444546 4748494a 65730055
   
  Purple : RCPU header
  Red : RXPMD
  Green : RXPMDFLEX
  Yellow :  Ingress packet
  Below example decode is based on sdk-6.5.25 & DNA_4_9_5_0
  RXPMD with order change :  0x0044 0x0100 0x0000 0x0000 0x0000 0x002a 0x0110 0x0005
  PKT_LENGTH=0x44
  SRC_PORT_NUM=1
  MATCH_ID_LO=0x2a
  EGR_ZONE_REMAP_CTRL=1
  COPY_TO_CPU=1
  MPB_FLEX_DATA_TYPE=5
  RXPMDFLEX with order change:
  0x8210 0x0000 0x0002 0x0000 0x0000 0x0000 0x0000 0x0000
  0x0000 0x0001 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
  0x0001 0x000a 0x0000 0x0000 0x0000 0x0000 0x0202 0x0000
  0x000a 0x0044
  VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0=2
  PKT_MISC_CTRL_0_3_0=2
  SYSTEM_SOURCE_15_0=1
  SYSTEM_OPCODE_3_0=1
  MPLS_LABEL_DECAP_COUNT_3_0=8
  INGRESS_PP_PORT_7_0=1
  VFI_15_0=0xa
  DROP_CODE_15_0=0x202
  EVENT_TRACE_VECTOR_31_0=0xa0044

https://brcmsemiconductor-csm.wolkenservicedesk.com/wolken-support/allcases/request-details?requestId=CS00012225291


l2 add mac=0x1 vl=50 port=cd0 static=1
l2 add mac=0x2 vl=50 port=cd1 static=1
l2 add mac=0x3 vl=50 port=cd2 static=1
vlan create 50 pbm=cd0,cd1,cd2
port cd0 en=1 lb=mac
port cd1 en=0 lb=mac
port cd2 en=0 lb=mac
tx 100 pbm=cd2 vl=50 sm=0x1 dm=0xABCDABCDABCD Length=155


