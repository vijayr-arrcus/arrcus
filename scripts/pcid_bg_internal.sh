#!/bin/bash

echoexec=/projects/NTSW_SW_USRS/common/intrn/echoexec.py
portallocrun=/projects/NTSW_SW_USRS/common/portallocrun
valgrind_path=/projects/NTSW_SW_USRS/common/valgrind-3.13.0/bin/valgrind
cmj2igdb="${SDK}"/systems/sim/cmodel/bin/linux/cmj2init.gdb

VGTestName=${VGTestName}
valgrind_log=""
if [[ "$VGTestName" != "" ]] ; then
    valgrind_log="--log-file=${SDK}/vgvar/${VGTestName}.vg "
fi

# In regression run folders are not git repository
if [[ "$REGRESSION_GIT_FOLDER" = "" ]] ;then
    REGRESSION_GIT_FOLDER="$SDK"
fi

valgrind_params="$valgrind_log \
             --trace-children=yes --num-callers=12 --error-limit=no \
             --gen-suppressions=all \
             --fullpath-after=${SDK}/ \
             --malloc-fill=33 --free-fill=66 --track-origins=yes \
             --leak-check=no \
             --read-var-info=yes"


# shellcheck disable=SC2034,SC2086,SC2046
dn=$(cd $(dirname ${0}); /bin/pwd)
if [[ -x ${dn}/bin/portallocrun ]]
then
    portallocrun=${dn}/bin/portallocrun
fi

if [[ -r ${dn}/cmj2init.gdb ]]
then
    cmj2igdb=${dn}/cmj2init.gdb
fi

declare -a process_ids

function now() {
    date "+%Y-%m-%d %H:%M:%S"
}

function tlog() {
    echo "$(now)" "${BASH_SOURCE[0]}:${BASH_LINENO[0]}" $@ >&2
}

function control_c {
    #echo "** Trapped CTRL-C"

    #echo "  killing pcid tasks ${process_ids[@]}"
    kill -KILL ${process_ids[@]} > /dev/null 2>&1

    kill 0
}

#export CMODEL_MEMORY_PORT=1222
#export CMODEL_PACKET_PORT=6815
#export CMODEL_SDK_INTERFACE_PORT=6816
#export CMODEL_EXTERNAL_EVENTS_PORT=6817


# Display the help information of pcid.sh
function pcid_help
{
    cat <<EOF
Usage:   pcid_bg.sh [suffix] device_id [count] [gdb] [SOC_BOOT_FLAGS=<flags>]
Example: pcid_bg.sh 88750_a0
Example: pcid_bg.sh 88750_a0 2
Example: pcid_bg.sh 88750_a0 gdb
Example: pcid_bg.sh 88750_a0 2 gdb


Runs pcid locally with device revision \'device_id\'.

Options:

suffix
  the suffix of the bcm.sim
  if the suffix is vendors it is used for bcm.sim and pcid.linux-64

Example: pcid_bg.sh vendors 88650_a0
Example: pcid_bg.sh pedantic-64 88650_a0
Example: pcid_bg.sh arad 88650_a0
Example: pcid_bg.sh arrakis 88650_a0

count
  number of chip instances to run
gdb
  run with debugger

C model Usage:

pcid_bg.sh cmodel device_id [gdb] [gdb_server] [SOC_BOOT_FLAGS=<flags>]
Example: pcid_bg.sh cmodel 88470_a0
Example: pcid_bg.sh cmodel 88680_a0 gdb
Example: pcid_bg.sh cmodel 88690_a0 gdb_server
Example: pcid_bg.sh cmodel 88690_a0 SOC_BOOT_FLAGS=0x21000
Example: pcid_bg.sh cmodel-dnx-only 88690_a0
Options:

gdb
  run with debugger
EOF
}

argv=($@)

if [[ $# -gt 3 && "$1" != "cmodel" ]] || [[ $# -gt 5 ]]; then
    pcid_help
    exit 1
fi

# Common arguments
SOC_BOOT_FLAGS_explicit=""
for ((i=1;i<=$#;i++))
do
    case "${!i}" in
    SOC_BOOT_FLAGS=*)
        SOC_BOOT_FLAGS_explicit=$(echo ${!i} | cut -d= -f2)
    esac
done

suffix=""
if [[ ("${1:0:1}" != "8") && ! ("$1" =~ ^cmodel) && ! ("$1" =~ help) ]] ; then
    suffix="-$1"
    shift
fi


# shellcheck disable=SC2046,SC2086
thisdir=$(cd $(dirname ${0}); /bin/pwd)
# shellcheck disable=SC2086
thisabs=${thisdir}/$(basename ${0})

devid=$1

count=1
debugger=""
server_debugger=""
cmodel_path=""
cmodel_exec=""
cmodel_cfg=""
server_only="no"
valgrind_run=""

# Print usage
if [[ $# -eq 1 ]]; then

    if [[ "$1" = "--help" || "$1" = "-help" || "$1" = "-h" ]]; then
        pcid_help
        exit 1
    fi

# Parse parameters for cmodel
elif [[ "$1" =~ cmodel* ]]; then
    suffix=-"$1"
    server_bg=""
    for ((i=3;i<=$#;i++))
    do
        case "${!i}" in
        gdb)
            echo "Debugging driver"
            debugger="gdb"
            ;;
        gdb_server)
            echo "Debugging server"
            server_debugger="gdb"
            ;;
        server_bg)
            echo "run server in background"
            server_bg="yes"
            ;;
        --record)
           set_record=1
           ;;
        server_only)
            echo "run only cmodel server"
            server_only="yes"
            ;;
        valgrind)
            echo "run with valgrind"
            valgrind_run="$valgrind_path $valgrind_params"
        esac
    done

    #Set default cmodel server mode
    mode="3"

    #Get device id and export it
    dev_type=${2:1:4}
    [[ ${PCID_BG_VERBOSE} -eq 0 ]] || echo "Exporting device id: $dev_type"
    export CMODEL_DEVID=$dev_type

    event_port_support="no"
    encryption_key_support1=""
    encryption_key_support2=""
    if [[ "$2" = "88470_a0" ]]; then
        cmodel_path="$COMMONDIR/cmodel/qax"
        cmodel_exec="QumranAX_cmodel"
        cmodel_cfg="qumran_ax.cfg"
    elif [[ "$2" = "88680_a0" ]]; then
        cmodel_path="$COMMONDIR/cmodel/jericho_plus"
        cmodel_exec="JerichoPlus_cmodel"
        cmodel_cfg="jericho_plus.cfg"
    elif [[ "$2" = "88690_a0" || "$2" = "88800_a0" ]]; then

        if [[ "$2" = "88690_a0" ]]; then
            device_name="Jericho2"
            common_device_name="jericho_2"
            cmodel_exec="Jericho2_cmodel"
            cmodel_cfg="jericho2.cfg"
        elif [[ "$2" = "88800_a0" ]]; then
            device_name="J2C"
            common_device_name="J2C"
            cmodel_exec="J2C_cmodel"
            cmodel_cfg="j2c.cfg"
        fi

        #Try to run server from locally defined path
        cmodel_path="$CMODEL_SERVER_PATH"
        if [ -z "$CMODEL_SERVER_PATH" ]; then
            #Run server from default location
            if [ -d "$SDK/systems/sim/cmodel/bin/linux/Jericho2" ]; then
                cmodel_path="$SDK/systems/sim/cmodel/bin/linux/$device_name"
                event_port_hash="3bd26db156dc45c5599acbfaaa1b7835149a67cd"

                pushd "$REGRESSION_GIT_FOLDER"/systems/sim/cmodel
                if [[ ! -d ../../../.git/modules/systems/sim/cmodel || `git merge-base --is-ancestor "$event_port_hash" HEAD` -eq 0 ]] ; then
                    event_port_support="yes"
                fi

                key_encryption_hash="d975aa1ffa7f33a7d6ab105b0b0c9a3a95be7b7a"
                if [[ ! -d ../../../.git/modules/systems/sim/cmodel ||  `git merge-base --is-ancestor "$key_encryption_hash" HEAD` -eq 0 ]] ; then
                    encryption_key_support1="-k"
                    encryption_key_support2="./device.key"
                fi
                popd
            else
                cmodel_path="$COMMONDIR/cmodel/$common_device_name"
            fi
        fi

        mode="sdk"
    else
        pcid_help
        exit 1
    fi

# Parse parameters for pcid
elif [[ $# -eq 2 ]]; then

    if [[ "$2" = "gdb" ]]; then
        debugger="gdb"
    else
        count=$2
    fi

elif [[ $# -eq 3 ]]; then

    count=$2
    if [[ "$3" = "gdb" ]]; then
        debugger="gdb"
    fi

fi

cd "$SDK"/systems/sim/dpp

export USE_SAND_SIM=0
export SOC_TARGET_COUNT=$count
if [[ "${SOC_BOOT_FLAGS_explicit:-x}" == "x" ]]
then
    if [[ "$1" =~ cmodel* ]]; then
	export SOC_BOOT_FLAGS=0x1020000
    else
	export SOC_BOOT_FLAGS=0x20000
    fi
else
    export SOC_BOOT_FLAGS="${SOC_BOOT_FLAGS_explicit}"
fi


tcp_ports=""
udp_ports=""
tcp_ar=()
udp_ar=()
ntcp=0
nudp=0
if [[ "$1" =~ cmodel* ]]
then
    tcp_ports="CMODEL_MEMORY_PORT,CMODEL_PACKET_PORT,CMODEL_SDK_INTERFACE_PORT"
    if [[ "$event_port_support" = "yes" ]] ; then
        udp_ports=",CMODEL_EXTERNAL_EVENTS_PORT"
    fi
    eports="${tcp_ports}${udp_ports}"
    tcp_ar=(${tcp_ports//,/ })
    udp_ar=(${udp_ports//,/ })
    ntcp=${#tcp_ar[@]}
    nudp=${#udp_ar[@]}
    nports=$((ntcp+nudp))
else
    tcp_ports=SOC_TARGET_PORT$(seq -s, 0 $((count-1)) | sed s/,/,SOC_TARGET_PORT/g)
    eports="$tcp_ports"
    nports=${count}
fi

if [[ "${_PORTS_ALLOCATED:0}" -eq 0 ]]
then
    export _PORTS_ALLOCATED=1
    # Recursive call to allocate ports
    if [[ ${PCID_BG_VERBOSE} -ne 0 ]]
    then
       ${echoexec} "${portallocrun}" -v 2 -pto 20 -envports "${eports}" \
                   -c "${thisabs}" ${argv[@]}
    else
        ${portallocrun} -pto 20 -envports "${eports}" -c "${thisabs}" ${argv[@]}
    fi
    rc=$?
    tlog exit ${rc}
    exit ${rc}
fi

# We get here via recursion call with ports already allocated.
# trap keyboard interrupt (control-c)
trap control_c SIGINT

# Generate grep pattern of ports numbers  (for example: "1001|1002|1003")
tmp_ports=$(eval echo \$"${tcp_ports//,/,$}")
tcp_portpat=${tmp_ports//,/|}
tmp_ports=$(eval echo \$"${udp_ports//,/,$}")
udp_portpat=${tmp_ports//,/|}

HOST_IP=$(hostname -I)
i=0
# shellcheck disable=SC2103
while [[ $i -lt $count ]]; do

    read SOC_TARGET_SERVER${i} <<< "127.0.0.1"
    export SOC_TARGET_SERVER${i}

    if [[ "$1" =~ cmodel* ]]; then

        echo "Load the C model's server"
        server_ex="$cmodel_path"/"$cmodel_exec"
        if [[ ! -x "$server_ex" ]]
        then
            tlog executable not found: "$server_ex"
            exit 1
        fi
        cd "$cmodel_path"

        sleep 2
        if [[ "$server_debugger" = "gdb" ]]; then
            echo "Set breakpoints and run with \"r -dsc $cmodel_cfg -p 1 -m $mode -ui sdk -pin sdk $encryption_key_support1 $encryption_key_support2 \" "
	    cat <<EOF
The "$cmodel_exec" will start under "$server_debugger" debugger.
It will stop after bind()-ing to socket ports.
Then you will be able to set breakpoints and continue.
EOF
            # shellcheck disable=SC1001
            \xterm -e "$server_debugger" -x "${cmj2igdb}" ./"$cmodel_exec" &

        else
            if [[ "$server_bg" = "yes" ]] ; then
                \tail -f /dev/null | ./$cmodel_exec -dsc $cmodel_cfg -p 1 -m $mode -ui sdk -pin sdk $encryption_key_support1  $encryption_key_support2 > /dev/null &
#| tee ~/scripts/test.out & 
#< /dev/null > /dev/null &
            else
                # Without UI

                if [[ $set_record -eq 1 ]]; then
                  record_dir="./.records"
                  if [[ ! -d "$record_dir" && ! -x "$record_dir" ]]; then
                      mkdir "./.records"
                  fi
                  sub_dir=$(date "+%Y-%m-%d_%H:%M:%S")
                  server_cmd="-c \"set record dir $record_dir name $sub_dir text\""
                  cmodel_cfg="jericho2_record.cfg"
                else
                  server_cmd=""
                fi
                # shellcheck disable=SC1001
                \xterm -e "./$cmodel_exec -dsc $cmodel_cfg -p 1 -m $mode -ui sdk -pin sdk $encryption_key_support1  $encryption_key_support2 -c \"$server_cmd\""&

                # With UI
                #  \xterm -e ./"$cmodel_exec" -dsc "$cmodel_cfg" -p 1 -m 3 -ui cli -pin sdk &
                sleep 4
            fi
        fi

        cd -
    else
	p1="" # tp suppress SC2154
	# shellcheck disable=SC1083
        eval p1=\${SOC_TARGET_PORT${i}}

        pcid_suffix=""
        if [[ "$suffix" = "-vendors"  || "$suffix" = "-dnxf" ]] ; then
            pcid_suffix="$suffix"
        fi

        server_ex="$SDK"/systems/sim/pcid/pcid.linux-64"$pcid_suffix"
        if [[ ! -x "$server_ex" ]]
        then
            tlog executable not found: "$server_ex"
            exit 1
        fi
echo "RUNNING $server_ex -p $p1 $devid" 
        "$server_ex" -p "$p1" "$devid" &
    fi

    pid=$!
    #save the process id
    process_ids=("${process_ids[@]}" $pid)
    # wait 2s to make sure the pcid listen to the port before checking next available port
    sleep 2

    ((i++))
done

# Wait for server(s) to listen on ports
np_listened=0
slept=0

wait_timeout=$((2*count + 10))
[[ ${PCID_BG_VERBOSE} -eq 0 ]] || tlog wait_timeout=${wait_timeout}
while [[ "${np_listened}" -ne ${nports} && "${slept}" -lt ${wait_timeout} ]]
do
    tcp_listened=$(netstat -tulpn 2>/dev/null | egrep "^tcp.*:(${tcp_portpat}) " | wc -l)
    udp_listened=0
    if [[ "$nudp" -gt 0 ]] ; then
        udp_listened=$(netstat -tulpn 2>/dev/null | egrep "^udp.*:(${udp_portpat}) " | wc -l)
    fi
echo "Ports: TCP: ${tcp_portpat} UDP: ${udp_portpat}"
    np_listened=$((tcp_listened+udp_listened))
    kill -0 "${pid}"
    if [[ $? -ne 0 ]]
    then
        tlog server process "${pid}" does not exist
        wait_timeout=0 # exit loop
    fi
    sleep 1
    ((slept=slept+1))
done
if [[ ${PCID_BG_VERBOSE} -ne 0 ]]
then
    tlog np_listened="${np_listened}" slept="${slept}"
fi
if [[ "${np_listened}" -ne ${nports} ]]
then
    tlog "Timeout for waiting for ports reached."
    tlog Check the executable expected to listen to them: "$server_ex"
    ls -lG "$server_ex"
    kill -KILL ${process_ids[@]} > /dev/null 2>&1
    exit 1
fi

if [[ "$server_only" = "yes" ]] ; then
    printf "%-15s   %-11s   %-12s   %-19s %-20s\n" "IP" "MEMORY PORT" "PACKET PORT" "SDK INTERFACE PORT" "EXTERNAL EVENTS PORT"
    printf "%-15s   %-11s   %-12s   %-19s %-20s\n" "$HOST_IP" "$CMODEL_MEMORY_PORT" "$CMODEL_PACKET_PORT" "$CMODEL_SDK_INTERFACE_PORT" \
    "$CMODEL_EXTERNAL_EVENTS_PORT"
    wait "${process_ids[@]}"
    exit 1
fi

if [[ ! -x ./bcm.sim"$suffix" ]]
then
    tlog executable not found: "$(/bin/pwd)"/bcm.sim"$suffix"
    kill -KILL ${process_ids[@]} > /dev/null 2>&1
    exit 1
fi

$valgrind_run $debugger  ./bcm.sim"$suffix"

kill -KILL ${process_ids[@]} > /dev/null 2>&1
