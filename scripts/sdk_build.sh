#!/bin/bash
sudo apt-get install -y elfutils
sudo apt-get install -y libdata-compare-perl
sudo apt-get install -y libmoose-perl
sudo apt-get install -y libnamespace-autoclean-perl
sudo apt-get install -y libconfig-yaml-perl
sudo apt-get install -y linux-headers-amd64
sudo apt-get install -y libelf-dev
sudo apt-get install -y bc
sudo apt-get install -y binutils
sudo apt-get install -y python
sudo apt-get install -y python-yaml
sudo apt-get install -y python-filelock
sudo apt-get install -y libyaml-dev

export KERNDIR=/usr/src/linux-headers-4.19.84-arrcus
export LINUX_UAPI_SPLIT=1
export PHYMOD_EPDM_LIB_PATH=$PWD/pkgs/epdm_4_3_2
export PHYMOD_BRNC_LIB_PATH=$PWD/pkgs/BRNC_2_10
export PHYMOD_EPIL_LIB_PATH=$PWD/pkgs/MILN_5_1
export PHYMOD_MILB_LIB_PATH=$PWD/pkgs/MILB_5_2
export PHYMOD_BARC_LIB_PATH=$PWD/pkgs/BARC_3_4
export KBP_KERNEL_DRIVER_PATH=$PWD/pkgs/sdk/src/soc/kbp/alg_kbp/xpt/pcie/device_driver
export HSDK_YAML_LIB_PATH=$PWD/pkgs/YAML_0_1_7
export YAML=${HSDK_YAML_LIB_PATH}/build/
export ELFUTILS_DIR=/usr/include/elfutils/
export SDK_OVERRIDE_LOCK=0
export SDK_LOCK=0

echo "1. First time build."
echo "2. First time build in a patch queue."
echo "3. Incremental build (j=20)."
echo "4. Debug build (j=1)."

echo -n "Enter option > "
read option

if [ $option == 1 ]
then
  echo "First time build."
  debian/rules prepare prepare-pkgs build-stash build-phymod build-yaml build-xgs
elif [ $option == 2 ]
then
  echo "First time build in a patch queue."
  debian/rules prepare-pkgs build-stash build-phymod build-yaml build-xgs
elif [ $option == 3 ]
then
  echo "Incremental build (j=20)"
  make -j20 -f $PWD/systems/linux/user/x86-smp_generic_64-2_6/Makefile MAKE_LOCAL=$PWD/make/Make.local.xgs
elif [ $option == 4 ]
then
  echo "Debug build (j=1)"
  make -j1 -f $PWD/systems/linux/user/x86-smp_generic_64-2_6/Makefile MAKE_LOCAL=$PWD/make/Make.local.xgs
else
  echo "invalid input"
fi
