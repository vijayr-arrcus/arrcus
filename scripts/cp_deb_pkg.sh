#!/bin/bash
IP_ADDR=$1
tar zcvf /tmp/all_debs.tgz *arcos*.deb arrcus-base*.deb arrcus-cp-api*.deb arrcus-lc-api*.deb arrcus-pltf-api*.deb arrcus-wh*.deb arrcus-cp*.deb arrcus-lc-common*.deb arrcus-lc-cp*.deb arrcus-lc*hw*.deb arrcus-ppm-api*.deb arrcus-init-scripts*.deb arrcus-pltf-agent*.deb
scp /tmp/all_debs.tgz arrcus-arcos*.deb root@${IP_ADDR}:/
rm /tmp/all_debs.tgz
scp ~/scripts/deploy_debs.sh root@${IP_ADDR}:/
