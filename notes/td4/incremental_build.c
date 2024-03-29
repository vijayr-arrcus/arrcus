sudo apt-get install elfutils

export HSDK_YAML_LIB_PATH=$PWD/pkgs/YAML_0_1_7

export YAML=${HSDK_YAML_LIB_PATH}/build/

export KERNDIR=/usr/src/linux-headers-4.19.84-arrcus

export LINUX_UAPI_SPLIT=1

export PHYMOD_EPDM_LIB_PATH=$PWD/pkgs/epdm_3_3_0

export PHYMOD_EPIL_LIB_PATH=$PWD/pkgs/MILN_5_1

export ELFUTILS_DIR=/usr/lib/x86_64-linux-gnu/

sudo apt-get install -y linux-headers-amd64

sudo apt-get install -y libelf-dev

sudo apt-get install -y bc

sudo apt-get install -y binutils

sudo apt-get install -y python

sudo apt-get install -y python-yaml

sudo apt-get install -y python-filelock

sudo apt-get install -y libyaml-dev

wget http://ftp.us.debian.org/debian/pool/main/e/elfutils/elfutils_0.168-1_amd64.deb
sudo dpkg -i *.deb
debian/rules prepare build-stash build-phymod build-yaml





make -f $PWD/systems/linux/user/x86-smp_generic_64-2_6/Makefile -j32 MAKE_LOCAL=$PWD/make/Make.local.xgs

cp $PWD/build/linux/user/common/libsdk.so libbcm_sdk_xgs.so

dwp -v -e $PWD/libbcm_sdk_xgs.so -o libbcm_sdk_xgs.so.dwp

scp libbcm_sdk_xgs* root@10.9.8.33:/usr/arcos/lib/



cd $PWD/build/linux-x86-smp_generic_64-2_6/x86-smp_generic_64-2_6



scp *.ko root@10.9.8.33:/lib/modules/4.19.84-arrcus/bcm/xgs/
