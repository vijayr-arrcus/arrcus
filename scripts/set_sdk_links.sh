#!/bin/bash

## Set link in this directories
APPDIR="/linux/user/gto-2_6 /sim/dpp /sim /vxworks/gto /linux/user/wrx-3_7 /linux/user/xlr-3_14 /linux/user/x86-smp_generic_64-2_6 /linux/user/slk /linux/user/slk_be"
APPPATH="$SDK/systems"

## Straight file name
LINKFILE="rc.soc dune.soc dfe.soc arad.soc arad_dram.soc combo28_dram.soc atmf.soc jer.soc dnx.soc dnxf.soc qax.soc pcp_sweep.txt bcm88750_board.soc bcm88950_board.soc bcm88790_board.soc bcm88790_pizza_board.soc bcm88790_dvt_board.soc bcm88375_board.soc bcm88675_board.soc bcm88470_board.soc bcm88270_board.soc bcm88680_board.soc config-dcmn.bcm config-jer2.bcm config-ramon.bcm combo28_dram_multi_88675.soc config-jer2pemla.bcm config-j2c.bcm config-j2cpemla.bcm"
LINKFILE="${LINKFILE} bcm88790_board_no_swap.soc"
LINKFILE="${LINKFILE} bcm88690_board.soc bcm88690_board.bcm bcm88690_revB_board.bcm bcm88690_dvt_board.soc"
LINKPATH="$SDK/rc"

## Set DB link in this directories
DBDIR="/regress/bcm"
DBPATH="$SDK"

for appdir in $APPDIR
do
    dir=$APPPATH$appdir
    echo "cd $dir"
    cd "$dir"

    for linkfile in $LINKFILE
    do
        file=$LINKPATH/$linkfile
        echo "ln -fs $file"
        ln -fs "$file"
    done

    ## Link to different file name

    echo "ln -fs $LINKPATH/config-sand.bcm config.bcm"
    ln -fs "$LINKPATH"/config-sand.bcm config.bcm
    echo "ln -fs $LINKPATH/reload-dune.soc reload.soc"
    ln -fs "$LINKPATH"/reload-dune.soc reload.soc

    if [[ "$appdir" = "/linux/user/wrx-3_7" || "$appdir" = "/linux/user/xlr-3_14" || "$appdir" = "/linux/user/slk_be" ]]; then
        kbp_driver_folder=""
        if [[ "$appdir" = "/linux/user/wrx-3_7" ]]; then
            kbp_driver_folder="unix-linux-64"
        elif [[ "$appdir" = "/linux/user/xlr-3_14" ]]; then
            kbp_driver_folder="xlr-3_14"
        elif [[ "$appdir" = "/linux/user/slk_be" ]]; then
            kbp_driver_folder="slk"
        fi
        echo "ln -fs $SDK/src/soc/kbp/alg_kbp/lib/kbp_driver/$kbp_driver_folder/kbp_driver.ko kbp_driver.ko"
        ln -fs "$SDK"/src/soc/kbp/alg_kbp/lib/kbp_driver/"$kbp_driver_folder"/kbp_driver.ko kbp_driver.ko
    fi

    if [[ -d "$SDK"/tools/sand/db ]]; then
        echo "ln -fs $SDK/tools/sand/db"
        ln -fs "$SDK"/tools/sand/db
    elif [[ -d "$SDK"/src/appl/diag/dcmn/db ]]; then
        echo ln -fs "$SDK"/src/appl/diag/dcmn/db
        ln -fs "$SDK"/src/appl/diag/dcmn/db
    fi
done

for dbdir in $DBDIR
do
    dir=$DBPATH$dbdir
    echo "cd $dir"
    cd "$dir"
    if [[ -d "$SDK"/tools/sand/db ]]; then
        echo ln -fs "$SDK"/tools/sand/db
        ln -fs "$SDK"/tools/sand/db
    elif [[ -d "$SDK"/src/appl/diag/dcmn/db ]]; then
        echo ln -fs "$SDK"/src/appl/diag/dcmn/db
        ln -fs "$SDK"/src/appl/diag/dcmn/db
    fi
done
