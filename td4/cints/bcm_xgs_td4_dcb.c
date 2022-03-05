/*
 * Filename: bcm_xgs_td4_dcb.c
 * Description:
 * Copyright 2022 by Arrcus, Inc.
 * All rights reserved.
 */
#include <stdbool.h>
#include <stdint.h>
#include <bcm_xgs_plugin_common.h>
#include <libutils/arrcus/arrcus_types.h>
#include <bcm/pktio.h>
#include <bcmpkt/bcmpkt_rxpmd_defs.h>

char *
bcmsdk_xgs_td4_metadata_decode (char *packet, int _pkt_len,
                                uint32_t *attrdata, uint32_t *total_attrs)
{
  bcm_pktio_pkt_t pkt;
//  bcmlrd_variant_t variant;
  int i = 0;
  uint32_t src_port = 0;
  uint32_t *pk_32 = (uint32_t *)(packet + 32);
  //bcmdrd_dev_type_t dev_val = 0;
  //bcmpkt_rcpu_hdr_t *rcpu = (bcmpkt_rcpu_hdr_t *)packet;
  uint32_t *rxpmd = NULL;
  uint32_t vlan_id = 0;
  uint32_t vlan_id_2 = 0;
  uint32_t pkt_len = 0;

//  bcmpkt_dev_type_get(0, &dev_val);
//  printf("----------------------\nInside bcmsdk_xgs_td4_metadata_decode - devtype %d sizeof rcpu %d \n",
//         dev_val, (int)sizeof(bcmpkt_rcpu_hdr_t));
//  rxpmd = (uint32_t *)(packet + sizeof(bcmpkt_rcpu_hdr_t));
  rxpmd = (uint32_t *)(packet + 32);
  memset(&pkt, 0, sizeof(bcm_pktio_pkt_t));
  pkt.unit = 0;
  pkt.flags = 2;
  pkt.pmd.rxpmd = rxpmd;
  bcm_pktio_pmd_field_get(0, &pkt, bcmPktioPmdTypeRx, BCMPKT_RXPMD_SRC_PORT_NUM, &src_port);
  bcm_pktio_pmd_field_get(0, &pkt, bcmPktioPmdTypeRx, BCMPKT_RXPMD_OUTER_VID, &vlan_id);
  bcm_pktio_pmd_field_get(0, &pkt, bcmPktioPmdTypeRx, BCMPKT_RXPMD_INNER_VID, &vlan_id_2);
  bcm_pktio_pmd_field_get(0, &pkt, bcmPktioPmdTypeRx, BCMPKT_RXPMD_PKT_LENGTH, &pkt_len);
  printf("src port %d \n", src_port);
  printf("vland id%d \n", vlan_id);
  printf("vland id - 2%d \n", vlan_id_2);
  printf("pkt len %d \n", pkt_len);
#if 0
  bcmpkt_rxpmd_field_get(dev_val, rxpmd, BCMPKT_RXPMD_SRC_PORT_NUM, &src_port);
  bcmpkt_rxpmd_field_get(dev_val, rxpmd, BCMPKT_RXPMD_OUTER_VID, &vlan_id);
  bcmpkt_rxpmd_field_get(dev_val, rxpmd, BCMPKT_RXPMD_INNER_VID, &vlan_id_2);
  bcmpkt_rxpmd_field_get(dev_val, rxpmd, BCMPKT_RXPMD_PKT_LENGTH, &pkt_len);
  variant = bcmlrd_variant_get(0);

  printf("src port %d \n", src_port);
  printf("vland id%d \n", vlan_id);
  printf("vland id - 2%d \n", vlan_id_2);
  printf("pkt len %d \n", pkt_len);
  printf("dev variant %d \n", variant);
#endif

  while ( i < 50) {
    printf("0x%x ", pk_32[i]);
    i++;
  }
  printf("\ninside bcmsdk_xgs_td4_metadata_decode end \n -------------------- \n");
  return packet;
}
